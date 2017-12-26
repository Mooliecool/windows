/****************************** Module Header ******************************\
* Module Name: BasePage.cs
* Project:     CSASPNETEmbedLanguageInUrl
* Copyright (c) Microsoft Corporation
*
* The multiple language web pages are inheriting this class.
* The BasePage class will check the request url language part
* and name part, and set the Page's Culture and UICultrue
* properties.
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
using System.Threading;
using System.Globalization;

namespace CSASPNETEmbedLanguageInUrl
{
    public class BasePage:Page
    {
        public BasePage()
        {

        }

        /// <summary>
        /// The BasePage class used to set Page.Culture and Page.UICulture.
        /// </summary>
        protected override void InitializeCulture()
        {
            try
            {
                string language = RouteData.Values["language"].ToString().ToLower();
                string pageName = RouteData.Values["pageName"].ToString();
                Session["info"] = language + "," + pageName;
                Page.Culture = language;
                Page.UICulture = language;
            }
            catch (Exception)
            {
                Session["info"] = "error,error";
            }
            
        }
    }
}