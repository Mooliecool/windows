using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace MaskedEditSample
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		private MaskEditSample.MaskedEditTextBox maskedEditTextBox1;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.ErrorProvider errorProvider1;
		private MaskEditSample.MaskedEditTextBox maskedEditTextBox2;
		private System.Windows.Forms.Label label2;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Form1()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.maskedEditTextBox1 = new MaskEditSample.MaskedEditTextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.button1 = new System.Windows.Forms.Button();
			this.errorProvider1 = new System.Windows.Forms.ErrorProvider();
			this.maskedEditTextBox2 = new MaskEditSample.MaskedEditTextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// maskedEditTextBox1
			// 
			this.maskedEditTextBox1.Expression = "((\\(\\d{3}\\) ?)|(\\d{3}-))?\\d{3}-\\d{4}";
			this.maskedEditTextBox1.KeyPressExpression = "";
			this.maskedEditTextBox1.Location = new System.Drawing.Point(136, 32);
			this.maskedEditTextBox1.Mode = MaskEditSample.MaskedEditTextBox.MaskMode.Leave;
			this.maskedEditTextBox1.Name = "maskedEditTextBox1";
			this.maskedEditTextBox1.Size = new System.Drawing.Size(144, 20);
			this.maskedEditTextBox1.TabIndex = 0;
			this.maskedEditTextBox1.Text = "";
			this.maskedEditTextBox1.TextInvalid += new System.EventHandler(this.ShowError);
			this.maskedEditTextBox1.TextChanged += new System.EventHandler(this.HideError);
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(8, 32);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(128, 23);
			this.label1.TabIndex = 2;
			this.label1.Text = "(Leave) Phone Number:";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(208, 128);
			this.button1.Name = "button1";
			this.button1.TabIndex = 4;
			this.button1.Text = "PlaceHolder";
			// 
			// errorProvider1
			// 
			this.errorProvider1.DataMember = null;
			// 
			// maskedEditTextBox2
			// 
			this.maskedEditTextBox2.Expression = "";
			this.maskedEditTextBox2.KeyPressExpression = "1?\\d?\\d?\\x08?";
			this.maskedEditTextBox2.Location = new System.Drawing.Point(136, 72);
			this.maskedEditTextBox2.Mode = MaskEditSample.MaskedEditTextBox.MaskMode.KeyPress;
			this.maskedEditTextBox2.Name = "maskedEditTextBox2";
			this.maskedEditTextBox2.Size = new System.Drawing.Size(40, 20);
			this.maskedEditTextBox2.TabIndex = 3;
			this.maskedEditTextBox2.Text = "";
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(8, 72);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(128, 23);
			this.label2.TabIndex = 5;
			this.label2.Text = "(KeyPress) Age:";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(352, 165);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.label2,
																		  this.maskedEditTextBox2,
																		  this.button1,
																		  this.label1,
																		  this.maskedEditTextBox1});
			this.Name = "Form1";
			this.Text = "Masked Edit TextBox Sample";
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

		private void maskedEditTextBox1_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
		{
			MessageBox.Show("Hi");
		}

		private void HideError(object sender, System.EventArgs e)
		{
			errorProvider1.SetError(maskedEditTextBox1,"");
		}

		private void ShowError(object sender, System.EventArgs e)
		{
			errorProvider1.SetError(maskedEditTextBox1, "The phone number is invalid!");
			maskedEditTextBox1.Focus();
			maskedEditTextBox1.SelectAll();
		}
	}
}
