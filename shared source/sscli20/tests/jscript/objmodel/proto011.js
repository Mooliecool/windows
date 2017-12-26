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


var iTestID = 53143;



function Foo(){ }


@if(!@aspx)
   function Me(){ this.prop = "Pass"; }
   function Test1() { return true; }
   function Bar() { } 
@else
   expando function Me(){ this.prop = "Pass"; }
   expando function Test1() { return true; }
   expando function Bar() { } 
@end

function proto011(){
	apInitTest( "proto011: bug_Regressions" );
@if(@_fast)
	var res, x;
@end
	apInitScenario( "1. Compare Function and Object valueOf " ); 
	if ( Function.prototype.valueOf != Object.prototype.valueOf )
		apLogFailInfo( "1st valueOf compare failed", true, false,"Scripting: 181" );
	Object.prototype.valueOf = 0;
	if ( Function.prototype.valueOf != Object.prototype.valueOf )
		apLogFailInfo( "2nd valueOf compare failed", true, false,"Scripting: 181" );

	apInitScenario( "2. Verify object.prototype is inherited - not in fast mode" ); 

@if(!@_fast)
	Object.prototype.sTest = "Pass";
	res = new Function();
	if ( res.sTest != "Pass" )
		apLogFailInfo( "Property not inherited for function", "Pass", res.sTest,"Scripting: 58" );

	res = new Array();
	if ( res.sTest != "Pass" )
		apLogFailInfo( "Property not inherited for Array","Pass", res.sTest,"Scripting: 58" );

	res = new Date();
	if ( res.sTest != "Pass" )
		apLogFailInfo( "Property not inherited for Date", "Pass", res.sTest,"Scripting: 58" );

	res = new Object();
	if ( res.sTest != "Pass" )
		apLogFailInfo( "Property not inherited for object", "Pass", res.sTest,"Scripting: 58" );

	res = new String();
	if ( res.sTest != "Pass" )
		apLogFailInfo( "Property not inherited for function", "Pass", res.sTest,"Scripting: 58" );

	res = new Number();
	if ( res.sTest != "Pass" )
		apLogFailInfo( "Property not inherited for number", "Pass", res.sTest,"Scripting: 58" );

	res = new Boolean();
	if ( res.sTest != "Pass" )
		apLogFailInfo( "Property not inherited for Boolean", "Pass", res.sTest,"Scripting:58" );
@end
	apInitScenario( "3. prototypes are assignable" ); 
	// Custom object proto
	Foo.prototype.Prop1 = 123.456;
	Foo.prototype.Prop2 = "Pass";
	Foo.prototype.Prop3 = true;
	Foo.prototype.Prop4 = Test1;
	Foo.prototype.test = "Pass2";

	Bar.prototype = Foo.prototype;
	x = new Bar();
	if ( x.Prop1 != 123.456 )
		apLogFailInfo( "Prototype not assignable", 123.456, x.Prop1,"" );
	if ( x.Prop2 != "Pass" )
		apLogFailInfo( "Prototype not assignable", "Pass", x.Prop2,"" );
	if ( x.Prop3 != true )
		apLogFailInfo( "Prototype not assignable",true, x.Prop3,"" );
	if ( x.Prop4() != true )
		apLogFailInfo( "Prototype not assignable", true, x.Prop4(),"" );
	if ( x.test != "Pass2" )
		apLogFailInfo( "Prototype not assignable", "Pass2", x.test,"" );

	apInitScenario( "4. prototypes contain constructor props" );
	
	// For custom objects
	var y = new Me.prototype.constructor();
	if ( y.prop != "Pass" )
		apLogFailInfo( "Constructor prop not available", "Pass", y.prop,"VS" );
	Me.prototype.prop2  = 1234.56;
	y = new Me.prototype.constructor();
	if ( y.prop != "Pass" )
		apLogFailInfo( "Constructor prop not available", "Pass", y.prop,"" );
	if ( y.prop2 != 1234.56 )
		apLogFailInfo( "Constructor prop not available", "Pass", y.prop,"" );

	// For builtin objects
	y = new Array.prototype.constructor();
	y[0] = 123.456;
	y[1] = "asd";
	if ( y[0] != 123.456 || y[1] != "asd" )
		apLogFailInfo( "Array Constructor prop not available", "", "","" );
	y = new Number.prototype.constructor();
	y = 123.456;
	if ( y != 123.456)
		apLogFailInfo( "Constructor prop not available", 123.456, y,"" );
	y = new String.prototype.constructor();
	y = "test string";
	if ( y != "test string")
		apLogFailInfo( "Constructor prop not available", "test string", y,"" );
	y = new Boolean.prototype.constructor();
	y = true;
	if ( y != true)
		apLogFailInfo( "Constructor prop not available", true, y,"" );
	y = new Date.prototype.constructor(0);
	var z = new Date(0);
	if ( y.toLocaleString() != z.toLocaleString())
		apLogFailInfo( "Constructor prop not available", true, y,"" );

	apEndTest();
}


proto011();


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
