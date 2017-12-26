using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace DragDrop
{
	/// <summary>
	/// This form demonstrates the use of DragDrop events in Windows Controls.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		private System.Windows.Forms.ListBox listBox1;
		private System.Windows.Forms.TextBox textBox1;
		private System.Windows.Forms.NumericUpDown numericUpDown1;
		private System.Windows.Forms.DateTimePicker dateTimePicker1;
		private System.Windows.Forms.RichTextBox richTextBox1;
		private System.Windows.Forms.ContextMenu contextMenu1;
		private System.Windows.Forms.MenuItem mnuCopy;
		private System.Windows.Forms.MenuItem mnuMove;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		/// <summary>
		/// Used to track the source control from which data is being dragged.
		/// This allows the DragEnter and DragDrop event handlers to determine
		/// which control on the form is the originator of the DragDrop operation.
		/// </summary>
		private Control sourceControl;

		/// <summary>
		/// Used to track the mouse button that initiated the DragDrop operation.
		/// The mouse button is recorded in the MouseDown event of the source controls
		/// and is then accessible to the DragEnter and DragDrop event handlers of the 
		/// target control.
		/// </summary>
		private MouseButtons mouseButton;


		/// <summary>
		///  Used to store the data from the DragDrop event in a string format
		///  for use in the mnuCopy and mnuMove click event handlers.
		/// </summary>
		private string sourceData;

		/// <summary>
		/// Used to determine if the data in the source control should be deleted
		/// after being copied into the target control.  Deletion of source data
		/// equates to a Move, rather than a Copy.
		/// </summary>
		private bool deleteSource = false;

		/// <summary>
		/// This constant is used when checking for the CTRL key during drop operations.
		/// This makes the code more readable for future maintenance.
		/// </summary>
		private const int ctrlKey = 8;
	
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
			this.listBox1 = new System.Windows.Forms.ListBox();
			this.contextMenu1 = new System.Windows.Forms.ContextMenu();
			this.mnuCopy = new System.Windows.Forms.MenuItem();
			this.mnuMove = new System.Windows.Forms.MenuItem();
			this.textBox1 = new System.Windows.Forms.TextBox();
			this.numericUpDown1 = new System.Windows.Forms.NumericUpDown();
			this.dateTimePicker1 = new System.Windows.Forms.DateTimePicker();
			this.richTextBox1 = new System.Windows.Forms.RichTextBox();
			((System.ComponentModel.ISupportInitialize)(this.numericUpDown1)).BeginInit();
			this.SuspendLayout();
			// 
			// listBox1
			// 
			this.listBox1.AllowDrop = true;
			this.listBox1.Location = new System.Drawing.Point(216, 16);
			this.listBox1.Name = "listBox1";
			this.listBox1.Size = new System.Drawing.Size(168, 251);
			this.listBox1.TabIndex = 0;
			this.listBox1.DragDrop += new System.Windows.Forms.DragEventHandler(this.listBox1_DragDrop);
			this.listBox1.DragEnter += new System.Windows.Forms.DragEventHandler(this.listBox1_DragEnter);
			// 
			// contextMenu1
			// 
			this.contextMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																						 this.mnuCopy,
																						 this.mnuMove});
			// 
			// mnuCopy
			// 
			this.mnuCopy.Index = 0;
			this.mnuCopy.Text = "Copy";
			this.mnuCopy.Click += new System.EventHandler(this.mnuCopy_Click);
			// 
			// mnuMove
			// 
			this.mnuMove.Index = 1;
			this.mnuMove.Text = "Move";
			this.mnuMove.Click += new System.EventHandler(this.mnuMove_Click);
			// 
			// textBox1
			// 
			this.textBox1.Location = new System.Drawing.Point(8, 16);
			this.textBox1.Name = "textBox1";
			this.textBox1.Size = new System.Drawing.Size(192, 20);
			this.textBox1.TabIndex = 1;
			this.textBox1.Text = "This is a test message";
			this.textBox1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.textBox1_MouseDown);
			this.textBox1.TextChanged += new System.EventHandler(this.textBox1_TextChanged);
			// 
			// numericUpDown1
			// 
			this.numericUpDown1.Location = new System.Drawing.Point(8, 48);
			this.numericUpDown1.Maximum = new System.Decimal(new int[] {
																		   1000000,
																		   0,
																		   0,
																		   0});
			this.numericUpDown1.Name = "numericUpDown1";
			this.numericUpDown1.Size = new System.Drawing.Size(192, 20);
			this.numericUpDown1.TabIndex = 2;
			this.numericUpDown1.Value = new System.Decimal(new int[] {
																		 123456,
																		 0,
																		 0,
																		 0});
			this.numericUpDown1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.numericUpDown1_MouseDown);
			// 
			// dateTimePicker1
			// 
			this.dateTimePicker1.Location = new System.Drawing.Point(8, 80);
			this.dateTimePicker1.Name = "dateTimePicker1";
			this.dateTimePicker1.Size = new System.Drawing.Size(192, 20);
			this.dateTimePicker1.TabIndex = 3;
			this.dateTimePicker1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.dateTimePicker1_MouseDown);
			// 
			// richTextBox1
			// 
			this.richTextBox1.Location = new System.Drawing.Point(8, 120);
			this.richTextBox1.Name = "richTextBox1";
			this.richTextBox1.Size = new System.Drawing.Size(192, 144);
			this.richTextBox1.TabIndex = 4;
			this.richTextBox1.Text = "";
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(400, 292);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.richTextBox1,
																		  this.dateTimePicker1,
																		  this.numericUpDown1,
																		  this.textBox1,
																		  this.listBox1});
			this.Name = "Form1";
			this.Text = "DragDrop Sample Application";
			((System.ComponentModel.ISupportInitialize)(this.numericUpDown1)).EndInit();
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

		/// <summary>
		/// The MouseDown event of the source control is used to initiate a drag operation.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">A MouseEventArgs that contains the event data.</param>
		private void textBox1_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			// Sets a reference to the control that initiated the DragDrop operation, so 
			// that the target control can implement logic to handle data dragged from
			// specific controls differently.  This control reference is also used in the 
			// case of a Move effect to remove the data from the source control after a drop.
			sourceControl = textBox1;
			// Record the mouse button that initiated this operation in order to allow
			// target controls to respond differently to drags with the right or left
			// mouse buttons.
			mouseButton  = e.Button;
			// This initiates a DragDrop operation, specifying that the data to be dragged
			// with be the text stored in the textBox1 control.  This also specifies
			// that both Copy and Move effects will be allowed.
			textBox1.DoDragDrop(textBox1.Text, DragDropEffects.Move | DragDropEffects.Copy);
		}

		/// <summary>
		/// The MouseDown event of the source control is used to initiate a drag operation.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">A MouseEventArgs that contains the event data.</param>
		private void numericUpDown1_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			// Sets a reference to the control that initiated the DragDrop operation, so 
			// that the target control can implement logic to handle data dragged from
			// specific controls differently.
			sourceControl = numericUpDown1;
			// Record the mouse button that initiated this operation in order to allow
			// target controls to respond differently to drags with the right or left
			// mouse buttons.
			mouseButton  = e.Button;
			// This initiates a DragDrop operation, specifying that the data to be dragged
			// with be the value stored in the numericUpDown1 control.  This also specifies
			// that ONLY the Copy effect will be allowed.
			numericUpDown1.DoDragDrop(numericUpDown1.Value.ToString(), DragDropEffects.Copy);
		}

		/// <summary>
		/// The MouseDown event of the source control is used to initiate a drag operation.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">A MouseEventArgs that contains the event data.</param>
		private void dateTimePicker1_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			// Sets a reference to the control that initiated the DragDrop operation, so 
			// that the target control can implement logic to handle data dragged from
			// specific controls differently.
			sourceControl = dateTimePicker1;
			// Record the mouse button that initiated this operation in order to allow
			// target controls to respond differently to drags with the right or left
			// mouse buttons.
			mouseButton  = e.Button;
			// This initiates a DragDrop operation, specifying that the data to be dragged
			// with be the value stored in the dateTimePicker1 control.  This also specifies
			// that ONLY the Copy effect will be allowed.
			dateTimePicker1.DoDragDrop(dateTimePicker1.Value.ToString(), DragDropEffects.Copy);
		}

		/// <summary>
		/// The DragEnter event of the target control fires when the mouse enters
		/// a target control during a drag operation, and is used to determine if a drop
		/// will be allowed over this control.  This generally involves checking the type 
		/// of data being dragged, the type of effects allowed (copy, move, etc.), 
		/// and potentially the type and/or the specific instance of the source control that 
		/// initiated the drag operation.
		/// 
		/// This event will fire only if the AllowDrop property of the target control has
		/// been set to true.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">A DragEventArgs that contains the event data.</param>
		private void listBox1_DragEnter(object sender, System.Windows.Forms.DragEventArgs e)
		{
			// Display some information about the DragDrop information in the
			// richTextBox1 control to show some of the information available.
			richTextBox1.Text = "Source Control: " + sourceControl.Name + 
				"\r\nSource Control Type: " + sourceControl.GetType().Name + 
				"\r\nAllowed Effect: " + e.AllowedEffect + 
				"\r\nMouse Button: " + mouseButton.ToString() + "\r\n" +
				"\r\nAvailable Formats:\r\n";

			// Data may be available in more than one format, so loop through
			// all available formats and display them in richTextBox1.
			foreach (string availableFormat in e.Data.GetFormats(true))
			{
				richTextBox1.Text += "\t" + availableFormat + "\r\n";
			}

			// This control will use any dropped data to add items to the listbox.
			// Therefore, only data in a text format will be allowed.  Setting the 
			// autoConvert parameter to true specifies that any data that can be
			// converted to a text format is also acceptable.
			if (e.Data.GetDataPresent(DataFormats.Text, true))
			{
				// Some controls in this sample allow both Copy and Move effects.
				// If a Move effect is allowed, this implementation assumes a Move 
				// effect unless the CTRL key was pressed, in which case a Copy
				// effect is assumed.  This follows standard DragDrop conventions.
				if ((e.AllowedEffect & DragDropEffects.Move) == DragDropEffects.Move && (e.KeyState & ctrlKey) != ctrlKey)
				{
					// Show the standard Move icon.
					e.Effect = DragDropEffects.Move;
				}
				else
				{
					// Show the standard Copy icon.
					e.Effect = DragDropEffects.Copy;
				}
			}
		}

		/// <summary>
		/// The DragDrop event of the target control fires when a drop actually occurs over
		/// the target control.  This is where the data being dragged is actually processed.
		/// 
		/// This event will fire only if the AllowDrop property of the target control has
		/// been set to true.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">A DragEventArgs that contains the event data.</param>
		private void listBox1_DragDrop(object sender, System.Windows.Forms.DragEventArgs e)
		{

			// Store the data as a string so that it can be accessed from the
			// mnuCopy and mnuMove click events.
			sourceData = e.Data.GetData(DataFormats.Text, true).ToString();

			// If the right mouse button was used, provide a context menu to allow
			// the user to select a DragDrop effect.  The mouseButton is recorded in the 
			// MouseDown event of the source control.
			if (mouseButton == MouseButtons.Right)
			{
				// Show a context menu, asking which operation to perform.
				// The ProcessData() call is then made in the click event
				// of the mnuCopy and mnuMove menu items.  Show only those
				// menu items that correspond to an allowed effect.
				mnuCopy.Visible = ((e.AllowedEffect & DragDropEffects.Copy) == DragDropEffects.Copy);
				mnuMove.Visible = ((e.AllowedEffect & DragDropEffects.Move) == DragDropEffects.Move);
				contextMenu1.Show(listBox1, new Point(20,20));
			}
			else
			{
				// Set the deleteSource member field based on the Effect.
				// The Effect is preset in the DragEnter event handler.
				deleteSource = (e.Effect == DragDropEffects.Move);

				// The processing of the data is done in a separate call, since
				// this is also called from the click event of the contextMenu1 items.
				ProcessData();

			}
		}

		/// <summary>
		/// The ProcessData function performs the work associated with copying
		/// or moving the data from the source control into the listBox1 control.
		/// This code is pulled out into a separate function in order to facilitate
		/// calling it from either the listBox1's DragDrop event or from the Click
		/// events of the mnuCopy and mnuMove menu items.
		/// </summary>
		private void ProcessData()
		{

			// Create the list item using the data provided by the source control.
			listBox1.Items.Add(sourceData);

			// Delete the source data if requested.  This would apply to a Move effect.
			if (deleteSource)
			{
				// Allow for different means of removing source data based on the type of
				// source control.  In this sample, the only control that allows a Move effect
				// is the TextBox, so only that control type is implemented.
				switch (sourceControl.GetType().Name)
				{
					case "TextBox":
						((TextBox)sourceControl).Clear();
						break;
					default:
						break;
				}
			}

			// Reset the sourceControl reference to null. This will be set to a valid 
			// reference by any source control that initiates a new DragDrop operation.
			sourceControl = null;

			// Reset the mouseButton value to None. This will be set to a valid 
			// mouse button indicator by any source control that initiates a new DragDrop operation.
			mouseButton  = MouseButtons.None;

			// Reset the deleteSource value to false. This will be updated as needed in the
			// DragDrop event of the target control or in the click events of the context menu items.
			deleteSource = false;

		}

		/// <summary>
		/// This event handler sets the deleteSource field and calls ProcessData()
		/// in response to a Copy request made from the context menu.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs that contains the event data.</param>
		private void mnuCopy_Click(object sender, System.EventArgs e)
		{
			// Set the deleteSource member field to indicate that this
			// is a Copy.
			deleteSource = false;

			// Now process the data.
			ProcessData();
		}

		/// <summary>
		/// This event handler sets the deleteSource field and calls ProcessData()
		/// in response to a Move request made from the context menu.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs that contains the event data.</param>
		private void mnuMove_Click(object sender, System.EventArgs e)
		{
			// Set the deleteSource member field to indicate that this
			// is a Move.
			deleteSource = true;

			// Now process the data.
			ProcessData();
		}

		private void textBox1_TextChanged(object sender, System.EventArgs e)
		{
		
		}

	}
}
