//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;
using System.Activities.Statements;
using System.Activities.Validation;

namespace Microsoft.Samples.ExternalActivityValidation
{

    class ConstraintsLibrary
    {
        // Constraint is just an activity that contains validation logic.
        public static Constraint ConstraintError_IfShouldHaveThenOrElse() 
        {
            DelegateInArgument<If> element = new DelegateInArgument<If>();

            return new Constraint<If>
            {
                Body = new ActivityAction<If, ValidationContext>
                {
                    Argument1 = element,
                    Handler = new AssertValidation
                    {                        
                        Assertion = new InArgument<bool>(env => (element.Get(env).Then != null) || (element.Get(env).Else != null)),
                        Message = new InArgument<string>("'If' activity should have either Then or Else activity set."),
                    }
                }
            };
        }

        public static Constraint ConstraintWarning_PickHasOneBranch() 
        {
            DelegateInArgument<Pick> element = new DelegateInArgument<Pick>();

            return new Constraint<Pick>
            {
                Body = new ActivityAction<Pick, ValidationContext>
                {
                    Argument1 = element,
                    Handler = new AssertValidation
                    {
                        IsWarning = true,
                        Assertion = new InArgument<bool>(env => (element.Get(env).Branches.Count == 0) || (element.Get(env).Branches.Count > 1)),
                        Message = new InArgument<string>("This Pick activity has only one branch so the Pick itself is redundant."),
                    }
                }
            };
        }
    }
}
