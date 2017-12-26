/************************************* Module Header **************************************\
* Module Name:  Form2.cs
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
    public partial class Form2 : Form
    {
        public Form2()
        {
            InitializeComponent();

            this.Load += new EventHandler(FrmPassValueBetweenForms2_Load);
        }

        void FrmPassValueBetweenForms2_Load(object sender, EventArgs e)
        {
            this.lbDisplay.Text = this._valueToPassBetweenForms;
        }

        #region Method 1 - Use A Property

        private string _valueToPassBetweenForms;

        public string ValueToPassBetweenForms
        {
            get { return this._valueToPassBetweenForms; }
            set { this._valueToPassBetweenForms = value; }
        }

        #endregion

        #region Method 2 - Use A Method

        public void SetValueFromAnotherForm(string val)
        {
            this._valueToPassBetweenForms = val;
        }

        #endregion
    }
}
