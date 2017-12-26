//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Reflection;
using System.ServiceModel.Activities;
using Microsoft.VisualBasic.Activities;

namespace Microsoft.Samples.OperationScopeActivity
{
    [Designer(typeof(OperationScopeDesigner))]
    public sealed class OperationScope : NativeActivity
    {
        Sequence impl;
        Receive receive;
        SendReply reply;
        Activity bodyClone;
        Collection<Assign> assigns;
        Collection<Variable> variables;

        public Activity Body { get; set; }

        public bool CanCreateInstance { get; set; }
        
        public OperationScope() : base()
        {
            assigns = new Collection<Assign>();
            variables = new Collection<Variable>();
            receive = new Receive()
            {                
                Content = new ReceiveParametersContent()
            };
            reply = new SendReply()
            {
                Request = receive,
                Content = new SendParametersContent()
            };

        }

        protected override void CacheMetadata(NativeActivityMetadata metadata)
        {            
            if (this.Body == null)
            {
                metadata.AddValidationError("The Body property must contain an Activity.");
            }
            else if (impl == null)
            {    
                CreateImplementation();
                metadata.AddChild(impl);
            }
        }

        void CreateImplementation()
        {
            this.bodyClone = Activator.CreateInstance(this.Body.GetType()) as Activity;

            foreach (PropertyInfo pi in this.Body.GetType().GetProperties())
            {
                if (pi.CanWrite)
                {
                    pi.SetValue(this.bodyClone, pi.GetValue(this.Body, null), null);
                }

                if (pi.PropertyType.IsGenericType)
                {
                    Type argType = pi.PropertyType.GetGenericArguments()[0];
                    Type exprRefType = typeof(VisualBasicReference<>).MakeGenericType(argType);
                    Type exprValueType = typeof(VisualBasicValue<>).MakeGenericType(argType);

                    if (pi.PropertyType.GetGenericTypeDefinition() == typeof(InArgument<>))
                    {
                        // only expose InArguments that haven't already been bound 
                        if (pi.GetValue(this.Body, null) == null)
                        {
                            // create the Variable and add it internally
                            string variableName = pi.Name;
                            Variable var = Variable.Create(variableName, argType, VariableModifiers.None);
                            this.variables.Add(var);

                            // create the OutArgument by calling the VisualBasicReference<>(string expressionText) constructor and set it on the Receive                            
                            OutArgument outArg = Argument.Create(argType, ArgumentDirection.Out) as OutArgument;
                            outArg.Expression = (ActivityWithResult)Activator.CreateInstance(exprRefType, variableName);
                            ((ReceiveParametersContent)receive.Content).Parameters.Add(pi.Name, outArg);

                            // create the InArgument by calling the VisualBasicReference<>(string expressionText) constructor and set it to the Variable
                            InArgument inArg = Argument.Create(argType, ArgumentDirection.In) as InArgument;
                            inArg.Expression = (ActivityWithResult)Activator.CreateInstance(exprValueType, variableName);
                            pi.SetValue(this.bodyClone, inArg, null);
                        }                

                    }
                    else if (pi.PropertyType.GetGenericTypeDefinition() == typeof(OutArgument<>))
                    {
                        // create the Variable and add it internally
                        string variableName = pi.Name;
                        Variable var = Variable.Create(variableName, argType, VariableModifiers.None);
                        this.variables.Add(var);

                        if (pi.GetValue(this.Body, null) != null)
                        {
                            // copy the OutArgument
                            OutArgument refOutArg = ((OutArgument)pi.GetValue(this.Body, null));


                            string temp = refOutArg.Expression.ResultType.ToString();
                            InArgument assignInArg = Argument.Create(argType, ArgumentDirection.In) as InArgument;
                            assignInArg.Expression = (ActivityWithResult)Activator.CreateInstance(exprValueType, variableName);

                            Assign a = new Assign
                            {
                                To = refOutArg,
                                //To = OutArgument.CreateReference(varRef, pi.Name),
                                Value = assignInArg
                            };

                            assigns.Add(a);
                        }

                        // create an OutArgument by calling the VisualBasicReference<>(string expressionText) constructor and set it to the Variable
                        OutArgument outArg = Argument.Create(argType, ArgumentDirection.Out) as OutArgument;
                        outArg.Expression = (ActivityWithResult)Activator.CreateInstance(exprRefType, variableName);
                        pi.SetValue(this.bodyClone, outArg, null);

                        // create the InArgument by calling the VisualBasicReference<>(string expressionText) constructor and set it on the SendReply
                        InArgument inArg = Argument.Create(argType, ArgumentDirection.In) as InArgument;
                        inArg.Expression = (ActivityWithResult)Activator.CreateInstance(exprValueType, variableName);
                        ((SendParametersContent)reply.Content).Parameters.Add(variableName, inArg);

                    }
                }
            }


            // create internal Sequence and add the variables
            impl = new Sequence();
            foreach (Variable v in this.variables)
            {
                impl.Variables.Add(v);
            }

            // add the Receive
            receive.CanCreateInstance = this.CanCreateInstance;
            receive.OperationName = (this.DisplayName != "OperationScope" ? this.DisplayName : this.Body.DisplayName);
            impl.Activities.Add(receive);

            // add the activity which represents the operation body
            impl.Activities.Add(this.bodyClone);

            // add the Reply
            impl.Activities.Add(reply);

            // add any other Assigns to OutArguments
            foreach (Assign assignToOutArg in this.assigns)
            {
                impl.Activities.Add(assignToOutArg);
            }

        }

        protected override void Execute(NativeActivityContext context)
        {
            context.ScheduleActivity(this.impl);
        }
    }
}
