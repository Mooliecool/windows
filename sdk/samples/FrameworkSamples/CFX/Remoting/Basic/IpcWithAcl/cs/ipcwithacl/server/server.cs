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

using System;
using System.Collections;
using System.Security;
using System.Security.AccessControl;
using System.Runtime.Remoting;
using System.Runtime.Remoting.Channels;
using System.Runtime.Remoting.Channels.Ipc;
using System.Security.Principal;
using Microsoft.Samples.Implementation;

namespace Microsoft.Samples.Server
{
    /// <summary>
    /// Summary description for Class1.
    /// </summary>
    static class Class1
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        static void Main()
        {
            RemotingConfiguration.Configure("Server.exe.config", false /*ensureSecurity*/);

            IDictionary props = new Hashtable();
            props["portName"] = "test";

            // This is the wellknown sid for network sid
            string networkSidSddlForm = @"S-1-5-2";  
            // Local administrators sid
            SecurityIdentifier localAdminSid = new SecurityIdentifier(
                WellKnownSidType.BuiltinAdministratorsSid, null);
            // Local Power users sid
            SecurityIdentifier powerUsersSid = new SecurityIdentifier(
                WellKnownSidType.BuiltinPowerUsersSid, null);
            // Network sid
            SecurityIdentifier networkSid = new SecurityIdentifier(networkSidSddlForm);

            DiscretionaryAcl dacl = new DiscretionaryAcl(false, false, 1);

            // Disallow access from off machine
            dacl.AddAccess(AccessControlType.Deny, networkSid, -1,
                InheritanceFlags.None, PropagationFlags.None);

            // Allow acces only from local administrators and power users
            dacl.AddAccess(AccessControlType.Allow, localAdminSid, -1, 
                InheritanceFlags.None, PropagationFlags.None);
            dacl.AddAccess(AccessControlType.Allow, powerUsersSid, -1,
                InheritanceFlags.None, PropagationFlags.None);

            CommonSecurityDescriptor securityDescriptor = 
                new CommonSecurityDescriptor(false, false, 
                        ControlFlags.GroupDefaulted | 
                        ControlFlags.OwnerDefaulted | 
                        ControlFlags.DiscretionaryAclPresent, 
                        null, null, null, dacl);

            IpcServerChannel channel = new IpcServerChannel(
                                                    props, 
                                                    null, 
                                                    securityDescriptor);

            ChannelServices.RegisterChannel(channel, false /*ensureSecurity*/);

            foreach (IChannel chan in ChannelServices.RegisteredChannels)
            {
                Console.WriteLine(chan.ChannelName);
            }

            Console.WriteLine("Waiting for connections...");
            Console.WriteLine("Press enter to exit.");
            Console.ReadLine();
        }
    }
}
