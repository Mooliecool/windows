// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
/*============================================================
**
** Interface:  IAppDomain
**
**
** Purpose: Properties and methods exposed to COM
**
** 
===========================================================*/
namespace System {
    using System.Reflection;
    using System.Runtime.CompilerServices;
    using SecurityManager = System.Security.SecurityManager;
    using System.Security.Permissions;
    using IEvidenceFactory = System.Security.IEvidenceFactory;
    using System.Security.Principal;
    using System.Security.Policy;
    using System.Security;
    using System.Security.Util;
    using System.Collections;
    using System.Text;
    using System.Configuration.Assemblies;
    using System.Threading;
    using System.Runtime.InteropServices;
    using System.Runtime.Remoting;   
    using System.Runtime.Remoting.Contexts;
    using System.Reflection.Emit;
    using System.Runtime.Remoting.Activation;
    using System.Runtime.Remoting.Messaging;
    using CultureInfo = System.Globalization.CultureInfo;
    using System.IO;
    using System.Runtime.Serialization.Formatters.Binary;

    [GuidAttribute("05F696DC-2B29-3663-AD8B-C4389CF2A713")]
    [InterfaceTypeAttribute(ComInterfaceType.InterfaceIsIUnknown)]
    [CLSCompliant(false)]
[System.Runtime.InteropServices.ComVisible(true)]
    public interface _AppDomain
    {
        void GetTypeInfoCount(out uint pcTInfo);

        void GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo);

        void GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId);

        void Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr);

        String ToString();

        bool Equals (Object other);

        int GetHashCode ();

        Type GetType ();

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]   
        Object InitializeLifetimeService ();

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]   
        Object GetLifetimeService ();

        Evidence Evidence { get; }

        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        event EventHandler DomainUnload;

        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        event AssemblyLoadEventHandler AssemblyLoad;

        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        event EventHandler ProcessExit;

        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        event ResolveEventHandler TypeResolve;

        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        event ResolveEventHandler ResourceResolve;

        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        event ResolveEventHandler AssemblyResolve;

        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        event UnhandledExceptionEventHandler UnhandledException;

        AssemblyBuilder DefineDynamicAssembly(AssemblyName            name,
                                              AssemblyBuilderAccess   access);

        AssemblyBuilder DefineDynamicAssembly(AssemblyName            name,
                                              AssemblyBuilderAccess   access,
                                              String                  dir);

        AssemblyBuilder DefineDynamicAssembly(AssemblyName            name,
                                              AssemblyBuilderAccess   access,
                                              Evidence                evidence);

        AssemblyBuilder DefineDynamicAssembly(AssemblyName            name,
                                              AssemblyBuilderAccess   access,
                                              PermissionSet           requiredPermissions,
                                              PermissionSet           optionalPermissions,
                                              PermissionSet           refusedPermissions);

        AssemblyBuilder DefineDynamicAssembly(AssemblyName            name,
                                              AssemblyBuilderAccess   access,
                                              String                  dir,
                                              Evidence                evidence);

        AssemblyBuilder DefineDynamicAssembly(AssemblyName            name,
                                              AssemblyBuilderAccess   access,
                                              String                  dir,
                                              PermissionSet           requiredPermissions,
                                              PermissionSet           optionalPermissions,
                                              PermissionSet           refusedPermissions);

        AssemblyBuilder DefineDynamicAssembly(AssemblyName            name,
                                              AssemblyBuilderAccess   access,
                                              Evidence                evidence,
                                              PermissionSet           requiredPermissions,
                                              PermissionSet           optionalPermissions,
                                              PermissionSet           refusedPermissions);

        AssemblyBuilder DefineDynamicAssembly(AssemblyName            name,
                                              AssemblyBuilderAccess   access,
                                              String                  dir,
                                              Evidence                evidence,
                                              PermissionSet           requiredPermissions,
                                              PermissionSet           optionalPermissions,
                                              PermissionSet           refusedPermissions);

        AssemblyBuilder DefineDynamicAssembly(AssemblyName            name,
                                              AssemblyBuilderAccess   access,
                                              String                  dir,
                                              Evidence                evidence,
                                              PermissionSet           requiredPermissions,
                                              PermissionSet           optionalPermissions,
                                              PermissionSet           refusedPermissions,
                                              bool                    isSynchronized);

        ObjectHandle CreateInstance(String assemblyName,
                                    String typeName);

                                         
        ObjectHandle CreateInstanceFrom(String assemblyFile,
                                        String typeName);

                                         
        ObjectHandle CreateInstance(String assemblyName,
                                    String typeName,
                                    Object[] activationAttributes);

        ObjectHandle CreateInstanceFrom(String assemblyFile,
                                        String typeName,
                                        Object[] activationAttributes);

       ObjectHandle CreateInstance(String assemblyName, 
                                   String typeName, 
                                   bool ignoreCase,
                                   BindingFlags bindingAttr, 
                                   Binder binder,
                                   Object[] args,
                                    CultureInfo culture,
                                   Object[] activationAttributes,
                                   Evidence securityAttributes);

       ObjectHandle CreateInstanceFrom(String assemblyFile,
                                       String typeName, 
                                       bool ignoreCase,
                                       BindingFlags bindingAttr, 
                                       Binder binder,
                                        Object[] args,
                                       CultureInfo culture,
                                       Object[] activationAttributes,
                                       Evidence securityAttributes);

        Assembly Load(AssemblyName assemblyRef);

        Assembly Load(String assemblyString);

        Assembly Load(byte[] rawAssembly);

        Assembly Load(byte[] rawAssembly,
                      byte[] rawSymbolStore);

        Assembly Load(byte[] rawAssembly,
                      byte[] rawSymbolStore,
                      Evidence securityEvidence);

        Assembly Load(AssemblyName assemblyRef, 
                      Evidence assemblySecurity);     

        Assembly Load(String assemblyString, 
                      Evidence assemblySecurity);

        int ExecuteAssembly(String assemblyFile, 
                            Evidence assemblySecurity);

        int ExecuteAssembly(String assemblyFile);

        int ExecuteAssembly(String assemblyFile, 
                            Evidence assemblySecurity, 
                            String[] args);

        String FriendlyName
        { get; }

        String BaseDirectory
        { get; }

        String RelativeSearchPath
        { get; }

        bool ShadowCopyFiles
        { get; }

        Assembly[] GetAssemblies();

        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        void AppendPrivatePath(String path);

        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        void ClearPrivatePath();

        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        void SetShadowCopyPath (String s);

        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        void ClearShadowCopyPath ( );

        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        void SetCachePath (String s);

        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        void SetData(String name, Object data);

        Object GetData(string name);

        [SecurityPermission(SecurityAction.LinkDemand, ControlDomainPolicy=true)]
        void SetAppDomainPolicy(PolicyLevel domainPolicy);

        void SetThreadPrincipal(IPrincipal principal);

        void SetPrincipalPolicy(PrincipalPolicy policy);

        void DoCallBack(CrossAppDomainDelegate theDelegate);

        String DynamicDirectory
        { get; }
    }
}

