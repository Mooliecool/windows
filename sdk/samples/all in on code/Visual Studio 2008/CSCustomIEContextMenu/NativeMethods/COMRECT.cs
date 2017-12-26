/****************************** Module Header ******************************\
* Module Name:  COMRECT.cs
* Project:      CSCustomIEContextMenu
* Copyright (c) Microsoft Corporation.
* 
* The class COMRECT is used to define the outer rectangle of the border.
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

using System.Drawing;
using System.Runtime.InteropServices;

namespace CSCustomIEContextMenu.NativeMethods
{
    [StructLayout(LayoutKind.Sequential)]
    public class COMRECT
    {
        public int left;
        public int top;
        public int right;
        public int bottom;
        public override string ToString()
        {
            return string.Concat(new object[] {
                " Left = ", this.left, 
                " Top = ", this.top, 
                " Right = ", this.right,
                " Bottom = ", this.bottom });
        }

        public COMRECT()
        {
        }

        public COMRECT(Rectangle r)
        {
            this.left = r.X;
            this.top = r.Y;
            this.right = r.Right;
            this.bottom = r.Bottom;
        }

        public COMRECT(int left, int top, int right, int bottom)
        {
            this.left = left;
            this.top = top;
            this.right = right;
            this.bottom = bottom;
        }

        public static NativeMethods.COMRECT FromXYWH(int x, int y, int width, int height)
        {
            return new NativeMethods.COMRECT(x, y, x + width, y + height);
        }
    }


}
