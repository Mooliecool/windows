namespace FixedHierarchicalTopologySample
{
    using System;
    
    /// <summary>
    /// Data that is collected by the Wizard
    /// </summary>
    public class WizardData
    {
        string dataItem1;
        string dataItem2;
        string dataItem3;
        string dataItem4;
        
        public string DataItem1 {
            get { return this.dataItem1; }
            set { this.dataItem1 = value; }
        }

        public string DataItem2
        {
            get { return this.dataItem2; }
            set { this.dataItem2 = value; }
        }

        public string DataItem3
        {
            get { return this.dataItem3; }
            set { this.dataItem3 = value; }
        }

        public string DataItem4
        {
            get { return this.dataItem4; }
            set { this.dataItem4 = value; }
        }
    }
}
