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

namespace System {
    using System.Collections;
    using System.Collections.Generic;    
    using System.Globalization;
    
    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public abstract class StringComparer : IComparer, IEqualityComparer, IComparer<string>, IEqualityComparer<string>{
        private static StringComparer _invariantCulture = new CultureAwareComparer(CultureInfo.InvariantCulture, false);        
        private static StringComparer _invariantCultureIgnoreCase = new CultureAwareComparer(CultureInfo.InvariantCulture, true);      
        private static StringComparer _ordinal = new OrdinalComparer(false);
        private static StringComparer _ordinalIgnoreCase = new OrdinalComparer(true);        
        
        public static StringComparer InvariantCulture { 
            get {
                return _invariantCulture;
            }
        }
        
        public static StringComparer InvariantCultureIgnoreCase { 
            get {
                return _invariantCultureIgnoreCase;
            }
        }

        public static StringComparer CurrentCulture { 
            get {
                return new CultureAwareComparer(CultureInfo.CurrentCulture, false);                
            }
        }
        
        public static StringComparer CurrentCultureIgnoreCase { 
            get {
                return new CultureAwareComparer(CultureInfo.CurrentCulture, true);                
            }
        }

        public static StringComparer Ordinal { 
            get {
                return _ordinal;
            }
        }

        public static StringComparer OrdinalIgnoreCase { 
            get {
                return _ordinalIgnoreCase;
            }
        }

        public static StringComparer Create(CultureInfo culture, bool ignoreCase) {
                if( culture == null) {
                    throw new ArgumentNullException("culture");
                }
                
                return new CultureAwareComparer(culture, ignoreCase);            
        }  

        public int Compare(object x, object y) {
            if (x == y) return 0;
            if (x == null) return -1;
            if (y == null) return 1;
                        
            String sa = x as String;
            if (sa != null) {                
                String sb = y as String;                
                if( sb != null) {
                    return Compare(sa, sb);                    
                }
            }

            IComparable ia = x as IComparable;
            if (ia != null) {
                return ia.CompareTo(y);
            }

            throw new ArgumentException(Environment.GetResourceString("Argument_ImplementIComparable"));            
        }

       
        public new bool Equals(Object x, Object y) {
            if (x == y) return true;
            if (x == null || y == null) return false;
            
            String sa = x as String;
            if (sa != null) {
                String sb = y as String;                
                if( sb != null) {
                    return Equals(sa, sb);
                }
            }
            return x.Equals(y);                        
        }
        
        public int GetHashCode(object obj) {
            if( obj == null) {
                throw new ArgumentNullException("obj");
            }

            string s = obj as string;
            if( s != null) {
                return GetHashCode(s);
            }
            return obj.GetHashCode();            
        }
        
        public abstract int Compare(String x, String y);
        public abstract bool Equals(String x, String y);        
        public abstract int GetHashCode(string obj);        
    }
    
    [Serializable()]
    internal sealed class CultureAwareComparer : StringComparer {
         private CompareInfo _compareInfo;    
         private bool            _ignoreCase;

        internal CultureAwareComparer(CultureInfo culture, bool ignoreCase) {
               _compareInfo = culture.CompareInfo;
               _ignoreCase = ignoreCase;
        }

        public override int Compare(string x, string y) {
            if (Object.ReferenceEquals(x, y)) return 0;
            if (x == null) return -1;
            if (y == null) return 1;
            return _compareInfo.Compare(x, y, _ignoreCase? CompareOptions.IgnoreCase :  CompareOptions.None);
        }
                
        public override bool Equals(string x, string y) {
            if (Object.ReferenceEquals(x ,y)) return true;
            if (x == null || y == null) return false;

            return (_compareInfo.Compare(x, y, _ignoreCase? CompareOptions.IgnoreCase :  CompareOptions.None) == 0);
        }               
                
        public override int GetHashCode(string obj) {
            if( obj == null) {
                throw new ArgumentNullException("obj");
            }

            if( _ignoreCase) {
                return _compareInfo.GetHashCodeOfString(obj, CompareOptions.IgnoreCase);
            }
            
            return _compareInfo.GetHashCodeOfString(obj, CompareOptions.None);
        }       

        // Equals method for the comparer itself. 
        public override bool Equals(Object obj){
            CultureAwareComparer comparer = obj as CultureAwareComparer;
            if( comparer == null) {
                return false;
            }
            return (this._ignoreCase == comparer._ignoreCase) && ( this._compareInfo.Equals(comparer._compareInfo));
        }

        public override int GetHashCode() {
            int hashCode = _compareInfo.GetHashCode() ;
            return _ignoreCase ? (~hashCode) : hashCode; 
        }
    }


    // Provide x more optimal implementation of ordinal comparison.
    [Serializable()]
    internal sealed class OrdinalComparer : StringComparer  {
        private bool            _ignoreCase;
        
        internal OrdinalComparer(bool ignoreCase) {
               _ignoreCase = ignoreCase;
        }

        public override int Compare(string x, string y) {
            if (Object.ReferenceEquals(x, y)) return 0;
            if (x == null) return -1;
            if (y == null) return 1;
            
            if( _ignoreCase) {
                return TextInfo.CompareOrdinalIgnoreCase(x, y);
            }
                    
            return String.CompareOrdinal(x, y);                                
        }
                
        public override bool Equals(string x, string y) {
            if (Object.ReferenceEquals(x ,y)) return true;
            if (x == null || y == null) return false;

            if( _ignoreCase) {
                if( x.Length != y.Length) {
                    return false;
                }
                return (TextInfo.CompareOrdinalIgnoreCase(x, y) == 0);                                            
            }
            return x.Equals(y);
        }               
                
        public override int GetHashCode(string obj) {
            if( obj == null) {
                throw new ArgumentNullException("obj");
            }

            if( _ignoreCase) {
                return TextInfo.GetHashCodeOrdinalIgnoreCase(obj);                
            }
                        
            return obj.GetHashCode();
        }       

        // Equals method for the comparer itself. 
        public override bool Equals(Object obj){
            OrdinalComparer comparer = obj as OrdinalComparer;
            if( comparer == null) {
                return false;
            }
            return (this._ignoreCase == comparer._ignoreCase);
        }

        public override int GetHashCode() {
            string name = "OrdinalComparer";
            int hashCode = name.GetHashCode();
            return _ignoreCase ? (~hashCode) : hashCode;
        }                        
    }
}
