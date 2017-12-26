'/************************************* Module Header **************************************\
'* Module Name:	MainForm.vb
'* Project:		VBWinFormGroupRadioButtons
'* Copyright (c) Microsoft Corporation.
'* 
'* This sample demonstrates how to group the RadioButtons in the different containers.
'* 
'* For more information about the RadioButton control, see:
'* 
'*  Windows Forms RadioButton control
'*  http://msdn.microsoft.com/en-us/library/f5h102xz.aspx
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\******************************************************************************************/

Imports System.Windows.Forms

Public Class MainForm
    ' store the old RadioButton
    Private radTmp As RadioButton = Nothing

    Private Sub MainForm_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ' we checked the rad1 in the MainForm.Designer.vb file
        ' then this RadioButton would be the old one
        radTmp = Me.rad1
    End Sub

    ' let the 4 Radiobuttons use this method to deal with their CheckedChanged event
    ' in the MainForm.Designer.vb file
    Private Sub radioButton_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles rad3.CheckedChanged, rad2.CheckedChanged, rad1.CheckedChanged, rad4.CheckedChanged
        If (radTmp IsNot Nothing) Then
            ' uncheck the old one
            radTmp.Checked = False
            radTmp = DirectCast(sender, RadioButton)

            ' find out the checked one
            If (radTmp.Checked) Then
                Me.lb.Text = radTmp.Name + " has been selected"
            End If
        End If
    End Sub
End Class