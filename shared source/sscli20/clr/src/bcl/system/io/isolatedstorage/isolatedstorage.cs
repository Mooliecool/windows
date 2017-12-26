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

/*============================================================
 *
 * Class:  IsolatedStorage
 *
 *
 * Purpose: Provides access to Persisted Application / Assembly data
 *
 * Date:  Feb 15, 2000
 *
 ===========================================================*/
namespace System.IO.IsolatedStorage {

    using System;
    using System.IO;
    using System.Text;
    using System.Threading;
    using System.Reflection;
    using System.Collections;
    using System.Security;
    using System.Security.Policy;
    using System.Security.Permissions;
    using System.Security.Cryptography;    
    using System.Runtime.Serialization;
    using System.Runtime.CompilerServices;
    using System.Runtime.Serialization.Formatters.Binary;
    using System.Runtime.InteropServices;
    using Microsoft.Win32;

    [Flags, Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum IsolatedStorageScope
    {
        // Dependency in native : COMIsolatedStorage.h

        None       = 0x00,
        User       = 0x01,
        Domain     = 0x02,
        Assembly   = 0x04,
        Roaming    = 0x08,
        Machine    = 0x10,
        Application    = 0x20
    }

    // not serializable
[System.Runtime.InteropServices.ComVisible(true)]
    public abstract class IsolatedStorage : MarshalByRefObject
    {
        // Helper constants
        internal const IsolatedStorageScope c_Assembly = 
                                   (IsolatedStorageScope.User |
                                    IsolatedStorageScope.Assembly);

        internal const IsolatedStorageScope c_Domain = 
                                   (IsolatedStorageScope.User |
                                    IsolatedStorageScope.Assembly |
                                    IsolatedStorageScope.Domain);

        internal const IsolatedStorageScope c_AssemblyRoaming = 
                                   (IsolatedStorageScope.Roaming |
                                    IsolatedStorageScope.User |
                                    IsolatedStorageScope.Assembly);

        internal const IsolatedStorageScope c_DomainRoaming = 
                                   (IsolatedStorageScope.Roaming |
                                    IsolatedStorageScope.User |
                                    IsolatedStorageScope.Assembly |
                                    IsolatedStorageScope.Domain);

        internal const IsolatedStorageScope c_MachineAssembly = 
                                   (IsolatedStorageScope.Machine |
                                    IsolatedStorageScope.Assembly);

        internal const IsolatedStorageScope c_MachineDomain = 
                                   (IsolatedStorageScope.Machine |
                                    IsolatedStorageScope.Assembly |
                                    IsolatedStorageScope.Domain);

        internal const IsolatedStorageScope c_AppUser = 
                                   (IsolatedStorageScope.Application |
                                    IsolatedStorageScope.User);

        internal const IsolatedStorageScope c_AppMachine = 
                                   (IsolatedStorageScope.Application |
                                    IsolatedStorageScope.Machine);

        internal const IsolatedStorageScope c_AppUserRoaming = 
                                   (IsolatedStorageScope.Roaming |
                                    IsolatedStorageScope.Application |
                                    IsolatedStorageScope.User);

        private const String s_StrongName   = "StrongName";
        private const String s_Site         = "Site";
        private const String s_Url          = "Url";
        private const String s_Zone         = "Zone";

        private static Char[] s_Base32Char   = {
                'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 
                'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
                'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 
                'y', 'z', '0', '1', '2', '3', '4', '5'};

        private ulong   m_Quota;
        private bool    m_ValidQuota;
        
        private Object  m_DomainIdentity;
        private Object  m_AssemIdentity;
        private Object  m_AppIdentity;
        
        private String  m_DomainName;
        private String  m_AssemName;
        private String  m_AppName;
        
        private IsolatedStorageScope m_Scope;

        private static IsolatedStorageFilePermission s_PermDomain;
        private static IsolatedStorageFilePermission s_PermMachineDomain;
        private static IsolatedStorageFilePermission s_PermDomainRoaming;
        private static IsolatedStorageFilePermission s_PermAssem;
        private static IsolatedStorageFilePermission s_PermMachineAssem;
        private static IsolatedStorageFilePermission s_PermAssemRoaming;
        private static IsolatedStorageFilePermission s_PermAppUser;
        private static IsolatedStorageFilePermission s_PermAppMachine;
        private static IsolatedStorageFilePermission s_PermAppUserRoaming;
        
        private static SecurityPermission s_PermControlEvidence;
        private static PermissionSet s_PermReflection;
        private static PermissionSet s_PermUnrestricted;
        private static PermissionSet s_PermExecution;

#if _DEBUG
        private static bool s_fDebug = false;
        private static int  s_iDebug = 0;
#endif

        // This one should be a macro, expecting JIT to inline this.
        internal static bool IsRoaming(IsolatedStorageScope scope)
        {
            return ((scope & IsolatedStorageScope.Roaming) != 0);
        }

        internal bool IsRoaming()
        {
            return ((m_Scope & IsolatedStorageScope.Roaming) != 0);
        }

        // This one should be a macro, expecting JIT to inline this.
        internal static bool IsDomain(IsolatedStorageScope scope)
        {
            return ((scope & IsolatedStorageScope.Domain) != 0);
        }

        internal bool IsDomain()
        {
            return ((m_Scope & IsolatedStorageScope.Domain) != 0);
        }


        // This one should be a macro, expecting JIT to inline this.
        internal static bool IsMachine(IsolatedStorageScope scope)
        {
            return ((scope & IsolatedStorageScope.Machine) != 0);
        }

        internal bool IsAssembly()
        {
            return ((m_Scope & IsolatedStorageScope.Assembly) != 0);
        }


        // This one should be a macro, expecting JIT to inline this.
        internal static bool IsApp(IsolatedStorageScope scope)
        {
            return ((scope & IsolatedStorageScope.Application) != 0);
        }

        internal bool IsApp()
        {
            return ((m_Scope & IsolatedStorageScope.Application) != 0);
        }

        private String GetNameFromID(String typeID, String instanceID)
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(typeID);
            sb.Append(SeparatorInternal);
            sb.Append(instanceID);

            return sb.ToString();
        }

        private static String GetPredefinedTypeName(Object o)
        {
            if (o is StrongName)
                return s_StrongName;
            else if (o is Url)
                return s_Url;
            else if (o is Site)
                return s_Site;
            else if (o is Zone)
                return s_Zone;

            return null;
        }

        internal static String GetHash(Stream s)
        {                                           
            const int MAX_BUFFER_SIZE = 1024;
            byte[] buffer = new byte[MAX_BUFFER_SIZE];
            
            // 160 bits SHA1 output as defined in the Secure Hash Standard        
            const int MESSAGE_DIGEST_LENGTH = 20;
            int digestLength = 0;
            byte[] digest = new byte[MESSAGE_DIGEST_LENGTH];
            
            IntPtr hProv = (IntPtr) 0;
            IntPtr hHash = (IntPtr) 0;
            
            if( Win32Native.CryptAcquireContext(out hProv, null, null, 0, 0) == false)
               throw new IsolatedStorageException(Environment.GetResourceString("IsolatedStorage_Exception"));
    
            if( Win32Native.CryptCreateHash(hProv, Win32Native.CALG_SHA1,(IntPtr) 0, 0, out hHash) == false)
               throw new IsolatedStorageException(Environment.GetResourceString("IsolatedStorage_Exception"));            
    
            int bytesRead;      
            do {            
                bytesRead = s.Read(buffer,0,MAX_BUFFER_SIZE);
                if (bytesRead > 0) {
                    if(Win32Native.CryptHashData(hHash, buffer, bytesRead, 0) == false)                      
                    {                                             
                        throw new IsolatedStorageException(Environment.GetResourceString("IsolatedStorage_Exception"));                      
                    }  
                }
            } while (bytesRead > 0);
    
            // perform a sanity check to make sure the digest size equals MESSAGE_DIGEST_LENGTH
            int fourBytes = 4;
            if( Win32Native.CryptGetHashParam(hHash, Win32Native.HP_HASHSIZE, out digestLength, ref fourBytes, 0) == false
                || (digestLength != MESSAGE_DIGEST_LENGTH)
              )
            {
                throw new IsolatedStorageException(Environment.GetResourceString("IsolatedStorage_Exception"));
            }
                    
            if( Win32Native.CryptGetHashParam(hHash, Win32Native.HP_HASHVAL, digest, ref digestLength, 0) == false)
                throw new IsolatedStorageException(Environment.GetResourceString("IsolatedStorage_Exception"));
            
            if( Win32Native.CryptDestroyHash(hHash) == false)
               throw new IsolatedStorageException(Environment.GetResourceString("IsolatedStorage_Exception"));
    
            if( Win32Native.CryptReleaseContext(hProv, 0) == false )
               throw new IsolatedStorageException(Environment.GetResourceString("IsolatedStorage_Exception")); 
    
            return ToBase32StringSuitableForDirName(digest);       
        }

        internal static String ToBase32StringSuitableForDirName(byte[] buff)
        {
            // This routine is optimised to be used with buffs of length 20
            BCLDebug.Assert(((buff.Length % 5) == 0), "Unexpected hash length");

#if _DEBUG
            if (s_fDebug)
            {
                if (s_iDebug >= 10)
                {
                    Console.Write("Stream : ");
                    for (int j = 0; j<buff.Length; ++j)
                        Console.Write("{0} ", buff[j]);

                    Console.WriteLine("");
                }
            }
#endif
            StringBuilder sb = new StringBuilder();
            byte b0, b1, b2, b3, b4;
            int  l, i;
    
            l = buff.Length;
            i = 0;

            // Create l chars using the last 5 bits of each byte.  
            // Consume 3 MSB bits 5 bytes at a time.

            do
            {
                b0 = (i < l) ? buff[i++] : (byte)0;
                b1 = (i < l) ? buff[i++] : (byte)0;
                b2 = (i < l) ? buff[i++] : (byte)0;
                b3 = (i < l) ? buff[i++] : (byte)0;
                b4 = (i < l) ? buff[i++] : (byte)0;

                // Consume the 5 Least significant bits of each byte
                sb.Append(s_Base32Char[b0 & 0x1F]);
                sb.Append(s_Base32Char[b1 & 0x1F]);
                sb.Append(s_Base32Char[b2 & 0x1F]);
                sb.Append(s_Base32Char[b3 & 0x1F]);
                sb.Append(s_Base32Char[b4 & 0x1F]);
    
                // Consume 3 MSB of b0, b1, MSB bits 6, 7 of b3, b4
                sb.Append(s_Base32Char[(
                        ((b0 & 0xE0) >> 5) | 
                        ((b3 & 0x60) >> 2))]);
    
                sb.Append(s_Base32Char[(
                        ((b1 & 0xE0) >> 5) | 
                        ((b4 & 0x60) >> 2))]);
    
                // Consume 3 MSB bits of b2, 1 MSB bit of b3, b4
                
                b2 >>= 5;
    
                BCLDebug.Assert(((b2 & 0xF8) == 0), "Unexpected set bits");
    
                if ((b3 & 0x80) != 0)
                    b2 |= 0x08;
                if ((b4 & 0x80) != 0)
                    b2 |= 0x10;
    
                sb.Append(s_Base32Char[b2]);

            } while (i < l);

#if _DEBUG
            if (s_fDebug)
            {
                if (s_iDebug >= 10)
                    Console.WriteLine("Hash : " + sb.ToString());
            }
#endif
            return sb.ToString();
        }


        private static bool IsValidName(String s)
        {
            for (int i=0; i<s.Length; ++i)
            {
                if (!Char.IsLetter(s[i]) && !Char.IsDigit(s[i]))
                    return false;
            }

            return true;
        }

        private static PermissionSet GetReflectionPermission()
        {
            // Don't sync. OK to create this object more than once.
            if (s_PermReflection == null)
                s_PermReflection = new PermissionSet(
                    PermissionState.Unrestricted);

            return s_PermReflection;
        }

        private static SecurityPermission GetControlEvidencePermission()
        {
            // Don't sync. OK to create this object more than once.
            if (s_PermControlEvidence == null)
                s_PermControlEvidence = new SecurityPermission(
                    SecurityPermissionFlag.ControlEvidence);

            return s_PermControlEvidence;
        }

        private static PermissionSet GetExecutionPermission()
        {
            // Don't sync. OK to create this object more than once.
            if (s_PermExecution == null)
            {
                s_PermExecution = new PermissionSet(
                    PermissionState.None);
                s_PermExecution.AddPermission(
                    new SecurityPermission(SecurityPermissionFlag.Execution));
            }

            return s_PermExecution;
        }

        private static PermissionSet GetUnrestricted()
        {
            // Don't sync. OK to create this object more than once.
            if (s_PermUnrestricted == null)
                s_PermUnrestricted = new PermissionSet(
                    PermissionState.Unrestricted);

            return s_PermUnrestricted;
        }

        protected virtual char SeparatorExternal
        {
            get { return System.IO.Path.DirectorySeparatorChar; }
        }

        protected virtual char SeparatorInternal
        {
            get { return '.'; }
        }

        // gets "amount of space / resource used"

        [CLSCompliant(false)]
        public virtual ulong MaximumSize
        {
            get 
            { 
                if (m_ValidQuota) 
                    return m_Quota; 

                throw new InvalidOperationException(
                    Environment.GetResourceString(
                        "IsolatedStorage_QuotaIsUndefined"));
            }
        }

        [CLSCompliant(false)]
        public virtual ulong CurrentSize
        {
            get
            {
                throw new InvalidOperationException(
                    Environment.GetResourceString(
                        "IsolatedStorage_CurrentSizeUndefined"));
            }
        }

        public Object DomainIdentity
        {
            [SecurityPermissionAttribute(SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlPolicy)]
            get {

                if (IsDomain())
                    return m_DomainIdentity; 

                throw new InvalidOperationException(
                    Environment.GetResourceString(
                        "IsolatedStorage_DomainUndefined"));
            }
        }

        [ComVisible(false)]
        public Object ApplicationIdentity
        {
            [SecurityPermissionAttribute(SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlPolicy)]
            get {

                if (IsApp())
                    return m_AppIdentity; 

                throw new InvalidOperationException(
                    Environment.GetResourceString(
                        "IsolatedStorage_ApplicationUndefined"));
            }
        }

        public Object AssemblyIdentity
        {
            [SecurityPermissionAttribute(SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlPolicy)]
            get {

                if (IsAssembly())
                    return m_AssemIdentity; 

                throw new InvalidOperationException(
                    Environment.GetResourceString(
                        "IsolatedStorage_AssemblyUndefined"));
            }
        }

        // Returns the AppDomain Stream (if present).
        // Sets assem stream
        internal MemoryStream GetIdentityStream(IsolatedStorageScope scope)
        {
            BinaryFormatter bSer;
            MemoryStream    ms;
            Object          o;

            GetReflectionPermission().Assert();

            bSer = new BinaryFormatter();
            ms   = new MemoryStream();
            if (IsApp(scope))
                o = m_AppIdentity;
            else if (IsDomain(scope))
                o = m_DomainIdentity;
            else
                o = m_AssemIdentity;

            if (o != null)
                bSer.Serialize(ms, o);
            ms.Position = 0;
            return ms;
        }

        public IsolatedStorageScope Scope
        {
            get {  return m_Scope;  }
        }

        internal String DomainName
        {
            get {

                if (IsDomain())
                    return m_DomainName;

                throw new InvalidOperationException(
                    Environment.GetResourceString(
                        "IsolatedStorage_DomainUndefined"));
            }
        }

        internal String AssemName
        {
            get {

                if (IsAssembly())
                    return m_AssemName;

                throw new InvalidOperationException(
                    Environment.GetResourceString(
                        "IsolatedStorage_AssemblyUndefined"));
            }
        }

        internal String AppName
        {
            get {

                if (IsApp())
                    return m_AppName;

                throw new InvalidOperationException(
                    Environment.GetResourceString(
                        "IsolatedStorage_ApplicationUndefined"));
            }
        }

        protected void InitStore(IsolatedStorageScope scope,
                                           Type domainEvidenceType, 
                                           Type assemblyEvidenceType)
        {
            Assembly assem;
            AppDomain domain;
            PermissionSet psAllowed, psDenied;

            psAllowed = null;
            psDenied  = null;

            assem = nGetCaller();

            GetControlEvidencePermission().Assert();

            if (IsDomain(scope))
            {
                domain = Thread.GetDomain();

                if (!IsRoaming(scope))  // No quota for roaming
                {
                    domain.nGetGrantSet(out psAllowed, out psDenied);
    
                    if (psAllowed == null)
                        throw new IsolatedStorageException(
                            Environment.GetResourceString(
                                "IsolatedStorage_DomainGrantSet"));
                }

                _InitStore(scope, domain.Evidence, domainEvidenceType,
                            assem.Evidence, assemblyEvidenceType,
                            null,null);
            }
            else
            {
                if (!IsRoaming(scope))  // No quota for roaming
                {
                    assem.nGetGrantSet(out psAllowed, out psDenied);
    
                    if (psAllowed == null)
                        throw new IsolatedStorageException(
                            Environment.GetResourceString(
                                "IsolatedStorage_AssemblyGrantSet"));
                }

                _InitStore(scope, null, null, assem.Evidence, 
                    assemblyEvidenceType,
                    null,null);
            }

            SetQuota(psAllowed, psDenied);
        }

        protected void InitStore(IsolatedStorageScope scope,
                                           Type appEvidenceType)
        {
            Assembly assem;
            AppDomain domain;
            PermissionSet psAllowed, psDenied;

            psAllowed = null;
            psDenied  = null;

            assem = nGetCaller();

            GetControlEvidencePermission().Assert();

            if (IsApp(scope))
            {
                domain = Thread.GetDomain();

                if (!IsRoaming(scope))  // No quota for roaming
                {
                    domain.nGetGrantSet(out psAllowed, out psDenied);
                    if (psAllowed == null)
                        throw new IsolatedStorageException(
                            Environment.GetResourceString(
                                "IsolatedStorage_DomainGrantSet"));
                }

                throw new IsolatedStorageException(
                        Environment.GetResourceString(
                            "IsolatedStorage_ApplicationMissingIdentity"));
            }
            SetQuota(psAllowed, psDenied);
        }

        internal void InitStore(IsolatedStorageScope scope, 
            Object domain, Object assem, Object app)
        {
            Assembly callerAssembly;
            PermissionSet psAllowed = null, psDenied = null;
            Evidence domainEv = null, assemEv = null, appEv = null;

            if (IsApp(scope))
            {
                appEv = new Evidence();
                appEv.AddHost(app);
            }
            else 
            {
                assemEv =  new Evidence();
                assemEv.AddHost(assem);

                if (IsDomain(scope))
                {
                    domainEv = new Evidence();
                    domainEv.AddHost(domain);
                }
            }

            _InitStore(scope, domainEv, null, assemEv, null, appEv, null);

            // Set the quota based on the caller, not the evidence supplied

            if (!IsRoaming(scope))  // No quota for roaming
            {
                callerAssembly = nGetCaller();

                GetControlEvidencePermission().Assert();
                callerAssembly.nGetGrantSet(out psAllowed, out psDenied);
                
                if (psAllowed == null)
                    throw new IsolatedStorageException(
                        Environment.GetResourceString(
                            "IsolatedStorage_AssemblyGrantSet"));
            }

            // This can be called only by trusted assemblies.
            // This quota really does not correspond to the permissions
            // granted for this evidence.
            SetQuota(psAllowed, psDenied);
        }

        internal void InitStore(IsolatedStorageScope scope,
            Evidence domainEv, Type domainEvidenceType,
            Evidence assemEv, Type assemEvidenceType,
            Evidence appEv, Type appEvidenceType)            
        {
            PermissionSet psAllowed = null, psDenied = null;

            if (!IsRoaming(scope))
            {
                if (IsApp(scope))
                {
                    psAllowed = SecurityManager.ResolvePolicy(
                        appEv, GetExecutionPermission(),  GetUnrestricted(),
                        null, out psDenied);
    
                    if (psAllowed == null)
                        throw new IsolatedStorageException(
                            Environment.GetResourceString(
                                "IsolatedStorage_ApplicationGrantSet"));
                }
                else if (IsDomain(scope))
                {
                    psAllowed = SecurityManager.ResolvePolicy(
                        domainEv, GetExecutionPermission(),  GetUnrestricted(),
                        null, out psDenied);
    
                    if (psAllowed == null)
                        throw new IsolatedStorageException(
                            Environment.GetResourceString(
                                "IsolatedStorage_DomainGrantSet"));
                }
                else
                {
                    psAllowed = SecurityManager.ResolvePolicy(
                        assemEv, GetExecutionPermission(),  GetUnrestricted(),
                        null, out psDenied);
    
                    if (psAllowed == null)
                        throw new IsolatedStorageException(
                            Environment.GetResourceString(
                                "IsolatedStorage_AssemblyGrantSet"));
                }
            }

            _InitStore(scope, domainEv, domainEvidenceType,
                assemEv, assemEvidenceType,
                appEv, appEvidenceType);

            SetQuota(psAllowed, psDenied);
        }

        internal bool InitStore(IsolatedStorageScope scope, 
                Stream domain, Stream assem, Stream app, String domainName, String assemName, String appName)
        {
            BinaryFormatter bSer;

            try {

                GetReflectionPermission().Assert();
    
                bSer = new BinaryFormatter();
    
                if (IsApp(scope))
                {
                    // Get the Application Info
                    m_AppIdentity = bSer.Deserialize(app);
                    m_AppName = appName;
                }
                else
                {
                    // Get the Assem Info
                    m_AssemIdentity = bSer.Deserialize(assem);
                    m_AssemName = assemName;
        
                    if (IsDomain(scope))
                    {
                        // Get the AppDomain Info
                        m_DomainIdentity = bSer.Deserialize(domain);
                        m_DomainName = domainName;
                    }
                }

            }
            catch
            {
                return false;
            }

            BCLDebug.Assert(m_ValidQuota == false, "Quota should be invalid here");

            m_Scope = scope;

            return true;
        }

        private void _InitStore(IsolatedStorageScope scope,
                Evidence domainEv, Type domainEvidenceType, 
                Evidence assemEv, Type assemblyEvidenceType,
                Evidence appEv, Type appEvidenceType)
        {

            VerifyScope(scope);

            // If its app-scoped, we only use the appId and appName. Else assume assembly evidence present
            // and check if we need domain scoping too

            // Input arg checks
            if (IsApp(scope))
            {
                if (appEv == null) 
                    throw new IsolatedStorageException(
                        Environment.GetResourceString(
                            "IsolatedStorage_ApplicationMissingIdentity"));
            }
            else 
            {
                if (assemEv == null)
                    throw new IsolatedStorageException(
                        Environment.GetResourceString(
                            "IsolatedStorage_AssemblyMissingIdentity"));

                if (IsDomain(scope) && (domainEv == null))
                    throw new IsolatedStorageException(
                        Environment.GetResourceString(
                            "IsolatedStorage_DomainMissingIdentity"));
            }    


            // Security checks
            DemandPermission(scope);

            String typeHash = null, instanceHash = null;

            if (IsApp(scope)) 
            {
                m_AppIdentity = GetAccountingInfo(appEv, appEvidenceType, 
                    IsolatedStorageScope.Application, out typeHash, out instanceHash);
                m_AppName = GetNameFromID(typeHash, instanceHash);
            }
            else 
            {
                m_AssemIdentity = GetAccountingInfo(
                                    assemEv, assemblyEvidenceType, IsolatedStorageScope.Assembly,
                                    out typeHash, out instanceHash);

                m_AssemName = GetNameFromID(typeHash, instanceHash);

                if (IsDomain(scope))
                {
                    m_DomainIdentity = GetAccountingInfo(domainEv, domainEvidenceType, 
                                        IsolatedStorageScope.Domain, out typeHash, out instanceHash);

                    m_DomainName = GetNameFromID(typeHash, instanceHash);
                }
            }
            m_Scope = scope;
        }

        private static Object GetAccountingInfo(
                Evidence evidence, Type evidenceType, IsolatedStorageScope fAssmDomApp,
                out String typeName, out String instanceName)
        {
            Object o, oNormalized = null;

            MemoryStream    ms;
            BinaryWriter    bw;
            BinaryFormatter bSer;

            o = _GetAccountingInfo(evidence, evidenceType, fAssmDomApp, 
                    out oNormalized);

            // Get the type name
            typeName = GetPredefinedTypeName(o);

            if (typeName == null)
            {
                // This is not a predefined type. Serialize the type
                // and get a hash for the serialized stream

                GetReflectionPermission().Assert();
                ms   = new MemoryStream();
                bSer = new BinaryFormatter();
                bSer.Serialize(ms, o.GetType());
                ms.Position = 0;
                typeName = GetHash(ms);

#if _DEBUG
                DebugLog(o.GetType(), ms);
#endif
            }

            instanceName = null;

            // Get the normalized instance name if present.
            if (oNormalized != null)
            {
                if (oNormalized is Stream)
                {
                    instanceName = GetHash((Stream)oNormalized);
                }
                else if (oNormalized is String)
                {
                    if (IsValidName((String)oNormalized))
                    {
                        instanceName = (String)oNormalized;
                    }
                    else
                    {
                        // The normalized name has illegal chars
                        // serialize and get the hash.

                        ms = new MemoryStream();
                        bw = new BinaryWriter(ms);
                        bw.Write((String)oNormalized);
                        ms.Position = 0;
                        instanceName = GetHash(ms);
#if _DEBUG
                        DebugLog(oNormalized, ms);
#endif
                    }
                }
                
            }
            else
            {
                oNormalized = o;
            }

            if (instanceName == null)
            {
                // Serialize the instance and  get the hash for the 
                // serialized stream

                GetReflectionPermission().Assert();
                ms   = new MemoryStream();
                bSer = new BinaryFormatter();
                bSer.Serialize(ms, oNormalized);
                ms.Position = 0;
                instanceName = GetHash(ms);

#if _DEBUG
                DebugLog(oNormalized, ms);
#endif
            }

            return o;
        }

        private static Object _GetAccountingInfo(
                    Evidence evidence, Type evidenceType, IsolatedStorageScope fAssmDomApp,
                    out Object oNormalized)
        {
            Object          o = null;
            IEnumerator     e;

            BCLDebug.Assert(evidence != null, "evidence != null");

            e = evidence.GetHostEnumerator();

            if (evidenceType == null)
            {
                // Caller does not have any preference
                // Order of preference is Strong Name, Url, Site

                StrongName  sn   = null;
                Url         url  = null;
                Site        site = null;
                Zone        zone = null;

                while (e.MoveNext())
                {
                    o = e.Current;

                    if (o is StrongName)
                        sn = (StrongName) o;
                    else if (o is Url)
                        url = (Url) o;
                    else if (o is Site)
                        site = (Site) o;
                    else if (o is Zone)
                        zone = (Zone) o;
                }

                if (sn != null)
                {
                    o = sn;
                }
                else if (url != null)
                {
                    o = url;
                }
                else if (site != null)
                {
                    o = site;
                }
                else if (zone != null)
                {
                    o = zone; 
                } 
                else
                {
                    // The evidence object can have tons of other objects
                    // creatd by the policy system. Ignore those.

                    if (fAssmDomApp == IsolatedStorageScope.Domain)
                        throw new IsolatedStorageException(
                            Environment.GetResourceString(
                                "IsolatedStorage_DomainNoEvidence"));
                    else if (fAssmDomApp == IsolatedStorageScope.Application)
                        throw new IsolatedStorageException(
                            Environment.GetResourceString(
                                "IsolatedStorage_ApplicationNoEvidence"));
                    else
                        throw new IsolatedStorageException(
                            Environment.GetResourceString(
                                "IsolatedStorage_AssemblyNoEvidence"));
                }
            }
            else
            {
                Object obj;
                while (e.MoveNext())
                {
                    obj = e.Current;

                    if (obj.GetType().Equals(evidenceType))
                    {
                        o = obj;
                        break;
                    }
                }

                if (o == null)
                {
                    if (fAssmDomApp == IsolatedStorageScope.Domain)
                        throw new IsolatedStorageException(
                            Environment.GetResourceString(
                                "IsolatedStorage_DomainNoEvidence"));
                    else if (fAssmDomApp == IsolatedStorageScope.Application)
                        throw new IsolatedStorageException(
                            Environment.GetResourceString(
                                "IsolatedStorage_ApplicationNoEvidence"));
                    else
                        throw new IsolatedStorageException(
                            Environment.GetResourceString(
                                "IsolatedStorage_AssemblyNoEvidence"));
                }
            }

            // For startup Perf, Url, Site, StrongName types don't implement
            // INormalizeForIsolatedStorage interface, instead they have
            // Normalize() method.

            if (o is INormalizeForIsolatedStorage)
            {
                oNormalized = ((INormalizeForIsolatedStorage)o).Normalize(); 
            }
            else if (o is StrongName)
            {
                oNormalized = ((StrongName)o).Normalize(); 
            }
            else if (o is Url)
            {
                oNormalized = ((Url)o).Normalize(); 
            }
            else if (o is Site)
            {
                oNormalized = ((Site)o).Normalize(); 
            }
            else if (o is Zone)
            {
                oNormalized = ((Zone)o).Normalize(); 
            }
            else
            {
                oNormalized = null;
            }

            return o;
        }

        private static void DemandPermission(IsolatedStorageScope scope)
        {
            IsolatedStorageFilePermission ip = null;

            // Ok to create more than one instnace of s_PermXXX, the last one 
            // will be shared. No need to synchronize.

            // First check for permissions

            switch (scope)
            {
            case c_Domain:

                if (s_PermDomain == null)
                    s_PermDomain = new IsolatedStorageFilePermission(
                        IsolatedStorageContainment.DomainIsolationByUser, 
                        0, false);
                ip = s_PermDomain;
                break;

            case c_Assembly:
                if (s_PermAssem == null)
                    s_PermAssem = new IsolatedStorageFilePermission(
                        IsolatedStorageContainment.AssemblyIsolationByUser, 
                        0, false);
                ip = s_PermAssem;
                break;

            case c_DomainRoaming:
                if (s_PermDomainRoaming == null)
                    s_PermDomainRoaming = new IsolatedStorageFilePermission(
                        IsolatedStorageContainment.DomainIsolationByRoamingUser,
                        0, false);
                ip = s_PermDomainRoaming;
                break;

            case c_AssemblyRoaming: 
                if (s_PermAssemRoaming == null)
                    s_PermAssemRoaming = new IsolatedStorageFilePermission(
                        IsolatedStorageContainment.AssemblyIsolationByRoamingUser, 
                        0, false);
                ip = s_PermAssemRoaming;
                break;
            case c_MachineDomain:

                if (s_PermMachineDomain == null)
                    s_PermMachineDomain = new IsolatedStorageFilePermission(
                        IsolatedStorageContainment.DomainIsolationByMachine, 
                        0, false);
                ip = s_PermMachineDomain;
                break;

            case c_MachineAssembly:
                if (s_PermMachineAssem == null)
                    s_PermMachineAssem = new IsolatedStorageFilePermission(
                        IsolatedStorageContainment.AssemblyIsolationByMachine, 
                        0, false);
                ip = s_PermMachineAssem;
                break;

            case c_AppUser:
                if (s_PermAppUser == null)
                    s_PermAppUser = new IsolatedStorageFilePermission(
                        IsolatedStorageContainment.ApplicationIsolationByUser, 
                        0, false);
                ip = s_PermAppUser;
                break;

            case c_AppMachine:
                if (s_PermAppMachine == null)
                    s_PermAppMachine = new IsolatedStorageFilePermission(
                        IsolatedStorageContainment.ApplicationIsolationByMachine, 
                        0, false);
                ip = s_PermAppMachine;
                break;

            case c_AppUserRoaming:
                if (s_PermAppUserRoaming== null)
                    s_PermAppUserRoaming= new IsolatedStorageFilePermission(
                        IsolatedStorageContainment.ApplicationIsolationByRoamingUser, 
                        0, false);
                ip = s_PermAppUserRoaming;
                break;

#if _DEBUG
            default:
                BCLDebug.Assert(false, "Invalid scope");
                break;
#endif
            }

            ip.Demand();
        }

        internal static void VerifyScope(IsolatedStorageScope scope)
        {
            // The only valid ones are the ones that have a helper constant defined above (c_*)

            if ((scope == c_Domain) || (scope == c_Assembly) ||
                (scope == c_DomainRoaming) || (scope == c_AssemblyRoaming) ||
                (scope == c_MachineDomain) || (scope == c_MachineAssembly) ||
                (scope == c_AppUser) || (scope == c_AppMachine) ||
                (scope == c_AppUserRoaming))
                return;

            throw new ArgumentException(
                    Environment.GetResourceString(
                        "IsolatedStorage_Scope_Invalid"));
        }

        internal void SetQuota(PermissionSet psAllowed, PermissionSet psDenied)
        {
            IsolatedStoragePermission ispAllowed, ispDenied;

            ispAllowed = GetPermission(psAllowed);

            m_Quota = 0;

            if (ispAllowed != null)
            {
                if (ispAllowed.IsUnrestricted())
                    m_Quota = Int64.MaxValue;
                else
                    m_Quota = (ulong) ispAllowed.UserQuota;
            }

            if (psDenied != null)
            {
                ispDenied = GetPermission(psDenied);

                if (ispDenied != null)
                {
                    if (ispDenied.IsUnrestricted())
                    {
                        m_Quota = 0;
                    }
                    else
                    {
                        ulong denied = (ulong) ispDenied.UserQuota;
        
                        if (denied > m_Quota)
                            m_Quota = 0;
                        else
                            m_Quota -= denied;
                    }
                }
            }

            m_ValidQuota = true;

#if _DEBUG
            if (s_fDebug)
            {
                if (s_iDebug >= 1) {
                    if (psAllowed != null)
                        Console.WriteLine("Allowed PS : " + psAllowed);
                    if (psDenied != null)
                        Console.WriteLine("Denied PS : " + psDenied);
                }
            }
#endif
        }

#if _DEBUG
        private static void DebugLog(Object o, MemoryStream ms)
        {
            if (s_fDebug)
            {
                if (s_iDebug >= 1)
                    Console.WriteLine(o.ToString());

                if (s_iDebug >= 10)
                {
                    byte[] p = ms.GetBuffer();
    
                    for (int _i=0; _i<ms.Length; ++_i)
                    {
                        Console.Write(" ");
                        Console.Write(p[_i]);
                    }
    
                    Console.WriteLine("");
                }
            }
        }
#endif

        public abstract void Remove();

        protected abstract IsolatedStoragePermission GetPermission(PermissionSet ps);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern Assembly nGetCaller();
    }
}

