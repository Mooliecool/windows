//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.IdentityModel.Claims;
namespace ManagedCardWriter
{
    public class DefaultValues
    {
        static Dictionary<string, ClaimInfo> m_claims = new Dictionary<string, ClaimInfo>();

        public const string CardName = "My new card";
        public const string CardId = "http://www.fabrikam.com/card/1";
        public const string Issuer = "http://localhost:4444/sts";
        public const string Language = "en-us";
        public const string IssuerName = "Microsoft";
        public const string CardVersion = "1";
        public const string MexUri = "https://localhost:4445/sts/mex";

        static DefaultValues()
        {
            m_claims.Add(ClaimTypes.GivenName, new ClaimInfo(ClaimTypes.GivenName, "Given Name", "Given Name"));
            m_claims.Add(ClaimTypes.Surname, new ClaimInfo(ClaimTypes.Surname, "Last Name", "Last Name"));
            m_claims.Add(ClaimTypes.Email, new ClaimInfo(ClaimTypes.Email, "Email Address", "Email Address"));
            m_claims.Add(ClaimTypes.StreetAddress, new ClaimInfo(ClaimTypes.StreetAddress, "Street Address", "Street Address"));
            m_claims.Add(ClaimTypes.Locality, new ClaimInfo(ClaimTypes.Locality, "Locality", "Locality"));
            m_claims.Add(ClaimTypes.StateOrProvince, new ClaimInfo(ClaimTypes.StateOrProvince, "State or Province", "State or Province"));
            m_claims.Add(ClaimTypes.PostalCode, new ClaimInfo(ClaimTypes.PostalCode, "Postal Code", "Postal Code"));
            m_claims.Add(ClaimTypes.Country, new ClaimInfo(ClaimTypes.Country, "Country", "Country"));
            m_claims.Add(ClaimTypes.HomePhone, new ClaimInfo(ClaimTypes.HomePhone, "Home Phone", "Home Phone"));
            m_claims.Add(ClaimTypes.OtherPhone, new ClaimInfo(ClaimTypes.OtherPhone, "Other Phone", "Other Phone"));
            m_claims.Add(ClaimTypes.MobilePhone, new ClaimInfo(ClaimTypes.MobilePhone, "Mobile Phone", "Mobile Phone"));
            m_claims.Add(ClaimTypes.Gender, new ClaimInfo(ClaimTypes.Gender, "Gender", "Gender"));
            m_claims.Add(ClaimTypes.DateOfBirth, new ClaimInfo(ClaimTypes.DateOfBirth, "Date of Birth", "Date of Birth"));
            m_claims.Add(ClaimTypes.PPID, new ClaimInfo(ClaimTypes.PPID, "Site Specific ID", "Site Specific ID"));
            m_claims.Add(ClaimTypes.Webpage, new ClaimInfo(ClaimTypes.Webpage, "Webpage", "Webpage"));

        }

        public static Dictionary<string, ClaimInfo> Claims
        {
            get { return m_claims; }
        }

        internal static string[] ClaimsList = new string[]{
                ClaimTypes.GivenName, 
                ClaimTypes.Surname , 
                ClaimTypes.Email, 
                ClaimTypes.StreetAddress, 
                ClaimTypes.Locality, 
                ClaimTypes.StateOrProvince,
                ClaimTypes.PostalCode, 
                ClaimTypes.Country, 
                ClaimTypes.HomePhone, 
                ClaimTypes.OtherPhone,
                ClaimTypes.MobilePhone,
                ClaimTypes.Gender, 
                ClaimTypes.DateOfBirth,
                ClaimTypes.PPID, 
                ClaimTypes.Webpage};
        
        internal static string[] TokenTypeList =new string[] {
            "urn:oasis:names:tc:SAML:1.0:assertion",
            "http://docs.oasis-open.org/wss/oasis-wss-saml-token-profile-1.1#SAMLV1.1" };
        
        

        public enum CardType : int
        {
            UserNamePassword,
            KerberosAuth,
            SelfIssuedAuth,
            SmartCard,
            None

        }

    }

    public sealed class ClaimInfo
    {
        string m_displayTag;
        string m_description;
        string m_id;



        public ClaimInfo(string id, string tag, string des)
        {
            m_id = id;
            m_displayTag = tag;
            m_description = des;
        }
        public string DisplayTag
        {
            get { return m_displayTag; }
            set { m_displayTag = value; }
        }

        public string Description
        {
            get { return m_description; }
            set { m_description = value; }
        }

        public string Id
        {
            get { return m_id; }
            set { m_id = value; }
        }
    }

}