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


var iTestID = 170668;

/* -------------------------------------------------------------------------
  Test: 	regressiontest1
   
  Product:	JScript
 
  Area:		JS-Classes
 
  Purpose:	bug regression  
 
  Scenarios: 	94541, 94544, 172007
		
  Notes: 	Use Hashtable instead of Dictionary in bug 94541 since no Dictionary anymore.
 ---------------------------------------------------------------------------
  
 
	[00]	02-April-2001	    qiongou: Created Test
 -------------------------------------------------------------------------*/

@if(!@aspx)
	import System.Collections
@else
	</script>
	<%@ import namespace="System.Collections" %>
	<script language=jscript runat=server>
@end


class foo
{
	public var Stuff: Hashtable = new Hashtable();
}

class Bar
{
        function get prop() : String
        {
                return "Hello";
        }
}


function verify(sAct, sExp, sMes, sBug){
	if (sAct != sExp)
        apLogFailInfo( "*** Failed: "+sMes+" ", sExp+" ", sAct, sBug);
}

function regressiontest1()
{
	apInitTest("regressiontest1");

	apInitScenario ("Default indexed properties not working for class members");
	var f: foo = new foo();
	f.Stuff("bar") = 42;	
	verify(f.Stuff("bar"), 42, "Default indexed properties not working for class members", "94541");	

	apInitScenario ("Default indexed properties not working on Dictionaries");
	var d : Hashtable = new Hashtable();
	d("bar") = 42;
	verify(d("bar"), 42, "Default indexed properties not working on Dictionaries", "94544");
		

	apInitScenario("Can't access properties of JScript classes ");	
	var b : Bar = new Bar();
	verify(b.prop, "Hello", "Can't access properties of JScript classes ", "172007");

	apEndTest();
}



regressiontest1();


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
