/*************************************************************************************************
 *
 * File: MyControl.cs
 *
 * Description: Implements a simple custom control that supports UI Automation.
 * 
 * See ProviderForm.cs for a full description of this sample.
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
using System.Text;
using System.Windows.Automation.Provider;
using System.Windows.Automation;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;
using System.Security.Permissions;


namespace ElementProvider
{
    class CustomButton : Control, IRawElementProviderSimple, IInvokeProvider
    {
        bool buttonState = false;
        IntPtr myHandle;

        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="rect">Position and size of control.</param>
        public CustomButton()
        {
            myHandle = Handle;

            // Add event handlers.
            MouseDown += new System.Windows.Forms.MouseEventHandler(this.CustomButton_MouseDown);
            this.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.CustomButton_KeyPress);
            this.GotFocus += new EventHandler(CustomButton_ChangeFocus);
            this.LostFocus += new EventHandler(CustomButton_ChangeFocus);
        }

        /// <summary>
        /// Handles WM_GETOBJECT message; others are passed to base handler.
        /// </summary>
        /// <param name="m">Windows message.</param>
        /// <remarks>This method provides the link with UI Automation.</remarks>
        [PermissionSetAttribute(SecurityAction.Demand, Unrestricted = true)]
        protected override void WndProc(ref Message m)
        {
            // 0x3D == WM_GETOBJECT
            if ((m.Msg == 0x3D) && (m.LParam.ToInt32() == AutomationInteropProvider.RootObjectId))
            {
                m.Result = AutomationInteropProvider.ReturnRawElementProvider(
                    Handle, m.WParam, m.LParam, (IRawElementProviderSimple)this);
                return;
            }
            base.WndProc(ref m);
        }

        /// <summary>
        /// Ensure that the focus rectangle is drawn or erased when focus changes.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void CustomButton_ChangeFocus(object sender, EventArgs e)
        {
            Refresh();
        }


        /// <summary>
        /// Handles Paint event.
        /// </summary>
        /// <param name="e">Event arguments.</param>
        protected override void OnPaint(PaintEventArgs e)
        {
            Rectangle buttonRect = new Rectangle(ClientRectangle.Left + 2,
                ClientRectangle.Top + 2,
                ClientRectangle.Width - 4,
                ClientRectangle.Height - 4);
            System.Drawing.Drawing2D.HatchBrush brush;
            if (buttonState)
            {
                brush = new System.Drawing.Drawing2D.HatchBrush(
                    System.Drawing.Drawing2D.HatchStyle.DarkHorizontal, Color.Red, Color.White);
            }
            else
            {
                brush = new System.Drawing.Drawing2D.HatchBrush(
                    System.Drawing.Drawing2D.HatchStyle.DarkVertical, Color.Green, Color.White);
            }

            e.Graphics.FillRectangle(brush, buttonRect);
            if (Focused)
            {
                ControlPaint.DrawFocusRectangle(e.Graphics, ClientRectangle);
            }
        }

        /// <summary>
        /// Responds to a button click, regardless of whether it was caused by a mouse or
        /// keyboard click or by InvokePattern.Invoke. 
        /// </summary>
        private void RespondToClick()
        {
            buttonState = !buttonState;
            this.Focus();
            this.Refresh();

            // Raise an event.
            if (AutomationInteropProvider.ClientsAreListening)
            {
                AutomationEventArgs args = new AutomationEventArgs(InvokePatternIdentifiers.InvokedEvent);
                AutomationInteropProvider.RaiseAutomationEvent(InvokePatternIdentifiers.InvokedEvent, this, args);
            }
        }

        /// <summary>
        /// Handles MouseDown event.
        /// </summary>
        /// <param name="sender">Object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        public void CustomButton_MouseDown(object sender, MouseEventArgs e)
        {
            RespondToClick();
        }

        /// <summary>
        /// Handles Keypress event.
        /// </summary>
        /// <param name="sender">Object that raised the event.</param>
        /// <param name="e">Event arguments.</param>
        public void CustomButton_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == (char)Keys.Space)
            {
                RespondToClick();
            }
        }

        #region IRawElementProviderSimple

        /// <summary>
        /// Returns the object that supports the specified pattern.
        /// </summary>
        /// <param name="patternId">ID of the pattern.</param>
        /// <returns>Object that implements IInvokeProvider.</returns>
        object IRawElementProviderSimple.GetPatternProvider(int patternId)
        {
            if (patternId == InvokePatternIdentifiers.Pattern.Id)
            {
                return this;
            }
            else
            {
                return null;
            }
        }

        /// <summary>
        /// Returns property values.
        /// </summary>
        /// <param name="propId">Property identifier.</param>
        /// <returns>Property value.</returns>
        object IRawElementProviderSimple.GetPropertyValue(int propId)
        {
            if (propId == AutomationElementIdentifiers.ClassNameProperty.Id)
            {
                return "CustomButtonControlClass";
            }
            else if (propId == AutomationElementIdentifiers.ControlTypeProperty.Id)
            {
                return ControlType.Button.Id;
            }
            if (propId == AutomationElementIdentifiers.HelpTextProperty.Id)
            {
                return "Change the button color and pattern.";
            }
            if (propId == AutomationElementIdentifiers.IsEnabledProperty.Id)
            {
                return true;
            }
            else
            {
                return null;
            }
        }


        /// <summary>
        /// Tells UI Automation that this control is hosted in an HWND, which has its own
        /// provider.
        /// </summary>
        IRawElementProviderSimple IRawElementProviderSimple.HostRawElementProvider
        {
            get
            {
                return AutomationInteropProvider.HostProviderFromHandle(myHandle);
            }
        }

        /// <summary>
        /// Retrieves provider options.
        /// </summary>
        ProviderOptions IRawElementProviderSimple.ProviderOptions
        {
            get
            {
                return ProviderOptions.ServerSideProvider;
            }
        }
        #endregion IRawElementProviderSimple

        #region IInvokeProvider

        /// <summary>
        /// Responds to an InvokePattern.Invoke by simulating a MouseDown event.
        /// </summary>
        void IInvokeProvider.Invoke()
        {
            // If the control is not enabled, we're responsible for letting UI Automation know.
            // It catches the exception and then throws it to the client.
            IRawElementProviderSimple provider = this as IRawElementProviderSimple;
            if (false == (bool)provider.GetPropertyValue(AutomationElementIdentifiers.IsEnabledProperty.Id))
            {
                throw new ElementNotEnabledException();
            }

            // Create arguments for the click event. The parameters aren't used.
            MouseEventArgs mouseArgs = new MouseEventArgs(MouseButtons.Left, 1, 0, 0, 0);

            // Simulate a mouse click. We cannot call RespondToClick directly, 
            // because it is illegal to update the UI from a different thread.
            MouseEventHandler handler = CustomButton_MouseDown;
            BeginInvoke(handler, new object[] { this, mouseArgs });
        }

        #endregion InvokeProvider


    }  // CustomButton class.
} // Namespace.
