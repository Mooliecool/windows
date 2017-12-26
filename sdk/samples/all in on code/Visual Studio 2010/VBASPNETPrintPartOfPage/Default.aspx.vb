'**************************** Module Header ******************************\
' Module Name: Default.aspx.vb
' Project:     VBASPNETPrintPartOfPage
' Copyright (c) Microsoft Corporation
'
' This project illustrates how to print part of page by use Cascading Style
' Sheets and JavaScript. we need set div layer in the part of web page, and
' use JavaScript code to print the valid part of it.
' ' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, ' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED ' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.'*****************************************************************************/


Public Class _Default
    Inherits System.Web.UI.Page
    'define some strinngs, use to add div elements.
    Public PrintImageBegin As String
    Public PrintImageEnd As String
    Public PrintListBegin As String
    Public PrintListEnd As String
    Public PrintButtonBegin As String
    Public PrintButtonEnd As String
    Public PrintSearchBegin As String
    Public PrintSearchEnd As String
    Public Const EnablePirnt As String = "<div class=nonPrintable>"
    Public Const EndDiv As String = "</div>"

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        ' Check the status of CheckBox, set div elements.
        If chkDisplayImage.Checked Then
            PrintImageBegin = String.Empty
            PrintImageEnd = String.Empty
        Else
            PrintImageBegin = EnablePirnt
            PrintImageEnd = EndDiv
        End If
        If chkDisplayList.Checked Then
            PrintListBegin = String.Empty
            PrintListEnd = String.Empty
        Else
            PrintListBegin = EnablePirnt
            PrintListEnd = EndDiv
        End If
        If chkDisplayButton.Checked Then
            PrintButtonBegin = String.Empty
            PrintButtonEnd = String.Empty
        Else
            PrintButtonBegin = EnablePirnt
            PrintButtonEnd = EndDiv
        End If
        If chkDisplaySearch.Checked Then
            PrintSearchBegin = String.Empty
            PrintSearchEnd = String.Empty
        Else
            PrintSearchBegin = EnablePirnt
            PrintSearchEnd = EndDiv
        End If
    End Sub

    Protected Sub btnPrint_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnPrint.Click
        ClientScript.RegisterStartupScript(Me.[GetType](), "PrintOperation", "print()", True)
    End Sub
End Class