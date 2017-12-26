//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
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

namespace Microsoft.Samples.Workflow.OrderApplication
{
    partial class Mainform
    {
        private System.ComponentModel.IContainer components = null;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.buttonOrderCreated = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.textBoxOrderID = new System.Windows.Forms.TextBox();
            this.listViewOrders = new System.Windows.Forms.ListView();
            this.columnWorkflowInstanceID = new System.Windows.Forms.ColumnHeader();
            this.columnOrderID = new System.Windows.Forms.ColumnHeader();
            this.columnOrderState = new System.Windows.Forms.ColumnHeader();
            this.columnWorkflowStatus = new System.Windows.Forms.ColumnHeader();
            this.contextMenuOrdersList = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addOnHoldStateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.buttonOrderShipped = new System.Windows.Forms.Button();
            this.buttonOrderUpdated = new System.Windows.Forms.Button();
            this.buttonOrderCanceled = new System.Windows.Forms.Button();
            this.buttonOrderProcessed = new System.Windows.Forms.Button();
            this.comboBoxWorkflowStates = new System.Windows.Forms.ComboBox();
            this.buttonSetState = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.contextMenuOrdersList.SuspendLayout();
            this.SuspendLayout();
            // 
            // buttonOrderCreated
            // 
            this.buttonOrderCreated.Location = new System.Drawing.Point(140, 32);
            this.buttonOrderCreated.Name = "buttonOrderCreated";
            this.buttonOrderCreated.Size = new System.Drawing.Size(99, 23);
            this.buttonOrderCreated.TabIndex = 0;
            this.buttonOrderCreated.Text = "Order Created";
            this.buttonOrderCreated.Click += new System.EventHandler(this.ButtonOrderCreated_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(11, 19);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(44, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "OrderID";
            // 
            // textBoxOrderID
            // 
            this.textBoxOrderID.Location = new System.Drawing.Point(11, 34);
            this.textBoxOrderID.Name = "textBoxOrderID";
            this.textBoxOrderID.Size = new System.Drawing.Size(122, 20);
            this.textBoxOrderID.TabIndex = 3;
            // 
            // listViewOrders
            // 
            this.listViewOrders.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnWorkflowInstanceID,
            this.columnOrderID,
            this.columnOrderState,
            this.columnWorkflowStatus});
            this.listViewOrders.ContextMenuStrip = this.contextMenuOrdersList;
            this.listViewOrders.FullRowSelect = true;
            this.listViewOrders.GridLines = true;
            this.listViewOrders.Location = new System.Drawing.Point(2, 72);
            this.listViewOrders.MultiSelect = false;
            this.listViewOrders.Name = "listViewOrders";
            this.listViewOrders.Size = new System.Drawing.Size(537, 109);
            this.listViewOrders.TabIndex = 4;
            this.listViewOrders.UseCompatibleStateImageBehavior = false;
            this.listViewOrders.View = System.Windows.Forms.View.Details;
            this.listViewOrders.ItemSelectionChanged += new System.Windows.Forms.ListViewItemSelectionChangedEventHandler(this.ListViewOrders_ItemSelectionChanged);
            // 
            // columnWorkflowInstanceID
            // 
            this.columnWorkflowInstanceID.Name = "columnWorkflowInstanceID";
            this.columnWorkflowInstanceID.Text = "Workflow InstanceID";
            this.columnWorkflowInstanceID.Width = 186;
            // 
            // columnOrderID
            // 
            this.columnOrderID.Name = "columnOrderID";
            this.columnOrderID.Text = "OrderID";
            this.columnOrderID.Width = 100;
            // 
            // columnOrderState
            // 
            this.columnOrderState.Name = "columnOrderState";
            this.columnOrderState.Text = "Order State";
            this.columnOrderState.Width = 139;
            // 
            // columnWorkflowStatus
            // 
            this.columnWorkflowStatus.Name = "columnWorkflowStatus";
            this.columnWorkflowStatus.Text = "Workflow Status";
            this.columnWorkflowStatus.Width = 106;
            // 
            // contextMenuOrdersList
            // 
            this.contextMenuOrdersList.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addOnHoldStateToolStripMenuItem});
            this.contextMenuOrdersList.Name = "contextMenuOrdersList";
            this.contextMenuOrdersList.Size = new System.Drawing.Size(164, 26);
            this.contextMenuOrdersList.ItemClicked += new System.Windows.Forms.ToolStripItemClickedEventHandler(this.ContextMenuOrdersList_ItemClicked);
            // 
            // addOnHoldStateToolStripMenuItem
            // 
            this.addOnHoldStateToolStripMenuItem.Name = "addOnHoldStateToolStripMenuItem";
            this.addOnHoldStateToolStripMenuItem.Size = new System.Drawing.Size(163, 22);
            this.addOnHoldStateToolStripMenuItem.Text = "Add On Hold State";
            // 
            // buttonOrderShipped
            // 
            this.buttonOrderShipped.Enabled = false;
            this.buttonOrderShipped.Location = new System.Drawing.Point(439, 189);
            this.buttonOrderShipped.Name = "buttonOrderShipped";
            this.buttonOrderShipped.Size = new System.Drawing.Size(99, 23);
            this.buttonOrderShipped.TabIndex = 5;
            this.buttonOrderShipped.Text = "Order Shipped";
            this.buttonOrderShipped.Click += new System.EventHandler(this.ButtonOrderEvent_Click);
            // 
            // buttonOrderUpdated
            // 
            this.buttonOrderUpdated.Enabled = false;
            this.buttonOrderUpdated.Location = new System.Drawing.Point(88, 189);
            this.buttonOrderUpdated.Name = "buttonOrderUpdated";
            this.buttonOrderUpdated.Size = new System.Drawing.Size(99, 23);
            this.buttonOrderUpdated.TabIndex = 7;
            this.buttonOrderUpdated.Text = "Order Updated";
            this.buttonOrderUpdated.Click += new System.EventHandler(this.ButtonOrderEvent_Click);
            // 
            // buttonOrderCanceled
            // 
            this.buttonOrderCanceled.Enabled = false;
            this.buttonOrderCanceled.Location = new System.Drawing.Point(205, 189);
            this.buttonOrderCanceled.Name = "buttonOrderCanceled";
            this.buttonOrderCanceled.Size = new System.Drawing.Size(99, 23);
            this.buttonOrderCanceled.TabIndex = 8;
            this.buttonOrderCanceled.Text = "Order Canceled";
            this.buttonOrderCanceled.Click += new System.EventHandler(this.ButtonOrderEvent_Click);
            // 
            // buttonOrderProcessed
            // 
            this.buttonOrderProcessed.Enabled = false;
            this.buttonOrderProcessed.Location = new System.Drawing.Point(322, 189);
            this.buttonOrderProcessed.Name = "buttonOrderProcessed";
            this.buttonOrderProcessed.Size = new System.Drawing.Size(99, 23);
            this.buttonOrderProcessed.TabIndex = 9;
            this.buttonOrderProcessed.Text = "Order Processed";
            this.buttonOrderProcessed.Click += new System.EventHandler(this.ButtonOrderEvent_Click);
            // 
            // comboBoxWorkflowStates
            // 
            this.comboBoxWorkflowStates.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxWorkflowStates.FormattingEnabled = true;
            this.comboBoxWorkflowStates.Location = new System.Drawing.Point(291, 32);
            this.comboBoxWorkflowStates.Name = "comboBoxWorkflowStates";
            this.comboBoxWorkflowStates.Size = new System.Drawing.Size(152, 21);
            this.comboBoxWorkflowStates.TabIndex = 10;
            // 
            // buttonSetState
            // 
            this.buttonSetState.Location = new System.Drawing.Point(449, 32);
            this.buttonSetState.Name = "buttonSetState";
            this.buttonSetState.Size = new System.Drawing.Size(36, 23);
            this.buttonSetState.TabIndex = 11;
            this.buttonSetState.Text = "&Go";
            this.buttonSetState.Click += new System.EventHandler(this.ButtonSetState_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(288, 16);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(54, 13);
            this.label2.TabIndex = 12;
            this.label2.Text = "Set State:";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(543, 223);
            this.Controls.Add(this.buttonSetState);
            this.Controls.Add(this.comboBoxWorkflowStates);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.buttonOrderProcessed);
            this.Controls.Add(this.buttonOrderCanceled);
            this.Controls.Add(this.buttonOrderUpdated);
            this.Controls.Add(this.buttonOrderShipped);
            this.Controls.Add(this.listViewOrders);
            this.Controls.Add(this.textBoxOrderID);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.buttonOrderCreated);
            this.Name = "Form1";
            this.Text = "Order Application - State Machine Example";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Mainform_FormClosing);
            this.Load += new System.EventHandler(this.Mainform_Load);
            this.contextMenuOrdersList.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBoxOrderID;
        private System.Windows.Forms.ListView listViewOrders;
        private System.Windows.Forms.ColumnHeader columnOrderID;
        private System.Windows.Forms.ColumnHeader columnWorkflowInstanceID;
        private System.Windows.Forms.Button buttonOrderShipped;
        private System.Windows.Forms.Button buttonOrderUpdated;
        private System.Windows.Forms.Button buttonOrderCanceled;
        private System.Windows.Forms.Button buttonOrderProcessed;
        private System.Windows.Forms.ColumnHeader columnOrderState;
        private System.Windows.Forms.Button buttonOrderCreated;
        private System.Windows.Forms.ColumnHeader columnWorkflowStatus;
        private System.Windows.Forms.Button buttonSetState;
        private System.Windows.Forms.ComboBox comboBoxWorkflowStates;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ContextMenuStrip contextMenuOrdersList;
        private System.Windows.Forms.ToolStripMenuItem addOnHoldStateToolStripMenuItem;
    }
}

