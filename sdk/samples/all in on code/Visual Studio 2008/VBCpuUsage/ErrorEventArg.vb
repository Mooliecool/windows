'************************** Module Header ******************************\
' Module Name:  ErrorEventArgs.vb
' Project:      VBCpuUsage
' Copyright (c) Microsoft Corporation.
' 
' The ErrorEventArgs class is used by the ErrorOccurred event of the CpuUsageMonitorBase
' class. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*************************************************************************

Public Class ErrorEventArgs
    Inherits EventArgs

    Dim _error As Exception
    Public Property [Error]() As Exception
        Get
            Return _error
        End Get
        Set(ByVal value As Exception)
            _error = value
        End Set
    End Property
End Class