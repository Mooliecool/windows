//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

//This sample shows how to add validation logic to an out of the box activity, in other words to add validation to an activity 
//when you are not the activity author. In our scenario an activity author (from a 3rth party software company) wants to 
//enforce that all If activities must have an assign activity to Then or Else. They also want to warn the workflow author 
//of having a Pick activity with only one branch (it would be redundant). 

using System;
using System.Activities.Statements;
using System.Activities.Validation;
using System.Collections.Generic;

namespace Microsoft.Samples.ExternalActivityValidation
{

    class Program
    {
        static void Main()
        {
            Sequence wf = new Sequence
            {
                Activities =
                {
                    new If
                    {
                        Condition = true
                    },
                    new Pick
                    {
                        Branches =
                        {
                            new PickBranch
                            {
                                Trigger = new WriteLine
                                {
                                    Text = "When this completes..."
                                },
                                Action = new WriteLine
                                {
                                    Text = "... do this."
                                }
                            }
                        }
                    }
                }
            };

            // ValidationSettings enables the host to customize the behavior of ActivityValidationServices.Validate.
            ValidationSettings validationSettings = new ValidationSettings
            {
                // AdditionalConstraints enables the host to add specific validation logic (a constraint) to a specify type of activity in the Workflow.
                AdditionalConstraints =
                    {                             
                        {typeof(If), new List<Constraint> {ConstraintsLibrary.ConstraintError_IfShouldHaveThenOrElse()}}, 
                        {typeof(Pick), new List<Constraint> {ConstraintsLibrary.ConstraintWarning_PickHasOneBranch()}}
                    }
            };

            ValidationResults results = ActivityValidationServices.Validate(wf, validationSettings);
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
                // A ValidationError contains the message and error level information of a constraint failure.  
                // Note that a single constraint can generate zero or more ValidationErrors in a single execution.
                foreach (ValidationError error in results.Errors)
                {
                    Console.WriteLine("Error: " + error.Message);
                }
                foreach (ValidationError warning in results.Warnings)
                {
                    Console.WriteLine("Warning: " + warning.Message);
                }
            }

            Console.WriteLine();
        }
    }
}
