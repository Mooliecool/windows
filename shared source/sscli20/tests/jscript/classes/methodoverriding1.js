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


var iTestID = 172970;

/* -------------------------------------------------------------------------
  Test: 	MethodOverriding1 (MethodOverriding1.js)
   
  Product:	JScript
 
  Area:		classes
 
  Purpose:	method overriding. (focus on non-public method and others)
 
  Scenarios:	total 23
		1.0 Declare/instantiate for each classes, access private method through public method
		1.1 Declare/instantiate subclass, call its own public method which has same name as base class's private method
		1.2 Delcare/instantiate subclass, access private through one lever up public method but has same public in its own
		1.3 Declare type as base class,instantiate with subclass,call subclass public method
		1.4 Declare type as base class, instantiate with subclass,access private through one layer up method but has same public in its own
		2.0 Protected: declare/instantiate base class, access protected
		2.1 declare/instantiate subclass, use its own public to access other's protected
		2.2 Declare/instantiate subclass, use one layer up's public method to access its own protected
		2.3 Protected: declare as base type, instantiate with subclass to access protected
		2.4 Declare as base type/instantiate with subclass, pass to function with base type argument
		2.5 Declare/instantiate with subclass, pass to function with sub type argument
		2.6 Protected: calling inside class.
		2.7 Protected: calling inside class but through function
		3.1 final: Declare/instantiate all classes, to access 'final-none-final'
		3.2 Declare base type/instantiate with subclass, to access 'public-public(call fina)-final'
		4. static: witout override keyword.
		5.1 property Get/Set override access from outside of class
		5.2 access through super inside class
		5.3 access one layer up's property
		6. access overrided method through super from several layer
		7. override function without extending excplicitly from System.Object
		8.0 access override function inside of nested class
		8.1 one layer down access inside of nested class
		
  Notes: 	
 ---------------------------------------------------------------------------
  
 
	[00]	05-Apr-2001	    qiongou: Created Test
 -------------------------------------------------------------------------*/

var sAct="";
var sExp = "";
var sActErr = "";
var sExpErr = "";
var sErrThr = "";

class A
{	//************private
	private function pvt(c : char): char 
	{
		return "A";
	}
	public function access_pvt(c : char) : String
	{
		return pvt(c);
	}
	//*******protected
	protected function ptc(i: int) : int
	{
		return i;
	}
	public function access_ptc(i:int): int
	{
		return ptc(i);
	}
	//**********final
	final public function f1(): String
	{
		return "A final";
	}
	public function f2(): String
	{
		return "A f2";
	}
	//********static
	static var s: String = "I am A";
	static function f_static(s1:String): String
	{
		if ( s1 == s )
		{
			return s;
		}
		else
			return "I am A too";
	}
	//********property 
	var arr: Array = [0,1,2];
	function get name() : boolean
	{
		return false;
	}
	function set name (f:boolean)
	{ 
		arr[0] = 100;
	}
}

class B extends A
{
	//**************private
	private function pvt(c : char) : char
	{
		var b : char = "B";
		return b;
		
	}
	public function access_pvt(c : char): String
	{
		return pvt(c);
	}
	//*************protected
	public function access_ptc(i:int):int
	{
		return ptc(i)*2;
	}
	//*************final
	public function f2() :String
	{
		return f1();
	}
	//*************property
	function get name(): boolean
	{
		return true;
	}
	function set name(f:boolean)
	{
		arr[0] = 99;
	}
}

class C extends B
{
	//***********private->public
	public function pvt(c:char):char
	{
		return c;
	}
	//***********protected
	protected function ptc(i:int):int
	{
		return i+100;
	}
	//*********final
	final public function f1(): String
	{
		return "C final";
	}
	final function f2(): String
	{
		return "C f2";
	}
	//********static
	static var s_c: String = "I am C";
	static function f_static(s1:String): String
	{
			return s_c;
	}
	//*********property with super access
	function get name(): boolean
	{
		super.name = false;
		return super.name;
	}
}

class base
{
	function \super(): String
	{
		return "base";
	}	
}

class sub1 extends base
{
	function \super(): String
	{
		//print("super in sub1");
		return super.\super();
	}
}

class sub2 extends sub1
{
	function \super(): String
	{
		//print("super in sub2");
		return super.\super();
	}
}

class sub3 extends sub2
{
	function \super(): String
	{
		//print("super in sub3");
		return super.\super();
	}
}



function test_ptc_A (a: A, i: int): int
{
	return a.access_ptc(i);
}

function test_ptc_C(c: C, i:int):int
{
	return c.access_ptc(i);
}

class test_ptc
{
	var b: B = new B();
	public var c: C = new C();
	private var c_p : C;	
	
	public function access_class_ptc (i:int):int
	{
		c_p = new C();
		return c_p.access_ptc(i);
	}
	public function access_ptc(i:int, a: A) :int
	{
		return a.access_ptc(i);
	}
}	 

class CString 
{
	public var c= new String();
	
	public function CString(input_string)
	{
		c = input_string;
	}
	public function ToString():String
	{
		return this.c;
	}
}

class AA
{
	class BB
	{
		var  x : int =0;
		function test(): int {x=1; return x}
		class DD 
		{
			function test(): int
			{
				return 100;
			}
		}
		class EE extends DD
		{
			function test(): int
			{
				super.test();
				return 99;
			}
		}
	}
	class CC extends BB
	{
		function test(): int { x = 2; return x}
	}
}

//***********************************
function methodoverriding1()
{
	apInitTest("MethodOverriding1");
	
	//*****************
	apInitScenario ("1.0 Declare/instantiate for each classes, access private method through public method");

	var a1 : A = new A();
	var b1 : B = new B();
	var c1 : C = new C();
	
	sAct = "";
	sExp = "A";
	sAct = a1.access_pvt(101); //changed from "" to 101 see bug 305639
	if (sAct != sExp )
		apLogFailInfo ("1.0 A:access_pvt return wrong value", sExp, sAct, "");

	sAct = "";
	sExp = "B";
	sAct = b1.access_pvt(99); //changed from "" to 99 see bug 305639
	if (sAct != sExp )
		apLogFailInfo ("1.0 B:access_pvt return wrong value", sExp, sAct, "");
	
	//*****************
	apInitScenario ("1.1 Declare/instantiate subclass, call its own public method which has same name as base class's private method");
 	sAct = "";
	sExp = "D";
	sAct = c1.pvt("D");
	if (sAct != sExp)
		apLogFailInfo ("1.1 C: pvt return wrong value",sExp,sAct,"");

	//***************** #238044
	apInitScenario ("1.2 Delcare/instantiate subclass, access private through one lever up public method but has same public in its own");
	sAct = "";
	sExp = "B";
	sAct = c1.access_pvt("C");
	if (sAct != sExp)
		apLogFailInfo ("1.2 C: access_pvt return wrong value",sExp,sAct,"");


	//*****************
	var a1_3: A;
	a1_3 = new C();
	apInitScenario ("1.3 Declare type as base class,instantiate with subclass,call subclass public method");
	
	sExpErr = "ReferenceError: 'a1_3.pvt' is not accessible from this scope";
        var sExpErrNum = 1123
   	sActErr = "";
	var sActErrNum = 0
   	sErrThr = false;
   
   	try
   	{
@if (@_jscript_version == 7)     	 	
             eval ("a1_3.pvt('E')");
@end
@if (@_jscript_version > 7)
             eval ("a1_3.pvt('E')","unsafe");
@end
   	}
   	catch (error)
   	{
		sActErrNum = error.number&0x0000ffff
      		sErrThr = true;
     	 	sActErr = error;
   	}
   	if (apGetLocale() == 1033){
   	  if (sErrThr == false)
      		apLogFailInfo ("1.3 A=C: compile error", "An exception should be thrown", sErrThr, "");
   	  if (sActErr != sExpErr)
      		apLogFailInfo ("1.3 A=C: Wrong error message", sExpErr, sActErr, "");      
	}
   	if (sActErrNum != sExpErrNum)
      		apLogFailInfo ("1.3 A=C: Wrong error message", sExpErrNum, sActErrNum, "");      	
	//*****************#238044
	apInitScenario("1.4 Declare type as base class, instantiate with subclass,access private through one layer up method but has same public in its own");
	sAct = "";
	sExp = "B";
	sAct = a1_3.access_pvt("F");
	if (sAct != sExp)
		apLogFailInfo("1.4 A=C: access_pvt return wrong value", sExp,sAct,"");



	//*********************
	apInitScenario ("2.0 Protected: declare/instantiate base class, access protected");
	var a2_0: A = new A();
		
	sAct = 0;
	sExp = 1;
	sAct = a2_0.access_ptc(1);
	if (sAct != sExp)
		apLogFailInfo ("2.0 A: access_ptc return wrong value", sExp, sAct, "");
	
	//********************* 
	apInitScenario("2.1 declare/instantiate subclass, use its own public to access other's protected");
	var b2_1: B = new B();
	sAct = 0;
	sExp = 2;
	sAct = b2_1.access_ptc(1);
	if (sAct != sExp)
		apLogFailInfo ("2.1 B: access_ptc return wrong value", sExp, sAct, "");
	
	//*********************
	apInitScenario ("2.2 Declare/instantiate subclass, use one layer up's public method to access its own protected");	
  	var c2_2: C = new C();
	sAct = 0;
	sExp = 202;
	sAct = c2_2.access_ptc(1);
	if (sAct != sExp)
		apLogFailInfo ("2.2 C: access_ptc return wrong value", sExp, sAct,"");

	//*********************
	apInitScenario ("2.3 Protected: declare as base type, instantiate with subclass to access protected");
	var a2_3: A;
	a2_3 = new B();
	
	sAct = 0;
	sExp = 2;
	sAct = a2_3.access_ptc(1);
	if (sAct != sExp)
		apLogFailInfo ("2.3 B: access_ptc return wrong value", sExp, sAct,"");
	
	a2_3 = new C();
	sAct = 0;
	sExp = 202;
	sAct = a2_3.access_ptc(1);
	if (sAct != sExp)
		apLogFailInfo ("2.3 C: access_ptc return wrong value", sExp, sAct, "");
	
	//*********************
	apInitScenario ("2.4 Declare as base type/instantiate with subclass, pass to function with base type argument");
	var a2_4: A;
	
	a2_4 = new B();
	sAct = 0;
	sExp = 2;
	sAct = test_ptc_A(a2_4, 1);
	if (sAct != sExp)
		apLogFailInfo ("2.4 B: test_ptc_A return wrong value", sExp, sAct, "");
	
	a2_4 = new C();
	sAct = 0;
	sExp = 202;
	sAct = test_ptc_A(a2_4, 1);
	if (sAct != sExp)
		apLogFailInfo ("2.4 C: test_ptc_A return wrong value", sExp, sAct, "");

	//*********************
	apInitScenario ("2.5 Declare/instantiate with subclass, pass to function with sub type argument");
	var c2_5 = new C();
	sAct = 0;
	sExp = 202;
	sAct = test_ptc_C(c2_5, 1);
	if (sAct != sExp)
		apLogFailInfo ("2.5 C: test_ptc_C return wrong value", sExp, sAct, "");

	//*********************
	apInitScenario ("2.6 Protected: calling inside class.");
	var t: test_ptc = new test_ptc();
	
	sAct = 0;
	sExp = 2;
	sAct = t.b.access_ptc(1);
	if (sAct != sExp)
		apLogFailInfo("2.6: t.b return wrong value",sExp,sAct,"");
	
	sAct = 0;
	sExp = 202;
	sAct = t.c.access_ptc(1)
	if (sAct != sExp)
		apLogFailInfo ("2.6: t.c return wrong value", sExp, sAct,"");

	//*********************
	apInitScenario("2.7 Protected: calling inside class but through function");
	sAct = 0;
	sExp = 202;
	sAct = t.access_class_ptc(1);
	if (sAct != sExp)
		apLogFailInfo("2.7: t.access_class_ptc return wrong value",sExp, sAct,"");

	var a2_7: A;
	a2_7 = new B();
	sAct = 0;
	sExp = 2;
	sAct = t.access_ptc(1, a2_7);
	if (sAct != sExp)
		apLogFailInfo("2.7: t.access_ptc return wrong value",sExp,sAct,"");
	
	//*********************
	apInitScenario ("3.1 final: Declare/instantiate all classes, to access 'final-none-final' ");
	var a3_1 :A = new A();
	var b3_1 :B = new B();
	var c3_1 : C = new C();

	sAct = "";
	sExp = "A final";
	sAct = a3_1.f1();
	if (sAct != sExp)
		apLogFailInfo("3.1: a3_1.f1 return wrong value",sExp,sAct,"");
	
	sAct = "";
	sExp = "A final";
	sAct = b3_1.f1();
	if (sAct != sExp)
		apLogFailInfo("3.1: b3_1.f1 return wrong value",sExp,sAct,"");
	
	sAct = "";
	sExp = "C final";
	sAct = c3_1.f1();
	if (sAct != sExp)
		apLogFailInfo("3.1: c3_1.f1 return wrong value",sExp,sAct,"");

	//***********************
	apInitScenario ("3.2 Declare base type/instantiate with subclass, to access 'public-public(call fina)-final'");
	var a3_2 : A;
	
	a3_2 = new A();
	sAct = "";
	sExp = "A f2";
	sAct = a3_2.f2();
	if (sAct != sExp)
		apLogFailInfo("3.2: A. f2 return wrong value",sExp,sAct,"");
	
	a3_2 = new B();
	sAct = "";
	sExp = "A final";
	sAct = a3_2.f2();
	if (sAct != sExp)
		apLogFailInfo("3.2 B. f2 return wrong vlaue", sExp, sAct,"");
	
	a3_2 = new C();
	sAct = "";
	sExp = "C f2";
	sAct = a3_2.f2();
	if (sAct != sExp)
		apLogFailInfo("3.2 C. f2 return wrong vlaue", sExp, sAct,"");
	
	//***********************
	apInitScenario("4. static: witout override keyword.");
	sAct = "";
	sExp = "I am A";
	sAct = A.f_static(sExp);
	if (sAct != sExp)
		apLogFailInfo("4. A f_static return wrong vlaue", sExp, sAct,"");
	
	//design changed: can not access static member from sub class
	/*
	sAct = "";
	sExp = "I am A";
	sAct = B.f_static(sExp);
	if (sAct != sExp)
		apLogFailInfo("4. B f_static return wrong vlaue", sExp, sAct,"");
	*/

	sAct = "";
	sExp = "I am C";
	sAct = C.f_static(sExp);
	if (sAct != sExp)
		apLogFailInfo("4. C f_static return wrong vlaue", sExp, sAct, "");

	
	//***********************
	apInitScenario ("5.1 property Get/Set override access from outside of class");
	var a5 : A = new A();
	var b5 : B = new B();
	
	a5.arr[0] = 0;
	a5.name = true;
	sAct = 0;
	sExp = 100;
	sAct = a5.arr[0];
	if (sAct != sExp)
		apLogFailInfo("5.1 A set property return wrong value", sExp, sAct, "");

	sAct = true;
	sExp = false;
	sAct = a5.name;
	if (sAct != sExp)
		apLogFailInfo("5.1 A get property return wrong value", sExp, sAct, "");

	b5.arr[0] = 0;
	b5.name = false;
	sAct= 0;
	sExp = 99;
	sAct = b5.arr[0];
	if (sAct != sExp)
		apLogFailInfo("5.1 C set property return wrong value", sExp, sAct, "");

	sAct = false;
	sExp = true;
	sAct = b5.name;
	if (sAct != sExp)
		apLogFailInfo("5.1 C property return wrong value", sExp, sAct,"");
	
	//***********************
	apInitScenario ("5.2 access through super inside class");
	var c5 : C = new C();

	sAct = false;
	sExp = true;
	sAct = c5.name;
	if (sAct != sExp)
		apLogFailInfo("5.2 C property with super return wrong value", sExp, sAct,"");


	//***********************
	apInitScenario(" 5.3 access one layer up's property");
	var c5_3 :C = new C();

	c5.arr[0] = 0;
	c5.name = false;
	sAct = 0;
	sExp = 99;
	sAct = c5.arr[0];
	if (sAct != sExp)
		apLogFailInfo("5.3 C->B's set return wrong value",sExp,sAct,"");

	//***********************
	apInitScenario ("6. access overrided method through super from several layer");
	var x : base;
	
	x = new sub1();
	sAct = "";
	sExp = "base";
	sAct = x.\super();
	if (sAct != sExp)
		apLogFailInfo("6. sub1 return wrong value", sExp, sAct, "");

	x = new sub2();
	sAct = "";
	sExp = "base";
	sAct = x.\super();
	if (sAct != sExp)
		apLogFailInfo("6. sub1 return wrong value", sExp, sAct, "");

	x = new sub3();
	sAct = "";
	sExp = "base";
	sAct = x.\super();
	if (sAct != sExp)
		apLogFailInfo("6. sub1 return wrong value", sExp, sAct, "");

	//***********************
	apInitScenario ("7. override function without extending excplicitly from System.Object");
	var y = new CString("hello");
	
	sAct = "";
	sExp = "hello";
	sAct = y;
	if (sAct != sExp)
		apLogFailInfo ("7. CString return wrong value", sExp, sAct, "");

	//************************
	apInitScenario ("8.0 access override function inside of nested class");
	var aa: AA = new AA();
	var bb: AA.BB = new aa.BB();
	var cc: AA.CC = new aa.CC();
	var bd: AA.BB.DD = new bb.DD();
	var be: AA.BB.EE = new bb.EE();

	sAct = 0;
	sExp = 1;
	sAct = bb.test();
	if (sAct != sExp)
		apLogFailInfo ("8.0 bb.test return wrong valuse",sExp,sAct, "");

	sAct = 0;
	sExp = 2;
	sAct = cc.test();
	if (sAct != sExp)
		apLogFailInfo ("8.0 cc.test return wrong valuse",sExp,sAct, "");

	//************************
	apInitScenario ("8.1 one layer down access inside of nested class");
	sAct = 0;
	sExp = 100;
	sAct = bd.test();
	if (sAct != sExp)
		apLogFailInfo ("8.1 bd.test return wrong valuse",sExp,sAct, "");

	sAct = 0;
	sExp = 99;
	sAct = be.test();
	if (sAct != sExp)
		apLogFailInfo ("8.1 be.test return wrong valuse",sExp,sAct, "");
	apEndTest ();
}




methodoverriding1();


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
