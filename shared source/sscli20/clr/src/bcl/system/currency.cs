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
    
    using System;
    using System.Globalization;
    using System.Runtime.CompilerServices;
    [Serializable]
    internal struct Currency
    {
        internal long m_value;
        
        // Constructs a Currency from a Decimal value.
        //
        public Currency(Decimal value) {
            m_value = Decimal.ToCurrency(value).m_value;
        }
    
        // Constructs a Currency from a long value without scaling. The
        // ignored parameter exists only to distinguish this constructor
        // from the constructor that takes a long.  Used only in the System 
        // package, especially in Variant.
        internal Currency(long value, int ignored) {
            m_value = value;
        }
    
        // Creates a Currency from an OLE Automation Currency.  This method
        // applies no scaling to the Currency value, essentially doing a bitwise
        // copy.
        // 
        public static Currency FromOACurrency(long cy){
            return new Currency(cy, 0);
        }

        //Creates an OLE Automation Currency from a Currency instance.  This 
        // method applies no scaling to the Currency value, essentially doing 
        // a bitwise copy.
        // 
        public long ToOACurrency() {
            return m_value;
        }
    
        // Converts a Currency to a Decimal.
        //
        public static Decimal ToDecimal(Currency c)
        {
            Decimal result = new Decimal ();
            FCallToDecimal (ref result, c);
            return result;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void FCallToDecimal(ref Decimal result,Currency c);
    }
}
