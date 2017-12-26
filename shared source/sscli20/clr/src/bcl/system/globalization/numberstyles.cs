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
** Enum:  NumberStyles.cs
**
**
** Purpose: Contains valid formats for Numbers recognized by
** the Number class' parsing code.
**
**
===========================================================*/
namespace System.Globalization {
    
	using System;
    [Flags,Serializable] 
[System.Runtime.InteropServices.ComVisible(true)]
	public enum NumberStyles {
        // Bit flag indicating that leading whitespace is allowed. Character values
        // 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, and 0x0020 are considered to be
        // whitespace.
    
    	None				  = 0x00000000, 

        AllowLeadingWhite     = 0x00000001, 

        AllowTrailingWhite    = 0x00000002, //Bitflag indicating trailing whitespace is allowed.

        AllowLeadingSign      = 0x00000004, //Can the number start with a sign char.  
                                            //Specified by NumberFormatInfo.PositiveSign and NumberFormatInfo.NegativeSign

        AllowTrailingSign     = 0x00000008, //Allow the number to end with a sign char

        AllowParentheses      = 0x00000010, //Allow the number to be enclosed in parens

        AllowDecimalPoint     = 0x00000020, //Allow a decimal point

        AllowThousands        = 0x00000040, //Allow thousands separators (more properly, allow group separators)

        AllowExponent         = 0x00000080, //Allow an exponent

        AllowCurrencySymbol   = 0x00000100, //Allow a currency symbol.

        AllowHexSpecifier	  = 0x00000200, //Allow specifiying hexadecimal.
        //Common uses.  These represent some of the most common combinations of these flags.
    

        Integer  = AllowLeadingWhite | AllowTrailingWhite | AllowLeadingSign,

        HexNumber = AllowLeadingWhite | AllowTrailingWhite | AllowHexSpecifier,

        Number   = AllowLeadingWhite | AllowTrailingWhite | AllowLeadingSign | AllowTrailingSign |
                   AllowDecimalPoint | AllowThousands,

        Float    = AllowLeadingWhite | AllowTrailingWhite | AllowLeadingSign | 
                   AllowDecimalPoint | AllowExponent,

        Currency = AllowLeadingWhite | AllowTrailingWhite | AllowLeadingSign | AllowTrailingSign |
                   AllowParentheses  | AllowDecimalPoint | AllowThousands | AllowCurrencySymbol,

        Any      = AllowLeadingWhite | AllowTrailingWhite | AllowLeadingSign | AllowTrailingSign |
                   AllowParentheses  | AllowDecimalPoint | AllowThousands | AllowCurrencySymbol | AllowExponent,
             
    }
}
