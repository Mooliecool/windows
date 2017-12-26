'**************************** Module Header ******************************\
' Module Name: DisableScript.aspx.vb
' Project:     VBASPNETDisableScriptAfterExecution
' Copyright (c) Microsoft Corporation
'
' The sample code illustrates how to register script code at code behind and to 
' be disabled after execution. Sometimes users who register scripts do not want 
' them execute again, Actually they with to achieve this either in an automatic
' manner or by imitating an action for example, by clicking a link or button. 
' This maybe due to functional purpose, user experience or security concerns. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'****************************************************************************




Public Class DisableScript
    Inherits System.Web.UI.Page
    ''' <summary>
    ''' Register JavaScript functions at code behind page(Page_Load event). 
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Protected Sub Page_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Dim clientManager As ClientScriptManager = Page.ClientScript

        ' JavaScript function part 1, this function is used to execute user's logical
        ' code with an additional parameter, this parameter is a callback function 
        ' which is make a condition that if the main function had executed.
        Dim mainScript As String = vbCr & vbLf & " var flag = true;" & vbCr & vbLf &
                                                 " var mainFunc = function main(callBack) {" & vbCr & vbLf &
                                                 " var callBackPara = callBack;" & vbCr & vbLf &
                                                 " if (flag) {" & vbCr & vbLf &
                                                 " // User code." & vbCr & vbLf &
                                                 " alert('This is user code, in this sample code, your page will move down.');" & vbCr & vbLf &
                                                 " " & vbCr & vbLf &
                                                 " for (var i = 1; i <= 900; i++)" & vbCr & vbLf &
                                                 " {" & vbCr & vbLf &
                                                 " window.moveBy(0, 1);" & vbCr & vbLf &
                                                 " }" & vbCr & vbLf &
                                                 " window.moveBy(0,-750);" & vbCr & vbLf &
                                                 " " & vbCr & vbLf &
                                                 " // Disable JS function." & vbCr & vbLf &
                                                 " flag = undefined;" & vbCr & vbLf &
                                                 " }" & vbCr & vbLf &
                                                 " else {" & vbCr & vbLf &
                                                 " alert(callBackPara);" & vbCr & vbLf &
                                                 " }}"
        clientManager.RegisterClientScriptBlock(Me.[GetType](), "mainScript", mainScript, True)

        ' JavaScript function part 2, the callback function, check if flag variable is undefined.
        Dim callbackScript As String = " var callFunc = function callBackFunc() {" & vbCr & vbLf &
                                       " if (!flag) {" & vbCr & vbLf &
                                       " return 'The JavaScript function has been disabled..';" & vbCr & vbLf &
                                       " }}"
        clientManager.RegisterStartupScript(Me.[GetType](), "callBackScript", callbackScript, True)

    End Sub

End Class