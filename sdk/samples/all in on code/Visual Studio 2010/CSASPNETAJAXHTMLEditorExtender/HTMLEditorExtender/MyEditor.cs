/****************************** Module Header ******************************\
* Module Name:    MyEditor.cs
* Project:        CSASPNETHTMLEditorExtender
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to add a custom button to the toolbar of a 
* HTMLEditor in the Ajax Control Toolkit.
* 
* This class make a new Editor control to hold the toolbar button 
* which we created.
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
using AjaxControlToolkit.HTMLEditor;
using HTMLEditorExtender.ToolBar_buttons;

namespace HTMLEditorExtender
{
    public class MyEditor : Editor
    {
        protected override void FillTopToolbar()
        {
            base.FillTopToolbar();
            TopToolbar.Buttons.Add(new H1());
        }
    }
}
