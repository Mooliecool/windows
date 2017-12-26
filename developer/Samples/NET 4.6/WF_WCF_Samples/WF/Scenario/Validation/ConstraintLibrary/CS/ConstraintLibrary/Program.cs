//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Statements;
using System.Activities.Validation;
using System.Collections.Generic;

namespace Microsoft.Samples.ConstraintLibrary
{

    class Program
    {
        static void Main(string[] args)
        {
            //Create a WF with configuration errors
            Sequence wf = new Sequence()
            {
                Activities =
                {
                    new Sequence
                    {
                        DisplayName = "Sequence1"
                    },
                    new If
                    {
                        DisplayName = "If",
                        Condition = new InArgument<bool>(true)
                    },
                    new Switch<bool>
                    {
                        DisplayName = "Switch1",
                        Expression = new InArgument<bool>(true),  
                        Default = new WriteLine()    
                    },
                    new ForEach<int>
                    {
                        DisplayName = "ForEach2",
                        Values = new InArgument<IEnumerable<int>>((env) => new int[] { 1, 2, 3 })                        
                    },                    
                    new Parallel
                    {
                        DisplayName = "Parallel1"
                    },
                    new ParallelForEach<int>
                    {
                        DisplayName = "ParallelForEach1",
                        Values = new InArgument<IEnumerable<int>>((env) => new int[] { 1, 2, 3 })
                    },
                    new Pick
                    {
                        DisplayName = "Pick1",
                        Branches =
                        {
                            new PickBranch
                            {
                                Action = new WriteLine()
                            }
                        }
                    },
                    new Pick
                    {
                        DisplayName = "Pick2"
                    },
                    new WriteLine
                    {
                        DisplayName = "Wr"
                    }
                }
            };

            //Create an instance of Validation Settings.
            ValidationSettings settings = new ValidationSettings()
            {
                //Create value pairs constraints and activity types. We are providing a list of constraints that you want to apply on a specify activity type
                AdditionalConstraints =
                {
                    {typeof(Activity), new List<Constraint> {ConstraintLibrary.ActivityDisplayNameIsNotSetWarning()}},     
                    {typeof(ForEach<int>), new List<Constraint> {ConstraintLibrary.ForEachPropertyMustBeSetError<int>()}},
                    {typeof(WriteLine), new List<Constraint> {ConstraintLibrary.WriteLineHasNoTextWarning()}},
                    {typeof(Pick), new List<Constraint> {ConstraintLibrary.PickHasNoBranchesWarning(), ConstraintLibrary.PickHasOneBranchWarning()}},
                    {typeof(Parallel), new List<Constraint> {ConstraintLibrary.ParallelHasNoBranchesWarning()}},
                    {typeof(Switch<bool>), new List<Constraint> {ConstraintLibrary.SwitchHasDefaultButNoCasesWarning<bool>(), ConstraintLibrary.SwitchHasNoCasesOrDefaultWarning<bool>()}},                    
                    {typeof(If), new List<Constraint> {ConstraintLibrary.IfShouldHaveThenOrElseError()}},
                    {typeof(Sequence), new List<Constraint> {ConstraintLibrary.SequenceIsEmptyWarning()}}                         
                }
            };

            //Call the Validate method with the workflow you want to validate, and the settings you want to use.
            ValidationResults results = ActivityValidationServices.Validate(wf, settings);
            //Print the validation errors and warning that were generated my ActivityValidationServices.Validate.
            PrintResults(results);            
        }

        static void PrintResults(ValidationResults results)
        {
            Console.WriteLine();

            if (results.Errors.Count == 0 && results.Warnings.Count == 0)
            {
                Console.WriteLine("No warnings or errors");
            }
            else
            {
                foreach (ValidationError error in results.Errors)
                {
                    Console.WriteLine("Error in " + error.Source.DisplayName + ": " + error.Message);
                }
                foreach (ValidationError warning in results.Warnings)
                {
                    Console.WriteLine("Warning in " + warning.Source.DisplayName + ": " + warning.Message);
                }
            }
            Console.WriteLine();
        }
    }
}
