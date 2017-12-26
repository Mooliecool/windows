/****************************** Module Header ******************************\
* Module Name: XmlLoad.cs
* Project:     CSASPNETEmbedLanguageInUrl
* Copyright (c) Microsoft Corporation
*
* The class used to load xml data of Language.xml
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
using System.Xml;

namespace CSASPNETEmbedLanguageInUrl
{
    public class XmlLoad
    {
        public void XmlLoadMethod(string language,out string strTitle,out string strText, out string strElement,out bool flag)
        {
            try
            {
                flag = false;
                XmlDocument xmlDocument = new XmlDocument();
                xmlDocument.Load(AppDomain.CurrentDomain.BaseDirectory + "XmlFolder/Language.xml");
                XmlNodeList cnTitleList = xmlDocument.SelectSingleNode("root/title/element[@language='" + language + "']").ChildNodes;
                if (cnTitleList == null)
                {
                    flag = true;
                    language = "en-us";
                }
                cnTitleList = xmlDocument.SelectSingleNode("root/title/element[@language='" + language + "']").ChildNodes;
                strTitle = cnTitleList[0].InnerText;
                XmlNodeList cnTextList = xmlDocument.SelectSingleNode("root/text/element[@language='" + language + "']").ChildNodes;
                strText = cnTextList[0].InnerText;
                XmlNodeList cnElementList = xmlDocument.SelectSingleNode("root/comment/element[@language='" + language + "']").ChildNodes;
                strElement = cnElementList[0].InnerText;
            }
            catch (Exception)
            {
                strText = string.Empty;
                strElement = string.Empty;
                strTitle = string.Empty;
                flag = true;
            }
        }
    }
}