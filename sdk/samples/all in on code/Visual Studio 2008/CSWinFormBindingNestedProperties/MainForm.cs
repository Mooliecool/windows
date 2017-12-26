/************************************* Module Header **************************************\
* Module Name:  MainForm.cs
* Project:      CSWinFormBindToNestedProperties
* Copyright (c) Microsoft Corporation.
* 
* The sample demonstrates how to bind a DataGridView column to a nested property 
* in the data source.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\******************************************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Reflection;

namespace BindingNestedProperties
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }
                
        BindingList<Person> mylist = new BindingList<Person>();
        
        private void Form1_Load(object sender, EventArgs e)
        {         
            // bind the DataGridView to the list
            this.dataGridView1.AutoGenerateColumns = false;
            this.dataGridView1.DataSource = mylist;

            this.dataGridView1.Columns.Add("ID", "ID");
            this.dataGridView1.Columns.Add("Name", "Name");
            this.dataGridView1.Columns.Add("CityName", "City Name");
            this.dataGridView1.Columns.Add("PostCode", "Post Code");

            (this.dataGridView1.Columns["ID"] as DataGridViewTextBoxColumn).DataPropertyName = "ID";
            (this.dataGridView1.Columns["Name"] as DataGridViewTextBoxColumn).DataPropertyName = "Name";
            (this.dataGridView1.Columns["CityName"] as DataGridViewTextBoxColumn).DataPropertyName = "HomeAddr_CityName";
            (this.dataGridView1.Columns["PostCode"] as DataGridViewTextBoxColumn).DataPropertyName = "HomeAddr_PostCode";
            
        }

        private void button1_Click(object sender, EventArgs e)
        {
            // add objects of type Person to a list            
            Person p = new Person();
            p.ID = "1";
            p.Name = "aa";

            Address addr = new Address();
            addr.CityName = "city  name1";
            addr.PostCode = "post code1";
            p.HomeAddr = addr;

            mylist.Add(p);

            p = new Person();
            p.ID = "2";
            p.Name = "bb";

            addr = new Address();
            addr.CityName = "city name2";
            addr.PostCode = "post code2";
            p.HomeAddr = addr;

            mylist.Add(p);
        }
    }
}