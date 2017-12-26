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
        private string _receivedData;
        private string _resultData;


        public string ReceivedData
        {
            get { return _receivedData; }
            set { _receivedData = value; }
        }


        public string ResultData
        {
            get { return _resultData; }
            set { _resultData = value; }
        }


        private void codeActivity1_ExecuteCode(object sender, EventArgs e)
        {
            Console.WriteLine("Inside the WF with data: {0}", this.ReceivedData);
            this.ResultData = this.ReceivedData;
        }
    }
}
