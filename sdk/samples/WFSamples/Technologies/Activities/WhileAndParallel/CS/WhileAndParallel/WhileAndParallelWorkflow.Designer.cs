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
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.Workflow.WhileAndParallel
{
    public sealed partial class WhileAndParallelWorkflow
    {
        #region Designer generated code
        
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            System.Workflow.Activities.CodeCondition codecondition1 = new System.Workflow.Activities.CodeCondition();
            this.WhileLoop = new System.Workflow.Activities.WhileActivity();
            this.Parallel = new System.Workflow.Activities.ParallelActivity();
            this.Sequence1 = new System.Workflow.Activities.SequenceActivity();
            this.Sequence2 = new System.Workflow.Activities.SequenceActivity();
            this.ConsoleMessage1 = new System.Workflow.Activities.CodeActivity();
            this.ConsoleMessage2 = new System.Workflow.Activities.CodeActivity();
            // 
            // WhileLoop
            // 
            this.WhileLoop.Activities.Add(this.Parallel);
            codecondition1.Condition += new System.EventHandler<System.Workflow.Activities.ConditionalEventArgs>(this.WhileCondition);
            this.WhileLoop.Condition = codecondition1;
            this.WhileLoop.Name = "WhileLoop";
            // 
            // Parallel
            // 
            this.Parallel.Activities.Add(this.Sequence1);
            this.Parallel.Activities.Add(this.Sequence2);
            this.Parallel.Name = "Parallel";
            // 
            // Sequence1
            // 
            this.Sequence1.Activities.Add(this.ConsoleMessage1);
            this.Sequence1.Name = "Sequence1";
            // 
            // Sequence2
            // 
            this.Sequence2.Activities.Add(this.ConsoleMessage2);
            this.Sequence2.Name = "Sequence2";
            // 
            // ConsoleMessage1
            // 
            this.ConsoleMessage1.Name = "ConsoleMessage1";
            this.ConsoleMessage1.ExecuteCode += new System.EventHandler(this.OnConsoleMessage1);
            // 
            // ConsoleMessage2
            // 
            this.ConsoleMessage2.Name = "ConsoleMessage2";
            this.ConsoleMessage2.ExecuteCode += new System.EventHandler(this.OnConsoleMessage2);
            // 
            // WhileAndParallelWorkflow
            // 
            this.Activities.Add(this.WhileLoop);
            this.Name = "WhileAndParallelWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private ParallelActivity Parallel;
        private SequenceActivity Sequence1;
        private CodeActivity ConsoleMessage1;
        private SequenceActivity Sequence2;
        private CodeActivity ConsoleMessage2;
        private WhileActivity WhileLoop;

    }
}
