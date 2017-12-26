/****************************** Module Header ******************************\
 * Module Name:  TotalCpuUsageMonitor.cs
 * Project:      CSCpuUsage
 * Copyright (c) Microsoft Corporation.
 * 
 * This class inherits CpuUsageMonitorBase and it is used to monitor the total
 * CPU usage. 
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
using System.Linq;
using System.Text;
using System.Diagnostics;

namespace CSCpuUsage
{
    public class TotalCpuUsageMonitor : CpuUsageMonitorBase
    {
        const string categoryName = "Processor";
        const string counterName = "% Processor Time";
        const string instanceName = "_Total";

        public TotalCpuUsageMonitor(int timerPeriod, int valueArrayCapacity)
            : base(timerPeriod, valueArrayCapacity, categoryName, counterName, instanceName)
        { }
    }
}