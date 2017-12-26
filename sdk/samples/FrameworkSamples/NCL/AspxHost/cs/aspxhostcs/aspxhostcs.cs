//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

#region Using directives

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Web.Hosting;
using System.Net;
using System.Diagnostics;
using System.IO;
#endregion

namespace Microsoft.Samples.AspxHostCS
{
    class AspxHostCS
    {

        static void Main()
        {
            try
            {
                //Create a AspxVirtualRoot object with a http port and https port if required
                AspxVirtualRoot virtualRoot = new AspxVirtualRoot(80);

                //Configure a Physical directory as a virtual alias.

                /***
                 * Make sure that the ASPXHostCS.exe is present under the bin directory of
                 * the physical path configured. 
                 * To configure a directory as a AspxApplication, the exe for this project ASPXhost.exe
                 * and APSXHostcs.exe.config (if present) should be either present in the bin directory under the 
                 * physical directory being configured. The reason being, that the call to 
                 * API ApplicationHost.CreateApplicationhost creates a new app domain 
                 * and will instantiate a class specified in the typeof variable.
                 * Putting it in the bin directory enables the hosting api to load the class.
                 */

                //TODO: Replace the physical directory with the directory to be configured.
		virtualRoot.Configure("/", Path.GetFullPath(@"..\..\DemoPages"));

                //TODO: If Authentication is to be added, add it here
                //virtualRoot.AuthenticationSchemes = AuthenticationSchemes.Basic;

                Console.WriteLine("ASPXHostVirtualRoot successfully started");
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }

            //Wait till cancelled
            Console.ReadLine();
        }

    }
}
