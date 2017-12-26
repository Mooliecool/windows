'**********************************************************************************'
' Module Name:  HexStringByteArrayConverter.vb
' Project:      VBHexStringByteArrayConverter
' Copyright (c) Microsoft Corporation.
'
' The class provides helper functions of converting hex string to byte array, 
' converting byte array to hex string, and verifying the hex string input. 
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**********************************************************************************'

Imports System.Text
Imports System.Text.RegularExpressions


Friend Class HexStringByteArrayConverter
    Private Const hexDigits As String = "0123456789ABCDEF"

    ''' <summary>
    ''' Convert a byte array to hex string. Example output: "7F2C4A00".
    ''' 
    ''' Alternatively, you can also use the BitConverter.ToString method to 
    ''' convert byte array to string of hexadecimal pairs separated by hyphens, 
    ''' where each pair represents the corresponding element in value; for 
    ''' example, "7F-2C-4A-00".
    ''' </summary>
    ''' <param name="bytes">An array of bytes</param>
    ''' <returns>Hex string</returns>
    Public Shared Function BytesToHexString(ByVal bytes() As Byte) As String
        Dim sb As New StringBuilder(bytes.Length * 2)
        For Each b As Byte In bytes
            sb.AppendFormat("{0:X2}", b)
        Next b
        Return sb.ToString()
    End Function

    ''' <summary>
    ''' Convert a hex string to byte array.
    ''' </summary>
    ''' <param name="str">hex string. For example, "FF00EE11"</param>
    ''' <returns>An array of bytes</returns>
    Public Shared Function HexStringToBytes(ByVal str As String) As Byte()
        ' Determine the number of bytes
        Dim bytes((str.Length >> 1) - 1) As Byte
        For i As Integer = 0 To str.Length - 1 Step 2
            Dim highDigit As Integer = hexDigits.IndexOf(Char.ToUpperInvariant(str.Chars(i)))
            Dim lowDigit As Integer = hexDigits.IndexOf(Char.ToUpperInvariant(str.Chars(i + 1)))
            If highDigit = -1 OrElse lowDigit = -1 Then
                Throw New ArgumentException("The string contains an invalid digit.", "s")
            End If
            bytes(i >> 1) = CByte((highDigit << 4) Or lowDigit)
        Next i
        Return bytes
    End Function

    ''' <summary>
    ''' Verify the format of the hex string.
    ''' </summary>
    Public Shared Function VerifyHexString(ByVal str As String) As Boolean
        Dim regex_Renamed As New Regex("\A[0-9a-fA-F]+\z")
        Return regex_Renamed.IsMatch(str) AndAlso ((str.Length And 1) <> 1)
    End Function
End Class
