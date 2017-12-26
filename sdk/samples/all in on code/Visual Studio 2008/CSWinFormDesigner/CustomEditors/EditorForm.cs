/************************************* Module Header **************************************\
* Module Name:	EditorForm.cs
* Project:		CSWinFormDesigner
* Copyright (c) Microsoft Corporation.
* 
* 
* The CustomEditor sample demonstrates how to use a custom editor for a specific property 
* at design time. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 5/25/2009 3:00 PM Zhi-Xin Ye Created
* * 
* 
\******************************************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CSWinFormDesigner.CustomEditors
{
    public partial class EditorForm : Form
    {
        public EditorForm()
        {
            InitializeComponent();
            this.Load += new EventHandler(EditorForm_Load);
        }

        void EditorForm_Load(object sender, EventArgs e)
        {
            this.textBox1.Text = this.subCls.Name;
            this.monthCalendar1.SelectionStart = this.subCls.Date;
            this.monthCalendar1.SelectionEnd = this.subCls.Date;
        }
        private SubClass subCls = new SubClass();

        public SubClass SubCls
        {
            get
            {
                this.subCls.Name = this.textBox1.Text;
                this.subCls.Date = this.monthCalendar1.SelectionStart;
                return subCls;
            }
            set
            {
                this.subCls = value;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;

        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
        }
    }
}
