using System;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Collections;
using System.Drawing;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Serialization;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.Runtime;
using System.Workflow.Activities;
using System.Workflow.Activities.Rules;

namespace Microsoft.ServiceModel.Samples
{
	public partial class Workflow1: StateMachineWorkflowActivity
	{
        private string expression = "";
        private double answer = 0;
        
        private void handleExpressionExtended_Invoked(object sender, ExternalDataEventArgs e)
        {
            MathServiceEventArgs mathServiceEventArgs = e as MathServiceEventArgs;

            switch (mathServiceEventArgs.Opcode)
            {
                case "+":
                    answer += mathServiceEventArgs.Operand;
                    break;
                case "-":
                    answer -= mathServiceEventArgs.Operand;
                    break;
                case "*":
                    answer *= mathServiceEventArgs.Operand;
                    break;
                case "/":
                    answer /= mathServiceEventArgs.Operand;
                    break;
                default:
                    throw new InvalidOperationException("unexpected Opcode");
            }

            bool firstItem = expression.Length == 0;

            expression = (firstItem ? "" : "(" + expression + mathServiceEventArgs.Opcode + " ") + mathServiceEventArgs.Operand.ToString() + (firstItem ? " " : ") ");
        }

        public string Result
        {
            get { return expression + " = " + answer.ToString(); }
        }

	}

}
