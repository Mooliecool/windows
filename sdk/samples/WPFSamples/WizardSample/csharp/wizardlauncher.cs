namespace WizardSample
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Navigation;

    public class WizardLauncher : PageFunction<WizardResult>
    {
        WizardData wizardData = new WizardData();
        public event WizardReturnEventHandler WizardReturn;

        protected override void Start()
        {
            base.Start();

            // So we remember the WizardCompleted event registration
            this.KeepAlive = true;
            
            // Launch the wizard
            WizardPage1 wizardPage1 = new WizardPage1(this.wizardData);
            wizardPage1.Return += new ReturnEventHandler<WizardResult>(wizardPage_Return);
            this.NavigationService.Navigate(wizardPage1);
        }

        public void wizardPage_Return(object sender, ReturnEventArgs<WizardResult> e)
        {
            // Notify client that wizard has completed
            // NOTE: We need this custom event because the Return event cannot be
            // registered by window code - if WizardDialogBox registers an event handler with
            // the WizardLauncher's Return event, the event is not raised.
            if (this.WizardReturn != null)
            {
                this.WizardReturn(this, new WizardReturnEventArgs(e.Result, this.wizardData));
            }
            OnReturn(null);
        }
    }
}
