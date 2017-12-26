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
//  NetCodeGroup.cs
//
//  Representation for code groups used for the policy mechanism
//

namespace System.Security.Policy {

    using System;
    using System.Security.Util;
    using System.Security;
    using System.Collections;
    using System.Reflection;
    using System.Globalization;
    using System.Runtime.Serialization;
    using System.Runtime.Versioning;

    //
    //This is a simple property bag used to describe connect back access.
    //
    [Serializable]
    [System.Runtime.InteropServices.ComVisible(true)]
    public class CodeConnectAccess {
        private string _LowerCaseScheme;
        private string _LowerCasePort;
        private int    _IntPort;

        private const  string  DefaultStr = "$default";     //must remain in lower case
        private const  string  OriginStr  = "$origin";  //must remain in lower case

        internal const  int     NoPort  = -1;
        internal const  int     AnyPort = -2; // This safely excludes -1 (if we decide to support "any" port later)

        //
        // public helper fields to deal with special scheme and port values.
        //
        public static readonly int     DefaultPort   = -3;
        public static readonly int     OriginPort    = -4;
        public static readonly string  OriginScheme  = OriginStr;
        public static readonly string  AnyScheme     = "*";

        //
        // A set of public static class factories
        //
        public CodeConnectAccess(string allowScheme, int allowPort)
        {
            if (!IsValidScheme(allowScheme))
                throw new ArgumentOutOfRangeException("allowScheme");

            SetCodeConnectAccess(allowScheme.ToLower(CultureInfo.InvariantCulture), allowPort);
        }
        //
        public static CodeConnectAccess CreateOriginSchemeAccess(int allowPort)
        {
            CodeConnectAccess access = new CodeConnectAccess();
            access.SetCodeConnectAccess(OriginScheme, allowPort);
            return access;
        }
        //
        public static CodeConnectAccess CreateAnySchemeAccess(int allowPort)
        {
            CodeConnectAccess access = new CodeConnectAccess();
            access.SetCodeConnectAccess(AnyScheme, allowPort);
            return access;
        }
        //
        private CodeConnectAccess()
        {
        }
        //
        private void SetCodeConnectAccess(string lowerCaseScheme, int allowPort)
        {
            _LowerCaseScheme = lowerCaseScheme;

            if (allowPort == DefaultPort)
                _LowerCasePort = DefaultStr;
            else if (allowPort == OriginPort)
                _LowerCasePort = OriginStr;
            else
            {
                if (allowPort < 0 || allowPort > 0xFFFF)
                    throw new ArgumentOutOfRangeException("allowPort");

                _LowerCasePort = allowPort.ToString(CultureInfo.InvariantCulture);
            }

            _IntPort = allowPort;
        }
        //
        public String   Scheme {
            get {return _LowerCaseScheme;}
        }
        //
        public int      Port {
            get {return _IntPort;}
        }
        //
        public override bool Equals(object o)
        {
            if ((object)this == (object)o)
                return true;

            CodeConnectAccess that = (o as CodeConnectAccess);

            if (that == null)
                return false;

            return this.Scheme == that.Scheme && this.Port == that.Port;
        }
        //
        public override int GetHashCode()
        {
            return Scheme.GetHashCode() + Port.GetHashCode();
        }
        //
        // internal stuff
        //
        //
        // The valid scheme values are: "*", "$origin", or a valid Uri scheme
        // The valid port valies are "$origin" "$default" or a valid Uri port
        //
        internal CodeConnectAccess(string allowScheme, string allowPort)
        {
            if (allowScheme == null || allowScheme.Length == 0)
                throw new ArgumentNullException("allowScheme");

            if (allowPort == null || allowPort.Length == 0)
                throw new ArgumentNullException("allowPort");

            _LowerCaseScheme = allowScheme.ToLower(CultureInfo.InvariantCulture);

            if (_LowerCaseScheme == OriginScheme)
                _LowerCaseScheme = OriginScheme;
            else if (_LowerCaseScheme == AnyScheme)
                _LowerCaseScheme = AnyScheme;
            else if (!IsValidScheme(_LowerCaseScheme))
                throw new ArgumentOutOfRangeException("allowScheme");

            _LowerCasePort = allowPort.ToLower(CultureInfo.InvariantCulture);

            if (_LowerCasePort == DefaultStr)
                _IntPort = DefaultPort;
            else if (_LowerCasePort == OriginStr)
                _IntPort = OriginPort;
            else
            {
                _IntPort = Int32.Parse(allowPort, CultureInfo.InvariantCulture);

                if (_IntPort < 0 || _IntPort > 0xFFFF)
                    throw new ArgumentOutOfRangeException("allowPort");

                _LowerCasePort = _IntPort.ToString(CultureInfo.InvariantCulture);
            }
        }
        //
        internal bool     IsOriginScheme {
            get {return (object)_LowerCaseScheme == (object)OriginScheme;}
        }
        //
        internal bool     IsAnyScheme {
            get {return (object)_LowerCaseScheme == (object)AnyScheme;}
        }
        //
        internal bool     IsDefaultPort {
            get {return Port == DefaultPort;}
        }
        //
        internal bool     IsOriginPort {
            get {return Port == OriginPort;}
        }
        //
        // More Internal stuff
        //
        internal string  StrPort {
            get { return _LowerCasePort;}
        }
        //
        internal static bool IsValidScheme(string scheme)
        {
            if (((object)scheme == null) || (scheme.Length == 0) || !IsAsciiLetter(scheme[0]))
                return false;

            for (int i = scheme.Length - 1; i > 0; --i) {
                if (!(IsAsciiLetterOrDigit(scheme[i]) || (scheme[i] == '+') || (scheme[i] == '-') || (scheme[i] == '.')))
                    return false;
            }
            return true;
        }
        //
        private static bool IsAsciiLetterOrDigit(char character) {
            return IsAsciiLetter(character) || (character >= '0' && character <= '9');
        }
        //
        private static bool IsAsciiLetter(char character) {
            return (character >= 'a' && character <= 'z') ||
                   (character >= 'A' && character <= 'Z');
        }
    }

    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    sealed public class NetCodeGroup : CodeGroup, IUnionSemanticCodeGroup
    {
        [System.Diagnostics.Conditional( "_DEBUG" )]
        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Process, ResourceScope.Process)]
        private static void DEBUG_OUT( String str )
        {
#if _DEBUG
            if (debug)
            {
                if (to_file)
                {
                    System.Text.StringBuilder sb = new System.Text.StringBuilder();
                    sb.Append( str );
                    sb.Append ((char)13) ;
                    sb.Append ((char)10) ;
                    PolicyManager._DebugOut( file, sb.ToString() );
                }
                else
                    Console.WriteLine( str );
             }
#endif
        }

#if _DEBUG
        private static bool debug;
        private static readonly bool to_file;
        private const String file = "c:\\com99\\src\\bcl\\debug.txt";
#endif

        [OptionalField(VersionAdded = 2)]
        private ArrayList           m_schemesList;
        [OptionalField(VersionAdded = 2)]
        private ArrayList           m_accessList;

        [OnDeserializing]
        private void OnDeserializing(StreamingContext ctx)
        {
            m_schemesList = null;
            m_accessList = null;
            
            
        }
        

        private const string c_IgnoreUserInfo = ""; // don't need anymore since WebPermission will ignore userinfo anyway. was: @"([^\\/?#]*@)?";
        // not exactly correct syntax but should work fine assuming System.Uri will not accept bogus Uri schemes
        private const string c_AnyScheme = @"([0-9a-z+\-\.]+)://";

        private static readonly char[] c_SomeRegexChars = new char[] {'.', '-', '+', '[', ']', /* rest are unc-only*/ '{', '$', '^', '#', ')', '(', ' '};

        public static readonly string  AnyOtherOriginScheme= CodeConnectAccess.AnyScheme;
        public static readonly string  AbsentOriginScheme  = string.Empty;

        internal NetCodeGroup()
            : base()
        {
            SetDefaults();
        }

        public NetCodeGroup( IMembershipCondition membershipCondition )
            : base( membershipCondition, (PolicyStatement)null )
        {
            SetDefaults();
        }

        //
        // Reset the talkback access to nothing.
        // When a new instance of NetCodeGroup is created it's populated with default talkback rules
        //
        public void ResetConnectAccess()
        {
            m_schemesList = null;
            m_accessList = null;
        }
        //
        // Added public stuff for programmatic support of the talkback access
        // The connectAccess can be null means an empty access (no access) is added
        //
        public void AddConnectAccess(string originScheme, CodeConnectAccess connectAccess)
        {
            if (originScheme == null)
                throw new ArgumentNullException("originScheme");

            if (originScheme != AbsentOriginScheme && originScheme != AnyOtherOriginScheme && !CodeConnectAccess.IsValidScheme(originScheme))
                throw new ArgumentOutOfRangeException("originScheme");

            if (originScheme == AbsentOriginScheme && connectAccess.IsOriginScheme)
                throw new ArgumentOutOfRangeException("connectAccess");

            if (m_schemesList == null)
            {
                m_schemesList = new ArrayList();
                m_accessList = new ArrayList();
            }

            originScheme = originScheme.ToLower(CultureInfo.InvariantCulture);

            for (int i=0; i < m_schemesList.Count; ++i)
            {
                if ((string)m_schemesList[i] == originScheme)
                {
                    // originScheme entry is found and we may want to add nothing to it.
                    if (connectAccess == null)
                        return;

                    ArrayList list = (ArrayList)m_accessList[i];
                    for (i = 0; i < list.Count; ++i)
                    {
                        if (((CodeConnectAccess)list[i]).Equals(connectAccess))
                            return;
                    }
                    list.Add(connectAccess);
                    return;
                }
            }

            // originScheme entry is not found, create a new one.
            m_schemesList.Add(originScheme);
            ArrayList newOriginSchemeList = new ArrayList();
            m_accessList.Add(newOriginSchemeList);

            // we may want to keep it empty.
            if (connectAccess != null)
                newOriginSchemeList.Add(connectAccess);

        }
        //
        // Each DictionaryEntry will contain
        // Key=originScheme and Value=CodeConnectAccess[] array
        //
        public DictionaryEntry[] GetConnectAccessRules()
        {
            if (m_schemesList == null)
                return null;

            DictionaryEntry[] result = new DictionaryEntry[m_schemesList.Count];
            for (int i = 0; i < result.Length; ++i)
            {
                result[i].Key = m_schemesList[i];
                result[i].Value = ((ArrayList)m_accessList[i]).ToArray(typeof(CodeConnectAccess));
            }
            return result;
        }

        public override PolicyStatement Resolve( Evidence evidence )
        {
            if (evidence == null)
                throw new ArgumentNullException("evidence");

            if (this.MembershipCondition.Check( evidence ))
            {
                PolicyStatement thisPolicy = CalculateAssemblyPolicy( evidence );

                IEnumerator enumerator = this.Children.GetEnumerator();
                while (enumerator.MoveNext())
                {
                    PolicyStatement childPolicy = ((CodeGroup)enumerator.Current).Resolve( evidence );

                    if (childPolicy != null)
                    {
                        if (((thisPolicy.Attributes & childPolicy.Attributes) & PolicyStatementAttribute.Exclusive) == PolicyStatementAttribute.Exclusive)
                        {
                            throw new PolicyException(Environment.GetResourceString( "Policy_MultipleExclusive" ));
                        }

                        thisPolicy.GetPermissionSetNoCopy().InplaceUnion( childPolicy.GetPermissionSetNoCopy() );
                        thisPolicy.Attributes = thisPolicy.Attributes | childPolicy.Attributes;
                    }
                }

                return thisPolicy;
            }
            else
            {
                return null;
            }
        }

        /// <internalonly/>
        PolicyStatement IUnionSemanticCodeGroup.InternalResolve( Evidence evidence )
        {
            if (evidence == null)
                throw new ArgumentNullException("evidence");


            if (this.MembershipCondition.Check( evidence ))
            {
                return CalculateAssemblyPolicy( evidence );
            }

            return null;
        }

        public override CodeGroup ResolveMatchingCodeGroups( Evidence evidence )
        {
            if (evidence == null)
                throw new ArgumentNullException("evidence");

            if (this.MembershipCondition.Check( evidence ))
            {
                CodeGroup retGroup = this.Copy();

                retGroup.Children = new ArrayList();

                IEnumerator enumerator = this.Children.GetEnumerator();

                while (enumerator.MoveNext())
                {
                    CodeGroup matchingGroups = ((CodeGroup)enumerator.Current).ResolveMatchingCodeGroups( evidence );

                    // If the child has a policy, we are done.

                    if (matchingGroups != null)
                    {
                        retGroup.AddChild( matchingGroups );
                    }
                }

                return retGroup;

            }
            else
            {
                return null;
            }
        }
        //
        private string EscapeStringForRegex( string str )
        {
            int start = 0;
            int idx;
            System.Text.StringBuilder sb = null;

            while (start < str.Length && (idx = str.IndexOfAny(c_SomeRegexChars, start)) != -1)
            {
                if (sb == null) sb = new System.Text.StringBuilder(str.Length*2);
                sb.Append(str, start, idx - start).Append('\\').Append(str[idx]);
                start = idx+1;
            }
            if (sb == null)
                return str;

            if (start < str.Length)
                sb.Append(str, start, str.Length - start);

            return sb.ToString();
        }


        private SecurityElement CreateWebPermission(String host, String scheme, String port )
        {
            if (scheme == null)
                scheme = string.Empty;

            // If there is no OriginScheme host string, no talk back access is possible
            if (host == null || host.Length == 0)
                return null;

            host = host.ToLower(CultureInfo.InvariantCulture);
            scheme = scheme.ToLower(CultureInfo.InvariantCulture);

            int intPort = CodeConnectAccess.NoPort;
            if (port != null && port.Length != 0)
                intPort = Int32.Parse(port, CultureInfo.InvariantCulture );
            else
                port = string.Empty;

            CodeConnectAccess[] access = FindAccessRulesForScheme(scheme);
            if (access == null || access.Length == 0)
                return null;

            SecurityElement root = new SecurityElement( "IPermission" );

            root.AddAttribute( "class", "System.Net.WebPermission, System, Version=" + ThisAssembly.Version + ", Culture=neutral, PublicKeyToken=" + AssemblyRef.EcmaPublicKeyToken);
            root.AddAttribute( "version", "1" );

            SecurityElement connectAccess = new SecurityElement( "ConnectAccess" );

            host = EscapeStringForRegex(host);
            scheme = EscapeStringForRegex(scheme);
            string uriStr = TryPermissionAsOneString(access, scheme, host, intPort);

            if (uriStr != null)
            {
                SecurityElement uri = new SecurityElement( "URI" );
                uri.AddAttribute( "uri", uriStr );
                connectAccess.AddChild( uri );
            }
            else
            {
                if (port.Length != 0)
                    port = ":" + port;

                for (int i = 0; i < access.Length; ++i)
                {
                    uriStr = GetPermissionAccessElementString(access[i], scheme, host, port);
                    SecurityElement uri = new SecurityElement( "URI" );
                    uri.AddAttribute( "uri", uriStr );
                    connectAccess.AddChild( uri );
                }
            }

            root.AddChild( connectAccess );
            return root;
        }
        //
        //
        //
        private CodeConnectAccess[] FindAccessRulesForScheme(string lowerCaseScheme)
        {
            if (m_schemesList == null)
                return null;

            int i = m_schemesList.IndexOf(lowerCaseScheme);
            if (i == -1)
            {
                // Trying default rule but only if the passed string is not about "no scheme case"
                if (lowerCaseScheme == AbsentOriginScheme || (i = m_schemesList.IndexOf(AnyOtherOriginScheme)) == -1)
                    return null;
            }

            ArrayList accessList = (ArrayList)m_accessList[i];
            return (CodeConnectAccess[])accessList.ToArray(typeof(CodeConnectAccess));
        }
        //
        // This is an attempt to optimize resulting regex if the rules can be combined into one expression string
        //
        private string TryPermissionAsOneString(CodeConnectAccess[] access, string escapedScheme, string escapedHost, int intPort)
        {
            bool noPort = true;
            bool originPort = true;
            bool anyScheme = false;
            int  sameCustomPort = CodeConnectAccess.AnyPort;

            //
            // We can compact rules in one regex if the destination port is the same for all granted accesses.
            // We may have three cases (order is significant)
            // - No port (empty) in the resulting rule
            // - Origin server port that is intPort parameter
            // - Some custom port that is the same for all accesses
            //
            for (int i = 0; i < access.Length; ++i)
            {
                noPort    &= (access[i].IsDefaultPort || (access[i].IsOriginPort && intPort == CodeConnectAccess.NoPort));
                originPort&= (access[i].IsOriginPort  || access[i].Port == intPort);

                if (access[i].Port >= 0)
                {

                    if (sameCustomPort == CodeConnectAccess.AnyPort)
                    {
                        sameCustomPort = access[i].Port;
                    }
                    else if (access[i].Port != sameCustomPort)
                    {
                        // found conflicting ports
                        sameCustomPort = CodeConnectAccess.NoPort;
                    }
                }
                else
                {
                    // Cannot compress Regex if saw at least one "default port" access rule and another one with exact port.
                    sameCustomPort = CodeConnectAccess.NoPort;
                }

                if (access[i].IsAnyScheme)
                    anyScheme = true;
            }

            if (!noPort && !originPort && sameCustomPort == CodeConnectAccess.NoPort)
                return null;

            // We can produce the resulting expression as one string
            System.Text.StringBuilder sb = new System.Text.StringBuilder(c_AnyScheme.Length * access.Length + c_IgnoreUserInfo.Length*2 + escapedHost.Length);
            if (anyScheme)
                sb.Append(c_AnyScheme);
            else
            {
                sb.Append('(');
                int i = 0;
                for (; i < access.Length; ++i)
                {
                    // This is  to avoid output like (http|http|http)
                    int k = 0;
                    for (; k < i; ++k)
                    {
                        if (access[i].Scheme == access[k].Scheme)
                            break;
                    }
                    if (k == i)
                    {
                        if (i != 0)
                            sb.Append('|');
                        sb.Append(access[i].IsOriginScheme? escapedScheme: EscapeStringForRegex(access[i].Scheme));
                    }
                }
                sb.Append(")://");;
            }

            sb.Append(c_IgnoreUserInfo).Append(escapedHost);

            if (noPort) {;}
            else if (originPort) sb.Append(':').Append(intPort);
            else sb.Append(':').Append(sameCustomPort);

            sb.Append("/.*");
            return sb.ToString();
        }
        //
        // This tries to return a single element to be added into resulting WebPermission
        // Returns Null if there is nothing to add.
        //
        private string GetPermissionAccessElementString(CodeConnectAccess access, string escapedScheme, string escapedHost, string strPort)
        {
            System.Text.StringBuilder sb = new System.Text.StringBuilder(c_AnyScheme.Length*2 + c_IgnoreUserInfo.Length + escapedHost.Length);

            if (access.IsAnyScheme)
                sb.Append(c_AnyScheme);
            else if (access.IsOriginScheme)
                sb.Append(escapedScheme).Append("://");
            else
                sb.Append(EscapeStringForRegex(access.Scheme)).Append("://");

            sb.Append(c_IgnoreUserInfo).Append(escapedHost);

            if (access.IsDefaultPort) {;}
            else if (access.IsOriginPort)
                 sb.Append(strPort);
            else sb.Append(':').Append(access.StrPort);

            sb.Append("/.*");
            return sb.ToString();
        }

        internal PolicyStatement CalculatePolicy( String host, String scheme, String port )
        {
            SecurityElement webPerm = CreateWebPermission( host, scheme, port );

            SecurityElement root = new SecurityElement( "PolicyStatement" );
            SecurityElement permSet = new SecurityElement( "PermissionSet" );
            permSet.AddAttribute( "class", "System.Security.PermissionSet" );
            permSet.AddAttribute( "version", "1" );

            if (webPerm != null)
                permSet.AddChild( webPerm );

            root.AddChild( permSet );

            PolicyStatement policy = new PolicyStatement();
            policy.FromXml( root );
            return policy;
        }

        private PolicyStatement CalculateAssemblyPolicy( Evidence evidence )
        {
            IEnumerator evidenceEnumerator = evidence.GetHostEnumerator();

            PolicyStatement thisPolicy = null;

            Site site = null;

            while (evidenceEnumerator.MoveNext())
            {
                Url url = evidenceEnumerator.Current as Url;

                if (url != null)
                {
                    thisPolicy = CalculatePolicy( url.GetURLString().Host, url.GetURLString().Scheme, url.GetURLString().Port );
                }
                else
                {
                    if (site == null)
                        site = evidenceEnumerator.Current as Site;
                }
            }

            if (thisPolicy == null && site != null)
                thisPolicy = CalculatePolicy( site.Name, null, null );

            if (thisPolicy == null)
                thisPolicy = new PolicyStatement( new PermissionSet( false ), PolicyStatementAttribute.Nothing );

            return thisPolicy;
        }

        public override CodeGroup Copy()
        {
            NetCodeGroup group = new NetCodeGroup( this.MembershipCondition );

            group.Name = this.Name;
            group.Description = this.Description;
            if (m_schemesList != null)
            {
                group.m_schemesList = (ArrayList)this.m_schemesList.Clone();
                group.m_accessList = new ArrayList(this.m_accessList.Count);
                for (int i = 0; i < this.m_accessList.Count; ++i)
                    group.m_accessList.Add(((ArrayList)this.m_accessList[i]).Clone());
            }

            IEnumerator enumerator = this.Children.GetEnumerator();

            while (enumerator.MoveNext())
            {
                group.AddChild( (CodeGroup)enumerator.Current );
            }


            return group;
        }

        public override String MergeLogic
        {
            get
            {
                return Environment.GetResourceString( "MergeLogic_Union" );
            }
        }

        public override String PermissionSetName
        {
            get
            {
                return Environment.GetResourceString( "NetCodeGroup_PermissionSet" );
            }
        }


        public override String AttributeString
        {
            get
            {
                return null;
            }
        }
        //
        public override bool Equals( Object o)
        {
            if ((object)this == (object)o)
                return true;

            NetCodeGroup that = (o as NetCodeGroup);

            if (that == null || !base.Equals(that))
                return false;

            if ((this.m_schemesList == null) != (that.m_schemesList == null))
                return false;

            if (this.m_schemesList == null)
                return true;

            if (this.m_schemesList.Count != that.m_schemesList.Count)
                return false;


            for (int i = 0; i < this.m_schemesList.Count; ++i)
            {
                int idx = that.m_schemesList.IndexOf(this.m_schemesList[i]);
                if (idx == -1)
                    return false;

                ArrayList thisList = (ArrayList)this.m_accessList[i];
                ArrayList thatList = (ArrayList)that.m_accessList[idx];
                if (thisList.Count != thatList.Count)
                    return false;

                for (int k = 0; k < thisList.Count; ++k)
                {
                    if (!thatList.Contains(thisList[k]))
                        return false;
                }
            }

            return true;
        }
        //
        //
        public override int GetHashCode()
        {
            return base.GetHashCode() + GetRulesHashCode();
        }
        private int GetRulesHashCode()
        {
            if (m_schemesList == null)
                return 0;

            int result = 0;
            for(int i = 0; i < m_schemesList.Count; ++i)
                result += ((string)m_schemesList[i]).GetHashCode();

            foreach (ArrayList accessList in m_accessList)
                for(int i = 0; i < accessList.Count; ++i)
                    result += ((CodeConnectAccess)accessList[i]).GetHashCode();

            return result;
        }
        //
        protected override void CreateXml( SecurityElement element, PolicyLevel level )
        {
            DictionaryEntry[] rules = GetConnectAccessRules();
            if (rules == null)
                return;

            SecurityElement rulesElement = new SecurityElement("connectAccessRules");

            foreach (DictionaryEntry rule in rules)
            {
                SecurityElement codeOriginElement = new SecurityElement("codeOrigin");
                codeOriginElement.AddAttribute("scheme", (string) rule.Key);
                foreach (CodeConnectAccess access in (CodeConnectAccess[])rule.Value)
                {
                    SecurityElement accessElem = new SecurityElement("connectAccess");
                    accessElem.AddAttribute("scheme", access.Scheme);
                    accessElem.AddAttribute("port", access.StrPort);
                    codeOriginElement.AddChild(accessElem);
                }
                rulesElement.AddChild(codeOriginElement);
            }
            element.AddChild(rulesElement);
        }

        protected override void ParseXml( SecurityElement e, PolicyLevel level )
        {
            //Reset the exiting content
            ResetConnectAccess();

            SecurityElement et = e.SearchForChildByTag("connectAccessRules");

            if (et == null || et.Children == null)
            {
                SetDefaults();
                return;
            }

            foreach(SecurityElement codeOriginElem in et.Children)
            {
                if (codeOriginElem.Tag.Equals("codeOrigin"))
                {
                    string originScheme = codeOriginElem.Attribute("scheme");
                    bool oneAdded = false;

                    if (codeOriginElem.Children != null)
                    {
                        foreach(SecurityElement accessElem in codeOriginElem.Children)
                        {
                            if (accessElem.Tag.Equals("connectAccess"))
                            {
                                string connectScheme = accessElem.Attribute("scheme");
                                string connectPort   = accessElem.Attribute("port");
                                AddConnectAccess(originScheme, new CodeConnectAccess(connectScheme, connectPort));
                                oneAdded = true;
                            }
                            else {
                                // improper tag found, just ignore
                            }
                        }
                    }

                    if (!oneAdded)
                    {
                        //special case as to no talkback access for a given scheme
                        AddConnectAccess(originScheme, null);
                    }

                }
                else {
                    // improper tag found, just ignore
                }
            }
        }

        internal override String GetTypeName()
        {
            return "System.Security.Policy.NetCodeGroup";
        }
        //
        // This method is called at the ctor time to populate default accesses (V1.1 compat)
        //
        private void SetDefaults()
        {
            // No access for file://
            AddConnectAccess("file", null);

            // access fot http://
            AddConnectAccess("http", new CodeConnectAccess("http", CodeConnectAccess.OriginPort));
            AddConnectAccess("http", new CodeConnectAccess("https", CodeConnectAccess.OriginPort));
            /* CONSIDER
            AddConnectAccess("http", new CodeConnectAccess("https", CodeConnectAccess.DefaultPort));
            AddConnectAccess("http", new CodeConnectAccess("ftp", CodeConnectAccess.DefaultPort));
            AddConnectAccess("http", new CodeConnectAccess("soap.tcp", CodeConnectAccess.DefaultPort));
            */

            // access fot https://
            AddConnectAccess("https", new CodeConnectAccess("https", CodeConnectAccess.OriginPort));
            /* CONSIDER
            AddConnectAccess("https", new CodeConnectAccess("soap.tcp", CodeConnectAccess.DefaultPort));
            */


            // access fot ftp://
            /* CONSIDER
            AddConnectAccess("ftp", new CodeConnectAccess("ftp", CodeConnectAccess.OriginPort));
            AddConnectAccess("ftp", new CodeConnectAccess("http", CodeConnectAccess.DefaultPort));
            AddConnectAccess("ftp", new CodeConnectAccess("https", CodeConnectAccess.DefaultPort));
            AddConnectAccess("ftp", new CodeConnectAccess("soap.tcp", CodeConnectAccess.DefaultPort));
            */

            // access for no scheme and for any other scheme
            AddConnectAccess(NetCodeGroup.AbsentOriginScheme, CodeConnectAccess.CreateAnySchemeAccess(CodeConnectAccess.OriginPort));
            AddConnectAccess(NetCodeGroup.AnyOtherOriginScheme, CodeConnectAccess.CreateOriginSchemeAccess(CodeConnectAccess.OriginPort));
        }

    }

}
