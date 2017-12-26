//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;
using System.Activities.Statements;
using System.Activities.Validation;
using System.Collections.Generic;

namespace Microsoft.Samples.ContainmentValidation
{

    public sealed class CreateState : CodeActivity
    {
        public CreateState()
        {
            base.Constraints.Add(CheckParent());
            this.Cities = new List<Activity>();            
        }
        
        public List<Activity> Cities { get; set; }

        public string Name { get; set; }  

        static Constraint CheckParent()
        {
            DelegateInArgument<CreateState> element = new DelegateInArgument<CreateState>();
            DelegateInArgument<ValidationContext> context = new DelegateInArgument<ValidationContext>();                        
            Variable<bool> result = new Variable<bool>();
            DelegateInArgument<Activity> parent = new DelegateInArgument<Activity>();
           
            return new Constraint<CreateState>
            {                                   
                Body = new ActivityAction<CreateState,ValidationContext>
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
                                Values = new GetParentChain
                                {
                                    ValidationContext = context                                    
                                },
                                Body = new ActivityAction<Activity>
                                {   
                                    Argument = parent, 
                                    Handler = new If()
                                    {                                          
                                        Condition = new InArgument<bool>((env) => object.Equals(parent.Get(env).GetType(),typeof(CreateCountry))),                                        
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
                                Assertion = new InArgument<bool>(result),
                                Message = new InArgument<string> ("CreateState has to be inside a CreateCountry activity"),                                                                
                            }
                        }
                    }
                }
            };
        }

        protected override void Execute(CodeActivityContext context)
        {
            // not needed for the sample
        }
    }    
}
