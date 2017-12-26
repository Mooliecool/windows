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
  using System.Threading;
  using System.Reflection;
  using System.Reflection.Emit;
  using System.Security;
  using System.Security.Permissions;
  using Microsoft.JScript.Vsa;
    
  public static class Runtime {
    // *** VS58565 - JSEE: Remove all uses of IDebugObject::GetManagedObject from the code ***
    // JSEE does a funceval to this method to compare if to value types are equal.
    public static new bool Equals(Object v1, Object v2){
      Equality e = new Equality((int)JSToken.Equal);
      return e.EvaluateEquality(v1, v2);
    }


    //
    public static Int64 DoubleToInt64(double val) {
      if (Double.IsNaN(val)) { // Take care of NaN case first as it isn't ordered.
        return 0;
      }
      if (Int64.MinValue <= val && val <= Int64.MaxValue) {
        return (Int64)val;
      }

      // Remove cases where IEEERemainder will return NaN.
      if (Double.IsInfinity(val))
        return 0;
      
      // Definition of IEEERemandier returns:
      //  A number equal to x-(y*Q), where Q is the quotient of x/y rounded to the nearest
      //    integer (if x/y falls halfway between two integers, the even integer is returned).
      //
      // We use y = 2^64 = UInt64.MaxValue+1. The resulting remainder may range from -2^63 
      // to 2^63. i.e. x = (2^64+2^63) gives Q = 2 and result -2^63, x = -(2^64+2^63) gives
      // Q = -2 and result 2^63. To fit into Int64, we translate 2^63 by -2^64 to get -2^63
      // which is Int64.MinValue.
      double rem = Math.IEEERemainder(Math.Sign(val) * Math.Floor(Math.Abs(val)), (double)UInt64.MaxValue+1.0);
      if (rem == (double)Int64.MaxValue+1.0) {
        return Int64.MinValue;
      }
      return (Int64)rem;
    }
    
    
    private const Decimal DecimalTwoToThe64 = (Decimal)UInt64.MaxValue+1;        
    
    public static Int64 UncheckedDecimalToInt64(Decimal val) {
      // ToInteger
      val = Decimal.Truncate(val);
      
      // Ensure value is in range.
      if (val < Int64.MinValue || Int64.MaxValue <  val) {
        val = Decimal.Remainder(val, DecimalTwoToThe64);
        if (val < Int64.MinValue)
          val += DecimalTwoToThe64;
        else if (val > Int64.MaxValue)
          val -= DecimalTwoToThe64;
      }
      
      // Checked coercion is guaranteed to succeed
      Debug.Assert(Int64.MinValue <= val && val <= Int64.MaxValue);
      return (Int64)val;
    }
    
 
    // RUNTIME TYPEREFERENCES
    private static TypeReferences _typeRefs;    
    internal static TypeReferences TypeRefs {
      get {
        TypeReferences typeRefs = Runtime._typeRefs;
        if (typeRefs == null)
          typeRefs = Runtime._typeRefs = new TypeReferences(typeof(Runtime).Module);
        return typeRefs;
      }
    }
    
    // ASSEMBLY AND MODULE BUILDER FOR FIELDACCESSOR AND METHODINVOKER THUNKS
    private static ModuleBuilder _thunkModuleBuilder;
    internal static  ModuleBuilder ThunkModuleBuilder {
      get {
        ModuleBuilder moduleBuilder = Runtime._thunkModuleBuilder;
        if (null == moduleBuilder)
          moduleBuilder = Runtime._thunkModuleBuilder = CreateThunkModuleBuilder();
        return moduleBuilder;
      }
    }

    [ReflectionPermission(SecurityAction.Assert, ReflectionEmit = true), FileIOPermission(SecurityAction.Assert, Unrestricted = true)]
    private static ModuleBuilder CreateThunkModuleBuilder() {
      AssemblyName name = new AssemblyName();
      name.Name = "JScript Thunk Assembly";
      AssemblyBuilder assembly = Thread.GetDomain().DefineDynamicAssembly(name, AssemblyBuilderAccess.Run);
      ModuleBuilder module = assembly.DefineDynamicModule("JScript Thunk Module");  
      module.SetCustomAttribute(new CustomAttributeBuilder(typeof(SecurityTransparentAttribute).GetConstructor(new Type[0]), new Object[0]));
      return module;
    }
  }
}
