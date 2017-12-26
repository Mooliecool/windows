//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Expressions;
using System.Activities.Statements;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Management.Automation;
using Microsoft.Samples.Activities.PowerShell;

namespace Microsoft.Samples.PowerShell.CodedClient
{

    class PowerShellSample
    {
        static void Main(string[] args)
        {
            Console.WriteLine();
            Console.WriteLine("Running workflow...");
            WorkflowInvoker.Invoke(CreateWf());

            Console.WriteLine("Press <return> to continue...");
            Console.Read();
        }

        private static Activity CreateWf()
        {
            // variables.
            var pipeline = new DelegateInArgument<PSObject>() { Name = "Pipeline" };
            var processName = new DelegateOutArgument<string>() { Name = "Process Name" };
            var processes1 = new Variable<Collection<Process>> { Name = "Processes 1" };
            var processes2 = new Variable<Collection<Process>> { Name = "Processes 2" };
            var processes3 = new Variable<Collection<Process>> { Name = "Processes 3" };
            var processNames = new Variable<Collection<string>> { Name = "Process Names" };
            var errors = new Variable<Collection<ErrorRecord>> { Name = "Errors" };
            var error = new DelegateInArgument<ErrorRecord> { Name = "Error" };

            Sequence body = new Sequence()
            {
                Variables = { processes1, processes2, processes3, processNames, errors },
                Activities = 
                {
                    // Simple PowerShell invocation.
                    new WriteLine()
                    {
                        Text = "Simple PowerShell invocation. Launching notepad."
                    },

                    new InvokePowerShell()
                    {
                        CommandText = "notepad"
                    },
                    new WriteLine(),

                    // Using PowerShell<T> to capture output.
                    new WriteLine()
                    {
                        Text = "Getting process and then pass the output to a Collection."
                    },

                    new InvokePowerShell<Process>()
                    {
                        CommandText = "Get-Process",
                        Output = processes1,
                    },

                    CreatePrintProcessActivity(processes1),
          
                    // Do some post-processing after invocation.
                    new WriteLine()
                    {
                        Text = "Getting the names of the processes."
                    },

                    new InvokePowerShell<string>()
                    {
                        CommandText = "Get-Process",
                        Output = processNames,
                        InitializationAction = new ActivityFunc<PSObject,string>
                        {
                            Argument = pipeline,
                            Result = processName,
                            Handler = new Assign<string>
                            {
                                To = processName,
                                Value = new LambdaValue<string>(ctx => ((Process) pipeline.Get(ctx).BaseObject).ToString())
                            }
                        },
                    },

                    new WriteLine()
                    {
                        Text = new InArgument<string>(ctx => "The first process returned is: " + processNames.Get(ctx)[0]),
                    },
                    new WriteLine(),

                    // Passing data using an input pipeline.
                    new WriteLine()
                    {
                        Text = "Passing data using an input pipeline and then extracting unique process names."
                    },

                    new InvokePowerShell<Process>()
                    {
                        CommandText = "Get-Unique",
                        Input = processes1,
                        Output = processes2,
                    },

                    CreatePrintProcessActivity(processes2),

                    // Passing in a parameter to the command.
                    new WriteLine()
                    {
                        Text = "Reverse sorting."
                    },

                    new InvokePowerShell<Process>()
                    {
                        CommandText = "Sort-Object",
                        Input = processes2,
                        Output = processes3,
                        Parameters =
                        {
                            { "descending", new InArgument<Boolean>(true) }
                        }
                    },

                    CreatePrintProcessActivity(processes3),

                    // Run a command that results in errors.
                    new WriteLine()
                    {
                        Text = "Returning errors."
                    },

                    new InvokePowerShell<string>()
                    {
                        CommandText = "Write-Error",
                        Errors = errors,
                        Parameters =
                        {
                            { "message", new InArgument<string>("a short error message") }
                        }
                    },

                    new ForEach<ErrorRecord>()
                    {
                        Values = errors,
                        Body = new ActivityAction<ErrorRecord>
                        {
                            Argument = error,
                            Handler = new WriteLine()
                            {
                                Text = new InArgument<string>(ctx => "Error returned: " + error.Get(ctx).ToString()),
                            },
                        }
                    }
                }
            };

            return body;
        }

        private static Activity CreatePrintProcessActivity(Variable<Collection<Process>> processes)
        {
            var proc = new DelegateInArgument<Process>("process");

            return new Sequence
            {
                Activities =
                {
                    // Loop over processes and print them out.
                    new ForEach<Process>
                    {
                        Values = processes,
                        Body = new ActivityAction<Process>
                        {
                            Argument = proc,
                            Handler = new WriteLine
                            {
                                Text = new InArgument<string>(ctx => proc.Get(ctx).ToString()),
                            }
                        }
                    },

                    // Print out a new line.
                    new WriteLine(), 
                }
            };
        }
    }
}
