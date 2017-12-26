namespace CSVSXProjectSubType.ProjectFlavor
{
	partial class CustomPropertyPageView
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.lbStringProperty = new System.Windows.Forms.Label();
            this.tbStringProperty = new System.Windows.Forms.TextBox();
            this.chkBooleanProperty = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // lbStringProperty
            // 
            this.lbStringProperty.AutoSize = true;
            this.lbStringProperty.Location = new System.Drawing.Point(21, 25);
            this.lbStringProperty.Name = "lbStringProperty";
            this.lbStringProperty.Size = new System.Drawing.Size(76, 13);
            this.lbStringProperty.TabIndex = 0;
            this.lbStringProperty.Text = "String Property";
            // 
            // tbStringProperty
            // 
            this.tbStringProperty.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tbStringProperty.Location = new System.Drawing.Point(115, 23);
            this.tbStringProperty.Name = "tbStringProperty";
            this.tbStringProperty.Size = new System.Drawing.Size(326, 20);
            this.tbStringProperty.TabIndex = 1;
            // 
            // chkBooleanProperty
            // 
            this.chkBooleanProperty.AutoSize = true;
            this.chkBooleanProperty.Checked = true;
            this.chkBooleanProperty.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkBooleanProperty.Location = new System.Drawing.Point(21, 63);
            this.chkBooleanProperty.Name = "chkBooleanProperty";
            this.chkBooleanProperty.Size = new System.Drawing.Size(107, 17);
            this.chkBooleanProperty.TabIndex = 2;
            this.chkBooleanProperty.Text = "Boolean Property";
            // 
            // CustomPropertyPageView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.chkBooleanProperty);
            this.Controls.Add(this.tbStringProperty);
            this.Controls.Add(this.lbStringProperty);
            this.Name = "CustomPropertyPageView";
            this.Size = new System.Drawing.Size(445, 232);
            this.ResumeLayout(false);
            this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label lbStringProperty;
		private System.Windows.Forms.TextBox tbStringProperty;
		private System.Windows.Forms.CheckBox chkBooleanProperty;
	}
}