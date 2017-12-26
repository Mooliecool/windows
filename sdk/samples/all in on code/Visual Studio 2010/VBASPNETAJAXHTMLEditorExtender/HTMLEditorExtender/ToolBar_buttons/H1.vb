'***************************** Module Header ******************************\
'* Module Name:    H1.vb
'* Project:        VBASPNETHTMLEditorExtender
'* Copyright (c) Microsoft Corporation
'*
'* The project illustrates how to add a custom button to the toolbar of a 
'* HTMLEditor in the Ajax Control Toolkit.
'* 
'* This class realize the toolbar button. We need to embed some WebResources to 
'* this class.
'* 
'* The Ajax Control Toolkit contains a rich set of controls that you can use to
'* build highly responsive and interactive Ajax-enabled ASP.NET Web Forms 
'* applications. We can get start to learn and download the AjaxControlkit from
'* this link:
'* http://www.asp.net/ajaxlibrary/act.ashx
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\****************************************************************************


Imports AjaxControlToolkit
Imports AjaxControlToolkit.HTMLEditor.ToolbarButton
Imports System.Web.UI
Imports System.ComponentModel
Imports System.Diagnostics.CodeAnalysis


' Embed the images and js files as webresource to the project,
' then they would be compiled to the assembly.
#Region "[ Resources ]"
<Assembly: WebResource("HTMLEditorExtender.ed_format_h1_n.gif",
    "image/gif")> 
<Assembly: WebResource("HTMLEditorExtender.ed_format_h1_a.gif",
    "image/gif")> 
<Assembly: WebResource("HTMLEditorExtender.H1.pre.js",
    "application/x-javascript")> 

#End Region


<ToolboxItem(False)> _
<ParseChildren(True)> _
<PersistChildren(False)> _
<RequiredScript(GetType(CommonToolkitScripts))> _
<ClientScriptResource("Sys.Extended.UI.HTMLEditor.ToolbarButton.H1",
        "HTMLEditorExtender.H1.pre.js")> _
<SuppressMessage("Microsoft.Maintainability", "CA1501:AvoidExcessiveInheritance")> _
Public Class H1
    Inherits EditorToggleButton
    Protected Overrides Sub OnPreRender(ByVal e As EventArgs)
        ' Set the normal style of the toolbar button from the 
        ' embeded resource.
        NormalSrc = Page.ClientScript.GetWebResourceUrl(Me.[GetType](),
                            "HTMLEditorExtender.ed_format_h1_n.gif")

        ' Set the style of the toolbar button when you click down to
        ' the button.
        DownSrc = Page.ClientScript.GetWebResourceUrl(Me.[GetType](),
                            "HTMLEditorExtender.ed_format_h1_a.gif")

        ' Set the effective style of the toolbar button from the 
        ' embeded resource.
        ActiveSrc = DownSrc
        MyBase.OnPreRender(e)
    End Sub
End Class
