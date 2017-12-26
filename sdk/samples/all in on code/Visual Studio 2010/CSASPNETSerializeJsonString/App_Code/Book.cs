/****************************** Module Header ******************************\
* Module Name:    Book.cs
* Project:        CSASPNETSerializeJsonString
* Copyright (c) Microsoft Corporation
*
* This project illustrates how to serialize Json string. we use jQuery at client 
* side and manipulate XML data at server side.
* It demonstrates how to use the serializable json data through an autocomplete 
* example.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using System.Web;

public class Book
{

    /// <summary>
    /// autocomplete example needs "id", "value" and the "label" variables to be sent back.
    /// do not change or remove "id", "value" and the "label" variables
    /// </summary>  
    public string id { get; set; }
    public string label { get; set; }
    public string value { get; set; }
    
    public string Author { get; set; }
    public string Genre { get; set; }
    public string Price { get; set; }
    public string Publish_date { get; set; }
    public string Description { get; set; }
}