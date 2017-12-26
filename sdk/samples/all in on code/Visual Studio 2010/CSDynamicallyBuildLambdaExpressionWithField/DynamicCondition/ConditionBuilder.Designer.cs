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

using DynamicCondition;
using System.ComponentModel;

namespace DynamicCondition
{
	[Microsoft.VisualBasic.CompilerServices.DesignerGenerated()]
	public partial class ConditionBuilder : System.Windows.Forms.UserControl
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
			this.ConditionLine1 = new DynamicCondition.ConditionLine();
			this.SuspendLayout();
			//
			//ConditionLine1
			//
			this.ConditionLine1.Anchor = (System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) | System.Windows.Forms.AnchorStyles.Right));
			this.ConditionLine1.AutoScroll = true;
			this.ConditionLine1.DataSource = null;
			this.ConditionLine1.DataType = null;
			this.ConditionLine1.Location = new System.Drawing.Point(3, 3);
			this.ConditionLine1.Name = "ConditionLine1";
			this.ConditionLine1.OperatorType = DynamicCondition.DynamicQuery.Condition.Compare.And;
			this.ConditionLine1.Size = new System.Drawing.Size(456, 32);
			this.ConditionLine1.TabIndex = 0;
			//
			//ConditionBuilder
			//
			this.AutoScaleDimensions = new System.Drawing.SizeF(6.0F, 13.0F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoScroll = true;
			this.Controls.Add(this.ConditionLine1);
			this.Name = "ConditionBuilder";
			this.Size = new System.Drawing.Size(462, 37);
			this.ResumeLayout(false);

//INSTANT C# NOTE: Converted event handler wireups:
			base.Load += new System.EventHandler(ConditionBuilder_Load);

		}
		internal ConditionLine ConditionLine1;

	}

}