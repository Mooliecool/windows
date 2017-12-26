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


var iTestID = 53103;



@if(!@aspx)
function rectangle(w, h){ this.width=w; this.height=h; }
@else
expando function rectangle(w, h){ this.width=w;	this.height=h; }
@end

function verify(vAct, vExp, vMsg, bugNum){
    if (null == bugNum) bugNum = "";
    if (vAct != vExp)
        apLogFailInfo(vMsg, vExp, vAct, bugNum);
}

function funcobj1 (){
@if(@_fast)
    var rect1, exp1
@end
    apInitTest("funcobj1 ");
    
  //-------------------------------------------------------------------
    apInitScenario("test class existence");

    verify(typeof Function, "function", "", null);

   //----------------------------------------------------------------------------
    apInitScenario("verify instance is a func");

    verify(typeof (new Function()), "function", "", null );

   //----------------------------------------------------------------------------
    apInitScenario("verify function can execute");

    verify((new Function("return 5"))(), 5, "", 211237);

   //----------------------------------------------------------------------------
    apInitScenario("verify function interprets args correctly");

    verify((new Function("a","b","return a+b"))(10,3), 13,"",  null );

   //----------------------------------------------------------------------------
    apInitScenario("verify function interprets args correctly");

    verify((new Function("a","b","return a+b"))(10,3), 13, "", null );

   //----------------------------------------------------------------------------
    apInitScenario("verify function has an arguments member - not in fast mode");
@if(!@_fast)
    function fnSum1(){
        var i,tot=0; 
		for (i=0;i<fnSum1.arguments.length;++i) tot+=fnSum1.arguments[i];
		return tot;
    }
    verify(fnSum1(1,2,3,4,5), 15, "", null );
@end

    //----------------------------------------------------------------------------
//    apInitScenario("verify function has a caller member");

//    fnGetCaller = new Function("return caller.toString()");

//    verify(fnGetCaller(), funcobj1.toString(), "", null );

    //----------------------------------------------------------------------------
    apInitScenario("verify function member addition - not in fast mode");
@if(!@_fast)
    function fnGetNewMem() {return fnGetNewMem.newmem;}
    fnGetNewMem.newmem = "hi";

    verify(fnGetNewMem(), "hi", "", null );
@end
    //----------------------------------------------------------------------------
    apInitScenario("Function called as function returns func obj");

    var fnSum = Function("return true");
    verify(fnSum(1,2,3,4,5), true, "", null );
@if(!@_fast)
    fnSum = Function("if(arguments[0] == 'pass')return true; return false;");
    verify(fnSum(""), false, "", null );
    verify(fnSum("pass"), true, "", null );
@end
    //----------------------------------------------------------------------------
    apInitScenario("various types of new operator");

	fnSum  = new new Function("this.x = 10");
	verify(fnSum.x,10,"", null);

	fnSum  = new new Function("x", "this.x = x")(20);
	verify(fnSum.x,20,"", null);

	fnSum  = new new Function("a, b", "return new Function(a, b)")("y", "return y")(50);
	verify(fnSum,50,"", 216765);

    //----------------------------------------------------------------------------
    apInitScenario("function.contructor returns correct string");
	rect1 = new rectangle(1,2);
@if(!@aspx)
    apInitScenario("1. function.contructor returns correct string");
	exp1 =unescape("function%20rectangle%28w%2C%20h%29%7B%20this.width%3Dw%3B%20this.height%3Dh%3B%20%7D")
	if(rect1.constructor != exp1)
		apLogFailInfo("Failed. " ,exp1, rect1.constructor, 300);
@else
    apInitScenario("2. function.contructor returns correct string");
	rect1 = new rectangle(1,2);
	exp1 =unescape("expando%20function%20rectangle%28w%2C%20h%29%7B%20this.width%3Dw%3B%20this.height%3Dh%3B%20%7D")
	if(rect1.constructor != exp1)
		apLogFailInfo("Failed. " ,exp1, rect1.constructor, 300);
@end

    //----------------------------------------------------------------------------
    apInitScenario("control returned from last scenario");
    apEndTest();
}


funcobj1();


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
