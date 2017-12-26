#region Using directives

using System;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;
using System.Data;

#endregion
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
namespace Microsoft.Samples.DeviceNotification 
{
	using System;
	using System.Drawing;
	using System.Collections;
	using System.ComponentModel;
	using System.Windows.Forms;
	using System.Data;
	using Microsoft.WindowsCE.Forms;


    public partial class DeviceNotification : System.Windows.Forms.Form
    {
		Notification notify = new Notification();

        public DeviceNotification()
        {
            InitializeComponent();
		}



		private void DeviceNotification_Load(object sender, EventArgs e)
		{

			//Filling NotifyText with html for notification layout
			//This html defines the Notification bubble.
        
			System.Text.StringBuilder notifyHtml = new System.Text.StringBuilder("<html><body>");
			notifyHtml.Append("<a href='URL Here'>Test Link</a>");
			notifyHtml.Append("<p><form method='GET' action=mybubble>");
			notifyHtml.Append("<p>This is an <font color='#0000FF'><b>HTML</b></font> notification stored in a  <font color='#FF0000'><i>string</i></font> table!</p>");
			notifyHtml.Append("<p><input type=text name=textinput value='Input Sample'><input type='submit'></p>");
			notifyHtml.Append("<p align=right><input type=button name=OK value='Ok'> <input type=button name='cmd:2' value='Cancel'></p>");
			notifyHtml.Append("</body></html>");

			//Set notifyHtml text to NotifyText Control
			this.NotifyText.Text = notifyHtml.ToString();

			//Setting Notify with default data from controls
			notify.Caption = this.NotifyCaption.Text;
			notify.Critical = this.NotifyCritical.Checked;
			notify.InitialDuration = (Int32)this.NotifyDuration.Value;
			notify.Text = this.NotifyText.Text;

			this.NotifyIcon.Text = "Icon1";

			//Add response and Change Handlers
			notify.BalloonChanged += OnNotifyBubChange;
			notify.ResponseSubmitted += OnNotifyResponse;


		}
		private void OnNotifyBubChange(Object obj , BalloonChangedEventArgs e)
		{
			//handeles Nofificaiton Change
			this.NotifyVisable.Text = e.Visible.ToString();
		}
	
		 private void OnNotifyResponse(Object obj, ResponseSubmittedEventArgs e)
		 {
			 //handles response from Notification
			 this.NotifyReponce.Text = e.Response.ToString();
		 }

		private void NotifyShow_Click(object sender, EventArgs e)
		 {
			//Shows Current Notify state if notify has not been disposed.
			 notify.Visible = true;
		 }

		private void NotifyCaption_TextChanged(object sender, EventArgs e)
		 {
			//Sets Notify Caption to the content of the Forms NotifyCaption text box.
			 notify.Caption = this.NotifyCaption.Text;
		 }

		private void NotifyText_TextChanged(object sender, EventArgs e)
		 {
			//Sets Notify Text to the content of the Forms NotifyText text box.
			 notify.Text = this.NotifyText.Text;
		 }

		private void NotifyIcon_SelectedIndexChanged(object sender, EventArgs e)
		 {
			//Sets Notify Icon to the matching icon selection

			 switch (this.NotifyIcon.Text)
			 {
				 case "Icon1":
					 notify.Icon = new Icon(System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceStream("DeviceNotification.Resources.bel.ico"), 16, 16);
					 break;
				 case "Icon2":
					 notify.Icon = new Icon(System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceStream("DeviceNotification.Resources.cedialer.ico"), 16, 16);
					 break;
			 }


		}


		private void NotifyCritical_CheckStateChanged(object sender, EventArgs e)
		{
	        //Sets Notify Critical to checked state of NotifyCritical on the form
			notify.Critical = this.NotifyCritical.Checked;
		}

		private void NotifyHide_Click(object sender, EventArgs e)
		{
			//Hides Notify object
			notify.Visible = false;
		}



		private void NewNotificationAdd_Click(object sender, EventArgs e)
		{
			//creates a new notification to show how two notifications behave.  This notificaiton will not
			//report back to the form.

			Notification notify2 = new Notification();
			notify2.Text = "<html><body><p>Sample Notification<p/><p align=right><input type=button name=OK value='Ok'> <input type=button name='Cancel' value='Cancel'></p></body></html>";
			notify2.Icon = new Icon(System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceStream("DeviceNotification.Resources.cedialer.ico"), 16, 16);
			notify2.Visible = true;
		}

		private void DeviceNotification_Closing(object sender, CancelEventArgs e)
		{
			//Clean up the Nofity object
			notify.Dispose();
		}

		private void NotifyDuration_ValueChanged(object sender, EventArgs e)
		{
			//Sets Notify Duration to the content of the Forms NotifyDuration text box.
			notify.InitialDuration = (Int32)this.NotifyDuration.Value;
		}

    }
}

