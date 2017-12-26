'**************************** Module Header ******************************\
' Module Name: Default.aspx.vb
' Project:     VBASPNETDisplayAddtionalTextInCalendar
' Copyright (c) Microsoft Corporation
'
' The project illustrates how to display additional text in a calendar control.
' As we know, people may want to add different customize text when date of 
' Calendar has been selected. Here we give an easy way to set Calendar's specified
' text, background color, border properties and make Calendar looks better.
' 
' Calendar control host in this page.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'****************************************************************************




Imports System.Drawing

Public Class _Default
    Inherits System.Web.UI.Page
    ' Define a list of DateTime to store selected date.
    Dim SelectDates As New List(Of DateTime)()

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SelectDates = Me.GetDateTimeList
    End Sub

    ''' <summary>
    ''' The selected dates is stored in ViewState.
    ''' </summary>
    Public Property GetDateTimeList() As List(Of DateTime)
        Get
            If ViewState("SelectDate") Is Nothing Then
                Dim selectdates As New List(Of DateTime)()
                selectdates.Add(DateTime.MaxValue)
                ViewState("SelectDate") = selectdates
            End If
            Return DirectCast(ViewState("SelectDate"), List(Of DateTime))
        End Get
        Set(ByVal value As List(Of DateTime))
            ViewState("SelectDate") = value
        End Set
    End Property

    ''' <summary>
    ''' Calendar SelectionChanged event, add new date in SelectedDate and 
    ''' remove the same date.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Protected Sub myCalendar_SelectionChanged(ByVal sender As Object, ByVal e As EventArgs)
        Dim dateSeletion As DateTime = myCalendar.SelectedDate
        If SelectDates.Contains(dateSeletion) Then
            SelectDates.Remove(dateSeletion)
        Else
            SelectDates.Add(dateSeletion)
        End If
    End Sub

    ''' <summary>
    ''' Clear selected dates.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Protected Sub myCalendar_PreRender(ByVal sender As Object, ByVal e As EventArgs)
        myCalendar.SelectedDates.Clear()
    End Sub

    ''' <summary>
    ''' Add your customize text in Calendar control.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Protected Sub myCalendar_DayRender(ByVal sender As Object, ByVal e As DayRenderEventArgs)
        For Each time As DateTime In SelectDates
            myCalendar.SelectedDates.Add(time)
            If e.Day.[Date] = time Then
                e.Cell.BorderWidth = 1
                e.Cell.BackColor = Color.SlateBlue
                e.Cell.Controls.Add(New LiteralControl("<br />your text"))
            End If
        Next
    End Sub

    ''' <summary>
    ''' render selected dates.
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Protected Sub btnCheck_Click(ByVal sender As Object, ByVal e As EventArgs)
        For Each [date] As DateTime In SelectDates
            If [date] <> DateTime.MaxValue Then
                Response.Write([date].ToShortDateString())
                Response.Write("<br />")
            End If
        Next
    End Sub

End Class