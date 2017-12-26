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


var iTestID = 220496;

/* -------------------------------------------------------------------------
  Test: 	TYPARR13
   
  
 
  Component:	JScript
 
  Major Area:	Typed Arrays
 
  Test Area:	Matrix testing arrays of different types
 
  Keywords:	
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

			1.  var a : Error[];

			2.  var a : Error[][];

			3.	var a : Error[] = [x];

			4.  var a : Error[][] = [x][y];

			5.  var a : Error[][][] = [x][y][z];

			6.  var a : Error[] = new Error[x];

			7.  var a : Error[,] = new Error[x,y];

			8.  var a : Error[,,] = new Error[x,y,z];

			9.  var a : Error[,,,] = new Error[w,x,y,z];


  Abstract:	 Testing that single, jagged, and multi-dimensional arrays
			 can be created and accessed.
 ---------------------------------------------------------------------------
  Category:			Functionality
 
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


function typarr13() {

    apInitTest("typarr13: Typed Arrays -- Error"); 

	var err1 : Error = new Error(6);
	var err2 : Error = new Error(8);

	apInitScenario("1.  var a : Error[]");
		var a1 : Error[] = [err1,err1,err1,err1];
		verify (a1.GetType(),"Microsoft.JScript.ErrorObject[]","1.1 Wrong type","");
		verify (a1[0].GetType(),"Microsoft.JScript.ErrorObject","1.2 Wrong type","");
		verify (a1[2].GetType(),"Microsoft.JScript.ErrorObject","1.3 Wrong type","");
		verify (a1[0].number,6,"1.4 Wrong value","");
		verify (a1[2].number,6,"1.5 Wrong value","");
		a1[2] = err2;
		verify (a1[2].number,8,"1.6 Wrong value","");


	apInitScenario("2.  var a : Error[][]");
		var a2 : Error[][] = [[err1,err1,err1],[err1,err1,err1],[err1,err1,err1]];
		verify (a2.GetType(),"Microsoft.JScript.ErrorObject[][]","2.1 Wrong type","");
		verify (a2[0].GetType(),"Microsoft.JScript.ErrorObject[]","2.2 Wrong type","");
		verify (a2[1][2].GetType(),"Microsoft.JScript.ErrorObject","2.3 Wrong type","");
		verify (a2[1][2].number,6,"2.4 Wrong value","");
		a2[1][2] = err2;
		verify (a2[1][2].number,8,"2.5 Wrong value","");


	apInitScenario("3.  var a : Error[] = [x]");
		var x3 : Error = err1;
		var a3 : Error[] = [x3];
		a3[0] = err1;
		verify (a3.GetType(),"Microsoft.JScript.ErrorObject[]","3.1 Wrong type","");
		verify (a3[0].GetType(),"Microsoft.JScript.ErrorObject","3.2 Wrong type","");
		verify (a3[0].GetType(),"Microsoft.JScript.ErrorObject","3.3 Wrong type","");
		verify (a3[0].number,6,"3.4 Wrong value","");
		var x3a : Error[] = [err1,err1,err1];
		a3 = x3a;
		verify (a3[2].number,6,"3.5 Wrong value","");
		

	apInitScenario("4.  var a : Error[][] = [x][y]");
		var x4 : Error = err1;
		var y4 : Error = err1;
		var a4 : Error[][] = [[x4],[y4]];
		verify (a4.GetType(),"Microsoft.JScript.ErrorObject[][]","2.1 Wrong type","");
		verify (a4[0].GetType(),"Microsoft.JScript.ErrorObject[]","2.2 Wrong type","");
		verify (a4[0][0].GetType(),"Microsoft.JScript.ErrorObject","2.3 Wrong type","");
		verify (a4[0][0].number,6,"2.4 Wrong value","");


	apInitScenario("5.  var a : Error[][][] = [x][y][z]");
		var x5 : Error = err1;
		var y5 : Error = err1;
		var z5 : Error = err1;
		var a5 : Error[][][] = [[[x5],[y5],[z5]]];
		verify (a5.GetType(),"Microsoft.JScript.ErrorObject[][][]","5.1 Wrong type","");
		verify (a5[0].GetType(),"Microsoft.JScript.ErrorObject[][]","5.2 Wrong type","");
		verify (a5[0][0].GetType(),"Microsoft.JScript.ErrorObject[]","5.3 Wrong type","");
		verify (a5[0][0][0].number,6,"5.4 Wrong value","");


	apInitScenario("6.  var a : Error[] = new Error[x]");
		var a6 : Error[] = new Error[4];
		a6[0] = err1;
		a6[1] = err1;
		a6[2] = err1;
		a6[3] = err1;
		verify (a6.GetType(),"Microsoft.JScript.ErrorObject[]","6.1 Wrong type","");
		verify (a6[0].GetType(),"Microsoft.JScript.ErrorObject","6.2 Wrong type","");
		verify (a6[2].GetType(),"Microsoft.JScript.ErrorObject","6.3 Wrong type","");
		verify (a6[0].number,6,"6.4 Wrong value","");
		verify (a6[2].number,6,"6.5 Wrong value","");
		a6[2] = err2;
		verify (a6[2].number,8,"6.6 Wrong value","");


	apInitScenario("7.  var a : Error[,] = new Error[x,y]");
		var a7 : Error[,] = new Error[1,4];
		a7[0,0] = err1;
		a7[0,1] = err1;
		a7[0,2] = err1;
		a7[0,3] = err1;
		verify (a7.GetType(),"Microsoft.JScript.ErrorObject[,]","7.1 Wrong type","");
		verify (a7[0,2].GetType(),"Microsoft.JScript.ErrorObject","7.3 Wrong type","");
		verify (a7[0,0].number,6,"7.4 Wrong value","");
		verify (a7[0,2].number,6,"7.5 Wrong value","");
		a7[0,2] = err2;
		verify (a7[0,2].number,8,"7.6 Wrong value","");


	apInitScenario("8.  var a : Error[,,] = new Error[x,y,z]");
		var a8 : Error[,,] = new Error[1,1,4];
		a8[0,0,0] = err1;
		a8[0,0,1] = err1;
		a8[0,0,2] = err1;
		a8[0,0,3] = err1;
		verify (a8.GetType(),"Microsoft.JScript.ErrorObject[,,]","8.1 Wrong type","");
		verify (a8[0,0,2].GetType(),"Microsoft.JScript.ErrorObject","8.3 Wrong type","");
		verify (a8[0,0,0].number,6,"8.4 Wrong value","");
		verify (a8[0,0,2].number,6,"8.5 Wrong value","");
		a8[0,0,2] = err2;
		verify (a8[0,0,2].number,8,"8.6 Wrong value","");


	apInitScenario("9.  var a : Error[,,,] = new Error[w,x,y,z]");
		var a9 : Error[,,,] = new Error[1,1,1,4];
		a9[0,0,0,0] = err1;
		a9[0,0,0,1] = err1;
		a9[0,0,0,2] = err1;
		a9[0,0,0,3] = err1;
		verify (a9.GetType(),"Microsoft.JScript.ErrorObject[,,,]","9.1 Wrong type","");
		verify (a9[0,0,0,2].GetType(),"Microsoft.JScript.ErrorObject","9.3 Wrong type","");
		verify (a9[0,0,0,0].number,6,"9.4 Wrong value","");
		verify (a9[0,0,0,2].number,6,"9.5 Wrong value","");
		a9[0,0,0,2] = err2;
		verify (a9[0,0,0,2].number,8,"9.6 Wrong value","");


	apEndTest();
}


typarr13();


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
