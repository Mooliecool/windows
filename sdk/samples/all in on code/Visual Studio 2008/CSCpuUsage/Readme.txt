================================================================================
				Windows Application: CSCpuUsage                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

This sample demonstrates how to use PerformanceCounter to get the CPU Usage with
following features
1. The Total Processor Time.
2. The Processor time of a specific process.
3. Draw the CPU Usage History like Task Manager.

//////////////////////////////////////////////////////////////////////////////////
Prerequisite

Microsoft Chart Controls for Microsoft .NET Framework 3.5
http://www.microsoft.com/download/en/details.aspx?displaylang=en&id=14422

////////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Open this project in Visual Studio 2008.

Step2. Build the project and run CSCpuUsage.exe.

Step3. Check "Display total CPU Usage" and "Display the CPU Usage of a process". Click the 
       ComboBox and select a process (i.e. taskmgr, if Task Manager is running.) to monitor.

       You will see 2 charts on this application that display the CPU usage history.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

A. Design the CpuUsageMonitorBase class that supplies basic fields/events/functions/interfaces 
   of the monitors, such as Timer, PerformanceCounter and IDisposable interface.

   This is an abstract class, so CpuUsageMonitorBase class cannot be instantiated. When the
   child classes are instantiated, they have to pass the categoryName, counterName and instanceName
   to the constructor of CpuUsageMonitorBase class to initialize the performance counter.
   
           this.cpuPerformanceCounter =
                new PerformanceCounter(categoryName, counterName, instanceName);
    
   The timer is used to get the performance counter value every few seconds, and raise the 
   CpuUsageValueArrayChanged event. If there is any exception while reading the performance 
   counter value, the ErrorOccurred event will be raised. 
     

B. Design the TotalCpuUsageMonitor class that is used to monitor the total CPU usage. It inherits
   the CpuUsageMonitorBase class, and defines 3 constants:

        const string categoryName = "Processor";
        const string counterName = "% Processor Time";
        const string instanceName = "_Total";

   To get the total CPU usage, we can use above constants to initialize a performance counter.
    
        public TotalCpuUsageMonitor(int timerPeriod, int valueArrayCapacity)
            : base(timerPeriod, valueArrayCapacity, categoryName, counterName, instanceName)
        { }

C. Design the ProcessCpuUsageMonitor class that is used to monitor the CPU usage of a 
   specified process. It also inherits the CpuUsageMonitorBase class, and defines 2 constants:
        
        const string categoryName = "Process";
        const string counterName = "% Processor Time";
   
   To initialize a performance counter, we still need the instance name (a process name). So this 
   class also supplies a method to get available processes.

        static PerformanceCounterCategory category;
        public static PerformanceCounterCategory Category
        {
            get
            {
                if (category == null)
                {
                    category = new PerformanceCounterCategory(categoryName);
                }
                return category;
            }
        }

        public static string[] GetAvailableProcesses()
        {          
            return Category.GetInstanceNames().OrderBy(name => name).ToArray();
        }



D. Design the MainForm that initializes the totalCpuUsageMonitor and processCpuUsageMonitor,
   registers the CpuUsageValueArrayChanged and display the CPU usage history in the charts.

        /// <summary>
        /// Invoke the processCpuUsageMonitor_CpuUsageValueArrayChangedHandler to handle
        /// the CpuUsageValueArrayChanged event of processCpuUsageMonitor.
        /// </summary>
        void processCpuUsageMonitor_CpuUsageValueArrayChanged(object sender, 
            CpuUsageValueArrayChangedEventArg e)
        {
            this.Invoke(new EventHandler<CpuUsageValueArrayChangedEventArg>(
                processCpuUsageMonitor_CpuUsageValueArrayChangedHandler), sender, e);
        }

        void processCpuUsageMonitor_CpuUsageValueArrayChangedHandler(object sender,
            CpuUsageValueArrayChangedEventArg e)
        {
            var processCpuUsageSeries = chartProcessCupUsage.Series["ProcessCpuUsageSeries"];
            var values = e.Values;

            // Display the process CPU usage in the chart.
            processCpuUsageSeries.Points.DataBindY(e.Values);

        }


        /// <summary>
        /// Invoke the totalCpuUsageMonitor_CpuUsageValueArrayChangedHandler to handle
        /// the CpuUsageValueArrayChanged event of totalCpuUsageMonitor.
        /// </summary>
        void totalCpuUsageMonitor_CpuUsageValueArrayChanged(object sender, 
            CpuUsageValueArrayChangedEventArg e)
        {
            this.Invoke(new EventHandler<CpuUsageValueArrayChangedEventArg>(
                totalCpuUsageMonitor_CpuUsageValueArrayChangedHandler), sender, e);
        }
        void totalCpuUsageMonitor_CpuUsageValueArrayChangedHandler(object sender, 
            CpuUsageValueArrayChangedEventArg e)
        {
            var totalCpuUsageSeries = chartTotalCpuUsage.Series["TotalCpuUsageSeries"];
            var values = e.Values;

            // Display the total CPU usage in the chart.
            totalCpuUsageSeries.Points.DataBindY(e.Values);

        }

  If there is any error while reading the performance counter value, the UI will also handle
  this event. 

        /// <summary>
        /// Invoke the processCpuUsageMonitor_ErrorOccurredHandler to handle
        /// the ErrorOccurred event of processCpuUsageMonitor.
        /// </summary>
        void processCpuUsageMonitor_ErrorOccurred(object sender, ErrorEventArgs e)
        {
            this.Invoke(new EventHandler<ErrorEventArgs>(
                processCpuUsageMonitor_ErrorOccurredHandler), sender, e);
        }

        void processCpuUsageMonitor_ErrorOccurredHandler(object sender, ErrorEventArgs e)
        {
            if (processCpuUsageMonitor != null)
            {
                processCpuUsageMonitor.Dispose();
                processCpuUsageMonitor = null;
                var processCpuUsageSeries = chartProcessCupUsage.Series["ProcessCpuUsageSeries"];
                processCpuUsageSeries.Points.Clear();
            }
            MessageBox.Show(e.Error.Message);
        }       

        /// <summary>
        /// Invoke the totalCpuUsageMonitor_ErrorOccurredHandler to handle
        /// the ErrorOccurred event of totalCpuUsageMonitor.
        /// </summary>
        void totalCpuUsageMonitor_ErrorOccurred(object sender, ErrorEventArgs e)
        {
            this.Invoke(new EventHandler<ErrorEventArgs>(
                totalCpuUsageMonitor_ErrorOccurredHandler),sender,e);
        }

        void totalCpuUsageMonitor_ErrorOccurredHandler(object sender, ErrorEventArgs e)
        {
            if (totalCpuUsageMonitor != null)
            {
                totalCpuUsageMonitor.Dispose();
                totalCpuUsageMonitor = null;
            }
            MessageBox.Show(e.Error.Message);
        }

/////////////////////////////////////////////////////////////////////////////
References:

PerformanceCounter Class
http://msdn.microsoft.com/en-us/library/system.diagnostics.performancecounter.aspx

Chart Class
http://msdn.microsoft.com/en-us/library/system.windows.forms.datavisualization.charting.chart.aspx
/////////////////////////////////////////////////////////////////////////////