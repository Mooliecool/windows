//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

namespace Microsoft.Samples.ResultSetSample
{
    public partial class ResultSetSample : System.Windows.Forms.Form
    {
        /// <summary>
        /// Main menu for the form.
        /// </summary>
        private System.Windows.Forms.MainMenu mainMenu1;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.mainMenu1 = new System.Windows.Forms.MainMenu();
            this.menuItemFile = new System.Windows.Forms.MenuItem();
            this.menuItemUpdatable = new System.Windows.Forms.MenuItem();
            this.menuItemUseDataSet = new System.Windows.Forms.MenuItem();
            this.menuItemClear = new System.Windows.Forms.MenuItem();
            this.menuItemExit = new System.Windows.Forms.MenuItem();
            this.tabPane = new System.Windows.Forms.TabControl();
            this.tabDataGrid = new System.Windows.Forms.TabPage();
            this.dataGrid = new System.Windows.Forms.DataGrid();
            this.tbCommand = new System.Windows.Forms.TextBox();
            this.tabDetails = new System.Windows.Forms.TabPage();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.btnNext = new System.Windows.Forms.Button();
            this.btnPrev = new System.Windows.Forms.Button();
            this.btnExecute = new System.Windows.Forms.Button();
            this.textBox4 = new System.Windows.Forms.TextBox();
            this.tabPane.SuspendLayout();
            this.tabDataGrid.SuspendLayout();
            this.tabDetails.SuspendLayout();
            this.SuspendLayout();
            // 
            // mainMenu1
            // 
            this.mainMenu1.MenuItems.Add(this.menuItemFile);
            // 
            // menuItemFile
            // 
            this.menuItemFile.MenuItems.Add(this.menuItemUpdatable);
            this.menuItemFile.MenuItems.Add(this.menuItemUseDataSet);
            this.menuItemFile.MenuItems.Add(this.menuItemClear);
            this.menuItemFile.MenuItems.Add(this.menuItemExit);
            this.menuItemFile.Text = "File";
            // 
            // menuItemUpdatable
            // 
            this.menuItemUpdatable.Text = "Updatable";
            this.menuItemUpdatable.Click += new System.EventHandler(this.menuItemUpdatable_Click);
            // 
            // menuItemUseDataSet
            // 
            this.menuItemUseDataSet.Text = "Use DataSet";
            this.menuItemUseDataSet.Click += new System.EventHandler(this.menuItemUseDataSet_Click);
            // 
            // menuItemClear
            // 
            this.menuItemClear.Text = "Clear";
            this.menuItemClear.Click += new System.EventHandler(this.menuItemClear_Click);
            // 
            // menuItemExit
            // 
            this.menuItemExit.Text = "Exit";
            this.menuItemExit.Click += new System.EventHandler(this.menuItemExit_Click);
            // 
            // tabPane
            // 
            this.tabPane.Controls.Add(this.tabDataGrid);
            this.tabPane.Controls.Add(this.tabDetails);
            this.tabPane.Location = new System.Drawing.Point(0, 0);
            this.tabPane.Name = "tabPane";
            this.tabPane.SelectedIndex = 0;
            this.tabPane.Size = new System.Drawing.Size(240, 242);
            this.tabPane.TabIndex = 0;
            // 
            // tabDataGrid
            // 
            this.tabDataGrid.AutoScroll = true;
            this.tabDataGrid.Controls.Add(this.dataGrid);
            this.tabDataGrid.Controls.Add(this.tbCommand);
            this.tabDataGrid.Location = new System.Drawing.Point(0, 0);
            this.tabDataGrid.Name = "tabDataGrid";
            this.tabDataGrid.Size = new System.Drawing.Size(240, 219);
            this.tabDataGrid.Text = "DataGrid";
            // 
            // dataGrid
            // 
            this.dataGrid.Location = new System.Drawing.Point(0, 23);
            this.dataGrid.Name = "dataGrid";
            this.dataGrid.Size = new System.Drawing.Size(240, 193);
            this.dataGrid.TabIndex = 0;
            // 
            // tbCommand
            // 
            this.tbCommand.Dock = System.Windows.Forms.DockStyle.Top;
            this.tbCommand.Location = new System.Drawing.Point(0, 0);
            this.tbCommand.Name = "tbCommand";
            this.tbCommand.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.tbCommand.Size = new System.Drawing.Size(240, 21);
            this.tbCommand.TabIndex = 0;
            this.tbCommand.TextChanged += new System.EventHandler(this.tbCommand_TextChanged);
            // 
            // tabDetails
            // 
            this.tabDetails.AutoScroll = true;
            this.tabDetails.Controls.Add(this.textBox4);
            this.tabDetails.Controls.Add(this.textBox3);
            this.tabDetails.Controls.Add(this.textBox2);
            this.tabDetails.Controls.Add(this.textBox1);
            this.tabDetails.Controls.Add(this.btnNext);
            this.tabDetails.Controls.Add(this.btnPrev);
            this.tabDetails.Location = new System.Drawing.Point(0, 0);
            this.tabDetails.Name = "tabDetails";
            this.tabDetails.Size = new System.Drawing.Size(240, 219);
            this.tabDetails.Text = "Details";
            // 
            // textBox3
            // 
            this.textBox3.Location = new System.Drawing.Point(41, 82);
            this.textBox3.Name = "textBox3";
            this.textBox3.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.textBox3.Size = new System.Drawing.Size(162, 21);
            this.textBox3.TabIndex = 4;
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(41, 47);
            this.textBox2.Name = "textBox2";
            this.textBox2.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.textBox2.Size = new System.Drawing.Size(162, 21);
            this.textBox2.TabIndex = 3;
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(41, 10);
            this.textBox1.Name = "textBox1";
            this.textBox1.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.textBox1.Size = new System.Drawing.Size(162, 21);
            this.textBox1.TabIndex = 2;
            // 
            // btnNext
            // 
            this.btnNext.Location = new System.Drawing.Point(131, 156);
            this.btnNext.Name = "btnNext";
            this.btnNext.TabIndex = 1;
            this.btnNext.Text = " Next >>";
            this.btnNext.Click += new System.EventHandler(this.btnNext_Click);
            // 
            // btnPrev
            // 
            this.btnPrev.Location = new System.Drawing.Point(41, 155);
            this.btnPrev.Name = "btnPrev";
            this.btnPrev.TabIndex = 0;
            this.btnPrev.Text = "<< Prev";
            this.btnPrev.Click += new System.EventHandler(this.btnPrev_Click);
            // 
            // btnExecute
            // 
            this.btnExecute.Location = new System.Drawing.Point(33, 248);
            this.btnExecute.Name = "btnExecute";
            this.btnExecute.Size = new System.Drawing.Size(162, 20);
            this.btnExecute.TabIndex = 1;
            this.btnExecute.Text = "Query and Bind!";
            this.btnExecute.Click += new System.EventHandler(this.btnExecute_Click);
            // 
            // textBox4
            // 
            this.textBox4.Location = new System.Drawing.Point(41, 118);
            this.textBox4.Name = "textBox4";
            this.textBox4.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.textBox4.Size = new System.Drawing.Size(162, 21);
            this.textBox4.TabIndex = 5;
            // 
            // ResultSetSample
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(240, 268);
            this.Controls.Add(this.btnExecute);
            this.Controls.Add(this.tabPane);
            this.Menu = this.mainMenu1;
            this.Name = "ResultSetSample";
            this.Text = "ResultSet";
            this.tabPane.ResumeLayout(false);
            this.tabDataGrid.ResumeLayout(false);
            this.tabDetails.ResumeLayout(false);
            this.ResumeLayout(false);

        }
        #endregion

        private System.Windows.Forms.TabControl tabPane;
        private System.Windows.Forms.TabPage tabDataGrid;
        private System.Windows.Forms.DataGrid dataGrid;
        private System.Windows.Forms.MenuItem menuItemFile;
        private System.Windows.Forms.MenuItem menuItemExit;
        private System.Windows.Forms.TabPage tabDetails;
        private System.Windows.Forms.Button btnNext;
        private System.Windows.Forms.Button btnPrev;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.MenuItem menuItemUseDataSet;
        private System.Windows.Forms.MenuItem menuItemUpdatable;
        private System.Windows.Forms.TextBox tbCommand;
        private System.Windows.Forms.MenuItem menuItemClear;
        private System.Windows.Forms.Button btnExecute;
        private System.Windows.Forms.TextBox textBox4;

    }
}

