namespace FixedHierarchicalTopologySample
{
    using System;
    using System.Windows.Navigation;
    
    public class WizardLauncher : PageFunction<WizardContext>
    {
        WizardData WizardData = new WizardData();
        
        protected override void Start()
        {
            base.Start();

            // Retain instance in navigation history until Wizard is complete
            this.KeepAlive = true;
            
            // Launch the Wizard
            WizardPage1 WizardPage1 = new WizardPage1(this.WizardData);
            WizardPage1.Return += new ReturnEventHandler<WizardResult>(WizardPage_Return);
            this.NavigationService.Navigate(WizardPage1);
        }

        public void WizardPage_Return(object sender, ReturnEventArgs<WizardResult> e)
        {
            // Wizard was completed (finished or canceled), return WizardResult and WizardData
            OnReturn(new ReturnEventArgs<WizardContext>(new WizardContext(e.Result, this.WizardData)));
        }
    }
}
