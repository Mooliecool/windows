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
//@set @rotor=true;

import System;

var NULL_DISPATCH = null;
var apGlobalObj;
var apPlatform;
var lFailCount;
var lInScenario;

 



 



function verify(sAct, sExp, sMes, sBug){
	if (sBug == undefined) {
		sBug = "";
	}
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes, sExp, sAct, sBug);
}


 




@if(!@aspx)
	import System
@end


function typarr14() {

    apInitTest("typarr14: Typed Arrays -- Function"); 

	var f1 : Function = new Function("return 3");
	var f2 : Function = new Function("return 6");

	apInitScenario("1.  var a : Function[]");
		var a1 : Function[] = [f1,f1,f1,f1];
		verify (a1.GetType(),"Microsoft.JScript.ScriptFunction[]","1.1 Wrong type","");
		verify (a1[0].GetType(),"Microsoft.JScript.Closure","1.2 Wrong type","");
		verify (a1[2].GetType(),"Microsoft.JScript.Closure","1.3 Wrong type","");
		verify (a1[0].toString(),f1.toString(),"1.4 Wrong value","");
		verify (a1[2].toString(),f1.toString(),"1.5 Wrong value","");
		a1[2] = f2;
		verify (a1[2].toString(),f2.toString(),"1.f1.toString() Wrong value","");


	apInitScenario("2.  var a : Function[][]");
		var a2 : Function[][] = [[f1,f1,f1],[f1,f1,f1],[f1,f1,f1]];
		verify (a2.GetType(),"Microsoft.JScript.ScriptFunction[][]","2.1 Wrong type","");
		verify (a2[0].GetType(),"Microsoft.JScript.ScriptFunction[]","2.2 Wrong type","");
		verify (a2[1][2].GetType(),"Microsoft.JScript.Closure","2.3 Wrong type","");
		verify (a2[1][2].toString(),f1.toString(),"2.4 Wrong value","");
		a2[1][2] = f2;
		verify (a2[1][2].toString(),f2.toString(),"2.5 Wrong value","");


	apInitScenario("3.  var a : Function[] = [x]");
		var x3 : Function = f1;
		var a3 : Function[] = [x3];
		a3[0] = f1;
		verify (a3.GetType(),"Microsoft.JScript.ScriptFunction[]","3.1 Wrong type","");
		verify (a3[0].GetType(),"Microsoft.JScript.Closure","3.2 Wrong type","");
		verify (a3[0].GetType(),"Microsoft.JScript.Closure","3.3 Wrong type","");
		verify (a3[0].toString(),f1.toString(),"3.4 Wrong value","");
		var x3a : Function[] = [f1,f1,f1];
		a3 = x3a;
		verify (a3[2].toString(),f1.toString(),"3.5 Wrong value","");
		

	apInitScenario("4.  var a : Function[][] = [x][y]");
		var x4 : Function = f1;
		var y4 : Function = f1;
		var a4 : Function[][] = [[x4],[y4]];
		verify (a4.GetType(),"Microsoft.JScript.ScriptFunction[][]","2.1 Wrong type","");
		verify (a4[0].GetType(),"Microsoft.JScript.ScriptFunction[]","2.2 Wrong type","");
		verify (a4[0][0].GetType(),"Microsoft.JScript.Closure","2.3 Wrong type","");
		verify (a4[0][0].toString(),f1.toString(),"2.4 Wrong value","");


	apInitScenario("5.  var a : Function[][][] = [x][y][z]");
		var x5 : Function = f1;
		var y5 : Function = f1;
		var z5 : Function = f1;
		var a5 : Function[][][] = [[[x5],[y5],[z5]]];
		verify (a5.GetType(),"Microsoft.JScript.ScriptFunction[][][]","5.1 Wrong type","");
		verify (a5[0].GetType(),"Microsoft.JScript.ScriptFunction[][]","5.2 Wrong type","");
		verify (a5[0][0].GetType(),"Microsoft.JScript.ScriptFunction[]","5.3 Wrong type","");
		verify (a5[0][0][0].toString(),f1.toString(),"5.4 Wrong value","");


	apInitScenario("6.  var a : Function[] = new Function[x]");
		var a6 : Function[] = new Function[4];
		a6[0] = f1;
		a6[1] = f1;
		a6[2] = f1;
		a6[3] = f1;
		verify (a6.GetType(),"Microsoft.JScript.ScriptFunction[]","6.1 Wrong type","");
		verify (a6[0].GetType(),"Microsoft.JScript.Closure","6.2 Wrong type","");
		verify (a6[2].GetType(),"Microsoft.JScript.Closure","6.3 Wrong type","");
		verify (a6[0].toString(),f1.toString(),"6.4 Wrong value","");
		verify (a6[2].toString(),f1.toString(),"6.5 Wrong value","");
		a6[2] = f2;
		verify (a6[2].toString(),f2.toString(),"6.6 Wrong value","");


	apInitScenario("7.  var a : Function[,] = new Function[x,y]");
		var a7 : Function[,] = new Function[1,4];
		a7[0,0] = f1;
		a7[0,1] = f1;
		a7[0,2] = f1;
		a7[0,3] = f1;
		verify (a7.GetType(),"Microsoft.JScript.ScriptFunction[,]","7.1 Wrong type","");
		verify (a7[0,2].GetType(),"Microsoft.JScript.Closure","7.3 Wrong type","");
		verify (a7[0,0].toString(),f1.toString(),"7.4 Wrong value","");
		verify (a7[0,2].toString(),f1.toString(),"7.5 Wrong value","");
		a7[0,2] = f2;
		verify (a7[0,2].toString(),f2.toString(),"7.f1.toString() Wrong value","");


	apInitScenario("8.  var a : Function[,,] = new Function[x,y,z]");
		var a8 : Function[,,] = new Function[1,1,4];
		a8[0,0,0] = f1;
		a8[0,0,1] = f1;
		a8[0,0,2] = f1;
		a8[0,0,3] = f1;
		verify (a8.GetType(),"Microsoft.JScript.ScriptFunction[,,]","8.1 Wrong type","");
		verify (a8[0,0,2].GetType(),"Microsoft.JScript.Closure","8.3 Wrong type","");
		verify (a8[0,0,0].toString(),f1.toString(),"8.4 Wrong value","");
		verify (a8[0,0,2].toString(),f1.toString(),"8.5 Wrong value","");
		a8[0,0,2] = f2;
		verify (a8[0,0,2].toString(),f2.toString(),"8.f1.toString() Wrong value","");


	apInitScenario("9.  var a : Function[,,,] = new Function[w,x,y,z]");
		var a9 : Function[,,,] = new Function[1,1,1,4];
		a9[0,0,0,0] = f1;
		a9[0,0,0,1] = f1;
		a9[0,0,0,2] = f1;
		a9[0,0,0,3] = f1;
		verify (a9.GetType(),"Microsoft.JScript.ScriptFunction[,,,]","9.1 Wrong type","");
		verify (a9[0,0,0,2].GetType(),"Microsoft.JScript.Closure","9.3 Wrong type","");
		verify (a9[0,0,0,0].toString(),f1.toString(),"9.4 Wrong value","");
		verify (a9[0,0,0,2].toString(),f1.toString(),"9.5 Wrong value","");
		a9[0,0,0,2] = f2;
		verify (a9[0,0,0,2].toString(),f2.toString(),"9.f1.toString() Wrong value","");


	apEndTest();
}

typarr14();
if(lFailCount >= 0) System.Environment.ExitCode = lFailCount;
else System.Environment.ExitCode = 1;

function apInitTest(stTestName) {
    lFailCount = 0;

    apGlobalObj = new Object();
    apGlobalObj.apGetPlatform = function Funca() { return "Rotor" }
    apGlobalObj.LangHost = function Funcb() { return 1033;}
    apGlobalObj.apGetLangExt = function Funcc(num) { return "EN"; }

    apPlatform = apGlobalObj.apGetPlatform();
    var sVer = "1.0";  
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
