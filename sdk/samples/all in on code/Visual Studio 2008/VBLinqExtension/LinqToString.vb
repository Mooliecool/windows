'****************************** Module Header ******************************\
' Module Name:    LinqToString.vb
' Project:        VBLinqExtension
' Copyright (c) Microsoft Corporation.
'
' It is a simple LINQ to String library to show the digis characters in a 
' string, to count occurrences of a word in a string, and to query for 
' sentences that contain a specified set of words.
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
Imports System.Linq
Imports System.Text
Imports System.Runtime.CompilerServices

#End Region

Public Module LinqToString

    ''' <summary>
    ''' Extension method of string to get the digit characters 
    ''' in a string.
    ''' </summary>
    ''' <returns>IEnumerable collection digit characters</returns>
    ''' 
    <Extension()> _
    Public Function GetDigits(ByVal text As String) As IEnumerable(Of Char)
        ' Get the chars if it is a digit
        Dim digits = From ch In text _
            Where Char.IsDigit(ch) _
            Select ch

        Return digits
    End Function


    ''' <summary>
    ''' Extension method of string to count occurrences of a word 
    ''' in a string.
    ''' </summary>
    ''' <param name="searchTerm">The search item string</param>
    ''' <returns>The search item occurences</returns>
    ''' 
    <Extension()> _
    Public Function GetWordOccurrence(ByVal text As String, ByVal searchTerm As String) As Integer
        ' Split the source string into single words
        Dim source As String() = text.Split(New Char() {"."c, "?"c, "!"c, " "c, ";"c, ":"c, _
        ","c}, StringSplitOptions.RemoveEmptyEntries)

        ' Query the occurences of the search item
        Dim matchQueryCount As Integer = (From word In source _
            Where word.ToLowerInvariant() = searchTerm.ToLowerInvariant() _
            Select word).Count()

        Return (matchQueryCount)
    End Function

    ''' <summary>
    ''' Extension method of string to query for sentences that contain 
    ''' a specified set of words.
    ''' </summary>
    ''' <param name="wordsToMatch">The search item set</param>
    ''' <returns>The set of sentences</returns>
    ''' 
    <Extension()> _
    Public Function GetCertainSentences(ByVal text As String, ByVal wordsToMatch As String()) As IEnumerable(Of String)
        ' Split the source string into single sentences
        Dim sentences = text.Split(New Char() {"."c, "?"c, "!"c})

        ' Query for sentences that contain the set of search itmes
        Dim sentenceQuery = From sentence In sentences _
            Let w = sentence.Split(New Char() {"."c, "?"c, "!"c, " "c, ";"c, ":"c, _
            ","c}, StringSplitOptions.RemoveEmptyEntries) _
            Where w.Distinct().Intersect(wordsToMatch).Count() = wordsToMatch.Count() _
            Select sentence

        Return sentenceQuery
    End Function
End Module
