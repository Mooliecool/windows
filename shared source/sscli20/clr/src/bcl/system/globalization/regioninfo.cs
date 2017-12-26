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

////////////////////////////////////////////////////////////////////////////
//
//  Class:    RegionInfo
//
//  Purpose:  This class represents settings specified by de jure or
//            de facto standards for a particular country or region.  In
//            contrast to CultureInfo, the RegionInfo does not represent
//            preferences of the user and does not depend on the user's
//            language or culture.
//
//  Date:     March 31, 1999
//
////////////////////////////////////////////////////////////////////////////

namespace System.Globalization {

    using System;
    using System.Runtime.Serialization;

    [Serializable] 
[System.Runtime.InteropServices.ComVisible(true)]
    public class RegionInfo
    {
        //--------------------------------------------------------------------//
        //                        Internal Information                        //
        //--------------------------------------------------------------------//

        //
        //  Variables.
        //
        internal String m_name;

        //
        // m_cultureId only used for serialization to detect if we have created the Region using LCID. 
        // When creating region using culture Id we fake the name (for compatibility) so during the
        // deserialization we need to detect if we have faked name or real name.
        //
        [OptionalField(VersionAdded = 2)]
        int m_cultureId;
        
        //
        // The CultureTable instance that we are going to read data from.
        // For supported culture, this will be the CultureTable instance that read data from mscorlib assembly.
        // For customized culture, this will be the CultureTable instance that read data from user customized culture binary file.
        //
        [NonSerialized]internal CultureTableRecord m_cultureTableRecord;

        internal static RegionInfo m_currentRegionInfo;


        ////////////////////////////////////////////////////////////////////////
        //
        //  RegionInfo Constructors
        //
        ////////////////////////////////////////////////////////////////////////


        public RegionInfo(String name) {
            if (name==null)
                throw new ArgumentNullException("name");
            if (name.Length == 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidRegionName", name), "name");
            this.m_name = name.ToUpper(CultureInfo.InvariantCulture);
            
            m_cultureId = 0;

            //
            // First try it as an entire culture. We must have user override as true here so
            // that we can pick up custom cultures *before* built-in ones (if they want to
            // prefer built-in cultures they will pass "us" instead of "en-US").
            //
            this.m_cultureTableRecord = CultureTableRecord.GetCultureTableRecordForRegion(name, true);

            if (this.m_cultureTableRecord.IsNeutralCulture) {
                // Not supposed to be neutral
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidNeutralRegionName", name), "name");
            } 
        }


        public RegionInfo(int culture) {
            // Get the culture data item.

            if (culture == CultureInfo.LOCALE_INVARIANT) { //The InvariantCulture has no matching region
                throw new ArgumentException(Environment.GetResourceString("Argument_NoRegionInvariantCulture"));
            }

            if (CultureTableRecord.IsCustomCultureId(culture)) { // Customized culture
                throw new ArgumentException(Environment.GetResourceString("Argument_CustomCultureCannotBePassedByNumber", "culture"));
            }
            if (CultureInfo.GetSubLangID(culture) == 0)
            {
                // Supposed to have sublang (region) info
                throw new ArgumentException(Environment.GetResourceString("Argument_CultureIsNeutral", culture), "culture");
            }
            
            //
            // From this region data item, get the region data item.
            // Several culture ID may map to the same region.
            // For example, 0x1009 (English (Canada)) and 0x0c0c (French (Canada)) all map to
            // the same region "CA" (Canada).
            //
            this.m_cultureTableRecord = CultureTableRecord.GetCultureTableRecord(culture, true);

            if (this.m_cultureTableRecord.IsNeutralCulture) {
                // Not supposed to be neutral
                throw new ArgumentException(Environment.GetResourceString("Argument_CultureIsNeutral", culture), "culture");
            } 

            // Make sure that we initialize m_name since it will be used in deserialization.
            this.m_name = this.m_cultureTableRecord.SREGIONNAME;
            
            m_cultureId = culture;
        }

        internal RegionInfo(CultureTableRecord table)
        {
            this.m_cultureTableRecord = table;
            // Make sure that we initialize m_name since it will be used in deserialization.
            this.m_name = this.m_cultureTableRecord.SREGIONNAME;            
        }

#region Serialization 
        internal int m_dataItem;

        [OnDeserialized]
        private void OnDeserialized(StreamingContext ctx)
        {
            if (m_name == null)
            {
                BCLDebug.Assert(m_dataItem >= 0, "[RegionInfo.OnDeserialized] null name and invalid dataItem");
                m_cultureTableRecord = CultureTableRecord.GetCultureTableRecord(CultureTableRecord.IdFromEverettRegionInfoDataItem(m_dataItem), true);
                m_name = m_cultureTableRecord.SREGIONNAME;
            }
            else
            {
                if (m_cultureId != 0)
                    m_cultureTableRecord = CultureTableRecord.GetCultureTableRecord(m_cultureId, true);
                else
                    m_cultureTableRecord = CultureTableRecord.GetCultureTableRecordForRegion(m_name, true);
            }
        }

        [OnSerializing] 
        private void OnSerializing(StreamingContext ctx) 
        { 
            m_dataItem = m_cultureTableRecord.EverettRegionDataItem();
        }   
#endregion Serialization

        ////////////////////////////////////////////////////////////////////////
        //
        //  GetCurrentRegion
        //
        //  This instance provides methods based on the current user settings.
        //  These settings are volatile and may change over the lifetime of the
        //  thread.
        //
        ////////////////////////////////////////////////////////////////////////


        public static unsafe RegionInfo CurrentRegion {
            get {
                RegionInfo temp = m_currentRegionInfo;
                if (temp == null)
                {
                    temp = new RegionInfo(CultureInfo.CurrentCulture.m_cultureTableRecord);

                    // Need full name for custom cultures
                    if (temp.m_cultureTableRecord.IsCustomCulture)
                    {
                        temp.m_name=temp.m_cultureTableRecord.SNAME;
                    }
                    m_currentRegionInfo = temp;
                }
                return temp;
            }
        }

        ////////////////////////////////////////////////////////////////////////
        //
        //  GetName
        //
        //  Returns the name of the region in the UI language.
        //
        ////////////////////////////////////////////////////////////////////////


        public virtual String Name {
            get {
                if (m_name == null) {
                    m_name = this.m_cultureTableRecord.SREGIONNAME;
                }
                return (m_name);
            }
        }

        ////////////////////////////////////////////////////////////////////////
        //
        //  GetEnglishName
        //
        //  Returns the name of the region in English.
        //
        ////////////////////////////////////////////////////////////////////////


        public virtual String EnglishName
        {
            get
            {
                return (this.m_cultureTableRecord.SENGCOUNTRY);
            }
        }


        public virtual String DisplayName 
        {
            get 
            {
                if (m_cultureTableRecord.IsCustomCulture)
                {
                    if (m_cultureTableRecord.IsReplacementCulture)
                    {
                        // <SyntheticSupport/>
                        if (m_cultureTableRecord.IsSynthetic)
                            return m_cultureTableRecord.RegionNativeDisplayName;
                        else
                            return (Environment.GetResourceString("Globalization.ri_"+this.m_cultureTableRecord.SREGIONNAME));
                    }
                    else
                    {
                        return this.m_cultureTableRecord.SNATIVECOUNTRY;
                    }
                }
                else
                {
                    // <SyntheticSupport/>
                    if (m_cultureTableRecord.IsSynthetic)
                        return m_cultureTableRecord.RegionNativeDisplayName;
                    else
                        return (Environment.GetResourceString("Globalization.ri_"+this.m_cultureTableRecord.SREGIONNAME));
                }
            }
        }


        [System.Runtime.InteropServices.ComVisible(false)]
        public virtual String NativeName
        {
            get
            {
                return (this.m_cultureTableRecord.SNATIVECOUNTRY);
            }
        }


        public virtual String TwoLetterISORegionName
        {
            get
            {
                return (this.m_cultureTableRecord.SISO3166CTRYNAME);
            }
        }


        public virtual String ThreeLetterISORegionName
        {
            get
            {
                return (this.m_cultureTableRecord.SISO3166CTRYNAME2);
            }
        }


        public virtual bool IsMetric {
            get {
                int value = this.m_cultureTableRecord.IMEASURE;
                return (value==0);
            }
        }


        [System.Runtime.InteropServices.ComVisible(false)]        
        public virtual int GeoId 
        {
            get 
            {
                return (this.m_cultureTableRecord.IGEOID);
            }
        }


        public virtual String ThreeLetterWindowsRegionName
        {
            get
            {
                return (this.m_cultureTableRecord.SABBREVCTRYNAME);
            }
        }


        [System.Runtime.InteropServices.ComVisible(false)]
        public virtual String CurrencyEnglishName
        {
            get
            {
                return (this.m_cultureTableRecord.SENGLISHCURRENCY);
            }
        }


        [System.Runtime.InteropServices.ComVisible(false)]
        public virtual String CurrencyNativeName
        {
            get
            {
                return (this.m_cultureTableRecord.SNATIVECURRENCY);
            }
        }

        /*================================ CurrencySymbol =============================
        **Property: CurrencySymbol
        **Exceptions: None
        ==============================================================================*/


        public virtual String CurrencySymbol {
            get {
                return (this.m_cultureTableRecord.SCURRENCY);
            }
        }

        /*================================ ISOCurrencySymbol ==========================
        **Property: Three characters of the international monetary symbol specified in ISO 4217.
        **Exceptions: None
        ==============================================================================*/


        public virtual String ISOCurrencySymbol {
            get {
                return (this.m_cultureTableRecord.SINTLSYMBOL);
            }
        }

        ////////////////////////////////////////////////////////////////////////
        //
        //  Equals
        //
        //  Implements Object.Equals().  Returns a boolean indicating whether
        //  or not object refers to the same CultureInfo as the current instance.
        //
        ////////////////////////////////////////////////////////////////////////


        public override bool Equals(Object value)
        {
            RegionInfo that = value as RegionInfo;
            if (that != null)
            {
                return this.Name.Equals(that.Name);
            }

            return (false);
        }

        ////////////////////////////////////////////////////////////////////////
        //
        //  GetHashCode
        //
        //  Implements Object.GetHashCode().  Returns the hash code for the
        //  CultureInfo.  The hash code is guaranteed to be the same for RegionInfo
        //  A and B where A.Equals(B) is true.
        //
        ////////////////////////////////////////////////////////////////////////


        public override int GetHashCode()
        {
            return (this.Name.GetHashCode());
        }


        ////////////////////////////////////////////////////////////////////////
        //
        //  ToString
        //
        //  Implements Object.ToString().  Returns the name of the Region,
        //  eg. "United States".
        //
        ////////////////////////////////////////////////////////////////////////


        public override String ToString()
        {
            return (Name);
        }
    }
}
