'*************************** Module Header ********************************\
' Module Name:    IPConvert.vb
' Project:        VBASPNETIPtoLocation
' Copyright (c) Microsoft Corporation
'
' This project illustrates how to get the geographical location from a db file.
' You need install Sqlserver Express for run the web applicaiton.The code-sample
' only support Internet Protocol version 4.
' 
' This class use to calculate the IP number from IP address. 
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'*****************************************************************************/


Public Class IPConvert
    Public Shared Function ConvertToIPRange(ByVal ipAddress As String) As String
        Try
            Dim ipArray As String() = ipAddress.Split("."c)
            Dim number As Integer = ipArray.Length
            Dim ipRange As Double = 0
            If number <> 4 Then
                Return "error ipAddress"
            End If
            For i As Integer = 0 To 3
                Dim numPosition As Integer = Integer.Parse(ipArray(3 - i).ToString())
                If i = 4 Then
                    ipRange += numPosition
                Else
                    ipRange += ((numPosition Mod 256) * (Math.Pow(256, (i))))
                End If
            Next
            Return ipRange.ToString()
        Catch generatedExceptionName As Exception
            Return "error"
        End Try
    End Function

End Class
