using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace PrintDocument
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label lblCompany;
		private System.Windows.Forms.Label lblHeaderFont;
		private System.Windows.Forms.Label lblBodyFont;
		private System.Windows.Forms.Label lblReportData;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		/// <summary>
		/// Used to display a font selection dialog.  This single dialog is
		/// used for both the header and body text font selections.
		/// </summary>
		private System.Windows.Forms.FontDialog fontDialog1;

		/// <summary>
		/// Used to handle the actual formatting and printing of the report.
		/// </summary>
		private System.Drawing.Printing.PrintDocument printDocument1;

		/// <summary>
		/// Used to select a destination printer.
		/// </summary>
		private System.Windows.Forms.PrintDialog printDialog1;

		/// <summary>
		/// Font to use for printing the page header.
		/// </summary>
		private System.Drawing.Font headerFont;

		/// <summary>
		/// Font to use for printing the report body text.
		/// </summary>
		private System.Drawing.Font bodyFont;
		private System.Windows.Forms.TextBox headerFontDefinition;
		private System.Windows.Forms.TextBox companyName;
		private System.Windows.Forms.Button PrintButton;
		private System.Windows.Forms.TextBox reportData;
		private System.Windows.Forms.TextBox bodyFontDefinition;
		private System.Windows.Forms.Button SelectHeaderFont;
		private System.Windows.Forms.Button SelectBodyFont;

		/// <summary>
		/// Used to read report data from a static XML file.
		/// </summary>
		private System.Xml.XmlTextReader xmlReader;

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
			this.lblReportData = new System.Windows.Forms.Label();
			this.printDialog1 = new System.Windows.Forms.PrintDialog();
			this.printDocument1 = new System.Drawing.Printing.PrintDocument();
			this.headerFontDefinition = new System.Windows.Forms.TextBox();
			this.companyName = new System.Windows.Forms.TextBox();
			this.lblBodyFont = new System.Windows.Forms.Label();
			this.SelectHeaderFont = new System.Windows.Forms.Button();
			this.PrintButton = new System.Windows.Forms.Button();
			this.reportData = new System.Windows.Forms.TextBox();
			this.SelectBodyFont = new System.Windows.Forms.Button();
			this.bodyFontDefinition = new System.Windows.Forms.TextBox();
			this.lblHeaderFont = new System.Windows.Forms.Label();
			this.fontDialog1 = new System.Windows.Forms.FontDialog();
			this.lblCompany = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// lblReportData
			// 
			this.lblReportData.Location = new System.Drawing.Point(8, 160);
			this.lblReportData.Name = "lblReportData";
			this.lblReportData.TabIndex = 19;
			this.lblReportData.Text = "Report Data:";
			// 
			// printDialog1
			// 
			this.printDialog1.Document = this.printDocument1;
			// 
			// printDocument1
			// 
			this.printDocument1.DocumentName = "Customer List";
			this.printDocument1.PrintPage += new System.Drawing.Printing.PrintPageEventHandler(this.printDocument1_PrintPage);
			// 
			// headerFontDefinition
			// 
			this.headerFontDefinition.Location = new System.Drawing.Point(120, 40);
			this.headerFontDefinition.Multiline = true;
			this.headerFontDefinition.Name = "headerFontDefinition";
			this.headerFontDefinition.ReadOnly = true;
			this.headerFontDefinition.Size = new System.Drawing.Size(408, 48);
			this.headerFontDefinition.TabIndex = 14;
			this.headerFontDefinition.Text = "";
			// 
			// companyName
			// 
			this.companyName.Location = new System.Drawing.Point(120, 8);
			this.companyName.Name = "companyName";
			this.companyName.Size = new System.Drawing.Size(408, 20);
			this.companyName.TabIndex = 12;
			this.companyName.Text = "Sample Company";
			// 
			// lblBodyFont
			// 
			this.lblBodyFont.Location = new System.Drawing.Point(8, 104);
			this.lblBodyFont.Name = "lblBodyFont";
			this.lblBodyFont.TabIndex = 16;
			this.lblBodyFont.Text = "Body Text Font:";
			// 
			// SelectHeaderFont
			// 
			this.SelectHeaderFont.Location = new System.Drawing.Point(536, 40);
			this.SelectHeaderFont.Name = "SelectHeaderFont";
			this.SelectHeaderFont.Size = new System.Drawing.Size(24, 20);
			this.SelectHeaderFont.TabIndex = 15;
			this.SelectHeaderFont.Text = "...";
			this.SelectHeaderFont.Click += new System.EventHandler(this.SelectHeaderFont_Click);
			// 
			// PrintButton
			// 
			this.PrintButton.Location = new System.Drawing.Point(248, 312);
			this.PrintButton.Name = "PrintButton";
			this.PrintButton.TabIndex = 21;
			this.PrintButton.Text = "Print";
			this.PrintButton.Click += new System.EventHandler(this.PrintButton_Click);
			// 
			// reportData
			// 
			this.reportData.Location = new System.Drawing.Point(120, 160);
			this.reportData.Multiline = true;
			this.reportData.Name = "reportData";
			this.reportData.ReadOnly = true;
			this.reportData.ScrollBars = System.Windows.Forms.ScrollBars.Both;
			this.reportData.Size = new System.Drawing.Size(408, 144);
			this.reportData.TabIndex = 20;
			this.reportData.Text = "";
			// 
			// SelectBodyFont
			// 
			this.SelectBodyFont.Location = new System.Drawing.Point(536, 104);
			this.SelectBodyFont.Name = "SelectBodyFont";
			this.SelectBodyFont.Size = new System.Drawing.Size(24, 20);
			this.SelectBodyFont.TabIndex = 18;
			this.SelectBodyFont.Text = "...";
			this.SelectBodyFont.Click += new System.EventHandler(this.SelectBodyFont_Click);
			// 
			// bodyFontDefinition
			// 
			this.bodyFontDefinition.Location = new System.Drawing.Point(120, 104);
			this.bodyFontDefinition.Multiline = true;
			this.bodyFontDefinition.Name = "bodyFontDefinition";
			this.bodyFontDefinition.ReadOnly = true;
			this.bodyFontDefinition.Size = new System.Drawing.Size(408, 48);
			this.bodyFontDefinition.TabIndex = 17;
			this.bodyFontDefinition.Text = "";
			// 
			// lblHeaderFont
			// 
			this.lblHeaderFont.Location = new System.Drawing.Point(8, 40);
			this.lblHeaderFont.Name = "lblHeaderFont";
			this.lblHeaderFont.TabIndex = 13;
			this.lblHeaderFont.Text = "Header Text Font:";
			// 
			// lblCompany
			// 
			this.lblCompany.Location = new System.Drawing.Point(8, 8);
			this.lblCompany.Name = "lblCompany";
			this.lblCompany.TabIndex = 11;
			this.lblCompany.Text = "Company Name:";
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(568, 340);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.lblBodyFont,
																		  this.SelectHeaderFont,
																		  this.PrintButton,
																		  this.reportData,
																		  this.SelectBodyFont,
																		  this.bodyFontDefinition,
																		  this.lblHeaderFont,
																		  this.lblCompany,
																		  this.lblReportData,
																		  this.headerFontDefinition,
																		  this.companyName});
			this.Name = "Form1";
			this.Text = "Customer List Printing Sample";
			this.Load += new System.EventHandler(this.Form1_Load);
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
		/// Occurs before a form is displayed for the first time.
		/// Contains code to update read an XML file and update
		/// the text of some form controls.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs that contains the event data.</param>
		private void Form1_Load(object sender, System.EventArgs e)
		{
			
			// Set the default fonts for the header and text.
			headerFont = new Font("Arial", 14);
			bodyFont = new Font("Arial", 10);

			// Update the form controls to display the default font settings.
			headerFontDefinition.Text = headerFont.ToString();
			bodyFontDefinition.Text = bodyFont.ToString();
			

			// Perform the reading of the data file within a try block in case
			// the data file doesn't exist or an error is encountered reading the file.
			try
			{
				// The XmlTextReader reads the specified XML file and provides a way to
				// scroll through the data elements.
				xmlReader = new System.Xml.XmlTextReader("..\\..\\..\\Misc\\ReportData.xml");

				// Call the Read method in order to position the reader at the
				// first element.
				xmlReader.Read();

				// Show the entire contents of the data file in 
				// a textbox for comparison to the printed report.
				reportData.Text = xmlReader.ReadOuterXml();

				// Closes the XmlReader.
				xmlReader.Close();
			}
			catch(Exception ex) 
			{

				// An error was encountered opening or reading the data file.
				// Display an appropriate message to the user.
				MessageBox.Show("Error opening file: \r\n" + ex.Message);

				// Close the form since a report can't be printed without access
				// to the data file.
				this.Close();
			}
		}

		/// <summary>
		/// This event displays a printer selection dialog and then starts the print
		/// process.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs that contains the event data.</param>
		private void PrintButton_Click(object sender, System.EventArgs e)
		{

			// Perform the print within a try block in case a failure
			// of any type occurs.  For this sample, all errors will
			// be handled generically by simply displaying a messagebox.
			try 
			{
				// Open the XML Data file.
				xmlReader = new System.Xml.XmlTextReader("..\\..\\..\\Misc\\ReportData.xml");

				// Position the pointer to the first element.
				xmlReader.Read();

				// Display a printer selection dialog.
				// Only print the document if the user clicks OK.
				if (printDialog1.ShowDialog() == DialogResult.OK)
				{
					// This starts the actual print.  The code to output
					// text to the selected printer resides in the PrintDocument1_PrintPage
					// event handler.
					printDocument1.Print();
				}
				
				// Close the data file.
				xmlReader.Close();

			}
			catch (Exception ex)
			{
				// If any error occurs, display a messagebox.
				MessageBox.Show("Error printing report: \r\n" + ex.Message);
			}
		}

		/// <summary>
		/// This event occurs when the output to print for the current page is needed, and 
		/// contains the logic to handle the content and layout of the printed page.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">A PrintPageEventArgs that contains the event data.</param>
		private void printDocument1_PrintPage(object sender, System.Drawing.Printing.PrintPageEventArgs e)
		{
			// Determine the height of the header, based on the selected font.
			float headerHeight = headerFont.GetHeight(e.Graphics);

			// Determine the number of lines of body text that can be printed per page, taking
			// into account the presence of the header and the size of the selected body font.
			float linesPerPage = (e.MarginBounds.Height - headerHeight)/ bodyFont.GetHeight(e.Graphics);

			// Used to store the position at which the next body line
			// should be printed.
			float yPosition = 0;

			// Used to store the number of lines printed so far on the
			// current page.
			int count = 0;

			// User to store the text of the current line.
			string line = null;

			// Print the page header, as specified by the user in the form.
			// Use the header font for this line only.
			e.Graphics.DrawString(companyName.Text.Trim(), headerFont, Brushes.Black, e.MarginBounds.Left, e.MarginBounds.Top, new StringFormat());

			// Print each line of the data file, but don't exceed the maximum allowable
			// number of lines per page.  Also, stop when the end of the data file is
			// reached.  This event is called once per page, so if the data exceeds a single
			// page, the XmlTextReader will pick up where it left off on the previous page.
			while(count < linesPerPage && !xmlReader.EOF)
			{
				// Move the pointer to the next "context" line in the Xml data file.
				// If the line is not an XML element, then it can be skipped.
				// Because the initial Read() call made when the xmlReader was opened
				// positioned the file to the first element (<Customers>) the following
				// call actually moves to the first <Customer> tag the first time through.
				if (xmlReader.MoveToContent() == System.Xml.XmlNodeType.Element)
				{
					// Based on the element, determine what to print and where to print it.
					switch (xmlReader.Name)
					{
						case "Customers":
						{
							// This is not really required, since the initial Read() call
							// when the xmlReader is loaded effectively bypasses the opening element
							// as described in the comments above.  Included here for explanation only.

							// If a <Customers> tag is encountered, just print a blank line.
							line = "";

							// Tell the XmlTextReader to move on to the next element.
							xmlReader.Read();

							break;
						}
						case "Customer":
						{

							// Hitting a new <Customer> tag indicates the beginning of a 
							// new customer record.  Take this opportunity to print a blank line,
							// adding spacing between customer records in the report.
							line = "";

							// Tell the XmlTextReader to move on to the next element.
							xmlReader.Read();

							break;
						}
						default:
						{
							// All other elements in the sample XML file are actual data
							// fields pertaining to a customer record.  Print the field name
							// and value.  The ReadElementString retrieves the value and 
							// automatically forces the XmlTextReader to move on to the 
							// next element.  Because this is handled generically, any additional
							// customer fields added inside the <Customer> tag in the xml file will 
							// automatically be shown in the printed report.
							line = xmlReader.Name + ": " + xmlReader.ReadElementString();
							break;
						}
					}

					// Determine the position at which to print.  Since this report prints one line
					// at a time, only the height (or Y coordinate) needs to be calculated, because
					// every line will begin at the far left.  The Y coordinate must take into 
					// consideration the header, the height of each line of body text, and the 
					// number of body lines printed so far on this page.
					yPosition = e.MarginBounds.Top + headerHeight + (count * bodyFont.GetHeight(e.Graphics));

					// Draw the line of text on the page using the body font specified by the user.
					e.Graphics.DrawString(line, bodyFont, Brushes.Black, e.MarginBounds.Left, yPosition, new StringFormat());

					// Increment the counter to show that another line has been printed.
					// This is used in the positioning of future lines of text on the current page.
					count++;
				}
				else
				{
					// If the XmlTextReader is positioned on a line that is NOT an
					// Element, then just go on to the next line.
					xmlReader.Read();
				}
			}

			// If more data exists, print another page.  If not stop this event from firing
			// again by setting the HasMorePages property to false.
			if(xmlReader.EOF)
				e.HasMorePages = false;
			else
				e.HasMorePages = true;
		}

		/// <summary>
		/// Uses the fontDialog to allow the user to specify a font
		/// for the report header.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs that contains the event data.</param>
		private void SelectHeaderFont_Click(object sender, System.EventArgs e)
		{
			// Set the initial selection to the currently selected header font.
			// Since this one dialog is used for selection of both the header
			// and body fonts, this needs to be set each time.
			fontDialog1.Font = headerFont;

			// Show the font selection dialog.
			if (fontDialog1.ShowDialog() == DialogResult.OK)
			{
				// Update the selected font and the form controls
				// only if the user pressed OK.
				headerFont = fontDialog1.Font;
				headerFontDefinition.Text = headerFont.ToString();
			}
		}

		/// <summary>
		/// Uses the fontDialog to allow the user to specify a font
		/// for the report body.
		/// </summary>
		/// <param name="sender">The source of the event.</param>
		/// <param name="e">An EventArgs that contains the event data.</param>
		private void SelectBodyFont_Click(object sender, System.EventArgs e)
		{
			// Set the initial selection to the currently selected body font.
			// Since this one dialog is used for selection of both the header
			// and body fonts, this needs to be set each time.
			fontDialog1.Font = bodyFont;

			// Show the font selection dialog.
			if (fontDialog1.ShowDialog() == DialogResult.OK)
			{
				// Update the selected font and the form controls
				// only if the user pressed OK.
				bodyFont = fontDialog1.Font;
				bodyFontDefinition.Text = bodyFont.ToString();
			}
		}
	}
}
