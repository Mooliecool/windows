/****************************** Module Header ******************************\
Module Name:  Spinning.cs
Project:      CSApplicationHang
Copyright (c) Microsoft Corporation.



This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;


namespace CSApplicationHang
{
    class Spinning
    {
        private static void SpinningThreadProc()
        {
            while (true)
            {
                // Do work ...
            }
        }

        public static void Trigger()
        {
            Thread thread2 = new Thread(new ThreadStart(SpinningThreadProc));
            thread2.Start();
        }
    }
}
