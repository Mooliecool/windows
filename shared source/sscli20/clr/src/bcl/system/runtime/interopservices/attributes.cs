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
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
namespace System.Runtime.InteropServices {

    using System;
    using System.Reflection;

    [AttributeUsage(AttributeTargets.Delegate, AllowMultiple = false, Inherited = false)]
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class UnmanagedFunctionPointerAttribute : Attribute
    {
        CallingConvention m_callingConvention;
    
        public UnmanagedFunctionPointerAttribute(CallingConvention callingConvention) { m_callingConvention = callingConvention; }

        public CallingConvention CallingConvention { get { return m_callingConvention; } }
        
        public CharSet CharSet;
        public bool BestFitMapping;
        public bool ThrowOnUnmappableChar;
        public bool SetLastError;
        //public bool PreserveSig;
    }
    
    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Field | AttributeTargets.Property | AttributeTargets.Event, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class DispIdAttribute : Attribute
    {
        internal int _val;
        public DispIdAttribute(int dispId)
        {
            _val = dispId;
        }
        public int Value { get {return _val;} }
    }
    
    [Serializable()]
    [System.Runtime.InteropServices.ComVisible(true)]
    public enum ComInterfaceType 
    {
        InterfaceIsDual             = 0,
        InterfaceIsIUnknown         = 1,
        InterfaceIsIDispatch        = 2,
    }
    
    [AttributeUsage(AttributeTargets.Interface, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class InterfaceTypeAttribute : Attribute
    {
        internal ComInterfaceType _val;
        public InterfaceTypeAttribute(ComInterfaceType interfaceType)
        {
            _val = interfaceType;
        }
        public InterfaceTypeAttribute(short interfaceType)
        {
            _val = (ComInterfaceType)interfaceType;
        }
        public ComInterfaceType Value { get {return _val;} }    
    }
    
    [AttributeUsage(AttributeTargets.Class, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class ComDefaultInterfaceAttribute : Attribute
    {
        internal Type _val;

        public ComDefaultInterfaceAttribute(Type defaultInterface)
        {
            _val = defaultInterface;
        }

        public Type Value { get {return _val;} }    
    }
    
    [Serializable()]
    [System.Runtime.InteropServices.ComVisible(true)]
    public enum ClassInterfaceType 
    {
        None            = 0,
        AutoDispatch    = 1,
        AutoDual        = 2
    }
    
    [AttributeUsage(AttributeTargets.Assembly | AttributeTargets.Class, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class ClassInterfaceAttribute : Attribute
    {
        internal ClassInterfaceType _val;
        public ClassInterfaceAttribute(ClassInterfaceType classInterfaceType)
        {
            _val = classInterfaceType;
        
        }
        public ClassInterfaceAttribute(short classInterfaceType)
        {
            _val = (ClassInterfaceType)classInterfaceType;
        }
        public ClassInterfaceType Value { get {return _val;} }  
    }

    [AttributeUsage(AttributeTargets.Assembly | AttributeTargets.Interface | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Delegate | AttributeTargets.Enum | AttributeTargets.Field | AttributeTargets.Method | AttributeTargets.Property, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class ComVisibleAttribute : Attribute
    {
        internal bool _val;
        public ComVisibleAttribute(bool visibility)
        {
            _val = visibility;
        }
        public bool Value { get {return _val;} }    
    }

    [AttributeUsage(AttributeTargets.Interface, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class TypeLibImportClassAttribute : Attribute
    {
        internal String _importClassName;
        public TypeLibImportClassAttribute(Type importClass)
        {
            _importClassName = importClass.ToString();
        }
        public String Value { get {return _importClassName;} }
    }


    [Serializable]
    [System.Runtime.InteropServices.ComVisible(true)]
    public enum VarEnum
    {   
        VT_EMPTY            = 0,
        VT_NULL             = 1,
        VT_I2               = 2,
        VT_I4               = 3,
        VT_R4               = 4,
        VT_R8               = 5,
        VT_CY               = 6,
        VT_DATE             = 7,
        VT_BSTR             = 8,
        VT_ERROR            = 10,
        VT_BOOL             = 11,
        VT_VARIANT          = 12,
        VT_UNKNOWN          = 13,
        VT_DECIMAL          = 14,
        VT_I1               = 16,
        VT_UI1              = 17,
        VT_UI2              = 18,
        VT_UI4              = 19,
        VT_I8               = 20,
        VT_UI8              = 21,
        VT_INT              = 22,
        VT_UINT             = 23,
        VT_VOID             = 24,
        VT_HRESULT          = 25,
        VT_PTR              = 26,
        VT_SAFEARRAY        = 27,
        VT_CARRAY           = 28,
        VT_USERDEFINED      = 29,
        VT_LPSTR            = 30,
        VT_LPWSTR           = 31,
        VT_RECORD           = 36,
        VT_FILETIME         = 64,
        VT_BLOB             = 65,
        VT_STREAM           = 66,
        VT_STORAGE          = 67,
        VT_STREAMED_OBJECT  = 68,
        VT_STORED_OBJECT    = 69,
        VT_BLOB_OBJECT      = 70,
        VT_CF               = 71,
        VT_CLSID            = 72,
        VT_VECTOR           = 0x1000,
        VT_ARRAY            = 0x2000,
        VT_BYREF            = 0x4000
    }

    [Serializable()]
    [System.Runtime.InteropServices.ComVisible(true)]
    public enum UnmanagedType
    {
        Bool             = 0x2,         // 4 byte boolean value (true != 0, false == 0)

        I1               = 0x3,         // 1 byte signed value

        U1               = 0x4,         // 1 byte unsigned value

        I2               = 0x5,         // 2 byte signed value

        U2               = 0x6,         // 2 byte unsigned value

        I4               = 0x7,         // 4 byte signed value

        U4               = 0x8,         // 4 byte unsigned value

        I8               = 0x9,         // 8 byte signed value

        U8               = 0xa,         // 8 byte unsigned value

        R4               = 0xb,         // 4 byte floating point

        R8               = 0xc,         // 8 byte floating point

        Currency         = 0xf,         // A currency


        LPStr            = 0x14,        // Ptr to SBCS string

        LPWStr           = 0x15,        // Ptr to Unicode string

        LPTStr           = 0x16,        // Ptr to OS preferred (SBCS/Unicode) string

        ByValTStr        = 0x17,        // OS preferred (SBCS/Unicode) inline string (only valid in structs)

        IUnknown         = 0x19,        // COM IUnknown pointer. 


        Struct           = 0x1b,        // Structure


        ByValArray       = 0x1e,        // Array of fixed size (only valid in structs)

        SysInt           = 0x1f,        // Hardware natural sized signed integer

        SysUInt          = 0x20,         


        FunctionPtr      = 0x26,        // Function pointer

        AsAny            = 0x28,        // Paired with Object type and does runtime marshalling determination

        LPArray          = 0x2a,        // C style array

        LPStruct         = 0x2b,        // Pointer to a structure

        CustomMarshaler  = 0x2c,        

        Error            = 0x2d,        
    }

    [AttributeUsage(AttributeTargets.Parameter | AttributeTargets.Field | AttributeTargets.ReturnValue, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public unsafe sealed class MarshalAsAttribute : Attribute   
    { 
        internal static Attribute GetCustomAttribute(ParameterInfo parameter)
        {
            return GetCustomAttribute(parameter.MetadataToken, parameter.Member.Module);
        }

        internal static bool IsDefined(ParameterInfo parameter) 
        { 
            return GetCustomAttribute(parameter) != null; 
        }   

        internal static Attribute GetCustomAttribute(RuntimeFieldInfo field) 
        {
            return GetCustomAttribute(field.MetadataToken, field.Module); ;
        }

        internal static bool IsDefined(RuntimeFieldInfo field) 
        { 
            return GetCustomAttribute(field) != null; 
        }    

        internal static Attribute GetCustomAttribute(int token, Module scope) 
        {
            UnmanagedType unmanagedType, arraySubType;
            VarEnum safeArraySubType;
            int sizeParamIndex = 0, sizeConst = 0;
            string marshalTypeName = null, marshalCookie = null, safeArrayUserDefinedTypeName = null;
            int iidParamIndex = 0;

            ConstArray nativeType = scope.ModuleHandle.GetMetadataImport().GetFieldMarshal(token);

            if (nativeType.Length == 0)
                return null;

            MetadataImport.GetMarshalAs(nativeType,
                out unmanagedType, out safeArraySubType, out safeArrayUserDefinedTypeName, out arraySubType, out sizeParamIndex, 
                out sizeConst, out marshalTypeName, out marshalCookie, out iidParamIndex);

            Type safeArrayUserDefinedType = safeArrayUserDefinedTypeName == null || safeArrayUserDefinedTypeName.Length == 0 ? null : 
                RuntimeTypeHandle.GetTypeByNameUsingCARules(safeArrayUserDefinedTypeName, scope);
            Type marshalTypeRef = null;

            try
            {
                marshalTypeRef = marshalTypeName == null ? null : RuntimeTypeHandle.GetTypeByNameUsingCARules(marshalTypeName, scope);
            }
            catch (System.TypeLoadException)
            {
                // The user may have supplied a bad type name string causing this TypeLoadException
                // Regardless, we return the bad type name
                ASSERT.CONSISTENCY_CHECK(marshalTypeName != null);
            }

            return new MarshalAsAttribute(
                unmanagedType, safeArraySubType, safeArrayUserDefinedType, arraySubType, 
                (short)sizeParamIndex, sizeConst, marshalTypeName, marshalTypeRef, marshalCookie, iidParamIndex);
        }

        internal MarshalAsAttribute(UnmanagedType val, VarEnum safeArraySubType, Type safeArrayUserDefinedSubType, UnmanagedType arraySubType,
            short sizeParamIndex, int sizeConst, string marshalType, Type marshalTypeRef, string marshalCookie, int iidParamIndex)
        {
            _val = val;
            ArraySubType = arraySubType;
            SizeParamIndex = sizeParamIndex;
            SizeConst = sizeConst;
            MarshalType = marshalType;
            MarshalTypeRef = marshalTypeRef;
            MarshalCookie = marshalCookie;
        }

        internal UnmanagedType _val;
        public MarshalAsAttribute(UnmanagedType unmanagedType)
        {
            _val = unmanagedType;
        }
        public MarshalAsAttribute(short unmanagedType)
        {
            _val = (UnmanagedType)unmanagedType;
        }
        public UnmanagedType Value { get {return _val;} }   


        // Fields used with SubType = ByValArray and LPArray.
        // Array size =  parameter(PI) * PM + C
        public UnmanagedType      ArraySubType; 
        public short              SizeParamIndex;           // param index PI
        public int                SizeConst;                // constant C

        // Fields used with SubType = CustomMarshaler
[System.Runtime.InteropServices.ComVisible(true)]
        public String             MarshalType;              // Name of marshaler class
[System.Runtime.InteropServices.ComVisible(true)]
        public Type               MarshalTypeRef;           // Type of marshaler class
        public String             MarshalCookie;            // cookie to pass to marshaler
    }

    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Interface, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class ComImportAttribute : Attribute
    {
        internal static Attribute GetCustomAttribute(RuntimeType type) 
        { 
            if ((type.Attributes & TypeAttributes.Import) == 0)
                return null;

            return new ComImportAttribute();
        }

        internal static bool IsDefined(RuntimeType type) 
        {
            return (type.Attributes & TypeAttributes.Import) != 0; 
        }

        public ComImportAttribute()
        {
        }
    }

    [AttributeUsage(AttributeTargets.Assembly | AttributeTargets.Interface | AttributeTargets.Class | AttributeTargets.Enum | AttributeTargets.Struct | AttributeTargets.Delegate, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class GuidAttribute : Attribute
    {
        internal String _val;
        public GuidAttribute(String guid)
        {
            _val = guid;
        }
        public String Value { get {return _val;} }  
    }   

    [AttributeUsage(AttributeTargets.Method, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class PreserveSigAttribute : Attribute
    {
        internal static Attribute GetCustomAttribute(RuntimeMethodInfo method) 
        { 
            if ((method.GetMethodImplementationFlags() & MethodImplAttributes.PreserveSig) == 0)
                return null;

            return new PreserveSigAttribute();
        }
        
        internal static bool IsDefined(RuntimeMethodInfo method) 
        { 
            return (method.GetMethodImplementationFlags() & MethodImplAttributes.PreserveSig) != 0;
        }
    
        public PreserveSigAttribute()
        {
        }
    }

    [AttributeUsage(AttributeTargets.Parameter, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class InAttribute : Attribute
    {
        internal static Attribute GetCustomAttribute(ParameterInfo parameter) 
        { 
            return parameter.IsIn ? new InAttribute() : null; 
        }
        internal static bool IsDefined(ParameterInfo parameter) 
        { 
            return parameter.IsIn; 
        }
        
        public InAttribute()
        {
        }
    }

    [AttributeUsage(AttributeTargets.Parameter, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class OutAttribute : Attribute
    {
        internal static Attribute GetCustomAttribute(ParameterInfo parameter) 
        { 
            return parameter.IsOut ? new OutAttribute() : null; 
        }
        internal static bool IsDefined(ParameterInfo parameter) 
        { 
            return parameter.IsOut; 
        }   
    
        public OutAttribute()
        {
        }
    }

    [AttributeUsage(AttributeTargets.Parameter, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class OptionalAttribute : Attribute
    {
        internal static Attribute GetCustomAttribute(ParameterInfo parameter) 
        { 
            return parameter.IsOptional ? new OptionalAttribute() : null; 
        }
        internal static bool IsDefined(ParameterInfo parameter) 
        { 
            return parameter.IsOptional; 
        }
    
        public OptionalAttribute()
        {
        }
    }

    [AttributeUsage(AttributeTargets.Method, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public unsafe sealed class DllImportAttribute : Attribute
    {
        internal static Attribute GetCustomAttribute(RuntimeMethodInfo method) 
        { 
            if ((method.Attributes & MethodAttributes.PinvokeImpl) == 0)
                return null;

            MetadataImport scope = method.Module.ModuleHandle.GetMetadataImport();
            string entryPoint, dllName = null;
            int token = method.MetadataToken;
            PInvokeAttributes flags = 0;
            
            scope.GetPInvokeMap(token, out flags, out entryPoint, out dllName);

            CharSet charSet = CharSet.None;

            switch (flags & PInvokeAttributes.CharSetMask)
            {
                case PInvokeAttributes.CharSetNotSpec: charSet = CharSet.None; break;
                case PInvokeAttributes.CharSetAnsi: charSet = CharSet.Ansi; break;
                case PInvokeAttributes.CharSetUnicode: charSet = CharSet.Unicode; break;
                case PInvokeAttributes.CharSetAuto: charSet = CharSet.Auto; break;
                default: ASSERT.UNREACHABLE(); break;
            }

            CallingConvention callingConvention = CallingConvention.Cdecl;

            switch (flags & PInvokeAttributes.CallConvMask)
            {
                case PInvokeAttributes.CallConvWinapi :callingConvention = CallingConvention.Winapi; break;
                case PInvokeAttributes.CallConvCdecl :callingConvention = CallingConvention.Cdecl; break;
                case PInvokeAttributes.CallConvStdcall :callingConvention = CallingConvention.StdCall; break;
                case PInvokeAttributes.CallConvThiscall :callingConvention = CallingConvention.ThisCall; break;
                case PInvokeAttributes.CallConvFastcall :callingConvention = CallingConvention.FastCall; break;

                default: ASSERT.UNREACHABLE(); break;
            }

            bool exactSpelling = (flags & PInvokeAttributes.NoMangle) != 0;
            bool setLastError = (flags & PInvokeAttributes.SupportsLastError) != 0;
            bool bestFitMapping = (flags & PInvokeAttributes.BestFitMask) == PInvokeAttributes.BestFitEnabled;
            bool throwOnUnmappableChar = (flags & PInvokeAttributes.ThrowOnUnmappableCharMask) == PInvokeAttributes.ThrowOnUnmappableCharEnabled;
            bool preserveSig = (method.GetMethodImplementationFlags() & MethodImplAttributes.PreserveSig) != 0;

            return new DllImportAttribute(
                dllName, entryPoint, charSet, exactSpelling, setLastError, preserveSig,
                callingConvention, bestFitMapping, throwOnUnmappableChar);
        }

        internal static bool IsDefined(RuntimeMethodInfo method) 
        { 
            return (method.Attributes & MethodAttributes.PinvokeImpl) != 0; 
        }
        

        internal DllImportAttribute(
            string dllName, string entryPoint, CharSet charSet, bool exactSpelling, bool setLastError, bool preserveSig, 
            CallingConvention callingConvention, bool bestFitMapping, bool throwOnUnmappableChar)
        {
            _val = dllName;
            EntryPoint = entryPoint;
            CharSet = charSet;
            ExactSpelling = exactSpelling;
            SetLastError = setLastError;
            PreserveSig = preserveSig;
            CallingConvention = callingConvention;
            BestFitMapping = bestFitMapping;
            ThrowOnUnmappableChar = throwOnUnmappableChar;
        }

        internal String _val;

        public DllImportAttribute(String dllName)
        {
            _val = dllName;
        }
        public String Value { get {return _val;} }  

        public String             EntryPoint;
        public CharSet            CharSet;
        public bool               SetLastError;
        public bool               ExactSpelling;
        public bool               PreserveSig;
        public CallingConvention  CallingConvention;
        public bool               BestFitMapping;
        public bool               ThrowOnUnmappableChar;

    }

    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public unsafe sealed class StructLayoutAttribute : Attribute
    {
        private const int DEFAULT_PACKING_SIZE = 8;

        internal static Attribute GetCustomAttribute(Type type) 
        { 
            if (!IsDefined(type))
                return null;
        
            int pack = 0, size = 0;
            LayoutKind layoutKind = LayoutKind.Auto; 
            switch (type.Attributes & TypeAttributes.LayoutMask)
            {
                case TypeAttributes.ExplicitLayout: layoutKind = LayoutKind.Explicit; break;
                case TypeAttributes.AutoLayout: layoutKind = LayoutKind.Auto; break;
                case TypeAttributes.SequentialLayout: layoutKind = LayoutKind.Sequential; break;
                default: ASSERT.UNREACHABLE(); break;
            }

            CharSet charSet = CharSet.None;
            switch (type.Attributes & TypeAttributes.StringFormatMask)
            {
                case TypeAttributes.AnsiClass: charSet = CharSet.Ansi; break;
                case TypeAttributes.AutoClass: charSet = CharSet.Auto; break;
                case TypeAttributes.UnicodeClass: charSet = CharSet.Unicode; break;
                default: ASSERT.UNREACHABLE(); break;
            }
            type.Module.MetadataImport.GetClassLayout(type.MetadataToken, out pack, out size);

            // Metadata parameter checking should not have allowed 0 for packing size.
            // The runtime later converts a packing size of 0 to 8 so do the same here
            // because it's more useful from a user perspective. 
            if (pack == 0)
                pack = DEFAULT_PACKING_SIZE;

            return new StructLayoutAttribute(layoutKind, pack, size, charSet); 
        }

        internal static bool IsDefined(Type type) 
        { 
            if (type.IsInterface || type.HasElementType || type.IsGenericParameter)
                return false;
            
            return true; 
        }

        internal LayoutKind _val;

        internal StructLayoutAttribute(LayoutKind layoutKind, int pack, int size, CharSet charSet)
        {
            _val = layoutKind;
            Pack = pack;
            Size = size;
            CharSet = charSet;
        }

        public StructLayoutAttribute(LayoutKind layoutKind)
        {
            _val = layoutKind;
        }
        public StructLayoutAttribute(short layoutKind)
        {
            _val = (LayoutKind)layoutKind;
        }
        public LayoutKind Value { get {return _val;} }  
        public int                Pack;
        public int                Size;
        public CharSet            CharSet;
    }

    [AttributeUsage(AttributeTargets.Field, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public unsafe sealed class FieldOffsetAttribute : Attribute
    {
        internal static Attribute GetCustomAttribute(RuntimeFieldInfo field) 
        { 
            int fieldOffset;

            if (field.DeclaringType != null && 
                field.Module.MetadataImport.GetFieldOffset(field.DeclaringType.MetadataToken, field.MetadataToken, out fieldOffset))
                return new FieldOffsetAttribute(fieldOffset);            

            return null;
        }

        internal static bool IsDefined(RuntimeFieldInfo field) 
        { 
            return GetCustomAttribute(field) != null; 
        }
    
        internal int _val;
        public FieldOffsetAttribute(int offset)
        {
            _val = offset;
        }
        public int Value { get {return _val;} } 
    }


    [AttributeUsage(AttributeTargets.Assembly | AttributeTargets.Interface | AttributeTargets.Class | AttributeTargets.Struct, Inherited = false)]
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class BestFitMappingAttribute : Attribute
    {
        internal bool _bestFitMapping;

        public BestFitMappingAttribute(bool BestFitMapping)
        {
            _bestFitMapping = BestFitMapping;
        }

        public bool BestFitMapping { get {return _bestFitMapping;} }
        public bool ThrowOnUnmappableChar;
    }

    [AttributeUsage(AttributeTargets.Module, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class DefaultCharSetAttribute : Attribute
    {
        internal CharSet _CharSet;
        
        public DefaultCharSetAttribute(CharSet charSet)
        {
            _CharSet = charSet;
        }
        
        public CharSet CharSet { get {return _CharSet;} }
    }

    [Obsolete("This attribute has been deprecated.  Application Domains no longer respect Activation Context boundaries in IDispatch calls.", false)]
    [AttributeUsage(AttributeTargets.Assembly, Inherited = false)] 
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class SetWin32ContextInIDispatchAttribute : Attribute
    {
        public SetWin32ContextInIDispatchAttribute()
        {
        }
    }
}

