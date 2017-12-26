/****************************** Module Header ******************************\
* Module Name: ShowMe.aspx.cs
* Project:     CSASPNETEmbedLanguageInUrl
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to embed the language code in URL such
* as http://domain/en-us/ShowMe.aspx. The page will display different
* content according to the language code, the sample use url-routing 
* and resource files to localize the content of web page.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\*****************************************************************************/



using System;
using System.Collections.Generic;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Xml;
using System.IO;
using Resources;
using System.Resources;
using System.Globalization;

namespace CSASPNETEmbedLanguageInUrl
{
    public partial class Default : BasePage
    {
        /// <summary>
        /// Load this page with a certain language.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void Page_Load(object sender, EventArgs e)
        {
            string[] elementArray = Session["info"].ToString().Split(',');
            string language = elementArray[0];
            string pageName = elementArray[1];
            if (language == "error")
            {
                Response.Write("The url routing error: please restart the web application with Start.aspx page");
                return;
            }
            string xmlPath = Server.MapPath("~/XmlFolder/Language.xml");
            string strTitle = string.Empty;
            string strText = string.Empty;
            string strElement = string.Empty;
            bool flag = false;

            // Load xml data.
            XmlLoad xmlLoad = new XmlLoad();
            xmlLoad.XmlLoadMethod(language, out strTitle, out strText, out strElement, out flag);

            // If specific language is inexistence, return English version of this web page.
            if (flag == true)
            {
                language = "en-us";
                Response.Write("no language, use English web page");
                xmlLoad.XmlLoadMethod(language, out strTitle, out strText, out strElement, out flag);
            }
            lbTitleContent.Text = strTitle;
            lbTextContent.Text = strText;
            lbTimeContent.Text = DateTime.Now.ToLongDateString();
            lbCommentContent.Text = strElement;
            
 
        }
    }
}