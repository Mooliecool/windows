/****************************** Module Header ******************************\
* Module Name:  DropPanelContent
* Project:      CSASPNETAjaxScriptControl
* Copyright (c) Microsoft Corporation.
* Content panel template for DropPanel
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* History:
* * 2009/10/22 5:00 PM Vince Xu Created
\***************************************************************************/
using System.Web.UI;
using System.Web.UI.WebControls;
namespace CSASPNETAjaxScriptControl
{
    public class DropPanelContent : Panel,INamingContainer
    {
        protected override void RenderContents(HtmlTextWriter output)
        {
                base.RenderContents(output);
        }
    }
}
