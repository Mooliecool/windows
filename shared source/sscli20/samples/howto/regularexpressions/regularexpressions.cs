//------------------------------------------------------------------------------
// <copyright file="regularexpressions.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

using System;
using System.Text.RegularExpressions;

namespace RegularExpressionSample
{
	/// <summary>
	/// Main entry point for sample
	/// </summary>
	public class RegularExpressionsTest
	{
    public static void Main(string [] args)
    {
      Console.WriteLine("Regex.Escape static method allows creation of properly escaped constant strings\n");
      Console.Write("Input some text to be escaped: ");
      string Reply = Console.ReadLine();
      Console.WriteLine("Escaped string constant is: {0}", Regex.Escape(Reply));
      
      Console.WriteLine("\nVarious Expressions\n");


      ExpressionResults(@"(\d+)", 
                       "Simple decimal pattern (try abc1234)", 
                        RegexOptions.None,
                        null);

      ExpressionResults(@"(\W{3})", 
                       "3 non-word chars in a row (try abc;;;)", 
                       RegexOptions.None,
                       null);

      ExpressionResults(@"([^aeiou]+)", 
                      "One or more non-vowels (try abcdefgh)", 
                       RegexOptions.None,
                       null);

      ExpressionResults(@"(\S*\d+\s{2,3}\w+)", 
                       "Zero or more non-spaces, number, 2-3 space characters (try a4  x)", 
                       RegexOptions.None,
                       null);

      ExpressionResults(@"(\d+?[a-d]+?)", 
                       "At least one number and at least one from set [a-d] (try 234abcd)", 
                       RegexOptions.None,
                       null);

      ExpressionResults(@"((abc)+(\b[;,:]))", 
                       "Punctuation on word boundary (try abc;;;)", 
                       RegexOptions.None,
                       null);

/*
The next item illustrates the next match after an empty match.

A search for "a*" in the string "abaabb" returns the following sequence of matches.
"a", "", "aa", "", "", ""

Here is another view, in context:
(a)()b(aa)()b()b()

The first match gets the first a. The second match starts exactly where the first match ended, 
before the first b; it finds zero occurrences of a and returns the empty string.

The third match does not begin exactly where the second match ended, because the second match 
returned the empty string. Instead, it begins one character later, after the first b. 
The third match finds two occurrences of a and returns "aa".

The fourth match begins where the third match ended, before the second b, and finds the empty string. 
Then the fifth match begins before the last b and finds the empty string again. 
The sixth match begins after the last b and finds the empty string yet again.
*/ 

      ExpressionResults(@"(a*)", 
                       "Advancing on each empty or non-empty match (try abaabb)", 
                        RegexOptions.None,
                        null);

      ExpressionResults(@"gr(a|e)y", 
                       "Alternation 'or' and backtracking (try greengraygrowngrey)", 
                       RegexOptions.None,
                       null);

      ExpressionResults(@"(?(\d+)8080|port)", 
                       "Alternation swap (try 8080 or port)", 
                       RegexOptions.None,
                       null);

      ExpressionResults(@"(\d)\1", 
                       "Backreference 1 digit (try 5566)", 
                        RegexOptions.None,
                        null);

      ExpressionResults(@"(?<1>x)(?<1>\1y)*", 
                       "Backreference with multiple captures - most recent capture used (try xxyxyy)", 
                        RegexOptions.None,
                        null);

      ExpressionResults(@"(?<dig>\d)x\<dig>", 
                       "Backreference using named group (try 5x5)", 
                       RegexOptions.None,
                       null);

      ExpressionResults("(x(y))z", 
                       "Nested group pattern (try xyzxyzxyz)",
                        RegexOptions.None,
                        null);

      ExpressionResults("((x(y))z)+", 
                       "Nested group pattern with captures (try xyzxyzxyz)", 
                        RegexOptions.None,
                        null);

      ExpressionResults("((x(y))z)+?", 
                       "Nested with lazy ? (try xyzxyzxyz)",
                       RegexOptions.None,
                       null);


      ExpressionResults("((x(y))z)+", 
                       "Nested with RightToLeft option (try xyzxyzxyz)", 
                       RegexOptions.RightToLeft,
                       null);

      ExpressionResults("((x(?<1>y))z)+", 
                       "Nested with captures and option ExplicitCapture (try xyzxyzxyz)",
                       RegexOptions.ExplicitCapture,
                       null);

      ExpressionResults("((x(?:y))z)+", 
                       "Nested with captures and a non-capturing group (try xyzxyzxyz)",
                       RegexOptions.None,
                       null);

      ExpressionResults("((x(y))z(?=a))+", 
                       "Nested with zero-width lookahead (try xyzaxyza)",
                       RegexOptions.None,
                       null);

      ExpressionResults("(((?<=a)x(y))z)+", 
                       "Nested with zero-width lookbehind (try axyzaxyz)",
                       RegexOptions.None,
                       null);

      ExpressionResults("href\\s*=\\s*(?:\"(?<1>[^\"]*)\"|(?<1>\\S+))", 
                       "Scanning for hrefs (try href=#abcd)",
                       RegexOptions.IgnoreCase,
                       new string [] {"1"});

      ExpressionResults(@"^(?<proto>\w+)://[^/]+?(?<port>:\d+)?/", 
                       "Scanning URLs for protocol and port (try http://www.contoso.com:8080/test.html)",
                        RegexOptions.IgnoreCase,
                        new string [] {"proto", "port"});

//Begin replacement section    
      Console.WriteLine("\nSubstitutions in Replacements");
      Console.WriteLine(  "-----------------------------\n");

/* 
There are several ways to use substitutions within the Regex object model and each works differently.

Regex.Replace() takes the match and replaces it with the content of the replacement string.
By default if there is a match the output of Replace will include all the content not matched.
You can use the substitution syntax to access the content of the match using its group number or
name if it has one.

If you just want the content of named groups arranged differently but not the unmatched content,
then use the Result() method of the Match object.  By default the Result() method returns nothing that
is not specified explicitly in the replacement string.
*/

      ReplacementResults(@"(test)+",
                         @"",
                         "Removing a string (try abctestdef)",
                         RegexOptions.None);

      ReplacementResults(@"(\d)",
                         @"*",
                         "Changing digits to asterixes",
                         RegexOptions.None);

      ReplacementResults(@"(?:\()(?<area>\d{3})(?:\)\s*)(?<pre>\d{3})(?:-)(?<post>\d{4})",
                         @"areacode: ${area}, prefix: ${pre}, postfix: ${post}",
                         "Telephone number from formatted string (try (800) 555-1212)",
                         RegexOptions.None);

//Begin substitutions section
      Console.WriteLine("\nMatch.Result Substitutions");
      Console.WriteLine(   "-------------------------\n");

      SubstitutionResults(@"(test)+",
                          @"$`$'",
                          "Removing matched content (try abctestdef)",
                          RegexOptions.None);

      SubstitutionResults(@"(a(b))c",
                          @"last captured group: $+, group 0: $0",
                          "Getting the last captured group (try abcabc)",
                          RegexOptions.None);

      SubstitutionResults(@"(?:\D+)(?<num>[1-5]+)(?:\D+)",
                          @"total match: $&, named group num: ${num} ",
                          "Getting entire match vs named group (try 111aaa222bbb)",
                          RegexOptions.None);
    }//Main()

    /// <summary>
    /// Outputs the results of expression pattern results
    /// </summary>
    /// <param name="patternString"></param>
    /// Expression pattern.
    /// <param name="patternDescription"></param>
    /// Text to describe output to user.
    /// <param name="options"></param>
    /// Regex options
    /// <param name="namedGroups"></param>
    /// If not null this will contain a string array of named groups to output.
    public static void ExpressionResults(string patternString, 
                                         string patternDescription,
                                         RegexOptions options, 
                                         string [] namedGroups)
    {
      Console.WriteLine("{0}\n", patternDescription);
      Console.WriteLine("Pattern: {0}\n", patternString);
      Console.Write("Enter your test input: ");
      string userString = Console.ReadLine();
      Console.WriteLine();
      Regex rgx = new Regex(patternString, options);

      while (true)
      {
        MatchCollection mc = rgx.Matches(userString);

        if (mc.Count > 0)
        {
          Console.WriteLine("MatchCollection from Matches property");
          Console.WriteLine("\nTest string with character position");
          Console.WriteLine("\n{0}", userString);
          for (int i=0; i < userString.Length; i++)
          {
            Console.Write("-");
          }//for
          Console.WriteLine();
          for (int i=0; i < userString.Length; i++)
          {
            Console.Write(i%10);
          }//for
          Console.WriteLine("\n");

          //foreach (Match m in mc)
          for (int i=0; i < mc.Count; i++)
          {
            Match m = mc[i];
            GroupCollection gps = m.Groups;
            if (namedGroups == null)
            {
              Console.WriteLine("Match value item {0}: at index {1}: {2}", i, m.Index, m.Value);
              Console.WriteLine();
              for (int j = 0; j < gps.Count; j++)
              {
                Group g = gps[j];
                Console.WriteLine("\tGroup item {0}: at index location {1}: {2}", j, g.Index, g.Value);
                CaptureCollection caps = g.Captures;
                for (int k = 0; k < caps.Count; k++)
                {
                  Capture c = caps[k];
                  Console.WriteLine("\t\tCapture item {0} at index location {1}: {2}", k, c.Index, c.Value);
                }//for
                Console.WriteLine();
              }//for
            }//if
            else
            {
              foreach (string s in namedGroups)
              {
                Console.WriteLine("Found named group match <{0}> at location {1}: {2}",
                  s,
                  m.Groups[s].Index,
                  m.Groups[s].Value);
              }//foreach
            }//else
          }//foreach
        }//if
        else
        {
          Console.WriteLine("No matches found");
        }//else
        Console.Write("\nNew test input for {0} or enter to continue: ", patternString);
        userString = Console.ReadLine();
        Console.WriteLine();
        if (userString != "")
        {
          continue;
        }//if
        else  //exit the loop
        {
          Console.WriteLine("\n");
          return;
        }//else
      }//while (true)
    }//ExpressionResults()

    /// <summary>
    /// Outputs results of Regex.Replace()
    /// </summary>
    /// <param name="patternString"></param>
    /// Regular expression pattern.
    /// <param name="replacementString"></param>
    /// Replacement string with substitution patterns.
    /// <param name="replacementDescription"></param>
    /// Description of replacement for user.
    /// <param name="options"></param>
    /// Regex options.
    public static void ReplacementResults(string patternString, 
                                          string replacementString,
                                          string replacementDescription,
                                          RegexOptions options)
    {
      Console.WriteLine("{0}\n", replacementDescription);
      Console.WriteLine("Pattern: {0}", patternString);
      Console.WriteLine("Replacement pattern: {0}\n", replacementString);
      Console.Write("Enter input for replacement: ");
      string userString = Console.ReadLine();
      Console.WriteLine();
      Regex rgx = new Regex(patternString, options);

      while (true)
      {
      
        if (rgx.IsMatch(userString))
        {
          string result = rgx.Replace(userString, replacementString);
          Console.WriteLine("Replacement results: {0}", result);
        }//if
        else
        {
          Console.WriteLine("No match");
        }//else

        Console.Write("\nNew replacment input for {0} or enter to continue: ", patternString);
        userString = Console.ReadLine();
        Console.WriteLine();
        if (userString != "")
        {
          continue;
        }//if
        else  //exit the loop
        {
          Console.WriteLine("\n");
          return;
        }//else
      }//while (true)
    }//ReplacementResults()

    /// <summary>
    /// Outputs the result of Regex.Match().Result().
    /// </summary>
    /// <param name="patternString"></param>
    /// Regular expression pattern string.
    /// <param name="substitutionString"></param>
    /// Substitution string for Result() method.
    /// <param name="substitutionDescription"></param>
    /// Description of substitution for user.
    /// <param name="options"></param>
    /// Regex options.
    public static void SubstitutionResults(string patternString, 
                                           string substitutionString,
                                           string substitutionDescription,
                                           RegexOptions options)
    {
      Console.WriteLine("{0}\n", substitutionDescription);
      Console.WriteLine("Pattern: {0}", patternString);
      Console.WriteLine("Replacement pattern: {0}\n", substitutionString);
      Console.Write("Enter substitution string: ");
      string userString = Console.ReadLine();
      Console.WriteLine();
      Regex rgx = new Regex(patternString, options);

      while (true)
      {
        if (rgx.IsMatch(userString))
        {
          string result = rgx.Match(userString).Result(substitutionString);
          Console.WriteLine("Substitution results: {0}", result);
        }//if
        else
        {
          Console.WriteLine("No match");
        }//else

        Console.Write("\nNew substitution string for {0} or enter to continue: ", patternString);
        userString = Console.ReadLine();
        Console.WriteLine();
        if (userString !="")
        {
          continue;
        }//if
        else
        {
          Console.WriteLine("\n");
          return;
        }//else
      }//while (true)
    }//SubstitutionResults()
	}//class RegularExpressions
}//namespace
