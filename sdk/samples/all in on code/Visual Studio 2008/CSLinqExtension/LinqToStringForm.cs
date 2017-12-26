/****************************** Module Header ******************************\
* Module Name:    LinqToStringForm.cs
* Project:        CSLinqExtension
* Copyright (c) Microsoft Corporation.
*
* This LinqToStringForm uses the simple LINQ to String library to show the 
* digis characters in a string, to count occurrences of a word in a string, 
* and to query for sentences that contain a specified set of words.
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
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
#endregion


namespace CSLinqExtension
{
    public partial class LinqToStringForm : Form
    {
        public LinqToStringForm()
        {
            InitializeComponent();
        }


        #region Show Digists In a String
        private void btnDigist_Click(object sender, EventArgs e)
        {
            try
            {
                string text = txtBoxDigit.Text;

                // Check whether the string is empty
                if (String.IsNullOrEmpty(text))
                {
                    MessageBox.Show("Input string is empty!");
                    return;
                }

                // Call the GetGigits extension method to get all the digits
                var digits = text.GetDigits();

                // Display all the digits in a string
                string output = "";

                foreach (var ch in digits)
                {
                    output += ch.ToString();
                }

                MessageBox.Show(String.Format("The digits in the string: {0}", output));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }
        #endregion


        #region Count Occurences of a Word
        private void btnSearchCount_Click(object sender, EventArgs e)
        {
            try
            {
                string searchItem = txtBoxOccurence.Text;

                // Check whether the search source string is empty
                if (String.IsNullOrEmpty(txtSearchSource.Text))
                {
                    MessageBox.Show("Search source is empty!");
                    return;
                }

                // Check whether the search item string is empty
                if (String.IsNullOrEmpty(searchItem))
                {
                    MessageBox.Show("Search item is empty!");
                    return;
                }

                // Call the GetWordOccurrence extension method to get the
                // search item occurences in the search source text
                int wordCount = txtSearchSource.Text.GetWordOccurrence(
                    searchItem);

                // Display the query result
                MessageBox.Show(String.Format("The occurences of '{0}': {1}", searchItem, wordCount));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }
        #endregion


        #region Query for Sentences That Contain a Specified Set of Words
        private void btnSearchSentence_Click(object sender, EventArgs e)
        {
            try
            {
                // Split the search item sets into a string array
                string[] wordToSearch = txtBoxSearchItems.Text.Split(new 
                    char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);

                // Check whether the search source string is empty
                if (String.IsNullOrEmpty(txtSearchSource.Text))
                {
                    MessageBox.Show("Search source is empty!");
                    return;
                }

                // Check if the search item set is empty
                if (wordToSearch.Length == 0)
                {
                    MessageBox.Show("Search items set is empty!");
                    return;
                }

                // Call the GetCertainSentences extension method to query 
                // for sentences that contain a specified set of words.
                var sentences = txtSearchSource.Text.GetCertainSentences(
                    wordToSearch);

                // Display the query result
                string output = "";

                foreach (var sen in sentences)
                {
                    output += sen + "\r\n\r\n";
                }

                MessageBox.Show(String.Format("The sentence(s) with ({0}):\r\n\r\n{1}", txtBoxSearchItems.Text, output));
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }
        #endregion
    }
}
