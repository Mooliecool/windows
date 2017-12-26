/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:      CSXslTransformXml
* Copyright (c) Microsoft Corporation.
* 
* This sample project shows how to use XslCompiledTransform to transform an 
* XML data file to .csv file using an XSLT style sheet.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Text;
using System.Xml.Xsl;
#endregion


namespace CSXslTransformXml
{
    class Program
    {
        static void Main(string[] args)
        {
            XslCompiledTransform transform = new XslCompiledTransform();
            transform.Load("Books.xslt");
            transform.Transform("Books.xml", "Books.csv");
        }
    }
}
