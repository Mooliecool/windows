/****************************** Module Header ******************************\
* Module Name:    H1.cs
* Project:        CSASPNETHTMLEditorExtender
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to add a custom button to the toolbar of a 
* HTMLEditor in the Ajax Control Toolkit.
* 
* This class realize the toolbar button. We need to embed some WebResources to 
* this class.
* 
* The Ajax Control Toolkit contains a rich set of controls that you can use to
* build highly responsive and interactive Ajax-enabled ASP.NET Web Forms 
* applications. We can get start to learn and download the AjaxControlkit from
* this link:
* http://www.asp.net/ajaxlibrary/act.ashx
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using AjaxControlToolkit;
using AjaxControlToolkit.HTMLEditor.ToolbarButton;
using System.Web.UI;
using System.ComponentModel;
using System.Diagnostics.CodeAnalysis;


// Embed the images and js files as webresource to the project,
// then they would be compiled to the assembly.
#region [ Resources ]
[assembly: WebResource("HTMLEditorExtender.Images.ed_format_h1_n.gif",
                        "image/gif")]
[assembly: WebResource("HTMLEditorExtender.Images.ed_format_h1_a.gif",
                        "image/gif")]
[assembly: WebResource("HTMLEditorExtender.ToolBar_buttons.H1.pre.js",
                        "application/x-javascript")]

#endregion


namespace HTMLEditorExtender.ToolBar_buttons
{
    [ToolboxItem(false)]
    [ParseChildren(true)]
    [PersistChildren(false)]
    [RequiredScript(typeof(CommonToolkitScripts))]
    [ClientScriptResource("Sys.Extended.UI.HTMLEditor.ToolbarButton.H1",
                            "HTMLEditorExtender.ToolBar_buttons.H1.pre.js")]
    [SuppressMessage("Microsoft.Maintainability",
                        "CA1501:AvoidExcessiveInheritance")]
    public class H1 : EditorToggleButton
    {
        protected override void OnPreRender(EventArgs e)
        {
            // Set the normal style of the toolbar button from the 
            // embeded resource.
            NormalSrc = Page.ClientScript.GetWebResourceUrl(this.GetType(),
                "HTMLEditorExtender.Images.ed_format_h1_n.gif");

            // Set the style of the toolbar button when you click down to
            // the button.
            DownSrc = Page.ClientScript.GetWebResourceUrl(this.GetType(),
                "HTMLEditorExtender.Images.ed_format_h1_a.gif");

            // Set the effective style of the toolbar button from the 
            // embeded resource.
            ActiveSrc = DownSrc;
            base.OnPreRender(e);
        }
    }
}
