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
    using System.Text;
    using System;
    using System.Reflection;
    using System.Runtime.CompilerServices;
    using System.Runtime.InteropServices;
    using System.Security.Permissions;
    
    [ClassInterface(ClassInterfaceType.None)]
    [ComDefaultInterface(typeof(_SignatureHelper))]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class SignatureHelper : _SignatureHelper
    {
        #region Consts Fields
        internal const int mdtTypeRef = 0x01000000;
        internal const int mdtTypeDef = 0x02000000;
        internal const int mdtTypeSpec = 0x21000000;
    
        #region ElementType
        //The following fields are duplicated from cor.h and must be kept in sync
        internal const byte ELEMENT_TYPE_END            = 0x0;
        internal const byte ELEMENT_TYPE_VOID           = 0x1;
        internal const byte ELEMENT_TYPE_BOOLEAN        = 0x2;  
        internal const byte ELEMENT_TYPE_CHAR           = 0x3;  
        internal const byte ELEMENT_TYPE_I1             = 0x4;  
        internal const byte ELEMENT_TYPE_U1             = 0x5; 
        internal const byte ELEMENT_TYPE_I2             = 0x6;  
        internal const byte ELEMENT_TYPE_U2             = 0x7;  
        internal const byte ELEMENT_TYPE_I4             = 0x8;  
        internal const byte ELEMENT_TYPE_U4             = 0x9;  
        internal const byte ELEMENT_TYPE_I8             = 0xa;  
        internal const byte ELEMENT_TYPE_U8             = 0xb;  
        internal const byte ELEMENT_TYPE_R4             = 0xc;  
        internal const byte ELEMENT_TYPE_R8             = 0xd;  
        internal const byte ELEMENT_TYPE_STRING         = 0xe;  
        
        internal const byte ELEMENT_TYPE_PTR            = 0x0f;     // PTR <type> 
        internal const byte ELEMENT_TYPE_BYREF          = 0x10;     // BYREF <type> 
        
        internal const byte ELEMENT_TYPE_VALUETYPE      = 0x11;     // VALUETYPE <class Token> 
        internal const byte ELEMENT_TYPE_CLASS          = 0x12;     // CLASS <class Token>  
        internal const byte ELEMENT_TYPE_VAR            = 0x13;     // VAR <U1>
        
        internal const byte ELEMENT_TYPE_ARRAY          = 0x14;     // MDARRAY <type> <rank> <bcount> <bound1> ... <lbcount> <lb1> ...  
        internal const byte ELEMENT_TYPE_GENERICINST           = 0x15;     // WITH <type> <ntypars> <type1> ... <typen> 

        internal const byte ELEMENT_TYPE_TYPEDBYREF     = 0x16;     // This is a simple type.       

        internal const byte ELEMENT_TYPE_I              = 0x18;     // native-pointer-sized integer.
        internal const byte ELEMENT_TYPE_U              = 0x19;     // native-pointer-sized unsigned integer.
        internal const byte ELEMENT_TYPE_FNPTR          = 0x1B;     // FNPTR <complete sig for the function including calling convention>
        internal const byte ELEMENT_TYPE_OBJECT         = 0x1C;     // Shortcut for System.Object
        internal const byte ELEMENT_TYPE_SZARRAY        = 0x1D;     // SZARRAY <type> : Shortcut for single dimension zero lower bound array
        internal const byte ELEMENT_TYPE_MVAR           = 0x1E;     // MVAR <U1>
               
        internal const byte ELEMENT_TYPE_CMOD_REQD      = 0x1F;     // required C modifier : E_T_CMOD_REQD <mdTypeRef/mdTypeDef>
        internal const byte ELEMENT_TYPE_CMOD_OPT       = 0x20;     // optional C modifier : E_T_CMOD_OPT <mdTypeRef/mdTypeDef>
        internal const byte ELEMENT_TYPE_INTERNAL       = 0x21;

        internal const byte ELEMENT_TYPE_MAX            = 0x22;     // first invalid element type   

        internal const byte ELEMENT_TYPE_SENTINEL       = 0x41;     // SENTINEL for vararg
        internal const byte ELEMENT_TYPE_PINNED         = 0x45;
        #endregion

        #region Unmanged Calling Convetions
        internal const int IMAGE_CEE_UNMANAGED_CALLCONV_C = 0x1;  
        internal const int IMAGE_CEE_UNMANAGED_CALLCONV_STDCALL  = 0x2;  
        internal const int IMAGE_CEE_UNMANAGED_CALLCONV_THISCALL = 0x3;
        internal const int IMAGE_CEE_UNMANAGED_CALLCONV_FASTCALL = 0x4;
        #endregion
        
        #region Managed Calling Conventions
        internal const int IMAGE_CEE_CS_CALLCONV_DEFAULT        = 0x0;  
        internal const int IMAGE_CEE_CS_CALLCONV_VARARG         = 0x5;  
        internal const int IMAGE_CEE_CS_CALLCONV_FIELD          = 0x6;  
        internal const int IMAGE_CEE_CS_CALLCONV_LOCAL_SIG      = 0x7;
        internal const int IMAGE_CEE_CS_CALLCONV_PROPERTY       = 0x8;
        internal const int IMAGE_CEE_CS_CALLCONV_UNMGD          = 0x9;
        internal const int IMAGE_CEE_CS_CALLCONV_GENERICINST    = 0x0a;
        internal const int IMAGE_CEE_CS_CALLCONV_MAX            = 0x0b;   // first invalid calling convention    
        #endregion
            
        #region Misc
        // The high bits of the calling convention convey additional info   
        internal const int IMAGE_CEE_CS_CALLCONV_MASK      = 0x0f;  // Calling convention is bottom 4 bits 
        internal const int IMAGE_CEE_CS_CALLCONV_GENERIC   = 0x10;  // Generic method sig with explicit number of type parameters
        internal const int IMAGE_CEE_CS_CALLCONV_HASTHIS   = 0x20;  // Top bit indicates a 'this' parameter    
        internal const int IMAGE_CEE_CS_CALLCONV_RETPARAM  = 0x40;  // The first param in the sig is really the return value   
        internal const int NO_SIZE_IN_SIG                   = -1;
        #endregion
    
        #endregion

        #region Static Members
        public static SignatureHelper GetMethodSigHelper(Module mod, Type returnType, Type[] parameterTypes)
        {
            return GetMethodSigHelper(mod, CallingConventions.Standard, returnType, null, null, parameterTypes, null, null);
        }

        /// <include file='doc\SignatureHelper.uex' path='docs/doc[@for="SignatureHelper.GetMethodSigHelper"]/*' />
        internal static SignatureHelper GetMethodSigHelper(Module mod, CallingConventions callingConvention, Type returnType, int cGenericParam)
        {
            return GetMethodSigHelper(mod, callingConvention, cGenericParam, returnType, null, null, null, null, null);
        }
        
        public static SignatureHelper GetMethodSigHelper(Module mod, CallingConventions callingConvention, Type returnType)
        {
            return GetMethodSigHelper(mod, callingConvention, returnType, null, null, null, null, null);
        }

        internal static SignatureHelper GetMethodSpecSigHelper(Module scope, Type[] inst)
        {
            SignatureHelper sigHelp = new SignatureHelper(scope, IMAGE_CEE_CS_CALLCONV_GENERICINST);
            sigHelp.AddData(inst.Length);
            foreach(Type t in inst)
                sigHelp.AddArgument(t);
            return sigHelp;
        }
        
        internal static SignatureHelper GetMethodSigHelper(
            Module scope, CallingConventions callingConvention,
            Type returnType, Type[] requiredReturnTypeCustomModifiers, Type[] optionalReturnTypeCustomModifiers,
            Type[] parameterTypes, Type[][] requiredParameterTypeCustomModifiers, Type[][] optionalParameterTypeCustomModifiers)
        {
            return GetMethodSigHelper(scope, callingConvention, 0, returnType, requiredReturnTypeCustomModifiers, 
                optionalReturnTypeCustomModifiers, parameterTypes, requiredParameterTypeCustomModifiers, optionalParameterTypeCustomModifiers);
        }

        internal static SignatureHelper GetMethodSigHelper(
            Module scope, CallingConventions callingConvention, int cGenericParam,
            Type returnType, Type[] requiredReturnTypeCustomModifiers, Type[] optionalReturnTypeCustomModifiers,
            Type[] parameterTypes, Type[][] requiredParameterTypeCustomModifiers, Type[][] optionalParameterTypeCustomModifiers)
        {
            SignatureHelper sigHelp;
            int intCall;
                
            if (returnType == null)
            {
                returnType = typeof(void);
            }            

            intCall = IMAGE_CEE_CS_CALLCONV_DEFAULT;

            if ((callingConvention & CallingConventions.VarArgs) == CallingConventions.VarArgs)
                intCall = IMAGE_CEE_CS_CALLCONV_VARARG;

            if (cGenericParam > 0)
            {
                intCall |= IMAGE_CEE_CS_CALLCONV_GENERIC;
            }

            if ((callingConvention & CallingConventions.HasThis) == CallingConventions.HasThis)
                intCall |= IMAGE_CEE_CS_CALLCONV_HASTHIS;

            sigHelp = new SignatureHelper(scope, intCall, cGenericParam, returnType, 
                                            requiredReturnTypeCustomModifiers, optionalReturnTypeCustomModifiers);            
            sigHelp.AddArguments(parameterTypes, requiredParameterTypeCustomModifiers, optionalParameterTypeCustomModifiers);

            return sigHelp;
        }

        public static SignatureHelper GetMethodSigHelper(Module mod, CallingConvention unmanagedCallConv, Type returnType)
        {
            SignatureHelper sigHelp;
            int intCall;
                
            if (returnType == null)
                returnType = typeof(void);

            if (unmanagedCallConv == CallingConvention.Cdecl)
            {
                intCall = IMAGE_CEE_UNMANAGED_CALLCONV_C;
            }
            else if (unmanagedCallConv == CallingConvention.StdCall || unmanagedCallConv == CallingConvention.Winapi)
            {
                intCall = IMAGE_CEE_UNMANAGED_CALLCONV_STDCALL;
            }
            else if (unmanagedCallConv == CallingConvention.ThisCall)
            {
                intCall = IMAGE_CEE_UNMANAGED_CALLCONV_THISCALL;
            }
            else if (unmanagedCallConv == CallingConvention.FastCall)
            {
                intCall = IMAGE_CEE_UNMANAGED_CALLCONV_FASTCALL;
            }
            else
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_UnknownUnmanagedCallConv"), "unmanagedCallConv");                          
            }
            
            sigHelp = new SignatureHelper(mod, intCall, returnType, null, null);
            
            return sigHelp;
        }

        public static SignatureHelper GetLocalVarSigHelper()
        {                                                   
            return GetLocalVarSigHelper(null);
        }
        
        public static SignatureHelper GetMethodSigHelper(CallingConventions callingConvention, Type returnType)
        {
            return GetMethodSigHelper(null, callingConvention, returnType);
        }

        public static SignatureHelper GetMethodSigHelper(CallingConvention unmanagedCallingConvention, Type returnType)
        {
            return GetMethodSigHelper(null, unmanagedCallingConvention, returnType);
        }

        public static SignatureHelper GetLocalVarSigHelper(Module mod)
        {
            return new SignatureHelper(mod, IMAGE_CEE_CS_CALLCONV_LOCAL_SIG);
        }
        
        public static SignatureHelper GetFieldSigHelper(Module mod)
        {
            return new SignatureHelper(mod, IMAGE_CEE_CS_CALLCONV_FIELD);
        }
    
        public static SignatureHelper GetPropertySigHelper(Module mod, Type returnType, Type[] parameterTypes)
        {
            return GetPropertySigHelper(mod, returnType, null, null, parameterTypes, null, null);
        }

        public static SignatureHelper GetPropertySigHelper(Module mod, 
            Type returnType, Type[] requiredReturnTypeCustomModifiers, Type[] optionalReturnTypeCustomModifiers, 
            Type[] parameterTypes, Type[][] requiredParameterTypeCustomModifiers, Type[][] optionalParameterTypeCustomModifiers)
        {
            SignatureHelper sigHelp;
                
            if (returnType == null)
            {
                returnType = typeof(void);
            }            

            sigHelp = new SignatureHelper(mod, IMAGE_CEE_CS_CALLCONV_PROPERTY, 
                returnType, requiredReturnTypeCustomModifiers, optionalReturnTypeCustomModifiers);
            sigHelp.AddArguments(parameterTypes, requiredParameterTypeCustomModifiers, optionalParameterTypeCustomModifiers);

            return sigHelp;
        }
        
        internal static SignatureHelper GetTypeSigToken(Module mod, Type type)
        {
            if (mod == null)
                throw new ArgumentNullException("module");

            if (type == null)
                throw new ArgumentNullException("type");

            return new SignatureHelper(mod, type);
        }
        #endregion

        #region Private Data Members
        private byte[] m_signature;
        private int m_currSig; // index into m_signature buffer for next available byte
        private int m_sizeLoc; // size of the m_signature buffer
        private ModuleBuilder m_module;
        private bool m_sigDone;
        private int m_argCount; // tracking number of arguments in the signature
        #endregion

        #region Constructor
        private SignatureHelper(Module mod, int callingConvention)
        {
            // Use this constructor to instantiate a local var sig  or Field where return type is not applied.
            Init(mod, callingConvention);
        }

        private SignatureHelper(Module mod, int callingConvention, int cGenericParameters,
            Type returnType, Type[] requiredCustomModifiers, Type[] optionalCustomModifiers)
        {
            // Use this constructor to instantiate a any signatures that will require a return type.
            Init(mod, callingConvention, cGenericParameters);

            if (callingConvention == IMAGE_CEE_CS_CALLCONV_FIELD) 
                throw new ArgumentException(Environment.GetResourceString("Argument_BadFieldSig"));

            AddOneArgTypeHelper(returnType, requiredCustomModifiers, optionalCustomModifiers);                          
        }

        private SignatureHelper(Module mod, int callingConvention, 
            Type returnType, Type[] requiredCustomModifiers, Type[] optionalCustomModifiers)
            : this(mod, callingConvention, 0, returnType, requiredCustomModifiers, optionalCustomModifiers)
        {
        }

        private SignatureHelper(Module mod, Type type)
        {
            Init(mod);

            AddOneArgTypeHelper(type);
        }

        private void Init(Module mod)
        {
            m_signature = new byte[32];
            m_currSig = 0;
            m_module = mod as ModuleBuilder;
            m_argCount = 0;
            m_sigDone = false;
            m_sizeLoc = NO_SIZE_IN_SIG;

            if (m_module == null && mod != null) 
                throw new ArgumentException(Environment.GetResourceString("NotSupported_MustBeModuleBuilder"));
        }

        private void Init(Module mod, int callingConvention)
        {
            Init(mod, callingConvention, 0);
        }
        
        private void Init(Module mod, int callingConvention, int cGenericParam)
        {        
            Init(mod);

            AddData(callingConvention);

            if (callingConvention == IMAGE_CEE_CS_CALLCONV_FIELD || 
                callingConvention == IMAGE_CEE_CS_CALLCONV_GENERICINST) 
            {
                m_sizeLoc = NO_SIZE_IN_SIG;
            } 
            else 
            {
                if (cGenericParam > 0)
                    AddData(cGenericParam);
                
                m_sizeLoc = m_currSig++;
            }
        }

        #endregion

        #region Private Members
        private void AddOneArgTypeHelper(Type argument, bool pinned)
        {
            if (pinned)
                AddElementType(ELEMENT_TYPE_PINNED);

            AddOneArgTypeHelper(argument);
        }

        private void AddOneArgTypeHelper(Type clsArgument, Type[] requiredCustomModifiers, Type[] optionalCustomModifiers)
        {
            // This function will not increase the argument count. It only fills in bytes 
            // in the signature based on clsArgument. This helper is called for return type.

            ASSERT.PRECONDITION(clsArgument != null);
            ASSERT.PRECONDITION((optionalCustomModifiers == null && requiredCustomModifiers == null) || !clsArgument.ContainsGenericParameters);
            ASSERT.PRECONDITION(requiredCustomModifiers == null || Array.IndexOf(requiredCustomModifiers, null) == -1);
            ASSERT.PRECONDITION(optionalCustomModifiers == null || Array.IndexOf(optionalCustomModifiers, null) == -1);

            if (optionalCustomModifiers != null)
            {
                for (int i = 0; i < optionalCustomModifiers.Length; i++)
                {
                    AddElementType(ELEMENT_TYPE_CMOD_OPT);
                    ASSERT.CONSISTENCY_CHECK(!MetadataToken.IsNullToken(optionalCustomModifiers[i].MetadataTokenInternal));
                    AddToken(m_module.GetTypeToken(optionalCustomModifiers[i]).Token);                    
                }
            }

            if (requiredCustomModifiers != null)
            {
                for (int i = 0; i < requiredCustomModifiers.Length; i++)
                {
                    AddElementType(ELEMENT_TYPE_CMOD_REQD);
                    ASSERT.CONSISTENCY_CHECK(!MetadataToken.IsNullToken(requiredCustomModifiers[i].MetadataTokenInternal));
                    AddToken(m_module.GetTypeToken(requiredCustomModifiers[i]).Token);                                      
                }
            }

            AddOneArgTypeHelper(clsArgument);
        }

        private void AddOneArgTypeHelper(Type clsArgument) { AddOneArgTypeHelperWorker(clsArgument, false); }
        private void AddOneArgTypeHelperWorker(Type clsArgument, bool lastWasGenericInst)
        {
            if (clsArgument.IsGenericParameter)
            {
                if (clsArgument.DeclaringMethod != null)
                    AddData(ELEMENT_TYPE_MVAR);
                else
                    AddData(ELEMENT_TYPE_VAR);

                AddData(clsArgument.GenericParameterPosition);
            }
            else if (clsArgument.IsGenericType && (!clsArgument.IsGenericTypeDefinition || !lastWasGenericInst))
            {           
                AddElementType(ELEMENT_TYPE_GENERICINST);

                AddOneArgTypeHelperWorker(clsArgument.GetGenericTypeDefinition(), true);

                Type[] args = clsArgument.GetGenericArguments();

                AddData(args.Length);

                foreach (Type t in args)
                    AddOneArgTypeHelper(t);
            }
            else if (clsArgument is TypeBuilder)
            {
                TypeBuilder clsBuilder = (TypeBuilder)clsArgument;
                TypeToken tkType;

                if (clsBuilder.Module == m_module)
                {
                    tkType = clsBuilder.TypeToken;
                }
                else
                {
                    tkType = m_module.GetTypeToken(clsArgument);
                }

                if (clsArgument.IsValueType)
                {
                    InternalAddTypeToken(tkType, ELEMENT_TYPE_VALUETYPE);
                }
                else
                {
                    InternalAddTypeToken(tkType, ELEMENT_TYPE_CLASS);
                }
            }
            else if (clsArgument is EnumBuilder)
            {
                TypeBuilder clsBuilder = ((EnumBuilder)clsArgument).m_typeBuilder;
                TypeToken tkType;

                if (clsBuilder.Module == m_module)
                {
                    tkType = clsBuilder.TypeToken;
                }
                else
                {
                    tkType = m_module.GetTypeToken(clsArgument);
                }

                if (clsArgument.IsValueType)
                {
                    InternalAddTypeToken(tkType, ELEMENT_TYPE_VALUETYPE);
                }
                else
                {
                    InternalAddTypeToken(tkType, ELEMENT_TYPE_CLASS);
                }
            }
            else if (clsArgument.IsByRef)
            {
                AddElementType(ELEMENT_TYPE_BYREF);
                clsArgument = clsArgument.GetElementType();
                AddOneArgTypeHelper(clsArgument);
            }
            else if (clsArgument.IsPointer)
            {
                AddElementType(ELEMENT_TYPE_PTR);
                AddOneArgTypeHelper(clsArgument.GetElementType());
            }
            else if (clsArgument.IsArray)
            {           
                if (clsArgument.IsSzArray)
                {
                    AddElementType(ELEMENT_TYPE_SZARRAY);

                    AddOneArgTypeHelper(clsArgument.GetElementType());
                }
                else
                {                    
                    AddElementType(ELEMENT_TYPE_ARRAY);

                    AddOneArgTypeHelper(clsArgument.GetElementType());

                    // put the rank information
                    AddData(clsArgument.GetArrayRank());
                     
                    AddData(0);
                    AddData(0);
                }
            }
            else
            {
                RuntimeType rType = clsArgument as RuntimeType;
                int type = rType != null ? GetCorElementTypeFromClass(rType) : ELEMENT_TYPE_MAX;

                if (IsSimpleType(type))
                {
                    AddElementType(type);
                }
                else
                {
                    if (clsArgument == typeof(object))
                    {
                        AddElementType(ELEMENT_TYPE_OBJECT);
                    }
                    else if (clsArgument == typeof(string))
                    {
                        AddElementType(ELEMENT_TYPE_STRING);
                    }
                    else
                    {
                        if (m_module == null)
                        {
                            InternalAddRuntimeType(rType);
                        }
                        else
                        {
                            if (clsArgument.IsValueType)
                            {
                                InternalAddTypeToken(m_module.GetTypeToken(clsArgument), ELEMENT_TYPE_VALUETYPE);
                            }
                            else
                            {
                                InternalAddTypeToken(m_module.GetTypeToken(clsArgument), ELEMENT_TYPE_CLASS);
                            }
                        }
                    }
                }
            }
        }
    
        private void AddData(int data)
        {
            // A managed representation of CorSigCompressData; 

            if (m_currSig + 4 >= m_signature.Length)
            {
                m_signature = ExpandArray(m_signature);
            }
    
            if (data <= 0x7F)
            {
                m_signature[m_currSig++] = (byte)(data & 0xFF);
            } 
            else if (data <= 0x3FFF)
            {
                m_signature[m_currSig++] = (byte)((data >>8) | 0x80);
                m_signature[m_currSig++] = (byte)(data & 0xFF);
            } 
            else if (data <= 0x1FFFFFFF)
            {
                m_signature[m_currSig++] = (byte)((data >>24) | 0xC0);
                m_signature[m_currSig++] = (byte)((data >>16) & 0xFF);
                m_signature[m_currSig++] = (byte)((data >>8) & 0xFF);
                m_signature[m_currSig++] = (byte)((data) & 0xFF);
            } 
            else
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_LargeInteger"));
            }            
            
        }

        private void AddData(uint data)
        {
            if (m_currSig + 4 >= m_signature.Length)
            {
                m_signature = ExpandArray(m_signature);
            }

            m_signature[m_currSig++] = (byte)((data)     & 0xFF);
            m_signature[m_currSig++] = (byte)((data>>8)  & 0xFF);
            m_signature[m_currSig++] = (byte)((data>>16) & 0xFF);
            m_signature[m_currSig++] = (byte)((data>>24) & 0xFF);
        }
        
        private void AddData(ulong data)
        {
            if (m_currSig + 8 >= m_signature.Length)
            {
                m_signature = ExpandArray(m_signature);
            }

            m_signature[m_currSig++] = (byte)((data)     & 0xFF);
            m_signature[m_currSig++] = (byte)((data>>8)  & 0xFF);
            m_signature[m_currSig++] = (byte)((data>>16) & 0xFF);
            m_signature[m_currSig++] = (byte)((data>>24) & 0xFF);
            m_signature[m_currSig++] = (byte)((data>>32) & 0xFF);
            m_signature[m_currSig++] = (byte)((data>>40) & 0xFF);
            m_signature[m_currSig++] = (byte)((data>>48) & 0xFF);
            m_signature[m_currSig++] = (byte)((data>>56) & 0xFF);
        }
        
        private void AddElementType(int cvt)
        {
            // Adds an element to the signature.  A managed represenation of CorSigCompressElement
            if (m_currSig + 1 >= m_signature.Length) 
                m_signature = ExpandArray(m_signature);

            m_signature[m_currSig++] = (byte)cvt;
        }
    
        private void AddToken(int token) 
        {
            // A managed represenation of CompressToken
            // Pulls the token appart to get a rid, adds some appropriate bits
            // to the token and then adds this to the signature.

            int rid =  (token & 0x00FFFFFF); //This is RidFromToken;
            int type = (token & unchecked((int)0xFF000000)); //This is TypeFromToken;
    
            if (rid > 0x3FFFFFF) 
            {
                // token is too big to be compressed    
                throw new ArgumentException(Environment.GetResourceString("Argument_LargeInteger"));
            }
    
            rid = (rid << 2);   
            
            // TypeDef is encoded with low bits 00  
            // TypeRef is encoded with low bits 01  
            // TypeSpec is encoded with low bits 10    
            if (type == mdtTypeRef) 
            { 
                //if type is mdtTypeRef
                rid|=0x1;
            } 
            else if (type == mdtTypeSpec) 
            { 
                //if type is mdtTypeSpec
                rid|=0x2;
            }
    
            AddData(rid);
        }
    
        private void InternalAddTypeToken(TypeToken clsToken, int CorType)
        {
            // Add a type token into signature. CorType will be either ELEMENT_TYPE_CLASS or ELEMENT_TYPE_VALUECLASS
            AddElementType(CorType);
            AddToken(clsToken.Token);
        }
    
        private unsafe void InternalAddRuntimeType(Type type)
        {
            // Add a runtime type into the signature. 

            AddElementType(ELEMENT_TYPE_INTERNAL);

            void* handle = (void*)type.GetTypeHandleInternal().Value;

            if (sizeof(void*) == sizeof(uint)) 
            {
                AddData((uint)handle);
            }
            else 
            {
                AddData((ulong)handle);
            }
        }
    
        private byte[] ExpandArray(byte[] inArray)
        {
            // Expand the signature buffer size
            return ExpandArray(inArray, inArray.Length * 2);
        }

        private byte[] ExpandArray(byte[] inArray, int requiredLength)
        {
            // Expand the signature buffer size

            if (requiredLength < inArray.Length) 
                requiredLength = inArray.Length*2;

            byte[] outArray = new byte[requiredLength];
            Array.Copy(inArray, outArray, inArray.Length);
            return outArray;
        }
    
        private void IncrementArgCounts()
        {
            if (m_sizeLoc == NO_SIZE_IN_SIG) 
            { 
                //We don't have a size if this is a field.
                return;
            }

            m_argCount++;
        }
    
        private void SetNumberOfSignatureElements(bool forceCopy)
        {
            // For most signatures, this will set the number of elements in a byte which we have reserved for it.
            // However, if we have a field signature, we don't set the length and return.
            // If we have a signature with more than 128 arguments, we can't just set the number of elements,
            // we actually have to allocate more space (e.g. shift everything in the array one or more spaces to the
            // right.  We do this by making a copy of the array and leaving the correct number of blanks.  This new
            // array is now set to be m_signature and we use the AddData method to set the number of elements properly.
            // The forceCopy argument can be used to force SetNumberOfSignatureElements to make a copy of
            // the array.  This is useful for GetSignature which promises to trim the array to be the correct size anyway.

            byte[] temp;
            int newSigSize;
            int currSigHolder = m_currSig;
    
            if (m_sizeLoc == NO_SIZE_IN_SIG) 
                return;
    
            //If we have fewer than 128 arguments and we haven't been told to copy the
            //array, we can just set the appropriate bit and return.
            if (m_argCount < 0x80 && !forceCopy) 
            {
                m_signature[m_sizeLoc] = (byte)m_argCount;
                return;
            } 
    
            //We need to have more bytes for the size.  Figure out how many bytes here.
            //Since we need to copy anyway, we're just going to take the cost of doing a
            //new allocation.
            if (m_argCount < 0x7F)
            {
                newSigSize = 1;
            }
            else if (m_argCount < 0x3FFF)
            {
                newSigSize = 2;
            }
            else
            {
                newSigSize = 4;
            }
            
            //Allocate the new array.
            temp = new byte[m_currSig + newSigSize - 1];
    
            //Copy the calling convention.  The calling convention is always just one byte
            //so we just copy that byte.  Then copy the rest of the array, shifting everything
            //to make room for the new number of elements.
            temp[0] = m_signature[0];
            Array.Copy(m_signature, m_sizeLoc + 1, temp, m_sizeLoc+  newSigSize, currSigHolder - (m_sizeLoc + 1));
            m_signature = temp;
            
            //Use the AddData method to add the number of elements appropriately compressed.
            m_currSig = m_sizeLoc;
            AddData(m_argCount);
            m_currSig = currSigHolder + (newSigSize - 1);
        }
    
        #endregion

        #region Internal Members
        internal int ArgumentCount
        { 
            get 
            { 
                return m_argCount; 
            } 
        }

        internal static bool IsSimpleType(int type)
        {
            if (type <= ELEMENT_TYPE_STRING) 
                return true;

            if (type == ELEMENT_TYPE_TYPEDBYREF || type == ELEMENT_TYPE_I || type == ELEMENT_TYPE_U || type == ELEMENT_TYPE_OBJECT) 
                return true;

            return false;
        }
    
        internal byte[] InternalGetSignature(out int length)
        {
            // An internal method to return the signature.  Does not trim the
            // array, but passes out the length of the array in an out parameter.
            // This is the actual array -- not a copy -- so the callee must agree
            // to not copy it.
            //
            // param length : an out param indicating the length of the array.
            // return : A reference to the internal ubyte array.
    
            if (!m_sigDone)
            {
                m_sigDone = true;

                SetNumberOfSignatureElements(false);
            }
    
            length = m_currSig;
            return m_signature;
        }
    
         
         
         
        internal byte[] InternalGetSignatureArray()
        {
            int argCount = m_argCount; 
            int currSigLength = m_currSig;
            int newSigSize = currSigLength;
    
            //Allocate the new array.
            if (argCount < 0x7F)
                newSigSize += 1;
            else if (argCount < 0x3FFF)
                newSigSize += 2;
            else
                newSigSize += 4;
            byte[] temp = new byte[newSigSize];

            // copy the sig
            int sigCopyIndex = 0;
            // calling convention
            temp[sigCopyIndex++] = m_signature[0];
            // arg size
            if (argCount <= 0x7F)
                temp[sigCopyIndex++] = (byte)(argCount & 0xFF);
            else if (argCount <= 0x3FFF)
            {
                temp[sigCopyIndex++] = (byte)((argCount >>8) | 0x80);
                temp[sigCopyIndex++] = (byte)(argCount & 0xFF);
            } 
            else if (argCount <= 0x1FFFFFFF)
            {
                temp[sigCopyIndex++] = (byte)((argCount >>24) | 0xC0);
                temp[sigCopyIndex++] = (byte)((argCount >>16) & 0xFF);
                temp[sigCopyIndex++] = (byte)((argCount >>8) & 0xFF);
                temp[sigCopyIndex++] = (byte)((argCount) & 0xFF);
            } 
            else
                throw new ArgumentException(Environment.GetResourceString("Argument_LargeInteger"));
            // copy the sig part of the sig
            Array.Copy(m_signature, 2, temp, sigCopyIndex, currSigLength - 2);
            // mark the end of sig
            temp[newSigSize - 1] = ELEMENT_TYPE_END;
    
            return temp;
        }
    
        #endregion

        #region Public Methods
        public void AddArgument(Type clsArgument)
        {
            AddArgument(clsArgument, null, null);
        }

        public void AddArgument(Type argument, bool pinned)
        {
            if (argument == null)
                throw new ArgumentNullException("argument");

            IncrementArgCounts();
            AddOneArgTypeHelper(argument, pinned);
        }
		
        public void AddArguments(Type[] arguments, Type[][] requiredCustomModifiers, Type[][] optionalCustomModifiers)
        {
            if (requiredCustomModifiers != null && (arguments == null || requiredCustomModifiers.Length != arguments.Length))
                throw new ArgumentException(Environment.GetResourceString("Argument_MismatchedArrays", "requiredCustomModifiers", "arguments"));

            if (optionalCustomModifiers != null && (arguments == null || optionalCustomModifiers.Length != arguments.Length))
                throw new ArgumentException(Environment.GetResourceString("Argument_MismatchedArrays", "optionalCustomModifiers", "arguments"));

            if (arguments != null)
            {
                for (int i =0; i < arguments.Length; i++)
                {
                    AddArgument(arguments[i], 
                        requiredCustomModifiers == null ? null : requiredCustomModifiers[i], 
                        optionalCustomModifiers == null ? null : optionalCustomModifiers[i]);
                }
            }
        }

        public void AddArgument(Type argument, Type[] requiredCustomModifiers, Type[] optionalCustomModifiers)
        {
            if (m_sigDone)
                throw new ArgumentException(Environment.GetResourceString("Argument_SigIsFinalized"));
    
            if (argument == null)
                throw new ArgumentNullException("argument");

            if (requiredCustomModifiers != null)
            {
                for (int i = 0; i < requiredCustomModifiers.Length; i++)
                {
                    Type t = requiredCustomModifiers[i];

                    if (t == null)
                        throw new ArgumentNullException("requiredCustomModifiers");

                    if (t.HasElementType)
                        throw new ArgumentException(Environment.GetResourceString("Argument_ArraysInvalid"), "requiredCustomModifiers");

                    if (t.ContainsGenericParameters)
                        throw new ArgumentException(Environment.GetResourceString("Argument_GenericsInvalid"), "requiredCustomModifiers");

                    ASSERT.CONSISTENCY_CHECK(!MetadataToken.IsNullToken(t.MetadataTokenInternal));
                }
            }            

            if (optionalCustomModifiers != null)
            {
                for (int i = 0; i < optionalCustomModifiers.Length; i++)
                {
                    Type t = optionalCustomModifiers[i];

                    if (t == null)
                        throw new ArgumentNullException("optionalCustomModifiers");

                    if (t.HasElementType)
                        throw new ArgumentException(Environment.GetResourceString("Argument_ArraysInvalid"), "optionalCustomModifiers");

                    if (t.ContainsGenericParameters)
                        throw new ArgumentException(Environment.GetResourceString("Argument_GenericsInvalid"), "optionalCustomModifiers");
                    
                    ASSERT.CONSISTENCY_CHECK(!MetadataToken.IsNullToken(t.MetadataTokenInternal));
                }
            }
            
            IncrementArgCounts();
    
            // Add an argument to the signature. Takes a Type and determines whether it
            // is one of the primitive types of which we have special knowledge or a more
            // general class.  In the former case, we only add the appropriate short cut encoding, 
            // otherwise we will calculate proper description for the type.
            AddOneArgTypeHelper(argument, requiredCustomModifiers, optionalCustomModifiers);
        }

        public void AddSentinel()
        {
            AddElementType(ELEMENT_TYPE_SENTINEL);
        }

        public override bool Equals(Object obj)
        {
            if (!(obj is SignatureHelper)) 
            {
                return false;
            }
            
            SignatureHelper temp = (SignatureHelper)obj;
            
            if (temp.m_module !=m_module || temp.m_currSig!=m_currSig || temp.m_sizeLoc!=m_sizeLoc || temp.m_sigDone !=m_sigDone) 
            {
                return false;
            }
            
            for (int i=0; i<m_currSig; i++) 
            {
                if (m_signature[i]!=temp.m_signature[i]) 
                    return false;
            }
            return true;
        }
    
        public override int GetHashCode()
        {
            // Start the hash code with the hash code of the module and the values of the member variables.
            int HashCode = m_module.GetHashCode() + m_currSig + m_sizeLoc;

            // Add one if the sig is done.
            if (m_sigDone)
                HashCode += 1;

            // Then add the hash code of all the arguments.
            for (int i=0; i < m_currSig; i++) 
                HashCode += m_signature[i].GetHashCode();

            return HashCode;
        }

        public byte[] GetSignature()
        {
            return GetSignature(false);
        }
    
        internal byte[] GetSignature(bool appendEndOfSig)
        {
            // Chops the internal signature to the appropriate length.  Adds the 
            // end token to the signature and marks the signature as finished so that
            // no further tokens can be added. Return the full signature in a trimmed array.
            if (!m_sigDone) 
            {
                if (appendEndOfSig) 
                    AddElementType(ELEMENT_TYPE_END);
                SetNumberOfSignatureElements(true);
                m_sigDone = true;
            }
    
            // This case will only happen if the user got the signature through 
            // InternalGetSignature first and then called GetSignature.
            if (m_signature.Length > m_currSig) 
            {
                byte[] temp = new byte[m_currSig];
                Array.Copy(m_signature, temp, m_currSig);
                m_signature = temp;
            }

            return m_signature;
        }
    
        public override String ToString()
        {
            StringBuilder sb = new StringBuilder();
            sb.Append("Length: " + m_currSig + Environment.NewLine);

            if (m_sizeLoc != -1)
            {
                sb.Append("Arguments: " + m_signature[m_sizeLoc] + Environment.NewLine);
            }
            else
            {
                sb.Append("Field Signature" + Environment.NewLine);
            }

            sb.Append("Signature: " + Environment.NewLine);
            for (int i=0; i<=m_currSig; i++) 
            {
                sb.Append(m_signature[i] + "  ");
            }

            sb.Append(Environment.NewLine);
            return sb.ToString();
        }
        
        #endregion

        #region FCalls
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern int GetCorElementTypeFromClass(RuntimeType cls);
        #endregion

        void _SignatureHelper.GetTypeInfoCount(out uint pcTInfo)
        {
            throw new NotImplementedException();
        }

        void _SignatureHelper.GetTypeInfo(uint iTInfo, uint lcid, IntPtr ppTInfo)
        {
            throw new NotImplementedException();
        }

        void _SignatureHelper.GetIDsOfNames([In] ref Guid riid, IntPtr rgszNames, uint cNames, uint lcid, IntPtr rgDispId)
        {
            throw new NotImplementedException();
        }

        void _SignatureHelper.Invoke(uint dispIdMember, [In] ref Guid riid, uint lcid, short wFlags, IntPtr pDispParams, IntPtr pVarResult, IntPtr pExcepInfo, IntPtr puArgErr)
        {
            throw new NotImplementedException();
        }

    }
}

























