using System;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Collections;
using System.Drawing;
using System.Reflection;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Serialization;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.Runtime;
using System.Workflow.Activities;
using System.Workflow.Activities.Rules;

namespace Microsoft.Samples.ClassRegistration
{
	partial class RegistrationWorkflow
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
            System.Workflow.Activities.Rules.RuleConditionReference ruleconditionreference1 = new System.Workflow.Activities.Rules.RuleConditionReference();
            System.Workflow.Activities.Rules.RuleConditionReference ruleconditionreference2 = new System.Workflow.Activities.Rules.RuleConditionReference();
            this.updateRegistrationStatusRejected = new System.Workflow.Activities.CodeActivity();
            this.RejectedEvent = new System.Workflow.Activities.HandleExternalEventActivity();
            this.updateRegistrationStatusApproved = new System.Workflow.Activities.CodeActivity();
            this.ApprovedEvent = new System.Workflow.Activities.HandleExternalEventActivity();
            this.registrarRejected = new System.Workflow.Activities.EventDrivenActivity();
            this.registrarApproved = new System.Workflow.Activities.EventDrivenActivity();
            this.listenForRegistrarApproval = new System.Workflow.Activities.ListenActivity();
            this.updateRegistrationStatusWaitingForApproval = new System.Workflow.Activities.CodeActivity();
            this.updateRegistrationStatusWaitlisted = new System.Workflow.Activities.CodeActivity();
            this.notifyStudentClassFull = new System.Workflow.Activities.CodeActivity();
            this.classNotFull = new System.Workflow.Activities.IfElseBranchActivity();
            this.classFull = new System.Workflow.Activities.IfElseBranchActivity();
            this.notifyStudentOfSessionStatus = new System.Workflow.Activities.CodeActivity();
            this.addRegistrationToDatabase = new System.Workflow.Activities.CodeActivity();
            this.ifClassFull = new System.Workflow.Activities.IfElseActivity();
            this.notifyStudentAlreadyRegistered = new System.Workflow.Activities.CodeActivity();
            this.studentNotRegistered = new System.Workflow.Activities.IfElseBranchActivity();
            this.studentRegistered = new System.Workflow.Activities.IfElseBranchActivity();
            this.ifStudentAlreadyRegistered = new System.Workflow.Activities.IfElseActivity();
            this.validateRegistration = new System.Workflow.Activities.CodeActivity();
            // 
            // updateRegistrationStatusRejected
            // 
            this.updateRegistrationStatusRejected.Name = "updateRegistrationStatusRejected";
            this.updateRegistrationStatusRejected.ExecuteCode += new System.EventHandler(this.updateRegistrationStatusRejected_ExecuteCode);
            // 
            // RejectedEvent
            // 
            this.RejectedEvent.EventName = "Rejected";
            this.RejectedEvent.InterfaceType = typeof(Microsoft.Samples.ClassRegistration.IRegistrarService);
            this.RejectedEvent.Name = "RejectedEvent";
            // 
            // updateRegistrationStatusApproved
            // 
            this.updateRegistrationStatusApproved.Name = "updateRegistrationStatusApproved";
            this.updateRegistrationStatusApproved.ExecuteCode += new System.EventHandler(this.updateRegistrationStatusApproved_ExecuteCode);
            // 
            // ApprovedEvent
            // 
            this.ApprovedEvent.EventName = "Approved";
            this.ApprovedEvent.InterfaceType = typeof(Microsoft.Samples.ClassRegistration.IRegistrarService);
            this.ApprovedEvent.Name = "ApprovedEvent";
            // 
            // registrarRejected
            // 
            this.registrarRejected.Activities.Add(this.RejectedEvent);
            this.registrarRejected.Activities.Add(this.updateRegistrationStatusRejected);
            this.registrarRejected.Name = "registrarRejected";
            // 
            // registrarApproved
            // 
            this.registrarApproved.Activities.Add(this.ApprovedEvent);
            this.registrarApproved.Activities.Add(this.updateRegistrationStatusApproved);
            this.registrarApproved.Name = "registrarApproved";
            // 
            // listenForRegistrarApproval
            // 
            this.listenForRegistrarApproval.Activities.Add(this.registrarApproved);
            this.listenForRegistrarApproval.Activities.Add(this.registrarRejected);
            this.listenForRegistrarApproval.Name = "listenForRegistrarApproval";
            // 
            // updateRegistrationStatusWaitingForApproval
            // 
            this.updateRegistrationStatusWaitingForApproval.Name = "updateRegistrationStatusWaitingForApproval";
            this.updateRegistrationStatusWaitingForApproval.ExecuteCode += new System.EventHandler(this.updateRegistrationStatusWaitingForApproval_ExecuteCode);
            // 
            // updateRegistrationStatusWaitlisted
            // 
            this.updateRegistrationStatusWaitlisted.Name = "updateRegistrationStatusWaitlisted";
            this.updateRegistrationStatusWaitlisted.ExecuteCode += new System.EventHandler(this.updateRegistrationStatusWaitlisted_ExecuteCode);
            // 
            // notifyStudentClassFull
            // 
            this.notifyStudentClassFull.Name = "notifyStudentClassFull";
            this.notifyStudentClassFull.ExecuteCode += new System.EventHandler(this.notifyStudentClassFull_ExecuteCode);
            // 
            // classNotFull
            // 
            this.classNotFull.Activities.Add(this.updateRegistrationStatusWaitingForApproval);
            this.classNotFull.Activities.Add(this.listenForRegistrarApproval);
            this.classNotFull.Name = "classNotFull";
            // 
            // classFull
            // 
            this.classFull.Activities.Add(this.notifyStudentClassFull);
            this.classFull.Activities.Add(this.updateRegistrationStatusWaitlisted);
            ruleconditionreference1.ConditionName = "IsClassFullCondition";
            this.classFull.Condition = ruleconditionreference1;
            this.classFull.Name = "classFull";
            // 
            // notifyStudentOfSessionStatus
            // 
            this.notifyStudentOfSessionStatus.Name = "notifyStudentOfSessionStatus";
            this.notifyStudentOfSessionStatus.ExecuteCode += new System.EventHandler(this.notifyStudentOfSessionStatus_ExecuteCode);
            // 
            // addRegistrationToDatabase
            // 
            this.addRegistrationToDatabase.Name = "addRegistrationToDatabase";
            this.addRegistrationToDatabase.ExecuteCode += new System.EventHandler(this.addRegistrationToDatabase_ExecuteCode);
            // 
            // ifClassFull
            // 
            this.ifClassFull.Activities.Add(this.classFull);
            this.ifClassFull.Activities.Add(this.classNotFull);
            this.ifClassFull.Name = "ifClassFull";
            // 
            // notifyStudentAlreadyRegistered
            // 
            this.notifyStudentAlreadyRegistered.Name = "notifyStudentAlreadyRegistered";
            this.notifyStudentAlreadyRegistered.ExecuteCode += new System.EventHandler(this.notifyStudentAlreadyRegistered_ExecuteCode);
            // 
            // studentNotRegistered
            // 
            this.studentNotRegistered.Activities.Add(this.ifClassFull);
            this.studentNotRegistered.Activities.Add(this.addRegistrationToDatabase);
            this.studentNotRegistered.Activities.Add(this.notifyStudentOfSessionStatus);
            this.studentNotRegistered.Name = "studentNotRegistered";
            // 
            // studentRegistered
            // 
            this.studentRegistered.Activities.Add(this.notifyStudentAlreadyRegistered);
            ruleconditionreference2.ConditionName = "IsSessionRegistered";
            this.studentRegistered.Condition = ruleconditionreference2;
            this.studentRegistered.Name = "studentRegistered";
            // 
            // ifStudentAlreadyRegistered
            // 
            this.ifStudentAlreadyRegistered.Activities.Add(this.studentRegistered);
            this.ifStudentAlreadyRegistered.Activities.Add(this.studentNotRegistered);
            this.ifStudentAlreadyRegistered.Name = "ifStudentAlreadyRegistered";
            // 
            // validateRegistration
            // 
            this.validateRegistration.Name = "validateRegistration";
            this.validateRegistration.ExecuteCode += new System.EventHandler(this.validateRegistration_ExecuteCode);
            // 
            // RegistrationWorkflow
            // 
            this.Activities.Add(this.validateRegistration);
            this.Activities.Add(this.ifStudentAlreadyRegistered);
            this.Name = "RegistrationWorkflow";
            this.CanModifyActivities = false;

		}

		#endregion

        private CodeActivity updateRegistrationStatusWaitingForApproval;
        private CodeActivity notifyStudentOfSessionStatus;
        private CodeActivity updateRegistrationStatusRejected;
        private EventDrivenActivity registrarRejected;
        private EventDrivenActivity registrarApproved;
        private ListenActivity listenForRegistrarApproval;
        private HandleExternalEventActivity ApprovedEvent;
        private HandleExternalEventActivity RejectedEvent;
        private IfElseBranchActivity studentNotRegistered;
        private IfElseBranchActivity studentRegistered;
        private IfElseActivity ifStudentAlreadyRegistered;
        private CodeActivity notifyStudentAlreadyRegistered;
        private CodeActivity notifyStudentClassFull;
        private CodeActivity addRegistrationToDatabase;
        private IfElseBranchActivity classNotFull;
        private IfElseBranchActivity classFull;
        private IfElseActivity ifClassFull;
        private CodeActivity updateRegistrationStatusWaitlisted;
        private CodeActivity updateRegistrationStatusApproved;
        private CodeActivity validateRegistration;






































































    }
}
