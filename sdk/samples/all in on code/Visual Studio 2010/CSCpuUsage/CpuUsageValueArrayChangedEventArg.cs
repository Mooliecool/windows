/****************************** Module Header ******************************\
 * Module Name:  CpuUsageValueArrayChangedEventArg.cs
 * Project:      CSCpuUsage
 * Copyright (c) Microsoft Corporation.
 * 
 * The CpuUsageValueArrayChangedEventArg class is used by the CpuUsageValueArrayChanged
 * event of the CpuUsageMonitorBase class.
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

namespace CSCpuUsage
{
    public class CpuUsageValueArrayChangedEventArg : EventArgs
    {
        public double[] Values { get; set; }
    }
}
