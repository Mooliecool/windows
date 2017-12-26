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
**
** File:    Version
**
**
** Purpose: 
**
**
===========================================================*/
namespace System {

    using CultureInfo = System.Globalization.CultureInfo;

    // A Version object contains four hierarchical numeric components: major, minor,
    // build and revision.  Build and revision may be unspecified, which is represented 
    // internally as a -1.  By definition, an unspecified component matches anything 
    // (both unspecified and specified), and an unspecified component is "less than" any
    // specified component.

    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class Version : ICloneable, IComparable
        , IComparable<Version>, IEquatable<Version>
    {
        // AssemblyName depends on the order staying the same
        private int _Major;
        private int _Minor;
        private int _Build = -1;
        private int _Revision = -1;
    
        public Version(int major, int minor, int build, int revision) {
            if (major < 0) 
              throw new ArgumentOutOfRangeException("major",Environment.GetResourceString("ArgumentOutOfRange_Version"));

            if (minor < 0) 
              throw new ArgumentOutOfRangeException("minor",Environment.GetResourceString("ArgumentOutOfRange_Version"));

            if (build < 0)
              throw new ArgumentOutOfRangeException("build",Environment.GetResourceString("ArgumentOutOfRange_Version"));
            
            if (revision < 0) 
              throw new ArgumentOutOfRangeException("revision",Environment.GetResourceString("ArgumentOutOfRange_Version"));
            
            _Major = major;
            _Minor = minor;
            _Build = build;
            _Revision = revision;
        }

        public Version(int major, int minor, int build) {
            if (major < 0) 
                throw new ArgumentOutOfRangeException("major",Environment.GetResourceString("ArgumentOutOfRange_Version"));

            if (minor < 0) 
              throw new ArgumentOutOfRangeException("minor",Environment.GetResourceString("ArgumentOutOfRange_Version"));

            if (build < 0) 
              throw new ArgumentOutOfRangeException("build",Environment.GetResourceString("ArgumentOutOfRange_Version"));

            
            _Major = major;
            _Minor = minor;
            _Build = build;
        }
    
        public Version(int major, int minor) {
            if (major < 0) 
                throw new ArgumentOutOfRangeException("major",Environment.GetResourceString("ArgumentOutOfRange_Version"));

            if (minor < 0) 
                throw new ArgumentOutOfRangeException("minor",Environment.GetResourceString("ArgumentOutOfRange_Version"));
            
            _Major = major;
            _Minor = minor;
        }

        public Version(String version) {
            if ((Object) version == null)
                throw new ArgumentNullException("version");

            String[] parsedComponents = version.Split(new char[] {'.'});
            int parsedComponentsLength = parsedComponents.Length;
            if ((parsedComponentsLength < 2) || (parsedComponentsLength > 4)) throw new ArgumentException(Environment.GetResourceString("Arg_VersionString"));
            _Major = Int32.Parse(parsedComponents[0], CultureInfo.InvariantCulture);
            if (_Major < 0) 
              throw new ArgumentOutOfRangeException("version",Environment.GetResourceString("ArgumentOutOfRange_Version"));

            _Minor = Int32.Parse(parsedComponents[1], CultureInfo.InvariantCulture);
            if (_Minor < 0) 
              throw new ArgumentOutOfRangeException("version",Environment.GetResourceString("ArgumentOutOfRange_Version"));

            parsedComponentsLength -= 2;
            if (parsedComponentsLength > 0) {
                _Build = Int32.Parse(parsedComponents[2], CultureInfo.InvariantCulture);
                if (_Build < 0) 
                  throw new ArgumentOutOfRangeException("build",Environment.GetResourceString("ArgumentOutOfRange_Version"));

                parsedComponentsLength--;
                if (parsedComponentsLength > 0) {
                    _Revision = Int32.Parse(parsedComponents[3], CultureInfo.InvariantCulture);
                    if (_Revision < 0)
                      throw new ArgumentOutOfRangeException("revision",Environment.GetResourceString("ArgumentOutOfRange_Version"));
                }
            }
        }

        public Version() 
        {
            _Major = 0;
            _Minor = 0;
        }

        // Properties for setting and getting version numbers
        public int Major {
            get { return _Major; }
        }
    
        public int Minor {
            get { return _Minor; }
        }
    
        public int Build {
            get { return _Build; }
        }
    
        public int Revision {
            get { return _Revision; }
        }

        public short MajorRevision {
            get { return (short)(_Revision >> 16); }
        }

        public short MinorRevision {
            get { return (short)(_Revision & 0xFFFF); }
        }
     
        public Object Clone() {
            Version v = new Version();
            v._Major = _Major;
            v._Minor = _Minor;
            v._Build = _Build;
            v._Revision = _Revision;
            return(v);
        }

        public int CompareTo(Object version)
        {
            if (version == null)
                return 1;

            Version v = version as Version;
            if (v == null)
                throw new ArgumentException(Environment.GetResourceString("Arg_MustBeVersion"));

            if (this._Major != v._Major)
                if (this._Major > v._Major)
                    return 1;
                else
                    return -1;

            if (this._Minor != v._Minor)
                if (this._Minor > v._Minor)
                    return 1;
                else
                    return -1;

            if (this._Build != v._Build)
                if (this._Build > v._Build)
                    return 1;
                else
                    return -1;

            if (this._Revision != v._Revision)
                if (this._Revision > v._Revision)
                    return 1;
                else
                    return -1;

            return 0;
        }

        public int CompareTo(Version value)
        {
            if (value == null)
                return 1;

            if (this._Major != value._Major)
                if (this._Major > value._Major)
                    return 1;
                else
                    return -1;

            if (this._Minor != value._Minor)
                if (this._Minor > value._Minor)
                    return 1;
                else
                    return -1;

            if (this._Build != value._Build)
                if (this._Build > value._Build)
                    return 1;
                else
                    return -1;

            if (this._Revision != value._Revision)
                if (this._Revision > value._Revision)
                    return 1;
                else
                    return -1;

            return 0;
        }

        public override bool Equals(Object obj) {
            Version v = obj as Version;
            if (v == null)
                return false;

            // check that major, minor, build & revision numbers match
            if ((this._Major != v._Major) || 
                (this._Minor != v._Minor) || 
                (this._Build != v._Build) ||
                (this._Revision != v._Revision))
                return false;

            return true;
        }

        public bool Equals(Version obj)
        {
            if (obj == null)
                return false;

            // check that major, minor, build & revision numbers match
            if ((this._Major != obj._Major) || 
                (this._Minor != obj._Minor) || 
                (this._Build != obj._Build) ||
                (this._Revision != obj._Revision))
                return false;

            return true;
        }

        public override int GetHashCode()
        {
            // Let's assume that most version numbers will be pretty small and just
            // OR some lower order bits together.

            int accumulator = 0;

            accumulator |= (this._Major & 0x0000000F) << 28;
            accumulator |= (this._Minor & 0x000000FF) << 20;
            accumulator |= (this._Build & 0x000000FF) << 12;
            accumulator |= (this._Revision & 0x00000FFF);

            return accumulator;
        }

        public override String ToString() {
            if (_Build == -1) return(ToString(2));
            if (_Revision == -1) return(ToString(3));
            return(ToString(4));
        }
        
        public String ToString(int fieldCount) {
            switch (fieldCount) {
            case 0: 
                return(String.Empty);
            case 1: 
                return(String.Concat(_Major));
            case 2: 
                return(String.Concat(_Major,".",_Minor));
            default:
                if (_Build == -1)
                    throw new ArgumentException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("ArgumentOutOfRange_Bounds_Lower_Upper"), "0", "2"), "fieldCount");
                if (fieldCount == 3)
                    return( _Major + "." + _Minor + "." + _Build );

                if (_Revision == -1)
                    throw new ArgumentException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("ArgumentOutOfRange_Bounds_Lower_Upper"), "0", "3"), "fieldCount");

                if (fieldCount == 4)
                    return( Major + "." + _Minor + "." + _Build + "." + _Revision );

                throw new ArgumentException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("ArgumentOutOfRange_Bounds_Lower_Upper"), "0", "4"), "fieldCount");
            }
        }

        public static bool operator ==(Version v1, Version v2) {
            if (Object.ReferenceEquals(v1, null)) {
                return Object.ReferenceEquals(v2, null);
            }

            return v1.Equals(v2);
        }

        public static bool operator !=(Version v1, Version v2) {
            return !(v1 == v2);
        }

        public static bool operator <(Version v1, Version v2) {
            if ((Object) v1 == null)
                throw new ArgumentNullException("v1");
            return (v1.CompareTo(v2) < 0);
        }
        
        public static bool operator <=(Version v1, Version v2) {
            if ((Object) v1 == null)
                throw new ArgumentNullException("v1");
            return (v1.CompareTo(v2) <= 0);
        }
        
        public static bool operator >(Version v1, Version v2) {
            return (v2 < v1);
        }
        
        public static bool operator >=(Version v1, Version v2) {
            return (v2 <= v1);
        }
    }
}
