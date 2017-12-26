/****************************** Module Header ******************************\
* Module Name:  COMHelper.cs
* Project:      CSCOMService
* Copyright (c) Microsoft Corporation.
* 
* COMHelper provides the helper functions to register/unregister COM servers
* and encapsulates the native COM APIs to be used in .NET.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.Reflection;
#endregion


internal class COMHelper
{
    /// <summary>
    /// Register the component as a local service based on regasm according to
    /// http://msdn.microsoft.com/en-us/library/4cb7td54.aspx
    /// </summary>
    /// <param name="t"></param>
    public static void RegasmRegisterLocalService(Type t, Guid appId, String serviceName)
    {
        // Check the argument
        GuardNullType(t, "t");
        GuardAppID(appId);
        GuardServiceName(serviceName);

        // Open the CLSID key of the component.
        using (RegistryKey keyCLSID = Registry.ClassesRoot.OpenSubKey(
            @"CLSID\" + t.GUID.ToString("B"), /*writable*/true))
        {
            // Remove the auto-generated InprocServer32 key after registration
            // (REGASM puts it there but we are going out-of-proc).
            keyCLSID.DeleteSubKeyTree("InprocServer32");


            // Create "LocalServer32" under the CLSID key
            using (RegistryKey subkey = keyCLSID.CreateSubKey("LocalServer32"))
            {
                subkey.SetValue("", Assembly.GetExecutingAssembly().Location,
                    RegistryValueKind.String);
            }

            // Add the "AppID" value to the CLSID key.
            keyCLSID.SetValue("AppID", appId.ToString("B"), RegistryValueKind.String);
        }

        // Add the AppID key of the component.
        using (RegistryKey keyAppID = Registry.ClassesRoot.CreateSubKey(
            @"AppID\" + appId.ToString("B")))
        {
            // Set the service name
            keyAppID.SetValue("", serviceName, RegistryValueKind.String);
            
            // Add the "LocalService" value to the AppID key
            keyAppID.SetValue("LocalService", serviceName, RegistryValueKind.String);
        }
    }

    /// <summary>
    /// Unregister the component based on regasm.
    /// </summary>
    /// <param name="t"></param>
    public static void RegasmUnregisterLocalService(Type t, Guid appId)
    {
        // Check the argument
        GuardNullType(t, "t");
        GuardAppID(appId);

        // Delete the CLSID key of the component
        Registry.ClassesRoot.DeleteSubKeyTree(@"CLSID\" + t.GUID.ToString("B"));

        // Delete the AppID key of the component
        Registry.ClassesRoot.DeleteSubKeyTree(@"AppID\" + appId.ToString("B"));
    }

    private static void GuardNullType(Type t, String param)
    {
        if (t == null)
        {
            throw new ArgumentException("The CLR type must be specified.", param);
        }
    }

    private static void GuardAppID(Guid appId)
    {
        if (appId == null)
        {
            throw new ArgumentException("AppID must be valid.");
        }
    }

    private static void GuardServiceName(String serviceName)
    {
        if (String.IsNullOrEmpty(serviceName))
        {
            throw new ArgumentException("Service name must be valid.");
        }
    }
}

internal class COMNative
{
    /// <summary>
    /// Registers security and sets the default security values for the 
    /// process. This function is called exactly once per process, either 
    /// explicitly or implicitly. It can be called by the client, server, or
    /// both. For legacy applications and other applications that do not
    /// explicitly call CoInitializeSecurity, COM calls this function 
    /// implicitly with values from the registry. If you set process-wide
    /// security using the registry and then call CoInitializeSecurity, the
    /// AppID registry values will be ignored and the CoInitializeSecurity
    /// values will be used.
    /// </summary>
    /// <param name="securityDescriptor">Access permissions</param>
    /// <param name="cAuthSvc">Count of entries in asAuthSvc</param>
    /// <param name="asAuthSvc">Array of authentication services</param>
    /// <param name="pReserved1">Reserved for future use</param>
    /// <param name="level">Default authentication level</param>
    /// <param name="impers">Default impersonation level</param>
    /// <param name="pAuthList"></param>
    /// <param name="dwCapabilities">Additional capabilities</param>
    /// <param name="pReserved3">Reserved for future use</param>
    /// <returns></returns>
    [DllImport("ole32.dll")]
    public static extern int CoInitializeSecurity(
        IntPtr securityDescriptor,  // Access permissions 
        int cAuthSvc,               // Count of entries in asAuthSvc
        IntPtr asAuthSvc,           // Array of authentication services 
        IntPtr pReserved1,          // Reserved for future use
        RPC_C_AUTHN_LEVEL level,    // Default authentication level
        RPC_C_IMP_LEVEL impers,     // Default impersonation level
        IntPtr pAuthList,
        EOLE_AUTHENTICATION_CAPABILITIES dwCapabilities,
        IntPtr pReserved3           // Reserved for future use
        );

    /// <summary>
    /// Registers an EXE class object with OLE so other applications can 
    /// connect to it. EXE object applications should call 
    /// CoRegisterClassObject on startup. It can also be used to register 
    /// internal objects for use by the same EXE or other code (such as DLLs)
    /// that the EXE uses.
    /// </summary>
    /// <param name="rclsid">CLSID to be registered</param>
    /// <param name="pUnk">
    /// Pointer to the IUnknown interface on the class object whose 
    /// availability is being published.
    /// </param>
    /// <param name="dwClsContext">
    /// Context in which the executable code is to be run.
    /// </param>
    /// <param name="flags">
    /// How connections are made to the class object.
    /// </param>
    /// <param name="lpdwRegister">
    /// Pointer to a value that identifies the class object registered; 
    /// </param>
    /// <returns></returns>
    /// <remarks>
    /// PInvoking CoRegisterClassObject to register COM objects is not 
    /// supported.
    /// </remarks>
    [DllImport("ole32.dll")]
    public static extern int CoRegisterClassObject(
        ref Guid rclsid,
        [MarshalAs(UnmanagedType.Interface)] IClassFactory pUnk,
        CLSCTX dwClsContext,
        REGCLS flags,
        out int lpdwRegister);

    /// <summary>
    /// Informs OLE that a class object, previously registered with the 
    /// CoRegisterClassObject function, is no longer available for use.
    /// </summary>
    /// <param name="dwRegister">
    /// Token previously returned from the CoRegisterClassObject function
    /// </param>
    /// <returns></returns>
    [DllImport("ole32.dll")]
    public static extern UInt32 CoRevokeClassObject(int dwRegister);

    /// <summary>
    /// Called by a server that can register multiple class objects to inform 
    /// the SCM about all registered classes, and permits activation requests 
    /// for those class objects.
    /// </summary>
    /// <returns></returns>
    /// <remarks>
    /// Servers that can register multiple class objects call 
    /// CoResumeClassObjects once, after having first called 
    /// CoRegisterClassObject, specifying REGCLS_LOCAL_SERVER | 
    /// REGCLS_SUSPENDED for each CLSID the server supports. This function 
    /// causes OLE to inform the SCM about all the registered classes, and 
    /// begins letting activation requests into the server process.
    /// 
    /// This reduces the overall registration time, and thus the server 
    /// application startup time, by making a single call to the SCM, no 
    /// matter how many CLSIDs are registered for the server. Another 
    /// advantage is that if the server has multiple apartments with 
    /// different CLSIDs registered in different apartments, or is a free-
    /// threaded server, no activation requests will come in until the server 
    /// calls CoResumeClassObjects. This gives the server a chance to 
    /// register all of its CLSIDs and get properly set up before having to 
    /// deal with activation requests, and possibly shutdown requests. 
    /// </remarks>
    [DllImport("ole32.dll")]
    public static extern int CoResumeClassObjects();

    /// <summary>
    /// Interface Id of IClassFactory
    /// </summary>
    public const string GuidIClassFactory =
        "00000001-0000-0000-C000-000000000046";

    /// <summary>
    /// Interface Id of IUnknown
    /// </summary>
    public const string GuidIUnknown =
        "00000000-0000-0000-C000-000000000046";

    /// <summary>
    /// Class does not support aggregation (or class object is remote)
    /// </summary>
    public const int CLASS_E_NOAGGREGATION = unchecked((int)0x80040110);

    /// <summary>
    /// No such interface supported
    /// </summary>
    public const int E_NOINTERFACE = unchecked((int)0x80004002);
}

/// <summary>
/// You must implement this interface for every class that you register in 
/// the system registry and to which you assign a CLSID, so objects of that
/// class can be created.
/// http://msdn.microsoft.com/en-us/library/ms694364.aspx
/// </summary>
[ComImport(),
InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
Guid(COMNative.GuidIClassFactory)]
internal interface IClassFactory
{
    /// <summary>
    /// Creates an uninitialized object.
    /// </summary>
    /// <param name="pUnkOuter"></param>
    /// <param name="riid">
    /// Reference to the identifier of the interface to be used to 
    /// communicate with the newly created object. If pUnkOuter is NULL, this
    /// parameter is frequently the IID of the initializing interface.
    /// </param>
    /// <param name="ppvObject">
    /// Address of pointer variable that receives the interface pointer 
    /// requested in riid. 
    /// </param>
    /// <returns>S_OK means success.</returns>
    [PreserveSig]
    int CreateInstance(IntPtr pUnkOuter, ref Guid riid, out IntPtr ppvObject);

    /// <summary>
    /// Locks object application open in memory.
    /// </summary>
    /// <param name="fLock">
    /// If TRUE, increments the lock count; 
    /// if FALSE, decrements the lock count.
    /// </param>
    /// <returns>S_OK means success.</returns>
    [PreserveSig]
    int LockServer(bool fLock);
}

/// <summary>
/// Used in the security functions and interfaces to specify an 
/// authentication level, which indicates the amount of authentication 
/// provided to help protect the integrity of the data. Each level includes
/// the protection provided by the previous levels. 
/// </summary>
internal enum RPC_C_AUTHN_LEVEL
{
    DEFAULT = 0,
    NONE = 1,               // Perform no authentication
    CONNECT = 2,            // Authenticate only when connecting
    CALL = 3,               // Authenticate only at each RPC
    PKT = 4,                // Authenticate the source of data
    PKT_INTEGRITY = 5,      // Ensure no data is modified in transfer
    PKT_PRIVACY = 6         // Encrypted DCOM communication
}

/// <summary>
/// Used in the security functions and interfaces to specify an impersonation
/// level, which indicates the amount of authority given to the server when 
/// it is impersonating the client. 
/// </summary>
internal enum RPC_C_IMP_LEVEL
{
    DEFAULT = 0,
    ANONYMOUS = 1,          // Client is anonymous to server
    IDENTIFY = 2,           // No impersonation really required
    IMPERSONATE = 3,        // Impersonate
    DELEGATE = 4            // Delegate
}

/// <summary>
/// The EOLE_AUTHENTICATION_CAPABILITIES enumeration constants set various
/// capabilities in CoInitializeSecurity.
/// </summary>
internal enum EOLE_AUTHENTICATION_CAPABILITIES
{
    NONE = 0x0,
    MUTUAL_AUTH = 0x1,
    SECURE_REFS = 0x2,          // Enable secure DCOM references
    ACCESS_CONTROL = 0x4,
    APPID = 0x8,
    DYNAMIC = 0x10,
    STATIC_CLOAKING = 0x20,
    DYNAMIC_CLOAKING = 0x40,
    ANY_AUTHORITY = 0x80,
    MAKE_FULLSIC = 0x100,
    REQUIRE_FULLSIC = 0x200,
    AUTO_IMPERSONATE = 0x400,
    DEFAULT = 0x800,
    DISABLE_AAA = 0x1000,       // Disable Activate-as-activator
    NO_CUSTOM_MARSHAL = 0x2000  // Disable custom marshalling   
}

/// <summary>
/// Values from the CLSCTX enumeration are used in activation calls to 
/// indicate the execution contexts in which an object is to be run. These
/// values are also used in calls to CoRegisterClassObject to indicate the
/// set of execution contexts in which a class object is to be made available
/// for requests to construct instances.
/// </summary>
internal enum CLSCTX
{
    INPROC_SERVER = 0x1,
    INPROC_HANDLER = 0x2,
    LOCAL_SERVER = 0x4,
    INPROC_SERVER16 = 0x8,
    REMOTE_SERVER = 0x10,
    INPROC_HANDLER16 = 0x20,
    RESERVED1 = 0x40,
    RESERVED2 = 0x80,
    RESERVED3 = 0x100,
    RESERVED4 = 0x200,
    NO_CODE_DOWNLOAD = 0x400,
    RESERVED5 = 0x800,
    NO_CUSTOM_MARSHAL = 0x1000,
    ENABLE_CODE_DOWNLOAD = 0x2000,
    NO_FAILURE_LOG = 0x4000,
    DISABLE_AAA = 0x8000,
    ENABLE_AAA = 0x10000,
    FROM_DEFAULT_CONTEXT = 0x20000,
    ACTIVATE_32_BIT_SERVER = 0x40000,
    ACTIVATE_64_BIT_SERVER = 0x80000
}

/// <summary>
/// The REGCLS enumeration defines values used in CoRegisterClassObject to 
/// control the type of connections to a class object.
/// </summary>
internal enum REGCLS
{
    SINGLEUSE = 0,
    MULTIPLEUSE = 1,
    MULTI_SEPARATE = 2,
    SUSPENDED = 4,
    SURROGATE = 8,
}