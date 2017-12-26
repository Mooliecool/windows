using System;
using System.Windows.Threading;

namespace ActivationSample
{
    public class MailDispatcher
    {
        DispatcherTimer timer;

        public event EventHandler MailDispatched;

        public void Start()
        {
            this.timer = new DispatcherTimer();
            timer.Tick += timer_Tick;
            timer.IsEnabled = true;
            timer.Interval = new TimeSpan(0, 0, 5);
        }

        public void Stop()
        {
            this.timer.IsEnabled = false;
            this.timer.Tick -= timer_Tick;
            this.timer = null;
        }

        void timer_Tick(object sender, EventArgs e)
        {
            if (MailDispatched != null) MailDispatched(this, EventArgs.Empty);
        }
    }
}
