'*************************** Module Header ********************************\
' Module Name:    Default.aspx.vb
' Project:        VBASPNETAddControlDynamically
' Copyright (c) Microsoft Corporation
'
' This project shows how to add controls dynamically in ASP.NET pages. It 
' imitates a scenario that customers need to input more than one addr info 
' without max limit. So we use button to add new address input TextBoxes.
' After a customer inputs all the addresses, we also use button to update 
' these info into database, which is run as displaying these addresses in 
' this sample.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
'\**************************************************************************

Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub Page_PreLoad(ByVal sender As Object, ByVal e As EventArgs) Handles Me.PreLoad
        ' Use ViewState to store the number of the addresses.
        If ViewState("AddressCount") Is Nothing Then
            ViewState("AddressCount") = 0
        End If

        ' Get the the number of the addresses.
        Dim addresscount As Integer = CInt(ViewState("AddressCount"))

        ' Iterat adding addresses input component.
        For i As Integer = 0 To addresscount - 1
            AddAdress((i + 1).ToString())
        Next
    End Sub

    Protected Sub btnAddAddress_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnAddAddress.Click
        If ViewState("AddressCount") IsNot Nothing Then
            Dim btncount As Integer = CInt(ViewState("AddressCount"))

            ' Add a new component to pnlAddressContainer.
            AddAdress((btncount + 1).ToString())
            ViewState("AddressCount") = btncount + 1
        Else
            Response.Write("ERROR")
            Response.End()
        End If
    End Sub

    Protected Sub btnSave_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnSave.Click
        Dim addresscount As Integer = CInt(ViewState("AddressCount"))

        ' Display all the addresses on the page.
        ' This is an imitation that we update these addresses into database.
        For i As Integer = 0 To addresscount - 1
            Dim tb As TextBox = TryCast(pnlAddressContainer.FindControl("TextBox" & (i + 1).ToString()), TextBox)
            Dim address As String = If(tb.Text = "", "Empty", tb.Text)
            Response.Write("Address" & (i + 1).ToString() & " is " & address & ".<br />")
        Next

        ' Clear the ViewState.
        ViewState("AddressCount") = Nothing
    End Sub

    Protected Sub AddAdress(ByVal id As String)
        ' Label to display address No.
        Dim lb As New Label()
        lb.Text = "Address" & id & ": "

        ' TextBox for inputting the address.
        Dim tb As New TextBox()
        tb.ID = "TextBox" & id

        If id <> "1" Then
            ' Have a try on the code without this condition.
            ' We will get a odd behaviour on this after clicking Save button.
            tb.Text = Request.Form(tb.ID)
        End If

        ' Button to check the Address.
        ' Also to illustrate how to bind events to a dynamic control.
        Dim btn As New Button()
        btn.Text = "Check"
        btn.ID = "Button" & id

        ' Bind event using AddHandler and AddressOf key statement.
        AddHandler btn.Click, AddressOf ClickEvent

        Dim lt As New Literal() With {.Text = "<br />"}

        ' Add these controls to pnlAddressContainer as a component.
        pnlAddressContainer.Controls.Add(lb)
        pnlAddressContainer.Controls.Add(tb)
        pnlAddressContainer.Controls.Add(btn)
        pnlAddressContainer.Controls.Add(lt)

    End Sub

    Protected Sub ClickEvent(ByVal sender As Object, ByVal e As EventArgs)
        ' Get button instance from sender.
        Dim btn As Button = TryCast(sender, Button)

        ' Get TextBox instance and its value via FindControl() method.
        Dim tb As TextBox = TryCast(pnlAddressContainer.FindControl(btn.ID.Replace("Button", "TextBox")), TextBox)

        Dim address As String = If(tb.Text = "", "Empty", tb.Text)

        ' Alert a msg to show the address in the corresponding TextBox.
        Dim sb As New System.Text.StringBuilder()
        sb.Append("<script type=""text/javascript"">")
        sb.Append("alert(""Address" & btn.ID.Replace("Button", "") & " is " & address & "."");")
        sb.Append("</script>")

        If Not ClientScript.IsClientScriptBlockRegistered(Me.GetType(), "AlertClick") Then
            ClientScript.RegisterClientScriptBlock(Me.GetType(), "AlertClick", sb.ToString())
        End If
    End Sub


End Class