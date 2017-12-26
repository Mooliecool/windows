/****************************** Module Header ******************************\
 * Module Name:  CpuUsageMonitorBase.cs
 * Project:      CSCpuUsage
 * Copyright (c) Microsoft Corporation.
 * 
 * This is the base class of ProcessCpuUsageMonitor and TotalCpuUsageMonitor. It
 * supplies basic fields/events/functions/interfaces of the monitors, such as Timer, 
 * PerformanceCounter and IDisposable interface.
 * 
 * 
 * This source is subject to the Microsoft Public License.
 * See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 * All other rights reserved.
 * 
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;

namespace CSCpuUsage
{
    public abstract class CpuUsageMonitorBase : IDisposable
    {
        object locker = new object();

        // Specify whether this instance is disposed.
        bool disposed = false;

        // The timer is used to get the performance counter value every few seconds.
        Timer timer;

        // The CPU usage performance counter will be initialized in ProcessCpuUsageMonitor
        // and TotalCpuUsageMonitor.
        protected PerformanceCounter cpuPerformanceCounter = null;

        // The max length of the CpuUsageValueArray.
        int valueArrayCapacity;

        // The list is used to store the values.
        List<double> cpuUsageValueArray;

        /// <summary>
        /// Occurred if there is a new added value.
        /// </summary>
        public event EventHandler<CpuUsageValueArrayChangedEventArg> CpuUsageValueArrayChanged;


        public event EventHandler<ErrorEventArgs> ErrorOccurred;

        /// <summary>
        /// Initialize the timer and performance counter.
        /// </summary>
        /// <param name="timerPeriod">
        /// If this value is no more than 0, then the timer will not be enabled.
        /// </param>
        /// <param name="valueArrayCapacity">
        /// The max length of the CpuUsageValueArray.
        /// </param>
        /// <param name="categoryName">
        /// The name of the performance counter category (performance object) with which 
        /// this performance counter is associated. 
        /// </param>
        /// <param name="counterName">
        /// The name of the performance counter. 
        /// </param>
        /// <param name="instanceName">
        /// The name of the performance counter category instance, or an empty string (""),
        /// if the category contains a single instance.
        /// </param>
        public CpuUsageMonitorBase(int timerPeriod, int valueArrayCapacity,
            string categoryName, string counterName, string instanceName)
        {

            // Initialize the PerformanceCounter.
            this.cpuPerformanceCounter =
                new PerformanceCounter(categoryName, counterName, instanceName);

            this.valueArrayCapacity = valueArrayCapacity;
            cpuUsageValueArray = new List<double>();

            if (timerPeriod > 0)
            {

                // Delay the timer to invoke callback.
                this.timer = new Timer(new TimerCallback(Timer_Callback),
                    null, timerPeriod, timerPeriod);
            }
        }

        /// <summary>
        /// The method to be executed in the timer callback.
        /// </summary>
        void Timer_Callback(object obj)
        {
            lock (locker)
            {

                // Clear the list.
                if (this.cpuUsageValueArray.Count > this.valueArrayCapacity)
                {
                    this.cpuUsageValueArray.Clear();
                }

                try
                {
                    double value = GetCpuUsage();

                    if (value < 0)
                    {
                        value = 0;
                    }
                    if (value > 100)
                    {
                        value = 100;
                    }

                    this.cpuUsageValueArray.Add(value);

                    double[] values = new double[cpuUsageValueArray.Count];
                    cpuUsageValueArray.CopyTo(values, 0);

                    // Raise the event.
                    this.OnCpuUsageValueArrayChanged(
                        new CpuUsageValueArrayChangedEventArg() { Values = values });
                }
                catch (Exception ex)
                {
                    this.OnErrorOccurred(new ErrorEventArgs { Error = ex });
                }
            }
        }

        /// <summary>
        /// Get current CPU usage.
        /// </summary>
        protected virtual double GetCpuUsage()
        {
            if (!IsInstanceExist())
            {
                throw new ApplicationException(
                    string.Format("The instance {0} is not available. ",
                    this.cpuPerformanceCounter.InstanceName));
            }


            double value = cpuPerformanceCounter.NextValue();
            return value;
        }

        /// <summary>
        /// Child class may override this method to determine whether the instance exists.
        /// </summary>
        /// <returns></returns>
        protected virtual bool IsInstanceExist()
        {
            return true;
        }

        /// <summary>
        /// Raise the CpuUsageValueArrayChanged event.
        /// </summary>
        protected virtual void OnCpuUsageValueArrayChanged(CpuUsageValueArrayChangedEventArg e)
        {
            if (this.CpuUsageValueArrayChanged != null)
            {
                this.CpuUsageValueArrayChanged(this, e);
            }
        }

        /// <summary>
        /// Raise the ErrorOccurred event.
        /// </summary>
        protected virtual void OnErrorOccurred(ErrorEventArgs e)
        {
            if (this.ErrorOccurred != null)
            {
                this.ErrorOccurred(this, e);
            }
        }

        // Release the resources.
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            // Protect from being called multiple times.
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                if (timer != null)
                {
                    timer.Dispose();
                }

                if (cpuPerformanceCounter != null)
                {
                    cpuPerformanceCounter.Dispose();
                }
                disposed = true;
            }
        }
    }
}
