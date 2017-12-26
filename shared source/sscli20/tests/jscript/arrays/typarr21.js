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


var iTestID = 220504;

/* -------------------------------------------------------------------------
  Test: 	TYPARR21
   
  
 
  Component:	JScript
 
  Major Area:	Typed System.EventArgss
 
  Test Area:	Matrix testing arrays of different types
 
  Keywords:	
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

			1.  var a : System.EventArgs[];

			2.  var a : System.EventArgs[][];

			3.	var a : System.EventArgs[] = [x];

			4.  var a : System.EventArgs[][] = [x][y];

			5.  var a : System.EventArgs[][][] = [x][y][z];

			6.  var a : System.EventArgs[] = new System.EventArgs[x];

			7.  var a : System.EventArgs[,] = new System.EventArgs[x,y];

			8.  var a : System.EventArgs[,,] = new System.EventArgs[x,y,z];

			9.  var a : System.EventArgs[,,,] = new System.EventArgs[w,x,y,z];


  Abstract:	 Testing that single, jagged, and multi-dimensional arrays
			 can be created and accessed.
 ---------------------------------------------------------------------------
  Category:			System.EventArgsality
 
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


function typarr21() {

    apInitTest("typarr21: Typed System.EventArgss -- System.EventArgs"); 

	var s1 : System.EventArgs = new System.EventArgs();
	var s2 : System.EventArgs = new System.EventArgs();

	apInitScenario("1.  var a : System.EventArgs[]");
		var a1 : System.EventArgs[] = [s1,s1,s1,s1];
		verify (a1.GetType(),"System.EventArgs[]","1.1 Wrong type","");
		verify (a1[0].GetType(),"System.EventArgs","1.2 Wrong type","");
		verify (a1[2].GetType(),"System.EventArgs","1.3 Wrong type","");
		verify (a1[0],"System.EventArgs","1.4 Wrong value","");
		verify (a1[2],"System.EventArgs","1.5 Wrong value","");
		a1[2] = s2;
		verify (a1[2],"System.EventArgs","1.3 Wrong value","");


	apInitScenario("2.  var a : System.EventArgs[][]");
		var a2 : System.EventArgs[][] = [[s1,s1,s1],[s1,s1,s1],[s1,s1,s1]];
		verify (a2.GetType(),"System.EventArgs[][]","2.1 Wrong type","");
		verify (a2[0].GetType(),"System.EventArgs[]","2.2 Wrong type","");
		verify (a2[1][2].GetType(),"System.EventArgs","2.3 Wrong type","");
		verify (a2[1][2],"System.EventArgs","2.4 Wrong value","");
		a2[1][2] = s2;
		verify (a2[1][2],"System.EventArgs","2.5 Wrong value","");


	apInitScenario("3.  var a : System.EventArgs[] = [x]");
		var x3 : System.EventArgs = s1;
		var a3 : System.EventArgs[] = [x3];
		a3[0] = s1;
		verify (a3.GetType(),"System.EventArgs[]","3.1 Wrong type","");
		verify (a3[0].GetType(),"System.EventArgs","3.2 Wrong type","");
		verify (a3[0].GetType(),"System.EventArgs","3.3 Wrong type","");
		verify (a3[0],"System.EventArgs","3.4 Wrong value","");
		var x3a : System.EventArgs[] = [s1,s1,s1];
		a3 = x3a;
		verify (a3[2],"System.EventArgs","3.5 Wrong value","");
		

	apInitScenario("4.  var a : System.EventArgs[][] = [x][y]");
		var x4 : System.EventArgs = s1;
		var y4 : System.EventArgs = s1;
		var a4 : System.EventArgs[][] = [[x4],[y4]];
		verify (a4.GetType(),"System.EventArgs[][]","2.1 Wrong type","");
		verify (a4[0].GetType(),"System.EventArgs[]","2.2 Wrong type","");
		verify (a4[0][0].GetType(),"System.EventArgs","2.3 Wrong type","");
		verify (a4[0][0],"System.EventArgs","2.4 Wrong value","");


	apInitScenario("5.  var a : System.EventArgs[][][] = [x][y][z]");
		var x5 : System.EventArgs = s1;
		var y5 : System.EventArgs = s1;
		var z5 : System.EventArgs = s1;
		var a5 : System.EventArgs[][][] = [[[x5],[y5],[z5]]];
		verify (a5.GetType(),"System.EventArgs[][][]","5.1 Wrong type","");
		verify (a5[0].GetType(),"System.EventArgs[][]","5.2 Wrong type","");
		verify (a5[0][0].GetType(),"System.EventArgs[]","5.3 Wrong type","");
		verify (a5[0][0][0],"System.EventArgs","5.4 Wrong value","");


	apInitScenario("6.  var a : System.EventArgs[] = new System.EventArgs[x]");
		var a6 : Object[] = new Object[4];
		a6[0] = s1;
		a6[1] = s1;
		a6[2] = s1;
		a6[3] = s1;
		verify (a6.GetType(),"System.Object[]","6.1 Wrong type","");
		verify (a6[0].GetType(),"System.EventArgs","6.2 Wrong type","");
		verify (a6[2].GetType(),"System.EventArgs","6.3 Wrong type","");
		verify (a6[0],"System.EventArgs","6.4 Wrong value","");
		verify (a6[2],"System.EventArgs","6.5 Wrong value","");
		a6[2] = s2;
		verify (a6[2],"System.EventArgs","6.6 Wrong value","");


	apInitScenario("7.  var a : System.EventArgs[,] = new System.EventArgs[x,y]");
		var a7 : Object[,] = new Object[1,4];
		a7[0,0] = s1;
		a7[0,1] = s1;
		a7[0,2] = s1;
		a7[0,3] = s1;
		verify (a7.GetType(),"System.Object[,]","7.1 Wrong type","");
		verify (a7[0,2].GetType(),"System.EventArgs","7.3 Wrong type","");
		verify (a7[0,0],"System.EventArgs","7.4 Wrong value","");
		verify (a7[0,2],"System.EventArgs","7.5 Wrong value","");
		a7[0,2] = s2;
		verify (a7[0,2],"System.EventArgs","7.3 Wrong value","");


	apInitScenario("8.  var a : System.EventArgs[,,] = new System.EventArgs[x,y,z]");
		var a8 : Object[,,] = new Object[1,1,4];
		a8[0,0,0] = s1;
		a8[0,0,1] = s1;
		a8[0,0,2] = s1;
		a8[0,0,3] = s1;
		verify (a8.GetType(),"System.Object[,,]","8.1 Wrong type","");
		verify (a8[0,0,2].GetType(),"System.EventArgs","8.3 Wrong type","");
		verify (a8[0,0,0],"System.EventArgs","8.4 Wrong value","");
		verify (a8[0,0,2],"System.EventArgs","8.5 Wrong value","");
		a8[0,0,2] = s2;
		verify (a8[0,0,2],"System.EventArgs","8.3 Wrong value","");


	apInitScenario("9.  var a : System.EventArgs[,,,] = new System.EventArgs[w,x,y,z]");
		var a9 : Object[,,,] = new Object[1,1,1,4];
		a9[0,0,0,0] = s1;
		a9[0,0,0,1] = s1;
		a9[0,0,0,2] = s1;
		a9[0,0,0,3] = s1;
		verify (a9.GetType(),"System.Object[,,,]","9.1 Wrong type","");
		verify (a9[0,0,0,2].GetType(),"System.EventArgs","9.3 Wrong type","");
		verify (a9[0,0,0,0],"System.EventArgs","9.4 Wrong value","");
		verify (a9[0,0,0,2],"System.EventArgs","9.5 Wrong value","");
		a9[0,0,0,2] = s2;
		verify (a9[0,0,0,2],"System.EventArgs","9.3 Wrong value","");


	apEndTest();
}


typarr21();


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
