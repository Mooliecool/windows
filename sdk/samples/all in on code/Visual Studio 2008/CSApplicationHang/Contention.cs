/****************************** Module Header ******************************\
Module Name:  Contention.cs
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
using System.Threading;


namespace CSApplicationHang
{
    class Contention
    {
        private static AutoResetEvent _event = new AutoResetEvent(false);

        private static void ContentionThreadProc()
        {
            _event.WaitOne();
            
            // Do work
            // ...
            Thread.Sleep(50);

            _event.Set();
        }

        public static void Trigger()
        {
            // Create 500 threads
            for (int i = 0; i < 100; i++)
            {
                Thread thread = new Thread(new ThreadStart(ContentionThreadProc));
                thread.Start();
            }

            _event.Set();
        }
    }
}