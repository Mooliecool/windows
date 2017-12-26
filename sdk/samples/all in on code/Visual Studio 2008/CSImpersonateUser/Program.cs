/****************************** Module Header ********************************\
* Module Name:  Program.cs
* Project:      CSImpersonateUser
* Copyright (c) Microsoft Corporation.
*
* Windows Impersonation is a powerful feature Windows uses frequently in its 
* security model. In general Windows also uses impersonation in its client/
* server programming model.Impersonation lets a server to temporarily adopt 
* the security profile of a client making a resource request. The server can
* then access resources on behalf of the client, and the OS carries out the 
* access validations.
* A server impersonates a client only within the thread that makes the 
* impersonation request. Thread-control data structures contain an optional 
* entry for an impersonation token. However, a thread's primary token, which
* represents the thread's real security credentials, is always accessible in 
* the process's control structure.
* 
* After the server thread finishes its task, it reverts to its primary 
* security profile. These forms of impersonation are convenient for carrying 
* out specific actions at the request of a client and for ensuring that object
* accesses are audited correctly.
* 
* In this code sample we use the LogonUser API and the WindowsIdentity.
* Impersonate method to impersonate the user represented by the specified user
* token. Then display the current user via the WindowsIdentity.GetCurrent 
* method to show user impersonation. LogonUser can only be used to log onto 
* the local machine; it cannot log you onto a remote computer. The account 
* that you use in the LogonUser() call must also be known to the local 
* machine, either as a local account or as a domain account that is visible
* to the local computer. If LogonUser is successful, then it will give you an
* access token that specifies the credentials of the user account you chose.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 7/7/2009 9:32 AM Riquel Dong Created
* * 7/8/2009 6:13 PM Jialiang Ge Reviewed
\*****************************************************************************/

#region Using Directive
using System;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.Security.Permissions;
using Microsoft.VisualBasic;
using System.Security;
using System.ComponentModel;
using System.Runtime.ConstrainedExecution;
#endregion


class Program
{
    static void Main(string[] args)
    {
        /////////////////////////////////////////////////////////////////////
        // Gather the credential information of the impersonated user.
        // 

        Console.WriteLine("Before impersonation ...");
        Console.WriteLine("Current user is {0}", 
            WindowsIdentity.GetCurrent().Name);

        Console.WriteLine("Input user name");
        string username = Console.ReadLine();
        Console.WriteLine("Input domain name");
        string domain = Console.ReadLine();
        Console.WriteLine("Input password");
        SecureString password = ImpersonateUser.GetPassword();


        /////////////////////////////////////////////////////////////////////
        // Impersonate the specified user. The impersonation is automatically 
        // undone after the Impersonate method.
        // 

        ImpersonateUser.Impersonate<object, object>(username,
            domain, password, null, delegate
        {
            Console.WriteLine("\nDuring impersonation ...");
            Console.WriteLine("Current user is {0}", 
                WindowsIdentity.GetCurrent().Name);

            // Do the work as the user ...

            return null;
        });


        Console.WriteLine("\nAfter impersonation is undone ...");
        Console.WriteLine("Current user is {0}", 
            WindowsIdentity.GetCurrent().Name);
    }
}