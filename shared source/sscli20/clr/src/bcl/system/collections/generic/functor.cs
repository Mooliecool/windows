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
    public delegate void Action<T>(T obj); 

    public delegate int Comparison<T>(T x, T y);

    public delegate TOutput Converter<TInput, TOutput>(TInput input); 
    
    public delegate bool Predicate<T>(T obj); 
}

