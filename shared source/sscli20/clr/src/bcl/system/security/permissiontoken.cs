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
namespace System.Security {
    using System;
    using System.Security.Util;
    using Hashtable = System.Collections.Hashtable;
    using System.Runtime.Remoting.Activation;
    using System.Security.Permissions;
    using System.Reflection;
    using System.Collections;
    using System.Threading;
    using System.Globalization;

    [Flags]
    internal enum PermissionTokenType
    {
        Normal = 0x1,
        IUnrestricted = 0x2,
        DontKnow = 0x4,
        BuiltIn = 0x8
    }

    [Serializable()]
    internal sealed class PermissionTokenKeyComparer : IEqualityComparer
    {
        private Comparer _caseSensitiveComparer;
        private TextInfo _info;

        public PermissionTokenKeyComparer(CultureInfo culture)
        {
            _caseSensitiveComparer = new Comparer( culture );
            _info = culture.TextInfo;
        }

        public int Compare( Object a, Object b )
        {
            String strA = a as String;
            String strB = b as String;

            // if it's not a string then we just call the object comparer
            if (strA == null || strB == null)
                return _caseSensitiveComparer.Compare(a,b);

            int i = _caseSensitiveComparer.Compare(a,b);
            if (i == 0)
                return 0;

            if (SecurityManager._IsSameType( strA, strB ))
                return 0;
            
            return i;
        }

        public new bool Equals( Object a, Object b )
        {
            if (a == b) return true;
            if (a == null || b == null) return false;
            return Compare( a, b ) == 0;
        }

        // The data structure consuming this will be responsible for dealing with null objects as keys.
        public int GetHashCode( Object obj )
        {
            String str = obj as String;

            if (str == null)
                return obj.GetHashCode();

            int iComma = str.IndexOf( ',' );
            if (iComma == -1)
                iComma = str.Length;

            int accumulator = 0;
            for (int i = 0; i < iComma; ++i)
            {
                accumulator = (accumulator << 7) ^ str[i] ^ (accumulator >> 25);
            }

            return accumulator;
        }
    }

    [Serializable]
    internal sealed class PermissionToken : ISecurityEncodable
    {
        private static readonly PermissionTokenFactory s_theTokenFactory;
        private static ReflectionPermission s_reflectPerm = null;
        private const string c_mscorlibName = "mscorlib";

        internal int    m_index;
        internal PermissionTokenType m_type;
        internal String m_strTypeName;
        static internal TokenBasedSet s_tokenSet = new TokenBasedSet();

        internal static bool IsMscorlibClassName (string className) {
            BCLDebug.Assert( c_mscorlibName == Assembly.GetExecutingAssembly().nGetSimpleName(),
                "mscorlib name mismatch" );

            // If the class name does not look like a fully qualified name, we cannot simply determine if it's 
            // an mscorlib.dll type so we should return true so the type can be matched with the
            // right index in the TokenBasedSet.
            int index = className.IndexOf(',');
            if (index == -1)
                return true;

            index = className.LastIndexOf(']');
            if (index == -1)
                index = 0;

            // Search for the string 'mscorlib' in the classname. If we find it, we will conservatively assume it's an mscorlib.dll type and load it.
            for (int i = index; i < className.Length; i++) {
                if (className[i] == 'm' || className[i] == 'M') {
                    if (String.Compare(className, i, c_mscorlibName, 0, c_mscorlibName.Length, StringComparison.OrdinalIgnoreCase) == 0)
                        return true;
                }
            }
            return false;
        }

        static PermissionToken()
        {
            s_theTokenFactory = new PermissionTokenFactory( 4 );
        }

        internal PermissionToken()
        {
        }

        internal PermissionToken(int index, PermissionTokenType type, String strTypeName)
        {
            m_index = index;
            m_type = type;
            m_strTypeName = strTypeName;
        }

        public static PermissionToken GetToken(Type cls)
        {
            if (cls == null)
                return null;
            
            if (cls.GetInterface( "System.Security.Permissions.IBuiltInPermission" ) != null)
            {
                if (s_reflectPerm == null)
                    s_reflectPerm = new ReflectionPermission(PermissionState.Unrestricted);
                s_reflectPerm.Assert();
                MethodInfo method = cls.GetMethod( "GetTokenIndex", BindingFlags.Static | BindingFlags.NonPublic );
                BCLDebug.Assert( method != null, "IBuiltInPermission types should have a static method called 'GetTokenIndex'" );
                return s_theTokenFactory.BuiltInGetToken( (int)method.Invoke( null, null ), null, cls );
            }
            else
            {
                return s_theTokenFactory.GetToken(cls, null);
            }
        }

        public static PermissionToken GetToken(IPermission perm)
        {
            if (perm == null)
                return null;

            IBuiltInPermission ibPerm = perm as IBuiltInPermission;

            if (ibPerm != null)
                return s_theTokenFactory.BuiltInGetToken( ibPerm.GetTokenIndex(), perm, null );
            else
                return s_theTokenFactory.GetToken(perm.GetType(), perm);
        }

        public static PermissionToken GetToken(String typeStr)
        {
            return GetToken( typeStr, false );
        }

        public static PermissionToken GetToken(String typeStr, bool bCreateMscorlib)
        {
            if (typeStr == null)
                return null;

            if (IsMscorlibClassName( typeStr ))
            {
                if (!bCreateMscorlib)
                {
                    return null;
                }
                else
                {
                    return FindToken( Type.GetType( typeStr ) );
                }
            }
            else
            {
                PermissionToken token = s_theTokenFactory.GetToken(typeStr);
#if _DEBUG
                new PermissionSet(PermissionState.Unrestricted).Assert();
                StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
                Type type = RuntimeType.PrivateGetType( typeStr.Trim().Replace( '\'', '\"' ),
                                                false,
                                                false, ref stackMark);
                BCLDebug.Assert( (type == null) || (type.Module.Assembly != System.Reflection.Assembly.GetExecutingAssembly()) || (typeStr.IndexOf("mscorlib", StringComparison.Ordinal) < 0),
                    "We should not go through this path for mscorlib based permissions" );
#endif
                return token;
            }
        }

        public static PermissionToken FindToken( Type cls )
        {
            if (cls == null)
                return null;
             
            if (cls.GetInterface( "System.Security.Permissions.IBuiltInPermission" ) != null)
            {
                if (s_reflectPerm == null)
                    s_reflectPerm = new ReflectionPermission(PermissionState.Unrestricted);
                s_reflectPerm.Assert();
                MethodInfo method = cls.GetMethod( "GetTokenIndex", BindingFlags.Static | BindingFlags.NonPublic );
                BCLDebug.Assert( method != null, "IBuiltInPermission types should have a static method called 'GetTokenIndex'" );
                return s_theTokenFactory.BuiltInGetToken( (int)method.Invoke( null, null ), null, cls );
            }
            else
            {
                return s_theTokenFactory.FindToken( cls );
            }
        }

        public static PermissionToken FindTokenByIndex( int i )
        {
            return s_theTokenFactory.FindTokenByIndex( i );
        }

        public static bool IsTokenProperlyAssigned( IPermission perm, PermissionToken token )
        {
            PermissionToken heldToken = GetToken( perm );
            if (heldToken.m_index != token.m_index)
                return false;

            if (token.m_type != heldToken.m_type)
                return false;

            if (perm.GetType().Module.Assembly == Assembly.GetExecutingAssembly() &&
                heldToken.m_index >= BuiltInPermissionIndex.NUM_BUILTIN_NORMAL + BuiltInPermissionIndex.NUM_BUILTIN_UNRESTRICTED)
                return false;

            return true;
        }

        public SecurityElement ToXml()
        {
            BCLDebug.Assert( (m_type & PermissionTokenType.DontKnow) == 0, "Should have valid token type when ToXml is called" );
            SecurityElement elRoot = new SecurityElement( "PermissionToken" );
            if ((m_type & PermissionTokenType.BuiltIn) != 0)
                elRoot.AddAttribute( "Index", "" + this.m_index );
            else
                elRoot.AddAttribute( "Name", SecurityElement.Escape( m_strTypeName ) );
            elRoot.AddAttribute("Type", m_type.ToString("F"));
            return elRoot;
        }

        public void FromXml( SecurityElement elRoot )
        {
            // For the most part there is no parameter checking here since this is an
            // internal class and the serialization/deserialization path is controlled.

            if (!elRoot.Tag.Equals( "PermissionToken" ))
                BCLDebug.Assert( false, "Tried to deserialize non-PermissionToken element here" );

            String strName = elRoot.Attribute( "Name" );
            PermissionToken realToken;
            if (strName != null)
                realToken = GetToken( strName, true );
            else
                realToken = FindTokenByIndex( Int32.Parse( elRoot.Attribute( "Index" ), CultureInfo.InvariantCulture ) );
            
            this.m_index = realToken.m_index;
            this.m_type = (PermissionTokenType) Enum.Parse(typeof(PermissionTokenType), elRoot.Attribute("Type"));
            BCLDebug.Assert((this.m_type & PermissionTokenType.DontKnow) == 0, "Should have valid token type when FromXml is called.");
            this.m_strTypeName = realToken.m_strTypeName;
        }
    }

    // Package access only
    internal class PermissionTokenFactory
    {
        private int       m_size;
        private int       m_index;
        private Hashtable m_tokenTable;    // Cache of tokens by class string name
        private Hashtable m_handleTable;   // Cache of tokens by type handle (IntPtr)
        private Hashtable m_indexTable;    // Cache of tokens by index

        // We keep an array of tokens for our built-in permissions.
        // This is ordered in terms of unrestricted perms first, normals
        // second.  Of course, all the ordering is based on the individual
        // permissions sticking to the deal, so we do some simple boundary
        // checking but mainly leave it to faith.

        private PermissionToken[] m_builtIn;

        private const String s_unrestrictedPermissionInferfaceName = "System.Security.Permissions.IUnrestrictedPermission";

        internal PermissionTokenFactory( int size )
        {
            m_builtIn = new PermissionToken[BuiltInPermissionIndex.NUM_BUILTIN_NORMAL + BuiltInPermissionIndex.NUM_BUILTIN_UNRESTRICTED];

            m_size = size;
            m_index = BuiltInPermissionIndex.NUM_BUILTIN_NORMAL + BuiltInPermissionIndex.NUM_BUILTIN_UNRESTRICTED;
            m_tokenTable = null;
            m_handleTable = new Hashtable( size );
            m_indexTable = new Hashtable( size );
        }

        internal PermissionToken FindToken( Type cls )
        {
            IntPtr typePtr = cls.TypeHandle.Value;

            PermissionToken tok = (PermissionToken)m_handleTable[typePtr];

            if (tok != null)
                return tok;

            if (m_tokenTable == null)
                return null;

            tok = (PermissionToken)m_tokenTable[cls.AssemblyQualifiedName];

            if (tok != null)
            {
                lock (this)
                {
                    m_handleTable.Add(typePtr, tok);
                }
            }

            return tok;
        }

        internal PermissionToken FindTokenByIndex( int i )
        {
            PermissionToken token;

            if (i < BuiltInPermissionIndex.NUM_BUILTIN_NORMAL + BuiltInPermissionIndex.NUM_BUILTIN_UNRESTRICTED)
            {
                token = BuiltInGetToken( i, null, null );
            }
            else
            {
                token = (PermissionToken)m_indexTable[i];
            }

            return token;
        }

        internal PermissionToken GetToken(Type cls, IPermission perm)
        {
            BCLDebug.Assert( cls != null, "Must pass in valid type" );

            IntPtr typePtr = cls.TypeHandle.Value;
            object tok = m_handleTable[typePtr];
            if (tok == null)
            {
                String typeStr = cls.AssemblyQualifiedName;
                tok = m_tokenTable != null ? m_tokenTable[typeStr] : null; // Assumes asynchronous lookups are safe
                if (tok == null)
                {
                    lock (this)
                    {
                        if (m_tokenTable != null)
                            tok = m_tokenTable[typeStr]; // Make sure it wasn't just added
                        else
                            m_tokenTable = new Hashtable( m_size, 1.0f, new PermissionTokenKeyComparer( CultureInfo.InvariantCulture ) );

                        if (tok == null)
                        {
                            if (perm != null)
                            {
                                if (CodeAccessPermission.CanUnrestrictedOverride(perm))
                                    tok = new PermissionToken( m_index++, PermissionTokenType.IUnrestricted, typeStr );
                                else
                                    tok = new PermissionToken( m_index++, PermissionTokenType.Normal, typeStr );
                            }
                            else
                            {
                                if (cls.GetInterface(s_unrestrictedPermissionInferfaceName) != null)
                                    tok = new PermissionToken( m_index++, PermissionTokenType.IUnrestricted, typeStr );
                                else
                                    tok = new PermissionToken( m_index++, PermissionTokenType.Normal, typeStr );
                            }
                            m_tokenTable.Add(typeStr, tok);
                            m_indexTable.Add(m_index - 1, tok);
                            PermissionToken.s_tokenSet.SetItem( ((PermissionToken)tok).m_index, tok );
                        }

                        if (!m_handleTable.Contains( typePtr ))
                            m_handleTable.Add( typePtr, tok );
                    }
                }
                else
                {
                    lock (this)
                    {
                        if (!m_handleTable.Contains( typePtr ))
                            m_handleTable.Add( typePtr, tok );
                    }
                }
            }

            if ((((PermissionToken)tok).m_type & PermissionTokenType.DontKnow) != 0)
            {
                if (perm != null)
                {
                    BCLDebug.Assert( !(perm is IBuiltInPermission), "This should not be called for built-ins" );
                    if (CodeAccessPermission.CanUnrestrictedOverride(perm))
                        ((PermissionToken)tok).m_type = PermissionTokenType.IUnrestricted;
                    else
                        ((PermissionToken)tok).m_type = PermissionTokenType.Normal;
                    ((PermissionToken)tok).m_strTypeName = perm.GetType().AssemblyQualifiedName;
                }
                else
                {
                    BCLDebug.Assert( cls.GetInterface( "System.Security.Permissions.IBuiltInPermission" ) == null, "This shoudl not be called for built-ins" );
                    if (cls.GetInterface(s_unrestrictedPermissionInferfaceName) != null)
                        ((PermissionToken)tok).m_type = PermissionTokenType.IUnrestricted;
                    else
                        ((PermissionToken)tok).m_type = PermissionTokenType.Normal;
                    ((PermissionToken)tok).m_strTypeName = cls.AssemblyQualifiedName;
                }
            }

            return (PermissionToken)tok;
        }

        internal PermissionToken GetToken(String typeStr)
        {
            Object tok = null;
            tok = m_tokenTable != null ? m_tokenTable[typeStr] : null; // Assumes asynchronous lookups are safe
            if (tok == null)
            {
                lock (this)
                {
                    if (m_tokenTable != null)
                        tok = m_tokenTable[typeStr]; // Make sure it wasn't just added
                    else
                        m_tokenTable = new Hashtable( m_size, 1.0f, new PermissionTokenKeyComparer( CultureInfo.InvariantCulture ) );
                        
                    if (tok == null)
                    {
                        tok = new PermissionToken( m_index++, PermissionTokenType.DontKnow, typeStr );
                        m_tokenTable.Add(typeStr, tok);
                        m_indexTable.Add(m_index - 1, tok);
                        PermissionToken.s_tokenSet.SetItem( ((PermissionToken)tok).m_index, tok );
                    }
                }
            }

            return (PermissionToken)tok;
        }

        internal PermissionToken BuiltInGetToken( int index, IPermission perm, Type cls )
        {
            PermissionToken token = m_builtIn[index];

            if (token == null)
            {
                lock (this)
                {
                    token = m_builtIn[index];

                    if (token == null)
                    {
                        PermissionTokenType permType = PermissionTokenType.DontKnow;

                        if (perm != null)
                        {
                            if(CodeAccessSecurityEngine.DoesFullTrustMeanFullTrust() || perm is IUnrestrictedPermission)
                                permType = PermissionTokenType.IUnrestricted;
                            else
                                permType = PermissionTokenType.Normal;
                        }
                        else if (cls != null)
                        {
                            if(CodeAccessSecurityEngine.DoesFullTrustMeanFullTrust() || cls.GetInterface( "System.Security.Permissions.IUnrestrictedPermission" ) != null)
                                permType = PermissionTokenType.IUnrestricted;
                            else
                                permType = PermissionTokenType.Normal;
                        }

                        token = new PermissionToken( index, permType | PermissionTokenType.BuiltIn, null );
                        m_builtIn[index] = token;
                        PermissionToken.s_tokenSet.SetItem( token.m_index, token );
                    }
                }
            }

            if ((token.m_type & PermissionTokenType.DontKnow) != 0)
            {
                    token.m_type = PermissionTokenType.BuiltIn;

                    if (perm != null)
                    {
                        if(CodeAccessSecurityEngine.DoesFullTrustMeanFullTrust() || perm is IUnrestrictedPermission)
                            token.m_type |= PermissionTokenType.IUnrestricted;
                        else
                            token.m_type |= PermissionTokenType.Normal;
                    }
                    else if (cls != null)
                    {
                        if(CodeAccessSecurityEngine.DoesFullTrustMeanFullTrust() || cls.GetInterface( "System.Security.Permissions.IUnrestrictedPermission" ) != null)
                            token.m_type |= PermissionTokenType.IUnrestricted;
                        else
                            token.m_type |= PermissionTokenType.Normal;
                    }
                    else
                        token.m_type |= PermissionTokenType.DontKnow;
            }

            return token;
        }
    }
}
