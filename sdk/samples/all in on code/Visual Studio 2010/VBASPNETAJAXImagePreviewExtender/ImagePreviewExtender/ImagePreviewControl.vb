'***************************** Module Header ******************************\
'* Module Name:    ImagePreviewControl.cs
'* Project:        VBASPNETImagePreviewExtender
'* Copyright (c) Microsoft Corporation
'*
'* The project illustrates how to design an AJAX Control Extender.
'* In this sample, it is extender for images.
'* The images which use the extender control will show in a thumbnail mode at
'* first, if user click the image, a big image will pop up and show the true
'* size of the image.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\****************************************************************************

<TargetControlType(GetType(Control))> _
Public Class ImagePreviewControl
    Inherits ExtenderControl

    ''' <summary>
    ''' Define the css class which the image use in thumbnail mode.
    ''' </summary>
    Public Property ThumbnailCssClass() As String
        Get
            Return m_ThumbnailCssClass
        End Get
        Set(ByVal value As String)
            m_ThumbnailCssClass = value
        End Set
    End Property
    Private m_ThumbnailCssClass As String

    ''' <summary>
    ''' return the resource url of the close icon.
    ''' </summary>
    Private ReadOnly Property closeImage() As String
        Get
            Return Page.ClientScript.GetWebResourceUrl(Me.[GetType](), "ImagePreviewExtender.Close.png")
        End Get
    End Property

    Protected Overrides Function GetScriptDescriptors(ByVal targetControl As System.Web.UI.Control) As IEnumerable(Of ScriptDescriptor)
        Dim descriptor As New ScriptBehaviorDescriptor("ImagePreviewExtender.ImagePreviewBehavior", targetControl.ClientID)
        descriptor.AddProperty("ThumbnailCssClass", ThumbnailCssClass)

        descriptor.AddProperty("closeImage", closeImage)
        Return New List(Of ScriptDescriptor) From {descriptor}
    End Function

    ' Generate the script reference
    Protected Overrides Function GetScriptReferences() As IEnumerable(Of ScriptReference)
        Dim scriptRef As New ScriptReference("ImagePreviewExtender.ImagePreviewBehavior.js", Me.GetType().Assembly.FullName)

        Return New List(Of ScriptReference) From {scriptRef}
    End Function
End Class
