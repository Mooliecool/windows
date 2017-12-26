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


var iTestID = 51736;


function verify (sMsg, vAct, vExp, bugNum){
    if (null == bugNum) bugNum = "";

    if (vAct != vExp)
        apLogFailInfo(sMsg, vExp, vAct, bugNum);
}

@if(@aspx) expando @end function udObject () { /* nothing */ }

function arsort02 (){
@if(@_fast)
    var rg,str,d1,d2,udObj,num,b,arr,ob;
@end
    apInitTest("arSort02 ");

    //----------------------------------------------------------------------------
    // don't test elem removal since it's implementation specific and not important
    apInitScenario("test array compaction");

    rg = new Array();
    rg[45] = "hi";
    rg[3] = "bye";
    rg[12] = "nulk";
    rg[10] = "nulm";
    rg.sort();

    // it should be this, but for the null string conversion
    verify("compaction", rg[0]=="bye" && rg[1]=="hi" && rg[2]=="nulk" && rg[3]=="nulm", true, null);
    // therefore, we'll test like this:
    //verify("",rg[0]=="bye" && rg[1]=="hi" && rg[2]=="nulk" && rg[45]=="nulm", true, null);

    //----------------------------------------------------------------------------
    apInitScenario("test comparison by string conversion");

    str = new String("hello");
    d1 = new Date(0);
    d1.setTime(Date.UTC(70,0,1,0) + d1.getTimezoneOffset() * 60000 - 28800000);
    d2 = new Date(1000*60*24*365.25*10);
    d2.setTime(Date.UTC(70,2,2,21) + d1.getTimezoneOffset() * 6000 - 28800000);
    udObj = new udObject();
    num = new Number(45);
    b = new Boolean(true);
    arr = new Array(11,22,33,44);
    ob = new Object();
	var fn = new Function();

    rg = new Array();
    rg[ 0] = str;
    rg[ 1] = "hi";
    rg[ 2] = d1;
    rg[ 3] = "z";
    rg[ 4] = udObj;
    rg[ 5] = "a";
    rg[ 6] = "B";
    rg[ 7] = d2;
    rg[ 8] = "200";
    rg[ 9] = 4;
    rg[10] = "1";
    rg[11] = num;
    rg[12] = b;
    rg[13] = arr;
    rg[14] = ob;
	rg[15] = null;
	rg[16] = fn;
    rg.sort();


@if (@_win32||@_win64||@rotor)
    verify( "", rg[ 0] == "1" &&
            rg[ 1] == arr &&
            --(rg[ 2]) == "199" &&
            ++(rg[ 3]) == 5 &&
            rg[ 4] == num &&
            rg[ 5] == "B" &&
            rg[ 6] == d2 &&
            rg[ 7] == d1 &&
            ((
              rg[ 8] == udObj &&
              rg[ 9] == ob )
                 ||
            ( rg[ 8] == ob &&
              rg[ 9] == udObj )) &&
            rg[10] == "a" &&
			rg[11] == fn && 
            rg[12] == str &&
            rg[13] == "hi" &&
			rg[14] == null &&
            rg[15] == b &&
            rg[16] == "z", true, null );
@else
    verify( "", rg[ 1] == "1" &&
            rg[ 2] == arr &&
            --(rg[ 3]) == "199" &&
            ++(rg[ 4]) == 5 &&
            rg[ 5] == num &&
            rg[ 6] == "B" &&
            rg[ 7] == d2 &&
            rg[ 8] == d1 &&
            ((
              rg[ 9] == udObj &&
              rg[ 10] == ob )
                 ||
            ( rg[ 9] == ob &&
              rg[ 10] == udObj )) &&
            rg[11] == "a" &&
			rg[0] == fn && 
            rg[12] == str &&
            rg[13] == "hi" &&
			rg[14] == null &&
            rg[15] == b &&
            rg[16] == "z", true, null );
@end

    apEndTest();

}

function xprint(x) {
  apWriteDebug(x)
}


arsort02();


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
