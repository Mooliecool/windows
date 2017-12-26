'*************************** Module Header ***********************************\
'* Module Name:    HighlightCodePage.aspx.vb
'* Project:        VBASPNETHighlightCodeInPage
'* Copyright (c) Microsoft Corporation
'*
'* Sometimes we input code like VB or HTML in our post and we need these code 
'* to be highlighted for a better reading experience.The project illustrates how
'* to highlight the code in a page. 
'
'* This page is used to let user highlight the code. 
'
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\*****************************************************************************


Public Class HighlightCodePage
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As EventArgs) _
        Handles Me.Load

        Me.lbError.Visible = False
        Me.lbResult.Visible = False
    End Sub

    Protected Sub btnHighLight_Click(ByVal sender As Object, _
                                     ByVal e As EventArgs)
        Dim _error As String = String.Empty

        ' Check the value of user's input data.
        If CheckControlValue(Me.ddlLanguage.SelectedValue,
                             Me.tbCode.Text, _error) Then
            ' Initialize the Hashtable variable which used to
            ' store the different languages of code and their 
            ' related regular expressions with matching options.
            Dim _htb As Hashtable = CodeManager.Init()

            ' Initialize the suitable collection object.
            Dim _rg As New RegExp()
            _rg = DirectCast(_htb(Me.ddlLanguage.SelectedValue), RegExp)
            Me.lbResult.Visible = True
            If Me.ddlLanguage.SelectedValue <> "html" Then
                ' Display the highlighted code in a label control.
                Me.lbResult.Text = CodeManager.Encode(
                    CodeManager.HighlightCode(
                        Server.HtmlEncode(
                            Me.tbCode.Text).
                        Replace("&quot;", """").
                        Replace("&#39;", "'"),
                        Me.ddlLanguage.SelectedValue, _rg))
            Else
                ' Display the highlighted code in a label control.
                Me.lbResult.Text = CodeManager.Encode(
                    CodeManager.HighlightHTMLCode(Me.tbCode.Text, _htb))
            End If
        Else
            Me.lbError.Visible = True
            Me.lbError.Text = _error
        End If
    End Sub
    Public Function CheckControlValue(ByVal selectValue As String, _
                                      ByVal inputValue As String, _
                                      ByRef [error] As String) As Boolean
        [error] = String.Empty
        If selectValue = "-1" Then
            [error] = "Please choose the language."
            Return False
        End If
        If String.IsNullOrEmpty(inputValue) Then
            [error] = "Please paste your code in the textbox control."
            Return False
        End If
        Return True
    End Function


End Class