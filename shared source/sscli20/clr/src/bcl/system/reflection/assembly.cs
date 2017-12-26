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
** Class: Assembly
**
**
** Purpose: For Assembly-related stuff.
**
**
=============================================================================*/

using System;
using System.Collections;
using CultureInfo = System.Globalization.CultureInfo;
using System.Security;
using System.Security.Policy;
using System.Security.Permissions;
using System.IO;
using System.Reflection.Emit;
using System.Reflection.Cache;
using StringBuilder = System.Text.StringBuilder;
using System.Configuration.Assemblies;
using StackCrawlMark = System.Threading.StackCrawlMark;
using System.Runtime.InteropServices;
using BinaryFormatter = System.Runtime.Serialization.Formatters.Binary.BinaryFormatter;
using System.Runtime.CompilerServices;
using SecurityZone = System.Security.SecurityZone;
using IEvidenceFactory = System.Security.IEvidenceFactory;
using System.Runtime.Serialization;
using Microsoft.Win32;
using System.Threading;
using __HResults = System.__HResults;
using System.Runtime.Versioning;


namespace System.Reflection 
{

    [Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public delegate Module ModuleResolveEventHandler(Object sender, ResolveEventArgs e);


    [Serializable()]
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_Assembly))]
[System.Runtime.InteropServices.ComVisible(true)]
    public class Assembly : _Assembly, IEvidenceFactory, ICustomAttributeProvider, ISerializable
    {

        // READ ME
        // If you modify any of these fields, you must also update the 
        // AssemblyBaseObject structure in object.h
        internal AssemblyBuilderData m_assemblyData;

        [method:SecurityPermissionAttribute( SecurityAction.LinkDemand, ControlAppDomain = true )]
        public event ModuleResolveEventHandler ModuleResolve;

        private InternalCache m_cachedData;
        private IntPtr m_assembly;    // slack for ptr datum on unmanaged side
    
        private const String s_localFilePrefix = "file:";
       
        public virtual String CodeBase
        {
            get {
                String codeBase = nGetCodeBase(false);
                VerifyCodeBaseDiscovery(codeBase);
                return codeBase;
            }
        }

        public virtual String EscapedCodeBase
        {
            get {
                return AssemblyName.EscapeCodeBase(CodeBase);
            }
        }

        public virtual AssemblyName GetName()
        {
            return GetName(false);
        }

        internal unsafe AssemblyHandle AssemblyHandle
        {
            get {
                return new AssemblyHandle((void*)m_assembly);
            }
        }
    
        // If the assembly is copied before it is loaded, the codebase will be set to the
        // actual file loaded if fCopiedName is true. If it is false, then the original code base
        // is returned.
        public virtual AssemblyName GetName(bool copiedName)
        {
            AssemblyName an = new AssemblyName();

            String codeBase = nGetCodeBase(copiedName);
            VerifyCodeBaseDiscovery(codeBase);

            an.Init(nGetSimpleName(), 
                    nGetPublicKey(),
                    null, // public key token
                    GetVersion(),
                    GetLocale(),
                    nGetHashAlgorithm(),
                    AssemblyVersionCompatibility.SameMachine,
                    codeBase,
                    nGetFlags() | AssemblyNameFlags.PublicKey,
                    null); // strong name key pair
            
            an.ProcessorArchitecture = ComputeProcArchIndex();
            return an;
        }

        public virtual String FullName
        {
            get {
                // If called by Object.ToString(), return val may be NULL.
                String s;
                if ((s = (String)Cache[CacheObjType.AssemblyName]) != null)
                    return s;

                s = GetFullName();
                if (s != null)
                    Cache[CacheObjType.AssemblyName] = s;

                return s;
            }
        }


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern String CreateQualifiedName(String assemblyName, String typeName);
           
        public virtual MethodInfo EntryPoint
        {
            get {
                RuntimeMethodHandle methodHandle = nGetEntryPoint();

                if (!methodHandle.IsNullHandle())                   
                    return (MethodInfo)RuntimeType.GetMethodBase(methodHandle);

                return null;
            }
        }

        public static Assembly GetAssembly(Type type)
        {
            if (type == null)
                throw new ArgumentNullException("type");
    
            Module m = type.Module;
            if (m == null)
                return null;
            else
                return m.Assembly;
        }

        Type _Assembly.GetType()
        {
            return base.GetType();
        }

        public virtual Type GetType(String name)
        {
            return GetType(name, false, false);
        }
    
        public virtual Type GetType(String name, bool throwOnError)
        {
            return GetType(name, throwOnError, false);
        }
            
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Type GetType(String name, bool throwOnError, bool ignoreCase);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern virtual Type[] GetExportedTypes();

        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly, ResourceScope.Machine | ResourceScope.Assembly)]
        public virtual Type[] GetTypes()
        {
            Module[] m = nGetModules(true, false);

            int iNumModules = m.Length;
            int iFinalLength = 0;
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            Type[][] ModuleTypes = new Type[iNumModules][];

            for (int i = 0; i < iNumModules; i++) {
                ModuleTypes[i] = m[i].GetTypesInternal(ref stackMark);
                iFinalLength += ModuleTypes[i].Length;
            }
            
            int iCurrent = 0;
            Type[] ret = new Type[iFinalLength];
            for (int i = 0; i < iNumModules; i++) {
                int iLength = ModuleTypes[i].Length;
                Array.Copy(ModuleTypes[i], 0, ret, iCurrent, iLength);
                iCurrent += iLength;
            }

            return ret;
        }

        // Load a resource based on the NameSpace of the type.
        public virtual Stream GetManifestResourceStream(Type type, String name)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return GetManifestResourceStream(type, name, false, ref stackMark);
        }
    
        public virtual Stream GetManifestResourceStream(String name)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return GetManifestResourceStream(name, ref stackMark, false);
        }

        public Assembly GetSatelliteAssembly(CultureInfo culture)
        {
            return InternalGetSatelliteAssembly(culture, null, true);
        }

        // Useful for binding to a very specific version of a satellite assembly
        public Assembly GetSatelliteAssembly(CultureInfo culture, Version version)
        {
            return InternalGetSatelliteAssembly(culture, version, true);
        }

        public virtual Evidence Evidence
        {
            [SecurityPermissionAttribute( SecurityAction.Demand, ControlEvidence = true )]
            get
            {
                return nGetEvidence().Copy();
            }           
        }


        // ISerializable implementation
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)]
        public virtual void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            if (info==null)
                throw new ArgumentNullException("info");


            UnitySerializationHolder.GetUnitySerializationInfo(info,
                                                               UnitySerializationHolder.AssemblyUnity, 
                                                               this.FullName, 
                                                               this);
        }

        internal bool AptcaCheck(Assembly sourceAssembly) { return AssemblyHandle.AptcaCheck(sourceAssembly.AssemblyHandle); }

        [ComVisible(false)]
        public Module ManifestModule 
        { 
            get 
            {   
                ModuleHandle manifestModuleHandle = AssemblyHandle.GetManifestModule();
                if(!manifestModuleHandle.IsNullHandle())
                    return manifestModuleHandle.GetModule(); 
                return null;
            } 
        } 
    
        public virtual Object[] GetCustomAttributes(bool inherit)
        {
            return CustomAttribute.GetCustomAttributes(this, typeof(object) as RuntimeType);
        }
                    
        public virtual Object[] GetCustomAttributes(Type attributeType, bool inherit)
        {
            if (attributeType == null)
                throw new ArgumentNullException("attributeType");

            RuntimeType attributeRuntimeType = attributeType.UnderlyingSystemType as RuntimeType;

            if (attributeRuntimeType == null) 
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeType"),"attributeType");

            return CustomAttribute.GetCustomAttributes(this, attributeRuntimeType);
        }

        public virtual bool IsDefined(Type attributeType, bool inherit)
        {
            if (attributeType == null)
                throw new ArgumentNullException("attributeType");

            RuntimeType attributeRuntimeType = attributeType.UnderlyingSystemType as RuntimeType;

            if (attributeRuntimeType == null) 
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeType"),"caType");

            return CustomAttribute.IsDefined(this, attributeRuntimeType);
        }
        
        
        // Locate an assembly by the name of the file containing the manifest.
        public static Assembly LoadFrom(String assemblyFile)
        {
            // The stack mark is used for MDA filtering
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalLoadFrom(assemblyFile,
                                    null, // securityEvidence
                                    null, // hashValue
                                    AssemblyHashAlgorithm.None,
                                    false, // forIntrospection
                                    ref stackMark);
        }

        // Locate an assembly for reflection by the name of the file containing the manifest.
        public static Assembly ReflectionOnlyLoadFrom(String assemblyFile)
        {
            // The stack mark is ingored for ReflectionOnlyLoadFrom
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalLoadFrom(assemblyFile,
                                    null, //securityEvidence
                                    null, //hashValue
                                    AssemblyHashAlgorithm.None,
                                    true,  //forIntrospection
                                    ref stackMark);
        }

        // Evidence is protected in Assembly.Load()
        public static Assembly LoadFrom(String assemblyFile, 
                                        Evidence securityEvidence)
        {
            // The stack mark is used for MDA filtering
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalLoadFrom(assemblyFile,
                                    securityEvidence,
                                    null, // hashValue
                                    AssemblyHashAlgorithm.None,
                                    false, // forIntrospection
                                    ref stackMark);
        }

        // Evidence is protected in Assembly.Load()
        public static Assembly LoadFrom(String assemblyFile, 
                                        Evidence securityEvidence,
                                        byte[] hashValue, 
                                        AssemblyHashAlgorithm hashAlgorithm)
        {
            // The stack mark is used for MDA filtering
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalLoadFrom(assemblyFile, securityEvidence, hashValue, hashAlgorithm, false, ref stackMark);
        }


        private static Assembly InternalLoadFrom(String assemblyFile, 
                                                 Evidence securityEvidence,
                                                 byte[] hashValue, 
                                                 AssemblyHashAlgorithm hashAlgorithm,
                                                 bool forIntrospection,
                                                 ref StackCrawlMark stackMark)
        {
            if (assemblyFile == null)
                throw new ArgumentNullException("assemblyFile");
    
            AssemblyName an = new AssemblyName();
            an.CodeBase = assemblyFile;
            an.SetHashControl(hashValue, hashAlgorithm);

            return InternalLoad(an, securityEvidence, ref stackMark, forIntrospection);
        }


        // Locate an assembly by the long form of the assembly name. 
        // eg. "Toolbox.dll, version=1.1.10.1220, locale=en, publickey=1234567890123456789012345678901234567890"
        public static Assembly Load(String assemblyString)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalLoad(assemblyString, null, ref stackMark, false);
        }

        // Locate an assembly for reflection by the long form of the assembly name. 
        // eg. "Toolbox.dll, version=1.1.10.1220, locale=en, publickey=1234567890123456789012345678901234567890"
        //
        public static Assembly ReflectionOnlyLoad(String assemblyString)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalLoad(assemblyString, null, ref stackMark, true /*forIntrospection*/);
        }
    
        public static Assembly Load(String assemblyString, Evidence assemblySecurity)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalLoad(assemblyString, assemblySecurity, ref stackMark, false);
        }

        // Locate an assembly by its name. The name can be strong or
        // weak. The assembly is loaded into the domain of the caller.
        static public Assembly Load(AssemblyName assemblyRef)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalLoad(assemblyRef, null, ref stackMark, false);
        }


        static public Assembly Load(AssemblyName assemblyRef, Evidence assemblySecurity)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return InternalLoad(assemblyRef, assemblySecurity, ref stackMark, false);
        }

        // used by vm
        static unsafe private IntPtr LoadWithPartialNameHack(String partialName, bool cropPublicKey)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
	    
            Assembly result = null;
            AssemblyName an = new AssemblyName(partialName);

            if (!IsSimplyNamed(an))
            {
                if (cropPublicKey)
                {
                    an.SetPublicKey(null);
                    an.SetPublicKeyToken(null);
                }
                AssemblyName GACAssembly = EnumerateCache(an);
                if(GACAssembly != null)
                    result = InternalLoad(GACAssembly, null,
                                        ref stackMark, false);
            }
            
            if (result == null)
                return (IntPtr)0;
        
            return (IntPtr)result.AssemblyHandle.Value;
        }        

        [Obsolete("This method has been deprecated. Please use Assembly.Load() instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        static public Assembly LoadWithPartialName(String partialName)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return LoadWithPartialNameInternal(partialName, null, ref stackMark);
        }

        [Obsolete("This method has been deprecated. Please use Assembly.Load() instead. http://go.microsoft.com/fwlink/?linkid=14202")]
        static public Assembly LoadWithPartialName(String partialName, Evidence securityEvidence)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return LoadWithPartialNameInternal(partialName, securityEvidence, ref stackMark);
        }

        static internal Assembly LoadWithPartialNameInternal(String partialName, Evidence securityEvidence,
                                                             ref StackCrawlMark stackMark)
        {
            if (securityEvidence != null)
                new SecurityPermission( SecurityPermissionFlag.ControlEvidence ).Demand();
            
	    Assembly result = null;
            AssemblyName an = new AssemblyName(partialName);
            try {
                result = nLoad(an, null, securityEvidence, null, ref stackMark, true, false);
            }
            catch(Exception e) {
                if (e.IsTransient)
                    throw e;

                if (IsSimplyNamed(an))
                    return null;
                
                AssemblyName GACAssembly = EnumerateCache(an);
                if(GACAssembly != null)
                    return InternalLoad(GACAssembly, securityEvidence,
                                        ref stackMark, false);
            }


            return result;
        }


        [ComVisible(false)]
        public virtual bool ReflectionOnly
        {
             get {
                 return nReflection();
             }
        }
         
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern bool nReflection();

        static private AssemblyName EnumerateCache(AssemblyName partialName)
        {
            new SecurityPermission(SecurityPermissionFlag.UnmanagedCode).Assert();

            partialName.Version = null;

            ArrayList a = new ArrayList();
            Fusion.ReadCache(a, partialName.FullName, ASM_CACHE.GAC);
            
            IEnumerator myEnum = a.GetEnumerator();
            AssemblyName ainfoBest = null;
            CultureInfo refCI = partialName.CultureInfo;

            while (myEnum.MoveNext()) {
                AssemblyName ainfo = new AssemblyName((String)myEnum.Current);

                if (CulturesEqual(refCI, ainfo.CultureInfo)) {
                    if (ainfoBest == null)
                        ainfoBest = ainfo;
                    else {
                        // Choose highest version
                        if (ainfo.Version > ainfoBest.Version)
                            ainfoBest = ainfo;
                    }
                }
            }

            return ainfoBest;
        }
        
        static private bool CulturesEqual(CultureInfo refCI, CultureInfo defCI)
        {
            bool defNoCulture = defCI.Equals(CultureInfo.InvariantCulture);

            // cultured asms aren't allowed to be bound to if
            // the ref doesn't ask for them specifically
            if ((refCI == null) || refCI.Equals(CultureInfo.InvariantCulture))
                return defNoCulture;

            if (defNoCulture || 
                ( !defCI.Equals(refCI) ))
                return false;

            return true;
        }

        static private bool IsSimplyNamed(AssemblyName partialName)
        {
            byte[] pk = partialName.GetPublicKeyToken();
            if ((pk != null) &&
                (pk.Length == 0))
                return true;

            pk = partialName.GetPublicKey();
            if ((pk != null) &&
                (pk.Length == 0))
                return true;

            return false;
        }        

        // Loads the assembly with a COFF based IMAGE containing
        // an emitted assembly. The assembly is loaded into the domain
        // of the caller.
        static public Assembly Load(byte[] rawAssembly)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return nLoadImage(rawAssembly,
                              null, // symbol store
                              null, // evidence
                              ref stackMark,
                              false  // fIntrospection
                              );
        }

        // Loads the assembly for reflection with a COFF based IMAGE containing
        // an emitted assembly. The assembly is loaded into the domain
        // of the caller.
        static public Assembly ReflectionOnlyLoad(byte[] rawAssembly)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return nLoadImage(rawAssembly,
                              null, // symbol store
                              null, // evidence
                              ref stackMark,
                              true  // fIntrospection
                              );
        }

        // Loads the assembly with a COFF based IMAGE containing
        // an emitted assembly. The assembly is loaded into the domain
        // of the caller. The second parameter is the raw bytes
        // representing the symbol store that matches the assembly.
        static public Assembly Load(byte[] rawAssembly,
                                    byte[] rawSymbolStore)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return nLoadImage(rawAssembly,
                              rawSymbolStore,
                              null, // evidence
                              ref stackMark,
                              false  // fIntrospection
                             );
        }

        [SecurityPermissionAttribute(SecurityAction.Demand, Flags=SecurityPermissionFlag.ControlEvidence)]
        static public Assembly Load(byte[] rawAssembly,
                                    byte[] rawSymbolStore,
                                    Evidence securityEvidence)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return nLoadImage(rawAssembly,
                              rawSymbolStore,
                              securityEvidence,
                              ref stackMark,
                              false  // fIntrospection
                             );
        }

        static public Assembly LoadFile(String path)
        {
            new FileIOPermission(FileIOPermissionAccess.PathDiscovery | FileIOPermissionAccess.Read, path).Demand();
            return nLoadFile(path, null);
        }

        [SecurityPermissionAttribute(SecurityAction.Demand, Flags=SecurityPermissionFlag.ControlEvidence)]
        static public Assembly LoadFile(String path,
                                        Evidence securityEvidence)
        {
            new FileIOPermission(FileIOPermissionAccess.PathDiscovery | FileIOPermissionAccess.Read, path).Demand();
            return nLoadFile(path, securityEvidence);
        }

        public Module LoadModule(String moduleName,
                                 byte[] rawModule)
        {
            return nLoadModule(moduleName,
                               rawModule,
                               null,
                               Evidence); // does a ControlEvidence demand
        }

        public Module LoadModule(String moduleName,
                                 byte[] rawModule,
                                 byte[] rawSymbolStore)
        {
            return nLoadModule(moduleName,
                               rawModule,
                               rawSymbolStore,
                               Evidence); // does a ControlEvidence demand
        }

        //
        // Locates a type from this assembly and creates an instance of it using
        // the system activator. 
        //
        public Object CreateInstance(String typeName)
        {
            return CreateInstance(typeName,
                                  false, // ignore case
                                  BindingFlags.Public | BindingFlags.Instance,
                                  null, // binder
                                  null, // args
                                  null, // culture
                                  null); // activation attributes
        }

        public Object CreateInstance(String typeName,
                                     bool ignoreCase)
        {
            return CreateInstance(typeName,
                                  ignoreCase,
                                  BindingFlags.Public | BindingFlags.Instance,
                                  null, // binder
                                  null, // args
                                  null, // culture
                                  null); // activation attributes
        }

        public Object CreateInstance(String typeName, 
                                     bool ignoreCase,
                                     BindingFlags bindingAttr, 
                                     Binder binder,
                                     Object[] args,
                                     CultureInfo culture,
                                     Object[] activationAttributes)
        {
            Type t = GetType(typeName, false, ignoreCase);
            if (t == null) return null;
            return Activator.CreateInstance(t,
                                            bindingAttr,
                                            binder,
                                            args,
                                            culture,
                                            activationAttributes);
        }
                                     
        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        public Module[] GetLoadedModules()
        {
            return nGetModules(false, false);
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        public Module[] GetLoadedModules(bool getResourceModules)
        {
            return nGetModules(false, getResourceModules);
        }
                                     
        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        public Module[] GetModules()
        {
            return nGetModules(true, false);
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        public Module[] GetModules(bool getResourceModules)
        {
            return nGetModules(true, getResourceModules);
        }

        // Returns the module in this assembly with name 'name'
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        public extern Module GetModule(String name);

        // Returns the file in the File table of the manifest that matches the
        // given name.  (Name should not include path.)
        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        public virtual FileStream GetFile(String name)
        {
            Module m = GetModule(name);
            if (m == null)
                return null;

            return new FileStream(m.InternalGetFullyQualifiedName(),
                                  FileMode.Open,
                                  FileAccess.Read, FileShare.Read);
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        public virtual FileStream[] GetFiles()
        {
            return GetFiles(false);
        }

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [ResourceConsumption(ResourceScope.Machine | ResourceScope.Assembly)]
        public virtual FileStream[] GetFiles(bool getResourceModules)
        {
            Module[] m = nGetModules(true, getResourceModules);
            int iLength = m.Length;
            FileStream[] fs = new FileStream[iLength];

            for(int i = 0; i < iLength; i++)
                fs[i] = new FileStream(m[i].InternalGetFullyQualifiedName(),
                                       FileMode.Open,
                                       FileAccess.Read, FileShare.Read);

            return fs;
        }

        // Returns the names of all the resources
        public virtual String[] GetManifestResourceNames()
        {
            return nGetManifestResourceNames();
        }
    
        // Returns the names of all the resources
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern String[] nGetManifestResourceNames();
        
        /*
         * Get the assembly that the current code is running from.
         */
        public static Assembly GetExecutingAssembly()
        {
                // passing address of local will also prevent inlining
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return nGetExecutingAssembly(ref stackMark);
        }
       
        public static Assembly GetCallingAssembly()
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCallersCaller;
            return nGetExecutingAssembly(ref stackMark);
        }
       
        public static Assembly GetEntryAssembly() {
            AppDomainManager domainManager = AppDomain.CurrentDomain.DomainManager;
            if (domainManager == null)
                domainManager = new AppDomainManager();
            return domainManager.EntryAssembly;
        }
    
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern AssemblyName[] GetReferencedAssemblies();


        public virtual ManifestResourceInfo GetManifestResourceInfo(String resourceName)
        {
            Assembly assemblyRef;
            String fileName;
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            int location = nGetManifestResourceInfo(resourceName,
                                                    out assemblyRef,
                                                    out fileName, ref stackMark);

            if (location == -1)
                return null;
            else
                return new ManifestResourceInfo(assemblyRef, fileName,
                                                (ResourceLocation) location);
        }
        
        public override String ToString()
        {
            String displayName = FullName; 
            if (displayName == null)
                return base.ToString();
            else
                return displayName;
        }

        public virtual String Location
        {
            get {
                String location = GetLocation();

                if (location != null)
                    new FileIOPermission( FileIOPermissionAccess.PathDiscovery, location ).Demand();

                return location;
            }
        }

        [ComVisible(false)]
        public virtual String ImageRuntimeVersion
        {
            get{
                return nGetImageRuntimeVersion();
            }
        }


        /*
          Returns true if the assembly was loaded from the global assembly cache.
        */
        
        public bool GlobalAssemblyCache
        {
            get {
                return nGlobalAssemblyCache();
            }
        }

        [ComVisible(false)]
        public Int64 HostContext
        {
            get {
                return GetHostContext();
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern Int64 GetHostContext();

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal static String VerifyCodeBase(String codebase)
        {
            if(codebase == null)
                return null;

            int len = codebase.Length;
            if (len == 0)
                return null;


            int j = codebase.IndexOf(':');
            // Check to see if the url has a prefix
            if( (j != -1) &&
                (j+2 < len) &&
                ((codebase[j+1] == '/') || (codebase[j+1] == '\\')) &&
                ((codebase[j+2] == '/') || (codebase[j+2] == '\\')) )
                return codebase;
#if !PLATFORM_UNIX
            else if ((len > 2) && (codebase[0] == '\\') && (codebase[1] == '\\'))
                return "file://" + codebase;
            else
                return "file:///" + Path.GetFullPathInternal( codebase );
#else
            else
                return "file://" + Path.GetFullPathInternal( codebase );
#endif // !PLATFORM_UNIX
        }

        internal virtual Stream GetManifestResourceStream(Type type, String name,
                                                          bool skipSecurityCheck, ref StackCrawlMark stackMark)
        {
            StringBuilder sb = new StringBuilder();
            if(type == null) {
                if (name == null)
                    throw new ArgumentNullException("type");
            }
            else {
                String nameSpace = type.Namespace;
                if(nameSpace != null) {
                    sb.Append(nameSpace);
                    if(name != null) 
                        sb.Append(Type.Delimiter);
                }
            }

            if(name != null)
                sb.Append(name);
    
            return GetManifestResourceStream(sb.ToString(), ref stackMark, skipSecurityCheck);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern Module nLoadModule(String moduleName,
                                          byte[] rawModule,
                                          byte[] rawSymbolStore,
                                          Evidence securityEvidence);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern bool nGlobalAssemblyCache();
                
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern String nGetImageRuntimeVersion();        

        internal Assembly()
        {
            m_assemblyData = null;
        }
    
        // Create a new module in which to emit code. This module will not contain the manifest.
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        static internal extern ModuleBuilder nDefineDynamicModule(Assembly containingAssembly, bool emitSymbolInfo, String filename, ref StackCrawlMark stackMark);
        
        // The following functions are native helpers for creating on-disk manifest
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern void nPrepareForSavingManifestToDisk(Module assemblyModule);  // module to contain assembly information if assembly is embedded
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern int nSaveToFileList(String strFileName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern int nSetHashValue(int tkFile, String strFullFileName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern int nSaveExportedType(String strComTypeName, int tkAssemblyRef, int tkTypeDef, TypeAttributes flags);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern void nSavePermissionRequests(byte[] required, byte[] optional, byte[] refused);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern void nSaveManifestToDisk(
            String strFileName, int entryPoint, int fileKind, 
            int portableExecutableKind, int ImageFileMachine);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern int nAddFileToInMemoryFileList(String strFileName, Module module);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern ModuleBuilder nGetOnDiskAssemblyModule();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern ModuleBuilder nGetInMemoryAssemblyModule();

    
        private static void DecodeSerializedEvidence( Evidence evidence,
                                               byte[] serializedEvidence )
        {
            BinaryFormatter formatter = new BinaryFormatter();
                
            Evidence asmEvidence = null;
                
            PermissionSet permSet = new PermissionSet( false );
            permSet.SetPermission( new SecurityPermission( SecurityPermissionFlag.SerializationFormatter ) );
            permSet.PermitOnly();
            permSet.Assert();

            try {
                using(MemoryStream ms = new MemoryStream( serializedEvidence ))
                    asmEvidence = (Evidence)formatter.Deserialize( ms );
            }
            catch
            {
            }
                
            if (asmEvidence != null)
            {
                IEnumerator enumerator = asmEvidence.GetAssemblyEnumerator();
                    
                while (enumerator.MoveNext())
                {
                    Object obj = enumerator.Current;
                    evidence.AddAssembly( obj );
                }
            }
        }       
         
        
        private static void AddStrongName( Evidence evidence, byte[] blob, String strSimpleName, int major, int minor, int build, int revision )
        {
            evidence.AddHost( new StrongName( new StrongNamePublicKeyBlob( blob ), strSimpleName, new Version( major, minor, build, revision ) ) );
        }
    
        private static Evidence CreateSecurityIdentity(Assembly asm,
                                                String strUrl,
                                                int zone,
                                                byte[] cert,
                                                byte[] publicKeyBlob,
                                                String strSimpleName,
                                                int major,
                                                int minor,
                                                int build,
                                                int revision,
                                                byte[] serializedEvidence,
                                                Evidence additionalEvidence)
        {
            Evidence evidence = new Evidence();

            if (zone != -1)
                evidence.AddHost( new Zone((SecurityZone)zone) );
            if (strUrl != null)
            {
                evidence.AddHost( new Url(strUrl, true) );

                // Only create a site piece of evidence if we are not loading from a file.
                if (String.Compare( strUrl, 0, s_localFilePrefix, 0, 5, StringComparison.OrdinalIgnoreCase) != 0)
                    evidence.AddHost( Site.CreateFromUrl( strUrl ) );
            }


            // Determine if it's in the GAC and add some evidence about it
            if(asm != null && System.Runtime.InteropServices.RuntimeEnvironment.FromGlobalAccessCache(asm))
                evidence.AddHost( new GacInstalled() );

            // This code was moved to a different function because:
            // 1) it is rarely called so we should only JIT it if we need it.
            // 2) it references lots of classes that otherwise aren't loaded.
            if (serializedEvidence != null)
                DecodeSerializedEvidence( evidence, serializedEvidence );

            if ((publicKeyBlob != null) &&
                (publicKeyBlob.Length != 0))
                AddStrongName( evidence, publicKeyBlob, strSimpleName, major, minor, build, revision );


            // If the host (caller of Assembly.Load) provided evidence, merge it
            // with the evidence we've just created. The host evidence takes
            // priority.
            if (additionalEvidence != null)
                evidence.MergeWithNoDuplicates(additionalEvidence);

            if (asm != null) {
                // The host might want to modify the evidence of the assembly through
                // the HostSecurityManager provided in AppDomainManager, so take that into account.
                HostSecurityManager securityManager = AppDomain.CurrentDomain.HostSecurityManager;
                if ((securityManager.Flags & HostSecurityManagerOptions.HostAssemblyEvidence) == HostSecurityManagerOptions.HostAssemblyEvidence)
                    return securityManager.ProvideAssemblyEvidence(asm, evidence);
            }

            return evidence;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static Assembly nGetExecutingAssembly(ref StackCrawlMark stackMark);

        internal unsafe virtual Stream GetManifestResourceStream(String name, ref StackCrawlMark stackMark, bool skipSecurityCheck)
        {
            ulong length = 0;
            byte* pbInMemoryResource = GetResource(name, out length, ref stackMark, skipSecurityCheck);

            if (pbInMemoryResource != null) {
                //Console.WriteLine("Creating an unmanaged memory stream of length "+length);
                if (length > Int64.MaxValue)
                    throw new NotImplementedException(Environment.GetResourceString("NotImplemented_ResourcesLongerThan2^63"));

                return new UnmanagedMemoryStream(pbInMemoryResource, (long)length, (long)length, FileAccess.Read, true);
            }

            //Console.WriteLine("GetManifestResourceStream: Blob "+name+" not found...");
            return null;
        }

        internal Version GetVersion()
        {
            int majorVer, minorVer, build, revision;
            nGetVersion(out majorVer, out minorVer, out build, out revision);
            return new Version (majorVer, minorVer, build, revision);
        }

        internal CultureInfo GetLocale()
        {
            String locale = nGetLocale();
            if (locale == null)
                return CultureInfo.InvariantCulture;

            return new CultureInfo(locale);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern String nGetLocale();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern void nGetVersion(out int majVer, out int minVer,
                                         out int buildNum, out int revNum);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern bool nIsDynamic();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern int nGetManifestResourceInfo(String resourceName,
                                                    out Assembly assemblyRef,
                                                    out String fileName,
                                                    ref StackCrawlMark stackMark);

        private void VerifyCodeBaseDiscovery(String codeBase)
        {                
            if ((codeBase != null) &&
                (String.Compare( codeBase, 0, s_localFilePrefix, 0, 5, StringComparison.OrdinalIgnoreCase) == 0)) {
                System.Security.Util.URLString urlString = new System.Security.Util.URLString( codeBase, true );
                new FileIOPermission( FileIOPermissionAccess.PathDiscovery, urlString.GetFileName() ).Demand();
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern String GetLocation();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern byte[] nGetPublicKey();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern String  nGetSimpleName();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern String nGetCodeBase(bool fCopiedName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern AssemblyHashAlgorithm nGetHashAlgorithm();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern AssemblyNameFlags nGetFlags();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern Evidence nGetEvidence();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern void nGetGrantSet(out PermissionSet newGrant, out PermissionSet newDenied);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern String GetFullName();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private unsafe extern void* _nGetEntryPoint();
        private unsafe RuntimeMethodHandle nGetEntryPoint() 
        {
            return new RuntimeMethodHandle(_nGetEntryPoint());
        }

        // GetResource will return a handle to a file (or -1) and set the length.
        // It will also return a pointer to the resources if they're in memory.
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private unsafe extern byte* GetResource(String resourceName, out ulong length,
                                                ref StackCrawlMark stackMark, 
                                                bool skipSecurityCheck);

        internal static Assembly InternalLoad(String assemblyString,
                                              Evidence assemblySecurity,
                                              ref StackCrawlMark stackMark,
                                              bool forIntrospection)
        {
            if (assemblyString == null)
                throw new ArgumentNullException("assemblyString");
            if ((assemblyString.Length == 0) ||
                (assemblyString[0] == '\0'))
                throw new ArgumentException(Environment.GetResourceString("Format_StringZeroLength"));

            AssemblyName an = new AssemblyName();
            Assembly assembly = null;

            an.Name = assemblyString;
            int hr = an.nInit(out assembly, forIntrospection, true);
            
            if (hr == System.__HResults.FUSION_E_INVALID_NAME) {
                return assembly;
            }
            else
                return InternalLoad(an, assemblySecurity, ref stackMark, forIntrospection);
        }

        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        internal static Assembly InternalLoad(AssemblyName assemblyRef, Evidence assemblySecurity,
                                              ref StackCrawlMark stackMark, bool forIntrospection)
        {
       
            if (assemblyRef == null)
                throw new ArgumentNullException("assemblyRef");

            assemblyRef = (AssemblyName)assemblyRef.Clone();
            if (assemblySecurity != null)
                new SecurityPermission( SecurityPermissionFlag.ControlEvidence ).Demand();

            String codeBase = VerifyCodeBase(assemblyRef.CodeBase);
            if (codeBase != null) {
                
                if (String.Compare( codeBase, 0, s_localFilePrefix, 0, 5, StringComparison.OrdinalIgnoreCase) != 0) {
                    IPermission perm = CreateWebPermission( assemblyRef.EscapedCodeBase );
                    perm.Demand();
                }
                else {
                    System.Security.Util.URLString urlString = new System.Security.Util.URLString( codeBase, true );
                    new FileIOPermission( FileIOPermissionAccess.PathDiscovery | FileIOPermissionAccess.Read , urlString.GetFileName() ).Demand();
                }   
            }

            return nLoad(assemblyRef, codeBase, assemblySecurity, null, ref stackMark, true, forIntrospection);
        }

        // demandFlag:
        // 0 demand PathDiscovery permission only
        // 1 demand Read permission only
        // 2 demand both Read and PathDiscovery
        // 3 demand Web permission only
        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
        private static void DemandPermission(String codeBase, bool havePath,
                                             int demandFlag)
        {
            FileIOPermissionAccess access = FileIOPermissionAccess.PathDiscovery;
            switch(demandFlag) {

            case 0: // default
                break;
            case 1:
                access = FileIOPermissionAccess.Read;
                break;
            case 2:
                access = FileIOPermissionAccess.PathDiscovery | FileIOPermissionAccess.Read;
                break;

            case 3:
                IPermission perm = CreateWebPermission(AssemblyName.EscapeCodeBase(codeBase));
                perm.Demand();
                return;
            }

            if (!havePath) {
                System.Security.Util.URLString urlString = new System.Security.Util.URLString( codeBase, true );
                codeBase = urlString.GetFileName();
            }

            codeBase = Path.GetFullPathInternal(codeBase);  // canonicalize

            new FileIOPermission(access, codeBase).Demand();
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern Assembly nLoad(AssemblyName fileName,
                                             String codeBase,
                                             Evidence assemblySecurity,
                                             Assembly locationHint,
                                             ref StackCrawlMark stackMark,
                                             bool throwOnFileNotFound,
                                             bool forIntrospection);

        private static IPermission CreateWebPermission( String codeBase )
        {
            BCLDebug.Assert( codeBase != null, "Must pass in a valid CodeBase" );
            Assembly sys = Assembly.Load("System, Version=" + ThisAssembly.Version + ", Culture=neutral, PublicKeyToken=" + AssemblyRef.EcmaPublicKeyToken);

            Type type = sys.GetType("System.Net.NetworkAccess", true);

            IPermission retval = null;
            if (!type.IsEnum || !type.IsVisible)
                goto Exit;

            Object[] webArgs = new Object[2];
            webArgs[0] = (Enum) Enum.Parse(type, "Connect", true);
            if (webArgs[0] == null)
                goto Exit;

            webArgs[1] = codeBase;

            type = sys.GetType("System.Net.WebPermission", true);

            if (!type.IsVisible)
                goto Exit;

            retval = (IPermission) Activator.CreateInstance(type, webArgs);

        Exit:
            if (retval == null) {
                BCLDebug.Assert( false, "Unable to create WebPermission" );
                throw new ExecutionEngineException();
            }

            return retval;            
        }

        
        private Module OnModuleResolveEvent(String moduleName)
        {
            ModuleResolveEventHandler moduleResolve =ModuleResolve;
            if (moduleResolve == null)
                return null;

            Delegate[] ds = moduleResolve.GetInvocationList();
            int len = ds.Length;
            for (int i = 0; i < len; i++) {
                Module ret = ((ModuleResolveEventHandler) ds[i])(this, new ResolveEventArgs(moduleName));
                if (ret != null)
                    return ret;              
            }

            return null;
        }
    
    
        internal Assembly InternalGetSatelliteAssembly(CultureInfo culture,
                                                       Version version,
                                                       bool throwOnFileNotFound)
        {
            if (culture == null)
                throw new ArgumentNullException("culture");
                
            AssemblyName an = new AssemblyName();

            an.SetPublicKey(nGetPublicKey());
            an.Flags = nGetFlags() | AssemblyNameFlags.PublicKey;

            if (version == null)
                an.Version = GetVersion();
            else
                an.Version = version;

            an.CultureInfo = culture;
            an.Name = nGetSimpleName() + ".resources";

            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            Assembly a = nLoad(an, null, null, this, ref stackMark, throwOnFileNotFound, false);
            if (a == this) {
                    throw new FileNotFoundException(String.Format(culture, Environment.GetResourceString("IO.FileNotFound_FileName"), an.Name));
            }

            return a;
        }

        internal InternalCache Cache {
            get {
                // This grabs an internal copy of m_cachedData and uses
                // that instead of looking at m_cachedData directly because
                // the cache may get cleared asynchronously.  This prevents
                // us from having to take a lock.
                InternalCache cache = m_cachedData;
                if (cache == null) {
                    cache = new InternalCache("Assembly");
                    m_cachedData = cache;
                    GC.ClearCache += new ClearCacheHandler(OnCacheClear);
                }
                return cache;
            } 
        }

        internal void OnCacheClear(Object sender, ClearCacheEventArgs cacheEventArgs)
        {
            m_cachedData = null;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        static internal extern Assembly nLoadFile(String path,
                                                  Evidence evidence);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        static internal extern Assembly nLoadImage(byte[] rawAssembly,
                                                   byte[] rawSymbolStore,
                                                   Evidence evidence,
                                                   ref StackCrawlMark stackMark,
                                                   bool fIntrospection);
    
                                     
        // Add an entry to assembly's manifestResource table for a stand alone resource.
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern void nAddStandAloneResource(String strName, 
                                                    String strFileName,
                                                    String strFullFileName,
                                                    int    attribute);

        [ResourceExposure(ResourceScope.Machine | ResourceScope.Assembly)]
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern Module[] nGetModules(bool loadIfNotFound,
                                             bool getResourceModules);
                         
        internal ProcessorArchitecture ComputeProcArchIndex()
        {
            PortableExecutableKinds pek;
            ImageFileMachine ifm;
        
            Module manifestModule = ManifestModule;
            if(manifestModule != null) {
		if(manifestModule.MDStreamVersion > 0x10000) {
		    ManifestModule.GetPEKind(out pek, out ifm);
		    if((pek & System.Reflection.PortableExecutableKinds.PE32Plus) == System.Reflection.PortableExecutableKinds.PE32Plus) {
			switch(ifm) {
			case System.Reflection.ImageFileMachine.IA64:
			    return ProcessorArchitecture.IA64;
			case System.Reflection.ImageFileMachine.AMD64:
			    return ProcessorArchitecture.Amd64;
			case System.Reflection.ImageFileMachine.I386:
			    if ((pek & System.Reflection.PortableExecutableKinds.ILOnly) == System.Reflection.PortableExecutableKinds.ILOnly)
				return ProcessorArchitecture.MSIL;
			break;
			}           
		    }
    
		    else {
			if(ifm == System.Reflection.ImageFileMachine.I386) {
			    if((pek & System.Reflection.PortableExecutableKinds.Required32Bit) == System.Reflection.PortableExecutableKinds.Required32Bit)
				return ProcessorArchitecture.X86;
	    
			    if((pek & System.Reflection.PortableExecutableKinds.ILOnly) == System.Reflection.PortableExecutableKinds.ILOnly)
				return ProcessorArchitecture.MSIL;
		
			    return ProcessorArchitecture.X86;
			}
		    }
		}
            }

            return ProcessorArchitecture.None;
        }

    }
}

