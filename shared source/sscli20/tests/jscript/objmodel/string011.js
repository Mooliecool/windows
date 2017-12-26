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


var iTestID = 142146;


function verify(sCat, vAct, vExp, bugNum)
{
    if (null == bugNum) bugNum = "";

    if (vAct != vExp)
        apLogFailInfo( sCat+" failed", vExp, vAct, bugNum);
}

function string011(){
    var n, x;
    apInitTest("string011 - converting numerals to strings");
//----------------------------------------------------------------------------
    apInitScenario("1. test in base 10");

	n=new Number(3.141592653589793);
	verify("1.1 real no. explicit",n.toString(10),"3.141592653589793",null);

	n=new Number(3);
	verify("1.2 int. explicit",n.toString(10),"3",null);

	n=new Number(3141592653589793);
	verify("1.3 big int. explicit",n.toString(10),"3141592653589793",null);

	n=new Number(-3.141592653589793);
	verify("1.4 negative real explicit",n.toString(10),"-3.141592653589793",null);

	n=new Number(3.141592653589793);
	verify("1.5 real no. implicit",n.toString(),"3.141592653589793",null);

	n=new Number(3);
	verify("1.6 int. implicit",n.toString(),"3",null);

	n=new Number(3141592653589793);
	verify("1.7 big int. implicit",n.toString(),"3141592653589793",null);

	n=new Number(-3.141592653589793);
	verify("1.8 negative real implicit",n.toString(),"-3.141592653589793",null);

    apInitScenario("2. test in base 10 - in an eval");

	n=new Number(3.141592653589793);
       x = eval("n.toString(10)");
	verify("2.1 real no. explicit",x,"3.141592653589793",null);

	n=new Number(3);
       x = eval("n.toString(10)");
	verify("2.2 int. explicit",x,"3",null);

	n=new Number(3141592653589793);
       x = eval("n.toString(10)");
	verify("2.3 big int. explicit",x,"3141592653589793",null);

	n=new Number(-3.141592653589793);
       x = eval("n.toString(10)");
	verify("2.4 negative real explicit",x,"-3.141592653589793",null);

	n=new Number(3.141592653589793);
       x = eval("n.toString()");
	verify("2.5 real no. implicit",x,"3.141592653589793",null);

	n=new Number(3);
       x = eval("n.toString()");
	verify("2.6 int. implicit",x,"3",null);

	n=new Number(3141592653589793);
       x = eval("n.toString()");
	verify("2.7 big int. implicit",x,"3141592653589793",null);

	n=new Number(-3.141592653589793);
       x = eval("n.toString()");
	verify("2.8 negative real implicit",x,"-3.141592653589793",null);

    apInitScenario("3. test in base 16");

	n=new Number(3.141592653589793);
	verify("3.1 real no.",n.toString(16),"3.243f6a8885a3",null);

	n=new Number(3);
	verify("3.2 int.",n.toString(16),"3",null);

	n=new Number(16);
	verify("3.3 edge value",n.toString(16),"10",null);

	n=new Number(3141592653589793);
	verify("3.4 big int.",n.toString(16),"b29430a256d21",null);

	n=new Number(-3.141592653589793);
	verify("3.5 negative real",n.toString(16),"-3.243f6a8885a3",null);

    apInitScenario("4. test in base 16 - in an eval");

	n=new Number(3.141592653589793);
       x = eval("n.toString(16)");
	verify("4.1 real no.",x,"3.243f6a8885a3",null);

	n=new Number(3);
       x = eval("n.toString(16)");
	verify("4.2 int.",x,"3",null);

	n=new Number(16);
       x = eval("n.toString(16)");
	verify("4.3 edge value",x,"10",null);

	n=new Number(3141592653589793);
       x = eval("n.toString(16)");
	verify("4.4 big int.",x,"b29430a256d21",null);

	n=new Number(-3.141592653589793);
       x = eval("n.toString(16)");
	verify("4.5 negative real",x,"-3.243f6a8885a3",null);

    apInitScenario("5. test in base 8");

	n=new Number(3.141592653589793);
	verify("5.1 real no.",n.toString(8),"3.1103755242102643",null);

	n=new Number(3);
	verify("5.2 int.",n.toString(8),"3",null);

	n=new Number(16);
	verify("5.3 edge value",n.toString(8),"20",null);

	n=new Number(3141592653589793);
	verify("5.4 big int.",n.toString(8),"131224141211266441",null);

	n=new Number(-3.141592653589793);
	verify("5.5 negative real",n.toString(8),"-3.1103755242102643",null);

    apInitScenario("6. test in base 8 - in an eval");

	n=new Number(3.141592653589793);
       x = eval("n.toString(8)");
	verify("6.1 real no.",x,"3.1103755242102643",null);

	n=new Number(3);
       x = eval("n.toString(8)");
	verify("6.2 int.",x,"3",null);

	n=new Number(16);
       x = eval("n.toString(8)");
	verify("6.3 edge value",x,"20",null);

	n=new Number(3141592653589793);
       x = eval("n.toString(8)");
	verify("6.4 big int.",x,"131224141211266441",null);

	n=new Number(-3.141592653589793);
       x = eval("n.toString(8)");
	verify("6.5 negative real",x,"-3.1103755242102643",null);

    apInitScenario("7. test in base 2");

	n=new Number(3.141592653589793);
	verify("7.1 real no.",n.toString(2),"11.001001000011111101101010100010001000010110100011",null);

	n=new Number(1);
	verify("7.2 int.",n.toString(2),"1",null);

	n=new Number(16);
	verify("7.3 edge value",n.toString(2),"10000",null);

	n=new Number(3141592653589793);
	verify("7.4 big int.",n.toString(2),"1011001010010100001100001010001001010110110100100001",null);

	n=new Number(-3.141592653589793);
	verify("7.5 negative real",n.toString(2),"-11.001001000011111101101010100010001000010110100011",null);

    apInitScenario("8. test in base 2 - in an eval");

	n=new Number(3.141592653589793);
       x = eval("n.toString(2)");
	verify("8.1 real no.",x,"11.001001000011111101101010100010001000010110100011",null);

	n=new Number(1);
       x = eval("n.toString(2)");
	verify("8.2 int.",x,"1",null);

	n=new Number(16);
       x = eval("n.toString(2)");
	verify("8.3 edge value",x,"10000",null);

	n=new Number(3141592653589793);
       x = eval("n.toString(2)");
	verify("8.4 big int.",x,"1011001010010100001100001010001001010110110100100001",null);

	n=new Number(-3.141592653589793);
       x = eval("n.toString(2)");
	verify("8.5 negative real",x,"-11.001001000011111101101010100010001000010110100011",null);

    apEndTest();

}


string011();


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
