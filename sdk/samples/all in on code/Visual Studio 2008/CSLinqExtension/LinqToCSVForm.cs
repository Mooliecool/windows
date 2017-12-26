/****************************** Module Header ******************************\
* Module Name:    LinqToCSVForm.cs
* Project:        CSLinqExtension
* Copyright (c) Microsoft Corporation.
*
* This LinqToCSVForm creates a CSV document based on some of the 
* All-In-One Code Framework project information and uses the simple 
* LINQ to CSV library to read and display the CSV file contents.  
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
using System.IO;
#endregion


namespace CSLinqExtension
{
    public partial class LinqToCSVForm : Form
    {
        public LinqToCSVForm()
        {
            InitializeComponent();

            // Create the CSV file
            CreateCSVFile();
        }

        // The file name of the CSV file
        private string filePath = "Projects.csv";


        #region Load and Display CSV File

        // Use the simple LINQ to CSV library to load and display the CSV
        // file contents.
        private void btnLoad_Click(object sender, EventArgs e)
        {
            try
            {
                // Read the CSV file using StreamReader
                using (StreamReader reader = new StreamReader(filePath))
                {
                    // Use the extension method Lines of the StreamReader
                    // to load the contents into anonymous type objects
                    var projects = from p in reader.Lines()
                                   select new { ProjectName = p[0], Language = p[1], Description = p[2], Owner = p[3] };

                    // Bind the data to DataGridView
                    dgvLinqToCSV.DataSource = projects.ToList();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        #endregion


        #region Create CSV File

        /// <summary>
        /// This method creates a CSV file based on some of the All-In-One Code Framework project
        /// information.
        /// </summary>
        private void CreateCSVFile()
        {
            // Create the CodoFx projects objects
            Project[] projects = new Project[2] 
            {
                new Project() { ProjectName = "CSDllCOMServer",Language = "C#", Description = "An in-process COM server in C#", Owner = "Jialiang Ge" },
                new Project() { ProjectName = "CSLinqToDataSets", Language = "C#", Description = "Use LINQ to DataSets in C#", Owner = "Lingzhi Sun" },
            };

            // Create the CSV file using StreamWriter
            try
            {
                using (StreamWriter writer = new StreamWriter(filePath))
                {
                    // Write comment and data structure in CSV file
                    writer.WriteLine("#All-In-One Code Framework Projects (format: Project Name, Program Language, Description, Owner)");

                    // Write the data
                    foreach (var proj in projects)
                    {
                        writer.WriteLine(String.Format("{0},{1},{2},{3}", proj.ProjectName, proj.Language, proj.Description, proj.Owner));
                    }
                }

                MessageBox.Show("Projects.csv created successfully!");
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        #endregion
    }


    #region The All-In-One Code Framework project class
    public class Project
    {
        // The project name
        public string ProjectName { get; set; }

        // The project programme language
        public string Language { get; set; }

        // The project description
        public string Description { get; set; }

        // The project owner
        public string Owner { get; set; }
    }
    #endregion
}
