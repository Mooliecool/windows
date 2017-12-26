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

namespace Microsoft.Samples.Workflow.WorkflowReflectionUtility.Dialogs
{
    partial class EvaluateRuleDialog
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.ruleEvaluationPanel1 = new Microsoft.Samples.Workflow.WorkflowReflectionUtility.Components.RuleEvaluatorPanel();
            this.SuspendLayout();
            // 
            // ruleEvaluationPanel1
            // 
            this.ruleEvaluationPanel1.Location = new System.Drawing.Point(12, 12);
            this.ruleEvaluationPanel1.Name = "ruleEvaluationPanel1";
            this.ruleEvaluationPanel1.Size = new System.Drawing.Size(174, 67);
            this.ruleEvaluationPanel1.TabIndex = 0;
            // 
            // EvaluateRuleDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(505, 100);
            this.Controls.Add(this.ruleEvaluationPanel1);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "EvaluateRuleDialog";
            this.Text = "EvaluateRuleDialog";
            this.ResumeLayout(false);

        }

        #endregion

        private Microsoft.Samples.Workflow.WorkflowReflectionUtility.Components.RuleEvaluatorPanel ruleEvaluationPanel1;

    }
}