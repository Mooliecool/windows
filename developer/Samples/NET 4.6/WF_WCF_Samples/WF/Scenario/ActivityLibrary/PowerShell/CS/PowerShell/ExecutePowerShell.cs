//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Management.Automation;
using System.Management.Automation.Runspaces;
using System.Threading;
using Microsoft.Win32;

namespace Microsoft.Samples.Activities.PowerShell
{

    sealed class ExecutePowerShell : AsyncCodeActivity
    {
        const string PowerShellGlobalVariableNamePrefix = "Global.";

        static object lockObject = new object();
        static Nullable<bool> isPowerShellInstalled;

        static bool IsPowerShellInstalled
        {
            get
            {
                if (!isPowerShellInstalled.HasValue)
                {
                    lock (lockObject)
                    {
                        if (!isPowerShellInstalled.HasValue)
                        {
                            bool isInstalled = false;
                            using (RegistryKey registryKey = Registry.LocalMachine.OpenSubKey("Software\\Microsoft\\PowerShell\\1"))
                            {
                                if (registryKey != null)
                                {
                                    object value = registryKey.GetValue("Install");
                                    isInstalled = (value is Int32) && ((Int32)value == 1);
                                }
                            }
                            isPowerShellInstalled = isInstalled;
                        }
                    }
                }
                return isPowerShellInstalled.Value;
            }
        }

        public ExecutePowerShell()
            : base()
        {
        }

        public string CommandText
        {
            get;
            set;
        }

        public IDictionary<string, InArgument> Parameters
        {
            get;
            set;
        }

        public InArgument<IEnumerable> Input
        {
            get;
            set;
        }

        public OutArgument<Collection<PSObject>> PipelineOutput
        {
            get;
            set;
        }

        public OutArgument<Collection<ErrorRecord>> Errors
        {
            get;
            set;
        }

        public bool IsScript
        {
            get;
            set;
        }

        public IDictionary<string, Argument> PowerShellVariables
        {
            get;
            set;
        }

        void AddDictionaryOfArguments<T>(CodeActivityMetadata metadata, IList<RuntimeArgument> arguments,
            IDictionary<string, T> dictionary, string prefix, bool isParameter) where T : Argument
        {
            foreach (KeyValuePair<string, T> entry in dictionary)
            {
                Argument binding = entry.Value;
                if (binding.Expression != null)
                {
                    RuntimeArgument argument;
                    if (isParameter && binding.ArgumentType == typeof(bool))
                    {
                        //InArgument<bool> with null expression specifies a switch parameter. 
                        //Otherwise all the InArguments in Parameters collection cannot have null expression.
                        argument = new RuntimeArgument(prefix + entry.Key, binding.ArgumentType, binding.Direction, false);
                    }
                    else
                    {
                        argument = new RuntimeArgument(prefix + entry.Key, binding.ArgumentType, binding.Direction, true);
                    }
                    metadata.Bind(binding, argument);

                    arguments.Add(argument);
                }
            }
        }

        // Called before workflow execution to inform the runtime about arguments.
        protected override void CacheMetadata(CodeActivityMetadata metadata)
        {
            Collection<RuntimeArgument> arguments = new Collection<RuntimeArgument>();

            // Overridding base.OnGetArguments to avoid reflection costs 
            RuntimeArgument inputArgument = new RuntimeArgument("Input", typeof(IEnumerable), ArgumentDirection.In);
            metadata.Bind(this.Input, inputArgument);
            arguments.Add(inputArgument);

            RuntimeArgument pipelineOutputArgument = new RuntimeArgument("PipelineOutput", typeof(Collection<PSObject>), ArgumentDirection.Out);
            metadata.Bind(this.PipelineOutput, pipelineOutputArgument);
            arguments.Add(pipelineOutputArgument);

            RuntimeArgument errorsArgument = new RuntimeArgument("Errors", typeof(Collection<ErrorRecord>), ArgumentDirection.Out);
            metadata.Bind(this.Errors, errorsArgument);
            arguments.Add(errorsArgument);

            if (this.Parameters.Count != 0)
            {
                AddDictionaryOfArguments<InArgument>(metadata, arguments, this.Parameters, string.Empty, true);
            }

            if (this.PowerShellVariables.Count != 0)
            {
                //Prefix variable name so the name is unique from runtime's point of view,
                //this is to handle the case where user might use the same name for variables and parameters                
                AddDictionaryOfArguments<Argument>(metadata, arguments, this.PowerShellVariables, PowerShellGlobalVariableNamePrefix, false);
            }

            metadata.SetArgumentsCollection(arguments);
        }

        void DisposePipeline(Pipeline pipelineInstance)
        {
            if (pipelineInstance != null)
            {
                pipelineInstance.Runspace.Close();
                pipelineInstance.Dispose();
            }
            pipelineInstance = null;
        }

        // Called by the runtime to cancel the execution of this asynchronous activity.
        protected override void Cancel(AsyncCodeActivityContext context)
        {
            Pipeline pipeline = context.UserState as Pipeline;
            if (pipeline != null)
            {
                pipeline.Stop();
                DisposePipeline(pipeline);
            }
            base.Cancel(context);
        }

        // Called by the runtime to begin execution of this asynchronous activity.
        protected override IAsyncResult BeginExecute(AsyncCodeActivityContext context, AsyncCallback callback, object state)
        {
            if (!IsPowerShellInstalled)
            {
                throw new InvalidOperationException(ErrorMessages.PowerShellNotInstalled);
            }

            Runspace runspace = null;
            Pipeline pipeline = null;

            try
            {
                runspace = RunspaceFactory.CreateRunspace();
                runspace.Open();
                pipeline = runspace.CreatePipeline();
                Command cmd = new Command(this.CommandText, this.IsScript);
                if (this.Parameters != null)
                {
                    foreach (KeyValuePair<string, InArgument> parameter in this.Parameters)
                    {
                        if (parameter.Value.Expression != null)
                        {
                            cmd.Parameters.Add(parameter.Key, parameter.Value.Get(context));
                        }
                        else
                        {
                            cmd.Parameters.Add(parameter.Key, true);
                        }
                    }
                }
                if (this.PowerShellVariables != null)
                {
                    foreach (KeyValuePair<string, Argument> powerShellVariable in this.PowerShellVariables)
                    {
                        if ((powerShellVariable.Value.Direction == ArgumentDirection.In) || (powerShellVariable.Value.Direction == ArgumentDirection.InOut))
                        {
                            runspace.SessionStateProxy.SetVariable(powerShellVariable.Key, powerShellVariable.Value.Get(context));
                        }
                    }
                }
                pipeline.Commands.Add(cmd);
                IEnumerable pipelineInput = this.Input.Get(context);
                if (pipelineInput != null)
                {
                    foreach (object inputItem in pipelineInput)
                    {
                        pipeline.Input.Write(inputItem);
                    }
                }
                pipeline.Input.Close();

                context.UserState = pipeline;
                return new PipelineInvokerAsyncResult(pipeline, callback, state);
            }
            catch
            {
                if (runspace != null)
                {
                    runspace.Dispose();
                }

                if (pipeline != null)
                {
                    pipeline.Dispose();
                }
                
                throw;
            }
        }

        // Called by the runtime after execution of this asynchronous activity.
        protected override void EndExecute(AsyncCodeActivityContext context, IAsyncResult result)
        {
            PipelineInvokerAsyncResult asyncResult = result as PipelineInvokerAsyncResult;
            Pipeline pipeline = context.UserState as Pipeline;
            try
            {
                if (asyncResult.Exception != null)
                {
                    throw new PowerShellExecutionException(asyncResult.Exception, asyncResult.ErrorRecords);
                }
                this.PipelineOutput.Set(context, asyncResult.PipelineOutput);
                this.Errors.Set(context, asyncResult.ErrorRecords);

                foreach (KeyValuePair<string, Argument> entry in this.PowerShellVariables)
                {
                    if ((entry.Value.Direction == ArgumentDirection.Out) || (entry.Value.Direction == ArgumentDirection.InOut))
                    {
                        object value = pipeline.Runspace.SessionStateProxy.GetVariable(entry.Key);
                        entry.Value.Set(context, value);
                    }
                }
            }
            finally
            {
                DisposePipeline(pipeline);
            }
        }
    }

    // An instance of this object is returned to the runtime upon beginning
    // execution.  This also keeps a pointer to a callback function, which
    // it calls when it completes.
    class PipelineInvokerAsyncResult : IAsyncResult
    {
        AsyncCallback callback;
        object asyncState;
        EventWaitHandle asyncWaitHandle;

        Collection<ErrorRecord> errorRecords;
        public Collection<ErrorRecord> ErrorRecords
        {
            get
            {
                if (this.errorRecords == null)
                {
                    this.errorRecords = new Collection<ErrorRecord>();
                }

                return this.errorRecords;
            }
        }

        public Exception Exception
        {
            get;
            set;
        }

        public Collection<PSObject> PipelineOutput
        {
            get;
            set;
        }

        public object AsyncState
        {
            get { return this.asyncState; }
        }

        public WaitHandle AsyncWaitHandle
        {
            get { return this.asyncWaitHandle; }
        }

        public bool CompletedSynchronously
        {
            get { return false; }
        }

        public bool IsCompleted
        {
            get { return true; }
        }

        public PipelineInvokerAsyncResult(Pipeline pipeline, AsyncCallback callback, object state)
        {
            this.asyncState = state;
            this.callback = callback;
            this.asyncWaitHandle = new EventWaitHandle(false, EventResetMode.AutoReset);
            pipeline.StateChanged += new EventHandler<PipelineStateEventArgs>(OnStateChanged);
            pipeline.InvokeAsync();
        }

        void Complete()
        {
            this.asyncWaitHandle.Set();
            if (this.callback != null)
            {
                this.callback(this);
            }
        }

        // Called by the underlying PowerShell pipeline object on state changes.
        void OnStateChanged(object sender, PipelineStateEventArgs args)
        {
            try
            {
                PipelineState state = args.PipelineStateInfo.State;
                Pipeline pipeline = sender as Pipeline;

                if (state == PipelineState.Completed)
                {
                    this.PipelineOutput = pipeline.Output.ReadToEnd();
                    ReadErrorRecords(pipeline);
                    Complete();
                }
                else if (state == PipelineState.Failed)
                {
                    this.Exception = args.PipelineStateInfo.Reason;
                    ReadErrorRecords(pipeline);
                    Complete();
                }
                else if (state == PipelineState.Stopped)
                {
                    Complete(); ;
                }
                else
                {
                    return; // nothing to do
                }
            }
            catch (Exception e)
            {
                this.Exception = e;
                Complete();
            }
        }

        void ReadErrorRecords(Pipeline pipeline)
        {
            Collection<object> errorsRecords = pipeline.Error.ReadToEnd();
            if (errorsRecords.Count != 0)
            {
                foreach (PSObject item in errorsRecords)
                {
                    ErrorRecord errorRecord = item.BaseObject as ErrorRecord;
                    this.ErrorRecords.Add(errorRecord);
                }
            }
        }
    }
}
