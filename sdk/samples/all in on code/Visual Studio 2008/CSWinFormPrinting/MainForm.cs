/************************************* Module Header **************************************\
* Module Name:  MainForm.cs
* Project:      CSWinFormPrinting
* Copyright (c) Microsoft Corporation.
* 
* The Printing sample demonstrates how to do standard print job in Windows Forms Application.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 3/25/2009 3:00 PM Zhi-Xin Ye Created
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


namespace CSWinFormPrinting
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void frmPrinting_Load(object sender, EventArgs e)
        {
            // The example assumes your form has a Button control, 
            // a PrintDocument component named myDocument, 
            // and a PrintPreviewDialog control. 

            // Handle the PrintPage event to write the print logic.
            this.printDocument1.PrintPage += 
                new System.Drawing.Printing.PrintPageEventHandler(printDocument1_PrintPage);

            // Specify a PrintDocument instance for the PrintPreviewDialog component.
            this.printPreviewDialog1.Document = this.printDocument1;
        }

        void printDocument1_PrintPage(object sender, System.Drawing.Printing.PrintPageEventArgs e)
        {
            // Specify what to print and how to print in this event handler.
            // The follow code specify a string and a rectangle to be print 

            using (Font f = new Font("Vanada", 12))
            using (SolidBrush br = new SolidBrush(Color.Black))
            using (Pen p = new Pen(Color.Black)) 
            {
                e.Graphics.DrawString("This is a text.", f, br, 50, 50);

                e.Graphics.DrawRectangle(p, 50, 100, 300, 150);
            }
        }

        private void btnPrint_Click(object sender, EventArgs e)
        {
            this.printPreviewDialog1.ShowDialog();
        }
    }
}
