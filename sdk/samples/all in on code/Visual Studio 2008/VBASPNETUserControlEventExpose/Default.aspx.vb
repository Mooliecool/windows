'**************************** Module Header ********************************\
' Module Name:  Default.aspx.vb
' Project:      VBASPNETuserControlEventsExpose
' Copyright (c) Microsoft Corporation.
' 
' This page load the usercontrol and add the usercontrol into webpage.
' Then subscribe the MyEvent of usercontrol to react the button click of usercontrol in the webpage.
' When click the button of usercontrol, the webpage will display the selected value of dropdownlist.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\***************************************************************************/

Partial Public Class _Default
    Inherits System.Web.UI.Page

    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Dim myUserControl As MyUserControl = CType(LoadControl("MyUserControl.ascx"), MyUserControl)
        If (Not (myUserControl) Is Nothing) Then
            AddHandler myUserControl.MyEvent, AddressOf Me.userControlBtnClick
            Me.PlaceHolder1.Controls.Add(myUserControl)
        End If
    End Sub
    Public Sub userControlBtnClick(ByVal sender As Object, ByVal e As EventArgs)
        Response.Write(("Main Page Event Handler<BR/>Selected Value:" _
                        + (ddlTemp.SelectedItem.Text + "<BR/>")))
    End Sub

End Class