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


var iTestID = 220494;


/* -------------------------------------------------------------------------
  Test: 	TYPARR11
   
  
 
  Component:	JScript
 
  Major Area:	Typed Arrays
 
  Test Area:	Matrix testing arrays of different types
 
  Keywords:	
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

			1.  var a : Date[];

			2.  var a : Date[][];

			3.	var a : Date[] = [x];

			4.  var a : Date[][] = [x][y];

			5.  var a : Date[][][] = [x][y][z];

			6.  var a : Date[] = new Date[x];

			7.  var a : Date[,] = new Date[x,y];

			8.  var a : Date[,,] = new Date[x,y,z];

			9.  var a : Date[,,,] = new Date[w,x,y,z];


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


function typarr11() {

    apInitTest("typarr11: Typed Arrays -- Date"); 

	var result;

	apInitScenario("1.  var a : Date[]");
		var a1 : Date[] = [new Date(2001,8,11),new Date(2001,8,11),new Date(2001,8,11),new Date(2001,8,11)];
		verify (a1.GetType(),"Microsoft.JScript.DateObject[]","1.1 Wrong type","");
		verify (a1[0].GetType(),"Microsoft.JScript.DateObject","1.2 Wrong type","");
		verify (a1[2].GetType(),"Microsoft.JScript.DateObject","1.3 Wrong type","");
		verify (a1[0].toString(),new Date(2001,8,11),"1.4 Wrong value","");
		verify (a1[2].toString(),new Date(2001,8,11),"1.5 Wrong value","");
		a1[2] = new Date(2001,8,11);
		verify (a1[2].toString(),new Date(2001,8,11),"1.6 Wrong value","");


	apInitScenario("2.  var a : Date[][]");
		var a2 : Date[][] = [[new Date(2001,8,11),new Date(2001,8,11),new Date(2001,8,11)],[new Date(2001,8,11),new Date(2001,8,11),new Date(2001,8,11)],[new Date(2001,8,11),new Date(2001,8,11),new Date(2001,8,11)]];
		verify (a2.GetType(),"Microsoft.JScript.DateObject[][]","2.1 Wrong type","");
		verify (a2[0].GetType(),"Microsoft.JScript.DateObject[]","2.2 Wrong type","");
		verify (a2[1][2].GetType(),"Microsoft.JScript.DateObject","2.3 Wrong type","");
		verify (a2[1][2].toString(),new Date(2001,8,11),"2.4 Wrong value","");
		a2[1][2] = new Date(2001,8,11);
		verify (a2[1][2].toString(),new Date(2001,8,11),"2.5 Wrong value","");


	apInitScenario("3.  var a : Date[] = [x]");
		var x3 : Date = new Date(2001,8,11);
		var a3 : Date[] = [x3];
		a3[0] = new Date(2001,8,11);
		verify (a3.GetType(),"Microsoft.JScript.DateObject[]","3.1 Wrong type","");
		verify (a3[0].GetType(),"Microsoft.JScript.DateObject","3.2 Wrong type","");
		verify (a3[0].GetType(),"Microsoft.JScript.DateObject","3.3 Wrong type","");
		verify (a3[0].toString(),new Date(2001,8,11),"3.4 Wrong value","");
		var x3a : Date[] = [new Date(2001,8,11),new Date(2001,8,11),new Date(2001,8,11)];
		a3 = x3a;
		verify (a3[2].toString(),new Date(2001,8,11),"3.5 Wrong value","");
		

	apInitScenario("4.  var a : Date[][] = [x][y]");
		var x4 : Date = new Date(2001,8,11);
		var y4 : Date = new Date(2001,8,11);
		var a4 : Date[][] = [[x4],[y4]];
		verify (a4.GetType(),"Microsoft.JScript.DateObject[][]","2.1 Wrong type","");
		verify (a4[0].GetType(),"Microsoft.JScript.DateObject[]","2.2 Wrong type","");
		verify (a4[0][0].GetType(),"Microsoft.JScript.DateObject","2.3 Wrong type","");
		verify (a4[0][0].toString(),new Date(2001,8,11),"2.4 Wrong value","");


	apInitScenario("5.  var a : Date[][][] = [x][y][z]");
		var x5 : Date = new Date(2001,8,11);
		var y5 : Date = new Date(2001,8,11);
		var z5 : Date = new Date(2001,8,11);
		var a5 : Date[][][] = [[[x5],[y5],[z5]]];
		verify (a5.GetType(),"Microsoft.JScript.DateObject[][][]","5.1 Wrong type","");
		verify (a5[0].GetType(),"Microsoft.JScript.DateObject[][]","5.2 Wrong type","");
		verify (a5[0][0].GetType(),"Microsoft.JScript.DateObject[]","5.3 Wrong type","");
		verify (a5[0][0][0].toString(),new Date(2001,8,11),"5.4 Wrong value","");


	apInitScenario("6.  var a : Date[] = new Date[x]");
		var a6 : Date[] = new Date[4];
		a6[0] = new Date(2001,8,11);
		a6[1] = new Date(2001,8,11);
		a6[2] = new Date(2001,8,11);
		a6[3] = new Date(2001,8,11);
		verify (a6.GetType(),"Microsoft.JScript.DateObject[]","6.1 Wrong type","");
		verify (a6[0].GetType(),"Microsoft.JScript.DateObject","6.2 Wrong type","");
		verify (a6[2].GetType(),"Microsoft.JScript.DateObject","6.3 Wrong type","");
		verify (a6[0].toString(),new Date(2001,8,11),"6.4 Wrong value","");
		verify (a6[2].toString(),new Date(2001,8,11),"6.5 Wrong value","");
		a6[2] = new Date(2001,8,11);
		verify (a6[2].toString(),new Date(2001,8,11),"6.6 Wrong value","");


	apInitScenario("7.  var a : Date[,] = new Date[x,y]");
		var a7 : Date[,] = new Date[1,4];
		a7[0,0] = new Date(2001,8,11);
		a7[0,1] = new Date(2001,8,11);
		a7[0,2] = new Date(2001,8,11);
		a7[0,3] = new Date(2001,8,11);
		verify (a7.GetType(),"Microsoft.JScript.DateObject[,]","7.1 Wrong type","");
		verify (a7[0,2].GetType(),"Microsoft.JScript.DateObject","7.3 Wrong type","");
		verify (a7[0,0].toString(),new Date(2001,8,11),"7.4 Wrong value","");
		verify (a7[0,2].toString(),new Date(2001,8,11),"7.5 Wrong value","");
		a7[0,2] = new Date(2001,8,11);
		verify (a7[0,2].toString(),new Date(2001,8,11),"7.6 Wrong value","");


	apInitScenario("8.  var a : Date[,,] = new Date[x,y,z]");
		var a8 : Date[,,] = new Date[1,1,4];
		a8[0,0,0] = new Date(2001,8,11);
		a8[0,0,1] = new Date(2001,8,11);
		a8[0,0,2] = new Date(2001,8,11);
		a8[0,0,3] = new Date(2001,8,11);
		verify (a8.GetType(),"Microsoft.JScript.DateObject[,,]","8.1 Wrong type","");
		verify (a8[0,0,2].GetType(),"Microsoft.JScript.DateObject","8.3 Wrong type","");
		verify (a8[0,0,0].toString(),new Date(2001,8,11),"8.4 Wrong value","");
		verify (a8[0,0,2].toString(),new Date(2001,8,11),"8.5 Wrong value","");
		a8[0,0,2] = new Date(2001,8,11);
		verify (a8[0,0,2].toString(),new Date(2001,8,11),"8.6 Wrong value","");


	apInitScenario("9.  var a : Date[,,,] = new Date[w,x,y,z]");
		var a9 : Date[,,,] = new Date[1,1,1,4];
		a9[0,0,0,0] = new Date(2001,8,11);
		a9[0,0,0,1] = new Date(2001,8,11);
		a9[0,0,0,2] = new Date(2001,8,11);
		a9[0,0,0,3] = new Date(2001,8,11);
		verify (a9.GetType(),"Microsoft.JScript.DateObject[,,,]","9.1 Wrong type","");
		verify (a9[0,0,0,2].GetType(),"Microsoft.JScript.DateObject","9.3 Wrong type","");
		verify (a9[0,0,0,0].toString(),new Date(2001,8,11),"9.4 Wrong value","");
		verify (a9[0,0,0,2].toString(),new Date(2001,8,11),"9.5 Wrong value","");
		a9[0,0,0,2] = new Date(2001,8,11);
		verify (a9[0,0,0,2].toString(),new Date(2001,8,11),"9.6 Wrong value","");


	apEndTest();
}


typarr11();


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
