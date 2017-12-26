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


var iTestID = 220488;

/* -------------------------------------------------------------------------
  Test: 	TYPARR05
   
  
 
  Component:	JScript
 
  Major Area:	Typed Arrays
 
  Test Area:	Matrix testing arrays of different types
 
  Keywords:	
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

			1.  var a : boolean[];

			2.  var a : boolean[][];

			3.	var a : boolean[] = [x];

			4.  var a : boolean[][] = [x][y];

			5.  var a : boolean[][][] = [x][y][z];

			6.  var a : boolean[] = new boolean[x];

			7.  var a : boolean[,] = new boolean[x,y];

			8.  var a : boolean[,,] = new boolean[x,y,z];

			9.  var a : boolean[,,,] = new boolean[w,x,y,z];


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


function typarr05() {

    apInitTest("typarr05: Typed Arrays -- boolean"); 

	var result;

	apInitScenario("1.  var a : boolean[]");
		var a1 : boolean[] = [true,true,false,true];
		verify (a1.GetType(),"System.Boolean[]","1.1 Wrong type","");
		verify (a1[0].GetType(),"System.Boolean","1.2 Wrong type","");
		verify (a1[2].GetType(),"System.Boolean","1.3 Wrong type","");
		verify (a1[0],true,"1.4 Wrong value","");
		verify (a1[2],false,"1.5 Wrong value","");
		a1[2] = true;
		verify (a1[2],true,"1.6 Wrong value","");


	apInitScenario("2.  var a : boolean[][]");
		var a2 : boolean[][] = [[true,false,true],[false,true,false],[true,true,false]];
		verify (a2.GetType(),"System.Boolean[][]","2.1 Wrong type","");
		verify (a2[0].GetType(),"System.Boolean[]","2.2 Wrong type","");
		verify (a2[1][2].GetType(),"System.Boolean","2.3 Wrong type","");
		verify (a2[1][2],false,"2.4 Wrong value","");
		a2[1][2] = true;
		verify (a2[1][2],true,"2.5 Wrong value","");


	apInitScenario("3.  var a : boolean[] = [x]");
		var x3 : boolean = true;
		var a3 : boolean[] = [x3];
		a3[0] = false;
		verify (a3.GetType(),"System.Boolean[]","3.1 Wrong type","");
		verify (a3[0].GetType(),"System.Boolean","3.2 Wrong type","");
		verify (a3[0].GetType(),"System.Boolean","3.3 Wrong type","");
		verify (a3[0],false,"3.4 Wrong value","");
		var x3a : boolean[] = [true,true,true];
		a3 = x3a;
		verify (a3[2],true,"3.5 Wrong value","");
		

	apInitScenario("4.  var a : boolean[][] = [x][y]");
		var x4 : boolean = true;
		var y4 : boolean = false;
		var a4 : boolean[][] = [[x4],[y4]];
		verify (a4.GetType(),"System.Boolean[][]","2.1 Wrong type","");
		verify (a4[0].GetType(),"System.Boolean[]","2.2 Wrong type","");
		verify (a4[0][0].GetType(),"System.Boolean","2.3 Wrong type","");
		verify (a4[0][0],true,"2.4 Wrong value","");


	apInitScenario("5.  var a : boolean[][][] = [x][y][z]");
		var x5 : boolean = true;
		var y5 : boolean = false;
		var z5 : boolean = true;
		var a5 : boolean[][][] = [[[x5],[y5],[z5]]];
		verify (a5.GetType(),"System.Boolean[][][]","5.1 Wrong type","");
		verify (a5[0].GetType(),"System.Boolean[][]","5.2 Wrong type","");
		verify (a5[0][0].GetType(),"System.Boolean[]","5.3 Wrong type","");
		verify (a5[0][0][0],true,"5.4 Wrong value","");


	apInitScenario("6.  var a : boolean[] = new boolean[x]");
		var a6 : boolean[] = new boolean[4];
		a6[0] = true;
		a6[1] = true;
		a6[2] = false;
		a6[3] = true;
		verify (a6.GetType(),"System.Boolean[]","6.1 Wrong type","");
		verify (a6[0].GetType(),"System.Boolean","6.2 Wrong type","");
		verify (a6[2].GetType(),"System.Boolean","6.3 Wrong type","");
		verify (a6[0],true,"6.4 Wrong value","");
		verify (a6[2],false,"6.5 Wrong value","");
		a6[2] = true;
		verify (a6[2],true,"6.6 Wrong value","");


	apInitScenario("7.  var a : boolean[,] = new boolean[x,y]");
		var a7 : boolean[,] = new boolean[1,4];
		a7[0,0] = true;
		a7[0,1] = true;
		a7[0,2] = false;
		a7[0,3] = true;
		verify (a7.GetType(),"System.Boolean[,]","7.1 Wrong type","");
		verify (a7[0,2].GetType(),"System.Boolean","7.3 Wrong type","");
		verify (a7[0,0],true,"7.4 Wrong value","");
		verify (a7[0,2],false,"7.5 Wrong value","");
		a7[0,2] = true;
		verify (a7[0,2],true,"7.6 Wrong value","");


	apInitScenario("8.  var a : boolean[,,] = new boolean[x,y,z]");
		var a8 : boolean[,,] = new boolean[1,1,4];
		a8[0,0,0] = true;
		a8[0,0,1] = true;
		a8[0,0,2] = false;
		a8[0,0,3] = true;
		verify (a8.GetType(),"System.Boolean[,,]","8.1 Wrong type","");
		verify (a8[0,0,2].GetType(),"System.Boolean","8.3 Wrong type","");
		verify (a8[0,0,0],true,"8.4 Wrong value","");
		verify (a8[0,0,2],false,"8.5 Wrong value","");
		a8[0,0,2] = true;
		verify (a8[0,0,2],true,"8.6 Wrong value","");


	apInitScenario("9.  var a : boolean[,,,] = new boolean[w,x,y,z]");
		var a9 : boolean[,,,] = new boolean[1,1,1,4];
		a9[0,0,0,0] = true;
		a9[0,0,0,1] = true;
		a9[0,0,0,2] = false;
		a9[0,0,0,3] = true;
		verify (a9.GetType(),"System.Boolean[,,,]","9.1 Wrong type","");
		verify (a9[0,0,0,2].GetType(),"System.Boolean","9.3 Wrong type","");
		verify (a9[0,0,0,0],true,"9.4 Wrong value","");
		verify (a9[0,0,0,2],false,"9.5 Wrong value","");
		a9[0,0,0,2] = true;
		verify (a9[0,0,0,2],true,"9.6 Wrong value","");


	apEndTest();
}


typarr05();


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
