namespace AdaptiveTopologySample
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Navigation;
    using System.Windows.Threading;

    public class WizardNavigationHub : PageFunction<WizardContext>
    {
        List<PageFunction<WizardResult>> navigationOrder = new List<PageFunction<WizardResult>>();
        WizardData WizardData = new WizardData();
        static WizardNavigationHub wizardNavigationHub;

        public WizardNavigationHub()
        {
            wizardNavigationHub = this;
        }

        public static WizardNavigationHub Current
        {
            get { return wizardNavigationHub; }
        }

        #region Navigation for determining the navigation order of the Wizard
        protected override void Start()
        {
            // Keep this page function instance in navigation history until completion
            JournalEntry.SetKeepAlive(this, true);
            this.RemoveFromJournal = true;

            // Navigate to data entry page to determine navigation sequence
            DataEntryPage dataEntryPage = new DataEntryPage();
            dataEntryPage.Return += new ReturnEventHandler<WizardContext>(dataEntryPage_Return);
            this.NavigationService.Navigate(dataEntryPage);
        }
        void dataEntryPage_Return(object sender, ReturnEventArgs<WizardContext> e)
        {
            WizardContext dataEntryWizardContext = (WizardContext)e.Result;

            // Cancel Wizard if data entry page was canceled
            if (dataEntryWizardContext.Result == WizardResult.Canceled)
            {
                OnReturn(new ReturnEventArgs<WizardContext>(new WizardContext(WizardResult.Canceled, null)));
                return;
            }

            // Organize navigation direction
            if ((WizardNavigationDirection)dataEntryWizardContext.Data == WizardNavigationDirection.Forwards)
            {
                this.navigationOrder.Add((PageFunction<WizardResult>)new WizardPage1(this.WizardData));
                this.navigationOrder.Add((PageFunction<WizardResult>)new WizardPage2(this.WizardData));
                this.navigationOrder.Add((PageFunction<WizardResult>)new WizardPage3(this.WizardData));
            }
            else
            {
                this.navigationOrder.Add((PageFunction<WizardResult>)new WizardPage3(this.WizardData));
                this.navigationOrder.Add((PageFunction<WizardResult>)new WizardPage2(this.WizardData));
                this.navigationOrder.Add((PageFunction<WizardResult>)new WizardPage1(this.WizardData));
            }

            // Navigate to first page
            this.navigationOrder[0].Return += NavigationHub_Return;
            NavigationWindow host = (NavigationWindow)Application.Current.MainWindow;
            host.Navigate(this.navigationOrder[0]);
        }
        #endregion

        public PageFunction<WizardResult> GetNextWizardPage(PageFunction<WizardResult> currentPageFunction)
        {
            int index = this.navigationOrder.IndexOf((PageFunction<WizardResult>)currentPageFunction);
            return this.navigationOrder[++index];
        }
        public bool CanGoNext(PageFunction<WizardResult> currentPageFunction) {
            int index = this.navigationOrder.IndexOf((PageFunction<WizardResult>)currentPageFunction);
            return (index < this.navigationOrder.Count - 1);
        }
        public bool CanGoBack(PageFunction<WizardResult> currentPageFunction)
        {
            int index = this.navigationOrder.IndexOf((PageFunction<WizardResult>)currentPageFunction);
            return (index > 0);
        }
        public bool CanFinish(PageFunction<WizardResult> currentPageFunction)
        {
            int index = this.navigationOrder.IndexOf((PageFunction<WizardResult>)currentPageFunction);
            return (index == this.navigationOrder.Count - 1);
        }
                
        void NavigationHub_Return(object sender, ReturnEventArgs<WizardResult> e)
        {
            // If returning, Wizard was completed (finished or canceled),
            // so continue returning to calling page
            OnReturn(new ReturnEventArgs<WizardContext>(new WizardContext(e.Result, this.WizardData)));
        }
    }
}