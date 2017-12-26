//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using System.Collections.Specialized ;

using System.Collections;
using System.Globalization;

namespace Microsoft.Samples.XLinq
{
    /// <summary>
    /// This class holds the history of list of Unified Searches done by the user
    /// </summary>
    class SearchHistory
    {
     
        /// <summary>
        /// private List of string that holds the search history
        /// </summary>
        private List<string> values;


        /// <summary>
        /// public property that returns the last 5 search queries
        /// </summary>
        public NameValueCollection SearchHistoryValues
        {
            get 
            {
                //create a new NameValueCollection
                NameValueCollection searchHistory =new NameValueCollection ();
                int i=0;
                //take top 5 in the list and add it to the NameValueCollection
                foreach (string s in this.values.Take(5).ToList())
                {
                    string key = string.Format(CultureInfo.CurrentCulture,"search{0}", ++i);
                    searchHistory.Add(key, s);
                }

                return searchHistory;
                
            
            }            
        }
        
        /// <summary>
        /// Constructor for the class
        /// </summary>
        /// <param name="history"></param>
        public SearchHistory(NameValueCollection history)
        {
            //add the search queries to the list (only distinct ones)
            this.values = new List<string>();
            foreach (string key in history.Keys.OfType <string>().Distinct())
            {                
                this.values.Add(history[key]);            
            }

        }

        /// <summary>
        /// Adds a new entry to the top of the list
        /// </summary>
        /// <param name="keyword"></param>
        public void AddEntry(string keyword)
        {
            //if the list does not contain the keyword, then add it
            if(!this.values.Contains(keyword))
                this.values.Insert(0,keyword);
        }

        /// <summary>
        /// Returns the match string if it exists in the list
        /// </summary>
        /// <param name="s"></param>
        /// <returns></returns>
        public string Match(string s)
        {
            //call the Find method and pass it the Lambda Expression
            //lambda expression is v => v.ToLower().StartsWith(s.ToLower())
            string match = this.values.Find(v => v.ToLower().StartsWith(s.ToLower()));
            return match;
        }
    }
}
