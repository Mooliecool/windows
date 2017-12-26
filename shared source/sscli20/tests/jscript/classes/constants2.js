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


var iTestID = 212521;



// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;


// Put class declarations here
// -----------------------------------------------------------------------


// -----------------------------------------------------------------------
class Alpha1
{
	
	class Beta1 
	{

		public const value1 = "One"
	}
}

	
package Group2
{
	class Alpha2
	{
		public const value2 = "Two";
	}


}

class Alpha3
{

	
	static public var value3; 
 	static Alpha3
	{
		var constValue3 = "Three";
		value3 = constValue3;
		
	}
	
}

class Bravo4
{

	const value4b = "Four";

}
class Alpha4 extends Bravo4 
{
	public var value4;
	public function Alpha4()
	{
		value4 = super.value4b;
	}
}

class Alpha5 
{
	
	public var value5;
	const value6a = "Six";
	public var value6;
	public function Alpha5()
	{
		const value5a = "Five";
		value5 = value5a;
		value6 = value6a;
	}
}
abstract class Bravo7
{
	const value7 = "Seven";
	

}
class Alpha7 extends Bravo7
{
	

}

final class Alpha8
{
	const value8 = "Eight";
	

}


final class Bravo9
{
	const value9 = "Nine";
	

}
final class Alpha9
{
	public function getValue()
	{
		var bravo9 = new Bravo9();
		return bravo9.value9;		

	}
}



class Bravo10
{
	const value10 = 9;
	public function getValue() :int
	{
		
		var temp = value10;
		temp=+4;
		return temp
	}

}
class Alpha10 extends Bravo10
{
	public function getValue() :int
	{
		
		var temp = value10;
		temp++;
		return temp
	}

}
expando class Alpha11
{
	const value12 = 12;
}

  interface Alpha13          
         {                       
         	//const value13 = 13;	
         }



var exceptionThrown13 = false;

try
{
   eval ("  interface Alpha13            " +
         "  {                       " +
         "	const value13 = 13;	    "+
         "  }");
}
catch (error)
{
   exceptionThrown13 = true;
  
}  


class Alpha14
{

	const constValue14 = 14;
	var value14:int = 1;
	var value15:int = 1;
	
	public function get Value14():int
	{
		return constValue14 + value14;
	}
	public function set Value14(input:int)
	{
		value14 = input + constValue14;
	}
	
        public function get Value15():int
	{
		const constValue15 = 15;
		return constValue15 + value15;
	}
	public function set Value15(input:int)
	{
		const constValue15 = 16;
		value15 = input + constValue15;
	}

}


import Group2;


// -----------------------------------------------------------------------
function constants2()
{
   apInitTest ("Constants2");

   // -----------------------------------------------------------------------
   apInitScenario ("1. Access a constant declared in a nested class");
   
     
   expected = "One";
   actual = "";
   
   var alpha1 = new Alpha1();
   var beta1 = new alpha1.Beta1()
   
   actual = beta1.value1;
   if (actual != expected)
      apLogFailInfo ("Error in 1", expected, actual, "");
      
    
   // -----------------------------------------------------------------------
   apInitScenario ("2. Access a constant inside a class that's enclosed in a package");
   
   
   
   var alpha2: Alpha2 = new Alpha2();
   expected = "Two";
   actual = "";
   actual = alpha2.value2;
   if (actual != expected)
      	apLogFailInfo ("Error in 2", expected, actual, "");
   

   // -----------------------------------------------------------------------
   apInitScenario ("3. Declare and use a constant in a static initializer");
   
   
   
   expected = "Three";
   actual = "";
   actual = Alpha3.value3;
   if (actual != expected)
      	apLogFailInfo ("Error in 3", expected, actual, "");

   // -----------------------------------------------------------------------
   apInitScenario ("4. Declare a constant in a base class; call it from the subclass using the “super” keyword");
   
   
   var alpha4: Alpha4 = new Alpha4();
   expected = "Four";
   actual = "";
   actual = alpha4.value4;
   if (actual != expected)
      	apLogFailInfo ("Error in 4", expected, actual, "");
   

   // -----------------------------------------------------------------------
   apInitScenario ("5. Declare and use a constant inside a constructor");
   
   
   var alpha5: Alpha5 = new Alpha5();
   expected = "Five";
   actual = "";
   actual = alpha5.value5;
   if (actual != expected)
      	apLogFailInfo ("Error in 5", expected, actual, "");

   // -----------------------------------------------------------------------
   apInitScenario ("6. Declare a constant in a class; access it from the constructor");
   
   
   var alpha6: Alpha5 = new Alpha5();
   expected = "Six";
   actual = "";
   actual = alpha6.value6;
   if (actual != expected)
      	apLogFailInfo ("Error in 6", expected, actual, "");

   // -----------------------------------------------------------------------
   apInitScenario ("7. Declare a constant in an abstract class; access it from a subclass");
   
   
   var alpha7: Alpha7 = new Alpha7();
   expected = "Seven";
   actual = "";
   actual = alpha7.value7;
   if (actual != expected)
      	apLogFailInfo ("Error in 7", expected, actual, "");


   // -----------------------------------------------------------------------
   apInitScenario ("8. Declare and use a constant in a final class");
   
   
   var alpha8: Alpha8 = new Alpha8();
   expected = "Eight";
   actual = "";
   actual = alpha8.value8;
   if (actual != expected)
      	apLogFailInfo ("Error in 8", expected, actual, "");

   // -----------------------------------------------------------------------

   apInitScenario ("9. Declare a constant in a final class; access it using an instance of a final class");
   
   
   var alpha9: Alpha9 = new Alpha9();
   expected = "Nine";
   actual = "";
   actual = alpha9.getValue();
   if (actual != expected)
      	apLogFailInfo ("Error in 9", expected, actual, "");


   // -----------------------------------------------------------------------

   apInitScenario ("10. Declare a constant in a class; access it from a subclass that overrides the base class' method");
   
   
   var alpha10: Alpha10 = new Alpha10();
   expected = 10;
   actual = "";
   actual = alpha10.getValue();
   if (actual != expected)
      	apLogFailInfo ("Error in 10", expected, actual, "");


   // -----------------------------------------------------------------------

   apInitScenario ("11. Assign a constant to an expando property");
   
   
   var alpha11: Alpha11 = new Alpha11();
   expected = 11;
   actual = "";
   const valueA11 = 11;
   alpha11["value11"] = valueA11;
   alpha11["value11"];
   actual = alpha11["value11"];
   if (actual != expected)
      	apLogFailInfo ("Error in 11", expected, actual, "");

   // -----------------------------------------------------------------------

   apInitScenario ("12. Assign a class' constant to its own expando property");
   
   
   var alpha12: Alpha11 = new Alpha11();
   expected = 12;
   actual = "";
   
   alpha12["valueA12"] = alpha12.value12;
   actual = alpha12["valueA12"];
   if (actual != expected)
      	apLogFailInfo ("Error in 12", expected, actual, "");

   // -----------------------------------------------------------------------

   apInitScenario ("13. Declare a constant in an interface; Check for compiler error ");
   
   
   if (exceptionThrown13 == false)
      apLogFailInfo ("No compile error in 13", "Should give a compile error",exceptionThrown13,"");

// -----------------------------------------------------------------------

   apInitScenario ("14. Declare a constant in a class; use a constant in a class property (get & set) ");
   
   
   var alpha14: Alpha14 = new Alpha14();
   expected = 29;
   actual = "";
   
   alpha14.Value14 = 1;

   actual = alpha14.Value14;
   if (actual != expected)
      	apLogFailInfo ("Error in 14", expected, actual, "");

// -----------------------------------------------------------------------

   apInitScenario ("15. Declare a constant in a class property (get & set) Pass a constant as a parameter to a class property");
   
   
   var alpha15: Alpha14 = new Alpha14();
   expected = 32;
   actual = "";
   
   alpha15.Value15 = 1;

   actual = alpha15.Value15;
   if (actual != expected)
      	apLogFailInfo ("Error in 15", expected, actual, "");

   apEndTest();
}


constants2();


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
