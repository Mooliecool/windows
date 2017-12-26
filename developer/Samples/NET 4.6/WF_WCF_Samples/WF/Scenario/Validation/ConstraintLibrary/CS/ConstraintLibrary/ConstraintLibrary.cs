//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;
using System.Activities.Statements;
using System.Activities.Validation;

namespace Microsoft.Samples.ConstraintLibrary
{

    class ConstraintLibrary
    {
        public static Constraint ActivityDisplayNameIsNotSetWarning()
        {
            DelegateInArgument<Activity> element = new DelegateInArgument<Activity>();

            return new Constraint<Activity>
            {
                Body = new ActivityAction<Activity, ValidationContext>
                {
                    Argument1 = element,
                    Handler = new AssertValidation
                    {
                        IsWarning = true,
                        Assertion = new InArgument<bool>(env => (element.Get(env).DisplayName.Length > 2)),
                        Message = new InArgument<string>("It is a best practice to have a DisplayName of more than 2 characters."),
                    }
                }
            };
        }

        //Generic version of "ConstraintMessage_ForEachPropertyMustBeSet"
        public static Constraint ForEachPropertyMustBeSetError<T>()
        {
            DelegateInArgument<ForEach<T>> element = new DelegateInArgument<ForEach<T>>();

            return new Constraint<ForEach<T>>
            {
                Body = new ActivityAction<ForEach<T>, ValidationContext>
                {
                    Argument1 = element,
                    Handler = new Sequence
                    {
                        Activities = 
                        {
                            new AssertValidation 
                            {
                                Assertion = new InArgument<bool>((env) => element.Get(env).Body != null),
                                Message = new InArgument<string>((env) => "Body is a required property for " + element.Get(env).GetType().Name + "."),                                                              
                            },
                            new AssertValidation 
                            {
                                Assertion = new InArgument<bool>((env)  => element.Get(env).Body == null // guard against null refs w/ OR
                                    || element.Get(env).Body.Handler != null),
                                Message = new InArgument<string>((env) => "Body.Handler is a required property for " + element.Get(env).GetType().Name + "."),
                            },
                        }
                    }
                }
            };
        }

        public static Constraint WriteLineHasNoTextWarning()
        {
            DelegateInArgument<WriteLine> element = new DelegateInArgument<WriteLine>();

            return new Constraint<WriteLine>
            {
                Body = new ActivityAction<WriteLine, ValidationContext>
                {
                    Argument1 = element,
                    Handler = new AssertValidation
                    {
                        IsWarning = true,
                        Assertion = new InArgument<bool>(env => (element.Get(env).Text != null)),
                        Message = new InArgument<string>("This WriteLine has no Text set and will write a blank line."),
                    }
                }
            };
        }

        public static Constraint PickHasOneBranchWarning()
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
                        Assertion = new InArgument<bool>(env => !(element.Get(env).Branches.Count == 1)),
                        Message = new InArgument<string>("This Pick activity has only one branch so the Pick itself is redundant."),
                    }
                }
            };
        }

        public static Constraint PickHasNoBranchesWarning()
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
                        Assertion = new InArgument<bool>(env => (element.Get(env).Branches.Count > 0)),
                        Message = new InArgument<string>("This Pick activity has no branches and won't do anything."),
                    }
                }
            };
        }

        public static Constraint ParallelHasNoBranchesWarning()
        {
            DelegateInArgument<Parallel> element = new DelegateInArgument<Parallel>();

            return new Constraint<Parallel>
            {
                Body = new ActivityAction<Parallel, ValidationContext>
                {
                    Argument1 = element,
                    Handler = new AssertValidation
                    {
                        IsWarning = true,
                        Assertion = new InArgument<bool>(env => (element.Get(env).Branches.Count > 0)),
                        Message = new InArgument<string>("Parallel activity has no branches set, so it won't do anything."),
                    }
                }
            };
        }

        public static Constraint SwitchHasDefaultButNoCasesWarning<T>()
        {
            DelegateInArgument<Switch<T>> element = new DelegateInArgument<Switch<T>>();

            return new Constraint<Switch<T>>
            {
                Body = new ActivityAction<Switch<T>, ValidationContext>
                {
                    Argument1 = element,
                    Handler = new AssertValidation
                    {
                        Assertion = new InArgument<bool>(env => !((element.Get(env).Cases.Count == 0) && (element.Get(env).Default != null))),
                        Message = new InArgument<string>("A Switch with a 'Default' but no cases is redundant, since you could just execute the 'Default' directly. Did you mean to add some cases?"),
                    }
                }
            };
        }

        public static Constraint SwitchHasNoCasesOrDefaultWarning<T>()
        {
            DelegateInArgument<Switch<T>> element = new DelegateInArgument<Switch<T>>();

            return new Constraint<Switch<T>>
            {
                Body = new ActivityAction<Switch<T>, ValidationContext>
                {
                    Argument1 = element,
                    Handler = new AssertValidation
                    {
                        Assertion = new InArgument<bool>(env => (element.Get(env).Cases.Count > 0) || (element.Get(env).Default != null)),
                        Message = new InArgument<string>("A Switch with no cases and no 'Default' set will never do anything."),
                    }
                }
            };
        }

        public static Constraint IfShouldHaveThenOrElseError()
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

        public static Constraint SequenceIsEmptyWarning()
        {
            DelegateInArgument<Sequence> element = new DelegateInArgument<Sequence>();

            return new Constraint<Sequence>
            {
                Body = new ActivityAction<Sequence, ValidationContext>
                {
                    Argument1 = element,
                    Handler = new AssertValidation
                    {
                        IsWarning = true,
                        Assertion = new InArgument<bool>(env => (element.Get(env).Activities.Count > 0)),
                        Message = new InArgument<string>("This Sequence has no Activities in it and won't do anything."),
                    }
                }
            };
        }
    }
}
