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

namespace System.Reflection 
{
    [Flags()]
    public enum GenericParameterAttributes
    {
        None = 0x0000,
        VarianceMask = 0x0003,
        Covariant = 0x0001,
        Contravariant = 0x0002,
        SpecialConstraintMask = 0x001C,
        ReferenceTypeConstraint = 0x0004,
        NotNullableValueTypeConstraint = 0x0008,
        DefaultConstructorConstraint = 0x0010,
    }
}

