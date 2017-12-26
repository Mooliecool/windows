using System;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Threading;

namespace ActivationSample
{
    public partial class App : System.Windows.Application
    {
        private bool isActive;
        private MailDispatcher mailDispatcher = new MailDispatcher();
        private NotifyIcon newMailNotifyIcon = new NotifyIcon();

        void App_Startup(object sender, StartupEventArgs e)
        {
            this.mailDispatcher.MailDispatched += mailDispatcher_MailDispatched;
            this.mailDispatcher.Start();
            this.newMailNotifyIcon.Icon = ActivationSample.Properties.Resources.NewMailIcon;
        }

        void App_Activated(object sender, EventArgs e)
        {
            this.isActive = true;
            this.newMailNotifyIcon.Visible = false;
        }

        void App_Deactivated(object sender, EventArgs e)
        {
            this.isActive = false;
        }

        void App_Exit(object sender, ExitEventArgs e)
        {
            this.mailDispatcher.Stop();
        }

        void mailDispatcher_MailDispatched(object sender, EventArgs e)
        {
            ((MainWindow)this.MainWindow).AddMailItem(DateTime.Now.ToString());
            if (!this.isActive && this.newMailNotifyIcon.Visible == false)
            {
                this.newMailNotifyIcon.Visible = true;
            }
        }
    }
}