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


var iTestID = 160221;

/* -------------------------------------------------------------------------
  Test: 	TYPANN21
   
  
 
  Component:	JScript
 
  Major Area:	Type Annotation
 
  Test Area:	Type annotation for ActiveXObject
 
  Keywords:	type annotation annotate object activexobject
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of new or changed features
 
  Scenarios:

		1.  function foo (I : int, T)

		2.  function foo (l : long, T)

		3.  function foo (f : float, T)

		4.  function foo (d : double, T)

		5.  function foo (fso : ActiveXObject, T)

		6.  function foo (arr : Array, T)

		7.  function foo (b : Boolean, T)

		8.  function foo (d : Date, T)

		9.  function foo (enum : Enumerator, T)

		10. function foo (e : Error, T)

		11. function foo (func : Function, T)

		12. function foo (n : Number, T)

		13. function foo (re : RegExp, T)

		14. function foo (str : String, T)

		15. function foo (o : Person, T)

		16. function foo (i16 : int16, T)

		17. function foo (strS : System.String, T)


  Abstract:	 Testing variable assignment through type annotation..
 ---------------------------------------------------------------------------
  Category:			Functionality
 
  Product:			JScript
 
  Related Files: 
 
  Notes:
 ---------------------------------------------------------------------------
  
 


-------------------------------------------------------------------------*/

import System

/*----------
/
/  Helper functions
/
----------*/


function verify(sAct, sExp, sMes, sBug){
	if (sAct != sExp)
        apLogFailInfo( "*** Scenario failed: "+sMes, sExp, sAct, sBug);
}


function ReturnSysStringVal() {
	var n : System.String = "42";

	return n;
}


/*----------
/
/  Class definitions
/
----------*/

	class Person {
		private var iAge : int;
	
		public function GetAge() {
			return iAge;
		}
		
		public function SetAge(iNewAge) {
			iAge = iNewAge;
		}
	}

	class Student extends Person {
		private var iGPA : float;
	
		public function GetGPA() {
			return iGPA;
		}
		
		public function SetGPA(iNewGPA) {
			iGPA = iNewGPA;
		}
	}




function foo01 (I : int, T) {
	verify(I.GetType(),"System.Int32","01.  Wrong type","");
	return T.GetType(); }
function foo02 (l : long, T) {
	verify(l.GetType(),"System.Int64","02.  Wrong type","");
	return T.GetType(); }
function foo03 (f : float, T) {
	verify(f.GetType(),"System.Single","03.  Wrong type","");
	return T.GetType(); }
function foo04 (d : double, T) {
	verify(d.GetType(),"System.Double","04.  Wrong type","");
	return T.GetType(); }
@if(!@rotor)
function foo05 (fso : ActiveXObject, T) {
	verify(fso.GetType(),"System.__ComObject","05.  Wrong type","");
	return T.GetType(); }
@end
function foo06 (a : Array, T) {
	verify(a.GetType(),"Microsoft.JScript.ArrayObject","06.  Wrong type","");
	return T.GetType(); }
function foo07 (b : Boolean, T) {
	verify(b.GetType(),"System.Boolean","07.  Wrong type","");
	return T.GetType(); }
function foo08 (d : Date, T) {
	verify(d.GetType(),"Microsoft.JScript.DateObject","08.  Wrong type","");
	return T.GetType(); }

function foo10 (e : Error, T) {
	verify(e.GetType(),"Microsoft.JScript.ErrorObject","10.  Wrong type","");
	return T.GetType(); }
/*function foo11 (f : Function, T) {
	verify(f.GetType(),"Microsoft.JScript.Closure","11.  Wrong type","");
	return T.GetType(); }*/
function foo12 (n : Number, T) {
	verify(n.GetType(),"System.Double","12.  Wrong type","");
	return T.GetType(); }
function foo13 (r : RegExp, T) {
	verify(r.GetType(),"Microsoft.JScript.RegExpObject","13.  Wrong type","");
	return T.GetType(); }
function foo14 (s : String, T) {
	verify(s.GetType(),"System.String","14.  Wrong type","");
	return T.GetType(); }
function foo15 (o : Person, T) {
	verify(o.GetType(),"Person","15.  Wrong type","");
	return T.GetType(); }
function foo16 (i : Int16, T) {
	verify(i.GetType(),"System.Int16","16.  Wrong type","");
	return T.GetType(); }
function foo17 (s : System.String, T) {
	verify(s.GetType(),"System.String","17.  Wrong type","");
	return T.GetType(); }


function typann21() {

    apInitTest("typann21: Type annotation -- Object types"); 

	apInitScenario("1.  function foo (I : int, T)");
	verify (foo01(34,"Hello"), "System.String", "1.1 Wrong value","");
	verify (foo01(34,new RegExp("/(\d+)/")),"Microsoft.JScript.RegExpObject","1.2 Wrong value","");


	apInitScenario("2.  function foo (l : long, T)");
	verify (foo02((new Date("1/1/2001")).valueOf(),"hello"), "System.String", "2.1 Wrong value","");
	verify (foo02((new Date("1/1/2001")).valueOf(),new RegExp("/(\d+)/")), "Microsoft.JScript.RegExpObject", "2.2 Wrong value","");


	apInitScenario("3.  function foo (f : float, T)");
	verify (foo03(3.14159,"hello"), "System.String", "3.1 Wrong value","");
	verify (foo03(3.14159,new RegExp("/(\d+)/")), "Microsoft.JScript.RegExpObject", "3.2 Wrong value","");


	apInitScenario("4.  function foo (d : double, T)");
	verify (foo04(1.7976931348623157e+308,"hello"), "System.String", "4.1 Wrong value","");
	verify (foo04(1.7976931348623157e+308,new RegExp("/(\d+)/")), "Microsoft.JScript.RegExpObject", "4.2 Wrong value","");

@if(!@rotor)
	apInitScenario("5.  function foo (fso : ActiveXObject, T)");
	verify (foo05(new ActiveXObject("Scripting.FileSystemObject"),"hello"), "System.String", "5.1 Wrong value","");
	verify (foo05(new ActiveXObject("Scripting.FileSystemObject"),new RegExp("/(\d+)/")), "Microsoft.JScript.RegExpObject", "5.2 Wrong value","");
@end

	apInitScenario("6.  function foo (arr : Array, T)");
	verify (foo06(new Array(3,4,5,6),"hello"), "System.String", "6.1 Wrong value","");
	verify (foo06(new Array(3,4,5,6),new RegExp("/(\d+)/")), "Microsoft.JScript.RegExpObject", "6.2 Wrong value","");


	apInitScenario("7.  function foo (b : Boolean, T)");
	verify (foo07(new Boolean(true),"hello"), "System.String", "7.1 Wrong value","");
	verify (foo07(new Boolean(true),new RegExp("/(\d+)/")), "Microsoft.JScript.RegExpObject", "7.2 Wrong value","");


	apInitScenario("8.  function foo (d : Date, T)");
	verify (foo08(new Date(),"hello"), "System.String", "8.1 Wrong value","");
	verify (foo08(new Date(),new RegExp("/(\d+)/")), "Microsoft.JScript.RegExpObject", "8.2 Wrong value","");


//	apInitScenario("9.  function foo (enum : Enumerator, T)");

/*
		Spec issue pending
*/


	apInitScenario("10. function foo (e : Error, T)");
	verify (foo10(new Error(2047),"hello"), "System.String", "10.1 Wrong value","");
	verify (foo10(new Error(2047),new RegExp("/(\d+)/")), "Microsoft.JScript.RegExpObject", "10.2 Wrong value","");


/*	apInitScenario("11. function foo (func : Function, T)");
	verify (foo11(foo10,"hello"), "System.String", "11.1 Wrong value","");
	verify (foo11(foo10,new RegExp("/(\d+)/")), "Microsoft.JScript.RegExpObject", "11.2 Wrong value","");
*/

	apInitScenario("12. function foo (n : Number, T)");
	verify (foo12(new Number(42),"hello"), "System.String", "12.1 Wrong value","");
	verify (foo12(new Number(42),new RegExp("/(\d+)/")), "Microsoft.JScript.RegExpObject", "12.2 Wrong value","");


	apInitScenario("13. function foo (re : RegExp, T)");
	verify (foo13(new RegExp("\\s"),"hello"), "System.String", "13.1 Wrong value","");
	verify (foo13(new RegExp("\\s"),new RegExp("/(\d+)/")), "Microsoft.JScript.RegExpObject", "13.2 Wrong value","");


	apInitScenario("14. function foo (str : String, T)");
	verify (foo14("bah bye","hello"), "System.String", "14.1 Wrong value","");
	verify (foo14("buh bye",new RegExp("/(\d+)/")), "Microsoft.JScript.RegExpObject", "14.2 Wrong value","");


	apInitScenario("15. function foo (o : Person, T)");
	var o15 : Person = new Person();
	o15.SetAge(34);
	verify (foo15(o15,"hello"), "System.String", "15.1 Wrong value","");
	verify (foo15(o15,new RegExp("/(\d+)/")), "Microsoft.JScript.RegExpObject", "15.2 Wrong value","");


	apInitScenario("16. function foo (i16 : int16, T)");
	var i16 : Int16 = 32765;
	verify (foo16(i16,"hello"), "System.String", "16.1 Wrong value","");
	verify (foo16(i16,new RegExp("/(\d+)/")), "Microsoft.JScript.RegExpObject", "16.2 Wrong value","");


	apInitScenario("17. function foo (strS : System.String, T)");
	var s17 : System.String = "Boo yah";
	verify (foo17(s17,"hello"), "System.String", "17.1 Wrong value","");
	verify (foo17(s17,new RegExp("/(\d+)/")), "Microsoft.JScript.RegExpObject", "17.2 Wrong value","");


	apEndTest();
}


typann21();


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
