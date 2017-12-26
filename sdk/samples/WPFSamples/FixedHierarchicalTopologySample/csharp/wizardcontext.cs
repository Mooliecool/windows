namespace FixedHierarchicalTopologySample
{
    using System;
    using System.Collections.ObjectModel;
    using System.Windows.Navigation;

    /// <summary>
    /// Used to manage the state of a particular Wizard, including:
    ///    1) If completed, whether it was accepted or canceled (.Result)
    ///    2) The data that was collected by the Wizard (.Data).
    /// </summary>
    public class WizardContext
    {
        WizardResult result;
        object data;

        public WizardContext(WizardResult result, object data) {
            this.result = result;
            this.data = data;
        }
        
        public WizardResult Result
        {
            get { return this.result; }
            set { this.result = value; }
        }

        public object Data
        {
            get { return this.data; }
            set { this.data = value; }
        }
    }
}
