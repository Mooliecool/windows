/****************************** Module Header ******************************\
* Module Name:  CONTEXT_MENU_CONST.cs
* Project:      CSCustomIEContextMenu
* Copyright (c) Microsoft Corporation.
* 
* The class CONTEXT_MENU_CONST defines the constants that specify the identifier
* of the shortcut menu to be displayed. These values are defined in Mshtmhst.h. 
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

namespace CSCustomIEContextMenu.NativeMethods
{
    public static class CONTEXT_MENU_CONST
    {
        // The default shortcut menu for a Web page.
        public const int CONTEXT_MENU_DEFAULT = 0;

        // Shortcut menu for images. 
        public const int CONTEXT_MENU_IMAGE = 1;

        // Shortcut menu for scrollbars and select elements. 
        public const int CONTEXT_MENU_CONTROL = 2;

        // Not used. 
        public const int CONTEXT_MENU_TABLE = 3;

        // Shortcut menu for selected text. 
        public const int CONTEXT_MENU_TEXTSELECT = 4;

        // Shortcut menu for hyperlinks. 
        public const int CONTEXT_MENU_ANCHOR = 5;

        // Not used. 
        public const int CONTEXT_MENU_UNKNOWN = 6;

        // Internal used.
        public const int CONTEXT_MENU_IMGDYNSRC = 7;
        public const int CONTEXT_MENU_IMGART = 8;
        public const int CONTEXT_MENU_DEBUG = 9;

        // Shortcut menu for vertical scroll bar. 
        public const int CONTEXT_MENU_VSCROLL = 10;

        // Shortcut menu for horizontal scroll bar. 
        public const int CONTEXT_MENU_HSCROLL = 11;
    }
}
