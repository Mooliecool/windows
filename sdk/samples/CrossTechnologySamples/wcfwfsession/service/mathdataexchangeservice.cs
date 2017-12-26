using System;
using System.Collections.Generic;
using System.Text;
using System.Workflow.Activities;

namespace Microsoft.ServiceModel.Samples
{

    internal class MathDataExchangeService : IMathDataExchangeService
    {
        public event EventHandler<MathServiceEventArgs> ExpressionExtended;
        public event EventHandler<ExternalDataEventArgs> ResultRequested;
        internal event EventHandler<ResultEventArgs> ResultReceived;

        public void TriggerExpressionExtended(Guid instanceId, string opcode, double operand)
        {
            if (ExpressionExtended != null)
                ExpressionExtended(null, new MathServiceEventArgs(instanceId, opcode, operand));
        }

        public void TriggerResultRequested(Guid instanceId)
        {
            if (ResultRequested != null)
                ResultRequested(null, new ExternalDataEventArgs(instanceId));
        }

        public void Result(string result)
        {
            if (ResultReceived != null)
                ResultReceived(this, new ResultEventArgs(result));
        }
    }

    internal class ResultEventArgs : EventArgs
    {
        private string result;

        internal ResultEventArgs(string result)
        {
            this.result = result;
        }

        public string Result
        {
            get { return result; }
        }
    }
}
