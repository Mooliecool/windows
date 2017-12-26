//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;
using System.Activities.Statements;
using System.Activities.Validation;

namespace Microsoft.Samples.Activities.Data
{

    // This helper class creates a validation contraint that is used in EntityAdd,
    // EntityDelete, EntityLinqQuery, and EntitySqlQuery to verify that they have been
    // created inside of an ObjectContextScope.
    internal static class ConstraintHelper
    {
        public static Constraint VerifyParentIsObjectContextScope(Activity activity)
        {
            DelegateInArgument<Activity> element = new DelegateInArgument<Activity>();
            DelegateInArgument<ValidationContext> context = new DelegateInArgument<ValidationContext>();
            DelegateInArgument<Activity> child = new DelegateInArgument<Activity>();
            Variable<bool> result = new Variable<bool>();

            return new Constraint<Activity>
            {
                Body = new ActivityAction<Activity, ValidationContext>
                {
                    Argument1 = element,
                    Argument2 = context,
                    Handler = new Sequence
                    {
                        Variables = { result },
                        Activities =
                        {
                            new ForEach<Activity>
                            {                                
                                Values = new GetParentChain
                                {
                                    ValidationContext = context                                    
                                },
                                Body = new ActivityAction<Activity>
                                {                                    
                                    Argument = child, 
                                    Handler = new If
                                    {                                          
                                        Condition = new InArgument<bool>((env) => object.Equals(child.Get(env).GetType(), typeof(ObjectContextScope))),
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
                                Assertion = new InArgument<bool>(env => result.Get(env)),
                                Message = new InArgument<string> (string.Format("{0} can only be added inside an ObjectContextScope activity.", activity.GetType().Name)),
                                PropertyName = new InArgument<string>((env) => element.Get(env).DisplayName)
                            }
                        }
                    }
                }
            };
        }
    }
}
