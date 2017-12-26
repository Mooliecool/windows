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
/*=============================================================================
**
** Class: AppDomain
**
**
** Purpose: Domains represent an application within the runtime. Objects can 
**          not be shared between domains and each domain can be configured
**          independently. 
**
**
=============================================================================*/

namespace System {
    using System;
    using System.Reflection;
    using System.Runtime;
    using System.Runtime.Hosting;
    using System.Runtime.CompilerServices;
    using System.Runtime.Remoting.Channels;
    using System.Runtime.Remoting.Contexts;
    using System.Security;
    using System.Security.Permissions;
    using System.Security.Principal;
    using System.Security.Policy;
    using System.Security.Util;
    using System.Collections;
    using System.Collections.Generic;
    using System.Threading;
    using System.Runtime.InteropServices;
    using System.Runtime.Remoting;   
    using Context = System.Runtime.Remoting.Contexts.Context;
    using System.Reflection.Emit;
    using Message = System.Runtime.Remoting.Messaging.Message;
    using CultureInfo = System.Globalization.CultureInfo;
    using System.IO;
    using BinaryFormatter = System.Runtime.Serialization.Formatters.Binary.BinaryFormatter;
    using AssemblyHashAlgorithm = System.Configuration.Assemblies.AssemblyHashAlgorithm;
    using System.Text;
    using Microsoft.Win32;
    using System.Runtime.ConstrainedExecution;
    using System.Runtime.Versioning;

    [ComVisible(true)]
    public class ResolveEventArgs : EventArgs
    {
        private String _Name;

        public String Name {
            get {
                return _Name;
            }
        }

        public ResolveEventArgs(String name)
        {
            _Name = name;
        }
    }

    [ComVisible(true)]
    public class AssemblyLoadEventArgs : EventArgs
    {
        private Assembly _LoadedAssembly;

        public Assembly LoadedAssembly {
            get {
                return _LoadedAssembly;
            }
        }

        public AssemblyLoadEventArgs(Assembly loadedAssembly)
        {
            _LoadedAssembly = loadedAssembly;
        }
    }


    [Serializable()]
    [ComVisible(true)]
    public delegate Assembly ResolveEventHandler(Object sender, ResolveEventArgs args);

    [Serializable()]
    [ComVisible(true)]
    public delegate void AssemblyLoadEventHandler(Object sender, AssemblyLoadEventArgs args);

    [Serializable()]
    [ComVisible(true)]
    public delegate void AppDomainInitializer(string[] args);

    internal class AppDomainInitializerInfo
    {
        internal class ItemInfo
        {
            public string TargetTypeAssembly;
            public string TargetTypeName;
            public string MethodName;
        }

        internal ItemInfo[] Info;

        internal AppDomainInitializerInfo(AppDomainInitializer init)
        {
            Info=null;
            if (init==null)
                return;
            ArrayList itemInfo=new ArrayList();
            ArrayList nestedDelegates=new ArrayList();
            nestedDelegates.Add(init);
            int idx=0;
 
            while (nestedDelegates.Count>idx)
            {
                AppDomainInitializer curr=(AppDomainInitializer)nestedDelegates[idx++];
                Delegate[] list= curr.GetInvocationList();
                for (int i=0;i<list.Length;i++)
                {
                    if (!list[i].Method.IsStatic) 
                    {
                        if(list[i].Target==null)
                            continue;
                    
                        AppDomainInitializer nested = list[i].Target as AppDomainInitializer;
                        if (nested!=null)
                            nestedDelegates.Add(nested);
                        else
                            throw new ArgumentException(Environment.GetResourceString("Arg_MustBeStatic"),
                               list[i].Method.ReflectedType.FullName+"::"+list[i].Method.Name);
                    }
                    else
                    {
                        ItemInfo info=new ItemInfo();
                        info.TargetTypeAssembly=list[i].Method.ReflectedType.Module.Assembly.FullName;
                        info.TargetTypeName=list[i].Method.ReflectedType.FullName;
                        info.MethodName=list[i].Method.Name;
                        itemInfo.Add(info);
                    }
                    
                }
            }

            Info=(ItemInfo[])itemInfo.ToArray(typeof(ItemInfo));            
        }
        
        internal AppDomainInitializer Unwrap()
        {
            if (Info==null)
                return null;
            AppDomainInitializer retVal=null;
            new ReflectionPermission(ReflectionPermissionFlag.MemberAccess).Assert();
            for (int i=0;i<Info.Length;i++)
            {
                Assembly assembly=Assembly.Load(Info[i].TargetTypeAssembly);
                AppDomainInitializer newVal=(AppDomainInitializer)Delegate.CreateDelegate(typeof(AppDomainInitializer),
                        assembly.GetType(Info[i].TargetTypeName),
                        Info[i].MethodName);
                if(retVal==null)
                    retVal=newVal;
                else
                    retVal+=newVal;
            }
            return retVal;
        }
    }


    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(System._AppDomain))]
    [ComVisible(true)]
    public sealed class AppDomain : MarshalByRefObject, _AppDomain, IEvidenceFactory
    {
        // Domain security information
        // These fields initialized from the other side only. (NOTE: order 
        // of these fields cannot be changed without changing the layout in 
        // the EE)

        private AppDomainManager _domainManager;
        private Hashtable        _LocalStore;
        private AppDomainSetup   _FusionStore;
        private Evidence         _SecurityIdentity;
#pragma warning disable 169
        private Object[]         _Policies; // Called from the VM.
#pragma warning restore 169
        
        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        public event AssemblyLoadEventHandler AssemblyLoad;
        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        public event ResolveEventHandler TypeResolve;
        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        public event ResolveEventHandler ResourceResolve;
        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        public event ResolveEventHandler AssemblyResolve;

        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        public event ResolveEventHandler ReflectionOnlyAssemblyResolve;

        private Context          _DefaultContext;


        private IPrincipal       _DefaultPrincipal;
        private DomainSpecificRemotingData _RemotingData;
        private EventHandler     _processExit;
        private EventHandler     _domainUnload;
        private UnhandledExceptionEventHandler _unhandledException;

#pragma warning disable 169
        private IntPtr           _dummyField;                   // this is an unmanaged pointer (AppDomain * m_pDomain)` used from the VM.
#pragma warning restore 169
        private PrincipalPolicy  _PrincipalPolicy;              // this is an enum
        private bool             _HasSetPolicy;

        // this method is required so Object.GetType is not made virtual by the compiler
        public new Type GetType()
        {
            return base.GetType();
        }
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern string nGetDomainManagerAsm ();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern string nGetDomainManagerType ();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void nSetHostSecurityManagerFlags (HostSecurityManagerOptions flags);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void nSetSecurityHomogeneousFlag ();

        // Called from the VM during the default domain initialization phase.
        private void SetDefaultDomainManager (string fullName, string[] manifestPaths, string[] activationData) {
            SetDomainManager(null, null, IntPtr.Zero, false);
        }

        // Called for every AppDomain (including the default domain) to initialize 
        // the AppDomainManager instance and security of the AppDomain.
        private void SetDomainManager (Evidence providedSecurityInfo, Evidence creatorsSecurityInfo, IntPtr parentSecurityDescriptor, bool publishAppDomain) {

            // Create the AppDomainManager for the AppDomain if specified.
            string domainManagerAssemblyName = nGetDomainManagerAsm ();
            string domainManagerTypeName = nGetDomainManagerType ();

            if (domainManagerAssemblyName != null && domainManagerTypeName != null)
                _domainManager = CreateDomainManager(domainManagerAssemblyName, domainManagerTypeName);

            AppDomainSetup adSetup = FusionStore;
            // Call AppDomainManager initialization -- runs host custom code.
            if (_domainManager != null) {
                // Call InitializeNewDomain and register the AppDomainManager with the host if needed.
                _domainManager.InitializeNewDomain(adSetup);
                AppDomainManagerInitializationOptions flags = _domainManager.InitializationFlags;
                if ((flags & AppDomainManagerInitializationOptions.RegisterWithHost) == AppDomainManagerInitializationOptions.RegisterWithHost)
                    _domainManager.nRegisterWithHost();
            }


            // Set the new evidence to the default loader behavior.
            Evidence newAppDomainEvidence = (providedSecurityInfo != null ? providedSecurityInfo : creatorsSecurityInfo);
            if (_domainManager != null) {
                // Give the host a chance to alter the AppDomain evidence
                HostSecurityManager securityManager = _domainManager.HostSecurityManager;
                if (securityManager != null) {
                    nSetHostSecurityManagerFlags (securityManager.Flags);
                    if ((securityManager.Flags & HostSecurityManagerOptions.HostAppDomainEvidence) == HostSecurityManagerOptions.HostAppDomainEvidence)
                        newAppDomainEvidence = securityManager.ProvideAppDomainEvidence(newAppDomainEvidence);
                }
            }

            // Set the evidence on the managed side
            _SecurityIdentity = newAppDomainEvidence;

            // Set the evidence of the AppDomain in the VM.
            // Also, now that the initialization is complete, signal that to the security system.
            // Finish the AppDomain initialization and resolve the policy for the AppDomain evidence.
            nSetupDomainSecurity(newAppDomainEvidence, parentSecurityDescriptor, publishAppDomain);

            // The AppDomain is now resolved. Go ahead and set the PolicyLevel
            // from the HostSecurityManager if specified.
            if (_domainManager != null)
                RunDomainManagerPostInitialization(_domainManager);
        }

        private AppDomainManager CreateDomainManager (string domainManagerAssemblyName, string domainManagerTypeName) {
            AppDomainManager domainManager = null;
            try {
                domainManager = CreateInstanceAndUnwrap(domainManagerAssemblyName, domainManagerTypeName) as AppDomainManager;
            }
            catch (FileNotFoundException) {}
            catch (TypeLoadException) {}
            finally {
                if (domainManager == null)
                    throw new TypeLoadException(Environment.GetResourceString("Argument_NoDomainManager"));
            }
            return domainManager;
        }

        private void RunDomainManagerPostInitialization (AppDomainManager domainManager) {
            // force creation of the HostExecutionContextManager for the current AppDomain
            HostExecutionContextManager contextManager = domainManager.HostExecutionContextManager;

            HostSecurityManager securityManager = domainManager.HostSecurityManager;
            if (securityManager != null) {
                if ((securityManager.Flags & HostSecurityManagerOptions.HostPolicyLevel) == HostSecurityManagerOptions.HostPolicyLevel) {
                    // set AppDomain policy if specified
                    PolicyLevel level = securityManager.DomainPolicy;
                    if (level != null)
                        SetAppDomainPolicy(level);
                }
            }
        }


        // This method is called from CorHost2::ExecuteApplication to activate a ClickOnce application in the default AppDomain.
        private int ActivateApplication () {
            return 0;
        }

        public AppDomainManager DomainManager {
            [SecurityPermission(SecurityAction.LinkDemand, ControlDomainPolicy=true)]
            get {
                return _domainManager;
            }
        }

        internal HostSecurityManager HostSecurityManager {
            get {
                HostSecurityManager securityManager = null;
                AppDomainManager domainManager = AppDomain.CurrentDomain.DomainManager;
                if (domainManager != null)
                    securityManager = domainManager.HostSecurityManager;

                if (securityManager == null)
                    securityManager = new HostSecurityManager();
                return securityManager;
            }
        }

        private Assembly ResolveAssemblyForIntrospection(Object sender, ResolveEventArgs args)
        {
            return Assembly.ReflectionOnlyLoad(ApplyPolicy(args.Name));
        }

        private void EnableResolveAssembliesForIntrospection()
        {
            CurrentDomain.ReflectionOnlyAssemblyResolve += new ResolveEventHandler(ResolveAssemblyForIntrospection);
        }

       /**********************************************
        * If an AssemblyName has a public key specified, the assembly is assumed
        * to have a strong name and a hash will be computed when the assembly
        * is saved.
        **********************************************/
        public AssemblyBuilder DefineDynamicAssembly(
            AssemblyName            name,
            AssemblyBuilderAccess   access)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalDefineDynamicAssembly(name, access, null,
                                                 null, null, null, null, ref stackMark);
        }
    
        public AssemblyBuilder DefineDynamicAssembly(
            AssemblyName            name,
            AssemblyBuilderAccess   access,
            String                  dir)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalDefineDynamicAssembly(name, access, dir,
                                                 null, null, null, null,
                                                 ref stackMark);
        }
    
        public AssemblyBuilder DefineDynamicAssembly(
            AssemblyName            name,
            AssemblyBuilderAccess   access,
            Evidence                evidence)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalDefineDynamicAssembly(name, access, null,
                                                 evidence, null, null, null,
                                                 ref stackMark);
        }
    
        public AssemblyBuilder DefineDynamicAssembly(
            AssemblyName            name,
            AssemblyBuilderAccess   access,
            PermissionSet           requiredPermissions,
            PermissionSet           optionalPermissions,
            PermissionSet           refusedPermissions)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalDefineDynamicAssembly(name, access, null, null,
                                                 requiredPermissions,
                                                 optionalPermissions,
                                                 refusedPermissions,
                                                 ref stackMark);
        }
    
        public AssemblyBuilder DefineDynamicAssembly(
            AssemblyName            name,
            AssemblyBuilderAccess   access,
            String                  dir,
            Evidence                evidence)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalDefineDynamicAssembly(name, access, dir, evidence,
                                                 null, null, null, ref stackMark);
        }
    
        public AssemblyBuilder DefineDynamicAssembly(
            AssemblyName            name,
            AssemblyBuilderAccess   access,
            String                  dir,
            PermissionSet           requiredPermissions,
            PermissionSet           optionalPermissions,
            PermissionSet           refusedPermissions)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalDefineDynamicAssembly(name, access, dir, null,
                                                 requiredPermissions,
                                                 optionalPermissions,
                                                 refusedPermissions,
                                                 ref stackMark);
        }
    
        public AssemblyBuilder DefineDynamicAssembly(
            AssemblyName            name,
            AssemblyBuilderAccess   access,
            Evidence                evidence,
            PermissionSet           requiredPermissions,
            PermissionSet           optionalPermissions,
            PermissionSet           refusedPermissions)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalDefineDynamicAssembly(name, access, null,
                                                 evidence,
                                                 requiredPermissions,
                                                 optionalPermissions,
                                                 refusedPermissions,
                                                 ref stackMark);
        }
    
        public AssemblyBuilder DefineDynamicAssembly(
            AssemblyName            name,
            AssemblyBuilderAccess   access,
            String                  dir,
            Evidence                evidence,
            PermissionSet           requiredPermissions,
            PermissionSet           optionalPermissions,
            PermissionSet           refusedPermissions)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalDefineDynamicAssembly(name, access, dir,
                                                 evidence,
                                                 requiredPermissions,
                                                 optionalPermissions,
                                                 refusedPermissions,
                                                 ref stackMark);
        }


        public AssemblyBuilder DefineDynamicAssembly(
            AssemblyName            name,
            AssemblyBuilderAccess   access,
            String                  dir,
            Evidence                evidence,
            PermissionSet           requiredPermissions,
            PermissionSet           optionalPermissions,
            PermissionSet           refusedPermissions,
            bool                    isSynchronized)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            AssemblyBuilder assemblyBuilder = InternalDefineDynamicAssembly(name,
                                                                            access,
                                                                            dir,
                                                                            evidence,
                                                                            requiredPermissions,
                                                                            optionalPermissions,
                                                                            refusedPermissions,
                                                                            ref stackMark);
            assemblyBuilder.m_assemblyData.m_isSynchronized = isSynchronized;
            return assemblyBuilder;
        }


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern String nApplyPolicy(AssemblyName an);
        
        // Return the assembly name that results from applying policy.
        [ComVisible(false)]
        public String ApplyPolicy(String assemblyName)
        {
            AssemblyName asmName = new AssemblyName(assemblyName);  

            byte[] pk = asmName.GetPublicKeyToken();
            if (pk == null)
                pk = asmName.GetPublicKey();

            // Simply-named assemblies cannot have policy, so for those,
            // we simply return the passed-in assembly name.
            if ((pk == null) || (pk.Length == 0))
                return assemblyName;
            else
                return nApplyPolicy(asmName);
        }


        public ObjectHandle CreateInstance(String assemblyName,
                                           String typeName)
                                         
        {
            // jit does not check for that, so we should do it ...
            if (this == null)
                throw new NullReferenceException();

            if (assemblyName == null)
                throw new ArgumentNullException("assemblyName");

            return Activator.CreateInstance(assemblyName,
                                            typeName);
        }

        internal ObjectHandle InternalCreateInstanceWithNoSecurity (string assemblyName, string typeName) {
            PermissionSet.s_fullTrust.Assert();
            return CreateInstance(assemblyName, typeName);
        }

        public ObjectHandle CreateInstanceFrom(String assemblyFile,
                                               String typeName)
                                         
        {
            // jit does not check for that, so we should do it ...
            if (this == null)
                throw new NullReferenceException();

            return Activator.CreateInstanceFrom(assemblyFile,
                                                typeName);
        }

        internal ObjectHandle InternalCreateInstanceFromWithNoSecurity (string assemblyName, string typeName) {
            PermissionSet.s_fullTrust.Assert();
            return CreateInstanceFrom(assemblyName, typeName);
        }


        public ObjectHandle CreateInstance(String assemblyName,
                                           String typeName,
                                           Object[] activationAttributes)
                                         
        {
            // jit does not check for that, so we should do it ...
            if (this == null)
                throw new NullReferenceException();

            if (assemblyName == null)
                throw new ArgumentNullException("assemblyName");

            return Activator.CreateInstance(assemblyName,
                                            typeName,
                                            activationAttributes);
        }
                                  
        public ObjectHandle CreateInstanceFrom(String assemblyFile,
                                               String typeName,
                                               Object[] activationAttributes)
                                               
        {
            // jit does not check for that, so we should do it ...
            if (this == null)
                throw new NullReferenceException();

            return Activator.CreateInstanceFrom(assemblyFile,
                                                typeName,
                                                activationAttributes);
        }
                                         
        public ObjectHandle CreateInstance(String assemblyName, 
                                           String typeName, 
                                           bool ignoreCase,
                                           BindingFlags bindingAttr, 
                                           Binder binder,
                                           Object[] args,
                                           CultureInfo culture,
                                           Object[] activationAttributes,
                                           Evidence securityAttributes)
        {
            // jit does not check for that, so we should do it ...
            if (this == null)
                throw new NullReferenceException();
            
            if (assemblyName == null)
                throw new ArgumentNullException("assemblyName");

            return Activator.CreateInstance(assemblyName,
                                            typeName,
                                            ignoreCase,
                                            bindingAttr,
                                            binder,
                                            args,
                                            culture,
                                            activationAttributes,
                                            securityAttributes);
        }

        internal ObjectHandle InternalCreateInstanceWithNoSecurity (string assemblyName, 
                                                                    string typeName,
                                                                    bool ignoreCase,
                                                                    BindingFlags bindingAttr,
                                                                    Binder binder,
                                                                    Object[] args,
                                                                    CultureInfo culture,
                                                                    Object[] activationAttributes,
                                                                    Evidence securityAttributes) {
            PermissionSet.s_fullTrust.Assert();
            return CreateInstance(assemblyName, typeName, ignoreCase, bindingAttr, binder, args, culture, activationAttributes, securityAttributes);
        }

        public ObjectHandle CreateInstanceFrom(String assemblyFile,
                                               String typeName, 
                                               bool ignoreCase,
                                               BindingFlags bindingAttr, 
                                               Binder binder,
                                               Object[] args,
                                               CultureInfo culture,
                                               Object[] activationAttributes,
                                               Evidence securityAttributes)

        {
            // jit does not check for that, so we should do it ...
            if (this == null)
                throw new NullReferenceException();

            return Activator.CreateInstanceFrom(assemblyFile,
                                                typeName,
                                                ignoreCase,
                                                bindingAttr,
                                                binder,
                                                args,
                                                culture,
                                                activationAttributes,
                                                securityAttributes);
        }

        internal ObjectHandle InternalCreateInstanceFromWithNoSecurity (string assemblyName, 
                                                                        string typeName,
                                                                        bool ignoreCase,
                                                                        BindingFlags bindingAttr,
                                                                        Binder binder,
                                                                        Object[] args,
                                                                        CultureInfo culture,
                                                                        Object[] activationAttributes,
                                                                        Evidence securityAttributes) {
            PermissionSet.s_fullTrust.Assert();
            return CreateInstanceFrom(assemblyName, typeName, ignoreCase, bindingAttr, binder, args, culture, activationAttributes, securityAttributes);
        }

        public Assembly Load(AssemblyName assemblyRef)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return Assembly.InternalLoad(assemblyRef, null, ref stackMark, false);
        }
        
        public Assembly Load(String assemblyString)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return Assembly.InternalLoad(assemblyString, null, ref stackMark, false);
        }

        public Assembly Load(byte[] rawAssembly)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return Assembly.nLoadImage(rawAssembly,
                                       null, // symbol store
                                       null, // evidence
                                       ref stackMark,
                                       false
                                      );

        }

        public Assembly Load(byte[] rawAssembly,
                             byte[] rawSymbolStore)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return Assembly.nLoadImage(rawAssembly,
                                       rawSymbolStore,
                                       null, // evidence
                                       ref stackMark,
                                       false // fIntrospection
                                      );
        }

        [SecurityPermissionAttribute( SecurityAction.Demand, ControlEvidence = true )]
        public Assembly Load(byte[] rawAssembly,
                             byte[] rawSymbolStore,
                             Evidence securityEvidence)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return Assembly.nLoadImage(rawAssembly,
                                       rawSymbolStore,
                                       securityEvidence,
                                       ref stackMark,
                                       false // fIntrospection
                                       );
        }

        public Assembly Load(AssemblyName assemblyRef,
                             Evidence assemblySecurity)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return Assembly.InternalLoad(assemblyRef, assemblySecurity, ref stackMark, false);
        }

        public Assembly Load(String assemblyString,
                             Evidence assemblySecurity)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return Assembly.InternalLoad(assemblyString, assemblySecurity, ref stackMark, false);
        }

        public int ExecuteAssembly(String assemblyFile)
        {
            return ExecuteAssembly(assemblyFile, null, null);
        }

        public int ExecuteAssembly(String assemblyFile,
                                   Evidence assemblySecurity)
        {
            return ExecuteAssembly(assemblyFile, assemblySecurity, null);
        }
    
        public int ExecuteAssembly(String assemblyFile,
                                   Evidence assemblySecurity,
                                   String[] args)
        {
            Assembly assembly = Assembly.LoadFrom(assemblyFile, assemblySecurity);
    
            if (args == null)
                args = new String[0];

            return nExecuteAssembly(assembly, args);
        }

        public int ExecuteAssembly(String assemblyFile,
                                   Evidence assemblySecurity,
                                   String[] args,
                                   byte[] hashValue, 
                                   AssemblyHashAlgorithm hashAlgorithm)
        {
            Assembly assembly = Assembly.LoadFrom(assemblyFile, 
                                                  assemblySecurity,
                                                  hashValue,
                                                  hashAlgorithm);
            if (args == null)
                args = new String[0];

            return nExecuteAssembly(assembly, args);
        }

        public int ExecuteAssemblyByName(String assemblyName)
        {
            return ExecuteAssemblyByName(assemblyName, null, null);
        }

        public int ExecuteAssemblyByName(String assemblyName,
                                         Evidence assemblySecurity)
        {
            return ExecuteAssemblyByName(assemblyName, assemblySecurity, null);
        }

        public int ExecuteAssemblyByName(String assemblyName,
                                         Evidence assemblySecurity,
                                         params String[] args)
        {
            Assembly assembly = Assembly.Load(assemblyName, assemblySecurity);
    
            if (args == null)
                args = new String[0];

            return nExecuteAssembly(assembly, args);
        }

        public int ExecuteAssemblyByName(AssemblyName assemblyName,
                                         Evidence assemblySecurity,
                                         params String[] args)
        {
            Assembly assembly = Assembly.Load(assemblyName, assemblySecurity);
    
            if (args == null)
                args = new String[0];

            return nExecuteAssembly(assembly, args);
        }

        public static AppDomain CurrentDomain
        {
            get { return Thread.GetDomain(); }
        }

        public Evidence Evidence
        {
            [SecurityPermissionAttribute(SecurityAction.Demand, ControlEvidence = true)]
            get {
                if (_SecurityIdentity == null) {
                    if (IsDefaultAppDomain()) {
                        //
                        // V1.x compatibility: the evidence of the default
                        // AppDomain is that of the entry assembly.
                        //
                        Assembly entryAsm = Assembly.GetEntryAssembly();
                        if (entryAsm != null)
                            return entryAsm.Evidence;
                        else
                            return new Evidence();
                    }
                    if (nIsDefaultAppDomainForSecurity()) {
                        //
                        // V1.x compatibility: If this is an AppDomain created 
                        // by the default appdomain without an explicit evidence
                        // then reuse the evidence of the default AppDomain.
                        //
                        return GetDefaultDomain().Evidence;
                    }
                }

                Evidence ev = this.InternalEvidence;
                return (ev == null ? ev : ev.Copy());
            }
        }

        internal Evidence InternalEvidence
        {
            get {
                return _SecurityIdentity;
            }
        }

        // Should return a monotonically increasing appdomain ID that is unique for
        // the lifetime of the process.
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern uint GetAppDomainId();

        public String FriendlyName
        {
            get { return nGetFriendlyName(); }
        } 

        public String BaseDirectory
        {
            get {
                return FusionStore.ApplicationBase;
            }
        }

        public String RelativeSearchPath
        {
            get { return FusionStore.PrivateBinPath; }
        }

        public bool ShadowCopyFiles
        {
            get {
                String s = FusionStore.ShadowCopyFiles;
                if((s != null) && 
                   (String.Compare(s, "true", StringComparison.OrdinalIgnoreCase) == 0))
                    return true;
                else
                    return false;
            }
        }

        public override String ToString()
        {
            StringBuilder sb = new StringBuilder();
            
            String fn = nGetFriendlyName();
            if (fn != null) {
                sb.Append(Environment.GetResourceString("Loader_Name") + fn);
                sb.Append(Environment.NewLine);
            }
    
            if(_Policies == null || _Policies.Length == 0) 
                sb.Append(Environment.GetResourceString("Loader_NoContextPolicies")
                          + Environment.NewLine);
            else {
                sb.Append(Environment.GetResourceString("Loader_ContextPolicies")
                          + Environment.NewLine);
                for(int i = 0;i < _Policies.Length; i++) {
                    sb.Append(_Policies[i]);
                    sb.Append(Environment.NewLine);
                }
            }
    
            return sb.ToString();
        }
        
        public Assembly[] GetAssemblies()
        {
            return nGetAssemblies(false /* forIntrospection */);
        }


        public Assembly[] ReflectionOnlyGetAssemblies()
        {
            return nGetAssemblies(true /* forIntrospection */);
        }



        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern Assembly[] nGetAssemblies(bool forIntrospection);

    // this is true when we've nuked the handles etc so really can't do anything
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern bool IsUnloadingForcedFinalize();

    // this is true when we've just started going through the finalizers and are forcing objects to finalize
    // so must be aware that certain infrastructure may have gone away
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool IsFinalizingForUnload();

        // Appends the following string to the private path. Valid paths
        // are of the form "bin;util/i386" etc.
        [Obsolete("AppDomain.AppendPrivatePath has been deprecated. Please investigate the use of AppDomainSetup.PrivateBinPath instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        public void AppendPrivatePath(String path)
        {
            if(path == null || path.Length == 0)
                return;

            String current = FusionStore.Value[(int) AppDomainSetup.LoaderInformation.PrivateBinPathValue];
            StringBuilder appendPath = new StringBuilder();

            if(current != null && current.Length > 0) {
                // See if the last character is a separator
                appendPath.Append(current);
                if((current[current.Length-1] != Path.PathSeparator) &&
                   (path[0] != Path.PathSeparator))
                    appendPath.Append(Path.PathSeparator);
            }
            appendPath.Append(path);

            String result = appendPath.ToString();
            InternalSetPrivateBinPath(result);
        }

        
        [Obsolete("AppDomain.ClearPrivatePath has been deprecated. Please investigate the use of AppDomainSetup.PrivateBinPath instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        public void ClearPrivatePath()
        {
            InternalSetPrivateBinPath(String.Empty);
        }

        [Obsolete("AppDomain.ClearShadowCopyPath has been deprecated. Please investigate the use of AppDomainSetup.ShadowCopyDirectories instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        public void ClearShadowCopyPath()
        {
            InternalSetShadowCopyPath(String.Empty);
        }

        [Obsolete("AppDomain.SetCachePath has been deprecated. Please investigate the use of AppDomainSetup.CachePath instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        public void SetCachePath(String path)
        {
            InternalSetCachePath(path);
        }

        [method:SecurityPermissionAttribute(SecurityAction.LinkDemand, ControlAppDomain = true)]
        [ResourceExposure(ResourceScope.AppDomain)]
        [ResourceConsumption(ResourceScope.AppDomain)]
        public void SetData (string name, object data) {
            SetDataHelper(name, data, null);
        }

        [method:SecurityPermissionAttribute(SecurityAction.LinkDemand, ControlAppDomain = true)]
        [ResourceExposure(ResourceScope.AppDomain)]
        [ResourceConsumption(ResourceScope.AppDomain)]
        public void SetData (string name, object data, IPermission permission) {
            SetDataHelper(name, data, permission);
        }

        [ResourceExposure(ResourceScope.AppDomain)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.AppDomain)]
        private void SetDataHelper (string name, object data, IPermission permission) {
            if (name == null)
                throw new ArgumentNullException("name");

            //
            // Synopsis:
            //   IgnoreSystemPolicy is provided as a legacy flag to allow callers to
            //   skip enterprise, machine and user policy levels. When this flag is set,
            //   any demands triggered in this AppDomain will be evaluated against the
            //   AppDomain CAS policy level that is set on the AppDomain.
            // Security Requirements:
            //   The caller needs to be fully trusted in order to be able to set
            //   this legacy mode.
            // Remarks:
            //   There needs to be an AppDomain policy level set before this compat
            //   switch can be set on the AppDomain.
            //

            if (name.Equals("IgnoreSystemPolicy")) {
                lock (this) {
                    if (!_HasSetPolicy)
                        throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_SetData"));
                }
                new PermissionSet(PermissionState.Unrestricted).Demand();
            }

            int key = AppDomainSetup.Locate(name);

            if(key == -1)
                LocalStore[name] = new object[] {data, permission};
            else {
                if (permission != null)
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_SetData"));
                // Be sure to call these properties, not Value, since
                // these do more than call Value.
                switch(key) {
                case (int) AppDomainSetup.LoaderInformation.DynamicBaseValue:
                    FusionStore.DynamicBase = (string) data;
                    break;
                case (int) AppDomainSetup.LoaderInformation.DevPathValue:
                    FusionStore.DeveloperPath = (string) data;
                    break;
                case (int) AppDomainSetup.LoaderInformation.ShadowCopyDirectoriesValue:
                    FusionStore.ShadowCopyDirectories = (string) data;
                    break;
                case (int) AppDomainSetup.LoaderInformation.DisallowPublisherPolicyValue:
                    if(data != null)
                        FusionStore.DisallowPublisherPolicy = true;
                    else
                        FusionStore.DisallowPublisherPolicy = false;
                    break;
                case (int) AppDomainSetup.LoaderInformation.DisallowCodeDownloadValue:
                    if (data != null)
                         FusionStore.DisallowCodeDownload = true;
                    else
                        FusionStore.DisallowCodeDownload = false;
                    break;
                case (int) AppDomainSetup.LoaderInformation.DisallowBindingRedirectsValue:
                    if(data != null)
                        FusionStore.DisallowBindingRedirects = true;
                    else
                        FusionStore.DisallowBindingRedirects = false;
                    break;
                case (int) AppDomainSetup.LoaderInformation.DisallowAppBaseProbingValue:
                    if(data != null)
                        FusionStore.DisallowApplicationBaseProbing = true;
                    else
                        FusionStore.DisallowApplicationBaseProbing = false;
                    break;
                case (int) AppDomainSetup.LoaderInformation.ConfigurationBytesValue:
                    FusionStore.SetConfigurationBytes((byte[]) data);
                    break;
                default:
                    FusionStore.Value[key] = (string) data;
                    break;
                }
            }
        }

        [ResourceExposure(ResourceScope.AppDomain)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        public Object GetData(string name)
        {
            if(name == null)
                throw new ArgumentNullException("name");

            int key = AppDomainSetup.Locate(name);
            if(key == -1) {
                if(name.Equals(AppDomainSetup.LoaderOptimizationKey))
                    return FusionStore.LoaderOptimization;
                else {
                    object[] data = (object[]) LocalStore[name];
                    if (data == null)
                        return null;
                    if (data[1] != null) {
                        IPermission permission = (IPermission) data[1];
                        permission.Demand();
                    }
                    return data[0];
                }
            }
           else {
                // Be sure to call these properties, not Value, so
                // that the appropriate permission demand will be done
                switch(key) {
                case (int) AppDomainSetup.LoaderInformation.ApplicationBaseValue:
                    return FusionStore.ApplicationBase;
                case (int) AppDomainSetup.LoaderInformation.ConfigurationFileValue:
                    return FusionStore.ConfigurationFile;
                case (int) AppDomainSetup.LoaderInformation.DynamicBaseValue:
                    return FusionStore.DynamicBase;
                case (int) AppDomainSetup.LoaderInformation.DevPathValue:
                    return FusionStore.DeveloperPath;
                case (int) AppDomainSetup.LoaderInformation.ApplicationNameValue:
                    return FusionStore.ApplicationName;
                case (int) AppDomainSetup.LoaderInformation.PrivateBinPathValue:
                    return FusionStore.PrivateBinPath;
                case (int) AppDomainSetup.LoaderInformation.PrivateBinPathProbeValue:
                    return FusionStore.PrivateBinPathProbe;
                case (int) AppDomainSetup.LoaderInformation.ShadowCopyDirectoriesValue:
                    return FusionStore.ShadowCopyDirectories;
                case (int) AppDomainSetup.LoaderInformation.ShadowCopyFilesValue:
                    return FusionStore.ShadowCopyFiles;
                case (int) AppDomainSetup.LoaderInformation.CachePathValue:
                    return FusionStore.CachePath;
                case (int) AppDomainSetup.LoaderInformation.LicenseFileValue:
                    return FusionStore.LicenseFile;
                case (int) AppDomainSetup.LoaderInformation.DisallowPublisherPolicyValue:
                    return FusionStore.DisallowPublisherPolicy;
                case (int) AppDomainSetup.LoaderInformation.DisallowCodeDownloadValue:
                    return FusionStore.DisallowCodeDownload;
                case (int) AppDomainSetup.LoaderInformation.DisallowBindingRedirectsValue:
                    return FusionStore.DisallowBindingRedirects;
                case (int) AppDomainSetup.LoaderInformation.DisallowAppBaseProbingValue:
                    return FusionStore.DisallowApplicationBaseProbing;
                case (int) AppDomainSetup.LoaderInformation.ConfigurationBytesValue:
                    return FusionStore.GetConfigurationBytes();
                default:
#if _DEBUG
                    BCLDebug.Assert(false, "Need to handle new LoaderInformation value in AppDomain.GetData()");
#endif
                    return null;
                }
            }
        }
        
        [Obsolete("AppDomain.GetCurrentThreadId has been deprecated because it does not provide a stable Id when managed threads are running on fibers (aka lightweight threads). To get a stable identifier for a managed thread, use the ManagedThreadId property on Thread.  http://go.microsoft.com/fwlink/?linkid=14202", false)]
        [DllImport(Microsoft.Win32.Win32Native.KERNEL32)]
        public static extern int GetCurrentThreadId();

        [SecurityPermissionAttribute( SecurityAction.Demand, ControlAppDomain = true ),
         ReliabilityContract(Consistency.MayCorruptAppDomain, Cer.MayFail)]            
        public static void Unload(AppDomain domain)
        {
            if (domain == null)
                throw new ArgumentNullException("domain");

            try {
                Int32 domainID = AppDomain.GetIdForUnload(domain);
                if (domainID==0)
                    throw new CannotUnloadAppDomainException();
                AppDomain.nUnload(domainID);
            }
            catch(Exception e) {
                throw e;    // throw it again to reset stack trace
            }
        }

        // Explicitly set policy for a domain (providing policy hasn't been set
        // previously). Making this call will guarantee that previously loaded
        // assemblies will be granted permissions based on the default machine
        // policy that was in place prior to this call.
        [SecurityPermission(SecurityAction.LinkDemand, ControlDomainPolicy=true)]
        public void SetAppDomainPolicy(PolicyLevel domainPolicy)
        {
            if (domainPolicy == null)
                throw new ArgumentNullException("domainPolicy");

            // Check that policy has not been set previously.
            lock (this) {
                if (_HasSetPolicy)
                    throw new PolicyException(Environment.GetResourceString("Policy_PolicyAlreadySet"));
                _HasSetPolicy = true;

                // Make sure that the loader allows us to change security policy
                // at this time (this will throw if not.)
                nChangeSecurityPolicy();
            }

            // Add the new policy level.
            SecurityManager.PolicyManager.AddLevel(domainPolicy);
        }


        // Set the default principal object to be attached to threads if they
        // attempt to bind to a principal while executing in this appdomain. The
        // default can only be set once.
        [SecurityPermissionAttribute(SecurityAction.Demand, Flags=SecurityPermissionFlag.ControlPrincipal)]
        public void SetThreadPrincipal(IPrincipal principal)
        {
            if (principal == null)
                throw new ArgumentNullException("principal");

            lock (this) {
                // Check that principal has not been set previously.
                if (_DefaultPrincipal != null)
                    throw new PolicyException(Environment.GetResourceString("Policy_PrincipalTwice"));

                _DefaultPrincipal = principal;
            }
        }

        // Similar to the above, but sets the class of principal to be created
        // instead.
        [SecurityPermissionAttribute(SecurityAction.Demand, Flags=SecurityPermissionFlag.ControlPrincipal)]
        public void SetPrincipalPolicy(PrincipalPolicy policy)
        {
            _PrincipalPolicy = policy;
        }

        // This method gives AppDomain an infinite life time by preventing a lease from being
        // created
        public override Object InitializeLifetimeService()
        {
            return null;
        }

        // This is useful for requesting execution of some code
        // in another appDomain ... the delegate may be defined 
        // on a marshal-by-value object or a marshal-by-ref or 
        // contextBound object.
        public void DoCallBack(CrossAppDomainDelegate callBackDelegate)
        {
            if (callBackDelegate == null)
                throw new ArgumentNullException("callBackDelegate");

            callBackDelegate();        
        }


        public String DynamicDirectory
        {
            get {
                String dyndir = GetDynamicDir();
                if (dyndir != null)
                    new FileIOPermission( FileIOPermissionAccess.PathDiscovery, dyndir ).Demand();

                return dyndir;
            }
        }
        
        public static AppDomain CreateDomain(String friendlyName,
                                             Evidence securityInfo) // Optional
        {
            return CreateDomain(friendlyName,
                                securityInfo,
                                null);
        }
    
        public static AppDomain CreateDomain(String friendlyName,
                                             Evidence securityInfo, // Optional
                                             String appBasePath,
                                             String appRelativeSearchPath,
                                             bool shadowCopyFiles)
        {            
            AppDomainSetup info = new AppDomainSetup();
            info.ApplicationBase = appBasePath;
            info.PrivateBinPath = appRelativeSearchPath;
            if(shadowCopyFiles)
                info.ShadowCopyFiles = "true";

            return CreateDomain(friendlyName,
                                securityInfo,
                                info);
        }

        public static AppDomain CreateDomain(String friendlyName)
        {
            return CreateDomain(friendlyName, null, null);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private String GetDynamicDir();

        // Private helpers called from unmanaged code.

        // Marshal a single object into a serialized blob.
        private static byte[] MarshalObject(Object o)
        {
            CodeAccessPermission.AssertAllPossible();

            return Serialize(o);
        }

        // Marshal two objects into serialized blobs.
        private static byte[] MarshalObjects(Object o1, Object o2, out byte[] blob2)
        {
            CodeAccessPermission.AssertAllPossible();

            byte[] blob1 = Serialize(o1);
            blob2 = Serialize(o2);
            return blob1;
        }

        // Unmarshal a single object from a serialized blob.
        private static Object UnmarshalObject(byte[] blob)
        {
            CodeAccessPermission.AssertAllPossible();

            return Deserialize(blob);
        }

        // Unmarshal two objects from serialized blobs.
        private static Object UnmarshalObjects(byte[] blob1, byte[] blob2, out Object o2)
        {
            CodeAccessPermission.AssertAllPossible();

            Object o1 = Deserialize(blob1);
            o2 = Deserialize(blob2);
            return o1;
        }

        // Helper routines.
        private static byte[] Serialize(Object o)
        {
            if (o == null)
            {
                return null;
            }
            else if (o is ISecurityEncodable)
            {
                SecurityElement element = ((ISecurityEncodable)o).ToXml();
                MemoryStream ms = new MemoryStream( 4096 );
                ms.WriteByte( 0 );
                StreamWriter writer = new StreamWriter( ms, Encoding.UTF8 );
                element.ToWriter( writer );
                writer.Flush();
                return ms.ToArray();
            }
            else
            {
                MemoryStream ms = new MemoryStream();
                ms.WriteByte( 1 );
                CrossAppDomainSerializer.SerializeObject(o, ms);
                return ms.ToArray();
            }
        }

        private static Object Deserialize(byte[] blob)
        {
            if (blob == null)
                return null;

            if (blob[0] == 0)
            {
                Parser parser = new Parser( blob, Tokenizer.ByteTokenEncoding.UTF8Tokens, 1 );
                SecurityElement root = parser.GetTopElement();
                if (root.Tag.Equals( "IPermission" ) || root.Tag.Equals( "Permission" ))
                {
                    IPermission ip = System.Security.Util.XMLUtil.CreatePermission( root, PermissionState.None, false );

                    if (ip == null)
                    {
                        BCLDebug.Assert( false, "Unable to create permission type" );
                        return null;
                    }

                    ip.FromXml( root );

                    return ip;
                }
                else if (root.Tag.Equals( "PermissionSet" ))
                {
                    PermissionSet permissionSet = new PermissionSet();

                    permissionSet.FromXml( root, false, false );

                    return permissionSet;
                }
                else if (root.Tag.Equals( "PermissionToken" ))
                {
                    PermissionToken pToken = new PermissionToken();

                    pToken.FromXml( root );

                    return pToken;
                }
                else
                {
                    BCLDebug.Assert( false, "Unexpected type found" );
                    return null;
                }

            }
            else
            {
                Object obj = null;
                using(MemoryStream stream = new MemoryStream( blob, 1, blob.Length - 1 )) {
                    obj = CrossAppDomainSerializer.DeserializeObject(stream);
                }

                BCLDebug.Assert( !(obj is IPermission), "IPermission should be xml deserialized" );
                BCLDebug.Assert( !(obj is PermissionSet), "PermissionSet should be xml deserialized" );

                return obj;
            }
        }

        private AppDomain() {
            throw new NotSupportedException(Environment.GetResourceString(ResId.NotSupported_Constructor));
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern AssemblyBuilder nCreateDynamicAssembly(AssemblyName name,
                               Evidence identity,
                               ref StackCrawlMark stackMark,
                               PermissionSet requiredPermissions,
                               PermissionSet optionalPermissions,
                               PermissionSet refusedPermissions,
                               AssemblyBuilderAccess access);

        internal AssemblyBuilder InternalDefineDynamicAssembly(
            AssemblyName            name,
            AssemblyBuilderAccess   access,
            String                  dir,
            Evidence                evidence,
            PermissionSet           requiredPermissions,
            PermissionSet           optionalPermissions,
            PermissionSet           refusedPermissions,
            ref StackCrawlMark      stackMark)
        {
            if (name == null)
                throw new ArgumentNullException("name");

            if (access != AssemblyBuilderAccess.Run && access != AssemblyBuilderAccess.Save &&
                access != AssemblyBuilderAccess.RunAndSave && access != AssemblyBuilderAccess.ReflectionOnly)
                throw new ArgumentException(Environment.GetResourceString("Arg_EnumIllegalVal", (int)access), "access");

            // Set the public key from the key pair if one has been provided.
            // (Overwite any public key in the Assembly name, since it's no
            // longer valid to have a disparity).
            if (name.KeyPair != null)
                name.SetPublicKey(name.KeyPair.PublicKey);

            // If the caller is trusted they can supply identity
            // evidence for the new assembly. Otherwise we copy the
            // current grant and deny sets from the caller's assembly,
            // inject them into the new assembly and mark policy as
            // resolved. If/when the assembly is persisted and
            // reloaded, the normal rules for gathering evidence will
            // be used.
            if (evidence != null)
                new SecurityPermission(SecurityPermissionFlag.ControlEvidence).Demand();
    
            AssemblyBuilder assemblyBuilder = nCreateDynamicAssembly(name,
                                                                     evidence,
                                                                     ref stackMark,
                                                                     requiredPermissions,
                                                                     optionalPermissions,
                                                                     refusedPermissions,
                                                                     access);

            assemblyBuilder.m_assemblyData = new AssemblyBuilderData(assemblyBuilder,
                                                                     name.Name,
                                                                     access,
                                                                     dir);
            assemblyBuilder.m_assemblyData.AddPermissionRequests(requiredPermissions,
                                                                 optionalPermissions,
                                                                 refusedPermissions);
            return assemblyBuilder;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern int nExecuteAssembly(Assembly assembly, String[] args);

        internal void CreateRemotingData()
        {
                    lock(this) {
                        if (_RemotingData == null)
                            _RemotingData = new DomainSpecificRemotingData();
                    }
        }
        
        internal DomainSpecificRemotingData RemotingData
        {
            get 
            { 
                if (_RemotingData == null) 
                    CreateRemotingData();

                return _RemotingData;
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern String nGetFriendlyName();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern bool nIsDefaultAppDomainForSecurity();

        // support reliability for certain event handlers, if the target
        // methods also participate in this discipline.  If caller passes
        // an existing MulticastDelegate, then we could use a MDA to indicate
        // that reliability is not guaranteed.  But if it is a single cast
        // scenario, we can make it work.

        public event EventHandler ProcessExit
        {
            [SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
            add
            {
                if (value != null)
                {
                    RuntimeHelpers.PrepareDelegate(value);
                    lock(this)
                        _processExit += value;
                }
            }
            [SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
            remove
            {
                lock(this)
                    _processExit -= value;
            }
        }


        public event EventHandler DomainUnload
        {
            [SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
            add
            {
                if (value != null)
                {
                    RuntimeHelpers.PrepareDelegate(value);
                    lock(this)
                        _domainUnload += value;
                }
            }
            [SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
            remove
            {
                lock(this)
                    _domainUnload -= value;
            }
        }


        public event UnhandledExceptionEventHandler UnhandledException
        {
            [SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
            add
            {
                if (value != null)
                {
                    RuntimeHelpers.PrepareDelegate(value);
                    lock(this)
                        _unhandledException += value;
                }
            }
            [SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
            remove
        {
                lock(this)
                    _unhandledException -= value;
            }
        }


        private void OnAssemblyLoadEvent(Assembly LoadedAssembly)
        {
            AssemblyLoadEventHandler eventHandler=AssemblyLoad;
            if (eventHandler != null) {
                AssemblyLoadEventArgs ea = new AssemblyLoadEventArgs(LoadedAssembly);
                eventHandler(this, ea);
            }
        }
    
        private Assembly OnResourceResolveEvent(String resourceName)
        {
            ResolveEventHandler eventHandler=ResourceResolve;
            if ( eventHandler == null)
                return null;

            Delegate[] ds = eventHandler.GetInvocationList();
            int len = ds.Length;
            for (int i = 0; i < len; i++) {
                Assembly ret = ((ResolveEventHandler) ds[i])(this, new ResolveEventArgs(resourceName));
                if (ret != null)
                    return ret;              
            }

            return null;
        }
        
        private Assembly OnTypeResolveEvent(String typeName)
        {
            ResolveEventHandler eventHandler=TypeResolve;
            if (eventHandler == null)
                return null;

            Delegate[] ds = eventHandler.GetInvocationList();
            int len = ds.Length;
            for (int i = 0; i < len; i++) {
                Assembly ret = ((ResolveEventHandler) ds[i])(this, new ResolveEventArgs(typeName));
                if (ret != null)
                    return ret;
            }

            return null;
        }

        private Assembly OnAssemblyResolveEvent(String assemblyFullName)
        {
            ResolveEventHandler eventHandler=AssemblyResolve;
            if (eventHandler == null)
                return null;

            Delegate[] ds = eventHandler.GetInvocationList();
            int len = ds.Length;
            for (int i = 0; i < len; i++) {
                Assembly ret = ((ResolveEventHandler) ds[i])(this, new ResolveEventArgs(assemblyFullName));
                if (ret != null)
                    return ret;
            }

            return null;
        }

        private Assembly OnReflectionOnlyAssemblyResolveEvent(String assemblyFullName)
        {
            ResolveEventHandler eventHandler = ReflectionOnlyAssemblyResolve;
            if (eventHandler != null) {

                Delegate[] ds = eventHandler.GetInvocationList();
                int len = ds.Length;
                for (int i = 0; i < len; i++) {
                    Assembly ret = ((ResolveEventHandler) ds[i])(this, new ResolveEventArgs(assemblyFullName));
                    if (ret != null)
                        return ret;              
                }
            }

            return null;
        }

        internal AppDomainSetup FusionStore
        {
            get {
#if _DEBUG
                BCLDebug.Assert(_FusionStore != null, 
                                "Fusion store has not been correctly setup in this domain");
#endif
                return _FusionStore;
            }
        }
        
        [ResourceExposure(ResourceScope.AppDomain)]
        private Hashtable LocalStore
        {
            get { 
                if (_LocalStore != null)
                    return _LocalStore;
                else {
                    _LocalStore = Hashtable.Synchronized(new Hashtable());
                    return _LocalStore;
                }
            }
        }

        private void TurnOnBindingRedirects()
        {
            _FusionStore.DisallowBindingRedirects = false;
        }

        // This will throw a CannotUnloadAppDomainException if the appdomain is
        // in another process.
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]  
        internal static Int32 GetIdForUnload(AppDomain domain)
        {
            if (RemotingServices.IsTransparentProxy(domain))
            {
                return RemotingServices.GetServerDomainIdForProxy(domain);
            }
            else
                return domain.Id;
        }

        // Used to determine if server object context is valid in
        // x-domain remoting scenarios.
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]
        internal static extern bool IsDomainIdValid(Int32 id);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        static internal extern AppDomain GetDefaultDomain();

        // Internal routine to retrieve the default principal object. If this is
        // called before the principal has been explicitly set, it will
        // automatically allocate a default principal based on the policy set by
        // SetPrincipalPolicy.
        internal IPrincipal GetThreadPrincipal()
        {
            IPrincipal principal = null;

            lock (this) {
                if (_DefaultPrincipal == null) {
                    switch (_PrincipalPolicy) {
                    case PrincipalPolicy.NoPrincipal:
                        principal = null;
                        break;
                    case PrincipalPolicy.UnauthenticatedPrincipal:
                        principal = new GenericPrincipal(new GenericIdentity("", ""),
                                                         new String[] {""});
                        break;
                    default:
                        principal = null;
                        break;
                    }
                }
                else
                    principal = _DefaultPrincipal;

                return principal;
            }
        }

        internal void CreateDefaultContext()
        {
                lock(this) {
                    // if it has not been created we ask the Context class to 
                    // create a new default context for this appdomain.
                    if (_DefaultContext == null)
                        _DefaultContext = Context.CreateDefaultContext();
                }            
        }

        internal Context GetDefaultContext()
        {
            if (_DefaultContext == null)
                CreateDefaultContext();
            return _DefaultContext;
        }

        [SecurityPermissionAttribute( SecurityAction.Demand, ControlAppDomain = true )]
        public static AppDomain CreateDomain(String friendlyName,
                                             Evidence securityInfo,
                                             AppDomainSetup info)
        {
            AppDomainManager domainManager = AppDomain.CurrentDomain.DomainManager;
            if (domainManager != null)
                return domainManager.CreateDomain(friendlyName, securityInfo, info);

            // No AppDomainManager is set up for this domain
            if (friendlyName == null)
                throw new ArgumentNullException(Environment.GetResourceString("ArgumentNull_String"));
                
            // If evidence is provided, we check to make sure that is allowed.    
            if (securityInfo != null)
                new SecurityPermission( SecurityPermissionFlag.ControlEvidence ).Demand();

            return nCreateDomain(friendlyName,
                                 info,
                                 securityInfo,
                                 securityInfo == null ? AppDomain.CurrentDomain.InternalEvidence : null,
                                 AppDomain.CurrentDomain.GetSecurityDescriptor());
        }


        public static AppDomain CreateDomain(String friendlyName,
                                             Evidence securityInfo, // Optional
                                             String appBasePath,
                                             String appRelativeSearchPath,
                                             bool shadowCopyFiles,
                                             AppDomainInitializer adInit, 
                                             string[] adInitArgs) 
        {
            AppDomainSetup info = new AppDomainSetup();
            info.ApplicationBase = appBasePath;
            info.PrivateBinPath = appRelativeSearchPath;
            info.AppDomainInitializer=adInit;
            info.AppDomainInitializerArguments=adInitArgs;
            if(shadowCopyFiles)
                info.ShadowCopyFiles = "true";

            return CreateDomain(friendlyName,
                                securityInfo,
                                info);
        }

        private void SetupFusionStore(AppDomainSetup info)
        {
            // Create the application base and configuration file from the imagelocation
            // passed in or use the Win32 Image name.
            if(info.Value[(int) AppDomainSetup.LoaderInformation.ApplicationBaseValue] == null ||
               info.Value[(int) AppDomainSetup.LoaderInformation.ConfigurationFileValue] == null ) {
                AppDomain defaultDomain = GetDefaultDomain();
                if (this == defaultDomain) {
                    // The default domain gets its defaults from the main process.
                    info.SetupDefaultApplicationBase(RuntimeEnvironment.GetModuleFileName());
                }
                else {
                    // Other domains get their defaults from the default domain. This way, a host process
                    // can use AppDomainManager to set up the defaults for every domain created in the process.
                    if (info.Value[(int) AppDomainSetup.LoaderInformation.ConfigurationFileValue] == null)
                        info.ConfigurationFile = defaultDomain.FusionStore.Value[(int) AppDomainSetup.LoaderInformation.ConfigurationFileValue];
                    if (info.Value[(int) AppDomainSetup.LoaderInformation.ApplicationBaseValue] == null)
                        info.ApplicationBase = defaultDomain.FusionStore.Value[(int) AppDomainSetup.LoaderInformation.ApplicationBaseValue];
                    if (info.Value[(int) AppDomainSetup.LoaderInformation.ApplicationNameValue] == null)
                        info.ApplicationName = defaultDomain.FusionStore.Value[(int) AppDomainSetup.LoaderInformation.ApplicationNameValue];
                }
            }

            // If there is no relative path then check the
            // environment
            if(info.Value[(int) AppDomainSetup.LoaderInformation.PrivateBinPathValue] == null)
                info.PrivateBinPath = Environment.nativeGetEnvironmentVariable(AppDomainSetup.PrivateBinPathEnvironmentVariable);

            // Add the developer path if it exists on this
            // machine.
            if(info.DeveloperPath == null)
                info.DeveloperPath = RuntimeEnvironment.GetDeveloperPath();

            // Set up the fusion context
            IntPtr fusionContext = GetFusionContext();
            info.SetupFusionContext(fusionContext);

            // Set loader optimization policy
            if (info.LoaderOptimization != LoaderOptimization.NotSpecified)
                UpdateLoaderOptimization((int) info.LoaderOptimization);

            // This must be the last action taken
            _FusionStore = info;
        }

        // used to package up evidence, so it can be serialized
        //   for the call to InternalRemotelySetupRemoteDomain
        [Serializable]
        private class EvidenceCollection
        {
            public Evidence ProvidedSecurityInfo;
            public Evidence CreatorsSecurityInfo;
        }

        private static void RunInitializer(AppDomainSetup setup)
        {
            if (setup.AppDomainInitializer!=null)
            {
                string[] args=null;
                if (setup.AppDomainInitializerArguments!=null)
                    args=(string[])setup.AppDomainInitializerArguments.Clone();
                setup.AppDomainInitializer(args);
            }
        }

        // Used to switch into other AppDomain and call SetupRemoteDomain.
        //   We cannot simply call through the proxy, because if there
        //   are any remoting sinks registered, they can add non-mscorlib
        //   objects to the message (causing an assembly load exception when
        //   we try to deserialize it on the other side)
        private static object RemotelySetupRemoteDomain(AppDomain appDomainProxy,
                                                        String friendlyName,
                                                        AppDomainSetup setup,
                                                        Evidence providedSecurityInfo,
                                                        Evidence creatorsSecurityInfo,
                                                        IntPtr parentSecurityDescriptor)
        {
            BCLDebug.Assert(RemotingServices.IsTransparentProxy(appDomainProxy),
                            "Expected a proxy to the AppDomain.");

            // get context and appdomain id
            IntPtr  contextId;
            int     domainId;
            RemotingServices.GetServerContextAndDomainIdForProxy(
               appDomainProxy, out contextId, out domainId);

            if (contextId==IntPtr.Zero)
                throw new AppDomainUnloadedException();
            
            // serialize evidence
            EvidenceCollection evidenceCollection = null;
            if ((providedSecurityInfo != null) ||
                (creatorsSecurityInfo != null)) {
                evidenceCollection = new EvidenceCollection();
                evidenceCollection.ProvidedSecurityInfo = providedSecurityInfo;
                evidenceCollection.CreatorsSecurityInfo = creatorsSecurityInfo;
            }

            bool bNeedGenericFormatter = false;
            char[] serProvidedEvidence = null, serCreatorEvidence = null;
            byte[] serializedEvidence = null;
            AppDomainInitializerInfo initializerInfo=null;

            if (providedSecurityInfo != null) {
                serProvidedEvidence = PolicyManager.MakeEvidenceArray(providedSecurityInfo, true);
                if (serProvidedEvidence == null)
                    bNeedGenericFormatter = true;
            }
            if (creatorsSecurityInfo != null && !bNeedGenericFormatter) {
                serCreatorEvidence = PolicyManager.MakeEvidenceArray(creatorsSecurityInfo, true);
                if (serCreatorEvidence == null)
                    bNeedGenericFormatter = true;
            }
            if (evidenceCollection != null && bNeedGenericFormatter) {
                serProvidedEvidence = serCreatorEvidence = null;
                serializedEvidence =
                    CrossAppDomainSerializer.SerializeObject(evidenceCollection).GetBuffer();                
            }

            if (setup!=null && setup.AppDomainInitializer!=null)
                initializerInfo=new AppDomainInitializerInfo(setup.AppDomainInitializer);

            return InternalRemotelySetupRemoteDomain(contextId,
                                              domainId,
                                              friendlyName, 
                                              setup,
                                              parentSecurityDescriptor,
                                              serProvidedEvidence,
                                              serCreatorEvidence,
                                              serializedEvidence,
                                              initializerInfo);    
        } // RemotelySetupRemoteDomain

        [MethodImplAttribute(MethodImplOptions.NoInlining)]
        private static Object InternalRemotelySetupRemoteDomainHelper(Object[] args)
        {
            String           friendlyName               = (String)args[0];
            AppDomainSetup   setup                      = (AppDomainSetup)args[1];
            IntPtr           parentSecurityDescriptor   = (IntPtr)args[2];
            char[]           serProvidedEvidence        = (char[])args[3];
            char[]           serCreatorEvidence         = (char[])args[4];
            byte[]           serializedEvidence         = (byte[])args[5];
            AppDomainInitializerInfo initializerInfo    = (AppDomainInitializerInfo)args[6];

            AppDomain ad = Thread.CurrentContext.AppDomain;
            AppDomainSetup newSetup=new AppDomainSetup(setup,false);
            ad.SetupFusionStore(newSetup);

            // extract evidence
            Evidence providedSecurityInfo = null;
            Evidence creatorsSecurityInfo = null;

            if (serializedEvidence == null) {
                if (serProvidedEvidence != null)
                    providedSecurityInfo = new Evidence(serProvidedEvidence);
                if (serCreatorEvidence != null)
                    creatorsSecurityInfo = new Evidence(serCreatorEvidence);
            }
            else {
                EvidenceCollection evidenceCollection = (EvidenceCollection)
                    CrossAppDomainSerializer.DeserializeObject(new MemoryStream(serializedEvidence));
                providedSecurityInfo  = evidenceCollection.ProvidedSecurityInfo;
                creatorsSecurityInfo  = evidenceCollection.CreatorsSecurityInfo;
            }

            // set up the friendly name
            ad.nSetupFriendlyName(friendlyName);

            // set up the AppDomainManager for this domain and initialize security.
            ad.SetDomainManager(providedSecurityInfo,
                                creatorsSecurityInfo,
                                parentSecurityDescriptor,
                                true);

            // can load user code now
            if(initializerInfo!=null)
                newSetup.AppDomainInitializer=initializerInfo.Unwrap();
            RunInitializer(newSetup);

            // Activate the application if needed.
            ObjectHandle oh = null;
            return RemotingServices.MarshalInternal(oh, null, null);
        }

        [MethodImplAttribute(MethodImplOptions.NoInlining)]
        private static object InternalRemotelySetupRemoteDomain(IntPtr contextId, 
                                                              int domainId,
                                                              String friendlyName,
                                                              AppDomainSetup setup,
                                                              IntPtr parentSecurityDescriptor,
                                                              char[] serProvidedEvidence,
                                                              char[] serCreatorEvidence,
                                                              byte[] serializedEvidence,
                                                              AppDomainInitializerInfo initializerInfo)
        {
            InternalCrossContextDelegate xctxDel = 
                new InternalCrossContextDelegate(InternalRemotelySetupRemoteDomainHelper);

            Object[] args = new Object[] 
                {
                    friendlyName, 
                    setup, 
                    parentSecurityDescriptor, 
                    serProvidedEvidence,
                    serCreatorEvidence,
                    serializedEvidence,
                    initializerInfo
                };
            return Thread.CurrentThread.InternalCrossContextCallback(null, contextId, domainId, xctxDel, args);
        } // InternalSetupRemoteDomain

        // This routine is called from unmanaged code to 
        // set the default fusion context.
        private void SetupDomain(bool allowRedirects, String path, String configFile)
        {
            // It is possible that we could have multiple threads initializing
            // the default domain. We will just take the winner of these two.
            // (eg. one thread doing a com call and another doing attach for IJW)
            lock (this) {
                if(_FusionStore == null) {
                    AppDomainSetup setup = new AppDomainSetup();
                    if(path != null)
                        setup.Value[(int) AppDomainSetup.LoaderInformation.ApplicationBaseValue] = path;
                    if(configFile != null)
                        setup.Value[(int) AppDomainSetup.LoaderInformation.ConfigurationFileValue] = configFile;

                    // Default fusion context starts with binding redirects turned off.
                    if (!allowRedirects)
                        setup.DisallowBindingRedirects = true;

                    SetupFusionStore(setup);
                }
            }
        }

       private void SetupLoaderOptimization(LoaderOptimization policy)
        {
            if(policy != LoaderOptimization.NotSpecified) {
#if _DEBUG
                BCLDebug.Assert(FusionStore.LoaderOptimization == LoaderOptimization.NotSpecified,
                                "It is illegal to change the Loader optimization on a domain");
#endif
                FusionStore.LoaderOptimization = policy;
                UpdateLoaderOptimization((int) FusionStore.LoaderOptimization);
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern IntPtr GetFusionContext();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern IntPtr GetSecurityDescriptor();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern AppDomain nCreateDomain(String friendlyName,
                                                      AppDomainSetup setup,
                                                      Evidence providedSecurityInfo,
                                                      Evidence creatorsSecurityInfo,
                                                      IntPtr parentSecurityDescriptor);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern ObjRef nCreateInstance(String friendlyName,
                                                      AppDomainSetup setup,
                                                      Evidence providedSecurityInfo,
                                                      Evidence creatorsSecurityInfo,
                                                      IntPtr parentSecurityDescriptor);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void nSetupDomainSecurity(Evidence appDomainEvidence, IntPtr creatorsSecurityDescriptor, bool publishAppDomain);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void nSetupFriendlyName(string friendlyName);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern void UpdateLoaderOptimization(int optimization);

        //
        // This is just designed to prevent compiler warnings.
        // This field is used from native, but we need to prevent the compiler warnings.
        //

        [Obsolete("AppDomain.SetShadowCopyPath has been deprecated. Please investigate the use of AppDomainSetup.ShadowCopyDirectories instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        public void SetShadowCopyPath(String path)
        {
            InternalSetShadowCopyPath(path);
        }

        [Obsolete("AppDomain.SetShadowCopyFiles has been deprecated. Please investigate the use of AppDomainSetup.ShadowCopyFiles instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        public void SetShadowCopyFiles()
        {
            InternalSetShadowCopyFiles();
        }

        [Obsolete("AppDomain.SetDynamicBase has been deprecated. Please investigate the use of AppDomainSetup.DynamicBase instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        public void SetDynamicBase(String path)
        {
            InternalSetDynamicBase(path);
        }

        public AppDomainSetup SetupInformation
        {
            get {
                return new AppDomainSetup(FusionStore,true);
            }
        }

        internal void InternalSetShadowCopyPath(String path)
        {
            IntPtr fusionContext = GetFusionContext();
            AppDomainSetup.UpdateContextProperty(fusionContext, AppDomainSetup.ShadowCopyDirectoriesKey, path);
            FusionStore.ShadowCopyDirectories = path;
        }

        internal void InternalSetShadowCopyFiles()
        {
            IntPtr fusionContext = GetFusionContext();
            AppDomainSetup.UpdateContextProperty(fusionContext, AppDomainSetup.ShadowCopyFilesKey, "true");
            FusionStore.ShadowCopyFiles = "true";
        }

        internal void InternalSetCachePath(String path)
        {
            IntPtr fusionContext = GetFusionContext();
            FusionStore.CachePath = path;
            AppDomainSetup.UpdateContextProperty(fusionContext, AppDomainSetup.CachePathKey,
                                                 FusionStore.Value[(int) AppDomainSetup.LoaderInformation.CachePathValue]);
        }

        internal void InternalSetPrivateBinPath(String path)
        {
            IntPtr fusionContext = GetFusionContext();
            AppDomainSetup.UpdateContextProperty(fusionContext, AppDomainSetup.PrivateBinPathKey, path);
            FusionStore.PrivateBinPath = path;
        }

        internal void InternalSetDynamicBase(String path)
        {
            IntPtr fusionContext = GetFusionContext();
            FusionStore.DynamicBase = path;

            AppDomainSetup.UpdateContextProperty(fusionContext, AppDomainSetup.DynamicBaseKey,
                                                 FusionStore.Value[(int) AppDomainSetup.LoaderInformation.DynamicBaseValue]);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern String IsStringInterned(String str);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern String GetOrInternString(String str);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern void nGetGrantSet( out PermissionSet granted, out PermissionSet denied );

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern void nChangeSecurityPolicy();

        [MethodImplAttribute(MethodImplOptions.InternalCall),
         ReliabilityContract(Consistency.MayCorruptAppDomain, Cer.MayFail)]   
        internal static extern void nUnload(Int32 domainInternal);
           
        public Object CreateInstanceAndUnwrap(String assemblyName,
                                              String typeName)
        {
            ObjectHandle oh = CreateInstance(assemblyName, typeName);
            if (oh == null)
                return null;

            return oh.Unwrap();
        } // CreateInstanceAndUnwrap


        public Object CreateInstanceAndUnwrap(String assemblyName, 
                                              String typeName,
                                              Object[] activationAttributes)
        {
            ObjectHandle oh = CreateInstance(assemblyName, typeName, activationAttributes);
            if (oh == null)
                return null; 

            return oh.Unwrap();
        } // CreateInstanceAndUnwrap


        public Object CreateInstanceAndUnwrap(String assemblyName, 
                                              String typeName, 
                                              bool ignoreCase,
                                              BindingFlags bindingAttr, 
                                              Binder binder,
                                              Object[] args,
                                              CultureInfo culture,
                                              Object[] activationAttributes,
                                              Evidence securityAttributes)
        {
            ObjectHandle oh = CreateInstance(assemblyName, typeName, ignoreCase, bindingAttr,
                binder, args, culture, activationAttributes, securityAttributes);
            if (oh == null)
                return null; 
            
            return oh.Unwrap();
        } // CreateInstanceAndUnwrap



        // The first parameter should be named assemblyFile, but it was incorrectly named in a previous 
        //  release, and the compatibility police won't let us change the name now.
        public Object CreateInstanceFromAndUnwrap(String assemblyName,
                                                  String typeName)
        {
            ObjectHandle oh = CreateInstanceFrom(assemblyName, typeName);
            if (oh == null)
                return null;  

            return oh.Unwrap();                
        } // CreateInstanceAndUnwrap


        // The first parameter should be named assemblyFile, but it was incorrectly named in a previous 
        //  release, and the compatibility police won't let us change the name now.
        public Object CreateInstanceFromAndUnwrap(String assemblyName,
                                                  String typeName,
                                                  Object[] activationAttributes)
        {
            ObjectHandle oh = CreateInstanceFrom(assemblyName, typeName, activationAttributes);
            if (oh == null)
                return null; 

            return oh.Unwrap();
        } // CreateInstanceAndUnwrap


        // The first parameter should be named assemblyFile, but it was incorrectly named in a previous 
        //  release, and the compatibility police won't let us change the name now.
        public Object CreateInstanceFromAndUnwrap(String assemblyName, 
                                                  String typeName, 
                                                  bool ignoreCase,
                                                  BindingFlags bindingAttr, 
                                                  Binder binder,
                                                  Object[] args,
                                                  CultureInfo culture,
                                                  Object[] activationAttributes,
                                                  Evidence securityAttributes)
        {
            ObjectHandle oh = CreateInstanceFrom(assemblyName, typeName, ignoreCase, bindingAttr,
                binder, args, culture, activationAttributes, securityAttributes);
            if (oh == null)
                return null; 

            return oh.Unwrap();
        } // CreateInstanceAndUnwrap

        public Int32 Id
        {
            [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]  
            get {
                return GetId();
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [ReliabilityContract(Consistency.WillNotCorruptState, Cer.Success)]              
        internal extern Int32 GetId();

        public bool IsDefaultAppDomain()
        {
            if (this == GetDefaultDomain())
                return true;
            return false;
        }

        private static AppDomainSetup InternalCreateDomainSetup(String imageLocation)
        {
            int i = imageLocation.LastIndexOf('\\');
#if PLATFORM_UNIX
            int j = imageLocation.LastIndexOf('/');
            i = i > j ? i : j;
#endif
 
            BCLDebug.Assert(i != -1, "invalid image location");

            AppDomainSetup info = new AppDomainSetup();
            info.ApplicationBase = imageLocation.Substring(0, i+1);

            StringBuilder config = new StringBuilder(imageLocation.Substring(i+1));
            config.Append(AppDomainSetup.ConfigurationExtension);
            info.ConfigurationFile = config.ToString();

            return info;
        }

        // Used by the validator for testing but not executing an assembly
        private static AppDomain InternalCreateDomain(String imageLocation)
        {
            AppDomainSetup info = InternalCreateDomainSetup(imageLocation);

            return CreateDomain("Validator",
                                null,
                                info);
        }

        private void InternalSetDomainContext(String imageLocation)
        {
            SetupFusionStore(InternalCreateDomainSetup(imageLocation));
        }

        void _AppDomain.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _AppDomain.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _AppDomain.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _AppDomain.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }
    }

    //  CallBacks provide a facility to request execution of some code
    //  in another context/appDomain.
    //  CrossAppDomainDelegate type is defined for appdomain call backs. 
    //  The delegate used to request a callbak through the DoCallBack method
    //  must be of CrossContextDelegate type.
[System.Runtime.InteropServices.ComVisible(true)]
    public delegate void CrossAppDomainDelegate();
}
