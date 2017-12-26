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
using System.CodeDom;
using System.Windows.Forms;

namespace Microsoft.Samples.Workflow.WorkflowReflectionUtility.Dialogs
{
    /// <summary>
    /// Dialog box hosting the RuleEvaluatorPanel.
    /// </summary>
    public partial class EvaluateRuleDialog : Form
    {
        /// <summary>
        /// Create a new instance of the dialog for the specified
        /// expression and "this" type.
        /// </summary>
        /// <param name="expr">The expression to evaluate.</param>
        /// <param name="thisType">The type of the object referred
        /// to by CodeThisReferenceExpression.</param>
        public EvaluateRuleDialog(CodeExpression expression, Type thisType)
        {
            if (expression == null)
            {
                throw new ArgumentNullException("expression");
            }

            InitializeComponent();

            ruleEvaluationPanel1.CloneAndSetExpression(expression, thisType);
        }
    }
}