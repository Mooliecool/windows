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
    public sealed partial class Workflow1 : StateMachineWorkflowActivity
    {
        string ButtonPressed = "";

        private object phoneEventSenderValue  = new object();

        public object PhoneEventSender
        {
            get { return phoneEventSenderValue; }
            set { phoneEventSenderValue = value; }
        }
	
        private string menuTextValue;

        public string MenuText
        {
            get { return menuTextValue; }
            set { menuTextValue = value; }
        }
	
        public Workflow1()
        {
            
            InitializeComponent();
        }

        private void SetMainMenuText(object sender, EventArgs e)
        {
            if (this.ButtonPressed == null)
            {
                throw new Exception("Button Preseed cannot be null");
            }

            this.MenuText = "Welcome to the Microsoft .NET Framework 3.0 Hotline \n";
            this.MenuText += "Press 1 for Windows Workflow Foundation (WF) Information  \n";
            this.MenuText += "Press 2 for Microsoft Speech Server \n";
            this.MenuText += "Press 3 for Windows Communication Foundation Information \n";
            this.MenuText += "Press 4 for Windows Presentation Foundation Information \n";
            this.MenuText += "At any point in the session press * to end the conversation and 9 to go to the main menu \n";
        }

        private void SetEndSessionText(object sender, EventArgs e)
        {
            this.MenuText = "Thank You for Calling the Microsoft .NET Framework 3.0 HotLine \n";
            this.MenuText += "We Hope you found this information useful \n";
            this.MenuText += "please do call us again. GoodBye \n";
        }

        private void SetInfoText(object sender, EventArgs e)
        {
            this.MenuText = "Windows Workflow Foundation is the programming model\n";
            this.MenuText += "engine and tools for quickly building workflow-enabled applications on Windows.\n";
            this.MenuText += "WF offers different workflow creation styles \n";
            this.MenuText += "Please press 1 if you are interested in creating workflows in a sequential style \n";
            this.MenuText += "Please press 2 if you are interested in creating workflows as state machines \n";
        }

        private void SetSequentialText(object sender, EventArgs e)
        {
            this.MenuText = "Sequential workflows allow the creation of workflows \n";
            this.MenuText += "that run sequentially from one activity in the workflow to the next \n";
            this.MenuText += "Sequential workflows are good for process automation scenarios \n";
            this.MenuText += "\n Press 0 to Hear this again \n";
            this.MenuText += "Press 8 to go back to the Windows Workflow Menu";
        }

        private void SetFSMText(object sender, EventArgs e)
        {
            this.MenuText = "State Machine workflows allow the creation of workflows where there are lot of adhoc actions \n";
            this.MenuText += "The workflow may not follow a sequential path of execution \n";
            this.MenuText += "State Machine workflows are very useful in human wokflows \n";
            this.MenuText += "\n Press 0 to Hear this again \n";
            this.MenuText += "Press 8 to go back to the Windows Workflow Menu";
        }

        private void SetSpeechServerText(object sender, EventArgs e)
        {
            this.MenuText = "Microsoft Speech Server is the next generation Speech Application Platform \n";
            this.MenuText += "It helps in the creation of world class applications by driving adoption of speech technologies in the platform \n";
            this.MenuText += "MSS allows features such as telephony support to VoIP \n";
            this.MenuText += "MSS allows the cration of packaged speech enabled applications  \n";
            this.MenuText += "\n Press 0 to hear this again \n";
        }

        private void SetIndigoText(object sender, EventArgs e)
        {
            this.MenuText = "WCF is Microsoft's platform for building and deploying distributed applications \n";
            this.MenuText += "WCF is a new breed for communications infrastructure built around the Web services architecture \n";
            this.MenuText += "Advanced Web Services support in WCF provides secure, reliable, and transacted messaging along with interoperability \n";
            this.MenuText += "\n Press 0 to hear this again \n";
        }

        private void SetAvalonText(object sender, EventArgs e)
        {
            this.MenuText = "WPF is Microsoft's platform for building UI applications \n";
            this.MenuText += "\n Press 0 to hear this again \n";
        }

    }

}
