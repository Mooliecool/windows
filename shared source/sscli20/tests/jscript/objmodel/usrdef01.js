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


var iTestID = 53108;

@if(@_fast)
   var test1,test3,test5,test7,test8,test9;
@end
function verify(sCat, vAct, vExp, bugNum)
{
    if (null == bugNum) bugNum = "";

    if (vAct != vExp)
        apLogFailInfo( sCat+" failed", vExp, vAct, bugNum);
}


@if(!@aspx)
	function udef1 () { /* nothing */ }
	function udef2 () { this.a = "string1"; this.b = "string2"; }
	function udef3 (a,b,c) { this.a = a; this.b = b; this.c = c; }
	function udef4 (a,b,c) { this.foo = a; this.bar = b; this.baz = c; }
@else
	expando function udef1 () { /* nothing */ }
	expando function udef2 () { this.a = "string1"; this.b = "string2"; }
	expando function udef3 (a,b,c) { this.a = a; this.b = b; this.c = c; }
	expando function udef4 (a,b,c) { this.foo = a; this.bar = b; this.baz = c; }
@end

function usrdef01()
{
 
    apInitTest("usrdef01 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. test construction");

    test1 = new udef1();
    // success means no error.

    
    //----------------------------------------------------------------------------
    apInitScenario("2. verify construction with typeof");

    verify("typeof verification", typeof (new udef1()), "object", null);


    //----------------------------------------------------------------------------
    apInitScenario("3. verify indexed member addition, explicit lval");

    verify("obj member addition, explicit lval", (test3 = new udef1())[-13] = "bar", "bar", null);


    //----------------------------------------------------------------------------
    apInitScenario("4. verify indexed member addition, implicit lval");

    verify("obj member addition, implicit lval", (new udef1())[-13] = "bar", "bar", null);


    //----------------------------------------------------------------------------
    apInitScenario("5. verify named member addition, explicit lval");

    verify("obj member addition, explicit lval", (test5 = new udef1()).foo = "bar", "bar", null);


    //----------------------------------------------------------------------------
    apInitScenario("6. verify named member addition, implicit lval");

    verify("obj member addition, implicit lval", (new udef1()).foo = "bar", "bar", null);


    //----------------------------------------------------------------------------
    apInitScenario("7. verify default member addition");

    test7 = new udef2();

    verify("verify default member addition", test7.a+test7.b, "string1string2", null);


    //----------------------------------------------------------------------------
    apInitScenario("8. verify param member addition");

    test8 = new udef3(1,2,3);

    verify("verify param member addition", test8.a+test8.b+test8.c, 6, null);


    //----------------------------------------------------------------------------
    apInitScenario("9. verify dissimilarly named param member addition");

    test9 = new udef4(1,2,3);

    verify("verify dissimilarly member addition", test9.foo+test9.bar+test9.baz, 6, null);



    apEndTest();

}


usrdef01();


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
