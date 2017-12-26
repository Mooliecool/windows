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

namespace System.Reflection.Emit {

    using System;
    using System.Runtime.Remoting.Activation;
    using System.Reflection;
    using System.Security;
    using System.Security.Permissions;
    using System.Diagnostics;
    using System.Runtime.InteropServices;
    using System.Runtime.CompilerServices;
    using ArrayList = System.Collections.ArrayList;
    using CultureInfo = System.Globalization.CultureInfo;
    using System.Threading;


    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum PackingSize
    {
        Unspecified                 = 0,
        Size1                       = 1,
        Size2                       = 2,
        Size4                       = 4,
        Size8                       = 8,
        Size16                      = 16,
        Size32                      = 32,
        Size64                      = 64,
        Size128                     = 128,
    }


    [HostProtection(MayLeakOnAbort = true)]
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_TypeBuilder))]
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class TypeBuilder : Type, _TypeBuilder
    {
        #region Declarations
        internal class CustAttr
        {
            private ConstructorInfo m_con;
            private byte[] m_binaryAttribute;
            private CustomAttributeBuilder m_customBuilder;
            
            public CustAttr(ConstructorInfo con, byte[] binaryAttribute)
            {
                if (con == null)
                    throw new ArgumentNullException("con");

                if (binaryAttribute == null)
                    throw new ArgumentNullException("binaryAttribute");

                m_con = con;
                m_binaryAttribute = binaryAttribute;
            }

            public CustAttr(CustomAttributeBuilder customBuilder)
            {
                if (customBuilder == null)
                    throw new ArgumentNullException("customBuilder");

                m_customBuilder = customBuilder;
            }

            public void Bake(ModuleBuilder module, int token)
            {
                if (m_customBuilder == null)
                {
                    TypeBuilder.InternalCreateCustomAttribute(token, module.GetConstructorToken(m_con).Token,
                        m_binaryAttribute, module, false);
                }
                else
                {
                    m_customBuilder.CreateCustomAttribute(module, token);
                }
            }
        }
        #endregion
        
        #region Public Static Methods
        public static MethodInfo GetMethod(Type type, MethodInfo method)
        {
            if (!(type is TypeBuilder) && !(type is TypeBuilderInstantiation))
                throw new ArgumentException(Environment.GetResourceString("Argument_MustBeTypeBuilder"));

            // The following checks establishes invariants that more simply put require type to be generic and 
            // method to be a generic method definition declared on the generic type definition of type.
            // To create generic method G<Foo>.M<Bar> these invariants require that G<Foo>.M<S> be created by calling 
            // this function followed by MakeGenericMethod on the resulting MethodInfo to finally get G<Foo>.M<Bar>.
            // We could also allow G<T>.M<Bar> to be created before G<Foo>.M<Bar> (BindGenParm followed by this method) 
            // if we wanted to but that just complicates things so these checks are designed to prevent that scenario.
            
            if (method.IsGenericMethod && !method.IsGenericMethodDefinition)
                throw new ArgumentException(Environment.GetResourceString("Argument_NeedGenericMethodDefinition"), "method");
        
            if (method.DeclaringType == null || !method.DeclaringType.IsGenericTypeDefinition)
                throw new ArgumentException(Environment.GetResourceString("Argument_MethodNeedGenericDeclaringType"), "method");
        
            if (type.GetGenericTypeDefinition() != method.DeclaringType)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidMethodDeclaringType"), "type");

            // The following converts from Type or TypeBuilder of G<T> to TypeBuilderInstantiation G<T>. These types
            // both logically represent the same thing. The runtime displays a similar convention by having 
            // G<M>.M() be encoded by a typeSpec whose parent is the typeDef for G<M> and whose instantiation is also G<M>.
            if (type.IsGenericTypeDefinition)
                type = type.MakeGenericType(type.GetGenericArguments());
        
            if (!(type is TypeBuilderInstantiation))
                throw new ArgumentException(Environment.GetResourceString("Argument_NeedNonGenericType"), "type");

            return MethodOnTypeBuilderInstantiation.GetMethod(method, type as TypeBuilderInstantiation);
        }
        public static ConstructorInfo GetConstructor(Type type, ConstructorInfo constructor)
        {            
            if (!(type is TypeBuilder) && !(type is TypeBuilderInstantiation))
                throw new ArgumentException(Environment.GetResourceString("Argument_MustBeTypeBuilder"));

            if (!constructor.DeclaringType.IsGenericTypeDefinition)
                throw new ArgumentException(Environment.GetResourceString("Argument_ConstructorNeedGenericDeclaringType"), "constructor");
        
            if (!(type is TypeBuilderInstantiation))
                throw new ArgumentException(Environment.GetResourceString("Argument_NeedNonGenericType"), "type");

            // TypeBuilder G<T> ==> TypeBuilderInstantiation G<T>
            if (type is TypeBuilder && type.IsGenericTypeDefinition)
                type = type.MakeGenericType(type.GetGenericArguments());

            if (type.GetGenericTypeDefinition() != constructor.DeclaringType)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidConstructorDeclaringType"), "type");

            return ConstructorOnTypeBuilderInstantiation.GetConstructor(constructor, type as TypeBuilderInstantiation);
        }
        public static FieldInfo GetField(Type type, FieldInfo field)
        {
            if (!(type is TypeBuilder) && !(type is TypeBuilderInstantiation))
                throw new ArgumentException(Environment.GetResourceString("Argument_MustBeTypeBuilder"));

            if (!field.DeclaringType.IsGenericTypeDefinition)
                throw new ArgumentException(Environment.GetResourceString("Argument_FieldNeedGenericDeclaringType"), "field");
        
            if (!(type is TypeBuilderInstantiation))
                throw new ArgumentException(Environment.GetResourceString("Argument_NeedNonGenericType"), "type");

            // TypeBuilder G<T> ==> TypeBuilderInstantiation G<T>
            if (type is TypeBuilder && type.IsGenericTypeDefinition)
                type = type.MakeGenericType(type.GetGenericArguments());

            if (type.GetGenericTypeDefinition() != field.DeclaringType)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidFieldDeclaringType"), "type");

            return FieldOnTypeBuilderInstantiation.GetField(field, type as TypeBuilderInstantiation);
        }
        #endregion

        #region Public Const
        public const int UnspecifiedTypeSize = 0;
        #endregion

        #region Private Static FCalls
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void InternalSetParentType(int tdTypeDef, int tkParent, Module module);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void InternalAddInterfaceImpl(int tdTypeDef, int tkInterface, Module module);
        #endregion

        #region Internal Static FCalls
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern int InternalDefineMethod(int handle, String name, byte[] signature, int sigLength, 
            MethodAttributes attributes, Module module);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern int InternalDefineMethodSpec(int handle, byte[] signature, int sigLength, Module module);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern int InternalDefineField(int handle, String name, byte[] signature, int sigLength, 
            FieldAttributes attributes, Module module);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void InternalSetMethodIL(int methodHandle, bool isInitLocals, byte[] body, byte[] LocalSig, 
            int sigLength, int maxStackSize, int numExceptions, __ExceptionInstance[] exceptions, int []tokenFixups, 
            int []rvaFixups, Module module);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void InternalCreateCustomAttribute(int tkAssociate, int tkConstructor, 
            byte[] attr, Module module, bool toDisk, bool updateCompilerFlags);

        internal static void InternalCreateCustomAttribute(int tkAssociate, int tkConstructor, byte[] attr, Module module, bool toDisk)
        {
            byte[] localAttr = null;

            if (attr != null)
            {
                localAttr = new byte[attr.Length];
                Array.Copy(attr, localAttr, attr.Length);
            }
            TypeBuilder.InternalCreateCustomAttribute(tkAssociate, tkConstructor, localAttr, module, toDisk, false);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void InternalSetPInvokeData(Module module, String DllName, String name, int token,
            int linkType, int linkFlags);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern int InternalDefineProperty(Module module, int handle, String name, int attributes, 
            byte[] signature, int sigLength, int notifyChanging, int notifyChanged);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern int InternalDefineEvent(Module module, int handle, String name, int attributes, int tkEventType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void InternalDefineMethodSemantics(Module module, int tkAssociation, 
            MethodSemanticsAttributes semantics, int tkMethod);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void InternalDefineMethodImpl(ModuleBuilder module, int tkType, int tkBody, int tkDecl);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void InternalSetMethodImpl(Module module, int tkMethod, MethodImplAttributes MethodImplAttributes);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern int InternalSetParamInfo(Module module, int tkMethod, int iSequence, 
            ParameterAttributes iParamAttributes, String strParamName);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern int InternalGetTokenFromSig(Module module, byte[] signature, int sigLength);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void InternalSetFieldOffset(Module module, int fdToken, int iOffset);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void InternalSetClassLayout(Module module, int tdToken, PackingSize iPackingSize, int iTypeSize);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void InternalSetMarshalInfo(Module module, int tk, byte[] ubMarshal, int ubSize);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void InternalSetConstantValue(Module module, int tk, ref Variant var);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void InternalAddDeclarativeSecurity(Module module, int parent, SecurityAction action, byte[] blob);
        #endregion

        #region Internal\Private Static Members
        private static bool IsPublicComType(Type type)
        {
            // Internal Helper to determine if a type should be added to ComType table.
            // A top level type should be added if it is Public.
            // A nested type should be added if the top most enclosing type is Public 
            //      and all the enclosing types are NestedPublic

            Type enclosingType = type.DeclaringType;
            if (enclosingType != null)
            {
                if (IsPublicComType(enclosingType))
                {
                    if ((type.Attributes & TypeAttributes.VisibilityMask) == TypeAttributes.NestedPublic)
                    {
                        return true;
                    }
                }
            }
            else
            {
                if ((type.Attributes & TypeAttributes.VisibilityMask) == TypeAttributes.Public)
                {
                    return true;
                }
            }

            return false;
        }

        internal static bool IsTypeEqual(Type t1, Type t2)
        {
            // Maybe we are lucky that they are equal in the first place
            if (t1 == t2)
                return true;
            TypeBuilder tb1 = null;
            TypeBuilder tb2 = null;  
            Type runtimeType1 = null;              
            Type runtimeType2 = null;    
            
            // set up the runtimeType and TypeBuilder type corresponding to t1 and t2
            if (t1 is TypeBuilder)
            {
                tb1 =(TypeBuilder)t1;
                // This will be null if it is not baked.
                runtimeType1 = tb1.m_runtimeType;
            }
            else
            {
                runtimeType1 = t1;
            }

            if (t2 is TypeBuilder)
            {
                tb2 =(TypeBuilder)t2;
                // This will be null if it is not baked.
                runtimeType2 = tb2.m_runtimeType;
            }
            else
            {
                runtimeType2 = t2;
            }
                
            // If the type builder view is eqaul then it is equal                
            if (tb1 != null && tb2 != null && tb1 == tb2)
                return true;

            // if the runtimetype view is eqaul than it is equal                
            if (runtimeType1 != null && runtimeType2 != null && runtimeType1 == runtimeType2)                
                return true;

            return false;                
        }

        internal static void SetConstantValue(Module module, int tk, Type destType, Object value)
        {
            // This is a helper function that is used by ParameterBuilder, PropertyBuilder,
            // and FieldBuilder to validate a default value and save it in the meta-data.

            if (value == null)
            {
                if (destType.IsValueType)
                    throw new ArgumentException(Environment.GetResourceString("Argument_ConstantNull"));
            }
            else
            {
                // client is going to set non-null constant value
                Type type = value.GetType();
                
                // The default value on the enum typed field/parameter is the underlying type.
                if (destType.IsEnum == false)
                {
                    if (destType != type)
                        throw new ArgumentException(Environment.GetResourceString("Argument_ConstantDoesntMatch"));
                        
                    switch(Type.GetTypeCode(type))
                    {
                        case TypeCode.Boolean:
                        case TypeCode.Char:
                        case TypeCode.SByte:
                        case TypeCode.Byte:
                        case TypeCode.Int16:
                        case TypeCode.UInt16:
                        case TypeCode.Int32:
                        case TypeCode.UInt32:
                        case TypeCode.Int64:
                        case TypeCode.UInt64:
                        case TypeCode.Single:
                        case TypeCode.Double:
                        case TypeCode.Decimal:
                        case TypeCode.String:
                            break;
                        default:
                        {
                            if (type != typeof(System.DateTime))
                                throw new ArgumentException(Environment.GetResourceString("Argument_ConstantNotSupported"));
                            break;
                        }
                   
                    }
                }
                else
                {
                    // The constant value supplied should match the underlying type of the enum 
                    if (destType.UnderlyingSystemType != type)
                        throw new ArgumentException(Environment.GetResourceString("Argument_ConstantDoesntMatch"));                    
                }
            }

            // pass the variant by reference to avoid woes with passing valuetypes into FCALLs
            Variant var = new Variant(value);

            InternalSetConstantValue(module, tk, ref var);
        }

        #endregion

        #region Private Data Members
        internal ArrayList m_ca;
        internal MethodBuilder m_currentMethod;
        private TypeToken m_tdType; 
        private ModuleBuilder m_module;
        internal String m_strName;
        private String m_strNameSpace;
        private String m_strFullQualName;
        private Type m_typeParent;
        private Type[] m_typeInterfaces;
        internal TypeAttributes m_iAttr;
        internal GenericParameterAttributes m_genParamAttributes;
        internal ArrayList m_listMethods;
        private int m_constructorCount;
        private int m_iTypeSize;
        private PackingSize m_iPackingSize;
        private TypeBuilder m_DeclaringType;
        private Type m_underlyingSystemType; // set when user calls UnderlyingSystemType on TypeBuilder if TypeBuilder is an Enum        
        internal bool m_isHiddenGlobalType;
        internal bool m_isHiddenType;
        internal bool m_hasBeenCreated;
        internal RuntimeType m_runtimeType;

        private int m_genParamPos;
        private GenericTypeParameterBuilder[] m_inst;
        private bool m_bIsGenParam;
        private bool m_bIsGenTypeDef;
        private MethodBuilder m_declMeth;
        private TypeBuilder m_genTypeDef;
        #endregion

        #region Constructor
        private TypeBuilder(TypeBuilder genTypeDef, GenericTypeParameterBuilder[] inst)
        {
            m_genTypeDef = genTypeDef;
            m_DeclaringType = genTypeDef.m_DeclaringType;
            m_typeParent = genTypeDef.m_typeParent;
            m_runtimeType = genTypeDef.m_runtimeType;
            m_tdType = genTypeDef.m_tdType;
            m_strName = genTypeDef.m_strName;
            m_bIsGenParam = false;
            m_bIsGenTypeDef = false;
            m_module = genTypeDef.m_module;
            m_inst = inst;
            m_hasBeenCreated = true;
        }

        internal TypeBuilder(string szName, int genParamPos, MethodBuilder declMeth)
        {
            m_declMeth = declMeth;
            m_DeclaringType =(TypeBuilder)m_declMeth.DeclaringType;
            m_module =(ModuleBuilder)declMeth.Module;
            InitAsGenericParam(szName, genParamPos);
        }

        private TypeBuilder(string szName, int genParamPos, TypeBuilder declType)
        {
            m_DeclaringType = declType;
            m_module =(ModuleBuilder)declType.Module;
            InitAsGenericParam(szName, genParamPos);
        }

        private void InitAsGenericParam(string szName, int genParamPos)
        {
            m_strName = szName;
            m_genParamPos = genParamPos;
            m_bIsGenParam = true;
            m_bIsGenTypeDef = false;
            m_typeInterfaces = new Type[0];
        }

        internal TypeBuilder(String name, TypeAttributes attr, Type parent, Module module, PackingSize iPackingSize,
            int iTypeSize, TypeBuilder enclosingType)
        {
            Init(name, attr, parent, null, module, iPackingSize, iTypeSize, enclosingType);
        }

        internal TypeBuilder(String name, TypeAttributes attr, Type parent, Type[] interfaces, Module module,
            PackingSize iPackingSize, TypeBuilder enclosingType)
        {
            Init(name, attr, parent, interfaces, module, iPackingSize, UnspecifiedTypeSize, enclosingType);
        }

        internal TypeBuilder(ModuleBuilder module)
        {
            m_tdType = new TypeToken(SignatureHelper.mdtTypeDef);
            m_isHiddenGlobalType = true;
            m_module =(ModuleBuilder)module;
            m_listMethods = new ArrayList();
        }

        private void Init(String fullname, TypeAttributes attr, Type parent, Type[] interfaces, Module module,
            PackingSize iPackingSize, int iTypeSize, TypeBuilder enclosingType)
        {
            int i;
            int[] interfaceTokens;
            m_bIsGenTypeDef = false;
            interfaceTokens = null;
            m_bIsGenParam = false;
            m_hasBeenCreated = false;
            m_runtimeType = null;
            m_isHiddenGlobalType = false;
            m_isHiddenType = false;
            m_module =(ModuleBuilder) module;
            m_DeclaringType = enclosingType;
            Assembly containingAssem = m_module.Assembly;
            m_underlyingSystemType = null;          // used when client use TypeBuilder to define Enum

            if (fullname == null)
                throw new ArgumentNullException("fullname");

            if (fullname.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "fullname");

            if (fullname[0] == '\0')
                throw new ArgumentException(Environment.GetResourceString("Argument_IllegalName"), "fullname");
                                                               
             
            if (fullname.Length > 1023)
                throw new ArgumentException(Environment.GetResourceString("Argument_TypeNameTooLong"), "fullname");

            // cannot have two types within the same assembly of the same name
            containingAssem.m_assemblyData.CheckTypeNameConflict(fullname, enclosingType);

            if (enclosingType != null)
            {
                // Nested Type should have nested attribute set.
                // If we are renumbering TypeAttributes' bit, we need to change the logic here.
                if (((attr & TypeAttributes.VisibilityMask) == TypeAttributes.Public) ||((attr & TypeAttributes.VisibilityMask) == TypeAttributes.NotPublic))
                    throw new ArgumentException(Environment.GetResourceString("Argument_BadNestedTypeFlags"), "attr");
            }

            if (interfaces != null)
            {
                for(i = 0; i < interfaces.Length; i++)
                {
                    if (interfaces[i] == null)
                    {
                        // cannot contain null in the interface list
                        throw new ArgumentNullException("interfaces");
                    }
                }
                interfaceTokens = new int[interfaces.Length];
                for(i = 0; i < interfaces.Length; i++)
                {
                    interfaceTokens[i] = m_module.GetTypeToken(interfaces[i]).Token;
                }
            }

            int iLast = fullname.LastIndexOf('.');
            if (iLast == -1 || iLast == 0)
            {
                // no name space
                m_strNameSpace = String.Empty;
                m_strName = fullname;
            }
            else
            {
                // split the name space
                m_strNameSpace = fullname.Substring(0, iLast);
                m_strName = fullname.Substring(iLast + 1);
            }

            VerifyTypeAttributes(attr);

            m_iAttr = attr;

            SetParent(parent);

            m_listMethods = new ArrayList();

            SetInterfaces(interfaces);

            m_constructorCount=0;

            int tkParent = 0;
            if (m_typeParent != null)
                tkParent = m_module.GetTypeToken(m_typeParent).Token;

            int tkEnclosingType = 0;
            if (enclosingType != null)
            {
                tkEnclosingType = enclosingType.m_tdType.Token;
            }

            m_tdType = new TypeToken(InternalDefineClass(
                fullname, tkParent, interfaceTokens, m_iAttr, m_module, Guid.Empty, tkEnclosingType, 0));

            m_iPackingSize = iPackingSize;
            m_iTypeSize = iTypeSize;
            if ((m_iPackingSize != 0) ||(m_iTypeSize != 0))
                InternalSetClassLayout(Module, m_tdType.Token, m_iPackingSize, m_iTypeSize);

            // If the type is public and it is contained in a assemblyBuilder,
            // update the public COMType list.
            if (IsPublicComType(this))
            {
                if (containingAssem is AssemblyBuilder)
                {
                    AssemblyBuilder assemBuilder =(AssemblyBuilder) containingAssem;
                    if (assemBuilder.IsPersistable() && m_module.IsTransient() == false)
                    {
                        assemBuilder.m_assemblyData.AddPublicComType(this);
                    }
                }
            }
        }

        #endregion

        #region Private Members
        private MethodBuilder DefinePInvokeMethodHelper(
            String name, String dllName, String importName, MethodAttributes attributes, CallingConventions callingConvention, 
            Type returnType, Type[] returnTypeRequiredCustomModifiers, Type[] returnTypeOptionalCustomModifiers,
            Type[] parameterTypes, Type[][] parameterTypeRequiredCustomModifiers, Type[][] parameterTypeOptionalCustomModifiers,
            CallingConvention nativeCallConv, CharSet nativeCharSet)
        {
            CheckContext(returnType);
            CheckContext(returnTypeRequiredCustomModifiers, returnTypeOptionalCustomModifiers, parameterTypes);
            CheckContext(parameterTypeRequiredCustomModifiers);
            CheckContext(parameterTypeOptionalCustomModifiers);
        
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    return DefinePInvokeMethodHelperNoLock(name, dllName, importName, attributes, callingConvention, 
                                                           returnType, returnTypeRequiredCustomModifiers, returnTypeOptionalCustomModifiers,
                                                           parameterTypes, parameterTypeRequiredCustomModifiers, parameterTypeOptionalCustomModifiers,
                                                           nativeCallConv, nativeCharSet);
                }
            }
            else
            {
                return DefinePInvokeMethodHelperNoLock(name, dllName, importName, attributes, callingConvention, 
                                                       returnType, returnTypeRequiredCustomModifiers, returnTypeOptionalCustomModifiers,
                                                       parameterTypes, parameterTypeRequiredCustomModifiers, parameterTypeOptionalCustomModifiers,
                                                       nativeCallConv, nativeCharSet);
            }
        }

        private MethodBuilder DefinePInvokeMethodHelperNoLock(
            String name, String dllName, String importName, MethodAttributes attributes, CallingConventions callingConvention, 
            Type returnType, Type[] returnTypeRequiredCustomModifiers, Type[] returnTypeOptionalCustomModifiers,
            Type[] parameterTypes, Type[][] parameterTypeRequiredCustomModifiers, Type[][] parameterTypeOptionalCustomModifiers,
            CallingConvention nativeCallConv, CharSet nativeCharSet)
        {
       
            ThrowIfCreated();

            if (name == null)
                throw new ArgumentNullException("name");

            if (name.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "name");

            if (dllName == null)
                throw new ArgumentNullException("dllName");

            if (dllName.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "dllName");

            if (importName == null)
                throw new ArgumentNullException("importName");

            if (importName.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "importName");

            if ((m_iAttr & TypeAttributes.ClassSemanticsMask) == TypeAttributes.Interface)
                throw new ArgumentException(Environment.GetResourceString("Argument_BadPInvokeOnInterface"));

            if ((attributes & MethodAttributes.Abstract) != 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_BadPInvokeMethod"));

             
             
             
             
            attributes = attributes | MethodAttributes.PinvokeImpl;
            MethodBuilder method = new MethodBuilder(name, attributes, callingConvention, 
                returnType, returnTypeRequiredCustomModifiers, returnTypeOptionalCustomModifiers,
                parameterTypes, parameterTypeRequiredCustomModifiers, parameterTypeOptionalCustomModifiers,
                m_module, this, false);

            int sigLength;
            byte[] sigBytes = method.GetMethodSignature().InternalGetSignature(out sigLength);

            if (m_listMethods.Contains(method))
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_MethodRedefined"));
            }
            m_listMethods.Add(method);

            MethodToken token = method.GetToken();
            
            int linkFlags = 0;
            switch(nativeCallConv)
            {
                case CallingConvention.Winapi:
                    linkFlags =(int)PInvokeMap.CallConvWinapi;
                    break;
                case CallingConvention.Cdecl:
                    linkFlags =(int)PInvokeMap.CallConvCdecl;
                    break;
                case CallingConvention.StdCall:
                    linkFlags =(int)PInvokeMap.CallConvStdcall;
                    break;
                case CallingConvention.ThisCall:
                    linkFlags =(int)PInvokeMap.CallConvThiscall;
                    break;
                case CallingConvention.FastCall:
                    linkFlags =(int)PInvokeMap.CallConvFastcall;
                    break;
            }
            switch(nativeCharSet)
            {
                case CharSet.None:
                    linkFlags |=(int)PInvokeMap.CharSetNotSpec;
                    break;
                case CharSet.Ansi:
                    linkFlags |=(int)PInvokeMap.CharSetAnsi;
                    break;
                case CharSet.Unicode:
                    linkFlags |=(int)PInvokeMap.CharSetUnicode;
                    break;
                case CharSet.Auto:
                    linkFlags |=(int)PInvokeMap.CharSetAuto;
                    break;
            }
            
            InternalSetPInvokeData(m_module,
                dllName,
                importName,
                token.Token,
                0,
                linkFlags);
            method.SetToken(token);

            return method;
        }

        private FieldBuilder DefineDataHelper(String name, byte[] data, int size, FieldAttributes attributes)
        {
            String strValueClassName;
            TypeBuilder valueClassType;
            FieldBuilder fdBuilder;
            TypeAttributes typeAttributes;

            if (name == null)
                throw new ArgumentNullException("name");

            if (name.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "name");

            if (size <= 0 || size >= 0x003f0000)
                throw new ArgumentException(Environment.GetResourceString("Argument_BadSizeForData"));

            ThrowIfCreated();

            // form the value class name
            strValueClassName = ModuleBuilderData.MULTI_BYTE_VALUE_CLASS + size;

            // Is this already defined in this module?
            Type temp = m_module.FindTypeBuilderWithName(strValueClassName, false);
            valueClassType = temp as TypeBuilder;

            if (valueClassType == null)
            {
                typeAttributes = TypeAttributes.Public | TypeAttributes.ExplicitLayout | TypeAttributes.Class | TypeAttributes.Sealed | TypeAttributes.AnsiClass;

                // Define the backing value class
                valueClassType = m_module.DefineType(strValueClassName, typeAttributes, typeof(System.ValueType), PackingSize.Size1, size);
                valueClassType.m_isHiddenType = true;
                valueClassType.CreateType();
            }

            fdBuilder = DefineField(name, valueClassType,(attributes | FieldAttributes.Static));

            // now we need to set the RVA
            fdBuilder.SetData(data, size);
            return fdBuilder;
        }

        private void VerifyTypeAttributes(TypeAttributes attr)
        {
             


//            if (((attr & TypeAttributes.Sealed) != 0) &&((attr & TypeAttributes.Abstract) != 0))
//            {
//                throw new ArgumentException(Environment.GetResourceString("Argument_BadTypeAttrAbstractNFinal"));
//            }

            // Verify attr consistency for Nesting or otherwise.
            if (DeclaringType == null)
            {
                // Not a nested class.
                if (((attr & TypeAttributes.VisibilityMask) != TypeAttributes.NotPublic) &&((attr & TypeAttributes.VisibilityMask) != TypeAttributes.Public))
                {
                    throw new ArgumentException(Environment.GetResourceString("Argument_BadTypeAttrNestedVisibilityOnNonNestedType"));
                }
            }
            else
            {
                // Nested class.
                if (((attr & TypeAttributes.VisibilityMask) == TypeAttributes.NotPublic) ||((attr & TypeAttributes.VisibilityMask) == TypeAttributes.Public))
                {
                    throw new ArgumentException(Environment.GetResourceString("Argument_BadTypeAttrNonNestedVisibilityNestedType"));
                }
            }

            // Verify that the layout mask is valid.
            if (((attr & TypeAttributes.LayoutMask) != TypeAttributes.AutoLayout) &&((attr & TypeAttributes.LayoutMask) != TypeAttributes.SequentialLayout) &&((attr & TypeAttributes.LayoutMask) != TypeAttributes.ExplicitLayout))
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_BadTypeAttrInvalidLayout"));
            }

            // Check if the user attempted to set any reserved bits.
            if ((attr & TypeAttributes.ReservedMask) != 0)
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_BadTypeAttrReservedBitsSet"));
            }
        }

        public bool IsCreated()
        { 
            return m_hasBeenCreated;
        }
        
        #endregion

        #region FCalls
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern int InternalDefineClass(String fullname, int tkParent, int[] interfaceTokens, TypeAttributes attr,
            Module module, Guid guid, int tkEnclosingType, int tkTypeDef);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern int InternalDefineGenParam(
            String name, int tkParent, int position, int attributes, int[] interfaceTokens, Module module, int tkTypeDef);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern Type TermCreateClass(int handle, Module module);
        #endregion

        #region Internal Methods
        internal void ThrowIfCreated()
        {
            if (IsCreated())
                throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_TypeHasBeenCreated"));
        }
        
        #endregion

        #region Object Overrides
        public override String ToString()
        {
                return TypeNameBuilder.ToString(this, TypeNameBuilder.Format.ToString);
        }

        #endregion

        #region MemberInfo Overrides
        public override Type DeclaringType 
        {
            get { return m_DeclaringType; }
        }

        public override Type ReflectedType 
        {
            // Return the class that was used to obtain this field.
            
            get { return m_DeclaringType; }
        }

        public override String Name 
        {
            get { return m_strName; }
        }

        public override Module Module 
        {
            get { return m_module; }
        }

        internal override int MetadataTokenInternal
        {
            get { return m_tdType.Token; }
        }

        #endregion

        #region Type Overrides
        public override Guid GUID 
        {
            get 
            {
                if (m_runtimeType == null)
                    throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

                return m_runtimeType.GUID;
            }
        }

        public override Object InvokeMember(String name, BindingFlags invokeAttr, Binder binder, Object target,
            Object[] args, ParameterModifier[] modifiers, CultureInfo culture, String[] namedParameters)
        {
            if (m_runtimeType == null)
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            return m_runtimeType.InvokeMember(name, invokeAttr, binder, target, args, modifiers, culture, namedParameters);
        }

        public override Assembly Assembly 
        {
            get { return m_module.Assembly; }
        }

        public override RuntimeTypeHandle TypeHandle 
        {
             
            get { throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule")); }
        }

        public override String FullName 
        {
            get 
            { 
                if (m_strFullQualName == null)
                    m_strFullQualName = TypeNameBuilder.ToString(this, TypeNameBuilder.Format.FullName);

                return m_strFullQualName;
            }
        }

        public override String Namespace 
        {
            get { return m_strNameSpace; }
        }

        public override String AssemblyQualifiedName 
        {
            get 
            {                
                return TypeNameBuilder.ToString(this, TypeNameBuilder.Format.AssemblyQualifiedName);
            }
        }

        public override Type BaseType 
        {
            get{ return m_typeParent; }
        }

        protected override ConstructorInfo GetConstructorImpl(BindingFlags bindingAttr,Binder binder,
                CallingConventions callConvention, Type[] types,ParameterModifier[] modifiers)
        {
            if (m_runtimeType == null)
                 
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            return m_runtimeType.GetConstructor(bindingAttr, binder, callConvention, types, modifiers);
        }

[System.Runtime.InteropServices.ComVisible(true)]
        public override ConstructorInfo[] GetConstructors(BindingFlags bindingAttr)
        {
            if (m_runtimeType == null)
                 
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            return m_runtimeType.GetConstructors(bindingAttr);
        }

        protected override MethodInfo GetMethodImpl(String name,BindingFlags bindingAttr,Binder binder,
                CallingConventions callConvention, Type[] types,ParameterModifier[] modifiers)
        {
            if (m_runtimeType == null)
                
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            if (types == null)
            {
                return m_runtimeType.GetMethod(name, bindingAttr);
            }
            else
            {
                return m_runtimeType.GetMethod(name, bindingAttr, binder, callConvention, types, modifiers);
            }
        }

        public override MethodInfo[] GetMethods(BindingFlags bindingAttr)
        {
            if (m_runtimeType == null)
                 
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            return m_runtimeType.GetMethods(bindingAttr);
        }

        public override FieldInfo GetField(String name, BindingFlags bindingAttr)
        {
            if (m_runtimeType == null)
                 
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            return m_runtimeType.GetField(name, bindingAttr);
        }

        public override FieldInfo[] GetFields(BindingFlags bindingAttr)
        {
            if (m_runtimeType == null)
                 
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            return m_runtimeType.GetFields(bindingAttr);
        }

        public override Type GetInterface(String name,bool ignoreCase)
        {
            if (m_runtimeType == null)
                 
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
            
            return m_runtimeType.GetInterface(name, ignoreCase);
        }

        public override Type[] GetInterfaces()
        {
            if (m_runtimeType != null)
            {
                return m_runtimeType.GetInterfaces();
            }

            if (m_typeInterfaces == null)
            {
                return new Type[0];
            }

            Type[] interfaces = new Type[m_typeInterfaces.Length];
            Array.Copy(m_typeInterfaces, interfaces, m_typeInterfaces.Length);
            return interfaces;
        }

        public override EventInfo GetEvent(String name,BindingFlags bindingAttr)
        {
            if (m_runtimeType == null)
               
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            return m_runtimeType.GetEvent(name, bindingAttr);
        }

        public override EventInfo[] GetEvents()
        {
            if (m_runtimeType == null)
                
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            return m_runtimeType.GetEvents();
        }

        protected override PropertyInfo GetPropertyImpl(String name, BindingFlags bindingAttr, Binder binder,
                Type returnType, Type[] types, ParameterModifier[] modifiers)
        {
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
        }

        public override PropertyInfo[] GetProperties(BindingFlags bindingAttr)
        {
            if (m_runtimeType == null)
                
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            return m_runtimeType.GetProperties(bindingAttr);
        }

        public override Type[] GetNestedTypes(BindingFlags bindingAttr)
        {
            if (m_runtimeType == null)
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            return m_runtimeType.GetNestedTypes(bindingAttr);
        }

        public override Type GetNestedType(String name, BindingFlags bindingAttr)
        {
            if (m_runtimeType == null)
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            return m_runtimeType.GetNestedType(name,bindingAttr);
        }

        public override MemberInfo[] GetMember(String name, MemberTypes type, BindingFlags bindingAttr)
        {
            if (m_runtimeType == null)
                 
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            return m_runtimeType.GetMember(name, type, bindingAttr);
        }

[System.Runtime.InteropServices.ComVisible(true)]
        public override InterfaceMapping GetInterfaceMap(Type interfaceType)
        {
            if (m_runtimeType == null)
                
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            return m_runtimeType.GetInterfaceMap(interfaceType);
        }

        public override EventInfo[] GetEvents(BindingFlags bindingAttr)
        {
            if (m_runtimeType == null)
                 
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            return m_runtimeType.GetEvents(bindingAttr);
        }

        public override MemberInfo[] GetMembers(BindingFlags bindingAttr)
        {
            if (m_runtimeType == null)
               
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            return m_runtimeType.GetMembers(bindingAttr);
        }
        
        public override bool IsAssignableFrom(Type c)
        {
            if (TypeBuilder.IsTypeEqual(c, this))
                return true;
        
            RuntimeType fromRuntimeType = c as RuntimeType;
            TypeBuilder fromTypeBuilder = c as TypeBuilder;
            
            if (fromTypeBuilder != null && fromTypeBuilder.m_runtimeType != null)
                fromRuntimeType =(RuntimeType) fromTypeBuilder.m_runtimeType;                                      
                
            if (fromRuntimeType != null)
            {
                // fromType is baked. So if this type is not baked, it cannot be assignable to!
                if (m_runtimeType == null)
                    return false;
                    
                // since toType is also baked, delegate to the base
                return((RuntimeType) m_runtimeType).IsAssignableFrom(fromRuntimeType);
            }
            
            // So if c is not a runtimeType nor TypeBuilder. We don't know how to deal with it. 
            // return false then.
            if (fromTypeBuilder == null)
                return false;
                                 
            // If fromTypeBuilder is a subclass of this class, then c can be cast to this type.
            if (fromTypeBuilder.IsSubclassOf(this))
                return true;
                
            if (this.IsInterface == false)
                return false;
                                                                                  
            // now is This type a base type on one of the interface impl?
            Type[] interfaces = fromTypeBuilder.GetInterfaces();
            for(int i = 0; i < interfaces.Length; i++)
            {
                if (TypeBuilder.IsTypeEqual(interfaces[i], this))
                    return true;
            
                if (interfaces[i].IsSubclassOf(this))
                    return true;
            }
            return false;                                                                               
        }        

        protected override TypeAttributes GetAttributeFlagsImpl()
        {
            return m_iAttr;
        }

        protected override bool IsArrayImpl()
        {
            return false;
        }
        protected override bool IsByRefImpl()
        {
            return false;
        }
        protected override bool IsPointerImpl()
        {
            return false;
        }
        protected override bool IsPrimitiveImpl()
        {
            return false;
        }

        protected override bool IsCOMObjectImpl()
        {
            return((GetAttributeFlagsImpl() & TypeAttributes.Import) != 0) ? true : false;
        }

        public override Type GetElementType()
        {
            
            // You will never have to deal with a TypeBuilder if you are just referring to arrays.
            throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));
        }

        protected override bool HasElementTypeImpl()
        {
            return false;
        }
        
        [System.Runtime.InteropServices.ComVisible(true)]
        public override bool IsSubclassOf(Type c)
        {
            Type p = this;

            if (TypeBuilder.IsTypeEqual(p, c))
                return false;

            p = p.BaseType; 
               
            while(p != null) 
            {
                if (TypeBuilder.IsTypeEqual(p, c))
                    return true;

                p = p.BaseType;
            }

            return false;
        }
        
        public override Type UnderlyingSystemType 
        {
            get 
            {
                if (m_runtimeType != null)
                    return m_runtimeType.UnderlyingSystemType;

                if (!IsEnum)
                    return this;

                if (m_underlyingSystemType == null)
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_NoUnderlyingTypeOnEnum"));

                return m_underlyingSystemType;                       
            }
        }

        public override Type MakePointerType() 
        { 
            return SymbolType.FormCompoundType("*".ToCharArray(), this, 0); 
        }

        public override Type MakeByRefType() 
        {
            return SymbolType.FormCompoundType("&".ToCharArray(), this, 0);
        }

        public override Type MakeArrayType() 
        {
            return SymbolType.FormCompoundType("[]".ToCharArray(), this, 0);
        }

        public override Type MakeArrayType(int rank) 
        {
            if (rank <= 0)
                throw new IndexOutOfRangeException();

            string szrank = "";
            if (rank == 1)
            {
                szrank = "*";
            }
            else 
            {
                for(int i = 1; i < rank; i++)
                    szrank += ",";
            }

            string s = String.Format(CultureInfo.InvariantCulture, "[{0}]", szrank); // [,,]
            return SymbolType.FormCompoundType((s).ToCharArray(), this, 0);
        }

        #endregion

        #region ICustomAttributeProvider Implementation
        public override Object[] GetCustomAttributes(bool inherit)
        {
            if (m_runtimeType == null)
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            return CustomAttribute.GetCustomAttributes(m_runtimeType, typeof(object) as RuntimeType, inherit);
        }

        public override Object[] GetCustomAttributes(Type attributeType, bool inherit)
        {
            if (m_runtimeType == null)
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            if (attributeType == null)
                throw new ArgumentNullException("attributeType");

            RuntimeType attributeRuntimeType = attributeType.UnderlyingSystemType as RuntimeType;

            if (attributeRuntimeType == null) 
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeType"),"attributeType");

            return CustomAttribute.GetCustomAttributes(m_runtimeType, attributeRuntimeType, inherit);
        }

        public override bool IsDefined(Type attributeType, bool inherit)
        {
            if (m_runtimeType == null)
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

            RuntimeType attributeRuntimeType = attributeType.UnderlyingSystemType as RuntimeType;

            if (attributeRuntimeType == null) 
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeType"),"caType");

            return CustomAttribute.IsDefined(m_runtimeType, attributeRuntimeType, inherit);
        }

        #endregion

        #region Public Member
        
        #region DefineType
        internal void ThrowIfGeneric() { if (IsGenericType && !IsGenericTypeDefinition) throw new InvalidOperationException(); }
       
        public override GenericParameterAttributes GenericParameterAttributes { get { return m_genParamAttributes; } }

        internal void SetInterfaces(params Type[] interfaces) 
        { 
            ThrowIfCreated();

            if (interfaces == null)
            {
                m_typeInterfaces = new Type[0];
            }
            else
            {
                m_typeInterfaces = new Type[interfaces.Length];
                Array.Copy(interfaces, m_typeInterfaces, interfaces.Length);
            }
        }


        public GenericTypeParameterBuilder[] DefineGenericParameters(params string[] names)
        {
            if (m_inst != null)
                throw new InvalidOperationException();

            if (names == null)
                throw new ArgumentNullException("names");
           
            for (int i = 0; i < names.Length; i ++)
                if (names[i] == null)
                    throw new ArgumentNullException("names");

            if (names.Length == 0)
                throw new ArgumentException();

            m_bIsGenTypeDef = true;
            m_inst = new GenericTypeParameterBuilder[names.Length];
            for(int i = 0; i < names.Length; i ++)
                m_inst[i] = new GenericTypeParameterBuilder(new TypeBuilder(names[i], i, this));

            return m_inst;
        }

		
		public override Type MakeGenericType(params Type[] typeArguments)
		{
            CheckContext(typeArguments);
        
            if (!IsGenericTypeDefinition)
                throw new InvalidOperationException();

            return new TypeBuilderInstantiation(this, typeArguments); 
        }
		
        public override Type[] GetGenericArguments() { return m_inst; }
        public override bool IsGenericTypeDefinition { get { return m_bIsGenTypeDef; } }
       	public override bool IsGenericType { get { return m_inst != null; } }
        public override bool IsGenericParameter { get { return m_bIsGenParam; } }
        public override int GenericParameterPosition { get { return m_genParamPos; } }
        public override MethodBase DeclaringMethod { get { return m_declMeth; } }
        public override Type GetGenericTypeDefinition() { if (IsGenericTypeDefinition) return this; if (m_genTypeDef == null) throw new InvalidOperationException(); return m_genTypeDef; }
        #endregion

        #region Define Method
        public void DefineMethodOverride(MethodInfo methodInfoBody, MethodInfo methodInfoDeclaration)
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    DefineMethodOverrideNoLock(methodInfoBody, methodInfoDeclaration);
                }
            }
            else
            {
                DefineMethodOverrideNoLock(methodInfoBody, methodInfoDeclaration);
            }
        }

        private void DefineMethodOverrideNoLock(MethodInfo methodInfoBody, MethodInfo methodInfoDeclaration)
        {
            ThrowIfGeneric();
            ThrowIfCreated();
                           
            if (methodInfoBody == null)
                throw new ArgumentNullException("methodInfoBody");

            if (methodInfoDeclaration == null)
                throw new ArgumentNullException("methodInfoDeclaration");
                                     
            if (methodInfoBody.DeclaringType != this)
                // Loader restriction: body method has to be from this class
                throw new ArgumentException(Environment.GetResourceString("ArgumentException_BadMethodImplBody"));
            
            MethodToken     tkBody;
            MethodToken     tkDecl;

            tkBody = m_module.GetMethodToken(methodInfoBody);
            tkDecl = m_module.GetMethodToken(methodInfoDeclaration);

            InternalDefineMethodImpl(m_module, m_tdType.Token, tkBody.Token, tkDecl.Token);
        }

        public MethodBuilder DefineMethod(String name, MethodAttributes attributes, Type returnType, Type[] parameterTypes)
        {
            return DefineMethod(name, attributes, CallingConventions.Standard, returnType, parameterTypes);
        }

        public MethodBuilder DefineMethod(String name, MethodAttributes attributes)
        {
            return DefineMethod(name, attributes, CallingConventions.Standard, null, null);
        }

        public MethodBuilder DefineMethod(String name, MethodAttributes attributes, CallingConventions callingConvention)
        {
            return DefineMethod(name, attributes, callingConvention, null, null);
        }

        public MethodBuilder DefineMethod(String name, MethodAttributes attributes, CallingConventions callingConvention,
            Type returnType, Type[] parameterTypes)
        {
            return DefineMethod(name, attributes, callingConvention, returnType, null, null, parameterTypes, null, null);
        }
        
        public MethodBuilder DefineMethod(String name, MethodAttributes attributes, CallingConventions callingConvention,
            Type returnType, Type[] returnTypeRequiredCustomModifiers, Type[] returnTypeOptionalCustomModifiers,
            Type[] parameterTypes, Type[][] parameterTypeRequiredCustomModifiers, Type[][] parameterTypeOptionalCustomModifiers)
        {        
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    return DefineMethodNoLock(name, attributes, callingConvention, returnType, returnTypeRequiredCustomModifiers, 
                                              returnTypeOptionalCustomModifiers, parameterTypes, parameterTypeRequiredCustomModifiers, 
                                              parameterTypeOptionalCustomModifiers);
                }
            }
            else
            {
                return DefineMethodNoLock(name, attributes, callingConvention, returnType, returnTypeRequiredCustomModifiers, 
                                          returnTypeOptionalCustomModifiers, parameterTypes, parameterTypeRequiredCustomModifiers, 
                                          parameterTypeOptionalCustomModifiers);
            }
        }
            
        private MethodBuilder DefineMethodNoLock(String name, MethodAttributes attributes, CallingConventions callingConvention,
            Type returnType, Type[] returnTypeRequiredCustomModifiers, Type[] returnTypeOptionalCustomModifiers,
            Type[] parameterTypes, Type[][] parameterTypeRequiredCustomModifiers, Type[][] parameterTypeOptionalCustomModifiers)
        {
            CheckContext(returnType);
            CheckContext(returnTypeRequiredCustomModifiers, returnTypeOptionalCustomModifiers, parameterTypes);
            CheckContext(parameterTypeRequiredCustomModifiers);
            CheckContext(parameterTypeOptionalCustomModifiers);

            if (name == null)
                throw new ArgumentNullException("name");

            if (name.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "name");

            if (parameterTypes != null)
            {
                if (parameterTypeOptionalCustomModifiers != null && parameterTypeOptionalCustomModifiers.Length != parameterTypes.Length)
                    throw new ArgumentException(Environment.GetResourceString("Argument_MismatchedArrays", "parameterTypeOptionalCustomModifiers", "parameterTypes"));

                if (parameterTypeRequiredCustomModifiers != null && parameterTypeRequiredCustomModifiers.Length != parameterTypes.Length)
                    throw new ArgumentException(Environment.GetResourceString("Argument_MismatchedArrays", "parameterTypeRequiredCustomModifiers", "parameterTypes"));
            }

            ThrowIfGeneric();
            ThrowIfCreated();

            if (!m_isHiddenGlobalType)
            {
                if (((m_iAttr & TypeAttributes.ClassSemanticsMask) == TypeAttributes.Interface) &&
                   (attributes & MethodAttributes.Abstract) == 0 &&(attributes & MethodAttributes.Static) == 0)
                    throw new ArgumentException(Environment.GetResourceString("Argument_BadAttributeOnInterfaceMethod"));               
            }

            // pass in Method attributes
            MethodBuilder method = new MethodBuilder(
                name, attributes, callingConvention, 
                returnType, returnTypeRequiredCustomModifiers, returnTypeOptionalCustomModifiers,
                parameterTypes, parameterTypeRequiredCustomModifiers, parameterTypeOptionalCustomModifiers, 
                m_module, this, false);

            if (!m_isHiddenGlobalType)
            {
                //If this method is declared to be a constructor, increment our constructor count.
                if ((method.Attributes & MethodAttributes.SpecialName) != 0 && method.Name.Equals(ConstructorInfo.ConstructorName)) 
                {
                    m_constructorCount++;
                }
            }

            m_listMethods.Add(method);

            return method;
        }

        #endregion

        #region Define Constructor
[System.Runtime.InteropServices.ComVisible(true)]
        public ConstructorBuilder DefineTypeInitializer()
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    return DefineTypeInitializerNoLock();
                }
            }
            else
            {
                return DefineTypeInitializerNoLock();
            }
        }

        private ConstructorBuilder DefineTypeInitializerNoLock()
        {
            ThrowIfGeneric();
            ThrowIfCreated();

            // change the attributes and the class constructor's name
            MethodAttributes attr = MethodAttributes.Private | MethodAttributes.Static | MethodAttributes.SpecialName;

            ConstructorBuilder constBuilder = new ConstructorBuilder(
                ConstructorInfo.TypeConstructorName, attr, CallingConventions.Standard, null, m_module, this);

            return constBuilder;
        }

[System.Runtime.InteropServices.ComVisible(true)]
        public ConstructorBuilder DefineDefaultConstructor(MethodAttributes attributes)
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    return DefineDefaultConstructorNoLock(attributes);
                }
            }
            else
            {
                return DefineDefaultConstructorNoLock(attributes);
            }
        }

        private ConstructorBuilder DefineDefaultConstructorNoLock(MethodAttributes attributes)
        {
            ThrowIfGeneric();
        
            ConstructorBuilder constBuilder;

            // get the parent class's default constructor
            // We really don't want(BindingFlags.Instance|BindingFlags.Public|BindingFlags.NonPublic) here.  We really want
            // constructors visible from the subclass, but that is not currently
            // available in BindingFlags.  This more open binding is open to
            // runtime binding failures(like if we resolve to a private
            // constructor).
            ConstructorInfo con = null;

            if (m_typeParent is TypeBuilderInstantiation)
            {
                Type genericTypeDefinition = m_typeParent.GetGenericTypeDefinition();

                if (genericTypeDefinition is TypeBuilder)
                    genericTypeDefinition = ((TypeBuilder)genericTypeDefinition).m_runtimeType;

                if (genericTypeDefinition == null)
                    throw new NotSupportedException(Environment.GetResourceString("NotSupported_DynamicModule"));

                Type inst = genericTypeDefinition.MakeGenericType(m_typeParent.GetGenericArguments());

                if (inst is TypeBuilderInstantiation)
                    con = TypeBuilder.GetConstructor(inst, genericTypeDefinition.GetConstructor(
                        BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic, null, Type.EmptyTypes, null));
                else                
                    con = inst.GetConstructor(
                        BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic, null, Type.EmptyTypes, null);
            }

            if (con == null)
            {
                con = m_typeParent.GetConstructor(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic, null, Type.EmptyTypes, null);
            }

            if (con == null)
                throw new NotSupportedException(Environment.GetResourceString("NotSupported_NoParentDefaultConstructor"));

            // Define the constructor Builder
            constBuilder = DefineConstructor(attributes, CallingConventions.Standard, null);
            m_constructorCount++;

            // generate the code to call the parent's default constructor
            ILGenerator il = constBuilder.GetILGenerator();
            il.Emit(OpCodes.Ldarg_0);
            il.Emit(OpCodes.Call,con);
            il.Emit(OpCodes.Ret);

            constBuilder.m_ReturnILGen = false;
            return constBuilder;
        }

[System.Runtime.InteropServices.ComVisible(true)]
        public ConstructorBuilder DefineConstructor(MethodAttributes attributes, CallingConventions callingConvention, Type[] parameterTypes)
        {
            return DefineConstructor(attributes, callingConvention, parameterTypes, null, null);
        }

[System.Runtime.InteropServices.ComVisible(true)]
        public ConstructorBuilder DefineConstructor(MethodAttributes attributes, CallingConventions callingConvention, 
            Type[] parameterTypes, Type[][] requiredCustomModifiers, Type[][] optionalCustomModifiers)
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    return DefineConstructorNoLock(attributes, callingConvention, parameterTypes, requiredCustomModifiers, optionalCustomModifiers);
                }
            }
            else
            {
                return DefineConstructorNoLock(attributes, callingConvention, parameterTypes, requiredCustomModifiers, optionalCustomModifiers);
            }
        }

        private ConstructorBuilder DefineConstructorNoLock(MethodAttributes attributes, CallingConventions callingConvention, 
            Type[] parameterTypes, Type[][] requiredCustomModifiers, Type[][] optionalCustomModifiers)
        {
            CheckContext(parameterTypes);
            CheckContext(requiredCustomModifiers);
            CheckContext(optionalCustomModifiers);

            ThrowIfGeneric();
            ThrowIfCreated();

            String name;

            if ((attributes & MethodAttributes.Static) == 0)
            {
                name = ConstructorInfo.ConstructorName;
            }
            else
            {
                name = ConstructorInfo.TypeConstructorName;
            }

            attributes = attributes | MethodAttributes.SpecialName;

            ConstructorBuilder constBuilder = 
                new ConstructorBuilder(name, attributes, callingConvention, 
                    parameterTypes, requiredCustomModifiers, optionalCustomModifiers, m_module, this);

            m_constructorCount++;

            return constBuilder;
        }

        #endregion

        #region Define PInvoke
        public MethodBuilder DefinePInvokeMethod(String name, String dllName, MethodAttributes attributes,
            CallingConventions callingConvention, Type returnType, Type[] parameterTypes,
            CallingConvention nativeCallConv, CharSet nativeCharSet)
        {
            ThrowIfGeneric();
            
            MethodBuilder method = DefinePInvokeMethodHelper(
                name, dllName, name, attributes, callingConvention, returnType, null, null, 
                parameterTypes, null, null, nativeCallConv, nativeCharSet);
            return method;
        }

        public MethodBuilder DefinePInvokeMethod(String name, String dllName, String entryName, MethodAttributes attributes, 
            CallingConventions callingConvention, Type returnType, Type[] parameterTypes, 
            CallingConvention nativeCallConv, CharSet nativeCharSet)
        {
            MethodBuilder method = DefinePInvokeMethodHelper(
                name, dllName, entryName, attributes, callingConvention, returnType, null, null, 
                parameterTypes, null, null, nativeCallConv, nativeCharSet);
            return method;
        }

        public MethodBuilder DefinePInvokeMethod(String name, String dllName, String entryName, MethodAttributes attributes,
            CallingConventions callingConvention, 
            Type returnType, Type[] returnTypeRequiredCustomModifiers, Type[] returnTypeOptionalCustomModifiers,
            Type[] parameterTypes, Type[][] parameterTypeRequiredCustomModifiers, Type[][] parameterTypeOptionalCustomModifiers,
            CallingConvention nativeCallConv, CharSet nativeCharSet)
        {
            ThrowIfGeneric();

            MethodBuilder method = DefinePInvokeMethodHelper(
            name, dllName, entryName, attributes, callingConvention, returnType, returnTypeRequiredCustomModifiers, returnTypeOptionalCustomModifiers, 
            parameterTypes, parameterTypeRequiredCustomModifiers, parameterTypeOptionalCustomModifiers, nativeCallConv, nativeCharSet);
            return method;
        }

        #endregion

        #region Define Nested Type
        public TypeBuilder DefineNestedType(String name)
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    return DefineNestedTypeNoLock(name);
                }
            }
            else
            {
                return DefineNestedTypeNoLock(name);
            }
        }

        private TypeBuilder DefineNestedTypeNoLock(String name)
        {
            ThrowIfGeneric();
            TypeBuilder typeBuilder;
            typeBuilder =  new TypeBuilder(name, TypeAttributes.NestedPrivate, null, null, m_module, PackingSize.Unspecified, this);
            m_module.m_TypeBuilderList.Add(typeBuilder);
            return typeBuilder;
        }

[System.Runtime.InteropServices.ComVisible(true)]
        public TypeBuilder DefineNestedType(String name, TypeAttributes attr, Type parent, Type[] interfaces)
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    return DefineNestedTypeNoLock(name, attr, parent, interfaces);
                }
            }
            else
            {
                return DefineNestedTypeNoLock(name, attr, parent, interfaces);
            }
        }

        private TypeBuilder DefineNestedTypeNoLock(String name, TypeAttributes attr, Type parent, Type[] interfaces)
        {
            CheckContext(parent);
            CheckContext(interfaces);

            ThrowIfGeneric();

            TypeBuilder typeBuilder;
            typeBuilder =  new TypeBuilder(name, attr, parent, interfaces, m_module, PackingSize.Unspecified, this);
            m_module.m_TypeBuilderList.Add(typeBuilder);
            return typeBuilder;
        }

        public TypeBuilder DefineNestedType(String name, TypeAttributes attr, Type parent)
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    return DefineNestedTypeNoLock(name, attr, parent);
                }
            }
            else
            {
                return DefineNestedTypeNoLock(name, attr, parent);
            }
        }

        private TypeBuilder DefineNestedTypeNoLock(String name, TypeAttributes attr, Type parent)
        {
            ThrowIfGeneric();
            TypeBuilder typeBuilder;
            typeBuilder =  new TypeBuilder(name, attr, parent, null, m_module, PackingSize.Unspecified, this);
            m_module.m_TypeBuilderList.Add(typeBuilder);
            return typeBuilder;
        }

        public TypeBuilder DefineNestedType(String name, TypeAttributes attr)
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    return DefineNestedTypeNoLock(name, attr);
                }
            }
            else
            {
                return DefineNestedTypeNoLock(name, attr);
            }
        }

        private TypeBuilder DefineNestedTypeNoLock(String name, TypeAttributes attr)
        {
            ThrowIfGeneric();

            TypeBuilder typeBuilder;
            typeBuilder =  new TypeBuilder(name, attr, null, null, m_module, PackingSize.Unspecified, this);
            m_module.m_TypeBuilderList.Add(typeBuilder);
            return typeBuilder;
        }

        public TypeBuilder DefineNestedType(String name, TypeAttributes attr, Type parent, int typeSize)
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    return DefineNestedTypeNoLock(name, attr, parent, typeSize);
                }
            }
            else
            {
                return DefineNestedTypeNoLock(name, attr, parent, typeSize);
            }
        }

        private TypeBuilder DefineNestedTypeNoLock(String name, TypeAttributes attr, Type parent, int typeSize)
        {
            TypeBuilder typeBuilder;
            typeBuilder = new TypeBuilder(name, attr, parent, m_module, PackingSize.Unspecified, typeSize, this);
            m_module.m_TypeBuilderList.Add(typeBuilder);
            return typeBuilder;
        }

        public TypeBuilder DefineNestedType(String name, TypeAttributes attr, Type parent, PackingSize packSize)
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    return DefineNestedTypeNoLock(name, attr, parent, packSize);
                }
            }
            else
            {
                return DefineNestedTypeNoLock(name, attr, parent, packSize);
            }
        }

        private TypeBuilder DefineNestedTypeNoLock(String name, TypeAttributes attr, Type parent, PackingSize packSize)
        {
            ThrowIfGeneric();
            
            TypeBuilder typeBuilder;
            typeBuilder = new TypeBuilder(name, attr, parent, null, m_module, packSize, this);
            m_module.m_TypeBuilderList.Add(typeBuilder);
            return typeBuilder;
        }

        #endregion

        #region Define Field
        public FieldBuilder DefineField(String fieldName, Type type, FieldAttributes attributes) 
        {
            return DefineField(fieldName, type, null, null, attributes);
        }

        public FieldBuilder DefineField(String fieldName, Type type, Type[] requiredCustomModifiers, 
            Type[] optionalCustomModifiers, FieldAttributes attributes) 
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    return DefineFieldNoLock(fieldName, type, requiredCustomModifiers, optionalCustomModifiers, attributes);
                }
            }
            else
            {
                return DefineFieldNoLock(fieldName, type, requiredCustomModifiers, optionalCustomModifiers, attributes);
            }
        }

        private FieldBuilder DefineFieldNoLock(String fieldName, Type type, Type[] requiredCustomModifiers, 
            Type[] optionalCustomModifiers, FieldAttributes attributes) 
        {
            ThrowIfGeneric();
            ThrowIfCreated();
            CheckContext(type);
            CheckContext(requiredCustomModifiers);

            if (m_underlyingSystemType == null && IsEnum == true)
            {
                if ((attributes & FieldAttributes.Static) == 0)
                {
                    // remember the underlying type for enum type
                    m_underlyingSystemType = type;
                }                   
            }

            return new FieldBuilder(this, fieldName, type, requiredCustomModifiers, optionalCustomModifiers, attributes);
        }

        public FieldBuilder DefineInitializedData(String name, byte[] data, FieldAttributes attributes)
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
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
            ThrowIfGeneric();

            if (data == null)
                throw new ArgumentNullException("data");

            return DefineDataHelper(name, data, data.Length, attributes);
        }

        public FieldBuilder DefineUninitializedData(String name, int size, FieldAttributes attributes)
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
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
            ThrowIfGeneric();
            return DefineDataHelper(name, null, size, attributes);
        }

        #endregion

        #region Define Properties and Events
        public PropertyBuilder DefineProperty(String name, PropertyAttributes attributes, Type returnType, Type[] parameterTypes)
        {
            return DefineProperty(name, attributes, returnType, null, null, parameterTypes, null, null); 
        }

        public PropertyBuilder DefineProperty(String name, PropertyAttributes attributes, 
            Type returnType, Type[] returnTypeRequiredCustomModifiers, Type[] returnTypeOptionalCustomModifiers, 
            Type[] parameterTypes, Type[][] parameterTypeRequiredCustomModifiers, Type[][] parameterTypeOptionalCustomModifiers)
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    return DefinePropertyNoLock(name, attributes, returnType, returnTypeRequiredCustomModifiers, returnTypeOptionalCustomModifiers, 
                                                parameterTypes, parameterTypeRequiredCustomModifiers, parameterTypeOptionalCustomModifiers);
                }
            }
            else
            {
                return DefinePropertyNoLock(name, attributes, returnType, returnTypeRequiredCustomModifiers, returnTypeOptionalCustomModifiers, 
                                            parameterTypes, parameterTypeRequiredCustomModifiers, parameterTypeOptionalCustomModifiers);
            }
        }

        private PropertyBuilder DefinePropertyNoLock(String name, PropertyAttributes attributes, 
            Type returnType, Type[] returnTypeRequiredCustomModifiers, Type[] returnTypeOptionalCustomModifiers, 
            Type[] parameterTypes, Type[][] parameterTypeRequiredCustomModifiers, Type[][] parameterTypeOptionalCustomModifiers)
        {
            ThrowIfGeneric();
            
            CheckContext(returnType);
            CheckContext(returnTypeRequiredCustomModifiers, returnTypeOptionalCustomModifiers, parameterTypes);
            CheckContext(parameterTypeRequiredCustomModifiers);
            CheckContext(parameterTypeOptionalCustomModifiers);

            SignatureHelper sigHelper;
            int         sigLength;
            byte[]      sigBytes;

            if (name == null)
                throw new ArgumentNullException("name");
            if (name.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "name");
            ThrowIfCreated();

            // get the signature in SignatureHelper form
            sigHelper = SignatureHelper.GetPropertySigHelper(
                m_module,
                returnType, returnTypeRequiredCustomModifiers, returnTypeOptionalCustomModifiers,
                parameterTypes, parameterTypeRequiredCustomModifiers, parameterTypeOptionalCustomModifiers);

            // get the signature in byte form
            sigBytes = sigHelper.InternalGetSignature(out sigLength);

            PropertyToken prToken = new PropertyToken(InternalDefineProperty(
                m_module,
                m_tdType.Token,
                name,
               (int) attributes,
                sigBytes,
                sigLength,
                0,
                0));

            // create the property builder now.
            return new PropertyBuilder(
                    m_module,
                    name,
                    sigHelper,
                    attributes,
                    returnType,
                    prToken,
                    this);
        }

        public EventBuilder DefineEvent(String name, EventAttributes attributes, Type eventtype)
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    return DefineEventNoLock(name, attributes, eventtype);
                }
            }
            else
            {
                return DefineEventNoLock(name, attributes, eventtype);
            }
        }

        private EventBuilder DefineEventNoLock(String name, EventAttributes attributes, Type eventtype)
        {
            int             tkType;
            EventToken      evToken;
            
            CheckContext(eventtype);

            ThrowIfGeneric();
            ThrowIfCreated();

            if (name == null)
                throw new ArgumentNullException("name");
            if (name.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_EmptyName"), "name");
            if (name[0] == '\0')
                throw new ArgumentException(Environment.GetResourceString("Argument_IllegalName"), "name");

            tkType = m_module.GetTypeToken( eventtype ).Token;

            // Internal helpers to define property records
            evToken = new EventToken(InternalDefineEvent(
                m_module,
                m_tdType.Token,
                name,
               (int) attributes,
                tkType));

            // create the property builder now.
            return new EventBuilder(
                    m_module,
                    name,
                    attributes,
                    tkType,
                    this,
                    evToken);
        }

        #endregion

        #region Create Type
        public Type CreateType()
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    return CreateTypeNoLock();
                }
            }
            else
            {
                return CreateTypeNoLock();
            }
        }

        internal void CheckContext(params Type[][] typess)
        {
            ((AssemblyBuilder)Module.Assembly).CheckContext(typess);            
        }
        internal void CheckContext(params Type[] types)
        {
            ((AssemblyBuilder)Module.Assembly).CheckContext(types);
        }

        private Type CreateTypeNoLock()
        {
            
            if (IsCreated())
                return m_runtimeType;

            ThrowIfGeneric();
            ThrowIfCreated();

            if (m_typeInterfaces == null)
                m_typeInterfaces = new Type[0];

            int[] interfaceTokens = new int[m_typeInterfaces.Length];
            for(int i = 0; i < m_typeInterfaces.Length; i++)
            {
                interfaceTokens[i] = m_module.GetTypeToken(m_typeInterfaces[i]).Token;
            }

            int tkParent = 0;
            if (m_typeParent != null)
                tkParent = m_module.GetTypeToken(m_typeParent).Token;

            if (IsGenericParameter)
            {
                int[] constraints = new int[m_typeInterfaces.Length];

                if (m_typeParent != null)
                {
                    constraints = new int[m_typeInterfaces.Length + 1];
                    constraints[constraints.Length - 1] = tkParent;
                }

                for(int i = 0; i < m_typeInterfaces.Length; i++)
                {
                    constraints[i] = m_module.GetTypeToken(m_typeInterfaces[i]).Token;
                }

                int declMember = m_declMeth == null ? m_DeclaringType.m_tdType.Token : m_declMeth.GetToken().Token;
                m_tdType = new TypeToken(InternalDefineGenParam(
                    m_strName, declMember, m_genParamPos, (int)m_genParamAttributes, constraints, m_module, 0));

                if (m_ca != null)
                {
                    foreach (CustAttr ca in m_ca)
                        ca.Bake(m_module, MetadataTokenInternal);
                }

                m_hasBeenCreated = true;
                return this;
            }
            else
            {
                // Check for global typebuilder
                if (((m_tdType.Token & 0x00FFFFFF) != 0) && ((tkParent & 0x00FFFFFF) != 0))
                    InternalSetParentType(m_tdType.Token, tkParent, m_module);
            
                if (m_inst != null)
                    foreach (Type tb in m_inst)
                        if (tb is GenericTypeParameterBuilder)
                            ((GenericTypeParameterBuilder)tb).m_type.CreateType();
            }

            byte [] body;
            MethodAttributes methodAttrs;
            int maxstack;
                            
            if (!m_isHiddenGlobalType)
            {
                // create a public default constructor if this class has no constructor.
                // except if the type is Interface, ValueType, Enum, or a static class.
                if (m_constructorCount == 0 && ((m_iAttr & TypeAttributes.Interface) == 0) && !IsValueType && ((m_iAttr & (TypeAttributes.Abstract | TypeAttributes.Sealed)) != (TypeAttributes.Abstract | TypeAttributes.Sealed)))
                {
                    DefineDefaultConstructor(MethodAttributes.Public);
                }
            }

            int size = m_listMethods.Count;

            for(int i = 0; i < size; i++)
            {
                MethodBuilder meth =(MethodBuilder)m_listMethods[i];


                if (meth.IsGenericMethodDefinition)
                    meth.GetToken(); // Doubles as "CreateMethod" for MethodBuilder -- analagous to CreateType()

                methodAttrs = meth.Attributes;

                // Any of these flags in the implemenation flags is set, we will not attach the IL method body
                if (((meth.GetMethodImplementationFlags() &(MethodImplAttributes.CodeTypeMask|MethodImplAttributes.PreserveSig|MethodImplAttributes.Unmanaged)) != MethodImplAttributes.IL) ||
                    ((methodAttrs & MethodAttributes.PinvokeImpl) !=(MethodAttributes) 0))
                {
                    continue;
                }

                int sigLength;
                byte[] LocalSig= meth.GetLocalsSignature().InternalGetSignature(out sigLength);

                 
                // Check that they haven't declared an abstract method on a non-abstract class
                if (((methodAttrs & MethodAttributes.Abstract) != 0) &&((m_iAttr & TypeAttributes.Abstract) == 0))
                {
                    throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_BadTypeAttributesNotAbstract"));
                }

                body = meth.GetBody();

                // If this is an abstract method or an interface, we don't need to set the IL.

                if ((methodAttrs & MethodAttributes.Abstract) != 0)
                {
                    // We won't check on Interface because we can have class static initializer on interface.
                    // We will just let EE or validator to catch the problem.

                    //((m_iAttr & TypeAttributes.ClassSemanticsMask) == TypeAttributes.Interface))

                    if (body != null)
                        throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_BadMethodBody"));
                }
                else if (body == null || body.Length == 0)
                {
                    // If it's not an abstract or an interface, set the IL.
                    if (meth.m_ilGenerator != null)
                    {
                        // we need to bake the method here.
                        meth.CreateMethodBodyHelper(meth.GetILGenerator());
                    }

                    body = meth.GetBody();

                    if ((body == null || body.Length == 0) && !meth.m_canBeRuntimeImpl)
                        throw new InvalidOperationException(
                            String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("InvalidOperation_BadEmptyMethodBody"), meth.Name) ); 
                }

                if (meth.m_ilGenerator != null)
                {
                    maxstack = meth.m_ilGenerator.GetMaxStackSize();
                }
                else
                {
                    // this is the case when client provide an array of IL byte stream rather than going through ILGenerator.
                    maxstack = 16;
                }

                __ExceptionInstance[] Exceptions = meth.GetExceptionInstances();
                int[] TokenFixups = meth.GetTokenFixups();
                int[] RVAFixups = meth.GetRVAFixups();


                __ExceptionInstance[] localExceptions = null;
                int[] localTokenFixups = null;
                int[] localRVAFixups = null;
                
                if (Exceptions != null)
                {
                    localExceptions = new __ExceptionInstance[Exceptions.Length];
                    Array.Copy(Exceptions, localExceptions, Exceptions.Length);
                }

                if (TokenFixups != null)
                {
                    localTokenFixups = new int[TokenFixups.Length];
                    Array.Copy(TokenFixups, localTokenFixups, TokenFixups.Length);
                }
                
                if (RVAFixups != null)
                {
                    localRVAFixups = new int[RVAFixups.Length];
                    Array.Copy(RVAFixups, localRVAFixups, RVAFixups.Length);
                }

                InternalSetMethodIL(meth.GetToken().Token, meth.InitLocals, body, LocalSig, sigLength, maxstack,
                    meth.GetNumberOfExceptions(), localExceptions, localTokenFixups, localRVAFixups, m_module);
            }

            m_hasBeenCreated = true;

            // Terminate the process.
            Type cls = TermCreateClass(m_tdType.Token, m_module);

            if (!m_isHiddenGlobalType)
            {
                m_runtimeType =(RuntimeType) cls;
              
                // if this type is a nested type, we need to invalidate the cached nested runtime type on the nesting type
                if (m_DeclaringType != null &&(RuntimeType)m_DeclaringType.m_runtimeType != null)
                {
                   ((RuntimeType)m_DeclaringType.m_runtimeType).InvalidateCachedNestedType();
                }

                return cls;
            }
            else
            {
                return null;
            }
        }

        #endregion

        #region Misc
        public int Size
        {
            get { return m_iTypeSize; }
        }
        
        public PackingSize PackingSize 
        {
            get { return m_iPackingSize; }
        }

        public void SetParent(Type parent)
        {
            ThrowIfGeneric();
            ThrowIfCreated();
            CheckContext(parent);

            if (parent != null)
            {
                m_typeParent = parent;
            }
            else
            {
                if ((m_iAttr & TypeAttributes.Interface) != TypeAttributes.Interface)
                {
                    m_typeParent = typeof(Object);
                }
                else
                {
                    if ((m_iAttr & TypeAttributes.Abstract) == 0)
                        throw new InvalidOperationException(Environment.GetResourceString("InvalidOperation_BadInterfaceNotAbstract"));

                    // there is no extends for interface class
                    m_typeParent = null;
                }
            }
        }

[System.Runtime.InteropServices.ComVisible(true)]
        public void AddInterfaceImplementation(Type interfaceType)
        {
            ThrowIfGeneric();
            
            CheckContext(interfaceType);
            
            if (interfaceType == null)
            {
                throw new ArgumentNullException("interfaceType");
            }
            ThrowIfCreated();

            TypeToken tkInterface = m_module.GetTypeToken(interfaceType);
            InternalAddInterfaceImpl(m_tdType.Token, tkInterface.Token, m_module);
        }

        public void AddDeclarativeSecurity(SecurityAction action, PermissionSet pset)
        {
            if (Module.Assembly.m_assemblyData.m_isSynchronized)
            {
                lock(Module.Assembly.m_assemblyData)
                {
                    AddDeclarativeSecurityNoLock(action, pset);
                }
            }
            else
            {
                AddDeclarativeSecurityNoLock(action, pset);
            }
        }

        private void AddDeclarativeSecurityNoLock(SecurityAction action, PermissionSet pset)
        {
            ThrowIfGeneric();

            if (pset == null)
                throw new ArgumentNullException("pset");

            if (!Enum.IsDefined(typeof(SecurityAction), action) || 
                action == SecurityAction.RequestMinimum ||
                action == SecurityAction.RequestOptional ||
                action == SecurityAction.RequestRefuse )
                throw new ArgumentOutOfRangeException("action");

            ThrowIfCreated();

            // Translate permission set into serialized format(uses standard binary serialization format).
            byte[] blob = null;
            if (!pset.IsEmpty())
                blob = pset.EncodeXml();

            // Write the blob into the metadata.
            InternalAddDeclarativeSecurity(m_module, m_tdType.Token, action, blob);
        }

        public TypeToken TypeToken 
        {
            get 
            {
                if (IsGenericParameter)
                    ThrowIfCreated();

                return m_tdType; 
            }
        }

[System.Runtime.InteropServices.ComVisible(true)]

        public void SetCustomAttribute(ConstructorInfo con, byte[] binaryAttribute)
        {
            ThrowIfGeneric();
            
            if (con == null)
                throw new ArgumentNullException("con");

            if (binaryAttribute == null)
                throw new ArgumentNullException("binaryAttribute");

            TypeBuilder.InternalCreateCustomAttribute(m_tdType.Token,((ModuleBuilder )m_module).GetConstructorToken(con).Token,
                binaryAttribute, m_module, false);
        }

        public void SetCustomAttribute(CustomAttributeBuilder customBuilder)
        {
            ThrowIfGeneric();
            
            if (customBuilder == null)
                throw new ArgumentNullException("customBuilder");

            customBuilder.CreateCustomAttribute((ModuleBuilder)m_module, m_tdType.Token);
        }

        #endregion

        #endregion

        void _TypeBuilder.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _TypeBuilder.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _TypeBuilder.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _TypeBuilder.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }
    }
}





















