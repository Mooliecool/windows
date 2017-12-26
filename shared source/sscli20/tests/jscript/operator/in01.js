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


var iTestID = 227614;

/* -------------------------------------------------------------------------
  Test: 	in01.js
   
  Product:	JScript
 
  Area:		Control
 
  Purpose:	check In for CC Run.
 
  Scenarios:	
		
  Notes: 	
 ---------------------------------------------------------------------------
  
 
	[0] 	qiongou -Created 11/27/2001
 -------------------------------------------------------------------------*/

/*---------------------------------*/
// System is auto imported for aspx
@if (!@aspx)
  import System;
@end

var sAct="";
var sExp = "";
var sActErr = "";
var sExpErr = "";
var sErrThr = "";




//***********************************
function in01()
{
	apInitTest("");
	
	apInitScenario ("1. In Math Object");
	var a1 = ("PI" in Math);
	var b1 : boolean = ("me" in Math);
	sAct = a1;
	sExp = true;
	if (sAct != sExp) 
		apLogFailInfo ("1.1 In Math Object",sExp,sAct,"");
	sAct = b1;
	sExp = false;
	if (sAct != sExp) 
		apLogFailInfo ("1.2 In Math Object",sExp,sAct,"");
	
	apInitScenario ("2. In an index Object");
	var myObj : Object = {"a": "Athens","b":"Belgrade", "c": "Cairo"}
	var key : String = "b";
	sAct = (key in myObj) 
	if (sAct != true)
		apLogFailInfo ("2.1 In an Object",true,sAct,"");
	if (myObj[key] != "Belgrade")
		apLogFailInfo ("2.1 In an Object","Belgrade",sAct,"");
	
	key = "s";
	sAct = (key in myObj)
	if (sAct != false)
		apLogFailInfo ("2.2 In an Object",false,sAct,"");

	
	apInitScenario ("3. check a named property in an Object")
	var Obj3 : Object = new Object;
	Obj3.property = "hello";
	sAct = ( "property" in Obj3)
	if (sAct != true) 
		apLogFailInfo ("3.1 check a property named property",true ,sAct,"");

	Obj3['prop'] = 1;
	sAct = ( 'prop' in Obj3);
	if (sAct != true) 
		apLogFailInfo ("3.2 check a property named property",true ,sAct,"");

	
	apInitScenario ("4. In an Array object");
	var arr4 = new Array(3);
	arr4[0] = "red";
	arr4['prop'] = "green";
	
	sAct = ( 'prop' in arr4)
	if (sAct != true) 
		apLogFailInfo ("4.1 In an Array object",true ,sAct,"");
	
	sAct = "length" in arr4;
	if (sAct != true) 
		apLogFailInfo ("4.2 In an Array object",true ,sAct,"");

	apInitScenario ("5. prototype Property");
	var p = "prototype"
	sAct =  p in Array;
	if (sAct != true)
		apLogFailInfo ("5.1 prototype Property",true ,sAct,"");
	sAct = p in Function;
	if (sAct != true)
		apLogFailInfo ("5.2 prototype Property",true ,sAct,"");
	sAct = p in RegExp;
	if (sAct != true)
		apLogFailInfo ("5.3 prototype Property",true ,sAct,"");
	sAct = p in Date;
	if (sAct != true)
		apLogFailInfo ("5.4 prototype Property",true ,sAct,"");
	sAct = p in Number;
	if (sAct != true)
		apLogFailInfo ("5.5 prototype Property",true ,sAct,"");
	sAct = p in Boolean;
	if (sAct != true)
		apLogFailInfo ("5.6 prototype Property",true ,sAct,"");
	sAct = p in Object;
	if (sAct != true)
		apLogFailInfo ("5.7 prototype Property",true ,sAct,"");
	sAct = p in String;
	if (sAct != true)
		apLogFailInfo ("5.8 prototype Property",true ,sAct,"");
	
	apInitScenario ("6. modified prototype property")
@if (!@_fast)
	function array_max() {
   		var i, max = this[0];
   		for (i = 1; i < this.length; i++) {
      		if (max < this[i])
         		max = this[i];
   		}
   		return max;
	}
	Array.prototype.max = array_max;
	var x = new Array(1, 2, 3, 4, 5, 6);
	print(x.max());
	sAct = "max" in x;
	if (sAct != true)
		apLogFailInfo ("6. modified prototype property",true ,sAct,"");

@end

	apInitScenario ("7. property chain");
	var obj7 : Object = new Object;
	obj7.sole_property = new Object;
	obj7.sole_property.first = "Ready";
	obj7.sole_property.second = "go";
	sAct = "sole_property" in obj7;
	if (sAct != true)
		apLogFailInfo ("7.1 property chain",true ,sAct,"");
	sAct = "first" in obj7.sole_property
	if (sAct != true)
		apLogFailInfo ("7.2 property chain",true ,sAct,"");


	apInitScenario ("8. constructor property")
@if (!@_fast)
	p = "constructor"
	var a8 :Array = new Array();
	sAct =  p in a8
	if (sAct != true)
		apLogFailInfo ("8.1 constructor property",true ,sAct,"320265");
	var s8 = new String("abc");
	sAct = p in s8;
	if (sAct != true)
		apLogFailInfo ("8.2 constructor property",true ,sAct,"320265");

	var d8 = new Date();
	sAct = p in d8;
	if (sAct != true)
		apLogFailInfo ("8.3 constructor property",true ,sAct,"320265");
	
	var n8 = new Number("123");
	sAct = p in n8;
	if (sAct != true)
		apLogFailInfo ("8.4 constructor property",true ,sAct,"320265");
	
	var b8 = new Boolean("false");
	sAct = p in b8;
	if (sAct != true)
		apLogFailInfo ("8.5 constructor property",true ,sAct,"320265");
	var o8 = new Object;
	sAct = p in o8;
	if (sAct != true)
		apLogFailInfo ("8.6 constructor property",true ,sAct,"320265");

@end
	p = "constructor";
	function myFunction() {} 
	var f8 = new myFunction;
	sAct = p in f8;
	if (sAct != true)
		apLogFailInfo ("8.7 constructor property",true ,sAct,"");
	
	apInitScenario ("9. prototype chain" );
	/*
	function Thingy_bit1() {
		this.prototype = null;
		this.color = "blue";
	}
	function Thingy_bit2() {
		this.shape = 'round';
	}
	function Thingy(name) {
		this.name = name;
	}
	Thingy.prototype = new Thingy_bit1();
	Thingy_bit1.prototype = new Thingy_bit2();

	var thing = new Thingy('Jack');
	*/ 
@if (@_fast)
	function Thingy(name)
	{
		this.name = name;
	}
	function Thingy_common(){
		this.prototype = null;
		this.color = 'blue';
		this.shape = "round";
	}
	
	Thingy.prototype = new Thingy_common()
	var thing = new Thingy("Jack");
	sAct = "name" in thing
	if (sAct != true)
		apLogFailInfo ("9.1 prototype chain",true ,sAct,"32031");
	sAct = "color" in thing
	if (sAct != true)
		apLogFailInfo ("9.2 prototype chain",true ,sAct,"32031");
@end
	apInitScenario ("10. inside of eval")
	var sr = false;
	var s10 = " var str = new String('ff'); " +
		  " str.x = 123; "+
		  " sr = 'x' in str;"
	eval(s10);
	if (sr != true)
		apLogFailInfo ("10.inside of eval ",true ,sAct,"");
		
	apEndTest ();

}

	

in01();


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
