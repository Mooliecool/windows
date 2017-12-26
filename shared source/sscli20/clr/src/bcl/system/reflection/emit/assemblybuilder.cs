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
namespace System.Reflection.Emit
{  
    using System;
    using System.IO;
    using System.Diagnostics.SymbolStore;
    using System.Reflection;
    using System.Diagnostics;
    using System.Resources;    
    using System.Security.Permissions;
    using System.Runtime.Remoting.Activation;
    using CultureInfo = System.Globalization.CultureInfo;
    using System.Runtime.Serialization;
    using System.Security;
    using System.Threading;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;
    using System.Runtime.Versioning;

    // AssemblyBuilder class.
    // deliberately not [serializable]
    [HostProtection(MayLeakOnAbort = true)]
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_AssemblyBuilder))]
    [ComVisible(true)]
    public sealed class AssemblyBuilder : Assembly, _AssemblyBuilder
    {

        /**********************************************
        *
        * Defines a named dynamic module. It is an error to define multiple 
        * modules within an Assembly with the same name. This dynamic module is
        * a transient module.
        * 
        **********************************************/
        public ModuleBuilder DefineDynamicModule(
            String      name)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return DefineDynamicModuleInternal(name, false, ref stackMark);
        }

        public ModuleBuilder DefineDynamicModule(
            String      name,
            bool        emitSymbolInfo)         // specify if emit symbol info or not
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return DefineDynamicModuleInternal( name, emitSymbolInfo, ref stackMark );
        }

        internal ModuleBuilder DefineDynamicModuleInternal(
            String      name,
            bool        emitSymbolInfo,         // specify if emit symbol info or not
            ref StackCrawlMark stackMark)
        {
            BCLDebug.Log("DYNIL","## DYNIL LOGGING: AssemblyBuilder.DefineDynamicModule( " + name + " )");
            
            BCLDebug.Assert(m_assemblyData != null, "m_assemblyData is null in DefineDynamicModuleInternal");

            if (name == null)
                throw new ArgumentNullException("name");
            if (name.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "name");
           if (name[0] == '\0')
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidName"), "name");
             
            m_assemblyData.CheckNameConflict(name);
    
            // create the dynamic module
            ModuleBuilder   dynModule = nDefineDynamicModule(this, emitSymbolInfo, name, ref stackMark);
            ISymbolWriter   writer = null;

            if (emitSymbolInfo)
            {
                // create the default SymWriter
                Assembly assem = LoadISymWrapper();
                Type symWriter = assem.GetType("System.Diagnostics.SymbolStore.SymWriter", true, false);
                if (symWriter != null && !symWriter.IsVisible) 
                    symWriter = null;

                if (symWriter == null)
                {
                    // cannot find SymWriter
                    throw new ExecutionEngineException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString(ResId.MissingType), "SymWriter")); 
                }
                try {
                    (new PermissionSet(PermissionState.Unrestricted)).Assert();
                    writer = (ISymbolWriter) Activator.CreateInstance(symWriter);
                }
                finally {
                    CodeAccessPermission.RevertAssert();
                }
            }

            dynModule.Init(name, null, writer);
            m_assemblyData.AddModule(dynModule);
            return dynModule;
        }
        
        private Assembly LoadISymWrapper()
        {
            if (m_assemblyData.m_ISymWrapperAssembly != null)
                return m_assemblyData.m_ISymWrapperAssembly;

            Assembly assem = Assembly.Load("ISymWrapper, Version=" + ThisAssembly.Version +
                ", Culture=neutral, PublicKeyToken=" + AssemblyRef.MicrosoftPublicKeyToken);

            m_assemblyData.m_ISymWrapperAssembly = assem;
            return assem;
        }

        /**********************************************
        *
        * Defines a named dynamic module. It is an error to define multiple 
        * modules within an Assembly with the same name. No symbol information
        * will be emitted.
        * 
        **********************************************/
        public ModuleBuilder DefineDynamicModule(
            String      name, 
            String      fileName)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);

            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;

            // delegate to the next DefineDynamicModule 
            return DefineDynamicModuleInternal(name, fileName, false, ref stackMark); 
        }
    
        /**********************************************
        *
        * Emit symbol information if emitSymbolInfo is true using the
        * default symbol writer interface.
        * An exception will be thrown if the assembly is transient.
        *
        **********************************************/
        public ModuleBuilder DefineDynamicModule(
            String      name,                   // module name
            String      fileName,               // module file name
            bool        emitSymbolInfo)         // specify if emit symbol info or not
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            return DefineDynamicModuleInternal(name, fileName, emitSymbolInfo, ref stackMark); 
        }

        private ModuleBuilder DefineDynamicModuleInternal(
            String      name,                   // module name
            String      fileName,               // module file name
            bool        emitSymbolInfo,         // specify if emit symbol info or not
            ref StackCrawlMark stackMark)       // stack crawl mark used to find caller
        {
            if (m_assemblyData.m_isSynchronized)
            {
                lock(m_assemblyData)
                {
                    return DefineDynamicModuleInternalNoLock(name, fileName, emitSymbolInfo, ref stackMark);
                }
            }
            else
            {
                return DefineDynamicModuleInternalNoLock(name, fileName, emitSymbolInfo, ref stackMark);
            }
        }

        public override bool ReflectionOnly { get { return base.ReflectionOnly; } } 

        internal void CheckContext(params Type[][] typess)
        {
            if (typess == null)
                return;
            
            foreach(Type[] types in typess)
                if (types != null)
                    CheckContext(types);
        }

        internal void CheckContext(params Type[] types)
        {
            if (types == null)
                return;
        
            foreach (Type type in types)
            {
                if (type == null || type.Module.Assembly == typeof(object).Module.Assembly)
                    return;
                if (type.Module.Assembly.ReflectionOnly && !ReflectionOnly)
                    throw new InvalidOperationException(String.Format(
                        CultureInfo.CurrentCulture, Environment.GetResourceString("Arugment_EmitMixedContext1"), type.AssemblyQualifiedName));

                if (!type.Module.Assembly.ReflectionOnly && ReflectionOnly)
                    throw new InvalidOperationException(String.Format(
                        CultureInfo.CurrentCulture, Environment.GetResourceString("Arugment_EmitMixedContext2"), type.AssemblyQualifiedName));
            }
        }

        private ModuleBuilder DefineDynamicModuleInternalNoLock(
            String      name,                   // module name
            String      fileName,               // module file name
            bool        emitSymbolInfo,         // specify if emit symbol info or not
            ref StackCrawlMark stackMark)       // stack crawl mark used to find caller
        {
            BCLDebug.Log("DYNIL","## DYNIL LOGGING: AssemblyBuilder.DefineDynamicModule( " + name + ", " + fileName + ", " + emitSymbolInfo + " )");
            if (m_assemblyData.m_access == AssemblyBuilderAccess.Run)
            {
                // Error! You cannot define a persistable module within a transient data.
                throw new NotSupportedException(Environment.GetResourceString("Argument_BadPersistableModuleInTransientAssembly"));
            }
    
            if (m_assemblyData.m_isSaved == true)
            {
                // assembly has been saved before!
                throw new InvalidOperationException(Environment.GetResourceString(
                    "InvalidOperation_CannotAlterAssembly"));                          
            }
    
            if (name == null)
                throw new ArgumentNullException("name");
            if (name.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "name");
            if (name[0] == '\0')
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidName"), "name");
            
            if (fileName == null)
                throw new ArgumentNullException("fileName");
            if (fileName.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyFileName"), "fileName");
            if (!String.Equals(fileName, Path.GetFileName(fileName)))
                throw new ArgumentException(Environment.GetResourceString("Argument_NotSimpleFileName"), "fileName");
    
            m_assemblyData.CheckNameConflict(name);
            m_assemblyData.CheckFileNameConflict(fileName);
    
            // ecall to create the dynamic module
            ModuleBuilder   dynModule = nDefineDynamicModule(this, emitSymbolInfo, fileName, ref stackMark);
            ISymbolWriter      writer = null;
    
            if (emitSymbolInfo)
            {
                // create the default SymWriter
                Assembly assem = LoadISymWrapper();
                Type symWriter = assem.GetType("System.Diagnostics.SymbolStore.SymWriter", true, false);
                if (symWriter != null && !symWriter.IsVisible) 
                    symWriter = null;
                
                if (symWriter == null)
                {
                    // cannot find SymWriter
                    throw new ExecutionEngineException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("MissingType"), "SymWriter")); 
                }
                try {
                    (new PermissionSet(PermissionState.Unrestricted)).Assert();
                    writer = (ISymbolWriter) Activator.CreateInstance(symWriter);
                }
                finally {
                    CodeAccessPermission.RevertAssert();
                }
    
            }

            // initialize the dynamic module's managed side information
            dynModule.Init(name, fileName, writer);
            m_assemblyData.AddModule(dynModule);
            return dynModule;
        }
    
    
        /**********************************************
        *
        * Define stand alone managed resource for Assembly
        *
        **********************************************/
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public IResourceWriter DefineResource(
            String      name,
            String      description,
            String      fileName)
        {
            return DefineResource(name, description, fileName, ResourceAttributes.Public);
        }

        /**********************************************
        *
        * Define stand alone managed resource for Assembly
        *
        **********************************************/
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public IResourceWriter DefineResource(
            String      name,
            String      description,
            String      fileName,
            ResourceAttributes attribute)
        {
            if (m_assemblyData.m_isSynchronized)
            {
                lock(m_assemblyData)
                {
                    return DefineResourceNoLock(name, description, fileName, attribute);
                }
            }
            else
            {
                return DefineResourceNoLock(name, description, fileName, attribute);
            }
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        private IResourceWriter DefineResourceNoLock(
            String      name,
            String      description,
            String      fileName,
            ResourceAttributes attribute)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            BCLDebug.Log("DYNIL","## DYNIL LOGGING: AssemblyBuilder.DefineResource( " + name + ", " + fileName + ")");

            if (name == null)
                throw new ArgumentNullException("name");
            if (name.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), name);
            if (fileName == null)
                throw new ArgumentNullException("fileName");
            if (fileName.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyFileName"), "fileName");
            if (!String.Equals(fileName, Path.GetFileName(fileName)))
                throw new ArgumentException(Environment.GetResourceString("Argument_NotSimpleFileName"), "fileName");

            m_assemblyData.CheckResNameConflict(name);
            m_assemblyData.CheckFileNameConflict(fileName);

            ResourceWriter resWriter;
            String  fullFileName;

            if (m_assemblyData.m_strDir == null)
            {
                // If assembly directory is null, use current directory
                fullFileName = Path.Combine(Environment.CurrentDirectory, fileName);
                resWriter = new ResourceWriter(fullFileName);
            }
            else
            {
                // Form the full path given the directory provided by user
                fullFileName = Path.Combine(m_assemblyData.m_strDir, fileName);
                resWriter = new ResourceWriter(fullFileName);
            }
            // get the full path    
            fullFileName = Path.GetFullPath(fullFileName);                            
            
            // retrieve just the file name
            fileName = Path.GetFileName(fullFileName);
            
            m_assemblyData.AddResWriter( new ResWriterData( resWriter, null, name, fileName, fullFileName, attribute) );
            return resWriter;
        }


        /**********************************************
        *
        * Add an existing resource file to the Assembly
        *
        **********************************************/
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public void AddResourceFile(
            String      name,
            String      fileName)
        {
            AddResourceFile(name, fileName, ResourceAttributes.Public);
        }

        /**********************************************
        *
        * Add an existing resource file to the Assembly
        *
        **********************************************/
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public void AddResourceFile(
            String      name,
            String      fileName,
            ResourceAttributes attribute)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (m_assemblyData.m_isSynchronized)
            {
                lock(m_assemblyData)
                {
                    AddResourceFileNoLock(name, fileName, attribute);
                }
            }
            else
            {
                AddResourceFileNoLock(name, fileName, attribute);
            }
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        private void AddResourceFileNoLock(
            String      name,
            String      fileName,
            ResourceAttributes attribute)
        {
            BCLDebug.Log("DYNIL","## DYNIL LOGGING: AssemblyBuilder.AddResourceFile( " + name + ", " + fileName + ")");

            if (name == null)
                throw new ArgumentNullException("name");
            if (name.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), name);
            if (fileName == null)
                throw new ArgumentNullException("fileName");
            if (fileName.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyFileName"), fileName);
            if (!String.Equals(fileName, Path.GetFileName(fileName)))
                throw new ArgumentException(Environment.GetResourceString("Argument_NotSimpleFileName"), "fileName");

            m_assemblyData.CheckResNameConflict(name);
            m_assemblyData.CheckFileNameConflict(fileName);

            String  fullFileName;

            if (m_assemblyData.m_strDir == null)
            {
                // If assembly directory is null, use current directory
                fullFileName = Path.Combine(Environment.CurrentDirectory, fileName);
            }
            else
            {
                // Form the full path given the directory provided by user
                fullFileName = Path.Combine(m_assemblyData.m_strDir, fileName);
            }
            
            // get the full path    
            fullFileName = Path.GetFullPath(fullFileName);                            
            
            // retrieve just the file name
            fileName = Path.GetFileName(fullFileName);
            
            if (File.Exists(fullFileName) == false)
                throw new FileNotFoundException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString(
                    "IO.FileNotFound_FileName"),
                    fileName), fileName);
            m_assemblyData.AddResWriter( new ResWriterData( null, null, name, fileName, fullFileName, attribute) );
        }
        
        // Returns the names of all the resources
        public override String[] GetManifestResourceNames()
        {
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
        }
        
        public override FileStream GetFile(String name)
        {
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
        }
        
        public override FileStream[] GetFiles(bool getResourceModules)
        {
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
        }     
        
        public override Stream GetManifestResourceStream(Type type, String name)
        {
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
        }     
        
        public override Stream GetManifestResourceStream(String name)
        {
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
        }
                      
        public override ManifestResourceInfo GetManifestResourceInfo(String resourceName)
        {
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
        }        

        public override String Location
        {
            get {
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
            }
        }

        public override String ImageRuntimeVersion
        {
            get
            {
                return RuntimeEnvironment.GetSystemVersion();
            }
        }
        
        public override String CodeBase {
            get {
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
            }
        }
        
       
        

        
        /**********************************************
        *
        * return a dynamic module with the specified name.
        *
        **********************************************/
        public ModuleBuilder GetDynamicModule(
            String      name)                   // the name of module for the look up
        {
            if (m_assemblyData.m_isSynchronized)
            {
                lock(m_assemblyData)
                {
                    return GetDynamicModuleNoLock(name);
                }
            }
            else
            {
                return GetDynamicModuleNoLock(name);
            }
        }

        private ModuleBuilder GetDynamicModuleNoLock(
            String      name)                   // the name of module for the look up
        {
            BCLDebug.Log("DYNIL","## DYNIL LOGGING: AssemblyBuilder.GetDynamicModule( " + name + " )");
            if (name == null)
                throw new ArgumentNullException("name");
            if (name.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "name");

            int size = m_assemblyData.m_moduleBuilderList.Count;
            for (int i=0;i<size;i++) {
                ModuleBuilder moduleBuilder = (ModuleBuilder) m_assemblyData.m_moduleBuilderList[i];
                if (moduleBuilder.m_moduleData.m_strModuleName.Equals(name))
                {
                    return moduleBuilder;
                }
            }        
            return null;
        }
    
        /**********************************************
        *
        * Setting the entry point if the assembly builder is building
        * an exe.
        *
        **********************************************/
        public void SetEntryPoint(
            MethodInfo  entryMethod) 
        {
            SetEntryPoint(entryMethod, PEFileKinds.ConsoleApplication);
        }
        public void SetEntryPoint(
            MethodInfo  entryMethod,        // entry method for the assembly. We use this to determine the entry module
            PEFileKinds fileKind)           // file kind for the assembly.
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (m_assemblyData.m_isSynchronized)
            {
                lock(m_assemblyData)
                {
                    SetEntryPointNoLock(entryMethod, fileKind);
                }
            }
            else
            {
                SetEntryPointNoLock(entryMethod, fileKind);
            }
        }

        private void SetEntryPointNoLock(
            MethodInfo  entryMethod,        // entry method for the assembly. We use this to determine the entry module
            PEFileKinds fileKind)           // file kind for the assembly.
        {

            BCLDebug.Log("DYNIL","## DYNIL LOGGING: AssemblyBuilder.SetEntryPoint");
            if (entryMethod == null)
                throw new ArgumentNullException("entryMethod");

            //  
            //
            Module tmpModule = entryMethod.Module;
            if (!(tmpModule is ModuleBuilder && this.Equals(tmpModule.Assembly)))
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_EntryMethodNotDefinedInAssembly"));
            m_assemblyData.m_entryPointModule = (ModuleBuilder) tmpModule;
            m_assemblyData.m_entryPointMethod = entryMethod;
            m_assemblyData.m_peFileKind = fileKind;
            m_assemblyData.m_entryPointModule.SetEntryPoint(entryMethod);
        }


        // Override the EntryPoint method on Assembly.
        public override MethodInfo EntryPoint {
            get {return m_assemblyData.m_entryPointMethod;}
        }

        /**********************************************
        * Use this function if client decides to form the custom attribute blob themselves
        **********************************************/
[System.Runtime.InteropServices.ComVisible(true)]
        public void SetCustomAttribute(ConstructorInfo con, byte[] binaryAttribute)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (con == null)
                throw new ArgumentNullException("con");
            if (binaryAttribute == null)
                throw new ArgumentNullException("binaryAttribute");
            
            ModuleBuilder       inMemoryAssemblyModule;
            inMemoryAssemblyModule = m_assemblyData.GetInMemoryAssemblyModule();
            TypeBuilder.InternalCreateCustomAttribute(
                AssemblyBuilderData.m_tkAssembly,           // This is the AssemblyDef token
                inMemoryAssemblyModule.GetConstructorToken(con).Token,
                binaryAttribute,
                inMemoryAssemblyModule,                     // pass in the in-memory assembly module
                false,
                typeof(System.Diagnostics.DebuggableAttribute) == con.DeclaringType);

            // Track the CA for persistence
            if (m_assemblyData.m_access == AssemblyBuilderAccess.Run)
            {
                return;
            }

            // tracking the CAs for persistence
            m_assemblyData.AddCustomAttribute(con, binaryAttribute);
        }

        /**********************************************
        * Use this function if client wishes to build CustomAttribute using CustomAttributeBuilder
        **********************************************/
        public void SetCustomAttribute(CustomAttributeBuilder customBuilder)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (customBuilder == null)
            {
                throw new ArgumentNullException("customBuilder");
            }

            ModuleBuilder       inMemoryAssemblyModule;
            inMemoryAssemblyModule = m_assemblyData.GetInMemoryAssemblyModule();
            customBuilder.CreateCustomAttribute(
                inMemoryAssemblyModule, 
                AssemblyBuilderData.m_tkAssembly);          // This is the AssemblyDef token 

            // Track the CA for persistence
            if (m_assemblyData.m_access == AssemblyBuilderAccess.Run)
            {
                return;
            }
            m_assemblyData.AddCustomAttribute(customBuilder);
        }


        /**********************************************
        *
        * Saves the assembly to disk. Also saves all dynamic modules defined
        * in this dynamic assembly. Assembly file name can be the same as one of 
        * the module's name. If so, assembly info is stored within that module.
        * Assembly file name can be different from all of the modules underneath. In
        * this case, assembly is stored stand alone. 
        *
        **********************************************/

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public void Save(String assemblyFileName)       // assembly file name
        {
            Save(assemblyFileName, System.Reflection.PortableExecutableKinds.ILOnly, System.Reflection.ImageFileMachine.I386);
        }
            
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public void Save(String assemblyFileName, 
            PortableExecutableKinds portableExecutableKind, ImageFileMachine imageFileMachine)
        {
            if (m_assemblyData.m_isSynchronized)
            {
                lock(m_assemblyData)
                {
                    SaveNoLock(assemblyFileName, portableExecutableKind, imageFileMachine);
                }
            }
            else
            {
                SaveNoLock(assemblyFileName, portableExecutableKind, imageFileMachine);
            }
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        private void SaveNoLock(String assemblyFileName, 
            PortableExecutableKinds portableExecutableKind, ImageFileMachine imageFileMachine)
        {
            int         i;
            int         size;
            Type        type;
            TypeBuilder typeBuilder;
            ModuleBuilder modBuilder;
            String      strModFileName;
            ModuleBuilder assemblyModule;
            ResWriterData tempRes;
            int[]       tkAttrs = null;
            int[]       tkAttrs2 = null;
            ModuleBuilder onDiskAssemblyModule;

            BCLDebug.Log("DYNIL","## DYNIL LOGGING: AssemblyBuilder.Save( " + assemblyFileName + " )");

            String      tmpVersionFile = null;

            try 
            {
                if (m_assemblyData.m_iCABuilder != 0)
                    tkAttrs = new int[m_assemblyData.m_iCABuilder];
                if ( m_assemblyData.m_iCAs != 0)
                    tkAttrs2 = new int[m_assemblyData.m_iCAs];
    
                if (m_assemblyData.m_isSaved == true)
                {
                    // assembly has been saved before!
                    throw new InvalidOperationException(String.Format(CultureInfo.InvariantCulture,
                        Environment.GetResourceString(ResId.InvalidOperation_AssemblyHasBeenSaved),
                        nGetSimpleName()));
                }
    
                if ((m_assemblyData.m_access & AssemblyBuilderAccess.Save) != AssemblyBuilderAccess.Save)
                {
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CantSaveTransientAssembly"));
                }
    
                if (assemblyFileName == null)
                    throw new ArgumentNullException("assemblyFileName");
                if (assemblyFileName.Length == 0)
                    throw new ArgumentException(Environment.GetResourceString("Argument_EmptyFileName"), "assemblyFileName");
                if (!String.Equals(assemblyFileName, Path.GetFileName(assemblyFileName)))
                    throw new ArgumentException(Environment.GetResourceString("Argument_NotSimpleFileName"), "assemblyFileName");
    
                // Check if assembly info is supposed to be stored with one of the module files.    
                assemblyModule = m_assemblyData.FindModuleWithFileName(assemblyFileName);
    
                if (assemblyModule != null)
                {
                    m_assemblyData.SetOnDiskAssemblyModule(assemblyModule);
                }
    
                // If assembly is to be stored alone, then no file name should conflict with it.
                // This check will ensure resource file names are different assembly file name.
                //
                if (assemblyModule == null)
                {
                    m_assemblyData.CheckFileNameConflict(assemblyFileName);
    
                }
    
                if (m_assemblyData.m_strDir == null)
                {
                    // set it to current directory
                    m_assemblyData.m_strDir = Environment.CurrentDirectory;
                }
                else if (Directory.Exists(m_assemblyData.m_strDir) == false)
                {
                    throw new ArgumentException(String.Format(CultureInfo.InvariantCulture,
                        Environment.GetResourceString("Argument_InvalidDirectory"), 
                        m_assemblyData.m_strDir));
                }
    
                // after this point, assemblyFileName is the full path name.
                assemblyFileName = Path.Combine(m_assemblyData.m_strDir, assemblyFileName);
                assemblyFileName = Path.GetFullPath(assemblyFileName);
    
                // Check caller has the right to create the assembly file itself.
                new FileIOPermission(FileIOPermissionAccess.Write | FileIOPermissionAccess.Append, assemblyFileName).Demand();
    
                // 1. setup/create the IMetaDataAssemblyEmit for the on disk version
                if (assemblyModule != null)
                {
                    // prepare saving CAs on assembly def. We need to introduce the MemberRef for
                    // the CA's type first of all. This is for the case the we have embedded manifest.
                    // We need to introduce these MRs before we call PreSave where we will snap
                    // into a ondisk metadata. If we do it after this, the ondisk metadata will
                    // not contain the proper MRs.
                    //
                    for (i=0; i < m_assemblyData.m_iCABuilder; i++)
                    {
                        tkAttrs[i] = m_assemblyData.m_CABuilders[i].PrepareCreateCustomAttributeToDisk(
                            assemblyModule); 
                    }
                    for (i=0; i < m_assemblyData.m_iCAs; i++)
                    {
                        tkAttrs2[i] = assemblyModule.InternalGetConstructorToken(m_assemblyData.m_CACons[i], true).Token;
                    }
                    assemblyModule.PreSave(assemblyFileName, portableExecutableKind, imageFileMachine);
                }
                nPrepareForSavingManifestToDisk(assemblyModule);
    
                // This function will return the embedded manifest module, an already exposed ModuleBuilder
                // created by user, or make the stand alone manifest module exposed through managed code.
                //
                onDiskAssemblyModule = m_assemblyData.GetOnDiskAssemblyModule();
    
    
                if (assemblyModule == null)
                {
                
                    // This is for introducing the MRs for CA's type. This case is for stand alone
                    // manifest. We need to wait till nPrepareForSavingManifestToDisk is called. 
                    // That will trigger the creation of the on-disk stand alone manifest module.
                    //
                    for (i=0; i < m_assemblyData.m_iCABuilder; i++)
                    {
                        tkAttrs[i] = m_assemblyData.m_CABuilders[i].PrepareCreateCustomAttributeToDisk(
                            onDiskAssemblyModule); 
                    }
                    for (i=0; i < m_assemblyData.m_iCAs; i++)
                    {
                        tkAttrs2[i] = onDiskAssemblyModule.InternalGetConstructorToken(m_assemblyData.m_CACons[i], true).Token;
                    }
                }
            
                // 2. save all of the persistable modules contained by this AssemblyBuilder except the module that is going to contain
                // Assembly information
                // 
                // 3. create the file list in the manifest and track the file token. If it is embedded assembly,
                // the assembly file should not be on the file list.
                // 
                size = m_assemblyData.m_moduleBuilderList.Count;
                for (i = 0; i < size; i++) 
                {
                    ModuleBuilder mBuilder = (ModuleBuilder) m_assemblyData.m_moduleBuilderList[i];
                    if (mBuilder.IsTransient() == false && mBuilder != assemblyModule)
                    {
                        strModFileName = mBuilder.m_moduleData.m_strFileName;              
                        if (m_assemblyData.m_strDir != null)
                        {
                            strModFileName = Path.Combine(m_assemblyData.m_strDir, strModFileName);
                            strModFileName = Path.GetFullPath(strModFileName);                            
                        }
                        
                        // Check caller has the right to create the Module file itself.
                        new FileIOPermission(FileIOPermissionAccess.Write | FileIOPermissionAccess.Append, strModFileName).Demand();
                        
                        mBuilder.m_moduleData.m_tkFile = nSaveToFileList(mBuilder.m_moduleData.m_strFileName);
                        mBuilder.PreSave(strModFileName, portableExecutableKind, imageFileMachine);
                        mBuilder.Save(strModFileName, false, portableExecutableKind, imageFileMachine);
    
                        // Cannot set the hash value when creating the file since the file token
                        // is needed to created the entries for the embedded resources in the
                        // module and the resources need to be there before you figure the hash.
                        nSetHashValue(mBuilder.m_moduleData.m_tkFile, strModFileName);
                    }
                }
        
                // 4. Add the public ComType
                for (i=0; i < m_assemblyData.m_iPublicComTypeCount; i++)
                {   
                    type = m_assemblyData.m_publicComTypeList[i];
                    if (type is System.RuntimeType)
                    {
                        modBuilder = m_assemblyData.FindModuleWithName(type.Module.m_moduleData.m_strModuleName);
                        if (modBuilder != assemblyModule)
                            DefineNestedComType(type, modBuilder.m_moduleData.m_tkFile, type.MetadataTokenInternal);
                    }
                    else
                    {
                        // Could assert that "type" is a TypeBuilder, but next statement throws if it isn't.
                        typeBuilder = (TypeBuilder) type;
                        modBuilder = (ModuleBuilder) type.Module;
                        if (modBuilder != assemblyModule)
                            DefineNestedComType(type, modBuilder.m_moduleData.m_tkFile, typeBuilder.MetadataTokenInternal);
                    }
                }
    
                // 5. write AssemblyDef's CAs
                for (i=0; i < m_assemblyData.m_iCABuilder; i++)
                {
                    m_assemblyData.m_CABuilders[i].CreateCustomAttribute(
                        onDiskAssemblyModule, 
                        AssemblyBuilderData.m_tkAssembly,           // This is the AssemblyDef token 
                        tkAttrs[i], true);
                }
    
                for (i=0; i < m_assemblyData.m_iCAs; i++)
                {
                    TypeBuilder.InternalCreateCustomAttribute(
                        AssemblyBuilderData.m_tkAssembly,           // This is the AssemblyDef token
                        tkAttrs2[i],
                        m_assemblyData.m_CABytes[i],
                        onDiskAssemblyModule,                       // pass in the in-memory assembly module
                        true);
                }
    
                // 6. write security permission requests to the manifest.
                if (m_assemblyData.m_RequiredPset  != null || m_assemblyData.m_OptionalPset != null || m_assemblyData.m_RefusedPset != null)
                {
                    // Translate sets into internal encoding (uses standard binary serialization).
                    byte[] required = null;
                    byte[] optional = null;
                    byte[] refused = null;
                    if (m_assemblyData.m_RequiredPset != null)
                        required = m_assemblyData.m_RequiredPset.EncodeXml();
                    if (m_assemblyData.m_OptionalPset != null)
                        optional = m_assemblyData.m_OptionalPset.EncodeXml();
                    if (m_assemblyData.m_RefusedPset != null)
                        refused = m_assemblyData.m_RefusedPset.EncodeXml();
                    nSavePermissionRequests(required, optional, refused);
                }
    
                // 7. Save the stand alone managed resources
                size = m_assemblyData.m_resWriterList.Count;
                for ( i = 0; i < size; i++ )
                {
                    tempRes = null;
    
                    try
                    {
                        tempRes = (ResWriterData)m_assemblyData.m_resWriterList[i];
    
                        // If the user added an existing resource to the manifest, the
                        // corresponding ResourceWriter will be null.
                        if (tempRes.m_resWriter != null)
                            // Check caller has the right to create the Resource file itself.
                            new FileIOPermission(FileIOPermissionAccess.Write | FileIOPermissionAccess.Append, tempRes.m_strFullFileName).Demand();
                    }
                    finally
                    {
                        if (tempRes != null && tempRes.m_resWriter != null)
                            tempRes.m_resWriter.Close();
                    }
    
                    // Add entry to manifest for this stand alone resource
                    nAddStandAloneResource(tempRes.m_strName, tempRes.m_strFileName, tempRes.m_strFullFileName, (int)tempRes.m_attribute);
                }
    
                // Save now!!
                if (assemblyModule == null)
                {
                    
    
                    // Stand alone manifest
                    if (m_assemblyData.m_entryPointModule != null)
                    {
                        nSaveManifestToDisk(assemblyFileName, m_assemblyData.m_entryPointModule.m_moduleData.m_tkFile, (int)m_assemblyData.m_peFileKind, 
                            (int)portableExecutableKind, (int)imageFileMachine);
                    }
                    else
                    {
                        nSaveManifestToDisk(assemblyFileName, 0, (int)m_assemblyData.m_peFileKind, 
                            (int)portableExecutableKind, (int)imageFileMachine);
                    }
                }
                else
                {
                    // embedded manifest
                    
                    // If the module containing the entry point is not the manifest file, we need to
                    // let the manifest file point to the module which contains the entry point.
                    //  
                    //  
                    //  
                    // 
                    if (m_assemblyData.m_entryPointModule != null && m_assemblyData.m_entryPointModule != assemblyModule)
                        assemblyModule.m_EntryPoint = new MethodToken(m_assemblyData.m_entryPointModule.m_moduleData.m_tkFile);
                    assemblyModule.Save(assemblyFileName, true, portableExecutableKind, imageFileMachine);
                }    
                m_assemblyData.m_isSaved = true;
            }
            finally
            {
                if (tmpVersionFile != null)
                {
                    // Delete file.
                    System.IO.File.Delete(tmpVersionFile);
                }
            }
        }
    

        
        // Get an array of all the public types defined in this assembly
        public override Type[] GetExportedTypes()
        {
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicAssembly"));
        }
    
        internal bool IsPersistable()
        {
            if ((m_assemblyData.m_access & AssemblyBuilderAccess.Save) == AssemblyBuilderAccess.Save)
                return true;
            else
                return false;
        }
    
        /**********************************************
        *
        * Internal helper to walk the nested type hierachy
        *
        **********************************************/
        private int DefineNestedComType(Type type, int tkResolutionScope, int tkTypeDef)
        {
            Type        enclosingType = type.DeclaringType;
            if (enclosingType == null)
            {
                return nSaveExportedType(type.FullName, tkResolutionScope, tkTypeDef, type.Attributes);
            }
            else
            {
                tkResolutionScope = DefineNestedComType(enclosingType, tkResolutionScope, tkTypeDef);
            }
            return nSaveExportedType(type.FullName, tkResolutionScope, tkTypeDef, type.Attributes);
        }

        /**********************************************
         * 
         * Private methods
         * 
         **********************************************/
    
        /**********************************************
         * Make a private constructor so these cannot be constructed externally.
         * @internonly
         **********************************************/
        private AssemblyBuilder() {}

        void _AssemblyBuilder.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _AssemblyBuilder.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _AssemblyBuilder.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _AssemblyBuilder.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }
    }
}

