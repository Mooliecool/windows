/*************************************************************************************************
 *
 * File: ProviderForm.cs
 *
 * Description: 
 * 
 * This sample demonstrates how to implement a custom control that can interact
 * with UI Automation.
 * 
 * When you develop an application using standard controls such as Win32 or 
 * WinForms controls, UI Automation is automatically supported by standard 
 * proxy objects. UI Automation can find the controls on your form, get 
 * properties from them, and act on them; for example, by invoking a button
 * control or adding text to a textbox.
 * 
 * However, if you develop custom controls, perhaps within a custom user interface
 * not based on a standard UI such as WinForms, UI Automation will not be able to 
 * communicate with those custom controls unless, at a minimum, you take the steps
 * demonstrated in this sample.
 *     
 * The sample shows the simplest possible custom control, a "button" that changes 
 * its pattern when the user clicks on it with the mouse, or tabs to it and presses
 * the space bar. Although this control is derived from System.Windows.Forms.Control,
 * it is not a System.Windows.Forms.Button, and therefore it is not represented by 
 * a standard UI Automation button proxy.
 * 
 * Run UISpy.exe to see UIAutomation's view of the control. You can also Invoke the 
 * button by using the Control Patterns window in UISpy.
 * 
 *    
 *  This file is part of the Microsoft Windows SDK Code Samples.
 * 
 *  Copyright (C) Microsoft Corporation.  All rights reserved.
 * 
 * This source code is intended only as a supplement to Microsoft
 * Development Tools and/or on-line documentation.  See these other
 * materials for detailed information regarding Microsoft code samples.
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 * 
 *************************************************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;


namespace ElementProvider
{
    public partial class UIAProviderForm : Form
    {
        static CustomButton myCustomButton;

        /// <summary>
        /// Constructor for the application window.
        /// </summary>
        public UIAProviderForm()
        {
            InitializeComponent();

            // Create an instance of the custom control.
            Rectangle controlRect = new Rectangle(30,15,55,40);
            myCustomButton = new CustomButton();

            myCustomButton.Text = "CustomControl";   // This becomes the Name property for UI Automation.

            // Give the control a location and size so that it will trap mouse clicks
            // and will be repainted as necessary.
            myCustomButton.Location = new System.Drawing.Point(controlRect.X, controlRect.Y);
            myCustomButton.Size = new System.Drawing.Size(controlRect.Width, controlRect.Bottom);
            myCustomButton.TabIndex = 1;

            // Add it to the form's controls. Among other things, this makes it possible for
            // UI Automation to discover it, as it will become a child of the application window.
            this.Controls.Add(myCustomButton);
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

    }
}
