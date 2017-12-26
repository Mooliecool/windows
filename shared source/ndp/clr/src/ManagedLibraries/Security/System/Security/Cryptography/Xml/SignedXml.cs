// ==++==
// 
//   Copyright (c) Microsoft Corporation.  All rights reserved.
// 
// ==--==
// <OWNER>[....]</OWNER>
// 

//
// SignedXml.cs
//
// 21 [....] 2000
// 

namespace System.Security.Cryptography.Xml
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.Collections.ObjectModel;
    using System.Globalization;
    using System.IO;
    using System.Runtime.InteropServices;
    using System.Security.Cryptography;
    using System.Security.Cryptography.X509Certificates;
    using System.Security.Permissions;
    using System.Xml;
    using Microsoft.Win32;

    [System.Security.Permissions.HostProtection(MayLeakOnAbort = true)]
    public class SignedXml {
        /// <internalonly/>
        protected Signature m_signature;
        /// <internalonly/>
        protected string m_strSigningKeyName;

        private AsymmetricAlgorithm m_signingKey;
        private XmlDocument m_containingDocument = null;
        private IEnumerator m_keyInfoEnum = null;
        private X509Certificate2Collection m_x509Collection = null;
        private IEnumerator m_x509Enum = null;

        private bool[] m_refProcessed = null;
        private int[] m_refLevelCache = null;

        internal XmlResolver m_xmlResolver = null;
        internal XmlElement m_context = null;
        private bool m_bResolverSet = false;

        private Func<SignedXml, bool> m_signatureFormatValidator = DefaultSignatureFormatValidator;
        private Collection<string> m_safeCanonicalizationMethods;

        // Built in canonicalization algorithm URIs
        private static IList<string> s_knownCanonicalizationMethods = null;

        // additional HMAC Url identifiers
        private const string XmlDsigMoreHMACMD5Url = "http://www.w3.org/2001/04/xmldsig-more#hmac-md5";
        private const string XmlDsigMoreHMACSHA256Url = "http://www.w3.org/2001/04/xmldsig-more#hmac-sha256";
        private const string XmlDsigMoreHMACSHA384Url = "http://www.w3.org/2001/04/xmldsig-more#hmac-sha384";
        private const string XmlDsigMoreHMACSHA512Url = "http://www.w3.org/2001/04/xmldsig-more#hmac-sha512";
        private const string XmlDsigMoreHMACRIPEMD160Url = "http://www.w3.org/2001/04/xmldsig-more#hmac-ripemd160";

        // defines the XML encryption processing rules
        private EncryptedXml m_exml = null;

        //
        // public constant Url identifiers most frequently used within the XML Signature classes
        //

        public const string XmlDsigNamespaceUrl = "http://www.w3.org/2000/09/xmldsig#";
        public const string XmlDsigMinimalCanonicalizationUrl = "http://www.w3.org/2000/09/xmldsig#minimal";
        public const string XmlDsigCanonicalizationUrl = XmlDsigC14NTransformUrl;
        public const string XmlDsigCanonicalizationWithCommentsUrl = XmlDsigC14NWithCommentsTransformUrl;

        public const string XmlDsigSHA1Url = "http://www.w3.org/2000/09/xmldsig#sha1";
        public const string XmlDsigDSAUrl = "http://www.w3.org/2000/09/xmldsig#dsa-sha1";
        public const string XmlDsigRSASHA1Url = "http://www.w3.org/2000/09/xmldsig#rsa-sha1";
        public const string XmlDsigHMACSHA1Url = "http://www.w3.org/2000/09/xmldsig#hmac-sha1";

        public const string XmlDsigC14NTransformUrl = "http://www.w3.org/TR/2001/REC-xml-c14n-20010315"; 
        public const string XmlDsigC14NWithCommentsTransformUrl = "http://www.w3.org/TR/2001/REC-xml-c14n-20010315#WithComments"; 
        public const string XmlDsigExcC14NTransformUrl = "http://www.w3.org/2001/10/xml-exc-c14n#";
        public const string XmlDsigExcC14NWithCommentsTransformUrl = "http://www.w3.org/2001/10/xml-exc-c14n#WithComments"; 
        public const string XmlDsigBase64TransformUrl = "http://www.w3.org/2000/09/xmldsig#base64";
        public const string XmlDsigXPathTransformUrl = "http://www.w3.org/TR/1999/REC-xpath-19991116";
        public const string XmlDsigXsltTransformUrl = "http://www.w3.org/TR/1999/REC-xslt-19991116";
        public const string XmlDsigEnvelopedSignatureTransformUrl = "http://www.w3.org/2000/09/xmldsig#enveloped-signature";
        public const string XmlDecryptionTransformUrl = "http://www.w3.org/2002/07/decrypt#XML";
        public const string XmlLicenseTransformUrl = "urn:mpeg:mpeg21:2003:01-REL-R-NS:licenseTransform";

        //
        // public constructors
        //

        public SignedXml() {
            Initialize(null);
        }

        public SignedXml(XmlDocument document) {
            if (document == null)
                throw new ArgumentNullException("document");
            Initialize(document.DocumentElement);
        }

        public SignedXml(XmlElement elem) {
            if (elem == null)
                throw new ArgumentNullException("elem");
            Initialize(elem);
        }

        private void Initialize (XmlElement element) {
            m_containingDocument = (element == null ? null : element.OwnerDocument);
            m_context = element;
            m_signature = new Signature();
            m_signature.SignedXml = this;
            m_signature.SignedInfo = new SignedInfo();
            m_signingKey = null;

            m_safeCanonicalizationMethods = new Collection<string>(KnownCanonicalizationMethods);
        }

        //
        // public properties
        //

        /// <internalonly/>
        public string SigningKeyName {
            get { return m_strSigningKeyName; }
            set { m_strSigningKeyName = value; }
        }

        [ComVisible(false)]
        public XmlResolver Resolver {
            // This property only has a setter. The rationale for this is that we don't have a good value
            // to return when it has not been explicitely set, as we are using XmlSecureResolver by default
            set { 
                m_xmlResolver = value;
                m_bResolverSet = true;
            }
        }

        internal bool ResolverSet {
            get { return m_bResolverSet; }
        }

        public Func<SignedXml, bool> SignatureFormatValidator {
            get { return m_signatureFormatValidator; }
            set { m_signatureFormatValidator = value; }
        }

        public Collection<string> SafeCanonicalizationMethods {
            get { return m_safeCanonicalizationMethods; }
        }

        public AsymmetricAlgorithm SigningKey {
            get { return m_signingKey; }
            set { m_signingKey = value; }
        }

        [ComVisible(false)]
        public EncryptedXml EncryptedXml {
            get {
                if (m_exml == null)
                    m_exml = new EncryptedXml(m_containingDocument); // default processing rules
                return m_exml; 
            }
            set { m_exml = value; }
        }

        public Signature Signature {
            get { return m_signature; }
        }

        public SignedInfo SignedInfo {
            get { return m_signature.SignedInfo; }
        }

        public string SignatureMethod  {
            get { return m_signature.SignedInfo.SignatureMethod; }
        }

        public string SignatureLength {
            get { return m_signature.SignedInfo.SignatureLength; }
        }

        public byte[] SignatureValue {
            get { return m_signature.SignatureValue; }
        }

        public KeyInfo KeyInfo {
            get { return m_signature.KeyInfo; }
            set { m_signature.KeyInfo = value; }
        }

        public XmlElement GetXml() {
            // If we have a document context, then return a signature element in this context
            if (m_containingDocument != null)
                return m_signature.GetXml(m_containingDocument);
            else
                return m_signature.GetXml();
        }

        public void LoadXml(XmlElement value) {
            if (value == null)
                throw new ArgumentNullException("value");

            m_signature.LoadXml(value);

            if (m_context == null) {
                m_context = value;
            }

            bCacheValid = false;
        }

        //
        // public methods
        //

        public void AddReference(Reference reference) {
            m_signature.SignedInfo.AddReference(reference);
        }

        public void AddObject(DataObject dataObject) {
            m_signature.AddObject(dataObject);
        }

        public bool CheckSignature() {
            AsymmetricAlgorithm signingKey;
            return CheckSignatureReturningKey(out signingKey);
        }

        public bool CheckSignatureReturningKey(out AsymmetricAlgorithm signingKey) {
            SignedXmlDebugLog.LogBeginSignatureVerification(this, m_context);

            signingKey = null;
            bool bRet = false;
            AsymmetricAlgorithm key = null;

            if (!CheckSignatureFormat()) {
                return false;
            }

            do {
                key = GetPublicKey();
                if (key != null) {
                    bRet = CheckSignature(key);
                    SignedXmlDebugLog.LogVerificationResult(this, key, bRet);
                }
            } while (key != null && bRet == false);

            signingKey = key;
            return bRet;
        }

        public bool CheckSignature(AsymmetricAlgorithm key) {
            if (!CheckSignatureFormat()) {
                return false;
            }

            if (!CheckSignedInfo(key)) {
                SignedXmlDebugLog.LogVerificationFailure(this, SecurityResources.GetResourceString("Log_VerificationFailed_SignedInfo"));
                return false;
            }

            // Now is the time to go through all the references and see if their DigestValues are good
            if (!CheckDigestedReferences()) {
                SignedXmlDebugLog.LogVerificationFailure(this, SecurityResources.GetResourceString("Log_VerificationFailed_References"));
                return false;
            }

            SignedXmlDebugLog.LogVerificationResult(this, key, true);
            return true;
        }

        public bool CheckSignature(KeyedHashAlgorithm macAlg) {
            if (!CheckSignatureFormat()) {
                return false;
            }

            if (!CheckSignedInfo(macAlg)) {
                SignedXmlDebugLog.LogVerificationFailure(this, SecurityResources.GetResourceString("Log_VerificationFailed_SignedInfo"));
                return false;
            }

            if (!CheckDigestedReferences()) {
                SignedXmlDebugLog.LogVerificationFailure(this, SecurityResources.GetResourceString("Log_VerificationFailed_References"));
                return false;
            }

            SignedXmlDebugLog.LogVerificationResult(this, macAlg, true);
            return true;
        }

        [ComVisible(false)]
        [SecuritySafeCritical]
        public bool CheckSignature(X509Certificate2 certificate, bool verifySignatureOnly) {
            if (!CheckSignature(certificate.PublicKey.Key)) {
                return false;
            }

            if (verifySignatureOnly) {
                SignedXmlDebugLog.LogVerificationResult(this, certificate, true);
                return true;
            }

            // Check key usages to make sure it is good for signing.
            foreach (X509Extension extension in certificate.Extensions) {
                if (String.Compare(extension.Oid.Value, CAPI.szOID_KEY_USAGE, StringComparison.OrdinalIgnoreCase) == 0) {
                    X509KeyUsageExtension keyUsage = new X509KeyUsageExtension();
                    keyUsage.CopyFrom(extension);
                    SignedXmlDebugLog.LogVerifyKeyUsage(this, certificate, keyUsage);

                    bool validKeyUsage = (keyUsage.KeyUsages & X509KeyUsageFlags.DigitalSignature) != 0 ||
                                         (keyUsage.KeyUsages & X509KeyUsageFlags.NonRepudiation) != 0;

                    if (!validKeyUsage) {
                        SignedXmlDebugLog.LogVerificationFailure(this, SecurityResources.GetResourceString("Log_VerificationFailed_X509KeyUsage"));
                        return false;
                    }
                    break;
                }
            }

            // Do the chain verification to make sure the certificate is valid.
            X509Chain chain = new X509Chain();
            chain.ChainPolicy.ExtraStore.AddRange(BuildBagOfCerts());
            bool chainVerified = chain.Build(certificate);
            SignedXmlDebugLog.LogVerifyX509Chain(this, chain, certificate);

            if (!chainVerified) {
                SignedXmlDebugLog.LogVerificationFailure(this, SecurityResources.GetResourceString("Log_VerificationFailed_X509Chain"));
                return false;
            }

            SignedXmlDebugLog.LogVerificationResult(this, certificate, true);
            return true;
        }

        public void ComputeSignature() {
            SignedXmlDebugLog.LogBeginSignatureComputation(this, m_context);

            BuildDigestedReferences();

            // Load the key
            AsymmetricAlgorithm key = SigningKey;

            if (key == null)
                throw new CryptographicException(SecurityResources.GetResourceString("Cryptography_Xml_LoadKeyFailed"));

            // Check the signature algorithm associated with the key so that we can accordingly set the signature method
            if (SignedInfo.SignatureMethod == null) {
                if (key is DSA) {
                    SignedInfo.SignatureMethod = XmlDsigDSAUrl;
                } else if (key is RSA) {
                    // Default to RSA-SHA1
                    if (SignedInfo.SignatureMethod == null) 
                        SignedInfo.SignatureMethod = XmlDsigRSASHA1Url;
                } else {
                    throw new CryptographicException(SecurityResources.GetResourceString("Cryptography_Xml_CreatedKeyFailed"));
                }
            }

            // See if there is a signature description class defined in the Config file
            SignatureDescription signatureDescription = CryptoConfig.CreateFromName(SignedInfo.SignatureMethod) as SignatureDescription;
            if (signatureDescription == null)
                throw new CryptographicException(SecurityResources.GetResourceString("Cryptography_Xml_SignatureDescriptionNotCreated"));
            HashAlgorithm hashAlg = signatureDescription.CreateDigest();
            if (hashAlg == null)
                throw new CryptographicException(SecurityResources.GetResourceString("Cryptography_Xml_CreateHashAlgorithmFailed"));
            byte[] hashvalue = GetC14NDigest(hashAlg);
            AsymmetricSignatureFormatter asymmetricSignatureFormatter = signatureDescription.CreateFormatter(key);

            SignedXmlDebugLog.LogSigning(this, key, signatureDescription, hashAlg, asymmetricSignatureFormatter);
            m_signature.SignatureValue = asymmetricSignatureFormatter.CreateSignature(hashAlg);
        }

        public void ComputeSignature(KeyedHashAlgorithm macAlg) {
            if (macAlg == null)
                throw new ArgumentNullException("macAlg");

            HMAC hash = macAlg as HMAC;
            if(hash == null)
                throw new CryptographicException(SecurityResources.GetResourceString("Cryptography_Xml_SignatureMethodKeyMismatch"));

            int signatureLength;
            if (m_signature.SignedInfo.SignatureLength == null)
                signatureLength = hash.HashSize;
            else
                signatureLength = Convert.ToInt32(m_signature.SignedInfo.SignatureLength, null);
            // signatureLength should be less than hash size
            if (signatureLength < 0 || signatureLength > hash.HashSize)
                throw new CryptographicException(SecurityResources.GetResourceString("Cryptography_Xml_InvalidSignatureLength"));
            if (signatureLength % 8 != 0)
                throw new CryptographicException(SecurityResources.GetResourceString("Cryptography_Xml_InvalidSignatureLength2"));

            BuildDigestedReferences();
            switch (hash.HashName) {
            case "SHA1":
                SignedInfo.SignatureMethod = SignedXml.XmlDsigHMACSHA1Url;
                break;
            case "SHA256":
                SignedInfo.SignatureMethod = SignedXml.XmlDsigMoreHMACSHA256Url;
                break;
            case "SHA384":
                SignedInfo.SignatureMethod = SignedXml.XmlDsigMoreHMACSHA384Url;
                break;
            case "SHA512":
                SignedInfo.SignatureMethod = SignedXml.XmlDsigMoreHMACSHA512Url;
                break;
            case "MD5":
                SignedInfo.SignatureMethod = SignedXml.XmlDsigMoreHMACMD5Url;
                break;
            case "RIPEMD160":
                SignedInfo.SignatureMethod = SignedXml.XmlDsigMoreHMACRIPEMD160Url;
                break;
            default:
                throw new CryptographicException(SecurityResources.GetResourceString("Cryptography_Xml_SignatureMethodKeyMismatch"));
            }

            byte[] hashValue = GetC14NDigest(hash);

            SignedXmlDebugLog.LogSigning(this, hash);
            m_signature.SignatureValue = new byte[signatureLength / 8];
            Buffer.BlockCopy(hashValue, 0, m_signature.SignatureValue, 0, signatureLength / 8);
        }

        //
        // virtual methods
        //

        protected virtual AsymmetricAlgorithm GetPublicKey() {
            if (KeyInfo == null)
                throw new CryptographicException(SecurityResources.GetResourceString("Cryptography_Xml_KeyInfoRequired"));

            if (m_x509Enum != null) {
                AsymmetricAlgorithm key = GetNextCertificatePublicKey();
                if (key != null)
                    return key;
            }

            if (m_keyInfoEnum == null)
                m_keyInfoEnum = KeyInfo.GetEnumerator();

            // In our implementation, we move to the next KeyInfo clause which is an RSAKeyValue, DSAKeyValue or KeyInfoX509Data
            while (m_keyInfoEnum.MoveNext()) {
                RSAKeyValue rsaKeyValue = m_keyInfoEnum.Current as RSAKeyValue;
                if (rsaKeyValue != null) 
                    return rsaKeyValue.Key;

                DSAKeyValue dsaKeyValue = m_keyInfoEnum.Current as DSAKeyValue;
                if (dsaKeyValue != null) 
                    return dsaKeyValue.Key;

                KeyInfoX509Data x509Data = m_keyInfoEnum.Current as KeyInfoX509Data;
                if (x509Data != null) {
                    m_x509Collection = Utils.BuildBagOfCerts (x509Data, CertUsageType.Verification);
                    if (m_x509Collection.Count > 0) {
                        m_x509Enum = m_x509Collection.GetEnumerator();
                        AsymmetricAlgorithm key = GetNextCertificatePublicKey();
                        if (key != null)
                            return key;
                    }
                }
            }

            return null;
        }

        private X509Certificate2Collection BuildBagOfCerts () {
            X509Certificate2Collection collection = new X509Certificate2Collection();
            if (this.KeyInfo != null) {
                foreach (KeyInfoClause clause in this.KeyInfo) {
                    KeyInfoX509Data x509Data = clause as KeyInfoX509Data;
                    if (x509Data != null) 
                        collection.AddRange(Utils.BuildBagOfCerts(x509Data, CertUsageType.Verification));
                }
            }

            return collection;
        }

        private AsymmetricAlgorithm GetNextCertificatePublicKey () {
            while (m_x509Enum.MoveNext()) {
                X509Certificate2 certificate = (X509Certificate2) m_x509Enum.Current;
                if (certificate != null)
                    return certificate.PublicKey.Key;
            }

            return null;
        }

        public virtual XmlElement GetIdElement (XmlDocument document, string idValue) {
            if (document == null)
                return null;

            // Get the element with idValue
            XmlElement elem = document.GetElementById(idValue);
            if (elem != null)
                return elem;
            elem = document.SelectSingleNode("//*[@Id=\"" + idValue + "\"]") as XmlElement;
            if (elem != null)
                return elem;
            elem = document.SelectSingleNode("//*[@id=\"" + idValue + "\"]") as XmlElement;
            if (elem != null)
                return elem;
            elem = document.SelectSingleNode("//*[@ID=\"" + idValue + "\"]") as XmlElement;

            return elem;
        }

        //
        // private methods
        //

        private bool bCacheValid = false;
        private byte[] _digestedSignedInfo = null;

        private static bool DefaultSignatureFormatValidator(SignedXml signedXml) {
            // Reject the signature if it uses a truncated HMAC
            if (signedXml.DoesSignatureUseTruncatedHmac()) {
                return false;
            }

            // Reject the signature if it uses a canonicalization algorithm other than
            // one of the ones explicitly allowed
            if (!signedXml.DoesSignatureUseSafeCanonicalizationMethod()) {
                return false;
            }

            // Otherwise accept it
            return true;
        }

        // Validation function to see if the current signature is signed with a truncated HMAC - one which
        // has a signature length of fewer bits than the whole HMAC output.
        private bool DoesSignatureUseTruncatedHmac() {
            // If we're not using the SignatureLength property, then we're not truncating the signature length
            if (SignedInfo.SignatureLength == null) {
                return false;
            }

            // See if we're signed witn an HMAC algorithm
            HMAC hmac = CryptoConfig.CreateFromName(SignatureMethod) as HMAC;
            if (hmac == null) {
                // We aren't signed with an HMAC algorithm, so we cannot have a truncated HMAC
                return false;
            }

            // Figure out how many bits the signature is using
            int actualSignatureSize = 0;
            if (!Int32.TryParse(SignedInfo.SignatureLength, out actualSignatureSize)) {
                // If the value wasn't a valid integer, then we'll conservatively reject it all together
                return true;
            }

            // Make sure the full HMAC signature size is the same size that was specified in the XML
            // signature.  If the actual signature size is not exactly the same as the full HMAC size, then
            // reject the signature.
            return actualSignatureSize != hmac.HashSize;
        }

        // Validation function to see if the signature uses a canonicalization algorithm from our list
        // of approved algorithm URIs.
        private bool DoesSignatureUseSafeCanonicalizationMethod() {
            foreach (string safeAlgorithm in SafeCanonicalizationMethods) {
                if (String.Equals(safeAlgorithm, SignedInfo.CanonicalizationMethod, StringComparison.OrdinalIgnoreCase)) {
                    return true;
                }
            }

            SignedXmlDebugLog.LogUnsafeCanonicalizationMethod(this, SignedInfo.CanonicalizationMethod, SafeCanonicalizationMethods);
            return false;
        }

        // Get a list of the built in canonicalization algorithms, as well as any that the machine admin has
        // added to the valid set.
        private static IList<string> KnownCanonicalizationMethods
        {
            get {
                if (s_knownCanonicalizationMethods == null) {
                    // Start with the list that the machine admin added, if any
                    List<string> safeAlgorithms = ReadAdditionalSafeCanonicalizationMethods();

                    // And add the built in algorithms as well
                    safeAlgorithms.Add(XmlDsigC14NTransformUrl);
                    safeAlgorithms.Add(XmlDsigC14NWithCommentsTransformUrl);
                    safeAlgorithms.Add(XmlDsigExcC14NTransformUrl);
                    safeAlgorithms.Add(XmlDsigExcC14NWithCommentsTransformUrl);

                    s_knownCanonicalizationMethods = safeAlgorithms;
                }

                return s_knownCanonicalizationMethods;
            }
        }

        // Allow machine admins to add additional canonicalization algorithms that should be considered valid when
        // validating XML signatuers by supplying a list in the 
        // HKLM\Software\Microsoft\.NETFramework\Security\SafeCanonicalizationMethods
        // key.  Each REG_SZ entry in this key will be considered a canonicalziation algorithm URI that should be
        // allowed by SignedXml instances on this machine.
        [RegistryPermission(SecurityAction.Assert, Unrestricted = true)]
        [SecuritySafeCritical]
        private static List<string> ReadAdditionalSafeCanonicalizationMethods() {
            List<string> additionalAlgorithms = new List<string>();

            try {
                using (RegistryKey canonicalizationAlgorithmsKey = Registry.LocalMachine.OpenSubKey(@"SOFTWARE\Microsoft\.NETFramework\Security\SafeCanonicalizationMethods", false)) {
                    if (canonicalizationAlgorithmsKey != null) {
                        foreach (string value in canonicalizationAlgorithmsKey.GetValueNames()) {
                            if (canonicalizationAlgorithmsKey.GetValueKind(value) == RegistryValueKind.String) {
                                string additionalAlgorithm = canonicalizationAlgorithmsKey.GetValue(value) as string;
                                if (!String.IsNullOrWhiteSpace(additionalAlgorithm)) {
                                    additionalAlgorithms.Add(additionalAlgorithm);
                                }
                            }
                        }
                    }
                }
            }
            catch (SecurityException) { /* we could not open the key - that's fine, we can proceed with no additional algorithms */ }

            return additionalAlgorithms;
        }

        private byte[] GetC14NDigest (HashAlgorithm hash) {
            if (!bCacheValid || !this.SignedInfo.CacheValid) {
                string baseUri = (m_containingDocument == null ? null : m_containingDocument.BaseURI);
                XmlResolver resolver = (m_bResolverSet ? m_xmlResolver : new XmlSecureResolver(new XmlUrlResolver(), baseUri));
                XmlDocument doc = Utils.PreProcessElementInput(SignedInfo.GetXml(), resolver, baseUri);

                // Add non default namespaces in scope
                CanonicalXmlNodeList namespaces = (m_context == null ? null : Utils.GetPropagatedAttributes(m_context));
                SignedXmlDebugLog.LogNamespacePropagation(this, namespaces);
                Utils.AddNamespaces(doc.DocumentElement, namespaces);

                Transform c14nMethodTransform = SignedInfo.CanonicalizationMethodObject;
                c14nMethodTransform.Resolver = resolver;
                c14nMethodTransform.BaseURI = baseUri;

                SignedXmlDebugLog.LogBeginCanonicalization(this, c14nMethodTransform);
                c14nMethodTransform.LoadInput(doc);
                SignedXmlDebugLog.LogCanonicalizedOutput(this, c14nMethodTransform);
                _digestedSignedInfo = c14nMethodTransform.GetDigestedOutput(hash);

                bCacheValid = true;
            }
            return _digestedSignedInfo;
        }

        private int GetReferenceLevel (int index, ArrayList references) {
            if (m_refProcessed[index]) return m_refLevelCache[index];
            m_refProcessed[index] = true;
            Reference reference = (Reference) references[index];
            if (reference.Uri == null || reference.Uri.Length == 0 || (reference.Uri.Length > 0 && reference.Uri[0] != '#')) {
                m_refLevelCache[index] = 0;
                return 0;
            }
            if (reference.Uri.Length > 0 && reference.Uri[0] == '#') {
                String idref = Utils.ExtractIdFromLocalUri(reference.Uri);
                if (idref == "xpointer(/)") {
                    m_refLevelCache[index] = 0;
                    return 0;
                }
                // If this is pointing to another reference
                for (int j=0; j < references.Count; ++j) {
                    if (((Reference)references[j]).Id == idref) {
                        m_refLevelCache[index] = GetReferenceLevel(j, references) + 1;
                        return (m_refLevelCache[index]);
                    }
                }
                // Then the reference points to an object tag
                m_refLevelCache[index] = 0;
                return 0;
            }
            // Malformed reference
            throw new CryptographicException(SecurityResources.GetResourceString("Cryptography_Xml_InvalidReference"));
        }

        private class ReferenceLevelSortOrder : IComparer {
            private ArrayList m_references = null;
            public ReferenceLevelSortOrder() {}

            public ArrayList References {
                get { return m_references; }
                set { m_references = value; }
            }

            public int Compare(Object a, Object b) {
                Reference referenceA = a as Reference;
                Reference referenceB = b as Reference;

                // Get the indexes
                int iIndexA = 0;
                int iIndexB = 0;
                int i = 0;
                foreach (Reference reference in References) {
                    if (reference == referenceA) iIndexA = i;
                    if (reference == referenceB) iIndexB = i;
                    i++;
                }

                int iLevelA = referenceA.SignedXml.GetReferenceLevel(iIndexA, References);
                int iLevelB = referenceB.SignedXml.GetReferenceLevel(iIndexB, References);
                return iLevelA.CompareTo(iLevelB);
            }
        }

        private void BuildDigestedReferences() {
            // Default the DigestMethod and Canonicalization
            ArrayList references = SignedInfo.References;
            // Reset the cache
            m_refProcessed = new bool[references.Count];
            m_refLevelCache = new int[references.Count];

            ReferenceLevelSortOrder sortOrder = new ReferenceLevelSortOrder();
            sortOrder.References = references;
            // Don't alter the order of the references array list
            ArrayList sortedReferences = new ArrayList();
            foreach (Reference reference in references) {
                sortedReferences.Add(reference);
            }
            sortedReferences.Sort(sortOrder);

            CanonicalXmlNodeList nodeList = new CanonicalXmlNodeList();
            foreach (DataObject obj in m_signature.ObjectList) {
                nodeList.Add(obj.GetXml());
            }
            foreach (Reference reference in sortedReferences) {
                // If no DigestMethod has yet been set, default it to sha1
                if (reference.DigestMethod == null)
                    reference.DigestMethod = XmlDsigSHA1Url;

                SignedXmlDebugLog.LogSigningReference(this, reference);

                reference.UpdateHashValue(m_containingDocument, nodeList);
                // If this reference has an Id attribute, add it
                if (reference.Id != null)
                    nodeList.Add(reference.GetXml());
            }
        }

        private bool CheckDigestedReferences () {
            ArrayList references = m_signature.SignedInfo.References;
            for (int i = 0; i < references.Count; ++i) {
                Reference digestedReference = (Reference) references[i];

                SignedXmlDebugLog.LogVerifyReference(this, digestedReference);
                byte[] calculatedHash = digestedReference.CalculateHashValue(m_containingDocument, m_signature.ReferencedItems);

                // Compare both hashes
                SignedXmlDebugLog.LogVerifyReferenceHash(this, digestedReference, calculatedHash, digestedReference.DigestValue);
                if (calculatedHash.Length != digestedReference.DigestValue.Length) 
                    return false;

                byte[] rgb1 = calculatedHash;
                byte[] rgb2 = digestedReference.DigestValue;
                for (int j = 0; j < rgb1.Length; ++j) {
                    if (rgb1[j] != rgb2[j]) return false;
                }
            }

            return true;
        }

        // If we have a signature format validation callback, check to see if this signature's format (not
        // the signautre itself) is valid according to the validator.  A return value of true indicates that
        // the signature format is acceptable, false means that the format is not valid.
        private bool CheckSignatureFormat() {
            if (m_signatureFormatValidator == null) {
                // No format validator means that we default to accepting the signature.  (This is
                // effectively compatibility mode with v3.5).
                return true;
            }
            
            SignedXmlDebugLog.LogBeginCheckSignatureFormat(this, m_signatureFormatValidator);

            bool formatValid = m_signatureFormatValidator(this);
            SignedXmlDebugLog.LogFormatValidationResult(this, formatValid);
            return formatValid;
        }

        private bool CheckSignedInfo (AsymmetricAlgorithm key) {
            if (key == null)
                throw new ArgumentNullException("key");

            SignedXmlDebugLog.LogBeginCheckSignedInfo(this, m_signature.SignedInfo);

            SignatureDescription signatureDescription = CryptoConfig.CreateFromName(SignatureMethod) as SignatureDescription;
            if (signatureDescription == null)
                throw new CryptographicException(SecurityResources.GetResourceString("Cryptography_Xml_SignatureDescriptionNotCreated"));

            // Let's see if the key corresponds with the SignatureMethod 
            Type ta = Type.GetType(signatureDescription.KeyAlgorithm);
            Type tb = key.GetType();
            if ((ta != tb) && !ta.IsSubclassOf(tb) && !tb.IsSubclassOf(ta)) 
                // Signature method key mismatch
                return false;

            HashAlgorithm hashAlgorithm = signatureDescription.CreateDigest();
            if (hashAlgorithm == null)
                throw new CryptographicException(SecurityResources.GetResourceString("Cryptography_Xml_CreateHashAlgorithmFailed"));
            byte[] hashval = GetC14NDigest(hashAlgorithm);

            AsymmetricSignatureDeformatter asymmetricSignatureDeformatter = signatureDescription.CreateDeformatter(key);
            SignedXmlDebugLog.LogVerifySignedInfo(this,
                                                  key,
                                                  signatureDescription,
                                                  hashAlgorithm,
                                                  asymmetricSignatureDeformatter,
                                                  hashval,
                                                  m_signature.SignatureValue);
            return asymmetricSignatureDeformatter.VerifySignature(hashval, m_signature.SignatureValue);
        }

        private bool CheckSignedInfo (KeyedHashAlgorithm macAlg) {
            if (macAlg == null)
                throw new ArgumentNullException("macAlg");

            SignedXmlDebugLog.LogBeginCheckSignedInfo(this, m_signature.SignedInfo);

            int signatureLength;
            if (m_signature.SignedInfo.SignatureLength == null)
                signatureLength = macAlg.HashSize;
            else 
                signatureLength = Convert.ToInt32(m_signature.SignedInfo.SignatureLength, null);

            // signatureLength should be less than hash size
            if (signatureLength < 0 || signatureLength > macAlg.HashSize)
                throw new CryptographicException(SecurityResources.GetResourceString("Cryptography_Xml_InvalidSignatureLength"));
            if (signatureLength % 8 != 0)
                throw new CryptographicException(SecurityResources.GetResourceString("Cryptography_Xml_InvalidSignatureLength2"));
            if (m_signature.SignatureValue == null)
                throw new CryptographicException(SecurityResources.GetResourceString("Cryptography_Xml_SignatureValueRequired"));
            if (m_signature.SignatureValue.Length != signatureLength / 8)
                throw new CryptographicException(SecurityResources.GetResourceString("Cryptography_Xml_InvalidSignatureLength"));

            // Calculate the hash
            byte[] hashValue = GetC14NDigest(macAlg);
            SignedXmlDebugLog.LogVerifySignedInfo(this, macAlg, hashValue, m_signature.SignatureValue);
            for (int i=0; i<m_signature.SignatureValue.Length; i++) {
                if (m_signature.SignatureValue[i] != hashValue[i]) return false;
            }
            return true; 
        }
    }
}
