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


var iTestID = 191767;

/////////////////////////////////////////////////////////////////////////////////

@if(!@aspx)
	import System
@end

function break003() {

    apInitTest("break003 ");

    var booleanTyped   : Boolean = Boolean(true);
    var booleanObj     : Object  = Boolean(true);

    var sbyteTyped     : SByte   = SByte(-50);
    var sbyteObj       : Object  = SByte(-50);
    var byteTyped      : Byte    = Byte(50);
    var byteObj        : Object  = Byte(50);

    var int16Typed     : Int16   = Int16(-50);
    var int16Obj       : Object  = Int16(-50);
    var uint16Typed    : UInt16  = UInt16(50);
    var uint16Obj      : Object  = UInt16(50);

    var int32Typed     : Int32   = Int32(-50);
    var int32Obj       : Object  = Int32(-50);
    var uint32Typed    : UInt32  = UInt32(50);
    var uint32Obj      : Object  = UInt32(50);

    var int64Typed     : Int64   = Int64(-50);
    var int64Obj       : Object  = Int64(-50);
    var uint64Typed    : UInt64  = UInt64(50);
    var uint64Obj      : Object  = UInt64(50);

    var singleTyped    : Single  = Single(-50);
    var singleObj      : Object  = Single(-50);

    var doubleTyped    : Double  = Double(-50);
    var doubleObj      : Object  = Double(-50);

    var numberTyped    : Number  = Number(-50);
    var numberObj      : Object  = Number(-50);

    var numobjTyped    : Number  = new Number(-50);
    var numobjObj      : Object  = new Number(-50);

    var strTyped       : String  = String("2");
    var strObj         : Object  = String("2");

    var charTyped      : Char    = Char("\u0032");
    var charObj        : Object  = Char("\u0032");


    //----------------------------------------------------------------------------
    apInitScenario("1. verify break nesting");

    var i:int = 0;

    for (booleanTyped = false; booleanTyped == false; ){
        for (booleanObj = false; booleanObj == false; ){

            for (sbyteTyped = 1; sbyteTyped>-20;sbyteTyped--){
                for (byteTyped = 0; byteTyped<20; byteTyped++){
                    for (sbyteObj = 1; sbyteObj>-20;sbyteObj--){
                        for (byteObj = 0; byteObj<20; byteObj++){

                            for (int64Typed = 1; int64Typed>-20;int64Typed--){
                                for (uint64Typed = 0; uint64Typed<20; uint64Typed++){
                                    for (int64Obj = 1; int64Obj>-20;int64Obj--){
                                        for (uint64Obj = 0; uint64Obj<20; uint64Obj++){
  
                                            for (numberTyped = 1; numberTyped>-20;numberTyped--){
                                                for (doubleObj = 0; doubleObj<20; doubleObj++){
                                                    for (charTyped = 0; charTyped<20;charTyped++){
                                                        for (strTyped = 0; strTyped<20; strTyped++){
                                                             
                                                             i++
                                                             
                                                            if (strTyped == 2) break
                                                        }				
                                                        if (charTyped == 2) break
                                                    }				
                                                    if (doubleObj == 2) break
                                                }				
                                                if (numberTyped == -1) break
                                            }				
  
                                            if (uint64Obj == 2) break
                                        }				
                                        if (int64Obj == -1) break
                                    }				
                                    if (uint64Typed == 2) break
                                }				
                                if (int64Typed == -1) break
                            }

                            if (byteObj == 2) break
                        }				
                        if (sbyteObj == -1) break
                    }				
                    if (byteTyped == 2) break
                }				
                if (sbyteTyped == -1) break
            }
  
            if (booleanObj == false) break
        }
        if (booleanTyped == false) break;
    }



    if (i != 531441) apLogFailInfo("wrong value calculated", 531441, i,"")

    apEndTest();

}


break003();


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
