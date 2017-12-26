/****************************** Module Header ******************************\
* Module Name: Default.aspx.cs
* Project:     CSASPNETControlPermissionForFiles
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to control the permission for protect files and 
* folders on server from being download. Here we give a solution that when the
* web application receive a URL request, we will make a judgment that if the 
* request file's extension name is not .jpg file then redirect to 
* NoPermissionPage page. Also, user can not access the image file via copy URL. 
* 
* This page is used to bind ListView control with XML file, users can visit image 
* file by click links.
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
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Xml;
using System.Data;

namespace CSASPNETControlPermissionForFiles
{
    public partial class Default : System.Web.UI.Page
    {
        /// <summary>
        /// Binding ListView control with XML files.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected void Page_Load(object sender, EventArgs e)
        {
            XmlDocument xmlDocument = new XmlDocument();
            xmlDocument.Load(AppDomain.CurrentDomain.BaseDirectory + "XmlFile/PermissionFilesXml.xml");
            XmlNodeList nodeList = xmlDocument.SelectNodes("Root/File");
            DataTable tabNodes = new DataTable();
            tabNodes.Columns.Add("ID", Type.GetType("System.Int32"));
            tabNodes.Columns.Add("Name", Type.GetType("System.String"));
            tabNodes.Columns.Add("Path", Type.GetType("System.String"));
            foreach (XmlNode node in nodeList)
            {
                DataRow drTab = tabNodes.NewRow();
                drTab["ID"] = node["ID"].InnerText;
                drTab["Name"] = node["Name"].InnerText;
                drTab["Path"] = node["FilePath"].InnerText;
                tabNodes.Rows.Add(drTab);
            }
            ListView1.DataSource = tabNodes;
            ListView1.DataBind();
        }
    }
}