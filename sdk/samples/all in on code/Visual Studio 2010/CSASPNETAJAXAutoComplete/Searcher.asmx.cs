/****************************** Module Header ******************************\
* Module Name:    Searcher.cs
* Project:        CSASPNETAJAXAutoComplete
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to use AutoCompleteExtender to display words 
* that begin with the prefix that is entered into a text box.When the user 
* has entered more characters than a specified minimum length, a popup 
* displays words or phrases that start with that value. By default, the 
* list of words is positioned at the bottom left side of the text box.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
\*****************************************************************************/


using System;
using System.Collections.Generic;
using System.Web.Services;
namespace CSASPNETAJAXAutoComplete
{
    [WebService(Namespace = "http://tempuri.org/")]
    [WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
    [System.Web.Script.Services.ScriptService]
    public class Searcher : System.Web.Services.WebService
    {

        public Searcher()
        {
        }

        [WebMethod]
        public string[] HelloWorld(string prefixText, int count)
        {
            if (count == 0)
            {
                count = 10;
            }

            if (prefixText.Equals("xyz"))
            {
                return new string[0];
            }

            Random random = new Random();
            List<string> items = new List<string>(count);
            char c1;
            char c2;
            char c3;
            for (int i = 0; i < count; i++)
            {
                c1 = (char)random.Next(65, 90);
                c2 = (char)random.Next(97, 122);
                c3 = (char)random.Next(97, 122);
                items.Add(prefixText + c1 + c2 + c3);
            }

            return items.ToArray();
        }

    }
}