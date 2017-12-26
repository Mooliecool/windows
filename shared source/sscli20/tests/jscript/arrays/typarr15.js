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


var iTestID = 220498;

/* -------------------------------------------------------------------------
  Test: 	TYPARR15
   
  
 
  Component:	JScript
 
  Major Area:	Typed Arrays
 
  Test Area:	Matrix testing arrays of different types
 
  Keywords:	
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

			1.  var a : RegExp[];

			2.  var a : RegExp[][];

			3.	var a : RegExp[] = [x];

			4.  var a : RegExp[][] = [x][y];

			5.  var a : RegExp[][][] = [x][y][z];

			6.  var a : RegExp[] = new RegExp[x];

			7.  var a : RegExp[,] = new RegExp[x,y];

			8.  var a : RegExp[,,] = new RegExp[x,y,z];

			9.  var a : RegExp[,,,] = new RegExp[w,x,y,z];


  Abstract:	 Testing that single, jagged, and multi-dimensional arrays
			 can be created and accessed.
 ---------------------------------------------------------------------------
  Category:			RegExpality
 
  Product:			JScript
 
  Related Files: 
 
  Notes:
 ---------------------------------------------------------------------------
  
 

 -------------------------------------------------------------------------*/


/*----------
/
/  Helper functions
/
----------*/


function verify(sAct, sExp, sMes, sBug){
	if (sBug == undefined) {
		sBug = "";
	}
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes, sExp, sAct, sBug);
}


/*----------
/
/  Global variables
/
----------*/



@if(!@aspx)
	import System
@end



function typarr15() {

    apInitTest("typarr15: Typed Arrays -- RegExp"); 

	var r1 : RegExp = new RegExp("hello","gi");
	var r2 : RegExp = new RegExp("dog","gi");

	apInitScenario("1.  var a : RegExp[]");
		var a1 : RegExp[] = [r1,r1,r1,r1];
		verify (a1.GetType(),"Microsoft.JScript.RegExpObject[]","1.1 Wrong type","");
		verify (a1[0].GetType(),"Microsoft.JScript.RegExpObject","1.2 Wrong type","");
		verify (a1[2].GetType(),"Microsoft.JScript.RegExpObject","1.3 Wrong type","");
		verify (a1[0].toString(),r1.toString(),"1.4 Wrong value","");
		verify (a1[2].toString(),r1.toString(),"1.5 Wrong value","");
		a1[2] = r2;
		verify (a1[2].toString(),r2.toString(),"1.r1.toString() Wrong value","");


	apInitScenario("2.  var a : RegExp[][]");
		var a2 : RegExp[][] = [[r1,r1,r1],[r1,r1,r1],[r1,r1,r1]];
		verify (a2.GetType(),"Microsoft.JScript.RegExpObject[][]","2.1 Wrong type","");
		verify (a2[0].GetType(),"Microsoft.JScript.RegExpObject[]","2.2 Wrong type","");
		verify (a2[1][2].GetType(),"Microsoft.JScript.RegExpObject","2.3 Wrong type","");
		verify (a2[1][2].toString(),r1.toString(),"2.4 Wrong value","");
		a2[1][2] = r2;
		verify (a2[1][2].toString(),r2.toString(),"2.5 Wrong value","");


	apInitScenario("3.  var a : RegExp[] = [x]");
		var x3 : RegExp = r1;
		var a3 : RegExp[] = [x3];
		a3[0] = r1;
		verify (a3.GetType(),"Microsoft.JScript.RegExpObject[]","3.1 Wrong type","");
		verify (a3[0].GetType(),"Microsoft.JScript.RegExpObject","3.2 Wrong type","");
		verify (a3[0].GetType(),"Microsoft.JScript.RegExpObject","3.3 Wrong type","");
		verify (a3[0].toString(),r1.toString(),"3.4 Wrong value","");
		var x3a : RegExp[] = [r1,r1,r1];
		a3 = x3a;
		verify (a3[2].toString(),r1.toString(),"3.5 Wrong value","");
		

	apInitScenario("4.  var a : RegExp[][] = [x][y]");
		var x4 : RegExp = r1;
		var y4 : RegExp = r1;
		var a4 : RegExp[][] = [[x4],[y4]];
		verify (a4.GetType(),"Microsoft.JScript.RegExpObject[][]","2.1 Wrong type","");
		verify (a4[0].GetType(),"Microsoft.JScript.RegExpObject[]","2.2 Wrong type","");
		verify (a4[0][0].GetType(),"Microsoft.JScript.RegExpObject","2.3 Wrong type","");
		verify (a4[0][0].toString(),r1.toString(),"2.4 Wrong value","");


	apInitScenario("5.  var a : RegExp[][][] = [x][y][z]");
		var x5 : RegExp = r1;
		var y5 : RegExp = r1;
		var z5 : RegExp = r1;
		var a5 : RegExp[][][] = [[[x5],[y5],[z5]]];
		verify (a5.GetType(),"Microsoft.JScript.RegExpObject[][][]","5.1 Wrong type","");
		verify (a5[0].GetType(),"Microsoft.JScript.RegExpObject[][]","5.2 Wrong type","");
		verify (a5[0][0].GetType(),"Microsoft.JScript.RegExpObject[]","5.3 Wrong type","");
		verify (a5[0][0][0].toString(),r1.toString(),"5.4 Wrong value","");


	apInitScenario("6.  var a : RegExp[] = new RegExp[x]");
		var a6 : RegExp[] = new RegExp[4];
		a6[0] = r1;
		a6[1] = r1;
		a6[2] = r1;
		a6[3] = r1;
		verify (a6.GetType(),"Microsoft.JScript.RegExpObject[]","6.1 Wrong type","");
		verify (a6[0].GetType(),"Microsoft.JScript.RegExpObject","6.2 Wrong type","");
		verify (a6[2].GetType(),"Microsoft.JScript.RegExpObject","6.3 Wrong type","");
		verify (a6[0].toString(),r1.toString(),"6.4 Wrong value","");
		verify (a6[2].toString(),r1.toString(),"6.5 Wrong value","");
		a6[2] = r2;
		verify (a6[2].toString(),r2.toString(),"6.6 Wrong value","");


	apInitScenario("7.  var a : RegExp[,] = new RegExp[x,y]");
		var a7 : RegExp[,] = new RegExp[1,4];
		a7[0,0] = r1;
		a7[0,1] = r1;
		a7[0,2] = r1;
		a7[0,3] = r1;
		verify (a7.GetType(),"Microsoft.JScript.RegExpObject[,]","7.1 Wrong type","");
		verify (a7[0,2].GetType(),"Microsoft.JScript.RegExpObject","7.3 Wrong type","");
		verify (a7[0,0].toString(),r1.toString(),"7.4 Wrong value","");
		verify (a7[0,2].toString(),r1.toString(),"7.5 Wrong value","");
		a7[0,2] = r2;
		verify (a7[0,2].toString(),r2.toString(),"7.r1.toString() Wrong value","");


	apInitScenario("8.  var a : RegExp[,,] = new RegExp[x,y,z]");
		var a8 : RegExp[,,] = new RegExp[1,1,4];
		a8[0,0,0] = r1;
		a8[0,0,1] = r1;
		a8[0,0,2] = r1;
		a8[0,0,3] = r1;
		verify (a8.GetType(),"Microsoft.JScript.RegExpObject[,,]","8.1 Wrong type","");
		verify (a8[0,0,2].GetType(),"Microsoft.JScript.RegExpObject","8.3 Wrong type","");
		verify (a8[0,0,0].toString(),r1.toString(),"8.4 Wrong value","");
		verify (a8[0,0,2].toString(),r1.toString(),"8.5 Wrong value","");
		a8[0,0,2] = r2;
		verify (a8[0,0,2].toString(),r2.toString(),"8.r1.toString() Wrong value","");


	apInitScenario("9.  var a : RegExp[,,,] = new RegExp[w,x,y,z]");
		var a9 : RegExp[,,,] = new RegExp[1,1,1,4];
		a9[0,0,0,0] = r1;
		a9[0,0,0,1] = r1;
		a9[0,0,0,2] = r1;
		a9[0,0,0,3] = r1;
		verify (a9.GetType(),"Microsoft.JScript.RegExpObject[,,,]","9.1 Wrong type","");
		verify (a9[0,0,0,2].GetType(),"Microsoft.JScript.RegExpObject","9.3 Wrong type","");
		verify (a9[0,0,0,0].toString(),r1.toString(),"9.4 Wrong value","");
		verify (a9[0,0,0,2].toString(),r1.toString(),"9.5 Wrong value","");
		a9[0,0,0,2] = r2;
		verify (a9[0,0,0,2].toString(),r2.toString(),"9.r1.toString() Wrong value","");


	apEndTest();
}

typarr15();


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
