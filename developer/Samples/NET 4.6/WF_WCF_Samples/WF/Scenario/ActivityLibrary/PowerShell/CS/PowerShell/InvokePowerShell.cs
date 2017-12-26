//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Expressions;
using System.Activities.Presentation.PropertyEditing;
using System.Activities.Statements;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Management.Automation;
using System.Windows.Markup;

namespace Microsoft.Samples.Activities.PowerShell
{

    class IPSHelper
    {
        public static void CacheMetadataHelper(
            ActivityMetadata metadata, InArgument<IEnumerable> input, OutArgument<Collection<ErrorRecord>> errors, string commandText, string typeName,
            string displayName, IDictionary<string, Argument> variables, IDictionary<string, InArgument> parameters,
            IDictionary<string, Argument> childVariables, IDictionary<string, InArgument> childParameters)
        {
            childVariables.Clear();
            childParameters.Clear();
            RuntimeArgument inputArgument = new RuntimeArgument("Input", typeof(IEnumerable), ArgumentDirection.In);
            metadata.Bind(input, inputArgument);
            metadata.AddArgument(inputArgument);

            RuntimeArgument errorArgument = new RuntimeArgument("Errors", typeof(Collection<ErrorRecord>), ArgumentDirection.Out);
            metadata.Bind(errors, errorArgument);
            metadata.AddArgument(errorArgument);

            if (commandText == null || string.IsNullOrEmpty(commandText.Trim()))
            {
                metadata.AddValidationError(string.Format(ErrorMessages.PowerShellRequiresCommand, displayName));
            }

            foreach (KeyValuePair<string, Argument> variable in variables)
            {
                string name = variable.Key;
                Argument argument = variable.Value;
                RuntimeArgument ra = new RuntimeArgument(name, argument.ArgumentType, argument.Direction, true);
                metadata.Bind(argument, ra);
                metadata.AddArgument(ra);

                Argument childArgument = Argument.Create(argument.ArgumentType, argument.Direction);
                childVariables.Add(name, Argument.CreateReference(argument, name));
            }

            foreach (KeyValuePair<string, InArgument> parameter in parameters)
            {
                string name = parameter.Key;
                InArgument argument = parameter.Value;
                RuntimeArgument ra;
                if (argument.ArgumentType == typeof(bool))
                {
                    ra = new RuntimeArgument(name, argument.ArgumentType, argument.Direction, false);
                }
                else
                {
                    ra = new RuntimeArgument(name, argument.ArgumentType, argument.Direction, true);
                }
                metadata.Bind(argument, ra);
                metadata.AddArgument(ra);

                Argument childArgument = Argument.Create(argument.ArgumentType, argument.Direction);
                childParameters.Add(name, Argument.CreateReference(argument, name) as InArgument);
            }
        }
    }


    [Designer(typeof(InvokePowerShellDesigner))]
    [ContentProperty("Parameters")]
    public sealed class InvokePowerShell : Activity
    {
        Dictionary<string, InArgument> parameters;
        Dictionary<string, Argument> powerShellVariables;
        Dictionary<string, InArgument> childParameters;
        Dictionary<string, Argument> childPowerShellVariables;

        [Editor(typeof(ArgumentDictionaryEditor), typeof(PropertyValueEditor))]
        [Browsable(true)]
        public IDictionary<string, InArgument> Parameters
        {
            get
            {
                if (this.parameters == null)
                {
                    this.parameters = new Dictionary<string, InArgument>();
                }
                return this.parameters;
            }
        }

        [Editor(typeof(ArgumentDictionaryEditor), typeof(PropertyValueEditor))]
        [Browsable(true)]
        public IDictionary<string, Argument> PowerShellVariables
        {
            get
            {
                if (this.powerShellVariables == null)
                {
                    this.powerShellVariables = new Dictionary<string, Argument>();
                }
                return this.powerShellVariables;
            }
        }

        [DefaultValue(null)]
        public string CommandText { get; set; }

        [DefaultValue(null)]
        public InArgument<IEnumerable> Input { get; set; }

        [CLSCompliant(false)]
        [DefaultValue(null)]
        public OutArgument<Collection<ErrorRecord>> Errors { get; set; }

        [DefaultValue(false)]
        public bool IsScript { get; set; }

        [CLSCompliant(false)]
        [DefaultValue(null)]
        public OutArgument<Collection<PSObject>> Output { get; set; }

        public InvokePowerShell()
            : base()
        {
            this.childParameters = new Dictionary<string, InArgument>();
            this.childPowerShellVariables = new Dictionary<string, Argument>();
            this.Implementation = () => new ExecutePowerShell
            {
                CommandText = this.CommandText,
                Parameters = this.childParameters,
                PowerShellVariables = this.childPowerShellVariables,
                PipelineOutput = new OutArgument<Collection<PSObject>>(ctx => this.Output.Get(ctx)),
                Errors = new ArgumentReference<Collection<ErrorRecord>> { ArgumentName = "Errors" },
                IsScript = this.IsScript,
                Input = new ArgumentValue<IEnumerable> { ArgumentName = "Input" },
            };
        }

        protected override void CacheMetadata(ActivityMetadata metadata)
        {
            IPSHelper.CacheMetadataHelper(metadata, this.Input, this.Errors, this.CommandText, GetType().Name,
                this.DisplayName, this.PowerShellVariables, this.Parameters, this.childPowerShellVariables, this.childParameters);

            RuntimeArgument outputArgument = new RuntimeArgument("Output", typeof(Collection<PSObject>), ArgumentDirection.Out, false);
            metadata.Bind(this.Output, outputArgument);
            metadata.AddArgument(outputArgument);
        }
    }

    [Designer(typeof(GenericInvokePowerShellDesigner))]
    [ContentProperty("Parameters")]
    public sealed class InvokePowerShell<TResult> : Activity
    {
        Dictionary<string, InArgument> parameters;
        Dictionary<string, Argument> powerShellVariables;
        Dictionary<string, InArgument> childParameters;
        Dictionary<string, Argument> childPowerShellVariables;

        [Editor(typeof(ArgumentDictionaryEditor), typeof(PropertyValueEditor))]
        [Browsable(true)]
        public IDictionary<string, InArgument> Parameters
        {
            get
            {
                if (this.parameters == null)
                {
                    this.parameters = new Dictionary<string, InArgument>();
                }
                return this.parameters;
            }
        }

        [Editor(typeof(ArgumentDictionaryEditor), typeof(PropertyValueEditor))]
        [Browsable(true)]
        public IDictionary<string, Argument> PowerShellVariables
        {
            get
            {
                if (this.powerShellVariables == null)
                {
                    this.powerShellVariables = new Dictionary<string, Argument>();
                }
                return this.powerShellVariables;
            }
        }

        [DefaultValue(null)]
        public string CommandText { get; set; }

        [DefaultValue(null)]
        public InArgument<IEnumerable> Input { get; set; }

        [CLSCompliant(false)]
        [DefaultValue(null)]
        public OutArgument<Collection<ErrorRecord>> Errors { get; set; }

        [DefaultValue(false)]
        public bool IsScript { get; set; }

        [CLSCompliant(false)]
        [DefaultValue(null)]
        public OutArgument<Collection<TResult>> Output { get; set; }

        [CLSCompliant(false)]
        [DefaultValue(null)]
        [Browsable(false)]
        public ActivityFunc<PSObject, TResult> InitializationAction { get; set; }

        public InvokePowerShell()
            : base()
        {
            DelegateOutArgument<TResult> result = new DelegateOutArgument<TResult>() { Name = "result" };
            DelegateInArgument<PSObject> psObject = new DelegateInArgument<PSObject>() { Name = "psObject" };

            this.childParameters = new Dictionary<string, InArgument>();
            this.childPowerShellVariables = new Dictionary<string, Argument>();
            this.Implementation = new Func<Activity>(this.CreateBody);
            this.InitializationAction = new ActivityFunc<PSObject, TResult>
            {
                Argument = psObject,
                Result = result,
            };
        }

        protected override void CacheMetadata(ActivityMetadata metadata)
        {
            IPSHelper.CacheMetadataHelper(metadata, this.Input, this.Errors, this.CommandText, GetType().Name,
                this.DisplayName, this.PowerShellVariables, this.Parameters, this.childPowerShellVariables, this.childParameters);

            RuntimeArgument outputArgument = new RuntimeArgument("Output", typeof(Collection<TResult>), ArgumentDirection.Out, false);
            metadata.Bind(this.Output, outputArgument);
            metadata.AddArgument(outputArgument);
        }

        Activity CreateBody()
        {
            Variable<Collection<PSObject>> psObjects = new Variable<Collection<PSObject>>();
            Variable<TResult> outputObject = new Variable<TResult>();
            DelegateInArgument<PSObject> psObject = new DelegateInArgument<PSObject>();
            Variable<Collection<TResult>> outputObjects = new Variable<Collection<TResult>>() { Default = new New<Collection<TResult>>(), };

            return new NoPersistZone
            {
                Body = new Sequence
                {
                    Variables = { psObjects, outputObjects, outputObject },
                    Activities =
                    {
                        new ExecutePowerShell
                        {
                            CommandText = this.CommandText,
                            Parameters = this.childParameters,
                            PowerShellVariables = this.childPowerShellVariables,
                            PipelineOutput = psObjects,
                            Errors = new ArgumentReference<Collection<ErrorRecord>> { ArgumentName = "Errors" },
                            IsScript = this.IsScript,
                            Input = new ArgumentValue<IEnumerable> { ArgumentName = "Input" },
                        },

                        new ForEach<PSObject>
                        {
                            Values = psObjects,
                            Body = new ActivityAction<PSObject>
                            {
                                Argument = psObject,
                                Handler = new Sequence
                                {
                                    Activities =
                                    {
                                        new If
                                        {
                                            Condition = new AndAlso
                                            {
                                                Left = new NotEqual<ActivityFunc<PSObject, TResult>, ActivityFunc<PSObject, TResult>, bool>
                                                {
                                                    Left = new LambdaValue<ActivityFunc<PSObject, TResult>>( ctx => this.InitializationAction ),
                                                    Right = new LambdaValue<ActivityFunc<PSObject, TResult>>( ctx => null ),
                                                },
                                                Right = new NotEqual<Activity, Activity, bool>
                                                {
                                                    Left = new PropertyValue<ActivityFunc<PSObject, TResult>, Activity>
                                                    {
                                                        Operand = new LambdaValue<ActivityFunc<PSObject, TResult>>( ctx => this.InitializationAction ),
                                                        PropertyName = "Handler",
                                                    },
                                                    Right = new LambdaValue<Activity>( ctx => null ),
                                                },
                                            },
                                            Then = new InvokeFunc<PSObject, TResult>
                                            {
                                                Argument = psObject,
                                                Result = outputObject,
                                                Func = this.InitializationAction
                                            },
                                            Else = new Assign<TResult>
                                            {
                                                To = outputObject,
                                                Value = new InArgument<TResult>(ctx => (TResult) psObject.Get(ctx).BaseObject),
                                            }
                                        },
                                        new AddToCollection<TResult>
                                        {
                                            Collection = outputObjects,
                                            Item = outputObject
                                        },
                                    }
                                }
                            }
                        },
                        new Assign<Collection<TResult>>
                        {
                            To = new OutArgument<Collection<TResult>>(ctx => this.Output.Get(ctx)),
                            Value = outputObjects,
                        }
                    },
                }
            };
        }
    }
}
