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


var iTestID = 220505;


/* -------------------------------------------------------------------------
  Test: 	TYPARR22
   
  
 
  Component:	JScript
 
  Major Area:	Typed Foos
 
  Test Area:	Matrix testing arrays of different types
 
  Keywords:	
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

			1.  var a : Foo[];

			2.  var a : Foo[][];

			3.	var a : Foo[] = [x];

			4.  var a : Foo[][] = [x][y];

			5.  var a : Foo[][][] = [x][y][z];

			6.  var a : Foo[] = new Foo[x];

			7.  var a : Foo[,] = new Foo[x,y];

			8.  var a : Foo[,,] = new Foo[x,y,z];

			9.  var a : Foo[,,,] = new Foo[w,x,y,z];


  Abstract:	 Testing that single, jagged, and multi-dimensional arrays
			 can be created and accessed.
 ---------------------------------------------------------------------------
  Category:			Fooality
 
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

class Foo {

	public var bar;

	function Foo() {
		bar = 3;
	}
	
	function Foo(x) {
		bar = x;
	}
}




@if(!@aspx)
	import System
@end


function typarr22() {

    apInitTest("typarr22: Typed Foos -- Foo"); 

	var s1 : Foo = new Foo();
	var s2 : Foo = new Foo(42);

	apInitScenario("1.  var a : Foo[]");
		var a1 : Foo[] = [s1,s1,s1,s1];
	
	@if(!@aspx)
		verify (a1.GetType(),"Foo[]","1.1 Wrong type","");
		verify (a1[0].GetType(),"Foo","1.2 Wrong type","");
		verify (a1[2].GetType(),"Foo","1.3 Wrong type","");
		verify (a1[0],"Foo","1.4 Wrong value","");
		verify (a1[2],"Foo","1.5 Wrong value","");
		a1[2] = s2;
		verify (a1[2],"Foo","1.6 Wrong value","");
	@else
		verify (a1.GetType(),"ASP.typarr22_aspx+Foo[]","1.1 Wrong type","");
		verify (a1[0].GetType(),"ASP.typarr22_aspx+Foo","1.2 Wrong type","");
		verify (a1[2].GetType(),"ASP.typarr22_aspx+Foo","1.3 Wrong type","");
		verify (a1[0],"ASP.typarr22_aspx+Foo","1.4 Wrong value","");
		verify (a1[2],"ASP.typarr22_aspx+Foo","1.5 Wrong value","");
		a1[2] = s2;
		verify (a1[2],"ASP.typarr22_aspx+Foo","1.6 Wrong value","");
	@end

	apInitScenario("2.  var a : Foo[][]");
		var a2 : Foo[][] = [[s1,s1,s1],[s1,s1,s1],[s1,s1,s1]];
	
	@if(!@aspx)
		verify (a2.GetType(),"Foo[][]","2.1 Wrong type","");
		verify (a2[0].GetType(),"Foo[]","2.2 Wrong type","");
		verify (a2[1][2].GetType(),"Foo","2.3 Wrong type","");
		verify (a2[1][2],"Foo","2.4 Wrong value","");
		a2[1][2] = s2;
		verify (a2[1][2],"Foo","2.5 Wrong value","");
	@else
		verify (a2.GetType(),"ASP.typarr22_aspx+Foo[][]","2.1 Wrong type","");
		verify (a2[0].GetType(),"ASP.typarr22_aspx+Foo[]","2.2 Wrong type","");
		verify (a2[1][2].GetType(),"ASP.typarr22_aspx+Foo","2.3 Wrong type","");
		verify (a2[1][2],"ASP.typarr22_aspx+Foo","2.4 Wrong value","");
		a2[1][2] = s2;
		verify (a2[1][2],"ASP.typarr22_aspx+Foo","2.5 Wrong value","");
	@end

	apInitScenario("3.  var a : Foo[] = [x]");
		var x3 : Foo = s1;
		var a3 : Foo[] = [x3];
		a3[0] = s1;
	
	@if(!@aspx)
		verify (a3.GetType(),"Foo[]","3.1 Wrong type","");
		verify (a3[0].GetType(),"Foo","3.2 Wrong type","");
		verify (a3[0].GetType(),"Foo","3.3 Wrong type","");
		verify (a3[0],"Foo","3.4 Wrong value","");
		var x3a : Foo[] = [s1,s1,s1];
		a3 = x3a;
		verify (a3[2],"Foo","3.5 Wrong value","");
	@else
		verify (a3.GetType(),"ASP.typarr22_aspx+Foo[]","3.1 Wrong type","");
		verify (a3[0].GetType(),"ASP.typarr22_aspx+Foo","3.2 Wrong type","");
		verify (a3[0].GetType(),"ASP.typarr22_aspx+Foo","3.3 Wrong type","");
		verify (a3[0],"ASP.typarr22_aspx+Foo","3.4 Wrong value","");
		var x3a : Foo[] = [s1,s1,s1];
		a3 = x3a;
		verify (a3[2],"ASP.typarr22_aspx+Foo","3.5 Wrong value","");
	@end


	apInitScenario("4.  var a : Foo[][] = [x][y]");
		var x4 : Foo = s1;
		var y4 : Foo = s1;
		var a4 : Foo[][] = [[x4],[y4]];
	
	@if(!@aspx)
		verify (a4.GetType(),"Foo[][]","2.1 Wrong type","");
		verify (a4[0].GetType(),"Foo[]","2.2 Wrong type","");
		verify (a4[0][0].GetType(),"Foo","2.3 Wrong type","");
		verify (a4[0][0],"Foo","2.4 Wrong value","");
	@else
		verify (a4.GetType(),"ASP.typarr22_aspx+Foo[][]","2.1 Wrong type","");
		verify (a4[0].GetType(),"ASP.typarr22_aspx+Foo[]","2.2 Wrong type","");
		verify (a4[0][0].GetType(),"ASP.typarr22_aspx+Foo","2.3 Wrong type","");
		verify (a4[0][0],"ASP.typarr22_aspx+Foo","2.4 Wrong value","");
	@end


	apInitScenario("5.  var a : Foo[][][] = [x][y][z]");
		var x5 : Foo = s1;
		var y5 : Foo = s1;
		var z5 : Foo = s1;
		var a5 : Foo[][][] = [[[x5],[y5],[z5]]];
	
	@if(!@aspx)
		verify (a5.GetType(),"Foo[][][]","5.1 Wrong type","");
		verify (a5[0].GetType(),"Foo[][]","5.2 Wrong type","");
		verify (a5[0][0].GetType(),"Foo[]","5.3 Wrong type","");
		verify (a5[0][0][0],"Foo","5.4 Wrong value","");
	@else
		verify (a5.GetType(),"ASP.typarr22_aspx+Foo[][][]","5.1 Wrong type","");
		verify (a5[0].GetType(),"ASP.typarr22_aspx+Foo[][]","5.2 Wrong type","");
		verify (a5[0][0].GetType(),"ASP.typarr22_aspx+Foo[]","5.3 Wrong type","");
		verify (a5[0][0][0],"ASP.typarr22_aspx+Foo","5.4 Wrong value","");
	@end


	apInitScenario("6.  var a : Foo[] = new Foo[x]");
		var a6 : Object[] = new Object[4];
		a6[0] = s1;
		a6[1] = s1;
		a6[2] = s1;
		a6[3] = s1;
	
	@if(!@aspx)
		verify (a6.GetType(),"System.Object[]","6.1 Wrong type","");
		verify (a6[0].GetType(),"Foo","6.2 Wrong type","");
		verify (a6[2].GetType(),"Foo","6.3 Wrong type","");
		verify (a6[0],"Foo","6.4 Wrong value","");
		verify (a6[2],"Foo","6.5 Wrong value","");
		a6[2] = s2;
		verify (a6[2],"Foo","6.6 Wrong value","");
	@else
		verify (a6.GetType(),"System.Object[]","6.1 Wrong type","");
		verify (a6[0].GetType(),"ASP.typarr22_aspx+Foo","6.2 Wrong type","");
		verify (a6[2].GetType(),"ASP.typarr22_aspx+Foo","6.3 Wrong type","");
		verify (a6[0],"ASP.typarr22_aspx+Foo","6.4 Wrong value","");
		verify (a6[2],"ASP.typarr22_aspx+Foo","6.5 Wrong value","");
		a6[2] = s2;
		verify (a6[2],"ASP.typarr22_aspx+Foo","6.6 Wrong value","");
	@end


	apInitScenario("7.  var a : Foo[,] = new Foo[x,y]");
		var a7 : Object[,] = new Object[1,4];
		a7[0,0] = s1;
		a7[0,1] = s1;
		a7[0,2] = s1;
		a7[0,3] = s1;
	
	@if(!@aspx)
		verify (a7.GetType(),"System.Object[,]","7.1 Wrong type","");
		verify (a7[0,2].GetType(),"Foo","7.3 Wrong type","");
		verify (a7[0,0],"Foo","7.4 Wrong value","");
		verify (a7[0,2],"Foo","7.5 Wrong value","");
		a7[0,2] = s2;
		verify (a7[0,2],"Foo","7.3 Wrong value","");
	@else
		verify (a7.GetType(),"System.Object[,]","7.1 Wrong type","");
		verify (a7[0,2].GetType(),"ASP.typarr22_aspx+Foo","7.3 Wrong type","");
		verify (a7[0,0],"ASP.typarr22_aspx+Foo","7.4 Wrong value","");
		verify (a7[0,2],"ASP.typarr22_aspx+Foo","7.5 Wrong value","");
		a7[0,2] = s2;
		verify (a7[0,2],"ASP.typarr22_aspx+Foo","7.3 Wrong value","");
	@end


	apInitScenario("8.  var a : Foo[,,] = new Foo[x,y,z]");
		var a8 : Object[,,] = new Object[1,1,4];
		a8[0,0,0] = s1;
		a8[0,0,1] = s1;
		a8[0,0,2] = s1;
		a8[0,0,3] = s1;
	
	@if(!@aspx)
		verify (a8.GetType(),"System.Object[,,]","8.1 Wrong type","");
		verify (a8[0,0,2].GetType(),"Foo","8.3 Wrong type","");
		verify (a8[0,0,0],"Foo","8.4 Wrong value","");
		verify (a8[0,0,2],"Foo","8.5 Wrong value","");
		a8[0,0,2] = s2;
		verify (a8[0,0,2],"Foo","8.3 Wrong value","");
	@else
		verify (a8.GetType(),"System.Object[,,]","8.1 Wrong type","");
		verify (a8[0,0,2].GetType(),"ASP.typarr22_aspx+Foo","8.3 Wrong type","");
		verify (a8[0,0,0],"ASP.typarr22_aspx+Foo","8.4 Wrong value","");
		verify (a8[0,0,2],"ASP.typarr22_aspx+Foo","8.5 Wrong value","");
		a8[0,0,2] = s2;
		verify (a8[0,0,2],"ASP.typarr22_aspx+Foo","8.3 Wrong value","");
	@end


	apInitScenario("9.  var a : Foo[,,,] = new Foo[w,x,y,z]");
		var a9 : Object[,,,] = new Object[1,1,1,4];
		a9[0,0,0,0] = s1;
		a9[0,0,0,1] = s1;
		a9[0,0,0,2] = s1;
		a9[0,0,0,3] = s1;
	
	@if(!@aspx)
		verify (a9.GetType(),"System.Object[,,,]","9.1 Wrong type","");
		verify (a9[0,0,0,2].GetType(),"Foo","9.3 Wrong type","");
		verify (a9[0,0,0,0],"Foo","9.4 Wrong value","");
		verify (a9[0,0,0,2],"Foo","9.5 Wrong value","");
		a9[0,0,0,2] = s2;
		verify (a9[0,0,0,2],"Foo","9.3 Wrong value","");
	@else
		verify (a9.GetType(),"System.Object[,,,]","9.1 Wrong type","");
		verify (a9[0,0,0,2].GetType(),"ASP.typarr22_aspx+Foo","9.3 Wrong type","");
		verify (a9[0,0,0,0],"ASP.typarr22_aspx+Foo","9.4 Wrong value","");
		verify (a9[0,0,0,2],"ASP.typarr22_aspx+Foo","9.5 Wrong value","");
		a9[0,0,0,2] = s2;
		verify (a9[0,0,0,2],"ASP.typarr22_aspx+Foo","9.3 Wrong value","");
	@end


	apEndTest();
}

typarr22();


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
