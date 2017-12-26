'****************************** Module Header ******************************'
' Module Name:  TimePicker
' Project:	VBASPNETAjaxExtender
' Copyright (c) Microsoft Corporation.
' 
' TimePicker.cs
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 2010.2.5 Vince Xu Created
'***************************************************************************'

Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Linq
Imports System.Text
Imports System.Web
Imports System.Web.UI
Imports System.Web.UI.WebControls
Imports System.Web.UI.HtmlControls


<Assembly: WebResource("VBASPNETAjaxExtender.TimePicker.TimePicker.js", "application/x-javascript")> 
<Assembly: WebResource("VBASPNETAjaxExtender.TimePicker.TimePicker.css", "text/css", PerformSubstitution:=True)> 
<Assembly: WebResource("VBASPNETAjaxExtender.TimePicker.backgroundImage.png", "image/png")> 
<Assembly: WebResource("VBASPNETAjaxExtender.TimePicker.centerImage.png", "image/png")> 
<Assembly: WebResource("VBASPNETAjaxExtender.TimePicker.hourImage.png", "image/png")> 
<Assembly: WebResource("VBASPNETAjaxExtender.TimePicker.minImage.png", "image/png")> 
<Assembly: WebResource("VBASPNETAjaxExtender.TimePicker.closeImage.png", "image/png")> 

Namespace VBASPNETAjaxExtender

    <ToolboxData("<{0}:TimePicker runat=server></{0}:TimePicker>")> _
    <TargetControlType(GetType(TextBox))> _
    Public Class TimePicker
        Inherits ExtenderControl
        Public Sub New()

        End Sub

#Region "IExtenderControl Member"

        'Generate descriptor to render the related $create script function that will create behavior on client. 
        Protected Overloads Overrides Function GetScriptDescriptors(ByVal targetControl As Control) As IEnumerable(Of ScriptDescriptor)
            Dim descriptor As New ScriptControlDescriptor("VBASPNETAjaxExtender.TimePicker", targetControl.ClientID)

            'Register ErrorPresentControl element to client 
            If ErrorPresentControlID <> String.Empty Then
                descriptor.AddElementProperty("errorSpan", Me.NamingContainer.FindControl(ErrorPresentControlID).ClientID)
            End If

            'Register property 
            descriptor.AddProperty("cssClass", CssClass)
            descriptor.AddProperty("timeType", TimeType)

            'Register client event 
            If OnClientShowing <> String.Empty Then
                descriptor.AddEvent("showing", OnClientShowing)
            End If
            If OnClientShown <> String.Empty Then
                descriptor.AddEvent("shown", OnClientShown)
            End If
            If OnClientHiding <> String.Empty Then
                descriptor.AddEvent("hiding", OnClientHiding)
            End If
            If OnClientHidden <> String.Empty Then
                descriptor.AddEvent("hidden", OnClientHidden)
            End If
            If OnClientHourSelectionChanged <> String.Empty Then
                descriptor.AddEvent("hourSelectionChanged", OnClientHourSelectionChanged)
            End If
            If OnClientMinuteSelectionChanged <> String.Empty Then
                descriptor.AddEvent("minuteSelectionChanged", OnClientMinuteSelectionChanged)
            End If


        End Function

        ' Generate the script reference 
        Protected Overloads Overrides Function GetScriptReferences() As IEnumerable(Of ScriptReference)

            Dim scriptRef As New ScriptReference("VBASPNETAjaxExtender.TimePicker.js", Me.GetType().Assembly.FullName)
            Dim scriptRefs As New List(Of ScriptReference)
            scriptRefs.Add(scriptRef)
            Return scriptRefs
        End Function

#End Region

#Region "Properties"


        <Browsable(True)> _
        <Category("Appearance")> _
        <Description("")> _
        Public Overridable Property CssClass() As String
            Get
                Return DirectCast((If(ViewState("CssClass"), [String].Empty)), [String])
            End Get
            Set(ByVal value As String)
                ViewState("CssClass") = value
            End Set
        End Property


        <Browsable(True)> _
        <Category("Appearance")> _
        <Description("")> _
        Public Overridable Property ErrorPresentControlID() As String
            Get
                Return DirectCast((If(ViewState("ErrorPresentControlID"), [String].Empty)), [String])
            End Get
            Set(ByVal value As String)
                ViewState("ErrorPresentControlID") = value
            End Set
        End Property


        <Browsable(True)> _
        <Category("Appearance")> _
        <Description("")> _
        <DefaultValue(VBASPNETAjaxExtender.TimeType.H24)> _
        Public Property TimeType() As VBASPNETAjaxExtender.TimeType
            Get
                Return DirectCast((If(ViewState("TimeType"), VBASPNETAjaxExtender.TimeType.H24)), VBASPNETAjaxExtender.TimeType)
            End Get
            Set(ByVal value As VBASPNETAjaxExtender.TimeType)
                ViewState("TimeType") = value
            End Set
        End Property

        <DefaultValue("")> _
        <Browsable(True)> _
        <Category("Appearance")> _
        <Description("")> _
        Public Overridable Property OnClientShowing() As String
            Get
                Return DirectCast((If(ViewState("OnClientShowing"), String.Empty)), String)
            End Get
            Set(ByVal value As String)
                ViewState("OnClientShowing") = value
            End Set
        End Property

        <DefaultValue("")> _
        <Browsable(True)> _
        <Category("Appearance")> _
        <Description("")> _
        Public Overridable Property OnClientShown() As String
            Get
                Return DirectCast((If(ViewState("OnClientShown"), String.Empty)), String)
            End Get
            Set(ByVal value As String)
                ViewState("OnClientShown") = value
            End Set
        End Property

        <DefaultValue("")> _
        <Browsable(True)> _
        <Category("Appearance")> _
        <Description("")> _
        Public Overridable Property OnClientHiding() As String
            Get
                Return DirectCast((If(ViewState("OnClientHiding"), String.Empty)), String)
            End Get
            Set(ByVal value As String)
                ViewState("OnClientHiding") = value
            End Set
        End Property

        <DefaultValue("")> _
        <Browsable(True)> _
        <Category("Appearance")> _
        <Description("")> _
        Public Overridable Property OnClientHidden() As String
            Get
                Return DirectCast((If(ViewState("OnClientHidden"), String.Empty)), String)
            End Get
            Set(ByVal value As String)
                ViewState("OnClientHidden") = value
            End Set
        End Property

        <DefaultValue("")> _
        <Browsable(True)> _
        <Category("Appearance")> _
        <Description("")> _
        Public Overridable Property OnClientHourSelectionChanged() As String
            Get
                Return DirectCast((If(ViewState("OnClientHourSelectionChanged"), String.Empty)), String)
            End Get
            Set(ByVal value As String)
                ViewState("OnClientHourSelectionChanged") = value
            End Set
        End Property

        <DefaultValue("")> _
        <Browsable(True)> _
        <Category("Appearance")> _
        <Description("")> _
        Public Overridable Property OnClientMinuteSelectionChanged() As String
            Get
                Return DirectCast((If(ViewState("OnClientMinuteSelectionChanged"), String.Empty)), String)
            End Get
            Set(ByVal value As String)
                ViewState("OnClientMinuteSelectionChanged") = value
            End Set
        End Property

#End Region

#Region "Render Phase"


        ' Add Css reference 
        Private Sub RenderCssReference()
            Dim cssUrl As String = Page.ClientScript.GetWebResourceUrl(Me.[GetType](), "VBASPNETAjaxExtender.TimePicker.TimePicker.css")

            Dim link As New HtmlLink()
            link.Href = cssUrl
            link.Attributes.Add("type", "text/css")
            link.Attributes.Add("rel", "stylesheet")
            Page.Header.Controls.Add(link)
        End Sub



        Protected Overloads Overrides Sub OnPreRender(ByVal e As EventArgs)
            MyBase.OnPreRender(e)
            'Register css reference 

            RenderCssReference()
        End Sub

#End Region

    End Class
End Namespace
