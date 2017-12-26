/****************************** Module Header ********************************\
* Module Name:  ImpersonateUser.cs
* Project:      CSImpersonateUser
* Copyright (c) Microsoft Corporation.
* 
* The wrapper class of P/Invoke signatures for impersonating user
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 7/7/2009 9:32 AM Riquel Dong Created
* * 7/8/2009 6:13 PM Jialiang Ge Reviewed
\*****************************************************************************/

#region Using directives
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.Security.Permissions;
using Microsoft.VisualBasic;
using System.Security;
using System.ComponentModel;
using System.Runtime.ConstrainedExecution;
#endregion


/// <summary>
/// The wrapper class for impersonating user
/// </summary>
public class ImpersonateUser
{
    /// <summary>
    /// A delegate that will be called under the impersonation context
    /// </summary>
    /// <typeparam name="TReturn"></typeparam>
    /// <typeparam name="TParameter"></typeparam>
    /// <param name="paramter"></param>
    /// <returns></returns>
    public delegate TReturn
        ImpersonationWorkFunction<TReturn, TParameter>(TParameter paramter);

    /// <summary>
    /// This method calles LogonUser API to impersonation the user and is 
    /// a wrapper around the code exposed by the delegate which makes it 
    /// run while impersonating 
    /// </summary>
    /// <typeparam name="TReturn">
    /// Generic return type of the delegated function
    /// </typeparam>
    /// <typeparam name="TParameter">
    /// Generic parameter of the delegated function
    /// </typeparam>
    /// <param name="userName">The user name</param>
    /// <param name="domain">Domain</param>
    /// <param name="password">Password</param>
    /// <param name="parameter">Parameter of the delegated function</param>
    /// <param name="impersonationWork">
    /// Called method while impersonating
    /// </param>
    /// <param name="logonMethod">
    /// The type of logon operation to perform
    /// </param>
    /// <param name="provider">The logon provider</param>
    /// <returns>The return of the delegated function</returns>
    [SecurityPermission(SecurityAction.Demand, UnmanagedCode = true)]
    public static TReturn Impersonate<TReturn, TParameter>(
        string userName,
        string domain,
        SecureString password,
        TParameter parameter,
        ImpersonationWorkFunction<TReturn, TParameter> impersonationWork,
        NativeMethod.LogonType logonMethod,
        NativeMethod.LogonProvider provider)
    {
        // Check the parameters
        if (string.IsNullOrEmpty(userName))
        {
            throw new ArgumentNullException("userName");
        }
        if (password == null)
        {
            throw new ArgumentNullException("password");
        }
        if (impersonationWork == null)
        {
            throw new ArgumentNullException("impersonationWork");
        }
        if (logonMethod < NativeMethod.LogonType.LOGON32_LOGON_INTERACTIVE |
            NativeMethod.LogonType.LOGON32_LOGON_NEW_CREDENTIALS < logonMethod)
        {
            throw new ArgumentOutOfRangeException("logonMethod");
        }
        if (provider < NativeMethod.LogonProvider.LOGON32_PROVIDER_DEFAULT |
            NativeMethod.LogonProvider.LOGON32_PROVIDER_WINNT50 < provider)
        {
            throw new ArgumentOutOfRangeException("provider");
        }

        IntPtr passwordPtr = IntPtr.Zero;
        SafeUserToken token = null;
        WindowsImpersonationContext context = null;
        try
        {
            // Convert the password to a string
            passwordPtr = Marshal.SecureStringToBSTR(password);
            IntPtr handle = IntPtr.Zero;

            // Attempts to log a user on to the local computer
            if (!NativeMethod.LogonUser(userName, domain, passwordPtr,
                logonMethod, provider, out handle))
            {
                throw new Win32Exception();
            }
            else
            {
                token = new SafeUserToken(ref handle);
            }
        }
        finally
        {
            // Erase the memory that the password was stored in
            if (!IntPtr.Zero.Equals(passwordPtr))
            {
                Marshal.ZeroFreeBSTR(passwordPtr);
            }
        }

        try
        {
            // Impersonate
            Debug.Assert(token != null);
            context = WindowsIdentity.Impersonate(token.DangerousGetHandle());

            // Call out to the work function
            return impersonationWork(parameter);
        }
        finally
        {
            // Clean up
            UndoImpersonation(ref token, ref context);
        }
    }

    /// <summary>
    /// This method calles LogonUser API to impersonation the user and is 
    /// a wrapper around the code exposed by the delegate which makes it 
    /// run while impersonating 
    /// </summary>
    /// <typeparam name="TReturn">
    /// Generic return type of the delegated function
    /// </typeparam>
    /// <typeparam name="TParameter">
    /// Generic parameter of the delegated function
    /// </typeparam>
    /// <param name="userName">The user name</param>
    /// <param name="domain">Domain</param>
    /// <param name="password">Password</param>
    /// <param name="parameter">Parameter of the delegated function</param>
    /// <param name="impersonationWork">
    /// Called method while impersonating
    /// </param>
    /// <returns>The return of the delegated function</returns>
    public static TReturn Impersonate<TReturn, TParameter>(
        string userName,
        string domain,
        SecureString password,
        TParameter parameter,
        ImpersonationWorkFunction<TReturn, TParameter> impersonationWork)
    {
        return Impersonate(
            userName,
            domain,
            password,
            parameter,
            impersonationWork,
            NativeMethod.LogonType.LOGON32_LOGON_INTERACTIVE,
            NativeMethod.LogonProvider.LOGON32_PROVIDER_DEFAULT);
    }

    /// <summary>
    /// Undo impersonation for calling thread
    /// </summary>
    /// <param name="token"></param>
    /// <param name="context"></param>
    /// <returns></returns>
    private static bool UndoImpersonation(ref SafeUserToken token,
        ref WindowsImpersonationContext context)
    {
        if (context != null)
        {
            context.Undo();
            context = null;
        }
        if (token != null)
        {
            token.Dispose();
            token = null;
        }
        return true;
    }

    /// <summary>
    /// Get user's password with SecureString
    /// </summary>
    /// <returns></returns>
    public static SecureString GetPassword()
    {
        SecureString password = new SecureString();

        // Get the first character of the password
        ConsoleKeyInfo nextKey = Console.ReadKey(true);
        while (nextKey.Key != ConsoleKey.Enter)
        {
            if (nextKey.Key == ConsoleKey.Backspace)
            {
                if (password.Length > 0)
                {
                    password.RemoveAt(password.Length - 1);
                    // Erase the last * as well
                    Console.Write(nextKey.KeyChar);
                    Console.Write(" ");
                    Console.Write(nextKey.KeyChar);
                }
            }
            else
            {
                password.AppendChar(nextKey.KeyChar);
                Console.Write("*");
            }
            nextKey = Console.ReadKey(true);
        }
        Console.WriteLine();

        // Lock the password down
        password.MakeReadOnly();
        return password;
    }

    /// <summary>
    /// A SafeHandle wrapper around the user token so that we gain all the
    /// benefits presented by the new SafeHandle model
    /// </summary>
    internal sealed class SafeUserToken : SafeHandle
    {
        [SuppressUnmanagedCodeSecurity()]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        [DllImport("Kernel32", SetLastError = true, CharSet = CharSet.Auto)]
        private static extern bool CloseHandle(IntPtr hObject);

        // Constructor
        public SafeUserToken(ref IntPtr h)
            : base(h, true)
        {
        }

        // Check to see if the handle is zero
        public override bool IsInvalid
        {
            get { return IntPtr.Zero.Equals(handle); }
        }

        // Will call CloseHandle in order to close object handle
        protected override bool ReleaseHandle()
        {
            return CloseHandle(handle);
        }
    }
}

public class NativeMethod
{
    [DllImport("advapi32.dll", SetLastError = true, CharSet = CharSet.Auto)]
    public static extern bool LogonUser(
        string lpszUsername,          // The name of the user
        string lpszDomain,            // The name of the domain
        IntPtr lpszPassword,          // The user's password
        LogonType dwLogonType,        // The type of logon operation to perform
        LogonProvider dwLogonProvider,// The logon provider
        out IntPtr phToken            // Token handle of the specified user
    );

    public enum LogonType : int
    {
        // This logon type is intended for users who will be interactively 
        // using the computer
        LOGON32_LOGON_INTERACTIVE = 2,

        // This logon type is intended for high performance servers to 
        // authenticate plaintext passwords
        LOGON32_LOGON_NETWORK = 3,

        // This logon type is intended for batch servers
        LOGON32_LOGON_BATCH = 4,

        // Indicates a service-type logon
        LOGON32_LOGON_SERVICE = 5,

        // This logon type is for GINA DLLs that log on users who will be 
        // interactively using the computer       
        LOGON32_LOGON_UNLOCK = 7,

        // This logon type preserves the name and password in the 
        // authentication package
        LOGON32_LOGON_NETWORK_CLEARTEXT = 8,

        // This logon type allows the caller to clone its current token 
        // and specify new credentials for outbound connections.        
        LOGON32_LOGON_NEW_CREDENTIALS = 9
    }

    public enum LogonProvider : int
    {
        // Use the standard logon provider for the system        
        LOGON32_PROVIDER_DEFAULT = 0,
        // Use the negotiate logon provider
        LOGON32_PROVIDER_WINNT50 = 1,
        // Use the NTLM logon provider
        LOGON32_PROVIDER_WINNT40 = 2
    }
}