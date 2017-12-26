/************************************* Module Header **************************************\
* Module Name:  MainForm.cs
* Project:      CSWinFormControls
* Copyright (c) Microsoft Corporation.
* 
* The Control Customization sample demonstrates how to customize the Windows Forms controls.
* 
* In this sample, there're 4 examples:
* 
* 1. Multiple Column ComboBox.
*    Demonstrates how to display multiple columns of data in the dropdown of a ComboBox.
* 2. ListBox Items With Different ToolTips.
*    Demonstrates how to display different tooltips on each items of the ListBox.
* 3. Numeric-only TextBox.
*    Demonstrates how to make a TextBox only accepts numbers.
* 4. A Round Button.
*    Demonstrates how to create a Button with irregular shape.
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
using System.Drawing.Drawing2D;
#endregion


namespace CSWinFormControls
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void frmCtrlCustomization_Load(object sender, EventArgs e)
        {
            #region  Example 1 -- Multiple Column ComboBox

            //  DataSource setup:
            //  
            //  Create a Table named Test and add 2 columns
            //   ID:     int
            //   Name:   string
            //
            DataTable dtTest = new DataTable();
            dtTest.Columns.Add("ID", typeof(int));
            dtTest.Columns.Add("Name", typeof(string));

            dtTest.Rows.Add(1, "John");
            dtTest.Rows.Add(2, "Amy");
            dtTest.Rows.Add(3, "Tony");
            dtTest.Rows.Add(4, "Bruce");
            dtTest.Rows.Add(5, "Allen");

            // Bind the ComboBox to the DataTable
            this.comboBox1.DataSource = dtTest;
            this.comboBox1.DisplayMember = "Name";
            this.comboBox1.ValueMember = "ID";

            // Enable the owner draw on the ComboBox.
            this.comboBox1.DrawMode = DrawMode.OwnerDrawFixed;
            // Handle the DrawItem event to draw the items.
            this.comboBox1.DrawItem += delegate(object cmb, DrawItemEventArgs args)
            {
                // Draw the default background
                args.DrawBackground();


                // The ComboBox is bound to a DataTable,
                // so the items are DataRowView objects.
                DataRowView drv = (DataRowView)this.comboBox1.Items[args.Index];

                // Retrieve the value of each column.
                string id = drv["id"].ToString();
                string name = drv["name"].ToString();

                // Get the bounds for the first column
                Rectangle r1 = args.Bounds;
                r1.Width /= 2;

                // Draw the text on the first column
                using (SolidBrush sb = new SolidBrush(args.ForeColor))
                {
                    args.Graphics.DrawString(id, args.Font, sb, r1);
                }

                // Draw a line to isolate the columns 
                using (Pen p = new Pen(Color.Black))
                {
                    args.Graphics.DrawLine(p, r1.Right, 0, r1.Right, r1.Bottom);
                }

                // Get the bounds for the second column
                Rectangle r2 = args.Bounds;
                r2.X = args.Bounds.Width/2;
                r2.Width /= 2;

                // Draw the text on the second column
                using (SolidBrush sb = new SolidBrush(args.ForeColor))
                {
                    args.Graphics.DrawString(name, args.Font, sb, r2);
                }
            };

            #endregion
                
            #region Example 2 -- ListBox Items With Different ToolTips

            // Setup the ListBox items
            this.listBox1.Items.Add("Item1");
            this.listBox1.Items.Add("Item2");
            this.listBox1.Items.Add("Item3");
            this.listBox1.Items.Add("Item4");
            this.listBox1.Items.Add("Item5");

            this.listBox1.MouseMove += delegate(object lst, MouseEventArgs args)
            {
                // Retrieve the item index at where the mouse hovers
                int hoverIndex = this.listBox1.IndexFromPoint(args.Location);

                // If the mouse is over the items, display a tooltip
                if (hoverIndex >= 0 && hoverIndex < listBox1.Items.Count)
                {
                    this.toolTip1.SetToolTip(listBox1, listBox1.Items[hoverIndex].ToString());
                }
            };

            #endregion

            #region Example 3 -- Numeric-only TextBox

            // Handle the TextBox.KeyPress event to filter the input characters.
            this.textBox1.KeyPress += delegate(object tb, KeyPressEventArgs args)
            {
                if (!(char.IsNumber(args.KeyChar) || args.KeyChar == '\b'))
                {
                    // If the input character is not number or Backspace key
                    // Then set the Handled property to true to indicate that 
                    // the KeyPress event is handled, so that the TextBox just 
                    // bypass the input character.
                    args.Handled = true;
                }
            };


            #endregion

            #region Example 4 -- A Round Button

            this.roundButton1.Click += delegate(object btn, EventArgs args)
            {
                MessageBox.Show("Clicked!");
            };
            #endregion
        }
    }

    #region RoundButton Class

    public class RoundButton : Button
    {
        protected override void OnPaint(PaintEventArgs pevent)
        {
            // Change the region for the button so that when clicks outside the ellipse bounds,
            // the Click event won't fire.
            GraphicsPath path = new GraphicsPath();
            path.AddEllipse(0, 0, ClientSize.Width, ClientSize.Height);
            this.Region = new Region(path);

            base.OnPaint(pevent);
        }
    }

    #endregion
}
