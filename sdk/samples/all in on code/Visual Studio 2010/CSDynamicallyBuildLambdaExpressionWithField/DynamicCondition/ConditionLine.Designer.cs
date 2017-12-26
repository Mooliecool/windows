//INSTANT C# NOTE: Formerly VB project-level imports:
using System;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.Diagnostics;
using System.Windows.Forms;
using System.Linq;
using System.Xml.Linq;

using System.Reflection;
using DynamicCondition;
//using DynamicCondition.DynamicQuery.Condition;

namespace DynamicCondition
{
	[Microsoft.VisualBasic.CompilerServices.DesignerGenerated()]
	internal partial class ConditionLine : System.Windows.Forms.UserControl
	{

		//UserControl overrides dispose to clean up the component list.
		[System.Diagnostics.DebuggerNonUserCode()]
		protected override void Dispose(bool disposing)
		{
			try
			{
				if (disposing && components != null)
				{
					components.Dispose();
				}
			}
			finally
			{
				base.Dispose(disposing);
			}
		}

		//Required by the Windows Form Designer
		private System.ComponentModel.IContainer components;

		//NOTE: The following procedure is required by the Windows Form Designer
		//It can be modified using the Windows Form Designer.  
		//Do not modify it using the code editor.
		[System.Diagnostics.DebuggerStepThrough()]
		private void InitializeComponent()
		{
            this.Panel1 = new System.Windows.Forms.Panel();
            this.lb = new System.Windows.Forms.Label();
            this.cmbColumn = new System.Windows.Forms.ComboBox();
            this.cmbOperator = new System.Windows.Forms.ComboBox();
            this.pnlValue = new System.Windows.Forms.Panel();
            this.tbValue = new System.Windows.Forms.TextBox();
            this.chkValue = new System.Windows.Forms.CheckBox();
            this.dtpValue = new System.Windows.Forms.DateTimePicker();
            this.Panel1.SuspendLayout();
            this.pnlValue.SuspendLayout();
            this.SuspendLayout();
            // 
            // Panel1
            // 
            this.Panel1.Controls.Add(this.lb);
            this.Panel1.Controls.Add(this.cmbColumn);
            this.Panel1.Controls.Add(this.cmbOperator);
            this.Panel1.Location = new System.Drawing.Point(3, 3);
            this.Panel1.Name = "Panel1";
            this.Panel1.Size = new System.Drawing.Size(242, 29);
            this.Panel1.TabIndex = 6;
            // 
            // lb
            // 
            this.lb.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lb.ForeColor = System.Drawing.Color.Blue;
            this.lb.Location = new System.Drawing.Point(3, 3);
            this.lb.Name = "lb";
            this.lb.Size = new System.Drawing.Size(40, 20);
            this.lb.TabIndex = 6;
            this.lb.Text = "AND";
            this.lb.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.lb.Click += new System.EventHandler(this.lblOperator_Click);
            // 
            // cmbColumn
            // 
            this.cmbColumn.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Suggest;
            this.cmbColumn.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems;
            this.cmbColumn.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbColumn.FormattingEnabled = true;
            this.cmbColumn.Location = new System.Drawing.Point(49, 3);
            this.cmbColumn.Name = "cmbColumn";
            this.cmbColumn.Size = new System.Drawing.Size(100, 21);
            this.cmbColumn.TabIndex = 0;
            this.cmbColumn.SelectedIndexChanged += new System.EventHandler(this.cboColumn_SelectedIndexChanged);
            // 
            // cmbOperator
            // 
            this.cmbOperator.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbOperator.FormattingEnabled = true;
            this.cmbOperator.Location = new System.Drawing.Point(155, 3);
            this.cmbOperator.Name = "cmbOperator";
            this.cmbOperator.Size = new System.Drawing.Size(78, 21);
            this.cmbOperator.TabIndex = 1;
            // 
            // pnlValue
            // 
            this.pnlValue.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pnlValue.Controls.Add(this.tbValue);
            this.pnlValue.Controls.Add(this.chkValue);
            this.pnlValue.Controls.Add(this.dtpValue);
            this.pnlValue.Location = new System.Drawing.Point(251, 3);
            this.pnlValue.Name = "pnlValue";
            this.pnlValue.Size = new System.Drawing.Size(206, 29);
            this.pnlValue.TabIndex = 7;
            // 
            // tbValue
            // 
            this.tbValue.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tbValue.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbValue.Location = new System.Drawing.Point(3, 3);
            this.tbValue.Name = "tbValue";
            this.tbValue.Size = new System.Drawing.Size(203, 20);
            this.tbValue.TabIndex = 4;
            // 
            // chkValue
            // 
            this.chkValue.AutoSize = true;
            this.chkValue.Location = new System.Drawing.Point(3, 3);
            this.chkValue.Name = "chkValue";
            this.chkValue.Size = new System.Drawing.Size(15, 14);
            this.chkValue.TabIndex = 2;
            this.chkValue.UseVisualStyleBackColor = true;
            // 
            // dtpValue
            // 
            this.dtpValue.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.dtpValue.Location = new System.Drawing.Point(3, 3);
            this.dtpValue.Name = "dtpValue";
            this.dtpValue.Size = new System.Drawing.Size(203, 20);
            this.dtpValue.TabIndex = 3;
            this.dtpValue.Visible = false;
            // 
            // ConditionLine
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.Panel1);
            this.Controls.Add(this.pnlValue);
            this.Name = "ConditionLine";
            this.Size = new System.Drawing.Size(460, 35);
            this.Load += new System.EventHandler(this.ConditionLine_Load);
            this.Panel1.ResumeLayout(false);
            this.pnlValue.ResumeLayout(false);
            this.pnlValue.PerformLayout();
            this.ResumeLayout(false);

		}
		internal System.Windows.Forms.Panel Panel1;
		internal System.Windows.Forms.Label lb;
		internal System.Windows.Forms.ComboBox cmbColumn;
		internal System.Windows.Forms.ComboBox cmbOperator;
		internal System.Windows.Forms.Panel pnlValue;
		internal System.Windows.Forms.TextBox tbValue;
		internal System.Windows.Forms.CheckBox chkValue;
		internal System.Windows.Forms.DateTimePicker dtpValue;

	}

}