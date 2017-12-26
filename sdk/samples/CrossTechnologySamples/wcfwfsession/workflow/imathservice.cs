using System;
using System.Collections.Generic;
using System.Text;
using System.Workflow.Activities;

namespace Microsoft.ServiceModel.Samples
{
    [Serializable]
    public class MathServiceEventArgs : ExternalDataEventArgs
    {
        private string opcode;
        private double operand;

        public MathServiceEventArgs(Guid instanceID, string opcode, double operand)
            : base(instanceID)
        {
            base.WaitForIdle = true;
            this.opcode = opcode;
            this.operand = operand;
        }

        public string Opcode
        {
            get { return opcode; }
            set { opcode = value; }
        }

        public double Operand
        {
            get { return operand; }
            set { operand = value; }
        }
    }

    [ExternalDataExchange]
    public interface IMathDataExchangeService
    {
        event EventHandler<MathServiceEventArgs> ExpressionExtended;
        event EventHandler<ExternalDataEventArgs> ResultRequested;

        void Result(string result);
    }

}
