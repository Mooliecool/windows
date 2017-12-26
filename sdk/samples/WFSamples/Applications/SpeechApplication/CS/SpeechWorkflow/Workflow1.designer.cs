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
using System.Workflow.ComponentModel;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.SpeechApplication
{
    public sealed partial class Workflow1 
    {

        #region Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCode]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            System.Workflow.ComponentModel.ActivityBind activitybind1 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding1 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind2 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding2 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind3 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding3 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind4 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding4 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind5 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding5 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind6 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding6 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind7 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding7 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind8 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding8 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind9 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding9 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind10 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding10 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind11 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding11 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind12 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding12 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind13 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding13 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind14 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding14 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind15 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding15 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind16 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding16 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind17 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding17 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind18 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding18 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind19 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding19 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind20 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding20 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind21 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding21 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind22 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding22 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind23 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding23 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            this.sequentialSubMenu_TransitionToSelf = new System.Workflow.Activities.SetStateActivity();
            this.sequentialSubMenu_Button0PressedEventSink = new System.Workflow.Activities.HandleExternalEventActivity();
            this.sequentialSubMenu_TransitionToInfoSubMenu = new System.Workflow.Activities.SetStateActivity();
            this.sequentialSubMenu_Button8PressedEventSink = new System.Workflow.Activities.HandleExternalEventActivity();
            this.sequentialSubMenu_SendSequentialTextToMenu = new System.Workflow.Activities.CallExternalMethodActivity();
            this.stateMachineSubMenu_TransitionToSelf = new System.Workflow.Activities.SetStateActivity();
            this.stateMachineSubMenu_Button0PressedEventSink = new System.Workflow.Activities.HandleExternalEventActivity();
            this.stateMachineSubMenu_TransitionToInfoSubMenu = new System.Workflow.Activities.SetStateActivity();
            this.stateMachineSubMenu_Button8PressedEventSink = new System.Workflow.Activities.HandleExternalEventActivity();
            this.stateMachineSubMenu_SendFSMTextToMenu = new System.Workflow.Activities.CallExternalMethodActivity();
            this.infoSubMenu_TransitionToStateMachineSubMenu = new System.Workflow.Activities.SetStateActivity();
            this.infoSubMenu_Button2PressedEventSink = new System.Workflow.Activities.HandleExternalEventActivity();
            this.infoSubMenu_TransitionToSequentialSubMenu = new System.Workflow.Activities.SetStateActivity();
            this.infoSubMenu_Button1PressedEventSink = new System.Workflow.Activities.HandleExternalEventActivity();
            this.infoSubMenu_SendInfoTextToMenu = new System.Workflow.Activities.CallExternalMethodActivity();
            this.WPFSubMenu_TransitionToSelf = new System.Workflow.Activities.SetStateActivity();
            this.WPFSubMenu_Button0Pressed = new System.Workflow.Activities.HandleExternalEventActivity();
            this.WPFSubMenu_SendAvalonTextMenu = new System.Workflow.Activities.CallExternalMethodActivity();
            this.mainMenu_TransitionToWPFSubMenu = new System.Workflow.Activities.SetStateActivity();
            this.mainMenu_Button4PressedEventSink = new System.Workflow.Activities.HandleExternalEventActivity();
            this.mainMenu_TransitionToWCFSubMenu = new System.Workflow.Activities.SetStateActivity();
            this.mainMenu_Button3PressedEventSink = new System.Workflow.Activities.HandleExternalEventActivity();
            this.mainMenu_TransitionToSpeechServerSubMenu = new System.Workflow.Activities.SetStateActivity();
            this.mainMenu_Button2PressedEventSink = new System.Workflow.Activities.HandleExternalEventActivity();
            this.mainMenu_TransitionToInfoSubMenu = new System.Workflow.Activities.SetStateActivity();
            this.mainMenu_Button1PressedEventSink = new System.Workflow.Activities.HandleExternalEventActivity();
            this.mainMenu_SendMainMenuTextToMenu = new System.Workflow.Activities.CallExternalMethodActivity();
            this.WCFSubMenu_TransitionToSelf = new System.Workflow.Activities.SetStateActivity();
            this.WCFSubMenu_Button0PressedEventSink = new System.Workflow.Activities.HandleExternalEventActivity();
            this.WCFSubMenu_SendIndigoTextMenu = new System.Workflow.Activities.CallExternalMethodActivity();
            this.speechServerSubMenu_TransitionToSpeechServerSubMenu = new System.Workflow.Activities.SetStateActivity();
            this.speechServerSubMenu_Button0PressedEventSink = new System.Workflow.Activities.HandleExternalEventActivity();
            this.speechServerSubMenu_SendSpeechServerTextToMenu = new System.Workflow.Activities.CallExternalMethodActivity();
            this.SeqButton0Event = new System.Workflow.Activities.EventDrivenActivity();
            this.SeqButton8Event = new System.Workflow.Activities.EventDrivenActivity();
            this.BroadCastSequentialInformation = new System.Workflow.Activities.StateInitializationActivity();
            this.FSMButton0Event = new System.Workflow.Activities.EventDrivenActivity();
            this.FSMButton8Event = new System.Workflow.Activities.EventDrivenActivity();
            this.BroadCastStateMachineInformation = new System.Workflow.Activities.StateInitializationActivity();
            this.InfoButton2Event = new System.Workflow.Activities.EventDrivenActivity();
            this.InfoButton1Event = new System.Workflow.Activities.EventDrivenActivity();
            this.BroadcastInformation = new System.Workflow.Activities.StateInitializationActivity();
            this.WPFButton0Event = new System.Workflow.Activities.EventDrivenActivity();
            this.BroadcastAvalonInformation = new System.Workflow.Activities.StateInitializationActivity();
            this.Button4Event = new System.Workflow.Activities.EventDrivenActivity();
            this.Button3Event = new System.Workflow.Activities.EventDrivenActivity();
            this.Button2Event = new System.Workflow.Activities.EventDrivenActivity();
            this.Button1Event = new System.Workflow.Activities.EventDrivenActivity();
            this.BroadcastInitialStatement = new System.Workflow.Activities.StateInitializationActivity();
            this.WCFButton0Event = new System.Workflow.Activities.EventDrivenActivity();
            this.broadcastIndigoInformation = new System.Workflow.Activities.StateInitializationActivity();
            this.MSSButton0Event = new System.Workflow.Activities.EventDrivenActivity();
            this.BroadCastSpeechServerInformation = new System.Workflow.Activities.StateInitializationActivity();
            this.workflow_TransitionToMainMenu = new System.Workflow.Activities.SetStateActivity();
            this.workflow_Button9PressedEventSink = new System.Workflow.Activities.HandleExternalEventActivity();
            this.workflow_TransitionToEndSession = new System.Workflow.Activities.SetStateActivity();
            this.workflow_SendEndSessionTextToMenu = new System.Workflow.Activities.CallExternalMethodActivity();
            this.workflow_ButtonStarPressentEventSink = new System.Workflow.Activities.HandleExternalEventActivity();
            this.SequentialSubMenu = new System.Workflow.Activities.StateActivity();
            this.StateMachineSubMenu = new System.Workflow.Activities.StateActivity();
            this.InfoSubMenu = new System.Workflow.Activities.StateActivity();
            this.WPFSubMenu = new System.Workflow.Activities.StateActivity();
            this.EndSession = new System.Workflow.Activities.StateActivity();
            this.MainMenu = new System.Workflow.Activities.StateActivity();
            this.WCFSubMenu = new System.Workflow.Activities.StateActivity();
            this.SpeechServerSubMenu = new System.Workflow.Activities.StateActivity();
            this.NavigateToMainMenu = new System.Workflow.Activities.EventDrivenActivity();
            this.EndPhoneSession = new System.Workflow.Activities.EventDrivenActivity();
            // 
            // sequentialSubMenu_TransitionToSelf
            // 
            this.sequentialSubMenu_TransitionToSelf.Name = "sequentialSubMenu_TransitionToSelf";
            this.sequentialSubMenu_TransitionToSelf.TargetStateName = "SequentialSubMenu";
            // 
            // sequentialSubMenu_Button0PressedEventSink
            // 
            this.sequentialSubMenu_Button0PressedEventSink.EventName = "Button0Pressed";
            this.sequentialSubMenu_Button0PressedEventSink.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.sequentialSubMenu_Button0PressedEventSink.Name = "sequentialSubMenu_Button0PressedEventSink";
            activitybind1.Name = "Workflow1";
            activitybind1.Path = "PhoneEventSender";
            workflowparameterbinding1.ParameterName = "sender";
            workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind1)));
            this.sequentialSubMenu_Button0PressedEventSink.ParameterBindings.Add(workflowparameterbinding1);
            // 
            // sequentialSubMenu_TransitionToInfoSubMenu
            // 
            this.sequentialSubMenu_TransitionToInfoSubMenu.Name = "sequentialSubMenu_TransitionToInfoSubMenu";
            this.sequentialSubMenu_TransitionToInfoSubMenu.TargetStateName = "InfoSubMenu";
            // 
            // sequentialSubMenu_Button8PressedEventSink
            // 
            this.sequentialSubMenu_Button8PressedEventSink.EventName = "Button8Pressed";
            this.sequentialSubMenu_Button8PressedEventSink.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.sequentialSubMenu_Button8PressedEventSink.Name = "sequentialSubMenu_Button8PressedEventSink";
            activitybind2.Name = "Workflow1";
            activitybind2.Path = "PhoneEventSender";
            workflowparameterbinding2.ParameterName = "sender";
            workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind2)));
            this.sequentialSubMenu_Button8PressedEventSink.ParameterBindings.Add(workflowparameterbinding2);
            // 
            // sequentialSubMenu_SendSequentialTextToMenu
            // 
            this.sequentialSubMenu_SendSequentialTextToMenu.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.sequentialSubMenu_SendSequentialTextToMenu.MethodName = "SendMenuText";
            this.sequentialSubMenu_SendSequentialTextToMenu.Name = "sequentialSubMenu_SendSequentialTextToMenu";
            activitybind3.Name = "Workflow1";
            activitybind3.Path = "MenuText";
            workflowparameterbinding3.ParameterName = "menuText";
            workflowparameterbinding3.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind3)));
            this.sequentialSubMenu_SendSequentialTextToMenu.ParameterBindings.Add(workflowparameterbinding3);
            this.sequentialSubMenu_SendSequentialTextToMenu.MethodInvoking += new System.EventHandler(this.SetSequentialText);
            // 
            // stateMachineSubMenu_TransitionToSelf
            // 
            this.stateMachineSubMenu_TransitionToSelf.Name = "stateMachineSubMenu_TransitionToSelf";
            this.stateMachineSubMenu_TransitionToSelf.TargetStateName = "StateMachineSubMenu";
            // 
            // stateMachineSubMenu_Button0PressedEventSink
            // 
            this.stateMachineSubMenu_Button0PressedEventSink.EventName = "Button0Pressed";
            this.stateMachineSubMenu_Button0PressedEventSink.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.stateMachineSubMenu_Button0PressedEventSink.Name = "stateMachineSubMenu_Button0PressedEventSink";
            activitybind4.Name = "Workflow1";
            activitybind4.Path = "PhoneEventSender";
            workflowparameterbinding4.ParameterName = "sender";
            workflowparameterbinding4.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind4)));
            this.stateMachineSubMenu_Button0PressedEventSink.ParameterBindings.Add(workflowparameterbinding4);
            // 
            // stateMachineSubMenu_TransitionToInfoSubMenu
            // 
            this.stateMachineSubMenu_TransitionToInfoSubMenu.Name = "stateMachineSubMenu_TransitionToInfoSubMenu";
            this.stateMachineSubMenu_TransitionToInfoSubMenu.TargetStateName = "InfoSubMenu";
            // 
            // stateMachineSubMenu_Button8PressedEventSink
            // 
            this.stateMachineSubMenu_Button8PressedEventSink.EventName = "Button8Pressed";
            this.stateMachineSubMenu_Button8PressedEventSink.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.stateMachineSubMenu_Button8PressedEventSink.Name = "stateMachineSubMenu_Button8PressedEventSink";
            activitybind5.Name = "Workflow1";
            activitybind5.Path = "PhoneEventSender";
            workflowparameterbinding5.ParameterName = "sender";
            workflowparameterbinding5.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind5)));
            this.stateMachineSubMenu_Button8PressedEventSink.ParameterBindings.Add(workflowparameterbinding5);
            // 
            // stateMachineSubMenu_SendFSMTextToMenu
            // 
            this.stateMachineSubMenu_SendFSMTextToMenu.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.stateMachineSubMenu_SendFSMTextToMenu.MethodName = "SendMenuText";
            this.stateMachineSubMenu_SendFSMTextToMenu.Name = "stateMachineSubMenu_SendFSMTextToMenu";
            activitybind6.Name = "Workflow1";
            activitybind6.Path = "MenuText";
            workflowparameterbinding6.ParameterName = "menuText";
            workflowparameterbinding6.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind6)));
            this.stateMachineSubMenu_SendFSMTextToMenu.ParameterBindings.Add(workflowparameterbinding6);
            this.stateMachineSubMenu_SendFSMTextToMenu.MethodInvoking += new System.EventHandler(this.SetFSMText);
            // 
            // infoSubMenu_TransitionToStateMachineSubMenu
            // 
            this.infoSubMenu_TransitionToStateMachineSubMenu.Name = "infoSubMenu_TransitionToStateMachineSubMenu";
            this.infoSubMenu_TransitionToStateMachineSubMenu.TargetStateName = "StateMachineSubMenu";
            // 
            // infoSubMenu_Button2PressedEventSink
            // 
            this.infoSubMenu_Button2PressedEventSink.EventName = "Button2Pressed";
            this.infoSubMenu_Button2PressedEventSink.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.infoSubMenu_Button2PressedEventSink.Name = "infoSubMenu_Button2PressedEventSink";
            activitybind7.Name = "Workflow1";
            activitybind7.Path = "PhoneEventSender";
            workflowparameterbinding7.ParameterName = "sender";
            workflowparameterbinding7.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind7)));
            this.infoSubMenu_Button2PressedEventSink.ParameterBindings.Add(workflowparameterbinding7);
            // 
            // infoSubMenu_TransitionToSequentialSubMenu
            // 
            this.infoSubMenu_TransitionToSequentialSubMenu.Name = "infoSubMenu_TransitionToSequentialSubMenu";
            this.infoSubMenu_TransitionToSequentialSubMenu.TargetStateName = "SequentialSubMenu";
            // 
            // infoSubMenu_Button1PressedEventSink
            // 
            this.infoSubMenu_Button1PressedEventSink.EventName = "Button1Pressed";
            this.infoSubMenu_Button1PressedEventSink.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.infoSubMenu_Button1PressedEventSink.Name = "infoSubMenu_Button1PressedEventSink";
            activitybind8.Name = "Workflow1";
            activitybind8.Path = "PhoneEventSender";
            workflowparameterbinding8.ParameterName = "sender";
            workflowparameterbinding8.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind8)));
            this.infoSubMenu_Button1PressedEventSink.ParameterBindings.Add(workflowparameterbinding8);
            // 
            // infoSubMenu_SendInfoTextToMenu
            // 
            this.infoSubMenu_SendInfoTextToMenu.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.infoSubMenu_SendInfoTextToMenu.MethodName = "SendMenuText";
            this.infoSubMenu_SendInfoTextToMenu.Name = "infoSubMenu_SendInfoTextToMenu";
            activitybind9.Name = "Workflow1";
            activitybind9.Path = "MenuText";
            workflowparameterbinding9.ParameterName = "menuText";
            workflowparameterbinding9.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind9)));
            this.infoSubMenu_SendInfoTextToMenu.ParameterBindings.Add(workflowparameterbinding9);
            this.infoSubMenu_SendInfoTextToMenu.MethodInvoking += new System.EventHandler(this.SetInfoText);
            // 
            // WPFSubMenu_TransitionToSelf
            // 
            this.WPFSubMenu_TransitionToSelf.Name = "WPFSubMenu_TransitionToSelf";
            this.WPFSubMenu_TransitionToSelf.TargetStateName = "WPFSubMenu";
            // 
            // WPFSubMenu_Button0Pressed
            // 
            this.WPFSubMenu_Button0Pressed.EventName = "Button0Pressed";
            this.WPFSubMenu_Button0Pressed.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.WPFSubMenu_Button0Pressed.Name = "WPFSubMenu_Button0Pressed";
            activitybind10.Name = "Workflow1";
            activitybind10.Path = "PhoneEventSender";
            workflowparameterbinding10.ParameterName = "sender";
            workflowparameterbinding10.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind10)));
            this.WPFSubMenu_Button0Pressed.ParameterBindings.Add(workflowparameterbinding10);
            // 
            // WPFSubMenu_SendAvalonTextMenu
            // 
            this.WPFSubMenu_SendAvalonTextMenu.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.WPFSubMenu_SendAvalonTextMenu.MethodName = "SendMenuText";
            this.WPFSubMenu_SendAvalonTextMenu.Name = "WPFSubMenu_SendAvalonTextMenu";
            activitybind11.Name = "Workflow1";
            activitybind11.Path = "MenuText";
            workflowparameterbinding11.ParameterName = "menuText";
            workflowparameterbinding11.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind11)));
            this.WPFSubMenu_SendAvalonTextMenu.ParameterBindings.Add(workflowparameterbinding11);
            this.WPFSubMenu_SendAvalonTextMenu.MethodInvoking += new System.EventHandler(this.SetAvalonText);
            // 
            // mainMenu_TransitionToWPFSubMenu
            // 
            this.mainMenu_TransitionToWPFSubMenu.Name = "mainMenu_TransitionToWPFSubMenu";
            this.mainMenu_TransitionToWPFSubMenu.TargetStateName = "WPFSubMenu";
            // 
            // mainMenu_Button4PressedEventSink
            // 
            this.mainMenu_Button4PressedEventSink.EventName = "Button4Pressed";
            this.mainMenu_Button4PressedEventSink.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.mainMenu_Button4PressedEventSink.Name = "mainMenu_Button4PressedEventSink";
            activitybind12.Name = "Workflow1";
            activitybind12.Path = "PhoneEventSender";
            workflowparameterbinding12.ParameterName = "sender";
            workflowparameterbinding12.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind12)));
            this.mainMenu_Button4PressedEventSink.ParameterBindings.Add(workflowparameterbinding12);
            // 
            // mainMenu_TransitionToWCFSubMenu
            // 
            this.mainMenu_TransitionToWCFSubMenu.Name = "mainMenu_TransitionToWCFSubMenu";
            this.mainMenu_TransitionToWCFSubMenu.TargetStateName = "WCFSubMenu";
            // 
            // mainMenu_Button3PressedEventSink
            // 
            this.mainMenu_Button3PressedEventSink.EventName = "Button3Pressed";
            this.mainMenu_Button3PressedEventSink.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.mainMenu_Button3PressedEventSink.Name = "mainMenu_Button3PressedEventSink";
            activitybind13.Name = "Workflow1";
            activitybind13.Path = "PhoneEventSender";
            workflowparameterbinding13.ParameterName = "sender";
            workflowparameterbinding13.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind13)));
            this.mainMenu_Button3PressedEventSink.ParameterBindings.Add(workflowparameterbinding13);
            // 
            // mainMenu_TransitionToSpeechServerSubMenu
            // 
            this.mainMenu_TransitionToSpeechServerSubMenu.Name = "mainMenu_TransitionToSpeechServerSubMenu";
            this.mainMenu_TransitionToSpeechServerSubMenu.TargetStateName = "SpeechServerSubMenu";
            // 
            // mainMenu_Button2PressedEventSink
            // 
            this.mainMenu_Button2PressedEventSink.EventName = "Button2Pressed";
            this.mainMenu_Button2PressedEventSink.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.mainMenu_Button2PressedEventSink.Name = "mainMenu_Button2PressedEventSink";
            activitybind14.Name = "Workflow1";
            activitybind14.Path = "PhoneEventSender";
            workflowparameterbinding14.ParameterName = "sender";
            workflowparameterbinding14.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind14)));
            this.mainMenu_Button2PressedEventSink.ParameterBindings.Add(workflowparameterbinding14);
            // 
            // mainMenu_TransitionToInfoSubMenu
            // 
            this.mainMenu_TransitionToInfoSubMenu.Name = "mainMenu_TransitionToInfoSubMenu";
            this.mainMenu_TransitionToInfoSubMenu.TargetStateName = "InfoSubMenu";
            // 
            // mainMenu_Button1PressedEventSink
            // 
            this.mainMenu_Button1PressedEventSink.EventName = "Button1Pressed";
            this.mainMenu_Button1PressedEventSink.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.mainMenu_Button1PressedEventSink.Name = "mainMenu_Button1PressedEventSink";
            activitybind15.Name = "Workflow1";
            activitybind15.Path = "PhoneEventSender";
            workflowparameterbinding15.ParameterName = "sender";
            workflowparameterbinding15.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind15)));
            this.mainMenu_Button1PressedEventSink.ParameterBindings.Add(workflowparameterbinding15);
            // 
            // mainMenu_SendMainMenuTextToMenu
            // 
            this.mainMenu_SendMainMenuTextToMenu.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.mainMenu_SendMainMenuTextToMenu.MethodName = "SendMenuText";
            this.mainMenu_SendMainMenuTextToMenu.Name = "mainMenu_SendMainMenuTextToMenu";
            activitybind16.Name = "Workflow1";
            activitybind16.Path = "MenuText";
            workflowparameterbinding16.ParameterName = "menuText";
            workflowparameterbinding16.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind16)));
            this.mainMenu_SendMainMenuTextToMenu.ParameterBindings.Add(workflowparameterbinding16);
            this.mainMenu_SendMainMenuTextToMenu.MethodInvoking += new System.EventHandler(this.SetMainMenuText);
            // 
            // WCFSubMenu_TransitionToSelf
            // 
            this.WCFSubMenu_TransitionToSelf.Name = "WCFSubMenu_TransitionToSelf";
            this.WCFSubMenu_TransitionToSelf.TargetStateName = "WCFSubMenu";
            // 
            // WCFSubMenu_Button0PressedEventSink
            // 
            this.WCFSubMenu_Button0PressedEventSink.EventName = "Button0Pressed";
            this.WCFSubMenu_Button0PressedEventSink.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.WCFSubMenu_Button0PressedEventSink.Name = "WCFSubMenu_Button0PressedEventSink";
            activitybind17.Name = "Workflow1";
            activitybind17.Path = "PhoneEventSender";
            workflowparameterbinding17.ParameterName = "sender";
            workflowparameterbinding17.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind17)));
            this.WCFSubMenu_Button0PressedEventSink.ParameterBindings.Add(workflowparameterbinding17);
            // 
            // WCFSubMenu_SendIndigoTextMenu
            // 
            this.WCFSubMenu_SendIndigoTextMenu.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.WCFSubMenu_SendIndigoTextMenu.MethodName = "SendMenuText";
            this.WCFSubMenu_SendIndigoTextMenu.Name = "WCFSubMenu_SendIndigoTextMenu";
            activitybind18.Name = "Workflow1";
            activitybind18.Path = "MenuText";
            workflowparameterbinding18.ParameterName = "menuText";
            workflowparameterbinding18.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind18)));
            this.WCFSubMenu_SendIndigoTextMenu.ParameterBindings.Add(workflowparameterbinding18);
            this.WCFSubMenu_SendIndigoTextMenu.MethodInvoking += new System.EventHandler(this.SetIndigoText);
            // 
            // speechServerSubMenu_TransitionToSpeechServerSubMenu
            // 
            this.speechServerSubMenu_TransitionToSpeechServerSubMenu.Name = "speechServerSubMenu_TransitionToSpeechServerSubMenu";
            this.speechServerSubMenu_TransitionToSpeechServerSubMenu.TargetStateName = "SpeechServerSubMenu";
            // 
            // speechServerSubMenu_Button0PressedEventSink
            // 
            this.speechServerSubMenu_Button0PressedEventSink.EventName = "Button0Pressed";
            this.speechServerSubMenu_Button0PressedEventSink.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.speechServerSubMenu_Button0PressedEventSink.Name = "speechServerSubMenu_Button0PressedEventSink";
            activitybind19.Name = "Workflow1";
            activitybind19.Path = "PhoneEventSender";
            workflowparameterbinding19.ParameterName = "sender";
            workflowparameterbinding19.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind19)));
            this.speechServerSubMenu_Button0PressedEventSink.ParameterBindings.Add(workflowparameterbinding19);
            // 
            // speechServerSubMenu_SendSpeechServerTextToMenu
            // 
            this.speechServerSubMenu_SendSpeechServerTextToMenu.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.speechServerSubMenu_SendSpeechServerTextToMenu.MethodName = "SendMenuText";
            this.speechServerSubMenu_SendSpeechServerTextToMenu.Name = "speechServerSubMenu_SendSpeechServerTextToMenu";
            activitybind20.Name = "Workflow1";
            activitybind20.Path = "MenuText";
            workflowparameterbinding20.ParameterName = "menuText";
            workflowparameterbinding20.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind20)));
            this.speechServerSubMenu_SendSpeechServerTextToMenu.ParameterBindings.Add(workflowparameterbinding20);
            this.speechServerSubMenu_SendSpeechServerTextToMenu.MethodInvoking += new System.EventHandler(this.SetSpeechServerText);
            // 
            // SeqButton0Event
            // 
            this.SeqButton0Event.Activities.Add(this.sequentialSubMenu_Button0PressedEventSink);
            this.SeqButton0Event.Activities.Add(this.sequentialSubMenu_TransitionToSelf);
            this.SeqButton0Event.Name = "SeqButton0Event";
            // 
            // SeqButton8Event
            // 
            this.SeqButton8Event.Activities.Add(this.sequentialSubMenu_Button8PressedEventSink);
            this.SeqButton8Event.Activities.Add(this.sequentialSubMenu_TransitionToInfoSubMenu);
            this.SeqButton8Event.Name = "SeqButton8Event";
            // 
            // BroadCastSequentialInformation
            // 
            this.BroadCastSequentialInformation.Activities.Add(this.sequentialSubMenu_SendSequentialTextToMenu);
            this.BroadCastSequentialInformation.Name = "BroadCastSequentialInformation";
            // 
            // FSMButton0Event
            // 
            this.FSMButton0Event.Activities.Add(this.stateMachineSubMenu_Button0PressedEventSink);
            this.FSMButton0Event.Activities.Add(this.stateMachineSubMenu_TransitionToSelf);
            this.FSMButton0Event.Name = "FSMButton0Event";
            // 
            // FSMButton8Event
            // 
            this.FSMButton8Event.Activities.Add(this.stateMachineSubMenu_Button8PressedEventSink);
            this.FSMButton8Event.Activities.Add(this.stateMachineSubMenu_TransitionToInfoSubMenu);
            this.FSMButton8Event.Name = "FSMButton8Event";
            // 
            // BroadCastStateMachineInformation
            // 
            this.BroadCastStateMachineInformation.Activities.Add(this.stateMachineSubMenu_SendFSMTextToMenu);
            this.BroadCastStateMachineInformation.Name = "BroadCastStateMachineInformation";
            // 
            // InfoButton2Event
            // 
            this.InfoButton2Event.Activities.Add(this.infoSubMenu_Button2PressedEventSink);
            this.InfoButton2Event.Activities.Add(this.infoSubMenu_TransitionToStateMachineSubMenu);
            this.InfoButton2Event.Name = "InfoButton2Event";
            // 
            // InfoButton1Event
            // 
            this.InfoButton1Event.Activities.Add(this.infoSubMenu_Button1PressedEventSink);
            this.InfoButton1Event.Activities.Add(this.infoSubMenu_TransitionToSequentialSubMenu);
            this.InfoButton1Event.Name = "InfoButton1Event";
            // 
            // BroadcastInformation
            // 
            this.BroadcastInformation.Activities.Add(this.infoSubMenu_SendInfoTextToMenu);
            this.BroadcastInformation.Name = "BroadcastInformation";
            // 
            // WPFButton0Event
            // 
            this.WPFButton0Event.Activities.Add(this.WPFSubMenu_Button0Pressed);
            this.WPFButton0Event.Activities.Add(this.WPFSubMenu_TransitionToSelf);
            this.WPFButton0Event.Name = "WPFButton0Event";
            // 
            // BroadcastAvalonInformation
            // 
            this.BroadcastAvalonInformation.Activities.Add(this.WPFSubMenu_SendAvalonTextMenu);
            this.BroadcastAvalonInformation.Name = "BroadcastAvalonInformation";
            // 
            // Button4Event
            // 
            this.Button4Event.Activities.Add(this.mainMenu_Button4PressedEventSink);
            this.Button4Event.Activities.Add(this.mainMenu_TransitionToWPFSubMenu);
            this.Button4Event.Name = "Button4Event";
            // 
            // Button3Event
            // 
            this.Button3Event.Activities.Add(this.mainMenu_Button3PressedEventSink);
            this.Button3Event.Activities.Add(this.mainMenu_TransitionToWCFSubMenu);
            this.Button3Event.Name = "Button3Event";
            // 
            // Button2Event
            // 
            this.Button2Event.Activities.Add(this.mainMenu_Button2PressedEventSink);
            this.Button2Event.Activities.Add(this.mainMenu_TransitionToSpeechServerSubMenu);
            this.Button2Event.Name = "Button2Event";
            // 
            // Button1Event
            // 
            this.Button1Event.Activities.Add(this.mainMenu_Button1PressedEventSink);
            this.Button1Event.Activities.Add(this.mainMenu_TransitionToInfoSubMenu);
            this.Button1Event.Name = "Button1Event";
            // 
            // BroadcastInitialStatement
            // 
            this.BroadcastInitialStatement.Activities.Add(this.mainMenu_SendMainMenuTextToMenu);
            this.BroadcastInitialStatement.Name = "BroadcastInitialStatement";
            // 
            // WCFButton0Event
            // 
            this.WCFButton0Event.Activities.Add(this.WCFSubMenu_Button0PressedEventSink);
            this.WCFButton0Event.Activities.Add(this.WCFSubMenu_TransitionToSelf);
            this.WCFButton0Event.Name = "WCFButton0Event";
            // 
            // broadcastIndigoInformation
            // 
            this.broadcastIndigoInformation.Activities.Add(this.WCFSubMenu_SendIndigoTextMenu);
            this.broadcastIndigoInformation.Name = "broadcastIndigoInformation";
            // 
            // MSSButton0Event
            // 
            this.MSSButton0Event.Activities.Add(this.speechServerSubMenu_Button0PressedEventSink);
            this.MSSButton0Event.Activities.Add(this.speechServerSubMenu_TransitionToSpeechServerSubMenu);
            this.MSSButton0Event.Name = "MSSButton0Event";
            // 
            // BroadCastSpeechServerInformation
            // 
            this.BroadCastSpeechServerInformation.Activities.Add(this.speechServerSubMenu_SendSpeechServerTextToMenu);
            this.BroadCastSpeechServerInformation.Name = "BroadCastSpeechServerInformation";
            // 
            // workflow_TransitionToMainMenu
            // 
            this.workflow_TransitionToMainMenu.Name = "workflow_TransitionToMainMenu";
            this.workflow_TransitionToMainMenu.TargetStateName = "MainMenu";
            // 
            // workflow_Button9PressedEventSink
            // 
            this.workflow_Button9PressedEventSink.EventName = "Button9Pressed";
            this.workflow_Button9PressedEventSink.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.workflow_Button9PressedEventSink.Name = "workflow_Button9PressedEventSink";
            activitybind21.Name = "Workflow1";
            activitybind21.Path = "PhoneEventSender";
            workflowparameterbinding21.ParameterName = "sender";
            workflowparameterbinding21.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind21)));
            this.workflow_Button9PressedEventSink.ParameterBindings.Add(workflowparameterbinding21);
            // 
            // workflow_TransitionToEndSession
            // 
            this.workflow_TransitionToEndSession.Name = "workflow_TransitionToEndSession";
            this.workflow_TransitionToEndSession.TargetStateName = "EndSession";
            // 
            // workflow_SendEndSessionTextToMenu
            // 
            this.workflow_SendEndSessionTextToMenu.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.workflow_SendEndSessionTextToMenu.MethodName = "SendMenuText";
            this.workflow_SendEndSessionTextToMenu.Name = "workflow_SendEndSessionTextToMenu";
            activitybind22.Name = "Workflow1";
            activitybind22.Path = "MenuText";
            workflowparameterbinding22.ParameterName = "menuText";
            workflowparameterbinding22.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind22)));
            this.workflow_SendEndSessionTextToMenu.ParameterBindings.Add(workflowparameterbinding22);
            this.workflow_SendEndSessionTextToMenu.MethodInvoking += new System.EventHandler(this.SetEndSessionText);
            // 
            // workflow_ButtonStarPressentEventSink
            // 
            this.workflow_ButtonStarPressentEventSink.EventName = "ButtonStarPressed";
            this.workflow_ButtonStarPressentEventSink.InterfaceType = typeof(Microsoft.Samples.Workflow.SpeechApplication.ISpeechService);
            this.workflow_ButtonStarPressentEventSink.Name = "workflow_ButtonStarPressentEventSink";
            activitybind23.Name = "Workflow1";
            activitybind23.Path = "PhoneEventSender";
            workflowparameterbinding23.ParameterName = "sender";
            workflowparameterbinding23.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind23)));
            this.workflow_ButtonStarPressentEventSink.ParameterBindings.Add(workflowparameterbinding23);
            // 
            // SequentialSubMenu
            // 
            this.SequentialSubMenu.Activities.Add(this.BroadCastSequentialInformation);
            this.SequentialSubMenu.Activities.Add(this.SeqButton8Event);
            this.SequentialSubMenu.Activities.Add(this.SeqButton0Event);
            this.SequentialSubMenu.Name = "SequentialSubMenu";
            // 
            // StateMachineSubMenu
            // 
            this.StateMachineSubMenu.Activities.Add(this.BroadCastStateMachineInformation);
            this.StateMachineSubMenu.Activities.Add(this.FSMButton8Event);
            this.StateMachineSubMenu.Activities.Add(this.FSMButton0Event);
            this.StateMachineSubMenu.Name = "StateMachineSubMenu";
            // 
            // InfoSubMenu
            // 
            this.InfoSubMenu.Activities.Add(this.BroadcastInformation);
            this.InfoSubMenu.Activities.Add(this.InfoButton1Event);
            this.InfoSubMenu.Activities.Add(this.InfoButton2Event);
            this.InfoSubMenu.Name = "InfoSubMenu";
            // 
            // WPFSubMenu
            // 
            this.WPFSubMenu.Activities.Add(this.BroadcastAvalonInformation);
            this.WPFSubMenu.Activities.Add(this.WPFButton0Event);
            this.WPFSubMenu.Name = "WPFSubMenu";
            // 
            // EndSession
            // 
            this.EndSession.Name = "EndSession";
            // 
            // MainMenu
            // 
            this.MainMenu.Activities.Add(this.BroadcastInitialStatement);
            this.MainMenu.Activities.Add(this.Button1Event);
            this.MainMenu.Activities.Add(this.Button2Event);
            this.MainMenu.Activities.Add(this.Button3Event);
            this.MainMenu.Activities.Add(this.Button4Event);
            this.MainMenu.Name = "MainMenu";
            // 
            // WCFSubMenu
            // 
            this.WCFSubMenu.Activities.Add(this.broadcastIndigoInformation);
            this.WCFSubMenu.Activities.Add(this.WCFButton0Event);
            this.WCFSubMenu.Name = "WCFSubMenu";
            // 
            // SpeechServerSubMenu
            // 
            this.SpeechServerSubMenu.Activities.Add(this.BroadCastSpeechServerInformation);
            this.SpeechServerSubMenu.Activities.Add(this.MSSButton0Event);
            this.SpeechServerSubMenu.Name = "SpeechServerSubMenu";
            // 
            // NavigateToMainMenu
            // 
            this.NavigateToMainMenu.Activities.Add(this.workflow_Button9PressedEventSink);
            this.NavigateToMainMenu.Activities.Add(this.workflow_TransitionToMainMenu);
            this.NavigateToMainMenu.Name = "NavigateToMainMenu";
            // 
            // EndPhoneSession
            // 
            this.EndPhoneSession.Activities.Add(this.workflow_ButtonStarPressentEventSink);
            this.EndPhoneSession.Activities.Add(this.workflow_SendEndSessionTextToMenu);
            this.EndPhoneSession.Activities.Add(this.workflow_TransitionToEndSession);
            this.EndPhoneSession.Name = "EndPhoneSession";
            // 
            // Workflow1
            // 
            this.Activities.Add(this.EndPhoneSession);
            this.Activities.Add(this.NavigateToMainMenu);
            this.Activities.Add(this.SpeechServerSubMenu);
            this.Activities.Add(this.WCFSubMenu);
            this.Activities.Add(this.MainMenu);
            this.Activities.Add(this.EndSession);
            this.Activities.Add(this.WPFSubMenu);
            this.Activities.Add(this.InfoSubMenu);
            this.Activities.Add(this.StateMachineSubMenu);
            this.Activities.Add(this.SequentialSubMenu);
            this.CompletedStateName = "EndSession";
            this.DynamicUpdateCondition = null;
            this.InitialStateName = "MainMenu";
            this.Name = "Workflow1";
            this.CanModifyActivities = false;

        }

        #endregion

        private EventDrivenActivity Button4Event;
        private HandleExternalEventActivity mainMenu_Button4PressedEventSink;
        private SetStateActivity mainMenu_TransitionToWPFSubMenu;
        private StateActivity WPFSubMenu;
        private StateInitializationActivity BroadcastAvalonInformation;
        private CallExternalMethodActivity WPFSubMenu_SendAvalonTextMenu;
        private EventDrivenActivity WPFButton0Event;
        private HandleExternalEventActivity WPFSubMenu_Button0Pressed;
        private SetStateActivity WPFSubMenu_TransitionToSelf;
        private CallExternalMethodActivity workflow_SendEndSessionTextToMenu;
        private StateActivity InfoSubMenu;
        private StateInitializationActivity BroadcastInformation;
        private CallExternalMethodActivity infoSubMenu_SendInfoTextToMenu;
        private EventDrivenActivity InfoButton1Event;
        private HandleExternalEventActivity infoSubMenu_Button1PressedEventSink;
        private SetStateActivity infoSubMenu_TransitionToSequentialSubMenu;
        private EventDrivenActivity InfoButton2Event;
        private HandleExternalEventActivity infoSubMenu_Button2PressedEventSink;
        private SetStateActivity infoSubMenu_TransitionToStateMachineSubMenu;
        private CallExternalMethodActivity stateMachineSubMenu_SendFSMTextToMenu;
        private HandleExternalEventActivity stateMachineSubMenu_Button8PressedEventSink;
        private HandleExternalEventActivity stateMachineSubMenu_Button0PressedEventSink;
        private CallExternalMethodActivity sequentialSubMenu_SendSequentialTextToMenu;
        private HandleExternalEventActivity sequentialSubMenu_Button8PressedEventSink;
        private HandleExternalEventActivity sequentialSubMenu_Button0PressedEventSink;
        private CallExternalMethodActivity WCFSubMenu_SendIndigoTextMenu;
        private CallExternalMethodActivity speechServerSubMenu_SendSpeechServerTextToMenu;
        private HandleExternalEventActivity speechServerSubMenu_Button0PressedEventSink;
        private HandleExternalEventActivity WCFSubMenu_Button0PressedEventSink;
        private HandleExternalEventActivity workflow_Button9PressedEventSink;
        private HandleExternalEventActivity workflow_ButtonStarPressentEventSink;
        private HandleExternalEventActivity mainMenu_Button2PressedEventSink;
        private HandleExternalEventActivity mainMenu_Button3PressedEventSink;
        private CallExternalMethodActivity mainMenu_SendMainMenuTextToMenu;
        private HandleExternalEventActivity mainMenu_Button1PressedEventSink;
        private StateActivity SequentialSubMenu;
        private StateInitializationActivity BroadCastSequentialInformation;
        private EventDrivenActivity SeqButton8Event;
        private SetStateActivity sequentialSubMenu_TransitionToInfoSubMenu;
        private EventDrivenActivity SeqButton0Event;
        private SetStateActivity sequentialSubMenu_TransitionToSelf;
        private StateActivity SpeechServerSubMenu;
        private StateInitializationActivity BroadCastSpeechServerInformation;
        private EventDrivenActivity MSSButton0Event;
        private SetStateActivity speechServerSubMenu_TransitionToSpeechServerSubMenu;
        private EventDrivenActivity WCFButton0Event;
        private SetStateActivity WCFSubMenu_TransitionToSelf;
        private SetStateActivity stateMachineSubMenu_TransitionToSelf;
        private EventDrivenActivity FSMButton0Event;
        private EventDrivenActivity FSMButton8Event;
        private EventDrivenActivity Button1Event;
        private EventDrivenActivity Button2Event;
        private EventDrivenActivity Button3Event;
        private SetStateActivity mainMenu_TransitionToSpeechServerSubMenu;
        private StateActivity WCFSubMenu;
        private SetStateActivity mainMenu_TransitionToWCFSubMenu;
        private StateInitializationActivity BroadCastStateMachineInformation;
        private StateInitializationActivity broadcastIndigoInformation;
        private StateActivity StateMachineSubMenu;
        private EventDrivenActivity EndPhoneSession;
        private SetStateActivity workflow_TransitionToEndSession;
        private SetStateActivity workflow_TransitionToMainMenu;
        private StateInitializationActivity BroadcastInitialStatement;
        private EventDrivenActivity NavigateToMainMenu;
        private StateActivity EndSession;
        private SetStateActivity stateMachineSubMenu_TransitionToInfoSubMenu;
        private SetStateActivity mainMenu_TransitionToInfoSubMenu;
        private StateActivity MainMenu;

































































































    }
}
