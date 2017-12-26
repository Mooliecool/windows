//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Activities;
using System.Activities.Validation;

namespace Microsoft.Samples.ContainmentValidation
{

    class Program
    {
        static void Main()
        {           

            Activity wf1 = new CreateState
            {
                Name = "California"
            };

            ValidationResults results = ActivityValidationServices.Validate(wf1);
            Console.WriteLine("WF1:");
            PrintResults(results);

            Activity wf2 = new CreateCountry
            {
                Name = "Mexico",
                States = 
                {
                    new CreateCity
                    {
                        Name = "Monterrey"
                    }
                }
            };

            results = ActivityValidationServices.Validate(wf2);
            Console.WriteLine("WF2:");
            PrintResults(results);

            Activity wf3 = new CreateCountry
            {
                Name = "USA",
                States =
                {                            
                    new CreateState
                    {
                        Name = "Texas",
                        Cities =
                        {                            
                            new CreateCity
                            {
                                Name = "Houston"
                            }
                        }
                    }
                }
            };
            results = ActivityValidationServices.Validate(wf3);
            Console.WriteLine("WF3:");
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
