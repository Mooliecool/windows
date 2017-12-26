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


using System;
using System.Workflow.Activities;
    
namespace Microsoft.Samples.Workflow.SpeechApplication
{
    public class SpeechService : ISpeechService
    {
        private string currentMenuTextValue;

        public string CurrentMenuText
        {
            get { return currentMenuTextValue; }
            set { currentMenuTextValue = value; }
        }
	
        public delegate void UpdatePhoneTextEventHandler(object sender, UpdatePhoneTextEventArgs e);
	
        public event UpdatePhoneTextEventHandler PhoneTextChangedEventHandler;

        public SpeechService()
        {
        }

        public void SendMenuText(string menuText)
        {
            this.CurrentMenuText = menuText;
            if (this.PhoneTextChangedEventHandler != null)
            {
                this.PhoneTextChangedEventHandler(this, new UpdatePhoneTextEventArgs(CurrentMenuText));
            }
        }
        
        public void RaiseButtonPressed(SpeechEventArgs args)
        {
            switch (args.ButtonPressed)
            {
                case "button0":
                    Button0Pressed(null, args);
                    break;
                case "button1":
                    Button1Pressed(null, args);
                    break;
                case "button2":
                    Button2Pressed(null, args);
                    break;
                case "button3":
                    Button3Pressed(null, args);
                    break;
                case "button4":
                    Button4Pressed(null, args);
                    break;
                case "button5":
                    Button5Pressed(null, args);
                    break;
                case "button6":
                    Button6Pressed(null, args);
                    break;
                case "button7":
                    Button7Pressed(null, args);
                    break;
                case "button8":
                    Button8Pressed(null, args);
                    break;
                case "button9":
                    Button9Pressed(null, args);
                    break;
                case "buttonstar":
                    ButtonStarPressed(null, args);
                    break;
                case "buttonpound":
                    ButtonPoundPressed(null, args);
                    break;

            }
        }

        public event EventHandler<SpeechEventArgs> Button0Pressed;
        public event EventHandler<SpeechEventArgs> Button1Pressed;
        public event EventHandler<SpeechEventArgs> Button2Pressed;
        public event EventHandler<SpeechEventArgs> Button3Pressed;
        public event EventHandler<SpeechEventArgs> Button4Pressed;
        public event EventHandler<SpeechEventArgs> Button5Pressed;
        public event EventHandler<SpeechEventArgs> Button6Pressed;
        public event EventHandler<SpeechEventArgs> Button7Pressed;
        public event EventHandler<SpeechEventArgs> Button8Pressed;
        public event EventHandler<SpeechEventArgs> Button9Pressed;
        public event EventHandler<SpeechEventArgs> ButtonStarPressed;
        public event EventHandler<SpeechEventArgs> ButtonPoundPressed;

        
    }
}
