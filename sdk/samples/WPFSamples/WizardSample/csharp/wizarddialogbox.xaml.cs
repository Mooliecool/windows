using System;
using System.Windows;
using System.Windows.Navigation;

namespace WizardSample
{
    public partial class WizardDialogBox : NavigationWindow
    {
        WizardData wizardData;

        public WizardData WizardData
        {
            get { return this.wizardData; }
        }

        public WizardDialogBox()
        {
            InitializeComponent();

            // Launch the wizard
            WizardLauncher wizardLauncher = new WizardLauncher();
            wizardLauncher.WizardReturn += new WizardReturnEventHandler(wizardLauncher_WizardReturn);
            this.Navigate(wizardLauncher);
        }

        void wizardLauncher_WizardReturn(object sender, WizardReturnEventArgs e)
        {
            // Handle wizard return
            this.wizardData = e.Data as WizardData;
            if (this.DialogResult == null)
            {
                this.DialogResult = (e.Result == WizardResult.Finished);
            }
        }
    }
}