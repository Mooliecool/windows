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


var iTestID = 193318;

///////////////////////////////////////////////////////////////////////////////////
//import System

class go{

function start(){

    var booleanTyped   : boolean = boolean(true);
    var booleanObj     : Object  = boolean(true);

    var sbyteTyped     : sbyte   = sbyte(-50);
    var sbyteObj       : Object  = sbyte(-50);
    var byteTyped      : byte    = byte(50);
    var byteObj        : Object  = byte(50);

    var int16Typed     : short   = short(-50);
    var int16Obj       : Object  = short(-50);
    var uint16Typed    : ushort  = ushort(50);
    var uint16Obj      : Object  = ushort(50);

    var int32Typed     : int     = int(-50);
    var int32Obj       : Object  = int(-50);
    var uint32Typed    : uint    = uint(50);
    var uint32Obj      : Object  = uint(50);

    var int64Typed     : long    = long(-50);
    var int64Obj       : Object  = long(-50);
    var uint64Typed    : ulong   = ulong(50);
    var uint64Obj      : Object  = ulong(50);

    var singleTyped    : float   = float(-50);
    var singleObj      : Object  = float(-50);

    var doubleTyped    : double  = double(-50);
    var doubleObj      : Object  = double(-50);

    var numberTyped    : Number  = Number(-50);
    var numberObj      : Object  = Number(-50);

    var numobjTyped    : Number  = new Number(-50);
    var numobjObj      : Object  = new Number(-50);

    var strTyped       : String  = String("2");
    var strObj         : Object  = String("2");

    var charTyped      : char    = char("\u0032");
    var charObj        : Object  = char("\u0032");

    var DateTyped      : Date    = new Date(-50)
    var DateObj        : Object  = new Date(-50)

    var decimalTyped   : decimal = decimal(-50)
    var decimalObj     : Object  = decimal(-50)

    var count:long



////////////////////////////////////////////////////////////////////////////////////
    apInitScenario("do loop with a hard typed boolean")

    count = 0;
    booleanTyped = true
    do{
        count++
        if (count >9) booleanTyped = false
    }while(booleanTyped)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    booleanTyped = false
    do{
        count++
        if (count >9) booleanTyped = true
    }while(!booleanTyped)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")




    apInitScenario("do loop with a hard typed Object/boolean")

    count = 0;
    booleanObj = true
    do{
        count++
        if (count >9) booleanObj = false
    }while(booleanObj)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    booleanObj = false
    do{
        count++
        if (count >9) booleanObj = true
    }while(!booleanObj)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")






////////////////////////////////////////////////////////////////////////////////////
    apInitScenario("do loop with a hard typed sbyte")
    count = 0;
    sbyteTyped = -119
    do{
        count++
        if (sbyteTyped > sbyte.MinValue) sbyteTyped--
        else sbyteTyped = 0
    }while(sbyteTyped)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    sbyteTyped = 118
    do{
        count++
        if (sbyteTyped < sbyte.MaxValue) sbyteTyped++
        else sbyteTyped = 0
    }while(sbyteTyped)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")


    apInitScenario("do loop with a hard typed Object/sbyte")
    count = 0;
    sbyteObj = -119
    do{
        count++
        if (sbyteObj > sbyte.MinValue) sbyteObj--
        else sbyteObj = 0
    }while(sbyteObj)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    sbyteObj = 118
    do{
        count++
        if (sbyteObj < sbyte.MaxValue) sbyteObj++
        else sbyteObj = 0
    }while(sbyteObj)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")



    apInitScenario("do loop with a hard typed byte")
    count = 0;
    byteTyped = 246
    do{
        count++
        if (byteTyped < byte.MaxValue) byteTyped++
        else byteTyped = 0
    }while(byteTyped)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    apInitScenario("do loop with a hard typed Object/byte")
    count = 0;
    byteObj = 246
    do{
        count++
        if (byteObj < byte.MaxValue) byteObj++
        else byteObj = 0
    }while(byteObj)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")



////////////////////////////////////////////////////////////////////////////////////
    apInitScenario("do loop with a hard typed Int16")
    count = 0;
    int16Typed = -32749
    do{
        count++
        if (int16Typed > short.MinValue) int16Typed--
        else int16Typed = 0
    }while(int16Typed)
    if (count != 20) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    int16Typed = 32748
    do{
        count++
        if (int16Typed < short.MaxValue) int16Typed++
        else int16Typed = 0
    }while(int16Typed)
    if (count != 20) apLogFailInfo("Looped wrong number of times", 10, count, "")


    apInitScenario("do loop with a hard typed Object/Int16")
    count = 0;
    int16Obj = -32749
    do{
        count++
        if (int16Obj > short.MinValue) int16Obj--
        else int16Obj = 0
    }while(int16Obj)
    if (count != 20) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    int16Obj = 32748
    do{
        count++
        if (int16Obj < short.MaxValue) int16Obj++
        else int16Obj = 0
    }while(int16Obj)
    if (count != 20) apLogFailInfo("Looped wrong number of times", 10, count, "")




    apInitScenario("do loop with a hard typed UInt16")
    count = 0;
    uint16Typed = 65516
    do{
        count++
        if (uint16Typed < ushort.MaxValue) uint16Typed++
        else uint16Typed = 0
    }while(uint16Typed)
    if (count != 20) apLogFailInfo("Looped wrong number of times", 10, count, "")


    apInitScenario("do loop with a hard typed Object/UInt16")
    count = 0;
    uint16Obj = 65516
    do{
        count++
        if (uint16Obj < ushort.MaxValue) uint16Obj++
        else uint16Obj = 0
    }while(uint16Obj)
    if (count != 20) apLogFailInfo("Looped wrong number of times", 10, count, "")


////////////////////////////////////////////////////////////////////////////////////

    apInitScenario("do loop with a hard typed Int32")
    count = 0;
    int32Typed = -2147483639
    do{
        count++
        if (int32Typed > int.MinValue) int32Typed--
        else int32Typed = 0
    }while(int32Typed)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    int32Typed = 2147483638
    do{
        count++
        if (int32Typed < int.MaxValue) int32Typed++
        else int32Typed = 0
    }while(int32Typed)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    apInitScenario("do loop with a hard typed Object/Int32")
    count = 0;
    int32Obj = -2147483639
    do{
        count++
        if (int32Obj > int.MinValue) int32Obj--
        else int32Obj = 0
    }while(int32Obj)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    int32Obj = 2147483638
    do{
        count++
        if (int32Obj < int.MaxValue) int32Obj++
        else int32Obj = 0
    }while(int32Obj)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")




    apInitScenario("do loop with a hard typed UInt32")
    count = 0;
    uint32Typed = 4294967286
    do{
        count++
        if (uint32Typed < uint.MaxValue) uint32Typed++
        else uint32Typed = 0
    }while(uint32Typed)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")


    apInitScenario("do loop with a hard typed Object/UInt32")
    count = 0;
    uint32Obj = 4294967286
    do{
        count++
        if (uint32Obj < uint.MaxValue) uint32Obj++
        else uint32Obj = 0
    }while(uint32Obj)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")



////////////////////////////////////////////////////////////////////////////////////

    apInitScenario("do loop with a hard typed Int64")
    count = 0;
    int64Typed = -9223372036854775799
    do{
        count++
        if (int64Typed > long.MinValue) int64Typed--
        else int64Typed = 0
    }while(int64Typed)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    int64Typed = 9223372036854775798
    do{
        count++
        if (int64Typed < long.MaxValue) int64Typed++
        else int64Typed = 0
    }while(int64Typed)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")


    apInitScenario("do loop with a hard typed Object/Int64")
    count = 0;
    int64Obj = -9223372036854775799
    do{
        count++
        if (int64Obj > long.MinValue) int64Obj--
        else int64Obj = 0
    }while(int64Obj)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")
    
    count = 0;
    int64Obj = 9223372036854775798
    do{
        count++
        if (int64Obj < long.MaxValue) int64Obj++
        else int64Obj = 0
    }while(int64Obj)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")




    apInitScenario("do loop with a hard typed UInt64")
    count = 0;
    uint64Typed = 18446744073709551606
    do{
        count++
        if (uint64Typed < ulong.MaxValue) uint64Typed++
        else uint64Typed = 0
    }while(uint64Typed)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    apEndTest();

    apInitScenario("do loop with a hard typed Object/UInt64")
    count = 0;
    uint64Obj = 18446744073709551606
    do{
        count++
        if (uint64Obj < ulong.MaxValue) uint64Obj++
        else uint64Obj = 0
    }while(uint64Obj)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

////////////////////////////////////////////////////////////////////////////////////
    apInitScenario("do loop with a hard typed single")
    count = 0;
    do{
        if (++count == 10) break;
    }while(float.NegativeInfinity)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    count = 0;
    do{
        if (++count == 10) break;
    }while(float.PositiveInfinity)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    count = 0;
    singleTyped = float.MinValue
    do{
        count++
        singleTyped /= 10
    }while(singleTyped < -float.Epsilon)
    if (count != 84) apLogFailInfo("Looped wrong number of times", 10, count, "")


    apInitScenario("do loop with a hard typed Object/single")
    count = 0;
    do{
        if (++count == 10) break;
    }while(Object(float.NegativeInfinity))
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    count = 0;
    do{
        if (++count == 10) break;
    }while(Object(float.PositiveInfinity))
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    count = 0;
    singleObj = float.MinValue
    do{
        count++
        singleObj /= 10
    }while(singleObj < -float.Epsilon)
    if (count != 84) apLogFailInfo("Looped wrong number of times", 10, count, "")


////////////////////////////////////////////////////////////////////////////////////


    apInitScenario("do loop with a hard typed Double")
    count = 0;
    do{
        if (++count == 10) break;
    }while(double.NegativeInfinity)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    count = 0;
    do{
        if (++count == 10) break;
    }while(double.PositiveInfinity)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    count = 0;
    doubleTyped = double.MinValue
    do{
        count++
        doubleTyped /= 100
    }while(doubleTyped)
    if (count != 316) apLogFailInfo("Looped wrong number of times", 10, count, "")



    apInitScenario("do loop with a hard typed Object/Double")
    count = 0;
    do{
        if (++count == 10) break;
    }while(Object(double.NegativeInfinity))
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    count = 0;
    do{
        if (++count == 10) break;
    }while(Object(double.PositiveInfinity))
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")


    count = 0;
    doubleObj = double.MinValue
    do{
        count++
        doubleObj /= 100
    }while(doubleObj)
    if (count != 316) apLogFailInfo("Looped wrong number of times", 10, count, "")


////////////////////////////////////////////////////////////////////////////////////



    apInitScenario("do loop with a hard typed Number")

    count = 0;
    numberTyped = double.MinValue
    do{
        count++
        numberTyped /= 100
    }while(numberTyped)
    if (count != 316) apLogFailInfo("Looped wrong number of times", 10, count, "")

    count = 0;
    numberTyped = -5
    do{
        count++
        numberTyped += .5
        if (numberTyped >10) break
    }while(numberTyped)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")



    apInitScenario("do loop with a hard typed Object/Number")

    count = 0;
    numberObj = double.MinValue
    do{
        count++
        numberObj /= 100
    }while(numberObj)
    if (count != 316) apLogFailInfo("Looped wrong number of times", 10, count, "")


    count = 0;
    numberTyped = 5
    do{
        count++
        numberTyped -= .5
        if (numberTyped >10) break
    }while(numberTyped)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")


////////////////////////////////////////////////////////////////////////////////////



    apInitScenario("do loop with a hard typed Decimal")

    count = 0;
    decimalTyped = decimal.MinValue
    do{
        count++
        decimalTyped /= 100
    }while(decimalTyped)
    if (count != 29) apLogFailInfo("Looped wrong number of times", 10, count, "")

    count = 0;
    decimalTyped = -5
    do{
        count++
        decimalTyped += .5
        if (decimalTyped >10) break
    }while(decimalTyped)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")



    apInitScenario("do loop with a hard typed Object/Decimal")

    count = 0;
    decimalObj = decimal.MinValue
    do{
        count++
        decimalObj /= 100
    }while(decimalObj)
    if (count != 29) apLogFailInfo("Looped wrong number of times", 10, count, "")


    count = 0;
    numberTyped = 5
    do{
        count++
        numberTyped -= .5
        if (numberTyped >10) break
    }while(numberTyped)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")


////////////////////////////////////////////////////////////////////////////////////

    apInitScenario("do loop with a hard typed String")

    count = 0;
    strTyped = null
    do{
        count++
	if (count>10) break;
    }while(strTyped)
    if (count != 1) apLogFailInfo("Looped wrong number of times", 1, count, "")

    count = 0;
    strTyped = ""
    do{
        count++
	if (count>10) break;
    }while(strTyped)
    if (count != 1) apLogFailInfo("Looped wrong number of times", 1, count, "")

    count = 0;
    strTyped = "-5"
    do{
        count++
        strTyped++
	if (strTyped == "5") break;
    }while(strTyped)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")



    apInitScenario("do loop with a hard typed Object\String")

    count = 0;
    strObj = String(null)
    do{
        count++
	if (count>9) break;
    }while(strObj)
    if (count != 10) apLogFailInfo("Looped wrong number of times", 10, count, "")

    count = 0;
    strObj = String("")
    do{
        count++
	if (count>10) break;
    }while(strObj)
    if (count != 1) apLogFailInfo("Looped wrong number of times", 1, count, "292755")


////////////////////////////////////////////////////////////////////////////////////





    apInitScenario("do loop with a hard typed char")
    count = 0;
    charTyped = 65516
    do{
        count++
        if (charTyped < char.MaxValue) charTyped++
        else charTyped = 0
    }while(charTyped)
    if (count != 20) apLogFailInfo("Looped wrong number of times", 20, count, "")


    apInitScenario("do loop with a hard typed Object/char")
    count = 0;
    charObj = 65516
    do{
        count++
        if (charObj < char.MaxValue) charObj++
        else charObj = 0
    }while(charObj)
    if (count != 20) apLogFailInfo("Looped wrong number of times", 20, count, "")


}
}


function do201() {
    var x = new go

    apInitTest("do201 ");
    x.start()
    apEndTest();


}


do201();


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
