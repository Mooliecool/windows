using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Windows.Navigation;
using System.Collections.Generic;
using System.Windows.Media.Animation;


namespace Microsoft.Samples.MSBuildG
{
    // Summary:
    // Interaction logic for WizardWindow.xaml
    

    public partial class WizardWindow : NavigationWindow
    {
        private BuildProject m_Project;
        private List<WizardPageFunction> wizardPages;

        public WizardWindow()
        {
            InitializeComponent();

            m_Project = new BuildProject();

            wizardPages = new List<WizardPageFunction>();

            //Create the list of pages I have here
            wizardPages.Add(new WizardPage1());
            wizardPages.Add(new WizardPage2());
            wizardPages.Add(new WizardPage3());

            wizardPages.Add(new WizardPageDone());
        }

        public Frame WizardFrame
        {
            get
            {
                return (this.GetTemplateChild("WizardFrame") as Frame);
            }
        }

        public Button NavigateBackButton
        {
            get
            {
                return (this.GetTemplateChild("NavigateBackButton") as Button);
            }
        }

        public Button NavigateNextButton
        {
            get
            {
                return (this.GetTemplateChild("NavigateNextButton") as Button);
            }
        }


        public StackPanel NavContainer
        {
            get
            {
                return (this.GetTemplateChild("NavContainer") as StackPanel);
            }
        }

        public void NavigateBack() 
        {
            if (this.CanGoBack)
            {
                this.GoBack();
                this.NavigateNextButton.IsEnabled = true;
            }

            this.NavigateBackButton.IsEnabled = this.CanGoBack;
        }

        public void NavigateNext()
        {
            WizardPageFunction currentPage = (WizardPageFunction)WizardFrame.Content;
            currentPage.Done(m_Project);
        }

        private void WindowLoaded(object sender, EventArgs e)
        {
            this.NavigateToPage(wizardPages[0]);
        }

        private void NavigateToPage(WizardPageFunction wizardPage)
        {
            if (wizardPage == null)
            {
                throw new ArgumentNullException("wizardPage");
            }

            wizardPage.Initialize(m_Project);

            //wizardPage.Return += new ReturnEventHandler<BuildProject>(wizardPage_Return);
            
            //TEMP: Because PageFunctions are broken
            wizardPage.ReturnTemp += new WizardPageFunction.ReturnEventDelegate(wizardPage_Return);

            WizardFrame.Navigate(wizardPage);
        }

        void wizardPage_Return(object sender, ReturnEventArgs<BuildProject> e)
        {
            //Retrieve the current page
            WizardPageFunction currentPage = (WizardPageFunction)WizardFrame.Content;

            //Get the index of the next page
            int nextIndex = wizardPages.IndexOf(currentPage) + 1;

            if (nextIndex == wizardPages.Count - 1)
            {
                //clear history
                while (this.CanGoBack)
                {
                    this.RemoveBackEntry();
                }

                this.NavContainer.BeginStoryboard(FindResource("HideNavBar") as Storyboard);
            }

            //Navigate to the next page
            if (nextIndex < wizardPages.Count)
            {
                this.NavigateToPage(wizardPages[nextIndex]);
            }
            else
            {
                DesignerWindow designWindow = new DesignerWindow(m_Project);
                designWindow.Show();

                this.Close();
            }

            //See if the Next and Back buttons are allowed
            this.NavigateNextButton.IsEnabled = (nextIndex < (wizardPages.Count - 1));
            this.NavigateBackButton.IsEnabled = (nextIndex > 0);
        }


        private void WindowClosing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (Application.Current.Windows.Count == 1)
            {
                PickerWindow pickerDialog = new PickerWindow();
                pickerDialog.Show();
            }
        }
    }
}