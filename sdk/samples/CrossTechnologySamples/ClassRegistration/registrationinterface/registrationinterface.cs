using System;
using System.Workflow.ComponentModel;
using System.Workflow.Activities;

namespace Microsoft.Samples.ClassRegistration
{
    [Serializable]
    public class RegistrationEventArgs : ExternalDataEventArgs
    {
        private Guid registrationId;
        
        public RegistrationEventArgs(Guid instanceId) : base(instanceId)
        {
            this.RegistrationId = instanceId;
        }

        public Guid RegistrationId
        {
            get { return this.registrationId; }
            set { this.registrationId = value; }
        }
    }

    [ExternalDataExchange]
    public interface IRegistrarService
    {
        event EventHandler<RegistrationEventArgs> Approved;
        event EventHandler<RegistrationEventArgs> Rejected;
    }
}
