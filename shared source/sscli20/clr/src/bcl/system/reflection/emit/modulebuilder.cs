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
    using System.Runtime.InteropServices;
    using System;
    using IList = System.Collections.IList;
    using ArrayList = System.Collections.ArrayList;
    using CultureInfo = System.Globalization.CultureInfo;
    using ResourceWriter = System.Resources.ResourceWriter;
    using IResourceWriter = System.Resources.IResourceWriter;
    using System.Diagnostics.SymbolStore;
    using System.Reflection;
    using System.Diagnostics;
    using System.IO;
    using System.Security;
    using System.Security.Permissions;
    using System.Runtime.Serialization;
    using System.Text;
    using System.Threading;
    using System.Runtime.Versioning;

    // deliberately not [serializable]
    [HostProtection(MayLeakOnAbort = true)]
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_ModuleBuilder))]
[System.Runtime.InteropServices.ComVisible(true)]
    public class ModuleBuilder : Module, _ModuleBuilder
    {
        #region Internal Static Members
        static internal String UnmangleTypeName(String typeName)
        {
            // Gets the original type name, without '+' name mangling.

            int i = typeName.Length - 1;
            while (true)
            {
                i = typeName.LastIndexOf('+', i);
                if (i == -1)
                    break;

                bool evenSlashes = true;
                int iSlash = i;
                while (typeName[--iSlash] == '\\')
                    evenSlashes = !evenSlashes;

                // Even number of slashes means this '+' is a name separator
                if (evenSlashes)
                    break;

                i = iSlash;
            }

            return typeName.Substring(i + 1);
        }

        #endregion

        #region Private Data Members
        // WARNING!! WARNING!!
        // ModuleBuilder should not contain any data members as its reflectbase is the same as Module.
        #endregion

        #region Constructor
        private ModuleBuilder()
        {
        }

        #endregion

        #region Private Members
        private Type GetType(String strFormat, Type baseType)
        {
            // This function takes a string to describe the compound type, such as "[,][]", and a baseType.

            if (strFormat == null || strFormat.Equals(String.Empty))
            {
                return baseType;
            }

            // convert the format string to byte array and then call FormCompoundType
            char[]      bFormat = strFormat.ToCharArray();
            return SymbolType.FormCompoundType(bFormat, baseType, 0);

        }
        
        
        internal void CheckContext(params Type[][] typess)
        {
            ((AssemblyBuilder)Assembly).CheckContext(typess);
        }
        internal void CheckContext(params Type[] types)
        {
            ((AssemblyBuilder)Assembly).CheckContext(types);
        }
        #endregion

        #region Internal Members
        internal virtual Type FindTypeBuilderWithName(String strTypeName, bool ignoreCase)
        {
            int         size = m_TypeBuilderList.Count;
            int         i;
            Type        typeTemp = null;

            for (i = 0; i < size; i++) 
            {
                typeTemp = (Type) m_TypeBuilderList[i];
                if (ignoreCase == true)
                {
                    if (String.Compare(typeTemp.FullName, strTypeName, ((ignoreCase) ? StringComparison.OrdinalIgnoreCase : StringComparison.Ordinal)) == 0)
                        break;                    
                }
                else
                {
                    if (typeTemp.FullName.Equals( strTypeName))
                        break;
                }
            } 
            if (i == size)
                typeTemp = null;

            return typeTemp;
        }
        
        internal Type GetRootElementType(Type type)
        {
            // This function will walk compound type to the inner most BaseType. Such as returning int for "ptr[] int".
            if (type.IsByRef == false && type.IsPointer == false && type.IsArray == false)
                return type;

            return GetRootElementType(type.GetElementType());
        }

        internal void SetEntryPoint(MethodInfo entryPoint)
        {           
            // Sets the entry point of the module to be a given method.  If no entry point
            // is specified, calling EmitPEFile will generate a dll.
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            m_EntryPoint = GetMethodToken(entryPoint);
        }


        internal void PreSave(String fileName, 
            PortableExecutableKinds portableExecutableKind, ImageFileMachine imageFileMachine)
        {
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    PreSaveNoLock(fileName, portableExecutableKind, imageFileMachine);
                }
            }
            else
            {
                PreSaveNoLock(fileName, portableExecutableKind, imageFileMachine);
            }
        }

        private void PreSaveNoLock(String fileName, 
            PortableExecutableKinds portableExecutableKind, ImageFileMachine imageFileMachine)
        {
            // This is a helper called by AssemblyBuilder save to presave information for the persistable modules.
            Object      item;
            TypeBuilder typeBuilder;
            if (m_moduleData.m_isSaved == true)
            {
                // can only save once
                throw new InvalidOperationException(String.Format(CultureInfo.InvariantCulture,
                    Environment.GetResourceString("InvalidOperation_ModuleHasBeenSaved"),
                    m_moduleData.m_strModuleName));
            }
        
            if (m_moduleData.m_fGlobalBeenCreated == false && m_moduleData.m_fHasGlobal == true)
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_GlobalFunctionNotBaked")); 

            int size = m_TypeBuilderList.Count;
            for (int i=0; i<size; i++) 
            {
                item = m_TypeBuilderList[i];
                if (item is TypeBuilder)
                {
                    typeBuilder = (TypeBuilder) item;
                }
                else
                {
                    EnumBuilder enumBuilder = (EnumBuilder) item;
                    typeBuilder = enumBuilder.m_typeBuilder;
                }
                if (typeBuilder.m_hasBeenCreated == false && typeBuilder.m_isHiddenType == false)
                {
                    // cannot save to PE file without creating all of the types first 
                    throw new NotSupportedException(String.Format(CultureInfo.InvariantCulture,
                        Environment.GetResourceString("NotSupported_NotAllTypesAreBaked"), 
                        typeBuilder.FullName)); 
                }
            }

            InternalPreSavePEFile((int)portableExecutableKind, (int)imageFileMachine);
        }
        
        internal void Save(String fileName, bool isAssemblyFile, PortableExecutableKinds portableExecutableKind, 
            ImageFileMachine imageFileMachine)
        {
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    SaveNoLock(fileName, isAssemblyFile, portableExecutableKind, imageFileMachine);
                }
            }
            else
            {
                SaveNoLock(fileName, isAssemblyFile, portableExecutableKind, imageFileMachine);
            }
        }

        private void SaveNoLock(String fileName, bool isAssemblyFile, PortableExecutableKinds portableExecutableKind, 
            ImageFileMachine imageFileMachine)
        {
            // This is a helper called by AssemblyBuilder save to save information for the persistable modules.
            if (m_moduleData.m_embeddedRes != null)
            {
                // There are embedded resources for this module
                ResWriterData   resWriter;
                int             iCount;

                // Set the number of resources embedded in this PE
                for (resWriter = m_moduleData.m_embeddedRes, iCount = 0; resWriter != null; resWriter = resWriter.m_nextResWriter, iCount++);
                InternalSetResourceCounts(iCount);

                // Add each resource content into the to be saved PE file
                for (resWriter = m_moduleData.m_embeddedRes; resWriter != null; resWriter = resWriter.m_nextResWriter)
                {
                    if (resWriter.m_resWriter != null)
                        resWriter.m_resWriter.Generate();                    
                    
                    byte[] resBytes = new byte[resWriter.m_memoryStream.Length];
                    resWriter.m_memoryStream.Flush();
                    resWriter.m_memoryStream.Position = 0;
                    resWriter.m_memoryStream.Read(resBytes, 0, resBytes.Length);
                    InternalAddResource(resWriter.m_strName, 
                                        resBytes,
                                        resBytes.Length,
                                        m_moduleData.m_tkFile, (int)resWriter.m_attribute, 
                                        (int)portableExecutableKind, (int)imageFileMachine);
                }
            }


            if (isAssemblyFile)
                InternalSavePEFile(fileName, m_EntryPoint, (int)Assembly.m_assemblyData.m_peFileKind, true); 
            else
                InternalSavePEFile(fileName, m_EntryPoint, (int)PEFileKinds.Dll, false); 
            m_moduleData.m_isSaved = true;
        }
    
        internal int GetTypeRefNested(Type type, Module refedModule, String strRefedModuleFileName)
        {
            // This function will generate correct TypeRef token for top level type and nested type.

            Type enclosingType = type.DeclaringType;
            int tkResolution = 0;
            String typeName = type.FullName;

            if (enclosingType != null)
            {
                tkResolution = GetTypeRefNested(enclosingType, refedModule, strRefedModuleFileName);
                typeName = UnmangleTypeName(typeName);
            }

            return InternalGetTypeToken(typeName, refedModule, strRefedModuleFileName, tkResolution);
        }

        internal MethodToken InternalGetConstructorToken(ConstructorInfo con, bool usingRef)
        {
            // Helper to get constructor token. If usingRef is true, we will never use the def token
 
            
            int tr;
            int mr = 0;
            
            if (con == null)
                throw new ArgumentNullException("con");

            if (con is ConstructorBuilder)
            {
                ConstructorBuilder conBuilder = con as ConstructorBuilder;
                if (usingRef == false && conBuilder.ReflectedType.Module.Equals(this))
                    return conBuilder.GetToken();

                // constructor is defined in a different module
                tr = GetTypeToken(con.ReflectedType).Token;
                mr = InternalGetMemberRef(con.ReflectedType.Module, tr, conBuilder.GetToken().Token);
            }
            else if (con is ConstructorOnTypeBuilderInstantiation)
            {
                ConstructorOnTypeBuilderInstantiation conBuilder = con as ConstructorOnTypeBuilderInstantiation;
                if (usingRef == true) throw new InvalidOperationException();

                tr = GetTypeToken(con.DeclaringType).Token;
                mr = InternalGetMemberRef(con.DeclaringType.Module, tr, conBuilder.m_ctor.MetadataTokenInternal);
            }
            else if (con is RuntimeConstructorInfo && con.ReflectedType.IsArray == false)
            {
                // constructor is not a dynamic field
                // We need to get the TypeRef tokens

                tr = GetTypeToken(con.ReflectedType).Token;
                mr = InternalGetMemberRefOfMethodInfo(tr, con.GetMethodHandle());
            }
            else
            {
                // some user derived ConstructorInfo
                // go through the slower code path, i.e. retrieve parameters and form signature helper.
                ParameterInfo[] parameters = con.GetParameters();
                Type[] parameterTypes = new Type[parameters.Length];
                Type[][] requiredCustomModifiers = new Type[parameterTypes.Length][];
                Type[][] optionalCustomModifiers = new Type[parameterTypes.Length][];

                for (int i = 0; i < parameters.Length; i++)
                {
                    parameterTypes[i] = parameters[i].ParameterType;
                    requiredCustomModifiers[i] = parameters[i].GetRequiredCustomModifiers();
                    optionalCustomModifiers[i] = parameters[i].GetOptionalCustomModifiers();
                }

                tr = GetTypeToken(con.ReflectedType).Token;

                SignatureHelper sigHelp = SignatureHelper.GetMethodSigHelper(this, con.CallingConvention, null, null, null, parameterTypes, requiredCustomModifiers, optionalCustomModifiers);
                int length;
                byte[] sigBytes = sigHelp.InternalGetSignature(out length);

                mr = InternalGetMemberRefFromSignature(tr, con.Name, sigBytes, length);
            }
            
            return new MethodToken( mr );
        }
    
        internal void Init(String strModuleName, String strFileName, ISymbolWriter writer)
        {
            m_moduleData = new ModuleBuilderData(this, strModuleName, strFileName);
            m_TypeBuilderList = new ArrayList();    
            m_iSymWriter = writer;

            if (writer != null)
            {
                // Set the underlying writer for the managed writer
                // that we're using.  Note that this function requires
                // unmanaged code access.
                new SecurityPermission(SecurityPermissionFlag.UnmanagedCode).Assert();
                writer.SetUnderlyingWriter(m_pInternalSymWriter);
            }
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal void DefineUnmanagedResourceFileInternal(String resourceFileName)
        {
            // Shouldn't have resource bytes, but a previous file is OK, because assemblyBuilder.Save
            //  creates then deletes a temp file.
            if (m_moduleData.m_resourceBytes != null)
                throw new ArgumentException(Environment.GetResourceString("Argument_NativeResourceAlreadyDefined"));
            
            if (resourceFileName == null)            
                throw new ArgumentNullException("resourceFileName");
            
            if (m_moduleData.m_fHasExplicitUnmanagedResource)
                throw new ArgumentException(Environment.GetResourceString("Argument_NativeResourceAlreadyDefined"));
                                                               
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            
            // Check caller has the right to read the file.
            string strFullFileName;
            strFullFileName = Path.GetFullPath(resourceFileName);
            new FileIOPermission(FileIOPermissionAccess.Read, strFullFileName).Demand();
            
            new EnvironmentPermission(PermissionState.Unrestricted).Assert();
            try
            {
                if (File.Exists(resourceFileName) == false)
                    throw new FileNotFoundException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString(
                        "IO.FileNotFound_FileName"),
                        resourceFileName), resourceFileName);
            }
            finally
            {
                CodeAccessPermission.RevertAssert();
            }
                                                    
            m_moduleData.m_strResourceFileName = strFullFileName;
        }
        
        #endregion

        #region Module Overrides
        public override Type[] GetTypes()
        {
            int size = m_TypeBuilderList.Count;
            Type[] moduleTypes = new Type[size];
            TypeBuilder tmpTypeBldr;
            int filtered = 0;

            for (int i = 0; i < size; i++)
            {
                EnumBuilder enumBldr = m_TypeBuilderList[i] as EnumBuilder;
                tmpTypeBldr = m_TypeBuilderList[i] as TypeBuilder;
                if (enumBldr != null)
                    tmpTypeBldr = enumBldr.m_typeBuilder;
                    
                if (tmpTypeBldr != null)
                {
                    if (tmpTypeBldr.m_hasBeenCreated)
                        moduleTypes[i] = tmpTypeBldr.UnderlyingSystemType;
                    else
                        moduleTypes[i] = tmpTypeBldr;
                }
                else
                {
                    // RuntimeType case: This will happen in TlbImp.
                    moduleTypes[i] =  (Type) m_TypeBuilderList[i];
                }
            }

            if (filtered > 0)
            {
                Type[] filteredTypes = new Type[size - filtered];
                int src, dst;
                for (src = 0, dst = 0; src < size; src++)
                {
                    if (moduleTypes[src] != null)
                    {
                        filteredTypes[dst] = moduleTypes[src];
                        dst++;
                    }
                }
                moduleTypes = filteredTypes;
            }

            return moduleTypes;
        }

[System.Runtime.InteropServices.ComVisible(true)]
        public override Type GetType(String className)
        {
            return GetType(className, false, false);
        }
        
[System.Runtime.InteropServices.ComVisible(true)]
        public override Type GetType(String className, bool ignoreCase)
        {
            return GetType(className, false, ignoreCase);
        }
        
[System.Runtime.InteropServices.ComVisible(true)]
        public override Type GetType(String className, bool throwOnError, bool ignoreCase)
        {
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return GetTypeNoLock(className, throwOnError, ignoreCase);
                }
            }
            else
            {
                return GetTypeNoLock(className, throwOnError, ignoreCase);
            }
        }

        private Type GetTypeNoLock(String className, bool throwOnError, bool ignoreCase)
        {
            // public API to to a type. The reason that we need this function override from module
            // is because clients might need to get foo[] when foo is being built. For example, if 
            // foo class contains a data member of type foo[].
            // This API first delegate to the Module.GetType implementation. If succeeded, great! 
            // If not, we have to look up the current module to find the TypeBuilder to represent the base
            // type and form the Type object for "foo[,]".
                
            // Module.GetType() will verify className.                
            Type baseType = base.GetType(className, throwOnError, ignoreCase);
            if (baseType != null)
                return baseType;

            // Now try to see if we contain a TypeBuilder for this type or not.
            // Might have a compound type name, indicated via an unescaped
            // '[', '*' or '&'. Split the name at this point.
            String baseName = null;
            String parameters = null;
            int startIndex = 0;

            while (startIndex <= className.Length)
            {
                // Are there any possible special characters left?
                int i = className.IndexOfAny(new char[]{'[', '*', '&'}, startIndex);
                if (i == -1)
                {
                    // No, type name is simple.
                    baseName = className;
                    parameters = null;
                    break;
                }

                // Found a potential special character, but it might be escaped.
                int slashes = 0;
                for (int j = i - 1; j >= 0 && className[j] == '\\'; j--)
                    slashes++;

                // Odd number of slashes indicates escaping.
                if (slashes % 2 == 1)
                {
                    startIndex = i + 1;
                    continue;
                }

                // Found the end of the base type name.
                baseName = className.Substring(0, i);
                parameters = className.Substring(i);
                break;
            }

            // If we didn't find a basename yet, the entire class name is
            // the base name and we don't have a composite type.
            if (baseName == null)
            {
                baseName = className;
                parameters = null;
            }

            baseName = baseName.Replace(@"\\",@"\").Replace(@"\[",@"[").Replace(@"\*",@"*").Replace(@"\&",@"&");

            if (parameters != null)
            {
                // try to see if reflection can find the base type. It can be such that reflection
                // does not support the complex format string yet!

                baseType = base.GetType(baseName, false, ignoreCase);
            }

            if (baseType == null)
            {
                // try to find it among the unbaked types.
                // starting with the current module first of all.
                baseType = FindTypeBuilderWithName(baseName, ignoreCase);
                if (baseType == null && Assembly is AssemblyBuilder)
                {
                    // now goto Assembly level to find the type.
                    int         size;
                    ArrayList   modList;

                    modList = Assembly.m_assemblyData.m_moduleBuilderList;
                    size = modList.Count;
                    for (int i = 0; i < size && baseType == null; i++) 
                    {
                        ModuleBuilder mBuilder = (ModuleBuilder) modList[i];
                        baseType = mBuilder.FindTypeBuilderWithName(baseName, ignoreCase);
                    }
                }
                if (baseType == null)
                    return null;
            }
            
            if (parameters == null)         
                return baseType;
        
            return GetType(parameters, baseType);
        }

        public override String FullyQualifiedName
        {
            [ResourceExposure(ResourceScope.Machine)]
            [ResourceConsumption(ResourceScope.Machine)]
            get
            {
                String fullyQualifiedName = m_moduleData.m_strFileName;
                if (fullyQualifiedName == null)
                    return null;
                if (Assembly.m_assemblyData.m_strDir != null)
                {
                    fullyQualifiedName = Path.Combine(Assembly.m_assemblyData.m_strDir, fullyQualifiedName);
                    fullyQualifiedName = Path.GetFullPath(fullyQualifiedName);
                }
                
                if (Assembly.m_assemblyData.m_strDir != null && fullyQualifiedName != null) 
                {
                    new FileIOPermission( FileIOPermissionAccess.PathDiscovery, fullyQualifiedName ).Demand();
                }

                return fullyQualifiedName;
            }
        }
                               
        #endregion

        #region Public Members

        #region Define Type
        public TypeBuilder DefineType(String name)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return DefineTypeNoLock(name);
                }
            }
            else
            {
                return DefineTypeNoLock(name);
            }
        }

        private TypeBuilder DefineTypeNoLock(String name)
        {
            TypeBuilder typeBuilder;
            typeBuilder =  new TypeBuilder(name, TypeAttributes.NotPublic, null, null, this, PackingSize.Unspecified, null);
            m_TypeBuilderList.Add(typeBuilder);
            return typeBuilder;
        }
    
        public TypeBuilder DefineType(String name, TypeAttributes attr)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return DefineTypeNoLock(name, attr);
                }
            }
            else
            {
                return DefineTypeNoLock(name, attr);
            }
        }

        private TypeBuilder DefineTypeNoLock(String name, TypeAttributes attr)
        {
            TypeBuilder typeBuilder;
            typeBuilder =  new TypeBuilder(name, attr, null, null, this, PackingSize.Unspecified, null);
            m_TypeBuilderList.Add(typeBuilder);
            return typeBuilder;
        }
    
        public TypeBuilder DefineType(String name, TypeAttributes attr, Type parent)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return DefineTypeNoLock(name, attr, parent);
                }
            }
            else
            {
                return DefineTypeNoLock(name, attr, parent);
            }
        }

        private TypeBuilder DefineTypeNoLock(String name, TypeAttributes attr, Type parent)
        {
            CheckContext(parent);

            TypeBuilder typeBuilder;
            
            typeBuilder =  new TypeBuilder(
                name, attr, parent, null, this, PackingSize.Unspecified, null);
            m_TypeBuilderList.Add(typeBuilder);
            return typeBuilder;
        }
    
        public TypeBuilder DefineType(String name, TypeAttributes attr, Type parent, int typesize)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return DefineTypeNoLock(name, attr, parent, typesize);
                }
            }
            else
            {
                return DefineTypeNoLock(name, attr, parent, typesize);
            }
        }

        private TypeBuilder DefineTypeNoLock(String name, TypeAttributes attr, Type parent, int typesize)
        {
            TypeBuilder typeBuilder;
            typeBuilder = new TypeBuilder(name, attr, parent, this, PackingSize.Unspecified, typesize, null);
            m_TypeBuilderList.Add(typeBuilder);
            return typeBuilder;
        }

        public TypeBuilder DefineType(String name, TypeAttributes attr, Type parent, PackingSize packingSize, int typesize)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return DefineTypeNoLock(name, attr, parent, packingSize, typesize);
                }
            }
            else
            {
                return DefineTypeNoLock(name, attr, parent, packingSize, typesize);
            }
        }

        private TypeBuilder DefineTypeNoLock(String name, TypeAttributes attr, Type parent, PackingSize packingSize, int typesize)
        {
            TypeBuilder typeBuilder;
            typeBuilder = new TypeBuilder(name, attr, parent, this, packingSize, typesize, null);
            m_TypeBuilderList.Add(typeBuilder);
            return typeBuilder;
        }
        
[System.Runtime.InteropServices.ComVisible(true)]
        public TypeBuilder DefineType(String name, TypeAttributes attr, Type parent, Type[] interfaces)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return DefineTypeNoLock(name, attr, parent, interfaces);
                }
            }
            else
            {
                return DefineTypeNoLock(name, attr, parent, interfaces);
            }
        }

        private TypeBuilder DefineTypeNoLock(String name, TypeAttributes attr, Type parent, Type[] interfaces)
        {
            TypeBuilder typeBuilder;
            typeBuilder =  new TypeBuilder(name, attr, parent, interfaces, this, PackingSize.Unspecified, null);
            m_TypeBuilderList.Add(typeBuilder);
            return typeBuilder;
        }
    
        #endregion

        #region Define Enum
        public TypeBuilder DefineType(String name, TypeAttributes attr, Type parent, PackingSize packsize)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return DefineTypeNoLock(name, attr, parent, packsize);
                }
            }
            else
            {
                return DefineTypeNoLock(name, attr, parent, packsize);
            }
        }

        private TypeBuilder DefineTypeNoLock(String name, TypeAttributes attr, Type parent, PackingSize packsize)
        {
            TypeBuilder typeBuilder;
            typeBuilder = new TypeBuilder(name, attr, parent, null, this, packsize, null);
            m_TypeBuilderList.Add(typeBuilder);
            return typeBuilder;
        }
    
        public EnumBuilder DefineEnum(String name, TypeAttributes visibility, Type underlyingType)
        {
            CheckContext(underlyingType);
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return DefineEnumNoLock(name, visibility, underlyingType);
                }
            }
            else
            {
                return DefineEnumNoLock(name, visibility, underlyingType);
            }
        }

        private EnumBuilder DefineEnumNoLock(String name, TypeAttributes visibility, Type underlyingType)
        {
            EnumBuilder enumBuilder;
            enumBuilder = new EnumBuilder(name, underlyingType, visibility, this);
            m_TypeBuilderList.Add(enumBuilder);
            return enumBuilder;
        }
    
        #endregion

        #region Define Resource
        public IResourceWriter DefineResource(String name, String description)
        {
            // Define embedded managed resource to be stored in this module
             
            return DefineResource(name, description, ResourceAttributes.Public);
        }

        public IResourceWriter DefineResource(String name, String description, ResourceAttributes attribute)
        {
            // Define embedded managed resource to be stored in this module
             

            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return DefineResourceNoLock(name, description, attribute);
                }
            }
            else
            {
                return DefineResourceNoLock(name, description, attribute);
            }
        }

        private IResourceWriter DefineResourceNoLock(String name, String description, ResourceAttributes attribute)
        {
            // Define embedded managed resource to be stored in this module
             

            if (IsTransient())
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_BadResourceContainer"));

            if (name == null)
                throw new ArgumentNullException("name");
            if (name.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "name");

            Assembly assembly = this.Assembly;
            if (assembly is AssemblyBuilder)
            {
                AssemblyBuilder asmBuilder = (AssemblyBuilder)assembly;
                if (asmBuilder.IsPersistable())
                {
                    asmBuilder.m_assemblyData.CheckResNameConflict(name);

                    MemoryStream stream = new MemoryStream();
                    ResourceWriter resWriter = new ResourceWriter(stream);
                    ResWriterData resWriterData = new ResWriterData( resWriter, stream, name, String.Empty, String.Empty, attribute);

                    // chain it to the embedded resource list
                    resWriterData.m_nextResWriter = m_moduleData.m_embeddedRes;
                    m_moduleData.m_embeddedRes = resWriterData;
                    return resWriter;
                }
                else
                {
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_BadResourceContainer"));
                }
            }
            else
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_BadResourceContainer"));
            }
        }

        public void DefineManifestResource(String name, Stream stream, ResourceAttributes attribute)
        {
            if (name == null)
                throw new ArgumentNullException("name");
            
            if (stream == null)
                throw new ArgumentNullException("stream");
            
            // Define embedded managed resource to be stored in this module
             

            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    DefineManifestResourceNoLock(name, stream, attribute);
                }
            }
            else
            {
                DefineManifestResourceNoLock(name, stream, attribute);
            }
        }

        private void DefineManifestResourceNoLock(String name, Stream stream, ResourceAttributes attribute)
        {
            // Define embedded managed resource to be stored in this module
            
        
            if (IsTransient())
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_BadResourceContainer"));
        
            if (name == null)
                throw new ArgumentNullException("name");
            if (name.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "name");
        
            Assembly assembly = this.Assembly;
            if (assembly is AssemblyBuilder)
            {
                AssemblyBuilder asmBuilder = (AssemblyBuilder)assembly;
                if (asmBuilder.IsPersistable())
                {
                    asmBuilder.m_assemblyData.CheckResNameConflict(name);

                    ResWriterData resWriterData = new ResWriterData( null, stream, name, String.Empty, String.Empty, attribute);
        
                    // chain it to the embedded resource list
                    resWriterData.m_nextResWriter = m_moduleData.m_embeddedRes;
                    m_moduleData.m_embeddedRes = resWriterData;
                }
                else
                { 
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_BadResourceContainer"));
                }
            }
            else
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_BadResourceContainer"));
            }
        }


        public void DefineUnmanagedResource(Byte[] resource)
        {
            if (m_moduleData.m_strResourceFileName != null || m_moduleData.m_resourceBytes != null)
                throw new ArgumentException(Environment.GetResourceString("Argument_NativeResourceAlreadyDefined"));
            
            if (resource == null)            
                throw new ArgumentNullException("resource");
            
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
        
            m_moduleData.m_resourceBytes = new byte[resource.Length];
            System.Array.Copy(resource, m_moduleData.m_resourceBytes, resource.Length);
            m_moduleData.m_fHasExplicitUnmanagedResource = true;
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public void DefineUnmanagedResource(String resourceFileName)
        {
            // No resource should have been set already.
            if (m_moduleData.m_strResourceFileName != null || m_moduleData.m_resourceBytes != null)
                throw new ArgumentException(Environment.GetResourceString("Argument_NativeResourceAlreadyDefined"));
            
            // Must have a file name.
            if (resourceFileName == null)            
                throw new ArgumentNullException("resourceFileName");
            
            // Defer to internal implementation.
            DefineUnmanagedResourceFileInternal(resourceFileName);
            m_moduleData.m_fHasExplicitUnmanagedResource = true;
        }
        
        #endregion

        #region Define Global Method
        public MethodBuilder DefineGlobalMethod(String name, MethodAttributes attributes, Type returnType, Type[] parameterTypes)
        {
            return DefineGlobalMethod(name, attributes, CallingConventions.Standard, returnType, parameterTypes);
        }

        public MethodBuilder DefineGlobalMethod(String name, MethodAttributes attributes, CallingConventions callingConvention, 
            Type returnType, Type[] parameterTypes)
        {
            return DefineGlobalMethod(name, attributes, callingConvention, returnType, null, null, parameterTypes, null, null);
        }

        public MethodBuilder DefineGlobalMethod(String name, MethodAttributes attributes, CallingConventions callingConvention, 
            Type returnType, Type[] requiredReturnTypeCustomModifiers, Type[] optionalReturnTypeCustomModifiers,
            Type[] parameterTypes, Type[][] requiredParameterTypeCustomModifiers, Type[][] optionalParameterTypeCustomModifiers)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return DefineGlobalMethodNoLock(name, attributes, callingConvention, returnType, 
                                                    requiredReturnTypeCustomModifiers, optionalReturnTypeCustomModifiers,
                                                    parameterTypes, requiredParameterTypeCustomModifiers, optionalParameterTypeCustomModifiers);
                }
            }
            else
            {
                return DefineGlobalMethodNoLock(name, attributes, callingConvention, returnType, 
                                                requiredReturnTypeCustomModifiers, optionalReturnTypeCustomModifiers,
                                                parameterTypes, requiredParameterTypeCustomModifiers, optionalParameterTypeCustomModifiers);
            }
        }

        private MethodBuilder DefineGlobalMethodNoLock(String name, MethodAttributes attributes, CallingConventions callingConvention, 
            Type returnType, Type[] requiredReturnTypeCustomModifiers, Type[] optionalReturnTypeCustomModifiers,
            Type[] parameterTypes, Type[][] requiredParameterTypeCustomModifiers, Type[][] optionalParameterTypeCustomModifiers)
        {
            CheckContext(returnType);
            CheckContext(requiredReturnTypeCustomModifiers, optionalReturnTypeCustomModifiers, parameterTypes);
            CheckContext(requiredParameterTypeCustomModifiers);
            CheckContext(optionalParameterTypeCustomModifiers);
            
            if (m_moduleData.m_fGlobalBeenCreated == true)
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_GlobalsHaveBeenCreated"));
        
            if (name == null)
                throw new ArgumentNullException("name");

            if (name.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "name");
        
            if ((attributes & MethodAttributes.Static) == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_GlobalFunctionHasToBeStatic"));

            m_moduleData.m_fHasGlobal = true;

            return m_moduleData.m_globalTypeBuilder.DefineMethod(name, attributes, callingConvention, 
                returnType, requiredReturnTypeCustomModifiers, optionalReturnTypeCustomModifiers, 
                parameterTypes, requiredParameterTypeCustomModifiers, optionalParameterTypeCustomModifiers);
        }
        
        public MethodBuilder DefinePInvokeMethod(String name, String dllName, MethodAttributes attributes, 
            CallingConventions callingConvention, Type returnType, Type[] parameterTypes, 
            CallingConvention nativeCallConv, CharSet nativeCharSet)
        {
            return DefinePInvokeMethod(name, dllName, name, attributes, callingConvention, returnType, parameterTypes, nativeCallConv, nativeCharSet);
        }

        public MethodBuilder DefinePInvokeMethod(String name, String dllName, String entryName, MethodAttributes attributes, 
            CallingConventions callingConvention, Type returnType, Type[] parameterTypes, CallingConvention nativeCallConv, 
            CharSet nativeCharSet)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return DefinePInvokeMethodNoLock(name, dllName, entryName, attributes, callingConvention, 
                                                     returnType, parameterTypes, nativeCallConv, nativeCharSet);
                }
            }
            else
            {
                return DefinePInvokeMethodNoLock(name, dllName, entryName, attributes, callingConvention, 
                                                 returnType, parameterTypes, nativeCallConv, nativeCharSet);
            }
        }

        private MethodBuilder DefinePInvokeMethodNoLock(String name, String dllName, String entryName, MethodAttributes attributes, 
            CallingConventions callingConvention, Type returnType, Type[] parameterTypes, CallingConvention nativeCallConv, 
            CharSet nativeCharSet)
        {
            CheckContext(returnType);
            CheckContext(parameterTypes);
  
            //Global methods must be static.        
            if ((attributes & MethodAttributes.Static) == 0)
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_GlobalFunctionHasToBeStatic"));
            }
            m_moduleData.m_fHasGlobal = true;
            return m_moduleData.m_globalTypeBuilder.DefinePInvokeMethod(name, dllName, entryName, attributes, callingConvention, returnType, parameterTypes, nativeCallConv, nativeCharSet);
        }
        
        public void CreateGlobalFunctions()
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    CreateGlobalFunctionsNoLock();
                }
            }
            else
            {
                CreateGlobalFunctionsNoLock();
            }
        }

        private void CreateGlobalFunctionsNoLock()
        {
            if (m_moduleData.m_fGlobalBeenCreated)
            {
                // cannot create globals twice
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NotADebugModule"));
            }
            m_moduleData.m_globalTypeBuilder.CreateType();
            m_moduleData.m_fGlobalBeenCreated = true;
        }
    
        #endregion

        #region Define Data
        public FieldBuilder DefineInitializedData(String name, byte[] data, FieldAttributes attributes)
        {
            // This method will define an initialized Data in .sdata. 
            // We will create a fake TypeDef to represent the data with size. This TypeDef
            // will be the signature for the Field.
 
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return DefineInitializedDataNoLock(name, data, attributes);
                }
            }
            else
            {
                return DefineInitializedDataNoLock(name, data, attributes);
            }
        }

        private FieldBuilder DefineInitializedDataNoLock(String name, byte[] data, FieldAttributes attributes)
        {
            // This method will define an initialized Data in .sdata. 
            // We will create a fake TypeDef to represent the data with size. This TypeDef
            // will be the signature for the Field.
            if (m_moduleData.m_fGlobalBeenCreated == true)
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_GlobalsHaveBeenCreated"));
            }
        
            m_moduleData.m_fHasGlobal = true;
            return m_moduleData.m_globalTypeBuilder.DefineInitializedData(name, data, attributes);
        }
        
        public FieldBuilder DefineUninitializedData(String name, int size, FieldAttributes attributes)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return DefineUninitializedDataNoLock(name, size, attributes);
                }
            }
            else
            {
                return DefineUninitializedDataNoLock(name, size, attributes);
            }
        }

        private FieldBuilder DefineUninitializedDataNoLock(String name, int size, FieldAttributes attributes)
        {
            // This method will define an uninitialized Data in .sdata. 
            // We will create a fake TypeDef to represent the data with size. This TypeDef
            // will be the signature for the Field. 

            if (m_moduleData.m_fGlobalBeenCreated == true)
            {
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_GlobalsHaveBeenCreated"));
            }
        
            m_moduleData.m_fHasGlobal = true;
            return m_moduleData.m_globalTypeBuilder.DefineUninitializedData(name, size, attributes);
        }
                
        #endregion

        #region GetToken
        public TypeToken GetTypeToken(Type type)
        {        
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return GetTypeTokenNoLock(type);
                }
            }
            else
            {
                return GetTypeTokenNoLock(type);
            }
        }

        private TypeToken GetTypeTokenNoLock(Type type)
        {
            CheckContext(type);
            
            // Return a token for the class relative to the Module.  Tokens
            // are used to indentify objects when the objects are used in IL
            // instructions.  Tokens are always relative to the Module.  For example,
            // the token value for System.String is likely to be different from
            // Module to Module.  Calling GetTypeToken will cause a reference to be
            // added to the Module.  This reference becomes a perminate part of the Module,
            // multiple calles to this method with the same class have no additional side affects.
            // This function is optimized to use the TypeDef token if Type is within the same module.
            // We should also be aware of multiple dynamic modules and multiple implementation of Type!!!

            TypeToken tkToken;
            bool isSameAssembly;
            bool isSameModule;
            Module refedModule;
            String strRefedModuleFileName = String.Empty;
        
            // assume that referenced module is non-transient. Only if the referenced module is dynamic,
            // and transient, this variable will be set to true.
            bool isRefedModuleTransient = false;

            if (type == null)
                throw new ArgumentNullException("type");

            refedModule = type.Module;
            isSameModule = refedModule.Equals(this);

            if (type.IsByRef)
                throw new ArgumentException(Environment.GetResourceString("Argument_CannotGetTypeTokenForByRef"));   
        
            if ((type.IsGenericType && !type.IsGenericTypeDefinition) || type.IsGenericParameter || 
                type.IsArray || type.IsPointer)
            {
                int length; 
                byte[] sig = SignatureHelper.GetTypeSigToken(this, type).InternalGetSignature(out length);
                return new TypeToken(InternalGetTypeSpecTokenWithBytes(sig, length));
            }

            // After this point, it is not an array type nor Pointer type

            if (isSameModule)
            {
                // no need to do anything additional other than defining the TypeRef Token
                if (type is TypeBuilder)
                {
                    // optimization: if the type is defined in this module,
                    // just return the token
                    //
                    TypeBuilder typeBuilder = (TypeBuilder) type;
                    return typeBuilder.TypeToken;
                }
                else if (type is EnumBuilder)
                {
                    TypeBuilder typeBuilder = ((EnumBuilder) type).m_typeBuilder; 
                    return typeBuilder.TypeToken; 
                }
                else if (type is GenericTypeParameterBuilder)
                {
                    return new TypeToken(type.MetadataTokenInternal);
                }
                
                return new TypeToken(GetTypeRefNested(type, this, String.Empty));
            }
                    
            // After this point, the referenced module is not the same as the referencing
            // module.
            //
            isSameAssembly = refedModule.Assembly.Equals(Assembly);
            if (refedModule is ModuleBuilder)
            {
                ModuleBuilder       refedModuleBuilder = (ModuleBuilder) refedModule;
                if (refedModuleBuilder.IsTransient())
                {
                    isRefedModuleTransient = true;
                }
                // get the referenced module's file name
                strRefedModuleFileName = refedModuleBuilder.m_moduleData.m_strFileName;
            }
            else
                strRefedModuleFileName = refedModule.ScopeName;
                    
            // We cannot have a non-transient module referencing to a transient module.
            if (IsTransient() == false && isRefedModuleTransient)
            {
                 
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_BadTransientModuleReference"));   
            }
            
            tkToken = new TypeToken(GetTypeRefNested(type, refedModule, strRefedModuleFileName));
            return tkToken;
        }
        
        public TypeToken GetTypeToken(String name)
        {
            
            
            return GetTypeToken(base.GetType(name, false, true));
        }

        public MethodToken GetMethodToken(MethodInfo method)
        {
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return GetMethodTokenNoLock(method);
                }
            }
            else
            {
                return GetMethodTokenNoLock(method);
            }
        }

        private MethodToken GetMethodTokenNoLock(MethodInfo method)
        {
            // Return a MemberRef token if MethodInfo is not defined in this module. Or 
            // return the MethodDef token. 

            int tr;
            int mr = 0;
            
            if (method == null) 
                throw new ArgumentNullException("method");

            if (method is MethodBuilder || method is MethodOnTypeBuilderInstantiation)
            {
                if (method.Module == this)
                    return new MethodToken(method.MetadataTokenInternal);  

                if (method.DeclaringType == null)
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CannotImportGlobalFromDifferentModule"));

                // method is defined in a different module
                tr = GetTypeToken(method.DeclaringType).Token;
                mr = InternalGetMemberRef(method.DeclaringType.Module, tr, method.MetadataTokenInternal);
            }
            else if (method is SymbolMethod)
            {
                SymbolMethod symMethod = method as SymbolMethod;

                if (symMethod.GetModule() == this)
                    return symMethod.GetToken();

                // form the method token
                return symMethod.GetToken(this);
            }
            else
            {
                Type declaringType = method.DeclaringType;

                // We need to get the TypeRef tokens
                if (declaringType == null)
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CannotImportGlobalFromDifferentModule"));

                if (declaringType.IsArray == true)
                {
                    // use reflection to build signature to work around the E_T_VAR problem in EEClass
                    ParameterInfo[] paramInfo = method.GetParameters();
                    
                    Type[] tt = new Type[paramInfo.Length];
                    
                    for (int i = 0; i < paramInfo.Length; i++)
                        tt[i] = paramInfo[i].ParameterType;

                    return GetArrayMethodToken(declaringType, method.Name, method.CallingConvention, method.ReturnType, tt);
                }
                else if (method is RuntimeMethodInfo)
                {
                    tr = GetTypeToken(declaringType).Token;
                    mr = InternalGetMemberRefOfMethodInfo(tr, method.GetMethodHandle());
                }
                else
                {
                    // some user derived ConstructorInfo
                    // go through the slower code path, i.e. retrieve parameters and form signature helper.
                    ParameterInfo[] parameters = method.GetParameters();

                    Type[] parameterTypes = new Type[parameters.Length];
                    Type[][] requiredCustomModifiers = new Type[parameterTypes.Length][];
                    Type[][] optionalCustomModifiers = new Type[parameterTypes.Length][];

                    for (int i = 0; i < parameters.Length; i++)
                    {
                        parameterTypes[i] = parameters[i].ParameterType;
                        requiredCustomModifiers[i] = parameters[i].GetRequiredCustomModifiers();
                        optionalCustomModifiers[i] = parameters[i].GetOptionalCustomModifiers();
                    }
          
                    tr = GetTypeToken(method.ReflectedType).Token;

                    SignatureHelper sigHelp;

                    try 
                    {
                        sigHelp = SignatureHelper.GetMethodSigHelper(
                        this, method.CallingConvention, method.ReturnType, 
                        method.ReturnParameter.GetRequiredCustomModifiers(), method.ReturnParameter.GetOptionalCustomModifiers(), 
                        parameterTypes, requiredCustomModifiers, optionalCustomModifiers);
                    } 
                    catch(NotImplementedException)
                    {
                        // Legacy code deriving from MethodInfo may not have implemented ReturnParameter.
                        sigHelp = SignatureHelper.GetMethodSigHelper(this, method.ReturnType, parameterTypes);
                    }

                    int length;                                           
                    byte[] sigBytes = sigHelp.InternalGetSignature(out length);
                    mr = InternalGetMemberRefFromSignature(tr, method.Name, sigBytes, length);                                         
                }
            }

            return new MethodToken(mr);
        }
    
        public MethodToken GetArrayMethodToken(Type arrayClass, String methodName, CallingConventions callingConvention, 
            Type returnType, Type[] parameterTypes)
        {
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return GetArrayMethodTokenNoLock(arrayClass, methodName, callingConvention, returnType, parameterTypes);
                }
            }
            else
            {
                return GetArrayMethodTokenNoLock(arrayClass, methodName, callingConvention, returnType, parameterTypes);
            }
        }

        private MethodToken GetArrayMethodTokenNoLock(Type arrayClass, String methodName, CallingConventions callingConvention, 
            Type returnType, Type[] parameterTypes)
        {
            CheckContext(returnType, arrayClass);
            CheckContext(parameterTypes);

            // Return a token for the MethodInfo for a method on an Array.  This is primarily
            // used to get the LoadElementAddress method. 

            Type baseType;
            int baseToken;
            int length;
        
            if (arrayClass == null)
                throw new ArgumentNullException("arrayClass");

            if (methodName == null)
                throw new ArgumentNullException("methodName");

            if (methodName.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "methodName");

            if (arrayClass.IsArray == false)
                throw new ArgumentException(Environment.GetResourceString("Argument_HasToBeArrayClass")); 

            SignatureHelper sigHelp = SignatureHelper.GetMethodSigHelper(
                this, callingConvention, returnType, null, null, parameterTypes, null, null);

            byte[] sigBytes = sigHelp.InternalGetSignature(out length);

            // track the TypeRef of the array base class
            for (baseType = arrayClass; baseType.IsArray; baseType = baseType.GetElementType());
            baseToken = GetTypeToken(baseType).Token;

            TypeToken typeSpec = GetTypeToken(arrayClass);

            return new MethodToken(nativeGetArrayMethodToken(
                typeSpec.Token, methodName, sigBytes, length, baseToken /* TODO: Remove */));
        }

        public MethodInfo GetArrayMethod(Type arrayClass, String methodName, CallingConventions callingConvention, 
            Type returnType, Type[] parameterTypes)
        {
            CheckContext(returnType, arrayClass);
            CheckContext(parameterTypes);

            // GetArrayMethod is useful when you have an array of a type whose definition has not been completed and 
            // you want to access methods defined on Array. For example, you might define a type and want to define a 
            // method that takes an array of the type as a parameter. In order to access the elements of the array, 
            // you will need to call methods of the Array class.

            MethodToken token = GetArrayMethodToken(arrayClass, methodName, callingConvention, returnType, parameterTypes);

            return new SymbolMethod(this, token, arrayClass, methodName, callingConvention, returnType, parameterTypes);
        }

        [System.Runtime.InteropServices.ComVisible(true)]
        public MethodToken GetConstructorToken(ConstructorInfo con)
        {
            // Return a token for the ConstructorInfo relative to the Module. 
            return InternalGetConstructorToken(con, false);
        }

        public FieldToken GetFieldToken(FieldInfo field) 
        {
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return GetFieldTokenNoLock(field);
                }
            }
            else
            {
                return GetFieldTokenNoLock(field);
            }
        }

        private FieldToken GetFieldTokenNoLock(FieldInfo field) 
        {
            int     tr;
            int     mr = 0;
            
            if (field == null) {
                throw new ArgumentNullException("con");
            }
            else if (field is FieldBuilder)
            {
                FieldBuilder fdBuilder = (FieldBuilder) field;

                if (field.DeclaringType != null && field.DeclaringType.IsGenericType)
                {
                    int length;
                    byte[] sig = SignatureHelper.GetTypeSigToken(this, field.DeclaringType).InternalGetSignature(out length);
                    tr = InternalGetTypeSpecTokenWithBytes(sig, length);
                    mr = InternalGetMemberRef(this, tr, fdBuilder.GetToken().Token);
                }
                else if (fdBuilder.GetTypeBuilder().Module.Equals(this))
                {
                    // field is defined in the same module
                    return fdBuilder.GetToken();
                }
                else
                {
                    // field is defined in a different module
                    if (field.DeclaringType == null)
                    {
                        throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CannotImportGlobalFromDifferentModule"));
                    }
                    tr = GetTypeToken(field.DeclaringType).Token;
                    mr = InternalGetMemberRef(field.ReflectedType.Module, tr, fdBuilder.GetToken().Token);
                }
            }
            else if (field is RuntimeFieldInfo)
            {
                // FieldInfo is not an dynamic field
                
                // We need to get the TypeRef tokens
                if (field.DeclaringType == null)
                {
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_CannotImportGlobalFromDifferentModule"));
                }
                
                if (field.DeclaringType != null && field.DeclaringType.IsGenericType)
                {
                    int length;
                    byte[] sig = SignatureHelper.GetTypeSigToken(this, field.DeclaringType).InternalGetSignature(out length);
                    tr = InternalGetTypeSpecTokenWithBytes(sig, length);
                    mr = InternalGetMemberRefOfFieldInfo(tr, field.DeclaringType.GetTypeHandleInternal(), field.MetadataTokenInternal);
                }
                else
                {
                    tr = GetTypeToken(field.DeclaringType).Token;       
                    mr = InternalGetMemberRefOfFieldInfo(tr, field.DeclaringType.GetTypeHandleInternal(), field.MetadataTokenInternal);
                }
            }
            else if (field is FieldOnTypeBuilderInstantiation)
            {
                FieldInfo fb = ((FieldOnTypeBuilderInstantiation)field).FieldInfo;
                int length;
                byte[] sig = SignatureHelper.GetTypeSigToken(this, field.DeclaringType).InternalGetSignature(out length);
                tr = InternalGetTypeSpecTokenWithBytes(sig, length);
                mr = InternalGetMemberRef(fb.ReflectedType.Module, tr, fb.MetadataTokenInternal);
            }
            else
            {
                // user defined FieldInfo
                tr = GetTypeToken(field.ReflectedType).Token;

                SignatureHelper sigHelp = SignatureHelper.GetFieldSigHelper(this);

                sigHelp.AddArgument(field.FieldType, field.GetRequiredCustomModifiers(), field.GetOptionalCustomModifiers());

                int length;
                byte[] sigBytes = sigHelp.InternalGetSignature(out length);

                mr = InternalGetMemberRefFromSignature(tr, field.Name, sigBytes, length);
            }
            
            return new FieldToken(mr, field.GetType());
        }
        
        public StringToken GetStringConstant(String str) 
        {
            // Returns a token representing a String constant.  If the string 
            // value has already been defined, the existing token will be returned.
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            return new StringToken(InternalGetStringConstant(str));
        }
    
        public SignatureToken GetSignatureToken(SignatureHelper sigHelper)
        {
            // Define signature token given a signature helper. This will define a metadata
            // token for the signature described by SignatureHelper.
            
            int sigLength;
            byte[] sigBytes;
    
            if (sigHelper == null)
            {
                throw new ArgumentNullException("sigHelper");
            }

            // get the signature in byte form
            sigBytes = sigHelper.InternalGetSignature(out sigLength);
            return new SignatureToken(TypeBuilder.InternalGetTokenFromSig(this, sigBytes, sigLength), this);
        }           
        public SignatureToken GetSignatureToken(byte[] sigBytes, int sigLength)
        {
            byte[] localSigBytes = null;
            
            if (sigBytes != null)
            {
                localSigBytes = new byte[sigBytes.Length];
                Array.Copy(sigBytes, localSigBytes, sigBytes.Length);
            }
            return new SignatureToken(TypeBuilder.InternalGetTokenFromSig(this, localSigBytes, sigLength), this);
        }
    
        #endregion

        #region Other
[System.Runtime.InteropServices.ComVisible(true)]
        public void SetCustomAttribute(ConstructorInfo con, byte[] binaryAttribute)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (con == null)
                throw new ArgumentNullException("con");
            if (binaryAttribute == null)
                throw new ArgumentNullException("binaryAttribute");
            
            TypeBuilder.InternalCreateCustomAttribute(
                1,                                          // This is hard coding the module token to 1
                this.GetConstructorToken(con).Token,
                binaryAttribute,
                this,
                false);
        }

        public void SetCustomAttribute(CustomAttributeBuilder customBuilder)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (customBuilder == null)
            {
                throw new ArgumentNullException("customBuilder");
            }

            customBuilder.CreateCustomAttribute(this, 1);   // This is hard coding the module token to 1
        }

        public ISymbolWriter GetSymWriter()
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            return m_iSymWriter;
        }
        
        public ISymbolDocumentWriter DefineDocument(String url, Guid language, Guid languageVendor, Guid documentType)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    return DefineDocumentNoLock(url, language, languageVendor, documentType);
                }
            }
            else
            {
                return DefineDocumentNoLock(url, language, languageVendor, documentType);
            }
        }

        private ISymbolDocumentWriter DefineDocumentNoLock(String url, Guid language, Guid languageVendor, Guid documentType)
        {
            if (m_iSymWriter == null)
            {
                // Cannot DefineDocument when it is not a debug module
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NotADebugModule"));
            }

            return m_iSymWriter.DefineDocument(url, language, languageVendor, documentType);
        }
    
        public void SetUserEntryPoint(MethodInfo entryPoint)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    SetUserEntryPointNoLock(entryPoint);
                }
            }
            else
            {
                SetUserEntryPointNoLock(entryPoint);
            }
        }

        private void SetUserEntryPointNoLock(MethodInfo entryPoint)
        {
            // Set the user entry point. Compiler may generate startup stub before calling user main.
            // The startup stub will be the entry point. While the user "main" will be the user entry
            // point so that debugger will not step into the compiler entry point.

            if (entryPoint == null)
            {
                throw new ArgumentNullException("entryPoint");
            }
        
            if (m_iSymWriter == null)
            {
                // Cannot set entry point when it is not a debug module
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NotADebugModule"));
            }

            if (entryPoint.DeclaringType != null)
            {
                if (entryPoint.Module != this)
                {
                    // you cannot pass in a MethodInfo that is not contained by this ModuleBuilder
                    throw new InvalidOperationException(Environment.GetResourceString("Argument_NotInTheSameModuleBuilder"));
                }
            }
            else
            {
                MethodBuilder mb = entryPoint as MethodBuilder;
                if (mb != null && mb.GetModule() != this)
                {
                    // you cannot pass in a MethodInfo that is not contained by this ModuleBuilder
                    throw new InvalidOperationException(Environment.GetResourceString("Argument_NotInTheSameModuleBuilder"));                    
                }                    
            }
                
            // get the metadata token value and create the SymbolStore's token value class
            SymbolToken       tkMethod = new SymbolToken(GetMethodToken(entryPoint).Token);

            // set the UserEntryPoint
            m_iSymWriter.SetUserEntryPoint(tkMethod);
        }
    
        public void SetSymCustomAttribute(String name, byte[] data)
        {
            CodeAccessPermission.DemandInternal(PermissionType.ReflectionEmit);
            if (Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Assembly.m_assemblyData)
                {
                    SetSymCustomAttributeNoLock(name, data);
                }
            }
            else
            {
                SetSymCustomAttributeNoLock(name, data);
            }
        }

        private void SetSymCustomAttributeNoLock(String name, byte[] data)
        {
            if (m_iSymWriter == null)
            {
                // Cannot SetSymCustomAttribute when it is not a debug module
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NotADebugModule"));
            }
        
             
             
            //SymbolToken      tk = new SymbolToken();
            //m_iSymWriter.SetSymAttribute(tk, name, data);
        }
    
        public bool IsTransient()
        {
            return m_moduleData.IsTransient();
        }
        
        #endregion

        #endregion    

        void _ModuleBuilder.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _ModuleBuilder.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _ModuleBuilder.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _ModuleBuilder.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }
    }
}

















