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

// Note: If you add a new ctor overloads you need to update ParameterInfo.RawDefaultValue

namespace System.Runtime.CompilerServices
{
    [Serializable, AttributeUsage(AttributeTargets.Field | AttributeTargets.Parameter, Inherited=false)]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class DecimalConstantAttribute : Attribute
    {
        [CLSCompliant(false)]
        public DecimalConstantAttribute(
            byte scale,
            byte sign,
            uint hi,
            uint mid,
            uint low
        )
        {
            dec = new System.Decimal((int) low, (int)mid, (int)hi, (sign != 0), scale);
        }

        public DecimalConstantAttribute(
            byte scale,
            byte sign,
            int hi,
            int mid,
            int low
        )
        {
            dec = new System.Decimal(low, mid, hi, (sign != 0), scale);
        }


        public System.Decimal Value
        {
            get {
                return dec;
            }
        }

        private System.Decimal dec;
    }
}

