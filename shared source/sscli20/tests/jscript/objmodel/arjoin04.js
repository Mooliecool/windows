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


var iTestID = 51733;


function verify(vAct, vExp, bugNum)
{
    if (null == bugNum) bugNum = "";

    if (vAct != vExp)
        apLogFailInfo("", vExp, vAct, bugNum);
}

@if(@aspx) expando @end function udObject () { /* nothing */ }

function arjoin04()
{ var ob; 
    apInitTest("arJoin04 ");
    
    //----------------------------------------------------------------------------
    apInitScenario("test zls delim");

    verify((new Array(1,2,3,4)).join(""), "1234", null);

    
    //----------------------------------------------------------------------------
    apInitScenario("test delim conversion: pos integer");

    verify((new Array(1,2,3,4)).join(0), "1020304", null);

    
    //----------------------------------------------------------------------------
    apInitScenario("test delim conversion: neg integer");

    verify((new Array(1,2,3,4)).join(-1), "1-12-13-14", null);

    
    //----------------------------------------------------------------------------
    apInitScenario("test delim conversion: real");

    verify((new Array(1,2,3,4)).join(4.5), "14.524.534.54", null);

    
    //----------------------------------------------------------------------------
    apInitScenario("test delim conversion: bool");

    verify((new Array(1,2,3,4)).join(true), "1true2true3true4", null);

    
    //----------------------------------------------------------------------------
    apInitScenario("test delim conversion: objects");

    ob = new Array();
    verify((new Array(1,2)).join(ob), "1"+ob.toString()+"2", null);

    ob = new Boolean();
    verify((new Array(1,2)).join(ob), "1"+ob.toString()+"2", null);

    ob = new Date();
    verify((new Array(1,2)).join(ob), "1"+ob.toString()+"2", null);

    ob = new Number();
    verify((new Array(1,2)).join(ob), "1"+ob.toString()+"2", null);

    ob = new Object();
    verify((new Array(1,2)).join(ob), "1"+ob.toString()+"2", null);

    ob = new String();
    verify((new Array(1,2)).join(ob), "1"+ob.toString()+"2", null);

    ob = new udObject();
    verify((new Array(1,2)).join(ob), "1"+ob.toString()+"2", null);

//----------------------------------------------------------------------------
    apInitScenario("test delim conversion: expressions");

    verify((new Array(1,2,3,4)).join(1+3), "1424344", null);
    verify((new Array(1,2,3,4)).join(5*10/25+9), "1112113114", null);
	verify((new Array(1,2,3,4)).join("st"+"op"), "1stop2stop3stop4", null);
	verify((new Array(1,2,3,4)).join(0/0), "1NaN2NaN3NaN4", null);
	verify((new Array(1,2,3,4)).join(1/0), "1Infinity2Infinity3Infinity4", null);

//----------------------------------------------------------------------------
    apInitScenario("test delim conversion: eval(expressions)");

    verify((new Array(1,2,3,4)).join(eval(1+3)), "1424344", null);
    verify((new Array(1,2,3,4)).join(eval(5*10/25+9)), "1112113114", null);
	var a=8,b=7;
	verify((new Array(1,2,3,4)).join(eval(a*b)), "1562563564", null);
	verify((new Array(1,2,3,4)).join(eval(0/0)), "1NaN2NaN3NaN4", null);
	verify((new Array(1,2,3,4)).join(eval(1/0)), "1Infinity2Infinity3Infinity4", null);

//----------------------------------------------------------------------------
    apInitScenario("test delim conversion: array element");

    verify((a=new Array(1,2,3,4)).join(a[0]), "1121314", null);
	verify((a=new Array(1,2,3,4)).join(a[5]), "1undefined2undefined3undefined4", null);
	apEndTest();

}


arjoin04();


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
