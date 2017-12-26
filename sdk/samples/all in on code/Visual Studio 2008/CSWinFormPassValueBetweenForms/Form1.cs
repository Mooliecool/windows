/************************************* Module Header **************************************\
* Module Name:  Form1.cs
* Project:      CSWinFormPassValueBetweenForms
* Copyright (c) Microsoft Corporation.
* 
* This sample demonstrates how to pass value between forms.
*  
* There're two common ways to pass value between forms:
* 
* 1. Use a property.
*    Create a public property on the target form class, then we can pass value to the target 
*    form by setting value for the property.
* 
* 2. Use a method.
*    Create a public method on the target form class, then we can pass value to the target 
*    form by passing the value as parameter to the method.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 4/07/2009 3:00 PM Zhi-Xin Ye Created
\******************************************************************************************/

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


namespace CSWinFormPassValueBetweenForms
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            // Method 1 - Use A Property

            Form2 f2 = new Form2();
            f2.ValueToPassBetweenForms = this.textBox1.Text;
            f2.ShowDialog();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            // Method 2 - Use A Method

            Form2 f2 = new Form2();
            f2.SetValueFromAnotherForm(this.textBox1.Text);
            f2.ShowDialog();
        }
    }
}
