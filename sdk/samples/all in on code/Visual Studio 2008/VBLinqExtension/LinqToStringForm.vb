'****************************** Module Header ******************************\
' Module Name:    LinqToStringForm.cs
' Project:        CSLinqExtension
' Copyright (c) Microsoft Corporation.
'
' This LinqToStringForm uses the simple LINQ to String library to show the 
' digis characters in a string, to count occurrences of a word in a string, 
' and to query for sentences that contain a specified set of words.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

#Region "Imports directives"
Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Data
Imports System.Drawing
Imports System.Linq
Imports System.Text
Imports System.Windows.Forms
#End Region

Public Class LinqToStringForm

#Region "Show Digists In a String"

    Private Sub btnDigits_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDigits.Click

        Try
            Dim text As String = txtBoxDigit.Text

            ' Check whether the string is empty
            If String.IsNullOrEmpty(text) Then
                MessageBox.Show("Input string is empty!")
                Return
            End If

            ' Call the GetGigits extension method to get all the digits
            Dim digits = text.GetDigits()

            ' Display all the digits in a string
            Dim output As String = ""

            For Each ch In digits
                output += ch.ToString()
            Next

            MessageBox.Show(String.Format("The digits in the string: " & output))
        Catch ex As Exception
            MessageBox.Show(ex.ToString())
        End Try

    End Sub

#End Region

#Region "Count Occurences of a Word"

    Private Sub btnSearchOccurence_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSearchOccurence.Click
        Try
            Dim searchItem As String = txtBoxOccurence.Text

            ' Check whether the search source string is empty
            If String.IsNullOrEmpty(txtSearchSource.Text) Then
                MessageBox.Show("Search source is empty!")
                Return
            End If

            ' Check whether the search item string is empty
            If String.IsNullOrEmpty(searchItem) Then
                MessageBox.Show("Search item is empty!")
                Return
            End If

            ' Call the GetWordOccurrence extension method to get the
            ' search item occurences in the search source text
            Dim wordCount As Integer = txtSearchSource.Text.GetWordOccurrence(searchItem)

            ' Display the query result
            MessageBox.Show(String.Format("The occurences of '{0}': {1}", searchItem, wordCount))
        Catch ex As Exception
            MessageBox.Show(ex.ToString())
        End Try
    End Sub

#End Region

#Region "Query for Sentences That Contain a Specified Set of Words"

    Private Sub btnSearchSentence_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSearchSentence.Click

        Try
            ' Split the search item sets into a string array
            Dim wordToSearch As String() = txtBoxSearchItems.Text.Split(New Char() {","c}, StringSplitOptions.RemoveEmptyEntries)

            ' Check whether the search source string is empty
            If String.IsNullOrEmpty(txtSearchSource.Text) Then
                MessageBox.Show("Search source is empty!")
                Return
            End If

            ' Check if the search item set is empty
            If wordToSearch.Length = 0 Then
                MessageBox.Show("Search items set is empty!")
                Return
            End If

            ' Call the GetCertainSentences extension method to query 
            ' for sentences that contain a specified set of words.
            Dim sentences = txtSearchSource.Text.GetCertainSentences(wordToSearch)

            ' Display the query result
            Dim output As String = ""

            For Each sen In sentences
                output += sen & vbCr & vbLf & vbCr & vbLf
            Next

            MessageBox.Show(String.Format("The sentence(s) with (" & txtBoxSearchItems.Text & _
                                          ": " & vbCr & vbLf & vbCr & vbLf & output))
        Catch ex As Exception
            MessageBox.Show(ex.ToString())
End Try

    End Sub

#End Region

 
End Class