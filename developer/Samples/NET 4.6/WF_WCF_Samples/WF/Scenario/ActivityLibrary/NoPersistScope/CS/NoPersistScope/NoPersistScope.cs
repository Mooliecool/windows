//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------
using System.Activities;
using System.Activities.Statements;
using System.Activities.Validation;
using System.ComponentModel;
using System.Windows.Markup;

namespace Microsoft.Samples.Activities.Statements
{
        
    [ContentProperty("Body")]
    [Designer(typeof(NoPersistScopeDesigner))]
    public sealed class NoPersistScope : NativeActivity
    {
        Variable<NoPersistHandle> noPersistHandle;
        
        public NoPersistScope()
        {
            // add the validation to the list of validations for this activity
            this.Constraints.Add(VerifiyNoChildPersistActivity());

            // create the variable to hold the NoPersistHandle
            this.noPersistHandle = new Variable<NoPersistHandle>();
        }
                
        [DefaultValue(null)]
        public Activity Body { get; set; }

        protected override void CacheMetadata(NativeActivityMetadata metadata)
        {
            metadata.AddChild(this.Body);
            metadata.AddImplementationVariable(this.noPersistHandle);
        }

        // Execute the activity (enter the NoPersistScope)
        protected override void Execute(NativeActivityContext context)
        {
            NoPersistHandle handle = this.noPersistHandle.Get(context);
            handle.Enter(context);
            
            if (Body != null)
            {
                context.ScheduleActivity(Body);
            }
        }

        // Create a validation to verify that there are no Persist activites inside of a NoPersistScope
        static Constraint VerifiyNoChildPersistActivity()
        {
            DelegateInArgument<NoPersistScope> element = new DelegateInArgument<NoPersistScope>();
            DelegateInArgument<ValidationContext> context = new DelegateInArgument<ValidationContext>();
            DelegateInArgument<Activity> child = new DelegateInArgument<Activity>();
            Variable<bool> result = new Variable<bool>();            

            return new Constraint<NoPersistScope>
            {
                Body = new ActivityAction<NoPersistScope, ValidationContext>
                {
                    Argument1 = element,
                    Argument2 = context,
                    Handler = new Sequence
                    {
                        Variables =
                        {
                            result 
                        },
                        Activities =
                        {
                            new ForEach<Activity>
                            {                                
                                Values = new GetChildSubtree
                                {
                                    ValidationContext = context                                    
                                },
                                Body = new ActivityAction<Activity>
                                {                                    
                                    Argument = child, 
                                    Handler = new If()
                                    {                                          
                                        Condition = new InArgument<bool>((env) => object.Equals(child.Get(env).GetType(),typeof(Persist))),
                                        Then = new Assign<bool>
                                        {
                                            Value = true,
                                            To = result
                                        }
                                    }
                                }                                
                            },
                            new AssertValidation
                            {
                                Assertion = new InArgument<bool>(env => !result.Get(env)),
                                Message = new InArgument<string> ("NoPersistScope activity can't contain a Persist activity"),                                
                                PropertyName = new InArgument<string>((env) => element.Get(env).DisplayName)
                            }
                        }
                    }
                }
            };
        }
    }
}
