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


var iTestID = 180811;

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

    if (Math.abs(actual / expected) <= delta && Math.abs(expected / actual) <= delta)
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
    var x = Number(type(xin))
    var y = Number(type(yin))

    if (isNaN(y)) return NaN
    if (y==+0 || y==-0) return 1
    if (isNaN(x) && y!=0) return NaN
    if (Math.abs(x)>1 && y==Infinity) return Infinity
    if (Math.abs(x)>1 && y==-Infinity) return +0
    if (Math.abs(x)==1 && y==Infinity) return NaN
    if (Math.abs(x)==1 && y==-Infinity) return NaN
    if (Math.abs(x)<1 && y==Infinity) return +0
    if (Math.abs(x)<1 && y==-Infinity) return Infinity
    if (x == Infinity && y>0) return Infinity
    if (x == Infinity && y<0) return +0
    if (x == -Infinity && y>0 && ((y%2)==1)) return -Infinity
    if (x == -Infinity && y>0 && ((y%2)!=1)) return Infinity
    if (x == -Infinity && y<0 && ((y%2)==1)) return -0
    if (x == -Infinity && y<0 && ((y%2)!=1)) return +0
    if (x == +0 && y>0) return +0
    if (x == +0 && y<0) return +Infinity
    if (x == -0 && y>0 && ((y%2)==1)) return -0
    if (x == -0 && y>0 && ((y%2)!=1)) return +0
    if (x == -0 && y<0 && ((y%2)==1)) return -Infinity
    if (x == -0 && y<0 && ((y%2)!=1)) return +Infinity
    if (x<0 && isFinite(x) && isFinite(y) && y!=Math.floor(y)) return NaN

    if (x<0){
        if (y == Math.floor(y))
            return (((y%2)*(y%2))*-2+1) * Math.exp(y * Math.log(-x))
	else return NaN
    }
    return Math.exp(y * Math.log(x))
}



function powh001() {


    var i
    func = Math.pow


    apInitTest("pow001 ");

    type = boolean
    apInitScenario("pow of a boolean");

    verifyParams("boolean: true ^ true", true, true, 1, 0)
    verifyParams("boolean: true ^ false", true, false, 1, 0)
    verifyParams("boolean: false ^ true", false, true, 0, 0)
    verifyParams("boolean: false ^ false", false, false, 1, 0)


    type = char;
    apInitScenario("pow of a char");

    verifyParams("char: 0 ^ 0", char.MinValue, char.MinValue, 1, 0)
    verifyParams("char: 0 ^ 1", char.MinValue, 1, 0, 0)
    verifyParams("char: 0 ^ char.max", char.MinValue, char.MaxValue, 0, 0)

    verifyParams("char: 1 ^ 0", 1, char.MinValue, 1, 0)
    verifyParams("char: 1 ^ 1", 1, 1, 1, 0)
    verifyParams("char: 1 ^ char.max", 1, char.MaxValue, 1, 0)

    verifyParams("char: char.max ^ 0", char.MaxValue, char.MinValue, 1, 0)
    verifyParams("char: char.max ^ 1", char.MaxValue, 1, 65535, 0)
    verifyParams("char: char.max ^ char.max", char.MaxValue, char.MaxValue, Infinity)

    tstRnd("char: char.min < x,y < 50", char.MinValue+1, 50)
    tstRnd("char: char.min < x,y < char.max", char.MinValue+1, char.MaxValue-1)


    type = Single;
    apInitScenario("pow of a Single");

    verifyParams("single: -0 ^ -0", -0, -0, 1, 0)
    verifyParams("single: -0 ^ +0", -0, +0, 1, 0)
    verifyParams("single: +0 ^ -0", +0, -0, 1, 0)
    verifyParams("single: +0 ^ +0", +0, +0, 1, 0)

    verifyParams("single: -1 ^ -1", -1, -1, -1, 0)
    verifyParams("single: -1 ^ +1", -1, +1, -1, 0)
    verifyParams("single: +1 ^ -1", +1, -1, 1, 0)
    verifyParams("single: +1 ^ +1", +1, +1, 1, 0)

    verifyParams("single: single.MinValue ^ single.MinValue", Single.MinValue , Single.MinValue , 0)
    verifyParams("single: single.MinValue ^ single.MaxValue", Single.MinValue , Single.MaxValue, Infinity)
    verifyParams("single: single.MaxValue ^ single.MinValue", Single.MaxValue, Single.MinValue , 0)
    verifyParams("single: single.MaxValue ^ single.MaxValue", Single.MaxValue, Single.MaxValue, Infinity)

    verifyParams("single: single.Epsilon ^ single.Epsilon", Single.Epsilon , Single.Epsilon , 1)
    verifyParams("single: single.Epsilon ^ single.MaxValue", Single.Epsilon , Single.MaxValue, 0)
    verifyParams("single: single.MaxValue ^ single.Epsilon", Single.MaxValue, Single.Epsilon , 1)
    verifyParams("single: single.Epsilon ^ single.MinValue", Single.Epsilon , Single.MinValue, Infinity)
    verifyParams("single: single.MinValue ^ single.Epsilon", Single.MinValue, Single.Epsilon , NaN)

    verifyParams("single: Infinity ^ Infinity", Single.PositiveInfinity, Single.PositiveInfinity, Infinity)
    verifyParams("single: Infinity ^ -Infinity", Single.PositiveInfinity, Single.NegativeInfinity, +0)
    verifyParams("single: -Infinity ^ Infinity", Single.NegativeInfinity, Single.PositiveInfinity, Infinity)
    verifyParams("single: -Infinity ^ -Infinity", Single.NegativeInfinity, Single.NegativeInfinity, 0)

    verifyParams("single: Infinity ^ 0", Single.PositiveInfinity, 0, 1)
    verifyParams("single: 0 ^ Infinity", 0, Single.PositiveInfinity, 0)
    verifyParams("single: Infinity ^ Single.Epsilon", Single.PositiveInfinity, Single.Epsilon, Infinity)
    verifyParams("single: Single.Epsilon ^ Infinity", Single.Epsilon, Single.PositiveInfinity, 0)
    verifyParams("single: Infinity ^ 1", Single.PositiveInfinity, 1, Infinity)
    verifyParams("single: 1 ^ Infinity", 1, Single.PositiveInfinity, NaN)
    verifyParams("single: Infinity ^ Single.MaxValue", Single.PositiveInfinity, Single.MaxValue, Infinity)
    verifyParams("single: Single.MaxValue ^ Infinity", Single.MaxValue, Single.PositiveInfinity, Infinity)

    tstExp("single: 0 < n < 100", 1e-2, 1e2, 1)
    tstExp("single: -100 < n < 0", -1e-2, -1e2, 1)
    tstExp("single: 0 < n < double.max", 1e-40, Single.MaxValue, 10)
    tstExp("single: double.min < n < 0", -1e-40, Single.MinValue, 10)


    type = double;
    apInitScenario("pow of a "+ type);

    verifyParams("double: -0 ^ -0", -0, -0, 1, 0)
    verifyParams("double: -0 ^ +0", -0, +0, 1, 0)
    verifyParams("double: +0 ^ -0", +0, -0, 1, 0)
    verifyParams("double: +0 ^ +0", +0, +0, 1, 0)

    verifyParams("double: -1 ^ -1", -1, -1, -1, 0)
    verifyParams("double: -1 ^ +1", -1, +1, -1, 0)
    verifyParams("double: +1 ^ -1", +1, -1, 1, 0)
    verifyParams("double: +1 ^ +1", +1, +1, 1, 0)

    verifyParams("double: double.MinValue ^ double.MinValue", double.MinValue , double.MinValue , 0)
    verifyParams("double: double.MinValue ^ double.MaxValue", double.MinValue , double.MaxValue, Infinity)
    verifyParams("double: double.MaxValue ^ double.MinValue", double.MaxValue, double.MinValue , 0)
    verifyParams("double: double.MaxValue ^ double.MaxValue", double.MaxValue, double.MaxValue, Infinity)

    verifyParams("double: double.Epsilon ^ double.Epsilon", double.Epsilon , double.Epsilon , 1)
    verifyParams("double: double.Epsilon ^ double.MaxValue", double.Epsilon , double.MaxValue, 0)
    verifyParams("double: double.MaxValue ^ double.Epsilon", double.MaxValue, double.Epsilon , 1)
    verifyParams("double: double.Epsilon ^ double.MinValue", double.Epsilon , double.MinValue, Infinity)
    verifyParams("double: double.MinValue ^ double.Epsilon", double.MinValue, double.Epsilon , NaN)

    verifyParams("double: Infinity ^ Infinity", double.PositiveInfinity, double.PositiveInfinity, Infinity)
    verifyParams("double: Infinity ^ -Infinity", double.PositiveInfinity, double.NegativeInfinity, +0)
    verifyParams("double: -Infinity ^ Infinity", double.NegativeInfinity, double.PositiveInfinity, Infinity)
    verifyParams("double: -Infinity ^ -Infinity", double.NegativeInfinity, double.NegativeInfinity, 0)

    verifyParams("double: Infinity ^ 0", double.PositiveInfinity, 0, 1)
    verifyParams("double: 0 ^ Infinity", 0, double.PositiveInfinity, 0)
    verifyParams("double: Infinity ^ double.Epsilon", double.PositiveInfinity, double.Epsilon, Infinity)
    verifyParams("double: double.Epsilon ^ Infinity", double.Epsilon, double.PositiveInfinity, 0)
    verifyParams("double: Infinity ^ 1", double.PositiveInfinity, 1, Infinity)
    verifyParams("double: 1 ^ Infinity", 1, double.PositiveInfinity, NaN)
    verifyParams("double: Infinity ^ double.MaxValue", double.PositiveInfinity, double.MaxValue, Infinity)
    verifyParams("double: double.MaxValue ^ Infinity", double.MaxValue, double.PositiveInfinity, Infinity)

    tstExp("double: 0 < n < float.max", 1e-2, 1e2, 1)
    tstExp("double: float.min < n < 0", -1e-2, -1e2, 1)
    tstExp("double: 0 < n < float.max", 1e-40, double.MaxValue, 10)
    tstExp("double: float.min < n < 0", -1e-40, double.MinValue, 10)


    type = decimal;
    apInitScenario("pow of a decimal");

    verifyParams("decimal: 0 ^ 0", 0, 0, 1, 0)

    verifyParams("decimal: -1 ^ -1", -1, -1, -1, 0)
    verifyParams("decimal: -1 ^ +1", -1, +1, -1, 0)
    verifyParams("decimal: +1 ^ -1", +1, -1, 1, 0)
    verifyParams("decimal: +1 ^ +1", +1, +1, 1, 0)

    verifyParams("decimal: decimal.MinValue ^ decimal.MinValue", decimal.MinValue , decimal.MinValue , 0)
    verifyParams("decimal: decimal.MinValue ^ decimal.MaxValue", decimal.MinValue , decimal.MaxValue, Infinity)
    verifyParams("decimal: decimal.MaxValue ^ decimal.MinValue", decimal.MaxValue, decimal.MinValue , 0)
    verifyParams("decimal: decimal.MaxValue ^ decimal.MaxValue", decimal.MaxValue, decimal.MaxValue, Infinity)

    tstDec("decimal: 0 < n < 100", 1e-2, 1e2, 1)
    tstDec("decimal: 100 < n < 0", -1e-2, -1e2, 1)
    tstDec("decimal: 0 < n < decimal.max", 1e-40, decimal.MaxValue, 10)
    tstDec("decimal: decimal.min < n < 0", -1e-40, decimal.MinValue, 10)


    type = byte;
    apInitScenario("pow of a byte");

    verifyParams("byte: 0 ^ 0", byte.MinValue, byte.MinValue, 1, 0)
    verifyParams("byte: 0 ^ 1", byte.MinValue, 1, 0, 0)
    verifyParams("byte: 0 ^ byte.max", byte.MinValue, byte.MaxValue, 0, 0)

    verifyParams("byte: 1 ^ 0", 1, byte.MinValue, 1, 0)
    verifyParams("byte: 1 ^ 1", 1, 1, 1, 0)
    verifyParams("byte: 1 ^ byte.max", 1, byte.MaxValue, 1, 0)

    verifyParams("byte: byte.max ^ 0", byte.MaxValue, byte.MinValue, 1, 0)
    verifyParams("byte: byte.max ^ 1", byte.MaxValue, 1, 255, 0)
    verifyParams("byte: byte.max ^ byte.max", byte.MaxValue, byte.MaxValue, Infinity)

    tstRnd("byte: byte.min < x,y < 50", byte.MinValue+1, 50)
    tstRnd("byte: byte.min < x,y < byte.max", byte.MinValue+1, byte.MaxValue-1)


    type = ushort;
    apInitScenario("pow of a ushort");

    verifyParams("byte: 0 ^ 0", byte.MinValue, byte.MinValue, 1, 0)
    verifyParams("byte: 0 ^ 1", byte.MinValue, 1, 0, 0)
    verifyParams("byte: 0 ^ byte.max", byte.MinValue, byte.MaxValue, 0, 0)

    verifyParams("byte: 1 ^ 0", 1, byte.MinValue, 1, 0)
    verifyParams("byte: 1 ^ 1", 1, 1, 1, 0)
    verifyParams("byte: 1 ^ byte.max", 1, byte.MaxValue, 1, 0)

    verifyParams("byte: byte.max ^ 0", byte.MaxValue, byte.MinValue, 1, 0)
    verifyParams("byte: byte.max ^ 1", byte.MaxValue, 1, 255, 0)
    verifyParams("byte: byte.max ^ byte.max", byte.MaxValue, byte.MaxValue, Infinity)

    tstRnd("byte: byte.min < x,y < 50", byte.MinValue+1, 50)
    tstRnd("byte: byte.min < x,y < byte.max", byte.MinValue+1, byte.MaxValue-1)


    type = uint;
    apInitScenario("pow of a uint");

    verifyParams("byte: 0 ^ 0", byte.MinValue, byte.MinValue, 1, 0)
    verifyParams("byte: 0 ^ 1", byte.MinValue, 1, 0, 0)
    verifyParams("byte: 0 ^ byte.max", byte.MinValue, byte.MaxValue, 0, 0)

    verifyParams("byte: 1 ^ 0", 1, byte.MinValue, 1, 0)
    verifyParams("byte: 1 ^ 1", 1, 1, 1, 0)
    verifyParams("byte: 1 ^ byte.max", 1, byte.MaxValue, 1, 0)

    verifyParams("byte: byte.max ^ 0", byte.MaxValue, byte.MinValue, 1, 0)
    verifyParams("byte: byte.max ^ 1", byte.MaxValue, 1, 255, 0)
    verifyParams("byte: byte.max ^ byte.max", byte.MaxValue, byte.MaxValue, Infinity)

    tstRnd("byte: byte.min < x,y < 50", byte.MinValue+1, 50)
    tstRnd("byte: byte.min < x,y < byte.max", byte.MinValue+1, byte.MaxValue-1)


    type = ulong;
    apInitScenario("pow of a ulong");

    verifyParams("byte: 0 ^ 0", byte.MinValue, byte.MinValue, 1, 0)
    verifyParams("byte: 0 ^ 1", byte.MinValue, 1, 0, 0)
    verifyParams("byte: 0 ^ byte.max", byte.MinValue, byte.MaxValue, 0, 0)

    verifyParams("byte: 1 ^ 0", 1, byte.MinValue, 1, 0)
    verifyParams("byte: 1 ^ 1", 1, 1, 1, 0)
    verifyParams("byte: 1 ^ byte.max", 1, byte.MaxValue, 1, 0)

    verifyParams("byte: byte.max ^ 0", byte.MaxValue, byte.MinValue, 1, 0)
    verifyParams("byte: byte.max ^ 1", byte.MaxValue, 1, 255, 0)
    verifyParams("byte: byte.max ^ byte.max", byte.MaxValue, byte.MaxValue, Infinity)

    tstRnd("byte: byte.min < x,y < 50", byte.MinValue+1, 50)
    tstRnd("byte: byte.min < x,y < byte.max", byte.MinValue+1, byte.MaxValue-1)


    type = sbyte;
    apInitScenario("pow of a sbyte");

    verifyParams("sbyte: sbyte.min ^ sbyte.min", sbyte.MinValue, sbyte.MinValue, 1.8928834978668395375564025560288e-270)
    verifyParams("sbyte: sbyte.min ^ 0", sbyte.MinValue, 0, 1)
    verifyParams("sbyte: sbyte.min ^ 1", sbyte.MinValue, 1, -128, 0)
    verifyParams("sbyte: sbyte.min ^ sbyte.max", sbyte.MinValue, sbyte.MaxValue, -4.1273010244973847371276545696603e+267)

    verifyParams("sbyte: -1 ^ sbyte.min", -1, sbyte.MinValue, 1, 0)
    verifyParams("sbyte: -1 ^ 0", -1, 0, 1, 0)
    verifyParams("sbyte: -1 ^ 1", -1, 1, -1, 0)
    verifyParams("sbyte: -1 ^ sbyte.max", -1, sbyte.MaxValue, -1, 0)

    verifyParams("sbyte: 0 ^ sbyte.min", 0, sbyte.MinValue, Infinity)
    verifyParams("sbyte: 0 ^ 0", 0, 0, 1, 0)
    verifyParams("sbyte: 0 ^ 1", 0, 1, 0, 0)
    verifyParams("sbyte: 0 ^ sbyte.max", 0, sbyte.MaxValue, 0, 0)

    verifyParams("sbyte: 1 ^ sbyte.min", 1, sbyte.MinValue, 1, 0)
    verifyParams("sbyte: 1 ^ 0", 1, 0, 1, 0)
    verifyParams("sbyte: 1 ^ 1", 1, 1, 1, 0)
    verifyParams("sbyte: 1 ^ sbyte.max", 1, sbyte.MaxValue, 1, 0)

    verifyParams("sbyte: sbyte.max ^ sbyte.min", sbyte.MaxValue, sbyte.MinValue, 5.1656350195937981028449140855535e-270)
    verifyParams("sbyte: sbyte.max ^ 0", sbyte.MaxValue, 0, 1, 0)
    verifyParams("sbyte: sbyte.max ^ 1", sbyte.MaxValue, 1, 127, 0)
    verifyParams("sbyte: sbyte.max ^ sbyte.max", sbyte.MaxValue, sbyte.MaxValue, 1.5243074119957225753809349966704e+267)

    tstRnd("sbyte: sbyte.min < x,y < 50", sbyte.MinValue+1, 50)
    tstRnd("sbyte: sbyte.min < x,y < sbyte.max", sbyte.MinValue+1, sbyte.MaxValue-1)


    type = short;
    apInitScenario("pow of a short");

    verifyParams("short: short.min ^ short.min", short.MinValue, short.MinValue, 0)
    verifyParams("short: short.min ^ 0", short.MinValue, 0, 1)
    verifyParams("short: short.min ^ 1", short.MinValue, 1, -32768, 0)
    verifyParams("short: short.min ^ short.max", short.MinValue, short.MaxValue, -Infinity)

    verifyParams("short: -1 ^ short.min", -1, short.MinValue, 1, 0)
    verifyParams("short: -1 ^ 0", -1, 0, 1, 0)
    verifyParams("short: -1 ^ 1", -1, 1, -1, 0)
    verifyParams("short: -1 ^ short.max", -1, short.MaxValue, -1, 0)

    verifyParams("short: 0 ^ short.min", 0, short.MinValue, Infinity)
    verifyParams("short: 0 ^ 0", 0, 0, 1, 0)
    verifyParams("short: 0 ^ 1", 0, 1, 0, 0)
    verifyParams("short: 0 ^ short.max", 0, short.MaxValue, 0, 0)

    verifyParams("short: 1 ^ short.min", 1, short.MinValue, 1, 0)
    verifyParams("short: 1 ^ 0", 1, 0, 1, 0)
    verifyParams("short: 1 ^ 1", 1, 1, 1, 0)
    verifyParams("short: 1 ^ short.max", 1, short.MaxValue, 1, 0)

    verifyParams("short: short.max ^ short.min", short.MaxValue, short.MinValue, 0)
    verifyParams("short: short.max ^ 0", short.MaxValue, 0, 1, 0)
    verifyParams("short: short.max ^ 1", short.MaxValue, 1, 32767, 0)
    verifyParams("short: short.max ^ short.max", short.MaxValue, short.MaxValue, Infinity)

    tstRnd("short: short.min < x,y < 50", short.MinValue+1, 50)
    tstRnd("short: short.min < x,y < short.max", short.MinValue+1, short.MaxValue-1)


    type = int;
    apInitScenario("pow of a int");

    verifyParams("int: int.min ^ int.min", int.MinValue, int.MinValue, 0)
    verifyParams("int: int.min ^ 0", int.MinValue, 0, 1)
    verifyParams("int: int.min ^ 1", int.MinValue, 1, -2147483648, 0)
    verifyParams("int: int.min ^ int.max", int.MinValue, int.MaxValue, -Infinity)

    verifyParams("int: -1 ^ int.min", -1, int.MinValue, 1, 0)
    verifyParams("int: -1 ^ 0", -1, 0, 1, 0)
    verifyParams("int: -1 ^ 1", -1, 1, -1, 0)
    verifyParams("int: -1 ^ int.max", -1, int.MaxValue, -1, 0)

    verifyParams("int: 0 ^ int.min", 0, int.MinValue, Infinity)
    verifyParams("int: 0 ^ 0", 0, 0, 1, 0)
    verifyParams("int: 0 ^ 1", 0, 1, 0, 0)
    verifyParams("int: 0 ^ int.max", 0, int.MaxValue, 0, 0)

    verifyParams("int: 1 ^ int.min", 1, int.MinValue, 1, 0)
    verifyParams("int: 1 ^ 0", 1, 0, 1, 0)
    verifyParams("int: 1 ^ 1", 1, 1, 1, 0)
    verifyParams("int: 1 ^ int.max", 1, int.MaxValue, 1, 0)

    verifyParams("int: int.max ^ int.min", int.MaxValue, int.MinValue, 0)
    verifyParams("int: int.max ^ 0", int.MaxValue, 0, 1, 0)
    verifyParams("int: int.max ^ 1", int.MaxValue, 1, 2147483647, 0)
    verifyParams("int: int.max ^ int.max", int.MaxValue, int.MaxValue, Infinity)

    tstRnd("int: int.min < x,y < 50", int.MinValue+1, 50)
    tstRnd("int: int.min < x,y < int.max", int.MinValue+1, int.MaxValue-1)


    type = long;
    apInitScenario("pow of a long");

    verifyParams("long: long.min ^ long.min", long.MinValue, long.MinValue, 0)
    verifyParams("long: long.min ^ 0", long.MinValue, 0, 1)
    verifyParams("long: long.min ^ 1", long.MinValue, 1, -9223372036854775808, 0)
    verifyParams("long: long.min ^ long.max", long.MinValue, long.MaxValue, Infinity)

    verifyParams("long: -1 ^ long.min", -1, long.MinValue, 1, 0)
    verifyParams("long: -1 ^ 0", -1, 0, 1, 0)
    verifyParams("long: -1 ^ 1", -1, 1, -1, 0)
    verifyParams("long: -1 ^ long.max", -1, long.MaxValue, -1, 0)

    verifyParams("long: 0 ^ long.min", 0, long.MinValue, Infinity)
    verifyParams("long: 0 ^ 0", 0, 0, 1, 0)
    verifyParams("long: 0 ^ 1", 0, 1, 0, 0)
    verifyParams("long: 0 ^ long.max", 0, long.MaxValue, 0, 0)

    verifyParams("long: 1 ^ long.min", 1, long.MinValue, 1, 0)
    verifyParams("long: 1 ^ 0", 1, 0, 1, 0)
    verifyParams("long: 1 ^ 1", 1, 1, 1, 0)
    verifyParams("long: 1 ^ long.max", 1, long.MaxValue, 1, 0)

    verifyParams("long: long.max ^ long.min", long.MaxValue, long.MinValue, 0)
    verifyParams("long: long.max ^ 0", long.MaxValue, 0, 1, 0)
    verifyParams("long: long.max ^ 1", long.MaxValue, 1, 9223372036854775807, 0)
    verifyParams("long: long.max ^ long.max", long.MaxValue, long.MaxValue, Infinity)

    tstRnd("long: long.min < x,y < 50", long.MinValue+1, 50)
    tstRnd("long: long.min < x,y < long.max", long.MinValue+1, long.MaxValue-1)


    type = String;
    apInitScenario("pow of a String");
    verifyParams("string: long.min ^ long.min", long.MinValue, long.MinValue, 0)
    verifyParams("string: long.min ^ 0", long.MinValue, 0, 1)
    verifyParams("string: long.min ^ 1", long.MaxValue, 1, -9223372036854775808, 0)
    verifyParams("string: long.min ^ long.max", long.MaxValue, long.MaxValue, Infinity)

    verifyParams("string: -1 ^ long.min", -1, long.MinValue, 1, 0)
    verifyParams("string: -1 ^ 0", -1, 0, 1, 0)
    verifyParams("string: -1 ^ 1", -1, 1, -1, 0)
    verifyParams("string: -1 ^ long.max", -1, long.MaxValue, -1, 0)

    verifyParams("string: 0 ^ long.min", 0, long.MinValue, Infinity)
    verifyParams("string: 0 ^ 0", 0, 0, 1, 0)
    verifyParams("string: 0 ^ 1", 0, 1, 0, 0)
    verifyParams("string: 0 ^ long.max", 0, long.MaxValue, 0, 0)

    verifyParams("string: 1 ^ long.min", 1, long.MinValue, 1, 0)
    verifyParams("string: 1 ^ 0", 1, 0, 1, 0)
    verifyParams("string: 1 ^ 1", 1, 1, 1, 0)
    verifyParams("string: 1 ^ long.max", 1, long.MaxValue, 1, 0)

    verifyParams("string: long.max ^ long.min", long.MaxValue, long.MinValue, 0)
    verifyParams("string: long.max ^ 0", long.MaxValue, 0, 1, 0)
    verifyParams("string: long.max ^ 1", long.MaxValue, 1, 9223372036854775807, 0)
    verifyParams("string: long.max ^ long.max", long.MaxValue, long.MaxValue, Infinity)

    tstRnd("long: long.min < x,y < 50", long.MinValue+1, 50)
    tstRnd("long: long.min < x,y < long.max", long.MinValue+1, long.MaxValue-1)


    type = Date;
    apInitScenario("pow of a Date");
    verifyParams("Date: 4 ^ 4", 4, 4, NaN)


    type = Array;
    apInitScenario("pow of a Array");
    var arrvar:Array = new Array(1)
    arrvar[0] = 1.5
    verifyParams("Array: 1.5 ^ 2", arrvar, 2, NaN)
    verifyParams("Array: 1.5 ^ 2", arrvar, arrvar, 1.8371173070873836)


    apEndTest();

}


powh001();


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
