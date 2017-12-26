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


var iTestID = 181182;

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
    
    if ((actual - expected) <= delta && (expected - actual) <= delta)
        return false
    return true
}


function retvar(val){return val}
function rettyped1(val){return type(val)}
function rettyped2(val){
	return eval("   var ret:type = val; ret")
}

function verifyParams(errstr, input, expected, ... params:Object[]){
    var delta = .00005
    if (params.length>0) delta = params[0]
    if (input!=0 && expected!=0) 
        delta = (Math.abs(input)+Math.abs(expected))*delta


    if (typeof(func(input)) !== typeof(expected))
        apLogFailInfo(errstr + ": type failure", typeof(expected), typeof(func(input)), "");


    if (diff( func(type(input)), expected, delta))
        apLogFailInfo(errstr + ": immediate failure", expected, func(type(input)), "");


    //eval("	var insttype:type=input; if (diff( func(insttype), expected, delta)) apLogFailInfo(errstr + ': typed instance failure', expected, func(insttype), ''); ")


    var instob:Object = type(input)
    if (diff( func(instob), expected, delta))
        apLogFailInfo(errstr + ": object instance failure", expected, func(instob), "");


    var memob = new Object
    memob.mem = type(input)
    if (diff( func(memob.mem), expected, delta))
        apLogFailInfo(errstr + ": object member failure", expected, func(memob.mem), "");


    if (diff( func(  retvar(type(input))  ), expected, delta))
        apLogFailInfo(errstr + ": function return failure", expected, func(retvar(type(input))), "");


    if (diff( func(  rettyped1(input)  ), expected, delta))
        apLogFailInfo(errstr + ": function typed return failure 1", expected, func(  rettyped1(input)  ), "");


    //if (diff( func(  rettyped2(input)  ), expected, delta))
    //    apLogFailInfo(errstr + ": function typed return failure 2", expected, func(  rettyped2(input)  ), "");

}


function tstRnd(errstr, min, max, ... info:Object[]){
    var i:int;
	var vNum:Number, rounds = 20

    if (info.length>0) rounds = info[0];
    for (i=0; i<rounds; i++)
    {
        vNum = Math.floor(Math.random()*(max+1-min))+min; // gives int num between min and max
        verifyParams(errstr,vNum,calcExpected(vNum));
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
		    verifyParams(errstr,sign*vNum1,calcExpected(sign*vNum1));

        vNum2 = (Math.random()*Math.pow(2,res))/Math.pow(2,iPow);
	    if (vNum2>min)
		    verifyParams(errstr,sign*vNum2,calcExpected(sign*vNum2));

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
                verifyParams(errstr,sign*vNum1,calcExpected(sign*vNum1), .0000000001);
        }catch(e){
            vNum1 = decimal.MaxValue
        }
        try{
            vNum2 = (Math.random()*Math.pow(2,res))/Math.pow(2,iPow);
            if (vNum2>min)
                verifyParams(errstr,sign*vNum2,calcExpected(sign*vNum2), .0000000001);
        }catch(e){
            vNum1 = decimal.MaxValue
        }

        if ((sign==1 && vNum1>=max && vNum2<=min) || (sign==-1 && vNum1>=max && vNum2<=min)) break
    }    
}


function calcExpected(input){
    var ans = 0
    
    if (input<0) return NaN
    if (input==0) return -Infinity

    if (input>=1){
        while (input>1e10){
            input /= 1e10
            ans += 23.025850929940456840179914546844
        }
        while (input>10){
            input /= 10
            ans += 2.3025850929940456840179914546844
        }
        while (input>2){
            input /= 2
            ans += 0.69314718055994530941723212145818
        }
        ans += Math.log(input)
    }
    else{
        while (input<1e-10){
            input *= 1e10
            ans -= 23.025850929940456840179914546844
        }
        while (input<1e-1){
            input *= 1e1
            ans -= 2.3025850929940456840179914546844
        }
        while (input<-.5){
            input *= 2
            ans -= 0.69314718055994530941723212145818
        }
        ans += Math.log(input)
    }
    return ans
}

function logh002() {


    var i
    func = System.Math.Log


    apInitTest("log002 ");

    type = boolean
    apInitScenario("log of a boolean");

    verifyParams("boolean: n = true", true, 0, 0)
    verifyParams("boolean: n = false", false, -Infinity, 0)


    type = char;
    apInitScenario("log of a char");

    verifyParams("char: n = 0", char.MinValue, -Infinity, 0)
    tstRnd("char: byte.min < n < byte.max", byte.MinValue+1, byte.MaxValue-1)
    verifyParams("char: n = byte.max", byte.MaxValue, 5.5412635451584261462455391880218)
    tstRnd("char: byte.max < n < char.max", byte.MaxValue+1, char.MaxValue-1)
    verifyParams("char: n = char.max", char.MaxValue, 11.090339630053645944597333502783)


    type = Single;
    apInitScenario("log of a Single");

    verifyParams("single: n = -0", -0, -Infinity, 0)
    verifyParams("single: n = +0", +0, -Infinity, 0)
    tstExp("single: 0 < n < float.max", 1e-40, Single.MaxValue, 10)
    tstExp("single: float.min < n < 0", -1e-40, Single.MinValue, 10)
    verifyParams("single: n = float.max", Single.MaxValue, 88.72283905206835)
    verifyParams("single: n = float.min", Single.MinValue, NaN)
    verifyParams("single: n = Infinity", Single.PositiveInfinity, Infinity)
    verifyParams("single: n = -Infinity", Single.NegativeInfinity, NaN)
    verifyParams("single: n = NaN", NaN, NaN)


    type = double;
    apInitScenario("log of a "+ type);

    verifyParams("double: n = -0", -0, -Infinity)
    verifyParams("double: n = +0", +0, -Infinity)
    tstExp("double: 0 < n < double.max", 1e-40, double.MaxValue, 10)
    tstExp("double: double.min < n < 0", -1e-40, double.MinValue, 10)
    verifyParams("double: n = double.max", double.MaxValue, 709.782712893384)
    verifyParams("double: n = double.min", double.MinValue, NaN)
    verifyParams("double: n = Infinity", double.PositiveInfinity, Infinity)
    verifyParams("double: n = -Infinity", double.NegativeInfinity, NaN)
    verifyParams("double: n = NaN", NaN, NaN)


    type = decimal;
    apInitScenario("log of a decimal");

    verifyParams("decimal: n = 0", decimal.Zero, -Infinity, 0)
    verifyParams("decimal: n = 1", decimal.One, 0)
    verifyParams("decimal: n = -1", decimal.MinusOne, NaN)
    tstDec("decimal: 0 < n < decimal.max", 1e-40, decimal.MaxValue, 10)
    tstDec("decimal: decimal.min < n < 0", -1e-40, decimal.MinValue, 10)
    //apLogFailInfo("bug: 277604", "", "", "");    
    verifyParams("decimal: n = decimal.max", decimal.MaxValue, 66.54212933375474)	
    verifyParams("decimal: n = decimal.min", decimal.MinValue, NaN)


    type = byte;
    apInitScenario("log of a byte");
    verifyParams("byte: n = 0", byte.MinValue, -Infinity)
    tstRnd("byte: byte.min < n < byte.max", byte.MinValue+1, byte.MaxValue-1)
    verifyParams("byte: n = byte.max", byte.MaxValue, 5.5412635451584261462455391880218)


    type = ushort;
    apInitScenario("log of a ushort");
    verifyParams("ushort: n = ushort.min", ushort.MinValue, -Infinity)
    tstRnd("ushort: ushort.min < n < ushort.max", ushort.MinValue+1, ushort.MaxValue-1)
    verifyParams("ushort: n = ushort.max", ushort.MaxValue, 11.090339630053645944597333502783)


    type = uint;
    apInitScenario("log of a uint");
    verifyParams("uint: n = uint.min", uint.MinValue, -Infinity)
    tstRnd("uint: uint.min < n < uint.max", uint.MinValue+1, uint.MaxValue-1)
    verifyParams("uint: n = uint.max", uint.MaxValue, 22.180709777685419257670453203439)


    type = ulong;
    apInitScenario("log of a ulong");
    verifyParams("ulong: n = ulong.min", ulong.MinValue, -Infinity)
    tstRnd("ulong: ulong.min < n < ulong.max", ulong.MinValue+1, ulong.MaxValue-1)
    verifyParams("ulong: n = ulong.max", ulong.MaxValue, 44.361419555836499802648645664699)


    type = sbyte;
    apInitScenario("log of a sbyte");
    verifyParams("sbyte: n = sbyte.min", sbyte.MinValue, NaN)
    verifyParams("sbyte: n = 0", 0, -Infinity)
    tstRnd("sbyte: sbyte.min < n < sbyte.max", sbyte.MinValue+1, sbyte.MaxValue-1)
    verifyParams("sbyte: n = sbyte.max", sbyte.MaxValue, 4.8441870864585912730474408077163)


    type = short;
    apInitScenario("log of a short");
    verifyParams("short: n = short.min", short.MinValue, NaN)
    verifyParams("short: n = 0", 0, -Infinity)
    tstRnd("short: short.min < n < short.max", short.MinValue+1, short.MaxValue-1)
    verifyParams("short: n = short.max", short.MaxValue, 10.39717719035538387983075336747)


    type = int;
    apInitScenario("log of a int");
    verifyParams("int: n = int.min", int.MinValue, NaN)
    verifyParams("int: n = 0", 0, -Infinity)
    tstRnd("int: int.min < n < int.max", int.MinValue+1, int.MaxValue-1)
    verifyParams("int: n = int.max", int.MaxValue, 21.487562596892643304518036290109)


    type = long;
    apInitScenario("log of a long");
    verifyParams("long: n = long.min", long.MinValue, NaN)
    verifyParams("long: n = 0", 0, -Infinity)
    tstRnd("long: long.min < n < long.max", long.MinValue+1, long.MaxValue-1)
    verifyParams("long: n = long.max", long.MaxValue, 43.668272375276554493177203434617)


    type = String;
    apInitScenario("log of a String");
    verifyParams("String: n = long.min", long.MinValue, NaN)
    verifyParams("String: n = 0", 0, -Infinity)
    tstRnd("String: long.min < n < long.max", long.MinValue+1, long.MaxValue-1)
    verifyParams("String: n = long.max", long.MaxValue, 43.668272375276554493177203434617)


    type = Date;
    apInitScenario("log of a Date");
    verifyParams("Date: n = -int.max", -int.MaxValue, NaN)


    type = Array;
    apInitScenario("log of a Array");
    var arrvar:Array = new Array(1)
    arrvar[0] = 12.34
    verifyParams("Array: n = 12.34", arrvar, 2.5128460184772417553788208906996)


    apEndTest();

}


logh002();


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
