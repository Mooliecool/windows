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

namespace Microsoft.JScript {
    
    using System;
    using System.Text;

    internal sealed class ConcatString : IConvertible{
      private StringBuilder buf;
      private bool isOwner;
      private int length;
    
      internal ConcatString(String str1, String str2){
        this.length = str1.Length+str2.Length;
        int capacity = this.length*2;
        if (capacity < 256) capacity = 256;
        this.buf = new StringBuilder(str1, capacity);
        this.buf.Append(str2);
        this.isOwner = true;
      }
      
      internal ConcatString(ConcatString str1, String str2){
        this.length = str1.length+str2.Length;
        if (str1.isOwner){
          this.buf = str1.buf;
          str1.isOwner = false;
        }else{
          int capacity = this.length*2;
          if (capacity < 256) capacity = 256;
          this.buf = new StringBuilder(str1.ToString(), capacity);
        }
        this.buf.Append(str2);
        this.isOwner = true;
      }
      
      TypeCode IConvertible.GetTypeCode(){
        return TypeCode.String;
      }
      
      bool IConvertible.ToBoolean(IFormatProvider provider){
        return this.ToIConvertible().ToBoolean(provider);
      }
      
      char IConvertible.ToChar(IFormatProvider provider){
        return this.ToIConvertible().ToChar(provider);
      }
      
      sbyte IConvertible.ToSByte(IFormatProvider provider){
        return this.ToIConvertible().ToSByte(provider);
      }
      
      byte IConvertible.ToByte(IFormatProvider provider){
        return this.ToIConvertible().ToByte(provider);
      }
      
      short IConvertible.ToInt16(IFormatProvider provider){
        return this.ToIConvertible().ToInt16(provider);
      }
      
      ushort IConvertible.ToUInt16(IFormatProvider provider){
        return this.ToIConvertible().ToUInt16(provider);
      }
      
      private IConvertible ToIConvertible(){
        return this.ToString();
      }
      
      int IConvertible.ToInt32(IFormatProvider provider){
        return this.ToIConvertible().ToInt32(provider);
      }
      
      uint IConvertible.ToUInt32(IFormatProvider provider){
        return this.ToIConvertible().ToUInt32(provider);
      }
      
      long IConvertible.ToInt64(IFormatProvider provider){
        return this.ToIConvertible().ToInt64(provider);
      }
      
      ulong IConvertible.ToUInt64(IFormatProvider provider){
        return this.ToIConvertible().ToUInt64(provider);
      }
      
      float IConvertible.ToSingle(IFormatProvider provider){
        return this.ToIConvertible().ToSingle(provider);
      }
      
      double IConvertible.ToDouble(IFormatProvider provider){
        return this.ToIConvertible().ToDouble(provider);
      }
      
      Decimal IConvertible.ToDecimal(IFormatProvider provider){
        return this.ToIConvertible().ToDecimal(provider);
      }
      
      DateTime IConvertible.ToDateTime(IFormatProvider provider){
        return this.ToIConvertible().ToDateTime(provider);
      }
      
      String IConvertible.ToString(IFormatProvider provider){
        return this.ToIConvertible().ToString(provider);
      }
      
      Object IConvertible.ToType(Type conversionType, IFormatProvider provider){
        return this.ToIConvertible().ToType(conversionType, provider);
      }
      
      public override String ToString(){
        return this.buf.ToString(0, this.length);
      }
    
    }
}
