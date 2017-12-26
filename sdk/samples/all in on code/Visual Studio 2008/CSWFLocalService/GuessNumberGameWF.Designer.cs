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

namespace CSWFLocalService
{
    partial class GuessNumberGameWF {
        #region Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCode]
        private void InitializeComponent() {
            this.CanModifyActivities = true;
            System.Workflow.ComponentModel.ActivityBind activitybind1 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding1 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.Activities.Rules.RuleConditionReference ruleconditionreference1 = new System.Workflow.Activities.Rules.RuleConditionReference();
            System.Workflow.ComponentModel.ActivityBind activitybind2 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding2 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            this.callExternalMethodActivity2 = new System.Workflow.Activities.CallExternalMethodActivity();
            this.codeActivity_GameCor = new System.Workflow.Activities.CodeActivity();
            this.handleExternalEventActivity1 = new System.Workflow.Activities.HandleExternalEventActivity();
            this.sequenceActivity1 = new System.Workflow.Activities.SequenceActivity();
            this.whileActivity1 = new System.Workflow.Activities.WhileActivity();
            this.callExternalMethodActivity1 = new System.Workflow.Activities.CallExternalMethodActivity();
            // 
            // callExternalMethodActivity2
            // 
            this.callExternalMethodActivity2.InterfaceType = typeof(CSWFLocalService.IGuessNumberGameService);
            this.callExternalMethodActivity2.MethodName = "MessageFromWFToLocalService";
            this.callExternalMethodActivity2.Name = "callExternalMethodActivity2";
            activitybind1.Name = "GuessNumberGameWF";
            activitybind1.Path = "OutMessage";
            workflowparameterbinding1.ParameterName = "message";
            workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind1)));
            this.callExternalMethodActivity2.ParameterBindings.Add(workflowparameterbinding1);
            // 
            // codeActivity_GameCor
            // 
            this.codeActivity_GameCor.Name = "codeActivity_GameCor";
            this.codeActivity_GameCor.ExecuteCode += new System.EventHandler(this.codeActivity_GameCor_ExecuteCode);
            // 
            // handleExternalEventActivity1
            // 
            this.handleExternalEventActivity1.EventName = "MessageFromHostToWFEvent";
            this.handleExternalEventActivity1.InterfaceType = typeof(CSWFLocalService.IGuessNumberGameService);
            this.handleExternalEventActivity1.Name = "handleExternalEventActivity1";
            this.handleExternalEventActivity1.Invoked += new System.EventHandler<System.Workflow.Activities.ExternalDataEventArgs>(this.handleExternalEventActivity1_Invoked);
            // 
            // sequenceActivity1
            // 
            this.sequenceActivity1.Activities.Add(this.handleExternalEventActivity1);
            this.sequenceActivity1.Activities.Add(this.codeActivity_GameCor);
            this.sequenceActivity1.Activities.Add(this.callExternalMethodActivity2);
            this.sequenceActivity1.Name = "sequenceActivity1";
            // 
            // whileActivity1
            // 
            this.whileActivity1.Activities.Add(this.sequenceActivity1);
            ruleconditionreference1.ConditionName = "CheckIfGuessRight";
            this.whileActivity1.Condition = ruleconditionreference1;
            this.whileActivity1.Name = "whileActivity1";
            // 
            // callExternalMethodActivity1
            // 
            this.callExternalMethodActivity1.InterfaceType = typeof(CSWFLocalService.IGuessNumberGameService);
            this.callExternalMethodActivity1.MethodName = "MessageFromWFToLocalService";
            this.callExternalMethodActivity1.Name = "callExternalMethodActivity1";
            activitybind2.Name = "GuessNumberGameWF";
            activitybind2.Path = "OutMessage";
            workflowparameterbinding2.ParameterName = "message";
            workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind2)));
            this.callExternalMethodActivity1.ParameterBindings.Add(workflowparameterbinding2);
            // 
            // GuessNumberGameWF
            // 
            this.Activities.Add(this.callExternalMethodActivity1);
            this.Activities.Add(this.whileActivity1);
            this.Name = "GuessNumberGameWF";
            this.CanModifyActivities = false;

        }

        #endregion

        private CodeActivity codeActivity_GameCor;
        private CallExternalMethodActivity callExternalMethodActivity2;
        private HandleExternalEventActivity handleExternalEventActivity1;
        private SequenceActivity sequenceActivity1;
        private WhileActivity whileActivity1;
        private CallExternalMethodActivity callExternalMethodActivity1;
    }
}
