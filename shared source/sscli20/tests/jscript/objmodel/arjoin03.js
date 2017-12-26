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


var iTestID = 51732;


function verify(vAct, vExp, bugNum)
{
    if (null == bugNum) bugNum = "";

    if (vAct != vExp)
        apLogFailInfo("", vExp, vAct, bugNum);
}

@if(@aspx) expando @end function udObject () { /* nothing */ }

function arjoin03(){
@if(@_fast)
    var ob;
@end
    apInitTest("arJoin03 ");

    
    //----------------------------------------------------------------------------
    apInitScenario("test arg conversion: pos integer");

    verify((new Array(1,2)).join(), "1,2", null);

    
    //----------------------------------------------------------------------------
    apInitScenario("test arg conversion: neg integer");

    verify((new Array(-1,-2)).join(), "-1,-2", null);

    
    //----------------------------------------------------------------------------
    apInitScenario("test arg conversion: real");

    verify((new Array(1.2,3.4)).join(), "1.2,3.4", null);

    
    //----------------------------------------------------------------------------
    apInitScenario("test arg conversion: bool");

    verify((new Array(false,true)).join(), "false,true", null);

    
    //----------------------------------------------------------------------------
    apInitScenario("test arg conversion: objects");

    ob = new Array();
    verify((new Array(ob)).join(), ob.toString(), null);

    ob = new Boolean();
    verify((new Array(ob)).join(), ob.toString(), null);

    ob = new Date();
    verify((new Array(ob)).join(), ob.toString(), null);

    ob = new Number();
    verify((new Array(ob)).join(), ob.toString(), null);

    ob = new Object();
    verify((new Array(ob)).join(), ob.toString(), null);

    ob = new String();
    verify((new Array(ob)).join(), ob.toString(), null);

    ob = new udObject();
    verify((new Array(ob)).join(), ob.toString(), null);

    //----------------------------------------------------------------------------
    apInitScenario("test arg conversion: roundtrip with String.split(,)");

    verify((((new Array(1.2,3,"4.5",true)).join()).split(",")).join("|"), 
			"1.2|3|4.5|true", null);

    apEndTest();

}


arjoin03();


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
