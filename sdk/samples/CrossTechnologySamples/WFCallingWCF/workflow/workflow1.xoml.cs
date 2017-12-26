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
	public partial class Workflow1 : SequentialWorkflowActivity
	{
        private string returnValue;
        private string input;

        public string ReturnValue
        {
            get { return returnValue; }
            private set { returnValue = value; }
        }

        public string Input
        {
            get { return input; }
            set { input = value; }
        }

        private void AfterInvoked(object sender, InvokeWebServiceEventArgs e)
        {
            Console.WriteLine("Received {0} invoking WCF service from within workflow\n", this.ReturnValue);
        }
	}
}
