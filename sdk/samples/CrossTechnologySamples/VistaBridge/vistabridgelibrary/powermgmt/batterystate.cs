using System;
using Microsoft.SDK.Samples.VistaBridge.Interop;

namespace Microsoft.SDK.Samples.VistaBridge.Library.PowerManagement
{
    /// <summary>
    /// A snapshot of the state of the battery.
    /// </summary>
    public class BatteryState
    {
        private bool _acOnline;
        private int _maxCharge;
        private int _currentCharge;
        private int _dischargeRate;
        private TimeSpan _estimatedTimeRemaining;
        private int _suggestedCriticalBatteryCharge;
        private int _suggestedBatteryWarningCharge;

        internal BatteryState()
        {
            SafeNativeMethods.SystemBatteryState battState = Power.GetSystemBatteryState();
            _acOnline = battState.AcOnLine;
            _maxCharge = (int)battState.MaxCapacity;
            _currentCharge = (int)battState.RemainingCapacity;
            _dischargeRate = (int)battState.Rate;
            long _estimatedTime = (long)battState.EstimatedTime;
            int _minutes = (int)(_estimatedTime/60);
            int _seconds = (int)(_estimatedTime % 60);
            _estimatedTimeRemaining = new TimeSpan(0, _minutes, _seconds);
            _suggestedCriticalBatteryCharge = (int)battState.DefaultAlert1;
            _suggestedBatteryWarningCharge = (int)battState.DefaultAlert2;
        }

        #region Public properties

        /// <summary>
        /// Gets a value that indicates whether the battery charger is 
        /// operating on external power.
        /// </summary>
        /// <value>A <see cref="System.Boolean"/> value. <b>True</b> indicates the battery charger is operating on AC power.</value>
        public bool ACOnline 
        { 
            get 
            {
                return _acOnline;
            } 
        }

        /// <summary>
        /// Gets the maximum charge of the battery (in mW).
        /// </summary>
        /// <value>An <see cref="System.Int32"/> value.</value>
        public int MaxCharge 
        { 
            get 
            {
                return _maxCharge;
            } 
        }

        /// <summary>
        /// Gets the current charge of the battery (in mW).
        /// </summary>
        /// <value>An <see cref="System.Int32"/> value.</value>
        public int CurrentCharge 
        { 
            get 
            {
                return _currentCharge;
            } 
        }

        /// <summary>
        /// Gets the rate of discharge for the battery (in mW). 
        /// </summary>
        /// <remarks>
        /// A negative value indicates the
        /// charge rate. Not all batteries support charge rate.
        /// </remarks>
        /// <value>An <see cref="System.Int32"/> value.</value>
        public int DischargeRate 
        { 
            get 
            {
                return _dischargeRate;
            } 
        }

        /// <summary>
        /// Gets the estimated time remaining until the battery is empty.
        /// </summary>
        /// <value>A <see cref="System.TimeSpan"/> object.</value>
        public TimeSpan EstimatedTimeRemaining 
        { 
            get 
            {
                return _estimatedTimeRemaining;
            } 
        }

        /// <summary>
        /// Gets the manufacturer's suggested battery charge level 
        /// that should cause a critical alert to be sent to the user.
        /// </summary>
        /// <value>An <see cref="System.Int32"/> value.</value>
        public int SuggestedCriticalBatteryCharge 
        { 
            get 
            {
                return _suggestedCriticalBatteryCharge;
            } 
        }

        /// <summary>
        /// Gets the manufacturer's suggested battery charge level
        /// that should cause a warning to be sent to the user.
        /// </summary>
        /// <value>An <see cref="System.Int32"/> value.</value>
        public int SuggestedBatteryWarningCharge 
        { 
            get 
            {
                return _suggestedBatteryWarningCharge;
            } 
        }

        #endregion

        /// <summary>
        /// Generates a string that represents this <b>BatteryState</b> object.
        /// </summary>
        /// <returns>A <see cref="System.String"/> representation of this object's current state.</returns>
        public override string ToString()
        {
            string output;
            output = "ACOnline: " + _acOnline + Environment.NewLine +
                "Max Charge: " + _maxCharge + Environment.NewLine + 
                "Current Charge: " + _currentCharge + Environment.NewLine +
                "Discharge Rate: " + _dischargeRate + Environment.NewLine + 
                "Estimated Time Remaining: " + _estimatedTimeRemaining
                + Environment.NewLine + 
                "Suggested Critical Battery Charge: " + _suggestedCriticalBatteryCharge
                + Environment.NewLine + 
                "Suggested Battery Warning Charge: " + _suggestedBatteryWarningCharge;
            return output;
        }
    }
}
