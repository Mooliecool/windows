//# ==++== 
//# 
//#   
//#    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//#   
//#    The use and distribution terms for this software are contained in the file
//#    named license.txt, which can be found in the root of this distribution.
//#    By using this software in any fashion, you are agreeing to be bound by the
//#    terms of this license.
//#   
//#    You must not remove this notice, or any other, from this software.
//#   
//# 
//# ==--== 
//####################################################################################
@cc_on


import System;

var NULL_DISPATCH = null;
var apGlobalObj;
var apPlatform;
var lFailCount;


var iTestID = 180869;

//////////////////////////////////////////////////////////////////////////////

@if(!@aspx)
	import System
@end
var func
var type

function diff(actual, expected, delta){
    if (!isFinite(actual) || !isFinite(expected)){
        if (isNaN(actual) && isNaN(expected)) return false
        if (actual == expected) return false
        else return true
    }
    if (actual == 0 || expected == 0){
        if ((actual - expected) <= (delta-1) && (expected - actual) <= (delta-1))
            return false
        else return true
    }

    if (((actual<0?-actual:actual) / (expected<0?-expected:expected)) <= delta && Math.abs((expected<0?-expected:expected) / (actual<0?-actual:actual)) <= delta)
        return false
    return true
}


function retvar(val){return val}
function rettyped1(val){return type(val)}
function rettyped2(val){
	return eval("   var ret:type = val; ret")
}

function verifyParams(errstr, input1, input2, expected, ... params:Object[]){
    var delta = 1.000001
    if (params.length>0) delta = 1+params[0]


    if (typeof(func(input1, input2)) !== typeof(expected))
        apLogFailInfo(errstr + ": type failure", typeof(expected), typeof(func(input1, input2)), "");


    if (diff( func(type(input1), type(input2)), expected, delta))
        apLogFailInfo(errstr + ": immediate failure", expected, func(type(input1), type(input2)), "");


    //eval("	var insttype1:type=input1, insttype2:type=input2; if (diff( func(insttype1, insttype2), expected, delta)) apLogFailInfo(errstr + ': typed instance failure', expected, func(insttype1, insttype2), ''); ")


    var instob1:Object = type(input1)
    var instob2:Object = type(input2)
    if (diff( func(instob1, instob2), expected, delta))
        apLogFailInfo(errstr + ": object instance failure", expected, func(instob1, instob2), "");


    var memob1 = new Object
    memob1.mem = type(input1)
    var memob2 = new Object
    memob2.mem = type(input2)
    if (diff( func(memob1.mem, memob2.mem), expected, delta))
        apLogFailInfo(errstr + ": object member failure", expected, func(memob1.mem, memob2.mem), "");


    if (diff( func(  retvar(type(input1)),retvar(type(input2))  ), expected, delta))
        apLogFailInfo(errstr + ": function return failure", expected, func(retvar(type(input1)),retvar(type(input2))), "");


    if (diff( func(  rettyped1(input1),rettyped1(input2)  ), expected, delta))
        apLogFailInfo(errstr + ": function typed return failure 1", expected, func(rettyped1(input1),rettyped1(input2)), "");


    //if (diff( func(  rettyped2(input1),rettyped2(input2)  ), expected, delta))
    //    apLogFailInfo(errstr + ": function typed return failure 2", expected, func(rettyped2(input1),rettyped2(input2)), "");

}


function tstRnd(errstr, min, max, ... info:Object[]){
    var i:int;
	var vNum1:Number, vNum2:Number, rounds = 20

    if (info.length>0) rounds = info[0];
    for (i=0; i<rounds; i++)
    {
        vNum1 = Math.floor(Math.random()*(max+1-min))+min; // gives int num between min and max
        vNum2 = Math.floor(Math.random()*(max+1-min))+min; // gives int num between min and max
        verifyParams(errstr,vNum1,vNum2,calcExpected(vNum1,vNum2));
    }
}


function tstExp(errstr, inMin, inMax, res){
    var vNum1, vNum2, nVal, iPow;
    var sign = inMax>0?1:-1;
    var max = sign*inMax, min = sign*inMin;

    for (iPow=0;;iPow+=res)
    {
        vNum1 = (Math.random()*Math.pow(2,res))*Math.pow(2,iPow);
        if (vNum1<max)
            verifyParams(errstr,sign*vNum1,sign*vNum1,calcExpected(sign*vNum1,sign*vNum1));

        vNum2 = (Math.random()*Math.pow(2,res))/Math.pow(2,iPow);
        if (vNum2>min)
            verifyParams(errstr,sign*vNum2,sign*vNum2,calcExpected(sign*vNum2,sign*vNum2));

        if (vNum1<max && vNum2>min){
            verifyParams (errstr, vNum1, vNum2, calcExpected(vNum1, vNum2))
            verifyParams (errstr, vNum1, -vNum2, calcExpected(vNum1, -vNum2))
            verifyParams (errstr, -vNum1, vNum2, calcExpected(-vNum1, vNum2))
            verifyParams (errstr, -vNum1, -vNum2, calcExpected(-vNum1, -vNum2))
            verifyParams (errstr, vNum2, vNum1, calcExpected(vNum2, vNum1))
            verifyParams (errstr, vNum2, -vNum1, calcExpected(vNum2, -vNum1))
            verifyParams (errstr, -vNum2, vNum1, calcExpected(-vNum2, vNum1))
            verifyParams (errstr, -vNum2, -vNum1, calcExpected(-vNum2, -vNum1))
        }
        if (vNum1>=max && vNum2<=min) break
    }    
}

function tstDec(errstr, inMin, inMax, res){
    var vNum1:decimal=0, vNum2:decimal=0, nVal:decimal, iPow:decimal;
    var sign:decimal = inMax>0?1:-1;
    var max = sign*inMax, min = sign*inMin;

    for (iPow=0;;iPow+=res)
    {
        try{
            vNum1 = (Math.random()*Math.pow(2,res))*Math.pow(2,iPow);
            if (vNum1<max)
                verifyParams(errstr,sign*vNum1,sign*vNum1,calcExpected(sign*vNum1,sign*vNum1), .0000000001);
        }catch(e){
            vNum1 = decimal.MaxValue
        }
        try{
            vNum2 = (Math.random()*Math.pow(2,res))/Math.pow(2,iPow);
            if (vNum2>min)
                verifyParams(errstr,sign*vNum2,sign*vNum2,calcExpected(sign*vNum2,sign*vNum2), .0000000001);
        }catch(e){
            vNum1 = decimal.MaxValue
        }
        if (vNum1<max && vNum2>min){
            verifyParams (errstr, vNum1, vNum2, calcExpected(vNum1, vNum2), .00000001)
            verifyParams (errstr, vNum1, -vNum2, calcExpected(vNum1, -vNum2), .00000001)
            verifyParams (errstr, -vNum1, vNum2, calcExpected(-vNum1, vNum2), .00000001)
            verifyParams (errstr, -vNum1, -vNum2, calcExpected(-vNum1, -vNum2), .00000001)
            verifyParams (errstr, vNum2, vNum1, calcExpected(vNum2, vNum1), .00000001)
            verifyParams (errstr, vNum2, -vNum1, calcExpected(vNum2, -vNum1), .00000001)
            verifyParams (errstr, -vNum2, vNum1, calcExpected(-vNum2, vNum1), .00000001)
            verifyParams (errstr, -vNum2, -vNum1, calcExpected(-vNum2, -vNum1), .00000001)
        }
        if ((sign==1 && vNum1>=max && vNum2<=min) || (sign==-1 && vNum1>=max && vNum2<=min)) break
    }    
}
	

function calcExpected(xin, yin){
	if (xin>yin) return xin
	else return yin
}



function maxh001() {


    var i
    func = Math.max


    apInitTest("maxh001 ");

    type = boolean
    apInitScenario("max of a boolean");

    verifyParams("boolean: true or true", true, true, 1, 0)
    verifyParams("boolean: true or false", true, false, 1, 0)
    verifyParams("boolean: false or true", false, true, 1, 0)
    verifyParams("boolean: false or false", false, false, 0, 0)


    type = char;
    apInitScenario("max of a char");

    verifyParams("char: 0 or 0", char.MinValue, char.MinValue, 0, 0)
    verifyParams("char: 0 or 1", char.MinValue, 1, 1, 0)
    verifyParams("char: 0 or char.max", char.MinValue, char.MaxValue, 65535, 0)

    verifyParams("char: 1 or 0", 1, char.MinValue, 1, 0)
    verifyParams("char: 1 or 1", 1, 1, 1, 0)
    verifyParams("char: 1 or char.max", 1, char.MaxValue, 65535, 0)

    verifyParams("char: char.max or 0", char.MaxValue, char.MinValue, 65535, 0)
    verifyParams("char: char.max or 1", char.MaxValue, 1, 65535, 0)
    verifyParams("char: char.max or char.max", char.MaxValue, char.MaxValue, 65535, 0)

    tstRnd("char: char.min < x,y < char.max", char.MinValue+1, char.MaxValue-1, 200)


    type = Single;
    apInitScenario("max of a Single");

    verifyParams("single: -0 or -0", -0, -0, -0, 0)
    verifyParams("single: -0 or +0", -0, +0, 0, 0)
    verifyParams("single: +0 or -0", +0, -0, 0, 0)
    verifyParams("single: +0 or +0", +0, +0, 0, 0)

    verifyParams("single: -1 or -1", -1, -1, -1, 0)
    verifyParams("single: -1 or +1", -1, +1, 1, 0)
    verifyParams("single: +1 or -1", +1, -1, 1, 0)
    verifyParams("single: +1 or +1", +1, +1, 1, 0)

    verifyParams("single: single.MinValue or single.MinValue", Single.MinValue , Single.MinValue, -3.4028234663852886e+38)
    verifyParams("single: single.MinValue or single.MaxValue", Single.MinValue , Single.MaxValue, 3.4028234663852886e+38)
    verifyParams("single: single.MaxValue or single.MinValue", Single.MaxValue, Single.MinValue, Single.MaxValue)
    verifyParams("single: single.MaxValue or single.MaxValue", Single.MaxValue, Single.MaxValue, Single.MaxValue)

    verifyParams("single: single.Epsilon or single.Epsilon", Single.Epsilon , Single.Epsilon , 1.401298464324817e-45)
    verifyParams("single: single.Epsilon or single.MaxValue", Single.Epsilon , Single.MaxValue, Single.MaxValue)
    verifyParams("single: single.MaxValue or single.Epsilon", Single.MaxValue, Single.Epsilon , Single.MaxValue)
    verifyParams("single: single.Epsilon or single.MinValue", Single.Epsilon , Single.MinValue, Single.Epsilon, 0)
    verifyParams("single: single.MinValue or single.Epsilon", Single.MinValue, Single.Epsilon , Single.Epsilon, 0)

    verifyParams("single: Infinity or Infinity", Single.PositiveInfinity, Single.PositiveInfinity, Infinity)
    verifyParams("single: Infinity or -Infinity", Single.PositiveInfinity, Single.NegativeInfinity, Infinity)
    verifyParams("single: -Infinity or Infinity", Single.NegativeInfinity, Single.PositiveInfinity, Infinity)
    verifyParams("single: -Infinity or -Infinity", Single.NegativeInfinity, Single.NegativeInfinity, -Infinity)

    verifyParams("single: Infinity or 0", Single.PositiveInfinity, 0, Infinity)
    verifyParams("single: 0 or Infinity", 0, Single.PositiveInfinity, Infinity)
    verifyParams("single: Infinity or Single.Epsilon", Single.PositiveInfinity, Single.Epsilon, Infinity)
    verifyParams("single: Single.Epsilon or Infinity", Single.Epsilon, Single.PositiveInfinity, Infinity)
    verifyParams("single: Infinity or 1", Single.PositiveInfinity, 1, Infinity)
    verifyParams("single: 1 or Infinity", 1, Single.PositiveInfinity, Infinity)
    verifyParams("single: Infinity or Single.MaxValue", Single.PositiveInfinity, Single.MaxValue, Infinity)
    verifyParams("single: Single.MaxValue or Infinity", Single.MaxValue, Single.PositiveInfinity, Infinity)

    tstExp("single: 0 < n < 100", 1e-2, 1e2, 1)
    tstExp("single: -100 < n < 0", -1e-2, -1e2, 1)
    tstExp("single: 0 < n < single.max", 1e-38, Single.MaxValue, 10)
    tstExp("single: single.min < n < 0", -1e-38, Single.MinValue, 10)


    type = double;
    apInitScenario("max of a double");

    verifyParams("double: -0 or -0", -0, -0, -0, 0)
    verifyParams("double: -0 or +0", -0, +0, 0, 0)
    verifyParams("double: +0 or -0", +0, -0, 0, 0)
    verifyParams("double: +0 or +0", +0, +0, 0, 0)

    verifyParams("double: -1 or -1", -1, -1, -1, 0)
    verifyParams("double: -1 or +1", -1, +1, 1, 0)
    verifyParams("double: +1 or -1", +1, -1, 1, 0)
    verifyParams("double: +1 or +1", +1, +1, 1, 0)

    verifyParams("double: double.MinValue or double.MinValue", double.MinValue , double.MinValue, -1.7976931348623157e+308)
    verifyParams("double: double.MinValue or double.MaxValue", double.MinValue , double.MaxValue, 1.7976931348623157e+308)
    verifyParams("double: double.MaxValue or double.MinValue", double.MaxValue, double.MinValue, double.MaxValue)
    verifyParams("double: double.MaxValue or double.MaxValue", double.MaxValue, double.MaxValue, double.MaxValue)

    verifyParams("double: double.Epsilon or double.Epsilon", double.Epsilon , double.Epsilon , 4.94065645841247e-324)
    verifyParams("double: double.Epsilon or double.MaxValue", double.Epsilon , double.MaxValue, double.MaxValue)
    verifyParams("double: double.MaxValue or double.Epsilon", double.MaxValue, double.Epsilon , double.MaxValue)
    verifyParams("double: double.Epsilon or double.MinValue", double.Epsilon , double.MinValue, double.Epsilon, 0)
    verifyParams("double: double.MinValue or double.Epsilon", double.MinValue, double.Epsilon , double.Epsilon, 0)

    verifyParams("double: Infinity or Infinity", double.PositiveInfinity, double.PositiveInfinity, Infinity)
    verifyParams("double: Infinity or -Infinity", double.PositiveInfinity, double.NegativeInfinity, Infinity)
    verifyParams("double: -Infinity or Infinity", double.NegativeInfinity, double.PositiveInfinity, Infinity)
    verifyParams("double: -Infinity or -Infinity", double.NegativeInfinity, double.NegativeInfinity, -Infinity)

    verifyParams("double: Infinity or 0", double.PositiveInfinity, 0, Infinity)
    verifyParams("double: 0 or Infinity", 0, double.PositiveInfinity, Infinity)
    verifyParams("double: Infinity or double.Epsilon", double.PositiveInfinity, double.Epsilon, Infinity)
    verifyParams("double: double.Epsilon or Infinity", double.Epsilon, double.PositiveInfinity, Infinity)
    verifyParams("double: Infinity or 1", double.PositiveInfinity, 1, Infinity)
    verifyParams("double: 1 or Infinity", 1, double.PositiveInfinity, Infinity)
    verifyParams("double: Infinity or double.MaxValue", double.PositiveInfinity, double.MaxValue, Infinity)
    verifyParams("double: double.MaxValue or Infinity", double.MaxValue, double.PositiveInfinity, Infinity)

    tstExp("double: 0 < n < 100", 1e-2, 1e2, 1)
    tstExp("double: -100 < n < 0", -1e-2, -1e2, 1)
    tstExp("double: 0 < n < double.max", 1e-38, double.MaxValue, 10)
    tstExp("double: double.min < n < 0", -1e-38, double.MinValue, 10)


    type = decimal;
    apInitScenario("max of a decimal");

    verifyParams("decimal: 0 or 0", 0, 0, 0, 0)

    verifyParams("decimal: -1 or -1", -1, -1, -1, 0)
    verifyParams("decimal: -1 or +1", -1, +1, 1, 0)
    verifyParams("decimal: +1 or -1", +1, -1, 1, 0)
    verifyParams("decimal: +1 or +1", +1, +1, 1, 0)
    //apLogFailInfo("decimal math bug", 0, 0, 282120)
/*    verifyParams("decimal: decimal.MinValue or decimal.MinValue", decimal.MinValue , decimal.MinValue , -79228162514264337593543950335)
    verifyParams("decimal: decimal.MinValue or decimal.MaxValue", decimal.MinValue , decimal.MaxValue, 79228162514264337593543950335)
    verifyParams("decimal: decimal.MaxValue or decimal.MinValue", decimal.MaxValue, decimal.MinValue , decimal.MaxValue)
    verifyParams("decimal: decimal.MaxValue or decimal.MaxValue", decimal.MaxValue, decimal.MaxValue, decimal.MaxValue)
*/
    tstDec("decimal: 0 < n < 100", 1e-2, 1e2, 1)
    tstDec("decimal: 100 < n < 0", -1e-2, -1e2, 1)
    //tstDec("decimal: 0 < n < decimal.max", 1e-40, decimal.MaxValue, 10)
    //tstDec("decimal: decimal.min < n < 0", -1e-40, decimal.MinValue, 10)


    type = byte;
    apInitScenario("max of a byte");

    verifyParams("byte: 0 or 0", byte.MinValue, byte.MinValue, 0, 0)
    verifyParams("byte: 0 or 1", byte.MinValue, 1, 1, 0)
    verifyParams("byte: 0 or byte.max", byte.MinValue, byte.MaxValue, 255, 0)

    verifyParams("byte: 1 or 0", 1, byte.MinValue, 1, 0)
    verifyParams("byte: 1 or 1", 1, 1, 1, 0)
    verifyParams("byte: 1 or byte.max", 1, byte.MaxValue, 255, 0)

    verifyParams("byte: byte.max or 0", byte.MaxValue, byte.MinValue, 255, 0)
    verifyParams("byte: byte.max or 1", byte.MaxValue, 1, 255, 0)
    verifyParams("byte: byte.max or byte.max", byte.MaxValue, byte.MaxValue, 255, 0)

    tstRnd("byte: byte.min < x,y < 50", byte.MinValue+1, 50)
    tstRnd("byte: byte.min < x,y < byte.max", byte.MinValue+1, byte.MaxValue-1)


    type = ushort;
    apInitScenario("max of a ushort");

    verifyParams("ushort: 0 or 0", ushort.MinValue, ushort.MinValue, 0, 0)
    verifyParams("ushort: 0 or 1", ushort.MinValue, 1, 1, 0)
    verifyParams("ushort: 0 or ushort.max", ushort.MinValue, ushort.MaxValue, 65535, 0)

    verifyParams("ushort: 1 or 0", 1, ushort.MinValue, 1, 0)
    verifyParams("ushort: 1 or 1", 1, 1, 1, 0)
    verifyParams("ushort: 1 or ushort.max", 1, ushort.MaxValue, 65535, 0)

    verifyParams("ushort: ushort.max or 0", ushort.MaxValue, ushort.MinValue, 65535, 0)
    verifyParams("ushort: ushort.max or 1", ushort.MaxValue, 1, 65535, 0)
    verifyParams("ushort: ushort.max or ushort.max", ushort.MaxValue, ushort.MaxValue, 65535, 0)

    tstRnd("ushort: ushort.min < x,y < 50", ushort.MinValue+1, 50)
    tstRnd("ushort: ushort.min < x,y < ushort.max", ushort.MinValue+1, ushort.MaxValue-1)


    type = uint;
    apInitScenario("max of a uint");

    verifyParams("uint: 0 or 0", uint.MinValue, uint.MinValue, 0, 0)
    verifyParams("uint: 0 or 1", uint.MinValue, 1, 1, 0)
    verifyParams("uint: 0 or uint.max", uint.MinValue, uint.MaxValue, 4294967295, 0)

    verifyParams("uint: 1 or 0", 1, uint.MinValue, 1, 0)
    verifyParams("uint: 1 or 1", 1, 1, 1, 0)
    verifyParams("uint: 1 or uint.max", 1, uint.MaxValue, 4294967295, 0)

    verifyParams("uint: uint.max or 0", uint.MaxValue, uint.MinValue, 4294967295, 0)
    verifyParams("uint: uint.max or 1", uint.MaxValue, 1, 4294967295, 0)
    verifyParams("uint: uint.max or uint.max", uint.MaxValue, uint.MaxValue, 4294967295, 0)

    tstRnd("uint: uint.min < x,y < 50", uint.MinValue+1, 50)
    tstRnd("uint: uint.min < x,y < uint.max", uint.MinValue+1, uint.MaxValue-1)


    type = ulong;
    apInitScenario("max of a ulong");

    verifyParams("ulong: 0 or 0", ulong.MinValue, ulong.MinValue, 0, 0)
    verifyParams("ulong: 0 or 1", ulong.MinValue, 1, 1, 0)
    verifyParams("ulong: 0 or ulong.max", ulong.MinValue, ulong.MaxValue, 18446744073709551615, 0)

    verifyParams("ulong: 1 or 0", 1, ulong.MinValue, 1, 0)
    verifyParams("ulong: 1 or 1", 1, 1, 1, 0)
    verifyParams("ulong: 1 or ulong.max", 1, ulong.MaxValue, 18446744073709551615, 0)

    verifyParams("ulong: ulong.max or 0", ulong.MaxValue, ulong.MinValue, 18446744073709551615, 0)
    verifyParams("ulong: ulong.max or 1", ulong.MaxValue, 1, 18446744073709551615, 0)
    verifyParams("ulong: ulong.max or ulong.max", ulong.MaxValue, ulong.MaxValue, 18446744073709551615, 0)

    tstRnd("ulong: ulong.min < x,y < 50", ulong.MinValue+1, 50)
    tstRnd("ulong: ulong.min < x,y < ulong.max", ulong.MinValue+1, ulong.MaxValue-1)


    type = sbyte;
    apInitScenario("max of a sbyte");

    verifyParams("sbyte: sbyte.min or sbyte.min", sbyte.MinValue, sbyte.MinValue, -128, 0)
    verifyParams("sbyte: sbyte.min or 0", sbyte.MinValue, 0, 0, 0)
    verifyParams("sbyte: sbyte.min or 1", sbyte.MinValue, 1, 1, 0)
    verifyParams("sbyte: sbyte.min or sbyte.max", sbyte.MinValue, sbyte.MaxValue, 127, 0)

    verifyParams("sbyte: -1 or sbyte.min", -1, sbyte.MinValue, -1, 0)
    verifyParams("sbyte: -1 or 0", -1, 0, 0, 0)
    verifyParams("sbyte: -1 or 1", -1, 1, 1, 0)
    verifyParams("sbyte: -1 or sbyte.max", -1, sbyte.MaxValue, sbyte.MaxValue, 0)

    verifyParams("sbyte: 0 or sbyte.min", 0, sbyte.MinValue, 0, 0)
    verifyParams("sbyte: 0 or 0", 0, 0, 0, 0)
    verifyParams("sbyte: 0 or 1", 0, 1, 1, 0)
    verifyParams("sbyte: 0 or sbyte.max", 0, sbyte.MaxValue, sbyte.MaxValue, 0)

    verifyParams("sbyte: 1 or sbyte.min", 1, sbyte.MinValue, 1, 0)
    verifyParams("sbyte: 1 or 0", 1, 0, 1, 0)
    verifyParams("sbyte: 1 or 1", 1, 1, 1, 0)
    verifyParams("sbyte: 1 or sbyte.max", 1, sbyte.MaxValue, sbyte.MaxValue, 0)

    verifyParams("sbyte: sbyte.max or sbyte.min", sbyte.MaxValue, sbyte.MinValue, sbyte.MaxValue, 0)
    verifyParams("sbyte: sbyte.max or 0", sbyte.MaxValue, 0, sbyte.MaxValue, 0)
    verifyParams("sbyte: sbyte.max or 1", sbyte.MaxValue, 1, sbyte.MaxValue, 0)
    verifyParams("sbyte: sbyte.max or sbyte.max", sbyte.MaxValue, sbyte.MaxValue, sbyte.MaxValue, 0)

    tstRnd("sbyte: sbyte.min < x,y < 50", sbyte.MinValue+1, 50)
    tstRnd("sbyte: sbyte.min < x,y < sbyte.max", sbyte.MinValue+1, sbyte.MaxValue-1)


    type = short;
    apInitScenario("max of a short");

    verifyParams("short: short.min or short.min", short.MinValue, short.MinValue, -32768, 0)
    verifyParams("short: short.min or 0", short.MinValue, 0, 0, 0)
    verifyParams("short: short.min or 1", short.MinValue, 1, 1, 0)
    verifyParams("short: short.min or short.max", short.MinValue, short.MaxValue, 32767, 0)

    verifyParams("short: -1 or short.min", -1, short.MinValue, -1, 0)
    verifyParams("short: -1 or 0", -1, 0, 0, 0)
    verifyParams("short: -1 or 1", -1, 1, 1, 0)
    verifyParams("short: -1 or short.max", -1, short.MaxValue, short.MaxValue, 0)

    verifyParams("short: 0 or short.min", 0, short.MinValue, 0, 0)
    verifyParams("short: 0 or 0", 0, 0, 0, 0)
    verifyParams("short: 0 or 1", 0, 1, 1, 0)
    verifyParams("short: 0 or short.max", 0, short.MaxValue, short.MaxValue, 0)

    verifyParams("short: 1 or short.min", 1, short.MinValue, 1, 0)
    verifyParams("short: 1 or 0", 1, 0, 1, 0)
    verifyParams("short: 1 or 1", 1, 1, 1, 0)
    verifyParams("short: 1 or short.max", 1, short.MaxValue, short.MaxValue, 0)

    verifyParams("short: short.max or short.min", short.MaxValue, short.MinValue, short.MaxValue, 0)
    verifyParams("short: short.max or 0", short.MaxValue, 0, short.MaxValue, 0)
    verifyParams("short: short.max or 1", short.MaxValue, 1, short.MaxValue, 0)
    verifyParams("short: short.max or short.max", short.MaxValue, short.MaxValue, short.MaxValue, 0)

    tstRnd("short: short.min < x,y < 50", short.MinValue+1, 50)
    tstRnd("short: short.min < x,y < short.max", short.MinValue+1, short.MaxValue-1)


    type = int;
    apInitScenario("max of a int");

    verifyParams("int: int.min or int.min", int.MinValue, int.MinValue, -2147483648, 0)
    verifyParams("int: int.min or 0", int.MinValue, 0, 0, 0)
    verifyParams("int: int.min or 1", int.MinValue, 1, 1, 0)
    verifyParams("int: int.min or int.max", int.MinValue, int.MaxValue, 2147483647, 0)

    verifyParams("int: -1 or int.min", -1, int.MinValue, -1, 0)
    verifyParams("int: -1 or 0", -1, 0, 0, 0)
    verifyParams("int: -1 or 1", -1, 1, 1, 0)
    verifyParams("int: -1 or int.max", -1, int.MaxValue, int.MaxValue, 0)

    verifyParams("int: 0 or int.min", 0, int.MinValue, 0, 0)
    verifyParams("int: 0 or 0", 0, 0, 0, 0)
    verifyParams("int: 0 or 1", 0, 1, 1, 0)
    verifyParams("int: 0 or int.max", 0, int.MaxValue, int.MaxValue, 0)

    verifyParams("int: 1 or int.min", 1, int.MinValue, 1, 0)
    verifyParams("int: 1 or 0", 1, 0, 1, 0)
    verifyParams("int: 1 or 1", 1, 1, 1, 0)
    verifyParams("int: 1 or int.max", 1, int.MaxValue, int.MaxValue, 0)

    verifyParams("int: int.max or int.min", int.MaxValue, int.MinValue, int.MaxValue, 0)
    verifyParams("int: int.max or 0", int.MaxValue, 0, int.MaxValue, 0)
    verifyParams("int: int.max or 1", int.MaxValue, 1, int.MaxValue, 0)
    verifyParams("int: int.max or int.max", int.MaxValue, int.MaxValue, int.MaxValue, 0)

    tstRnd("int: int.min < x,y < 50", int.MinValue+1, 50)
    tstRnd("int: int.min < x,y < int.max", int.MinValue+1, int.MaxValue-1)


    type = long;
    apInitScenario("max of a long");

    verifyParams("long: long.min or long.min", long.MinValue, long.MinValue, -9223372036854775808, 0)
    verifyParams("long: long.min or 0", long.MinValue, 0, 0, 0)
    verifyParams("long: long.min or 1", long.MinValue, 1, 1, 0)
    verifyParams("long: long.min or long.max", long.MinValue, long.MaxValue, 9223372036854775807, 0)

    verifyParams("long: -1 or long.min", -1, long.MinValue, -1, 0)
    verifyParams("long: -1 or 0", -1, 0, 0, 0)
    verifyParams("long: -1 or 1", -1, 1, 1, 0)
    verifyParams("long: -1 or long.max", -1, long.MaxValue, long.MaxValue, 0)

    verifyParams("long: 0 or long.min", 0, long.MinValue, 0, 0)
    verifyParams("long: 0 or 0", 0, 0, 0, 0)
    verifyParams("long: 0 or 1", 0, 1, 1, 0)
    verifyParams("long: 0 or long.max", 0, long.MaxValue, long.MaxValue, 0)

    verifyParams("long: 1 or long.min", 1, long.MinValue, 1, 0)
    verifyParams("long: 1 or 0", 1, 0, 1, 0)
    verifyParams("long: 1 or 1", 1, 1, 1, 0)
    verifyParams("long: 1 or long.max", 1, long.MaxValue, long.MaxValue, 0)

    verifyParams("long: long.max or long.min", long.MaxValue, long.MinValue, long.MaxValue, 0)
    verifyParams("long: long.max or 0", long.MaxValue, 0, long.MaxValue, 0)
    verifyParams("long: long.max or 1", long.MaxValue, 1, long.MaxValue, 0)
    verifyParams("long: long.max or long.max", long.MaxValue, long.MaxValue, long.MaxValue, 0)

    tstRnd("long: long.min < x,y < 50", long.MinValue+1, 50)
    tstRnd("long: long.min < x,y < long.max", long.MinValue+1, long.MaxValue-1)


    type = String;
    apInitScenario("max of a String");

    verifyParams("String: long.min or long.min", long.MinValue, long.MinValue, -9223372036854775808, 0)
    verifyParams("String: long.min or 0", long.MinValue, 0, 0, 0)
    verifyParams("String: long.min or 1", long.MinValue, 1, 1, 0)
    verifyParams("String: long.min or long.max", long.MinValue, long.MaxValue, 9223372036854775807, 0)

    verifyParams("String: -1 or long.min", -1, long.MinValue, -1, 0)
    verifyParams("String: -1 or 0", -1, 0, 0, 0)
    verifyParams("String: -1 or 1", -1, 1, 1, 0)
    verifyParams("String: -1 or long.max", -1, long.MaxValue, long.MaxValue, 0)

    verifyParams("String: 0 or long.min", 0, long.MinValue, 0, 0)
    verifyParams("String: 0 or 0", 0, 0, 0, 0)
    verifyParams("String: 0 or 1", 0, 1, 1, 0)
    verifyParams("String: 0 or long.max", 0, long.MaxValue, long.MaxValue, 0)

    verifyParams("String: 1 or long.min", 1, long.MinValue, 1, 0)
    verifyParams("String: 1 or 0", 1, 0, 1, 0)
    verifyParams("String: 1 or 1", 1, 1, 1, 0)
    verifyParams("String: 1 or long.max", 1, long.MaxValue, long.MaxValue, 0)

    verifyParams("String: long.max or long.min", long.MaxValue, long.MinValue, long.MaxValue, 0)
    verifyParams("String: long.max or 0", long.MaxValue, 0, long.MaxValue, 0)
    verifyParams("String: long.max or 1", long.MaxValue, 1, long.MaxValue, 0)
    verifyParams("String: long.max or long.max", long.MaxValue, long.MaxValue, long.MaxValue, 0)

    tstRnd("String: long.min < x,y < 50", long.MinValue+1, 50)
    tstRnd("String: long.min < x,y < long.max", long.MinValue+1, long.MaxValue-1)


    type = Date;
    apInitScenario("max of a Date");
    verifyParams("Date: 4 or 4", 4, 4, NaN)


    type = Array;
    apInitScenario("max of a Array");
    var arrvar:Array = new Array(1)
    arrvar[0] = 1.5
    verifyParams("Array: 1.5 or 2", arrvar, 1, 1.5)
    verifyParams("Array: 1.5 or 2", arrvar, 2, NaN)

    apEndTest();

}


maxh001();


if(lFailCount >= 0) System.Environment.ExitCode = lFailCount;
else System.Environment.ExitCode = 1;

function apInitTest(stTestName) {
    lFailCount = 0;

    apGlobalObj = new Object();
    apGlobalObj.apGetPlatform = function Funca() { return "Rotor" }
    apGlobalObj.LangHost = function Funcb() { return 1033;}
    apGlobalObj.apGetLangExt = function Funcc(num) { return "EN"; }

    apPlatform = apGlobalObj.apGetPlatform();
    var sVer = "1.0";  //navigator.appVersion.toUpperCase().charAt(navigator.appVersion.toUpperCase().indexOf("MSIE")+5);
    apGlobalObj.apGetHost = function Funcp() { return "Rotor " + sVer; }
    print ("apInitTest: " + stTestName);
}

function apInitScenario(stScenarioName) {print( "\tapInitScenario: " + stScenarioName);}

function apLogFailInfo(stMessage, stExpected, stActual, stBugNum) {
    lFailCount = lFailCount + 1;
    print ("***** FAILED:");
    print ("\t\t" + stMessage);
    print ("\t\tExpected: " + stExpected);
    print ("\t\tActual: " + stActual);
}

function apGetLocale(){ return 1033; }
function apWriteDebug(s) { print("dbg ---> " + s) }
function apEndTest() {}
