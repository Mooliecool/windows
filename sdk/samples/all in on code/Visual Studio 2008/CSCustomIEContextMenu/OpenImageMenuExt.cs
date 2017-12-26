/****************************** Module Header ******************************\
* Module Name:  OpenImageMenuExt.cs
* Project:      CSCustomIEContextMenu
* Copyright (c) Microsoft Corporation.
* 
* The class OpenImageMenuExt is used to add/remove the menu in registry when this 
* assembly is registered/unregistered.
*  
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using Microsoft.Win32;

namespace CSCustomIEContextMenu
{
    public class OpenImageMenuExt
    {
        private const string IEMenuExtRegistryKey =
            "Software\\Microsoft\\Internet Explorer\\MenuExt";

        public static void RegisterMenuExt()
        {

            // If the key exists, CreateSubKey will open it.
            RegistryKey ieMenuExtKey = Registry.CurrentUser.CreateSubKey(
                IEMenuExtRegistryKey + "\\Open image in new tab");


            // Get the path of Resource\OpenImage.htm.
            FileInfo fileIofo = new FileInfo(Assembly.GetExecutingAssembly().Location);
            string path = fileIofo.Directory.FullName + "\\Resource\\OpenImage.htm";

            // Set the default value of the key to the path.
            ieMenuExtKey.SetValue(string.Empty, path);

            // Set the value of Name.
            ieMenuExtKey.SetValue("Name", "Open_Image");

            // Set the value of Contexts to indicate which contexts your entry should
            // appear in the standard context menu by using a bit mask consisting of
            // the logical OR of the following values:
            // Default 0x1 
            // Images 0x2 
            // Controls 0x4 
            // Tables 0x8 
            // Text selection 0x10 
            // Anchor 0x20 
            ieMenuExtKey.SetValue("Contexts", 0x2);

            ieMenuExtKey.Close();
        }

        public static void UnRegisterMenuExt()
        {

            RegistryKey ieMenuExtskey = Registry.CurrentUser.OpenSubKey(
                IEMenuExtRegistryKey, true);

            if (ieMenuExtskey != null)
            {
                ieMenuExtskey.DeleteSubKey("Open image in new tab", false);
            }

            ieMenuExtskey.Close();
        }
    }
}
