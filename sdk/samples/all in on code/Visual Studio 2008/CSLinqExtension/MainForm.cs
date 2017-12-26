/****************************** Module Header ******************************\
* Module Name:    MainForm.cs
* Project:        CSLinqExtension
* Copyright (c) Microsoft Corporation.
*
* The CSLinqExtension sample demonstrates the Microsoft Language-Integrated 
* Query (LINQ) technology to access CSV files, File System information, .NET
* assemblies(Reflection), and string text.  
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
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }


        #region LINQ Extension Introduction

        // LINQ to CSV Introduction
        private string LinqToCSVIntro = 
            "A simple LINQ to CSV library to load simple CSV files into string arrays line by line.\r\n\r\n" + 
            "The CSV file stores All-In-One Code Framework project information and its format:\r\n(Project Name, Program Language, Description, Owner)";

        // LINQ to File System Introduction
        private string LinqToFileSystemIntro = 
            "A simple LINQ to File System library to show files grouped by extension names under one folder, " +
            "and to display the largest file name and the size.";
        
        // LINQ to Reflection Introduction
        private string LinqToReflectionIntro = 
            "A simple LINQ to Reflection library to display the methods of the exported class in an assembly grouped by return value type.";

        // LINQ to String Introduction
        private string LinqToStringIntro = 
            "A simple LINQ to String library to show the digis characters in a string, to count occurrences of a word in a string, " +
            "and to query for sentences that contain a specified set of words.";

        #endregion


        #region Load LINQ Extension Forms Button Event Handlers

        // LINQ to CSV Button
        private void btnLinqToCSV_Click(object sender, EventArgs e)
        {
            LinqToCSVForm form = new LinqToCSVForm();
            form.ShowDialog();
        }

        // LINQ to File System Button
        private void btnLinqToFileSystem_Click(object sender, EventArgs e)
        {
            LinqToFileSystemForm form = new LinqToFileSystemForm();
            form.ShowDialog();
        }

        // LINQ to Reflection Button
        private void btnLinqToReflection_Click(object sender, EventArgs e)
        {
            LinqToReflectionForm form = new LinqToReflectionForm();
            form.ShowDialog();
        }

        // LINQ to String Button
        private void btnLinqToString_Click(object sender, EventArgs e)
        {
            LinqToStringForm form = new LinqToStringForm();
            form.ShowDialog();
        }

        // LINQ to String Button Mouse Enter
        private void btnLinqToCSV_MouseEnter(object sender, EventArgs e)
        {
            // Display LINQ to CSV Introduction
            lblIntroduction.Text = "LINQ to CSV";   
            txtBoxIntro.Text = LinqToCSVIntro;
        }

        // LINQ to File System Button Mouse Enter
        private void btnLinqToFileSystem_MouseEnter(object sender, EventArgs e)
        {
            // Display LINQ to File System Introduction
            lblIntroduction.Text = "LINQ to File System";
            txtBoxIntro.Text = LinqToFileSystemIntro;
        }

        // LINQ to Reflection Button Mouse Enter
        private void btnLinqToReflection_MouseEnter(object sender, EventArgs e)
        {
            // Display LINQ to Reflection Introduction
            lblIntroduction.Text = "LINQ to Reflection";
            txtBoxIntro.Text = LinqToReflectionIntro;
        }

        // LINQ to String Button Mouse Enter
        private void btnLinqToString_MouseEnter(object sender, EventArgs e)
        {
            // Display LINQ to String Introduction
            lblIntroduction.Text = "LINQ to String";
            txtBoxIntro.Text = LinqToStringIntro;
        }

        // Button Mouse Leave
        private void btn_MouseLeave(object sender, EventArgs e)
        {
            // Clear the LINQ Extension Introduction
            lblIntroduction.Text = "";
            txtBoxIntro.Text = "";
        }

        #endregion
    }
}
