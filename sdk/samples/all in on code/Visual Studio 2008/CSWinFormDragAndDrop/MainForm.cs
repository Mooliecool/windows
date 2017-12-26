/************************************* Module Header **************************************\
* Module Name:  MainForm.cs
* Project:      CSWinFormDragAndDrop
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to perform drag-and-drop operations in a 
* Windows Forms Application.
* 
* For more information about the drag-and-drop operations, see:
* 
*  Performing Drag-and-Drop Operations in Windows Forms
*  http://msdn.microsoft.com/en-us/library/aa984430(VS.71).aspx
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 3/20/2009 3:00 PM ZhiXin Ye Created
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


namespace CSWinFormDragAndDrop
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void frmDragAndDrop_Load(object sender, EventArgs e)
        {
            // Add items to the ListBox
            this.listBox1.Items.Add("item1");
            this.listBox1.Items.Add("item2");
            this.listBox1.Items.Add("item3");
            this.listBox1.Items.Add("item4");
            this.listBox1.Items.Add("item5");

            // Enable dropping on the TreeView  
            this.treeView1.AllowDrop = true;

            this.listBox1.MouseDown += new MouseEventHandler(listBox1_MouseDown);
            this.treeView1.DragEnter += new DragEventHandler(treeView1_DragEnter);
            this.treeView1.DragDrop += new DragEventHandler(treeView1_DragDrop);
        }

        void listBox1_MouseDown(object sender, MouseEventArgs e)
        {
            // In the MouseDown event for the ListBox where the drag will begin, 
            // use the DoDragDrop method to set the data to be dragged 
            // and the allowed effect dragging will have.
            if (this.listBox1.SelectedItem != null)
            {
                this.listBox1.DoDragDrop(this.listBox1.SelectedItem, DragDropEffects.Copy);
            }
        }

        void treeView1_DragEnter(object sender, DragEventArgs e)
        {
            // Sets the effect that will happen when the drop occurs to a value 
            // in the DragDropEffects enumeration. 
            e.Effect = DragDropEffects.Copy;
        }

        void treeView1_DragDrop(object sender, DragEventArgs e)
        {
            // In the DragDrop event for the TreeView where the drop will occur, 
            // use the GetData method to retrieve the data being dragged.
            string item = (string)e.Data.GetData(e.Data.GetFormats()[0]);

            // Add the item strib
            this.treeView1.Nodes.Add(item);
        }
    }
}
