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
    using System.Diagnostics;

    public class DateConstructor : ScriptFunction{
      internal static readonly DateConstructor ob = new DateConstructor();  
      
      private DatePrototype originalPrototype; 
    
      internal DateConstructor()
        : base(FunctionPrototype.ob, "Date", 7) {
        this.originalPrototype = DatePrototype.ob;
        DatePrototype._constructor = this;
        this.proto = DatePrototype.ob;
      }
    
      internal DateConstructor(LenientFunctionPrototype parent, LenientDatePrototype prototypeProp)
        : base(parent, "Date", 7) {
        this.originalPrototype = prototypeProp;
        prototypeProp.constructor = this;
        this.proto = prototypeProp;
        this.noExpando = false;
      }
      
      internal override Object Call(Object[] args, Object thisob){
        return this.Invoke();
      }
      
      internal DateObject Construct(DateTime dt){
        return new DateObject(this.originalPrototype, dt.ToUniversalTime().Ticks  / DatePrototype.ticksPerMillisecond - DatePrototype.msTo1970);
      }
      
      internal override Object Construct(Object[] args){
        return this.CreateInstance(args);
      }
      
      [JSFunctionAttribute(JSFunctionAttributeEnum.HasVarArgs)]
      public new DateObject CreateInstance(params Object[] args){
        if (args.Length == 0)
          return new DateObject(this.originalPrototype, DateTime.Now.ToUniversalTime().Ticks  / DatePrototype.ticksPerMillisecond - DatePrototype.msTo1970);
        if (args.Length == 1){
          Object arg0 = args[0];
          IConvertible ic = Convert.GetIConvertible(arg0);
          TypeCode tcode = Convert.GetTypeCode(arg0, ic);
          if (tcode == TypeCode.DateTime)
            return new DateObject(this.originalPrototype, ic.ToDateTime(null).ToUniversalTime().Ticks  / DatePrototype.ticksPerMillisecond - DatePrototype.msTo1970);
          Object yv = Convert.ToPrimitive(arg0, PreferredType.Either, ref ic);
          if (Convert.GetTypeCode(yv, ic) == TypeCode.String)
            return new DateObject(this.originalPrototype, parse(ic.ToString(null)));
          double d = Convert.ToNumber(yv, ic);
          if (DatePrototype.minDate <= d && d <= DatePrototype.maxDate)
            return new DateObject(this.originalPrototype, d);
          return new DateObject(this.originalPrototype, Double.NaN);
        }
        double dyear = Convert.ToNumber(args[0]);
        double dmonth = Convert.ToNumber(args[1]);
        double ddate = (args.Length > 2) ? Convert.ToNumber(args[2]) : 1;
        double dhours = (args.Length > 3) ? Convert.ToNumber(args[3]) : 0;
        double dminutes = (args.Length > 4) ? Convert.ToNumber(args[4]) : 0;
        double dseconds = (args.Length > 5) ? Convert.ToNumber(args[5]) : 0;
        double dms = (args.Length > 6) ? Convert.ToNumber(args[6]) : 0;
        int y = (int)Runtime.DoubleToInt64(dyear);
        if (!Double.IsNaN(dyear) && 0 <= y  && y <= 99)
          dyear = y + 1900;
        double day = DatePrototype.MakeDay(dyear, dmonth, ddate);
        double time = DatePrototype.MakeTime(dhours, dminutes, dseconds, dms);
        return new DateObject(this.originalPrototype, DatePrototype.TimeClip(DatePrototype.UTC(DatePrototype.MakeDate(day, time))));
      }
      
      public String Invoke(){
        return DatePrototype.DateToString(DateTime.Now.ToUniversalTime().Ticks  / DatePrototype.ticksPerMillisecond - DatePrototype.msTo1970);
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Date_parse)]
      public static double parse(String str){
        return DatePrototype.ParseDate(str);
      }
      
      [JSFunctionAttribute(0, JSBuiltin.Date_UTC)]
      public static double UTC(Object year, Object month, Object date, Object hours, Object minutes, Object seconds, Object ms){
        if (year is Missing)
          return DateTime.Now.ToUniversalTime().Ticks  / DatePrototype.ticksPerMillisecond - DatePrototype.msTo1970;
        double dyear    =                          Convert.ToNumber(year);
        double dmonth   = month   is Missing ? 0 : Convert.ToNumber(month);
        double ddate    = date    is Missing ? 1 : Convert.ToNumber(date);
        double dhours   = hours   is Missing ? 0 : Convert.ToNumber(hours);
        double dminutes = minutes is Missing ? 0 : Convert.ToNumber(minutes);
        double dseconds = seconds is Missing ? 0 : Convert.ToNumber(seconds);
        double dms      = ms      is Missing ? 0 : Convert.ToNumber(ms);
        int y = (int)Runtime.DoubleToInt64(dyear);
        if (!Double.IsNaN(dyear) && 0 <= y  && y <= 99)
          dyear = y + 1900;
        double day = DatePrototype.MakeDay(dyear, dmonth, ddate);
        double time = DatePrototype.MakeTime(dhours, dminutes, dseconds, dms);
        return DatePrototype.TimeClip(DatePrototype.MakeDate(day, time));
      }
    }
}

