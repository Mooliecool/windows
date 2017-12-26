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


var iTestID = 220490;

/* -------------------------------------------------------------------------
  Test: 	TYPARR07
   
  
 
  Component:	JScript
 
  Major Area:	Typed Arrays
 
  Test Area:	Matrix testing arrays of different types
 
  Keywords:	
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

			1.  var a : byte[];

			2.  var a : byte[][];

			3.	var a : byte[] = [x];

			4.  var a : byte[][] = [x][y];

			5.  var a : byte[][][] = [x][y][z];

			6.  var a : byte[] = new byte[x];

			7.  var a : byte[,] = new byte[x,y];

			8.  var a : byte[,,] = new byte[x,y,z];

			9.  var a : byte[,,,] = new byte[w,x,y,z];


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


function typarr07() {

    apInitTest("typarr07: Typed Arrays -- byte"); 

	var result;

	apInitScenario("1.  var a : byte[]");
		var a1 : byte[] = [1,2,3,4];
		verify (a1.GetType(),"System.Byte[]","1.1 Wrong type","");
		verify (a1[0].GetType(),"System.Byte","1.2 Wrong type","");
		verify (a1[2].GetType(),"System.Byte","1.3 Wrong type","");
		verify (a1[0],1,"1.4 Wrong value","");
		verify (a1[2],3,"1.5 Wrong type","");
		a1[2] = 42;
		verify (a1[2],42,"1.6 Wrong type","");


	apInitScenario("2.  var a : byte[][]");
		var a2 : byte[][] = [[1,2,3],[4,5,6],[7,8,9]];
		verify (a2.GetType(),"System.Byte[][]","2.1 Wrong type","");
		verify (a2[0].GetType(),"System.Byte[]","2.2 Wrong type","");
		verify (a2[1][2].GetType(),"System.Byte","2.3 Wrong type","");
		verify (a2[1][2],6,"2.4 Wrong value","");
		a2[1][2] = 42;
		verify (a2[1][2],42,"2.5 Wrong type","");


	apInitScenario("3.  var a : byte[] = [x]");
		var x3 : byte = 42;
		var a3 : byte[] = [x3];
		a3[0] = 42;
		verify (a3.GetType(),"System.Byte[]","3.1 Wrong type","");
		verify (a3[0].GetType(),"System.Byte","3.2 Wrong type","");
		verify (a3[0].GetType(),"System.Byte","3.3 Wrong type","");
		verify (a3[0],42,"3.4 Wrong value","");
		var x3a : byte[] = [1,2,3];
		a3 = x3a;
		verify (a3[2],3,"3.5 Wrong type","");
		

	apInitScenario("4.  var a : byte[][] = [x][y]");
		var x4 : byte = 2;
		var y4 : byte = 3;
		var a4 : byte[][] = [[x4],[y4]];
		verify (a4.GetType(),"System.Byte[][]","2.1 Wrong type","");
		verify (a4[0].GetType(),"System.Byte[]","2.2 Wrong type","");
		verify (a4[0][0].GetType(),"System.Byte","2.3 Wrong type","");
		verify (a4[0][0],2,"2.4 Wrong value","");


	apInitScenario("5.  var a : byte[][][] = [x][y][z]");
		var x5 : byte = 2;
		var y5 : byte = 3;
		var z5 : byte = 4;
		var a5 : byte[][][] = [[[x5],[y5],[z5]]];
		verify (a5.GetType(),"System.Byte[][][]","5.1 Wrong type","");
		verify (a5[0].GetType(),"System.Byte[][]","5.2 Wrong type","");
		verify (a5[0][0].GetType(),"System.Byte[]","5.3 Wrong type","");
		verify (a5[0][0][0],2,"5.4 Wrong value","");


	apInitScenario("6.  var a : byte[] = new byte[x]");
		var a6 : byte[] = new byte[4];
		a6[0] = 1;
		a6[1] = 2;
		a6[2] = 3;
		a6[3] = 4;
		verify (a6.GetType(),"System.Byte[]","6.1 Wrong type","");
		verify (a6[0].GetType(),"System.Byte","6.2 Wrong type","");
		verify (a6[2].GetType(),"System.Byte","6.3 Wrong type","");
		verify (a6[0],1,"6.4 Wrong value","");
		verify (a6[2],3,"6.5 Wrong type","");
		a6[2] = 42;
		verify (a6[2],42,"6.6 Wrong type","");


	apInitScenario("7.  var a : byte[,] = new byte[x,y]");
		var a7 : byte[,] = new byte[1,4];
		a7[0,0] = 1;
		a7[0,1] = 2;
		a7[0,2] = 3;
		a7[0,3] = 4;
		verify (a7.GetType(),"System.Byte[,]","7.1 Wrong type","");
		verify (a7[0,2].GetType(),"System.Byte","7.3 Wrong type","");
		verify (a7[0,0],1,"7.4 Wrong value","");
		verify (a7[0,2],3,"7.5 Wrong type","");
		a7[0,2] = 42;
		verify (a7[0,2],42,"7.6 Wrong type","");


	apInitScenario("8.  var a : byte[,,] = new byte[x,y,z]");
		var a8 : byte[,,] = new byte[1,1,4];
		a8[0,0,0] = 1;
		a8[0,0,1] = 2;
		a8[0,0,2] = 3;
		a8[0,0,3] = 4;
		verify (a8.GetType(),"System.Byte[,,]","8.1 Wrong type","");
		verify (a8[0,0,2].GetType(),"System.Byte","8.3 Wrong type","");
		verify (a8[0,0,0],1,"8.4 Wrong value","");
		verify (a8[0,0,2],3,"8.5 Wrong type","");
		a8[0,0,2] = 42;
		verify (a8[0,0,2],42,"8.6 Wrong type","");


	apInitScenario("9.  var a : byte[,,,] = new byte[w,x,y,z]");
		var a9 : byte[,,,] = new byte[1,1,1,4];
		a9[0,0,0,0] = 1;
		a9[0,0,0,1] = 2;
		a9[0,0,0,2] = 3;
		a9[0,0,0,3] = 4;
		verify (a9.GetType(),"System.Byte[,,,]","9.1 Wrong type","");
		verify (a9[0,0,0,2].GetType(),"System.Byte","9.3 Wrong type","");
		verify (a9[0,0,0,0],1,"9.4 Wrong value","");
		verify (a9[0,0,0,2],3,"9.5 Wrong value","");
		a9[0,0,0,2] = 42;
		verify (a9[0,0,0,2],42,"9.6 Wrong value","");


	apEndTest();
}


typarr07();


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
