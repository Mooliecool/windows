'/****************************** Module Header ******************************\
' Module Name:    LinqToCSV.vb
' Project:        VBLinqExtension
' Copyright (c) Microsoft Corporation.
'
' It is a simple LINQ to CSV library to use StreamReader to read CSV contents 
' into string arrays line by line. It uses Regular Expression to parse the 
' data based on CSV file standard.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

#Region "ImportDirectives"
Imports System
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports System.IO
Imports System.Text.RegularExpressions
Imports System.Runtime.CompilerServices

#End Region

' Class LinqToCSV to make extension methods
Public Module LinqToCSV
    'Inherits StreamReader

    ''' <summary>
    ''' Extension method of StreamReader to read the CSV file into 
    ''' string arrays line by line.
    ''' </summary>
    ''' <remarks>IEnumerable collection of string arrays</remarks>
    ''' 
    <Extension()> _
    Public Function Lines(ByRef reader As StreamReader) As IEnumerable(Of String())

        If reader IsNot Nothing Then

            Dim line As String = reader.ReadLine()

            'While (InlineAssignHelper(line, reader.ReadLine())) IsNot Nothing
            While line IsNot Nothing

                ' Exclude the comment in CSV files
                If Not line.StartsWith("#") Then
                    ' yield return the data of each line
                    Return LineToArray(line)
                End If
            End While
        End If

        Return Nothing
    End Function

    '    Private Shared Function InlineAssignHelper(Of T)(ByRef target As T, ByVal value As T) As T
    '        target = value
    '        Return value
    '    End Function
    'End Class

    ''' <summary>
    ''' Use Regular Expression to parse each line of the CSV data.
    ''' </summary>
    ''' <param name="line">The string of a line of CSV file</param>
    ''' <returns>The string array that holds the data</returns>
    ''' 
    Private Function LineToArray(ByVal line As String) As String()
        Dim pattern As String = ",(?=(?:[^""]*""[^""]*"")*(?![^""]*""))"

        Dim regex As New Regex(pattern)

        Return (regex.Split(line))
    End Function
End Module
