//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Activities;
using System.Text.RegularExpressions;

namespace Microsoft.Samples.Activities.Statements
{

    class Program
    {
        static void Main(string[] args)
        {
            IsMatchSampleUse();

            MatchesSampleUse();

            ReplaceSampleUse();

            Console.WriteLine();
            Console.WriteLine("Press Enter to exit...");
            Console.ReadLine();
        }

        // Shows how to use IsMatch activity. This method contains two examples
        // 1) the input matches the regular expression 
        // 2) the input does not match the regular expression        
        static void IsMatchSampleUse()
        {               
            string pattern = @"^-?\d+(\.\d{2})?$";
            Console.WriteLine("Testing IsMatch (Currency expressions)");
            Console.WriteLine(string.Format("    Pattern: {0}", pattern));

            // this should match (20.00 is a valid currency)
            Console.WriteLine("    First test input: '20.00'");                    
            bool isMatchResult = WorkflowInvoker.Invoke(
                    new IsMatch
                    {
                        Pattern = new InArgument<string>(pattern),
                        Input = "20.00"
                    });
            Console.WriteLine("        Result: {0}", isMatchResult);

            // this shouldn't match ('this string' is not a valid currency)
            Console.WriteLine("    Second test input: 'this string'");
            isMatchResult = WorkflowInvoker.Invoke(
                    new IsMatch
                    {
                        Pattern = new InArgument<string>(pattern),
                        Input = "this string"
                    });
            Console.WriteLine("        Result: {0}", isMatchResult);
        }

        // Shows how to use Matches activity. 
        // Finds all the matches of a word in a phrase
        static void MatchesSampleUse()
        {
            // Matches sample
            Console.WriteLine("\r\nTesting Matches");
            Console.WriteLine("Input: 'The the quick brown fox  fox jumped over the lazy dog dog.'");
            MatchCollection matches = WorkflowInvoker.Invoke(
                new Matches
                    {
                        Pattern = @"\b(?<word>\w+)\s+(\k<word>)\b",
                        Input = "The the quick brown fox  fox jumped over the lazy dog dog."
                    });
            
            foreach (Match match in matches)
            {
                GroupCollection groups = match.Groups;
                Console.WriteLine("'{0}' repeated at positions {1} and {2}",
                                  groups["word"].Value,
                                  groups[0].Index,
                                  groups[1].Index);
            }
        }

        // Shows how to use Replace activity to replace 
        // all the occurrences of a pattern in a string 
        static void ReplaceSampleUse()
        {
            // Sample 1 - Replace using a string for replacement (Replacement propery)
            Console.WriteLine("\r\nTesting Replace (with string Replacement property)");
            Console.WriteLine("   Original: Hello World! This is a wonderful World");
            Console.WriteLine("   Replacing 'World' with 'Universe'...");
            string replaceTest = WorkflowInvoker.Invoke(
                new Replace
                {
                    Pattern = @"\bWorld\b",
                    Input = "Hello World! This is a wonderful World",
                    Replacement = "Universe"
                });
            Console.WriteLine("   Result: {0}", replaceTest);


            // Sample 2 - Replace using MatchEvaluator
            // Replace using a MatchEvaluator. In this case the pattern and input are parameterized
            string input = "Hello World! This is a wonderful world. world is wonderful.";
            string pattern = @"\bWorld\b";  
            Console.WriteLine("\r\nTesting Replace (with string Match property)");
            Console.WriteLine(string.Format("   Original: {0}", input));
            Console.WriteLine(string.Format("   Changing all the matches {0} to Pascal case...", pattern));

            replaceTest = WorkflowInvoker.Invoke(
                new Replace
                {
                    Pattern = new InArgument<string>(pattern),
                    Input = new InArgument<string>(input),
                    MatchEvaluator = new MatchEvaluator(CapText)                
                });
            Console.WriteLine("   Result: {0}", replaceTest);            
        }

        static string CapText(Match m)
        {
            // Get the matched string.
            string x = m.ToString();

            // If the first char is lower case...
            if (char.IsLower(x[0]))
            {
                // Capitalize it.
                return char.ToUpper(x[0]) + x.Substring(1, x.Length - 1);
            }
            return x;
        }
    }
}