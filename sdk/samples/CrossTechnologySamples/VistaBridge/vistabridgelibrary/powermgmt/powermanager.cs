using System;
using System.Collections.Generic;
using System.Text;
using System.Resources;
using Microsoft.SDK.Samples.VistaBridge.Interop;

namespace Microsoft.SDK.Samples.VistaBridge.Library.PowerManagement
{
    /// <summary>
    /// Enables registration for 
    /// power-related event notifications and provides access to power settings.
    /// </summary>
    public static class PowerManager
    {
        internal static PowerPersonality? powerPersonality;
        internal static PowerSource? powerSource;
        internal static int? batteryLifePercent;
        internal static bool? isMonitorOn;
        internal static bool monitorRequired;
        internal static bool requestBlockSleep;

        private static readonly object _personalitylock = new object();
        private static readonly object _powersrclock = new object();
        private static readonly object _batterylifelock = new object();
        private static readonly object _monitoronlock = new object();
        

        #region Notifications

        /// <summary>
        /// Raised each time the active power scheme changes.
        /// </summary>
        /// <exception cref="InvalidOperationException">The event handler specified for removal was not registered.</exception>
        /// <exception cref="System.PlatformNotSupportedException">Requires Vista/Windows Server 2008.</exception>
        public static event EventHandler PowerPersonalityChanged
        {
            add
            {
                ThrowIfNotVista();

                MessageManager.RegisterPowerEvent(
                    EventManager.PowerPersonalityChange, value);
            }

            remove
            {
                ThrowIfNotVista();

                MessageManager.UnregisterPowerEvent(
                    EventManager.PowerPersonalityChange, value);
            }
        }

        /// <summary>
        /// Raised when the power source changes.
        /// </summary>
        /// <exception cref="InvalidOperationException">The event handler specified for removal was not registered.</exception>
        /// <exception cref="System.PlatformNotSupportedException">Requires Vista/Windows Server 2008.</exception>
        public static event EventHandler PowerSourceChanged
        {
            add
            {
                ThrowIfNotVista();

                MessageManager.RegisterPowerEvent(
                    EventManager.PowerSourceChange, value);
            }

            remove
            {
                ThrowIfNotVista();

                MessageManager.UnregisterPowerEvent(
                    EventManager.PowerSourceChange, value);
            }
        }

        /// <summary>
        /// Raised when the remaining battery life changes.
        /// </summary>
        /// <exception cref="InvalidOperationException">The event handler specified for removal was not registered.</exception>
       /// <exception cref="System.PlatformNotSupportedException">Requires Vista/Windows Server 2008.</exception>
        public static event EventHandler BatteryLifePercentChanged
        {
            add
            {
                ThrowIfNotVista();

                MessageManager.RegisterPowerEvent(
                    EventManager.BatteryCapacityChange, value);
            }
            remove
            {
                ThrowIfNotVista();

                MessageManager.UnregisterPowerEvent(
                    EventManager.BatteryCapacityChange, value);
            }
        }

        /// <summary>
        /// Raised when the monitor status changes.
        /// </summary>
        /// <exception cref="InvalidOperationException">The event handler specified for removal was not registered.</exception>
        /// <exception cref="System.PlatformNotSupportedException">Requires Vista/Windows Server 2008.</exception>
        public static event EventHandler IsMonitorOnChanged
        {
            add
            {
                ThrowIfNotVista();

                MessageManager.RegisterPowerEvent(
                    EventManager.MonitorPowerStatus, value);
            }
            remove
            {
                ThrowIfNotVista();

                MessageManager.UnregisterPowerEvent(
                    EventManager.MonitorPowerStatus, value);
            }
        }

        /// <summary>
        /// Raised when the system will not be moving into an idle 
        /// state in the near future so applications should
        /// perform any tasks that 
        /// would otherwise prevent the computer from entering an idle state. 
        /// </summary>
        /// <exception cref="InvalidOperationException">The event handler specified for removal was not registered.</exception>
        /// <exception cref="System.PlatformNotSupportedException">Requires Vista/Windows Server 2008.</exception>
        public static event EventHandler SystemBusyChanged
        {
            add
            {
                ThrowIfNotVista();

                MessageManager.RegisterPowerEvent(
                    EventManager.BackgroundTaskNotification, value);
            }
            remove
            {
                ThrowIfNotVista();

                MessageManager.UnregisterPowerEvent(
                    EventManager.BackgroundTaskNotification, value);
            }
        }
        #endregion

        /// <summary>
        /// Gets a snapshot of the current battery state.
        /// </summary>
        /// <returns>A <see cref="Microsoft.SDK.Samples.VistaBridge.Library.PowerManagement.BatteryState"/> instance that represents 
        /// the state of the battery at the time this method was called.</returns>
        /// <exception cref="System.InvalidOperationException">The system does not have a battery.</exception>
        /// <exception cref="System.PlatformNotSupportedException">Requires XP/Windows Server 2003 or higher.</exception>

        public static BatteryState GetCurrentBatteryState()
        {
            ThrowIfNotXP();

            if (!Power.GetSystemBatteryState().BatteryPresent)
                throw new InvalidOperationException(
                    "Battery is not present on this system.");

            return new BatteryState();
        }

        #region Power System Properties

        /// <summary>
        /// Gets or sets a value that indicates whether the monitor is 
        /// set to remain active.  
        /// </summary>
        /// <exception cref="T:System.PlatformNotSupportedException">Requires XP/Windows Server 2003 or higher.</exception>
        /// <exception cref="T:System.Security.SecurityException">The caller does not have sufficient privileges to set this property.
        /// </exception>
        /// <remarks>This information is typically used by applications
        /// that display information but do not require 
        /// user interaction. For example, video playback applications.</remarks>
        /// <permission cref="T:System.Security.Permissions.SecurityPermission"> to set this property. Demand value: <see cref="F:System.Security.Permissions.SecurityAction.Demand"/>; Named Permission Sets: <b>FullTrust</b>.</permission>
        /// <value>A <see cref="System.Boolean"/> value. <b>True</b> if the monitor
        /// is required to remain on.</value>
        public static bool MonitorRequired
        {
            get 
            {
                ThrowIfNotXP();
                return monitorRequired;
            }
            [System.Security.Permissions.PermissionSetAttribute(System.Security.Permissions.SecurityAction.Demand, Name = "FullTrust")]
            set 
            {
                ThrowIfNotXP();

                if (value)
                {
                    Power.SetThreadExecutionState(ExecutionState.Continuous | ExecutionState.DisplayRequired);
                }
                else
                {
                    Power.SetThreadExecutionState(ExecutionState.Continuous);
                }

                monitorRequired = value;
            }
        }

        /// <summary>
        /// Gets or sets a value that indicates whether the system 
        /// is required to be in the working state.
        /// </summary>
        /// <exception cref="System.PlatformNotSupportedException">Requires XP/Windows Server 2003 or higher.</exception>
        /// <exception cref="System.Security.SecurityException">The caller does not have sufficient privileges to set this property.
        /// </exception>
        /// <permission cref="System.Security.Permissions.SecurityPermission"> to set this property. Demand value: <see cref="F:System.Security.Permissions.SecurityAction.Demand"/>; Named Permission Sets: <b>FullTrust</b>.</permission>
        /// <value>A <see cref="System.Boolean"/> value.</value>
        public static bool RequestBlockSleep
        {
            get 
            {
                ThrowIfNotXP();

                return requestBlockSleep;
            }
            [System.Security.Permissions.PermissionSetAttribute(System.Security.Permissions.SecurityAction.Demand, Name = "FullTrust")]
            set 
            {
                ThrowIfNotXP();

                if (value)
                    Power.SetThreadExecutionState(ExecutionState.Continuous | ExecutionState.SystemRequired);
                else
                    Power.SetThreadExecutionState(ExecutionState.Continuous);

                requestBlockSleep = value;
            }
        }

        /// <summary>
        /// Gets a value that indicates whether a battery is present.  
        /// The battery can be a short term battery.
        /// </summary>
        /// <exception cref="System.PlatformNotSupportedException">Requires XP/Windows Server 2003 or higher.</exception>
        /// <value>A <see cref="System.Boolean"/> value.</value>
        public static bool IsBatteryPresent
        {
            get
            {
                ThrowIfNotXP();

                return Power.GetSystemBatteryState().BatteryPresent;
            }
        }

        /// <summary>
        /// Gets a value that indicates whether the battery is a short term battery. 
        /// </summary>
        /// <exception cref="System.PlatformNotSupportedException">Requires XP/Windows Server 2003 or higher.</exception>
        /// <value>A <see cref="System.Boolean"/> value.</value>
        public static bool IsBatteryShortTerm
        {
            get
            {
                ThrowIfNotXP();
                return Power.GetSystemPowerCapabilities().BatteriesAreShortTerm;
            }
        }

        /// <summary>
        /// Gets a value that indicates a UPS is present to prevent 
        /// sudden loss of power.
        /// </summary>
        /// <exception cref="System.PlatformNotSupportedException">Requires XP/Windows Server 2003 or higher.</exception>
        /// <value>A <see cref="System.Boolean"/> value.</value>
        public static bool IsUpsPresent
        {
            get
            {
                ThrowIfNotXP();

                return Power.GetSystemPowerCapabilities().UpsPresent;
            }
        }

        /// <summary>
        /// Gets a value that indicates the current power scheme.  
        /// </summary>
        /// <exception cref="System.PlatformNotSupportedException">Requires Vista/Windows Server 2008.</exception>
        /// <value>A <see cref="Microsoft.SDK.Samples.VistaBridge.Library.PowerManagement.PowerPersonality"/> value.</value>
        public static PowerPersonality PowerPersonality
        {
            get 
            {
                ThrowIfNotVista();

                // The only way to get the current power personality is 
                // to register for an event so if the
                // personality value has not been set yet, 
                // a dummy event needs to be registered.  All
                // subsequent calls to this property get the value from memory.
                if (powerPersonality == null)
                {
                    lock (_personalitylock)
                    {
                        if (powerPersonality == null)
                        {
                            EventHandler dummy = delegate(object sender, EventArgs args) { };
                            PowerPersonalityChanged += dummy;
                            // Wait until Windows updates the personality 
                            // (through RegisterPowerSettingNotification).
                            EventManager.personalityReset.WaitOne();
                        }
                    }
                }
                return (PowerPersonality)powerPersonality;
            }
        }

        /// <summary>
        /// Gets a value that indicates the remaining battery life 
        /// (as a percentage of the full battery charge). 
        /// This value is in the range 0-100, 
        /// where 0 is not charged and 100 is fully charged.  
        /// </summary>
        /// <exception cref="System.InvalidOperationException">The system does not have a battery.</exception>
        /// <exception cref="System.PlatformNotSupportedException">Requires Vista/Windows Server 2008.</exception>
        /// <value>An <see cref="System.Int32"/> value.</value>
        public static int BatteryLifePercent
        {
            get 
            {
                if (!Power.GetSystemBatteryState().BatteryPresent)
                    throw new InvalidOperationException(
                        "Battery is not present on the system.");

                ThrowIfNotVista();

                if (batteryLifePercent == null)
                {
                    lock (_batterylifelock)
                    {
                        if (batteryLifePercent == null)
                        {
                            EventHandler dummy = delegate(object sender, EventArgs args) { };
                            BatteryLifePercentChanged += dummy;
                            // Wait until Windows updates the personality 
                            // (through RegisterPowerSettingNotification).
                            EventManager.batteryLifeReset.WaitOne();
                        }
                    }
                }
                return (int)batteryLifePercent;
            }
        }

        /// <summary>
        /// Gets a value that indictates whether the monitor is on. 
        /// </summary>
        /// <exception cref="System.PlatformNotSupportedException">Requires Vista/Windows Server 2008.</exception>
        /// <value>A <see cref="System.Boolean"/> value.</value>
        public static bool IsMonitorOn
        {
            get 
            {
                ThrowIfNotVista();

                if (isMonitorOn == null)
                {
                    lock (_monitoronlock)
                    {
                        if (isMonitorOn == null)
                        {
                            EventHandler dummy = delegate(object sender, EventArgs args) { };
                            IsMonitorOnChanged += dummy;
                            // Wait until Windows updates the power source 
                            // (through RegisterPowerSettingNotification)
                            EventManager.monitorOnReset.WaitOne();
                        }
                    }
                }

                return (bool)isMonitorOn;
            }
        }

        /// <summary>
        /// Gets the current power source.  
        /// </summary>
        /// <exception cref="System.PlatformNotSupportedException">Requires Vista/Windows Server 2008.</exception>
        ///         /// <value>A <see cref="Microsoft.SDK.Samples.VistaBridge.Library.PowerManagement.PowerSource"/> value.</value>
        public static PowerSource PowerSource
        {
            get 
            {
                ThrowIfNotVista();

                if (powerSource == null)
                {
                    lock (_powersrclock)
                    {
                        if (powerSource == null)
                        {
                            EventHandler dummy = delegate(object sender, EventArgs args) { ;};
                            PowerSourceChanged += dummy;
                            // Wait until Windows updates the power source 
                            // (through RegisterPowerSettingNotification).
                            EventManager.powerSrcReset.WaitOne();
                        }
                    }
                }

                return (PowerSource)powerSource;
            }
        }
        #endregion

        private const string _vistaRequired = 
            "Windows Vista/Server 2008 required.";
        private const string _xpRequired = 
            "Windows XP/Server 2003 or higher required.";

        private static void ThrowIfNotVista()
        {
            if (!Helpers.RunningOnVista)
                throw new PlatformNotSupportedException(_vistaRequired);
        }
        private static void ThrowIfNotXP()
        {
            if (!Helpers.RunningOnXP)
                throw new PlatformNotSupportedException(_xpRequired);
        }
    }
}
