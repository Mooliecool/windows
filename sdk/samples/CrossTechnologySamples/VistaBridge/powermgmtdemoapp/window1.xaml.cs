using System;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Microsoft.SDK.Samples.VistaBridge.Library.PowerManagement;
using System.Threading;
using System.Windows.Threading;
using System.ComponentModel;
using System.Diagnostics;
namespace DemoPMFeatures
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    /// 
    public partial class Window1 : Window
    {
        public delegate void MethodInvoker();
        MyPowerSettings settings;

        public Window1()
        {
            InitializeComponent();
            settings = (MyPowerSettings) this.FindResource("powerSettings");
        }
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            CapturePowerManagementEvents();
            GetPowerSettings();
        }
        // Get the current property values from PowerManager.
        // This method is called on startup.
        private void GetPowerSettings()
        {
            settings.PowerPersonality = PowerManager.PowerPersonality.ToString();
            settings.PowerSource = PowerManager.PowerSource.ToString();
            settings.BatteryPresent = PowerManager.IsBatteryPresent;
            settings.UpsPresent = PowerManager.IsUpsPresent;
            settings.MonitorOn = PowerManager.IsMonitorOn;
            settings.MonitorRequired = PowerManager.MonitorRequired;
            
            if (PowerManager.IsBatteryPresent)
            {
                settings.BatteryShortTerm = PowerManager.IsBatteryShortTerm;
                settings.BatteryLifePercent = PowerManager.BatteryLifePercent;
                settings.BatteryState = PowerManager.GetCurrentBatteryState().ToString();
            }
        }
        // Adds event handlers for PowerManager events.
        private void CapturePowerManagementEvents()
        {
            PowerManager.IsMonitorOnChanged += new EventHandler(MonitorOnChanged);
            PowerManager.PowerPersonalityChanged += new EventHandler(
                PowerPersonalityChanged);
            PowerManager.PowerSourceChanged += new EventHandler(PowerSourceChanged);
            if (PowerManager.IsBatteryPresent)
                PowerManager.BatteryLifePercentChanged += 
                    new EventHandler(BatteryLifePercentChanged);

           // PowerManager.SessionEnding += new EventHandler(SessionEnding);
            PowerManager.SystemBusyChanged += new EventHandler(SystemBusyChanged);
           // PowerManager.BlockShutdown("sample is running");
        }
        // PowerManager event handlers.

        void MonitorOnChanged(object sender, EventArgs e)
        {
            settings.MonitorOn = PowerManager.IsMonitorOn;
        }
      
        void PowerPersonalityChanged(object sender, EventArgs e)
        {
            settings.PowerPersonality = PowerManager.PowerPersonality.ToString();
        }
       
        void PowerSourceChanged(object sender, EventArgs e)
        {
            settings.PowerSource = PowerManager.PowerSource.ToString();
        }

        void BatteryLifePercentChanged(object sender, EventArgs e)
        {
            settings.BatteryLifePercent = PowerManager.BatteryLifePercent;
        }
        /*
        //TODO: Not blocking the ending!
        void SessionEnding(object sender, EventArgs e)
        {
            this.Dispatcher.Invoke(DispatcherPriority.Normal,
            (Window1.MethodInvoker)(() =>
            {
                textBlock2.Text = "The session is ending. Blocking it for 30 seconds.";
            }));
        
            ShutdownUnblocker unblocker = 
                PowerManager.BlockShutdown("App still working.");
            Debug.WriteLine("Preblock shutDown");

            System.Threading.Thread.Sleep(120000);
            Debug.WriteLine("Postblock shutDown");
            unblocker.UnblockShutdown();
        }
         * */
        // The event handler must use the window's Dispatcher
        // to update the UI directly. This is necessary because
        // the event handlers are invoked on a non-UI thread.
        void SystemBusyChanged(object sender, EventArgs e)
        {
            this.Dispatcher.Invoke(DispatcherPriority.Normal,
                (Window1.MethodInvoker)delegate
                {
                  textBlock2.Text =
                  String.Format("System busy changed at {0}", 
                    DateTime.Now.TimeOfDay);
                });
        }
    }
}
