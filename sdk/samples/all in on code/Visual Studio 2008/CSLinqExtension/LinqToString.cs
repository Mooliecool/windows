/****************************** Module Header ******************************\
* Module Name:    LinqToString.cs
* Project:        CSLinqExtension
* Copyright (c) Microsoft Corporation.
*
* It is a simple LINQ to String library to show the digis characters in a 
* string, to count occurrences of a word in a string, and to query for 
* sentences that contain a specified set of words.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
#endregion


namespace CSLinqExtension
{
    // Static class LinqToCSV to make extension methods
    public static class LinqToString
    {
        /// <summary>
        /// Extension method of string to get the digit characters 
        /// in a string.
        /// </summary>
        /// <returns>IEnumerable collection digit characters</returns>
        public static IEnumerable<char> GetDigits(this string text)
        {
            // Get the chars if it is a digit
            var digits = from ch in text
                         where char.IsDigit(ch)
                         select ch;

            return digits;
        }

        /// <summary>
        /// Extension method of string to count occurrences of a word 
        /// in a string.
        /// </summary>
        /// <param name="searchTerm">The search item string</param>
        /// <returns>The search item occurences</returns>
        public static int GetWordOccurrence(this string text, string searchTerm)
        {
            // Split the source string into single words
            string[] source = text.Split(new char[] { '.', '?', '!', ' ', ';', ':', ',' }, StringSplitOptions.RemoveEmptyEntries);

            // Query the occurences of the search item
            int matchQueryCount = (from word in source
                                   where word.ToLowerInvariant() == searchTerm.ToLowerInvariant()
                                   select word).Count();

            return matchQueryCount;
        }

        /// <summary>
        /// Extension method of string to query for sentences that contain 
        /// a specified set of words.
        /// </summary>
        /// <param name="wordsToMatch">The search item set</param>
        /// <returns>The set of sentences</returns>
        public static IEnumerable<string> GetCertainSentences(this string text, 
            string[] wordsToMatch)
        {
            // Split the source string into single sentences
            var sentences = text.Split(new char[] { '.', '?', '!' });

            // Query for sentences that contain the set of search itmes
            var sentenceQuery = 
                from sentence in sentences
                let w = sentence.Split(new char[] { '.', '?', '!', ' ', ';', ':', ',' }, StringSplitOptions.RemoveEmptyEntries)
                where w.Distinct().Intersect(wordsToMatch).Count() == wordsToMatch.Count()
                select sentence;

            return sentenceQuery;
        }
    }
}
