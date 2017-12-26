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
 ** Class: ISoapMessage
 **
 **
 ** Purpose: Interface For Soap Method Call
 **
 **
 ===========================================================*/

namespace System.Runtime.Serialization.Formatters {

    using System.Runtime.Remoting;
    using System.Runtime.Serialization;
    using System.Runtime.Remoting.Messaging;    
    using System;
    // Used to specify a call record to either the binary or xml serializer
    // The call record can be transmitted as the SOAP Top record which contains
    // a method name instead of an object name as the Top record's element name
[System.Runtime.InteropServices.ComVisible(true)]
    public interface ISoapMessage
    {
        // Name of parameters, if null the default param names will be used

        String[] ParamNames {get; set;}
    
        // Parameter Values
        Object[] ParamValues {get; set;}

        // Parameter Types
        Type[] ParamTypes {get; set;}        
    
        // MethodName
        String MethodName {get; set;}

        // MethodName XmlNameSpace
        String XmlNameSpace {get; set;}

        // Headers
        Header[] Headers {get; set;}
    }
}
