'************************** Module Header ******************************'
' Module Name:  CpuUsageValueArrayChangedEventArg.vb
' Project:      VBCpuUsage
' Copyright (c) Microsoft Corporation.
' 
' The event arguments used in the CpuUsageValueArrayChanged event of the 
' CpuUsageMonitorBase class.
' 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Public Class CpuUsageValueArrayChangedEventArg
    Inherits EventArgs
    Public Property Values() As Double()
End Class
