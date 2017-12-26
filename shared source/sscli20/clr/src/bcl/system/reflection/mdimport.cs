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

using System;
using System.Reflection;
using System.Globalization;
using System.Threading;
using System.Diagnostics;
using System.Security.Permissions;
using System.Collections;
using System.Runtime.CompilerServices;
using System.Security;
using System.Text;
using System.Reflection.Emit;
using System.Runtime.InteropServices;
using System.Configuration.Assemblies;

namespace System.Reflection
{
    [Serializable]
    internal enum CorCallingConvention : byte
    {
        Default         = 0x0,
        Vararg          = 0x5,
        Field           = 0x6,
        LocalSig        = 0x7,
        Property        = 0x8,
        Unmanaged       = 0x9,
        GenericInstance = 0x0a,  
    }

    [Serializable]
    internal enum CorElementType : byte 
    {
        End                        = 0x00,
        Void                       = 0x01,
        Boolean                    = 0x02,
        Char                       = 0x03,
        I1                         = 0x04,
        U1                         = 0x05,
        I2                         = 0x06,
        U2                         = 0x07,
        I4                         = 0x08,
        U4                         = 0x09,
        I8                         = 0x0A,
        U8                         = 0x0B,
        R4                         = 0x0C,
        R8                         = 0x0D,
        String                     = 0x0E,
        Ptr                        = 0x0F,
        ByRef                      = 0x10,
        ValueType                  = 0x11,
        Class                      = 0x12,
        Array                      = 0x14,
        TypedByRef                 = 0x16,
        I                          = 0x18,
        U                          = 0x19,
        FnPtr                      = 0x1B,
        Object                     = 0x1C,
        SzArray                    = 0x1D,
        CModReqd                   = 0x1F,
        CModOpt                    = 0x20,
        Internal                   = 0x21,
        Modifier                   = 0x40,
        Sentinel                   = 0x41,
        Pinned                     = 0x45,
    }

    [Serializable, Flags()]
    internal enum MetadataFileAttributes
    {
        ContainsMetadata = 0x0000,
        ContainsNoMetadata = 0x0001,
    }


    [Serializable, Flags()]
    internal enum ManifestResourceAttributes
    {
        VisibilityMask = 0x0007,
        Public = 0x0001,
        Private = 0x002,
    }


    [Serializable, Flags()]
    internal enum MdSigCallingConvention: byte
    {
        CallConvMask    = 0x0f,  // Calling convention is bottom 4 bits 

        Default         = 0x00,  
        C               = 0x01,
        StdCall         = 0x02,
        ThisCall        = 0x03,
        FastCall        = 0x04,
        Vararg          = 0x05,  
        Field           = 0x06,  
        LoclaSig        = 0x07,
        Property        = 0x08,
        Unmgd           = 0x09,
        GenericInst     = 0x0a,  // generic method instantiation
        
        Generic         = 0x10,  // Generic method sig with explicit number of type arguments (precedes ordinary parameter count)
        HasThis         = 0x20,  // Top bit indicates a 'this' parameter    
        ExplicitThis    = 0x40,  // This parameter is explicitly in the signature
    }


    [Serializable, Flags()]
    internal enum PInvokeAttributes
    { 
        NoMangle          = 0x0001,


        CharSetMask       = 0x0006,
        CharSetNotSpec    = 0x0000,
        CharSetAnsi       = 0x0002, 
        CharSetUnicode    = 0x0004,
        CharSetAuto       = 0x0006,
        

        BestFitUseAssem   = 0x0000,
        BestFitEnabled    = 0x0010,
        BestFitDisabled   = 0x0020,
        BestFitMask       = 0x0030,
        
        ThrowOnUnmappableCharUseAssem   = 0x0000,
        ThrowOnUnmappableCharEnabled    = 0x1000,
        ThrowOnUnmappableCharDisabled   = 0x2000,
        ThrowOnUnmappableCharMask       = 0x3000,

        SupportsLastError = 0x0040,   

        CallConvMask      = 0x0700,
        CallConvWinapi    = 0x0100,   
        CallConvCdecl     = 0x0200,
        CallConvStdcall   = 0x0300,
        CallConvThiscall  = 0x0400,   
        CallConvFastcall  = 0x0500,

        MaxValue          = 0xFFFF,
    }


    [Serializable, Flags()]
    internal enum MethodSemanticsAttributes
    {
        Setter          = 0x0001,
        Getter          = 0x0002,
        Other           = 0x0004,
        AddOn           = 0x0008,
        RemoveOn        = 0x0010,
        Fire            = 0x0020,  
    }


    [Serializable, Flags()]
    internal enum DeclSecurityAttributes
    {
        ActionMask                  =   0x001f,
        ActionNil                   =   0x0000,
        Request                     =   0x0001,
        Demand                      =   0x0002,
        Assert                      =   0x0003,
        Deny                        =   0x0004,
        PermitOnly                  =   0x0005, 
        LinktimeCheck               =   0x0006,
        InheritanceCheck            =   0x0007,
        RequestMinimum              =   0x0008,
        RequestOptional             =   0x0009, 
        RequestRefuse               =   0x000a,
        PrejitGrant                 =   0x000b, 
        PrejitDenied                =   0x000c,
        NonCasDemand                =   0x000d,
        NonCasLinkDemand            =   0x000e,
        NonCasInheritance           =   0x000f,
        MaximumValue                =   0x000f,
    }


    [Serializable]
    internal enum MetadataTokenType
    {
        Module = 0x00000000,       
        TypeRef              = 0x01000000,                 
        TypeDef              = 0x02000000,       
        FieldDef             = 0x04000000,       
        MethodDef            = 0x06000000,       
        ParamDef             = 0x08000000,       
        InterfaceImpl        = 0x09000000,       
        MemberRef            = 0x0a000000,       
        CustomAttribute      = 0x0c000000,       
        Permission           = 0x0e000000,       
        Signature            = 0x11000000,       
        Event                = 0x14000000,       
        Property             = 0x17000000,       
        ModuleRef            = 0x1a000000,       
        TypeSpec             = 0x1b000000,       
        Assembly             = 0x20000000,       
        AssemblyRef          = 0x23000000,       
        File                 = 0x26000000,       
        ExportedType         = 0x27000000,       
        ManifestResource     = 0x28000000,       
        GenericPar           = 0x2a000000,       
        MethodSpec           = 0x2b000000,       
        String               = 0x70000000,       
        Name                 = 0x71000000,       
        BaseType             = 0x72000000, 
        Invalid              = 0x7FFFFFFF, 
    }


    [Serializable]
    internal enum MetadataCodedTokenType
    {
        TypeDefOrRef,
        HasConstant,
        HasCustomAttribute,
        HasFieldMarshal,
        HasDeclSecurity,
        MemberRefParent,
        HasSemantic,
        MethodDefOrRef,
        MemberForwarded,
        Implementation,
        CustomAttributeType,
        ResolutionScope,
        TypeOrMethodDef,
    }


    [Serializable]
    internal enum MetadataTable
    {
        Module,
        TypeRef,
        TypeDef,
        FieldPtr,
        Field,
        MethodPtr,
        Method,
        ParamPtr,
        Param,
        InterfaceImpl,
        MemberRef,
        Constant,
        CustomAttribute,
        FieldMarshal,
        DeclSecurity,
        ClassLayout,
        FieldLayout,
        StandAloneSig,
        EventMap,
        EventPtr,
        Event,
        PropertyMap,
        PropertyPtr,
        Property,
        MethodSemantics,
        MethodImpl,
        ModuleRef,
        TypeSpec,
        ImplMap,
        FieldRVA,
        ENCLog,
        ENCMap,
        Assembly,
        AssemblyProcessor,
        AssemblyOS,
        AssemblyRef,
        AssemblyRefProcessor,
        AssemblyRefOS,
        File,
        ExportedType,
        ManifestResource,
        NestedClass,
        GenericParam,
        MethodSpec,
        GenericParamConstraint,
        Max,
    }


    [Serializable]
    internal enum MetadataColumnType
    {
        Module,
        TypeRef,
        TypeDef,
        FieldPtr,
        Field,
        MethodPtr,
        Method,
        ParamPtr,
        Param,
        InterfaceImpl,
        MemberRef,
        Constant,
        CustomAttribute,
        FieldMarshal,
        DeclSecurity,
        ClassLayout,
        FieldLayout,
        StandAloneSig,
        EventMap,
        EventPtr,
        Event,
        PropertyMap,
        PropertyPtr,
        Property,
        MethodSemantics,
        MethodImpl,
        ModuleRef,
        TypeSpec,
        ImplMap,
        FieldRVA,
        ENCLog,
        ENCMap,
        Assembly,
        AssemblyProcessor,
        AssemblyOS,
        AssemblyRef,
        AssemblyRefProcessor,
        AssemblyRefOS,
        File,
        ExportedType,
        ManifestResource,
        NestedClass,
        GenericParam,
        MethodSpec,
        GenericParamConstraint,
        TableIdMax              = 63,
        CodedToken              = 64,
        TypeDefOrRef,
        HasConstant,
        HasCustomAttribute,
        HasFieldMarshal,
        HasDeclSecurity,
        MemberRefParent,
        HasSemantic,
        MethodDefOrRef,
        MemberForwarded,
        Implementation,
        CustomAttributeType,
        ResolutionScope,
        TypeOrMethodDef,
        CodedTokenMax           = 95,
        Short                   = 96,
        UShort                  = 97,
        Long                    = 98,
        ULong                   = 99,
        Byte                    = 100,
        StringHeap              = 101,
        GuidHeap                = 102,
        BlobHeap                = 103,
    }


    [Serializable]
    internal enum MetadataColumn
    {
        ModuleGeneration = 0,
        ModuleName = 1,
        ModuleMvid = 2,
        ModuleEncId = 3,
        ModuleEncBaseId = 4,
        TypeRefResolutionScope = 0,
        TypeRefName = 1,
        TypeRefNamespace = 2,
        TypeDefFlags = 0,
        TypeDefName = 1,
        TypeDefNamespace = 2,
        TypeDefExtends = 3,
        TypeDefFieldList = 4,
        TypeDefMethodList = 5,
        FieldPtrField = 0,
        FieldFlags = 0,
        FieldName = 1,
        FieldSignature = 2,
        MethodPtrMethod = 0,
        MethodRVA = 0,
        MethodImplFlags = 1,
        MethodFlags = 2,
        MethodName = 3,
        MethodSignature = 4,
        MethodParamList = 5,
        ParamPtrParam = 0,
        ParamFlags = 0,
        ParamSequence = 1,
        ParamName = 2,
        InterfaceImplClass = 0,
        InterfaceImplInterface = 1,
        MemberRefClass = 0,
        MemberRefName = 1,
        MemberRefSignature = 2,
        ConstantType = 0,
        ConstantParent = 1,
        ConstantValue = 2,
        CustomAttributeParent = 0,
        CustomAttributeType = 1,
        CustomAttributeArgument = 2,
        FieldMarshalParent = 0,
        FieldMarshalNativeType = 1,
        DeclSecurityAction = 0,
        DeclSecurityParent = 1,
        DeclSecurityPermissionSet = 2,
        ClassLayoutPackingSize = 0,
        ClassLayoutClassSize = 1,
        ClassLayoutParent = 2,
        FieldLayoutOffSet = 0,
        FieldLayoutField = 1,
        StandAloneSigSignature = 0,
        EventMapParent = 0,
        EventMapEventList = 1,
        EventPtrEvent = 0,
        EventEventFlags = 0,
        EventName = 1,
        EventEventType = 2,
        PropertyMapParent = 0,
        PropertyMapPropertyList = 1,
        PropertyPtrProperty = 0,
        PropertyPropFlags = 0,
        PropertyName = 1,
        PropertyType = 2,
        MethodSemanticsSemantic = 0,
        MethodSemanticsMethod = 1,
        MethodSemanticsAssociation = 2,
        MethodImplClass = 0,
        MethodImplMethodBody = 1,
        MethodImplMethodDeclaration = 2,
        ModuleRefName = 0,
        TypeSpecSignature = 0,
        ImplMapMappingFlags = 0,
        ImplMapMemberForwarded = 1,
        ImplMapImportName = 2,
        ImplMapImportScope = 3,
        FieldRVARVA = 0,
        FieldRVAField = 1,
        ENCLogToken = 0,
        ENCLogFuncCode = 1,
        ENCMapToken = 0,
        AssemblyHashAlgId = 0,
        AssemblyMajorVersion = 1,
        AssemblyMinorVersion = 2,
        AssemblyBuildNumber = 3,
        AssemblyRevisionNumber = 4,
        AssemblyFlags = 5,
        AssemblyPublicKey = 6,
        AssemblyName = 7,
        AssemblyLocale = 8,
        AssemblyProcessorProcessor = 0,
        AssemblyOSOSPlatformId = 0,
        AssemblyOSOSMajorVersion = 1,
        AssemblyOSOSMinorVersion = 2,
        AssemblyRefMajorVersion = 0,
        AssemblyRefMinorVersion = 1,
        AssemblyRefBuildNumber = 2,
        AssemblyRefRevisionNumber = 3,
        AssemblyRefFlags = 4,
        AssemblyRefPublicKeyOrToken = 5,
        AssemblyRefName = 6,
        AssemblyRefLocale = 7,
        AssemblyRefHashValue = 8,
        AssemblyRefProcessorProcessor = 0,
        AssemblyRefProcessorAssemblyRef = 1,
        AssemblyRefOSOSPlatformId = 0,
        AssemblyRefOSOSMajorVersion = 1,
        AssemblyRefOSOSMinorVersion = 2,
        AssemblyRefOSAssemblyRef = 3,
        FileFlags = 0,
        FileName = 1,
        FileHashValue = 2,
        ExportedTypeFlags = 0,
        ExportedTypeTypeDefId = 1,
        ExportedTypeTypeName = 2,
        ExportedTypeTypeNamespace = 3,
        ExportedTypeImplementation = 4,
        ManifestResourceOffset = 0,
        ManifestResourceFlags = 1,
        ManifestResourceName = 2,
        ManifestResourceImplementation = 3,
        NestedClassNestedClass = 0,
        NestedClassEnclosingClass = 1,
        GenericParamNumber = 0,
        GenericParamFlags = 1,
        GenericParamOwner = 2,
        GenericParamName = 3,
        GenericParamKind = 4,
        GenericParamDeprecatedConstraint = 5,
        MethodSpecMethod = 0,
        MethodSpecInstantiation = 1,
        GenericParamConstraintOwner = 0,
        GenericParamConstraintConstraint = 1,
    }


    [Serializable()]    
    internal struct ConstArray
    {
        public IntPtr Signature { get { return m_constArray; } }
        public int Length { get { return m_length; } }
        public byte this[int index]
        {
            get
            {
                if (index < 0 || index > m_length)
                    throw new IndexOutOfRangeException();

                unsafe 
                {
                    return ((byte*)m_constArray.ToPointer())[index];
                }
            }
        }

        internal int m_length;
        internal IntPtr m_constArray;
    }
    
    [Serializable]
    internal struct MetadataToken
    {
        #region Implicit Cast Operators
        public static implicit operator int(MetadataToken token) { return token.Value; }
        public static implicit operator MetadataToken(int token) { return new MetadataToken(token); }
        #endregion

        #region Public Static Members
        public static bool IsTokenOfType(int token, params MetadataTokenType[] types)
        {
            for (int i = 0; i < types.Length; i++)
            {
                if ((int)(token & 0xFF000000) == (int)types[i])
                    return true;
            }

            return false;
        }

        public static bool IsNullToken(int token) 
        { 
            return (token & 0x00FFFFFF) == 0; 
        }
        #endregion

        #region Public Data Members
        public int Value;
        #endregion

        #region Constructor
        public MetadataToken(int token) { Value = token; } 
        #endregion
        
        #region Public Members
        public bool IsGlobalTypeDefToken {  get { return (Value == 0x02000001); } }
        public MetadataTokenType TokenType { get { return (MetadataTokenType)(Value & 0xFF000000); } }
        public bool IsTypeRef { get { return TokenType == MetadataTokenType.TypeRef; } }
        public bool IsTypeDef { get { return TokenType == MetadataTokenType.TypeDef; } }
        public bool IsFieldDef { get { return TokenType == MetadataTokenType.FieldDef; } }
        public bool IsMethodDef { get { return TokenType == MetadataTokenType.MethodDef; } }
        public bool IsMemberRef { get { return TokenType == MetadataTokenType.MemberRef; } }
        public bool IsEvent { get { return TokenType == MetadataTokenType.Event; } }
        public bool IsProperty { get { return TokenType == MetadataTokenType.Property; } }
        public bool IsParamDef { get { return TokenType == MetadataTokenType.ParamDef; } }
        public bool IsTypeSpec { get { return TokenType == MetadataTokenType.TypeSpec; } }
        public bool IsMethodSpec { get { return TokenType == MetadataTokenType.MethodSpec; } }
        public bool IsString { get { return TokenType == MetadataTokenType.String; } }
        public bool IsSignature { get { return TokenType == MetadataTokenType.Signature; } }
        #endregion

        #region Object Overrides
        public override string ToString() { return String.Format(CultureInfo.InvariantCulture, "0x{0:x8}", Value); }
        #endregion
    }


    [Serializable]
    internal struct MetadataFieldOffset
    {
        public int FieldToken;
        public int Offset;
    }


    [Serializable]
    internal static class MetadataArgs
    {
        #region Public Static Members
        [Serializable, StructLayout(LayoutKind.Auto)]
[System.Runtime.InteropServices.ComVisible(true)]
        public struct SkipAddresses
        {
            public string String;
            public int[] Int32Array;
            public byte[] ByteArray;
            public MetadataFieldOffset[] MetadataFieldOffsetArray;
            public int Int32;
            public TypeAttributes TypeAttributes;
            public MethodAttributes MethodAttributes;
            public PropertyAttributes PropertyAttributes;
            public MethodImplAttributes MethodImplAttributes;
            public ParameterAttributes ParameterAttributes;
            public FieldAttributes FieldAttributes;
            public EventAttributes EventAttributes;
            public MetadataColumnType MetadataColumnType;
            public PInvokeAttributes PInvokeAttributes;
            public MethodSemanticsAttributes MethodSemanticsAttributes;
            public DeclSecurityAttributes DeclSecurityAttributes;
            public CorElementType CorElementType;
            public ConstArray ConstArray;
            public Guid Guid;
        }
        public static SkipAddresses Skip = new SkipAddresses();
        #endregion
    }

    internal struct MetadataImport
    {
        #region Private Data Members
        private IntPtr m_metadataImport2;
        #endregion

        #region Make the struct work kind of crap
        internal static readonly MetadataImport EmptyImport = new MetadataImport((IntPtr)0);
                
        public override int GetHashCode()
        {
            return m_metadataImport2.GetHashCode();
        }

        public override bool Equals(object obj)
        {
            if(!(obj is MetadataImport))
                return false;
            return Equals((MetadataImport)obj);
        }
        
        internal bool Equals(MetadataImport import)
        {
            return import.m_metadataImport2 == m_metadataImport2;
        }

        #endregion

        #region Static Members
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _GetMarshalAs(IntPtr pNativeType, int cNativeType, out int unmanagedType, out int safeArraySubType, out string safeArrayUserDefinedSubType, 
            out int arraySubType, out int sizeParamIndex, out int sizeConst, out string marshalType, out string marshalCookie, 
            out int iidParamIndex);
        internal static void GetMarshalAs(ConstArray nativeType, 
            out UnmanagedType unmanagedType, out VarEnum safeArraySubType, out string safeArrayUserDefinedSubType, 
            out UnmanagedType arraySubType, out int sizeParamIndex, out int sizeConst, out string marshalType, out string marshalCookie, 
            out int iidParamIndex)
        {
            int _unmanagedType, _safeArraySubType, _arraySubType;

            _GetMarshalAs(nativeType.Signature, (int)nativeType.Length,
                out _unmanagedType, out _safeArraySubType, out safeArrayUserDefinedSubType, 
                out _arraySubType, out sizeParamIndex, out sizeConst, out marshalType, out marshalCookie, 
                out iidParamIndex);

            unmanagedType = (UnmanagedType)_unmanagedType;
            safeArraySubType = (VarEnum)_safeArraySubType;
            arraySubType = (UnmanagedType)_arraySubType;
        }
        #endregion

        #region Internal Static Members
        internal static Guid IID_IMetaDataImport = new Guid(0xd26df2ea, 0x7f58, 0x4183, 0x86, 0xbe, 0x30, 0xae, 0x29, 0xa7, 0x5d, 0x8d);
        internal static Guid IID_IMetaDataAssemblyImport = new Guid(0xee62470b, 0xe94b, 0x424e, 0x9b, 0x7c, 0x2f, 0x0, 0xc9, 0x24, 0x9f, 0x93);
        internal static Guid IID_IMetaDataTables = new Guid(0xd8f579ab, 0x402d, 0x4b8e, 0x82, 0xd9, 0x5d, 0x63, 0xb1, 0x6, 0x5c, 0x68);

        internal static void ThrowError(int hResult)
        { 
            throw new MetadataException(hResult); 
        }
        #endregion

        #region Constructor
        internal MetadataImport(IntPtr metadataImport2)
        { 
            m_metadataImport2 = metadataImport2; 
        }
        #endregion

        #region FCalls
        [MethodImplAttribute (MethodImplOptions.InternalCall)]
        private unsafe static extern void _Enum(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, int type, int parent, int* result, int count);
        [MethodImplAttribute (MethodImplOptions.InternalCall)]
        private static extern int _EnumCount(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, int type, int parent, out int count);

        public unsafe void Enum(int type, int parent, int* result, int count) 
        { 
            _Enum(m_metadataImport2, out MetadataArgs.Skip, type, parent, result, count);
        }
        public int EnumCount(int type, int parent) 
        { 
            int count = 0;
            _EnumCount(m_metadataImport2, out MetadataArgs.Skip, type, parent, out count);
            return count;
        }

        public unsafe void EnumNestedTypes(int mdTypeDef, int* result, int count) 
        {
            Enum((int)MetadataTokenType.TypeDef, mdTypeDef, result, count);
        }
        public int EnumNestedTypesCount(int mdTypeDef) 
        {
            return EnumCount((int)MetadataTokenType.TypeDef, mdTypeDef);
        }

        public unsafe void EnumCustomAttributes(int mdToken, int* result, int count) 
        {
            Enum((int)MetadataTokenType.CustomAttribute, mdToken, result, count);
        }
        public int EnumCustomAttributesCount(int mdToken) 
        {
            return EnumCount((int)MetadataTokenType.CustomAttribute, mdToken);
        }

        public unsafe void EnumParams(int mdMethodDef, int* result, int count) 
        {
            Enum((int)MetadataTokenType.ParamDef, mdMethodDef, result, count);
        }
        public int EnumParamsCount(int mdMethodDef) 
        {
            return EnumCount((int)MetadataTokenType.ParamDef, mdMethodDef);
        }

        public unsafe void GetAssociates(int mdPropEvent, AssociateRecord* result, int count) 
        {
            int* temp = stackalloc int[count*2];
            Enum((int)MetadataTokenType.MethodDef, mdPropEvent, temp, count);
            for (int i = 0; i < count; i++)
            {
                result[i].MethodDefToken = temp[i*2];
                result[i].Semantics = (MethodSemanticsAttributes)temp[i * 2 + 1];
            }
        }
        public int GetAssociatesCount(int mdPropEvent) 
        {
            return EnumCount((int)MetadataTokenType.MethodDef, mdPropEvent);
        }

        public unsafe void EnumFields(int mdTypeDef, int* result, int count) 
        { 
            Enum((int)MetadataTokenType.FieldDef, mdTypeDef, result, count); 
        }
        public int EnumFieldsCount(int mdTypeDef) 
        { 
            return EnumCount((int)MetadataTokenType.FieldDef, mdTypeDef); 
        }

        public unsafe void EnumProperties(int mdTypeDef, int* result, int count) 
        { 
            Enum((int)MetadataTokenType.Property, mdTypeDef, result, count); 
        }
        public int EnumPropertiesCount(int mdTypeDef) 
        { 
            return EnumCount((int)MetadataTokenType.Property, mdTypeDef); 
        }

        public unsafe void EnumEvents(int mdTypeDef, int* result, int count) 
        { 
            Enum((int)MetadataTokenType.Event, mdTypeDef, result, count); 
        }
        public int EnumEventsCount(int mdTypeDef) 
        { 
            return EnumCount((int)MetadataTokenType.Event, mdTypeDef); 
        }
        
        [MethodImplAttribute (MethodImplOptions.InternalCall)]
        private static extern void _GetDefaultValue(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, int mdToken, out long value, out int length, out int corElementType);
        public void GetDefaultValue(int mdToken, out long value, out int length, out CorElementType corElementType) 
        { 
            int _corElementType; 
            _GetDefaultValue(m_metadataImport2, out MetadataArgs.Skip, mdToken, out value, out length, out _corElementType); 
            corElementType = (CorElementType)_corElementType;
        }
        
        [MethodImplAttribute (MethodImplOptions.InternalCall)]
        private static unsafe extern void _GetUserString(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, int mdToken, void** name, out int length);
        public unsafe String GetUserString(int mdToken) 
        { 
            void* name;
            int length;
            _GetUserString(m_metadataImport2, out MetadataArgs.Skip, mdToken, &name, out length); 

            if (name == null)
                return null;

            char[] c = new char[length];
            for (int i = 0; i < length; i ++)
            {
#if ALIGN_ACCESS
                c[i] = (char)Marshal.ReadInt16( (IntPtr) (((char*)name) + i) );
#else
                c[i] = ((char*)name)[i];
#endif
            }

            return new String(c);
        }
        
        [MethodImplAttribute (MethodImplOptions.InternalCall)]
        private static unsafe extern void _GetName(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, int mdToken, void** name);
        public unsafe Utf8String GetName(int mdToken) 
        { 
            void* name;
            _GetName(m_metadataImport2, out MetadataArgs.Skip, mdToken, &name); 
            
            return new Utf8String(name);
        }
        
        [MethodImplAttribute (MethodImplOptions.InternalCall)]
        private static unsafe extern void _GetNamespace(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, int mdToken, void** namesp);
        public unsafe Utf8String GetNamespace(int mdToken) 
        { 
            void* namesp;
            _GetNamespace(m_metadataImport2, out MetadataArgs.Skip, mdToken, &namesp);
            
            return new Utf8String(namesp);
        }

        [MethodImplAttribute (MethodImplOptions.InternalCall)]
        private unsafe static extern void _GetEventProps(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, int mdToken, void** name, out int eventAttributes);
        public unsafe void GetEventProps(int mdToken, out void* name, out EventAttributes eventAttributes) 
        { 
            int _eventAttributes; 
            void* _name;
            _GetEventProps(m_metadataImport2, out MetadataArgs.Skip, mdToken, &_name, out _eventAttributes);
            name = _name;
            eventAttributes = (EventAttributes)_eventAttributes;
        }        
        
        [MethodImplAttribute (MethodImplOptions.InternalCall)]
        private static extern void _GetFieldDefProps(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, int mdToken, out int fieldAttributes);
        public void GetFieldDefProps(int mdToken, out FieldAttributes fieldAttributes) 
        { 
            int _fieldAttributes; 
            _GetFieldDefProps(m_metadataImport2, out MetadataArgs.Skip, mdToken, out _fieldAttributes);
            fieldAttributes = (FieldAttributes)_fieldAttributes;
        }        
        
        [MethodImplAttribute (MethodImplOptions.InternalCall)]
        private unsafe static extern void _GetPropertyProps(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, 
            int mdToken, void** name, out int propertyAttributes, out ConstArray signature);
        public unsafe void GetPropertyProps(int mdToken, out void* name, out PropertyAttributes propertyAttributes, out ConstArray signature) 
        { 
            int _propertyAttributes; 
            void* _name;
            _GetPropertyProps(m_metadataImport2, out MetadataArgs.Skip, mdToken, &_name, out _propertyAttributes, out signature); 
            name = _name;
            propertyAttributes = (PropertyAttributes)_propertyAttributes;
        }

        [MethodImplAttribute (MethodImplOptions.InternalCall)]
        private static extern void _GetParentToken(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, 
            int mdToken, out int tkParent);
        public int GetParentToken(int tkToken) 
        { 
            int tkParent;
            _GetParentToken(m_metadataImport2, out MetadataArgs.Skip, tkToken, out tkParent); 
            return tkParent;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _GetParamDefProps(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, 
            int parameterToken, out int sequence, out int attributes);
        public void GetParamDefProps(int parameterToken, out int sequence, out ParameterAttributes attributes)
        {
            int _attributes;

            _GetParamDefProps(m_metadataImport2, out MetadataArgs.Skip, parameterToken, out sequence, out _attributes);
            
            attributes = (ParameterAttributes)_attributes;
        }      

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _GetGenericParamProps(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, 
            int genericParameter, 
            out int flags);
        
        public void GetGenericParamProps(
            int genericParameter, 
            out GenericParameterAttributes attributes)
        {
            int _attributes;
            _GetGenericParamProps(m_metadataImport2, out MetadataArgs.Skip, genericParameter, out _attributes);
            attributes = (GenericParameterAttributes)_attributes;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _GetScopeProps(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, 
            out Guid mvid);
        
        public void GetScopeProps(
            out Guid mvid)
        {
            _GetScopeProps(m_metadataImport2, out MetadataArgs.Skip, out mvid);
        }


        public ConstArray GetMethodSignature(MetadataToken token)
        {
            if (token.IsMemberRef)
                return GetMemberRefProps(token);
                
            return GetSigOfMethodDef(token);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _GetSigOfMethodDef(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, 
            int methodToken, 
            ref ConstArray signature);
        
        public ConstArray GetSigOfMethodDef(int methodToken)
        {
            ConstArray signature = new ConstArray();

            _GetSigOfMethodDef(m_metadataImport2, out MetadataArgs.Skip, methodToken, ref signature);

            return signature;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _GetSignatureFromToken(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, 
            int methodToken, 
            ref ConstArray signature);
        
        public ConstArray GetSignatureFromToken(int token)
        {
            ConstArray signature = new ConstArray();

            _GetSignatureFromToken(m_metadataImport2, out MetadataArgs.Skip, token, ref signature);

            return signature;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _GetMemberRefProps(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, 
            int memberTokenRef, 
            out ConstArray signature);
        
        public ConstArray GetMemberRefProps(int memberTokenRef)
        {
            ConstArray signature = new ConstArray();
            
            _GetMemberRefProps(m_metadataImport2, out MetadataArgs.Skip, memberTokenRef, out signature);

            return signature;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _GetCustomAttributeProps(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, 
            int customAttributeToken, 
            out int constructorToken, 
            out ConstArray signature);
        
        public void GetCustomAttributeProps( 
            int customAttributeToken, 
            out int constructorToken, 
            out ConstArray signature)
        {
            _GetCustomAttributeProps(m_metadataImport2, out MetadataArgs.Skip, customAttributeToken, 
                out constructorToken, out signature);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _GetClassLayout(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, 
            int typeTokenDef, out int packSize, out int classSize);
        public void GetClassLayout(
            int typeTokenDef, 
            out int packSize, 
            out int classSize)
        {
            _GetClassLayout(m_metadataImport2, out MetadataArgs.Skip, typeTokenDef, out packSize, out classSize);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern bool _GetFieldOffset(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, 
            int typeTokenDef, int fieldTokenDef, out int offset);
        public bool GetFieldOffset(
            int typeTokenDef, 
            int fieldTokenDef, 
            out int offset)
        {
            return _GetFieldOffset(m_metadataImport2, out MetadataArgs.Skip, typeTokenDef, fieldTokenDef, out offset);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _GetSigOfFieldDef(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, 
            int fieldToken, 
            ref ConstArray fieldMarshal);
        
        public ConstArray GetSigOfFieldDef(int fieldToken)
        {
            ConstArray fieldMarshal = new ConstArray();

            _GetSigOfFieldDef(m_metadataImport2, out MetadataArgs.Skip, fieldToken, ref fieldMarshal);

            return fieldMarshal;
        }
  
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void _GetFieldMarshal(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, 
            int fieldToken, 
            ref ConstArray fieldMarshal);
        
        public ConstArray GetFieldMarshal(int fieldToken)
        {
            ConstArray fieldMarshal = new ConstArray();

            _GetFieldMarshal(m_metadataImport2, out MetadataArgs.Skip, fieldToken, ref fieldMarshal);

            return fieldMarshal;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private unsafe static extern void _GetPInvokeMap(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, 
            int token, 
            out int attributes, 
            void**  importName, 
            void**  importDll);
        
        public unsafe void GetPInvokeMap(
            int token, 
            out PInvokeAttributes attributes, 
            out String importName, 
            out String importDll)
        {
            int _attributes;
            void* _importName, _importDll;
            _GetPInvokeMap(m_metadataImport2, out MetadataArgs.Skip, token, out _attributes, &_importName, &_importDll);
            importName = new Utf8String(_importName).ToString();
            importDll = new Utf8String(_importDll).ToString();

            attributes = (PInvokeAttributes)_attributes;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern bool _IsValidToken(IntPtr scope, out MetadataArgs.SkipAddresses skipAddresses, int token);        
        public bool IsValidToken(int token) 
        { 
            return _IsValidToken(m_metadataImport2, out MetadataArgs.Skip, token); 
        }
        #endregion
    }
    

    internal struct AssociateRecord
    {
        internal int MethodDefToken;
        internal MethodSemanticsAttributes Semantics;
    }


    [StructLayout(LayoutKind.Sequential)]
    internal unsafe struct AssemblyMetadata
    {
        public short MajorVersion;          // Major Version.   
        public short MinorVersion;          // Minor Version.
        public short uuildNumber;           // Build Number.
        public short RevisionNumber;        // Revision Number.
        public IntPtr Locale;                 // Locale.
        public IntPtr Processor;                // Processor array.
        public int ulProcessor;                // [IN/OUT] Size of the processor array/Actual # of entries filled in.
        public IntPtr rOS;                      // OSINFO array.
        public int ulOS;                       // [IN/OUT]Size of the OSINFO array/Actual # of entries filled in.
    }

    internal class MetadataException : Exception
    {
        private int m_hr;
        internal MetadataException(int hr) { m_hr = hr; }
        
        public override string ToString() 
        { 
            return String.Format(CultureInfo.CurrentCulture, "MetadataException HResult = {0:x}.", m_hr); 
        }
    }
}


