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
    [Serializable]
    public class SpeechEventArgs : ExternalDataEventArgs
    {
        private string buttonIdValue;

        public SpeechEventArgs(Guid instanceId, string id)
            : base(instanceId)
        {
            buttonIdValue = id;
        }

        public string ButtonPressed
        {
            get { return buttonIdValue; }
            set { buttonIdValue = value; }
        }
    }

    [ExternalDataExchange]
    public interface ISpeechService
    {
        void SendMenuText(string menuText);

        event EventHandler<SpeechEventArgs> Button0Pressed;
        event EventHandler<SpeechEventArgs> Button1Pressed;
        event EventHandler<SpeechEventArgs> Button2Pressed;
        event EventHandler<SpeechEventArgs> Button3Pressed;
        event EventHandler<SpeechEventArgs> Button4Pressed;
        event EventHandler<SpeechEventArgs> Button5Pressed;
        event EventHandler<SpeechEventArgs> Button6Pressed;
        event EventHandler<SpeechEventArgs> Button7Pressed;
        event EventHandler<SpeechEventArgs> Button8Pressed;
        event EventHandler<SpeechEventArgs> Button9Pressed;
        event EventHandler<SpeechEventArgs> ButtonStarPressed;
        event EventHandler<SpeechEventArgs> ButtonPoundPressed;
    }

}
