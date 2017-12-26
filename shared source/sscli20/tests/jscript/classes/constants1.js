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


var iTestID = 176016;



// -----------------------------------------------------------------------
var actual = 0;
var expected = 0;
var exceptionThrown = false;
var expectedError = "";
var actualError = "";
var globalCtr = 0;


@if(!@aspx)
	import System
	import System.IO
	import System.Diagnostics
@else
	</script>
	<%@ import namespace="System.IO" %>
	<%@ import namespace="System.Diagnostics" %>
	<script language=jscript runat=server>
@end

var JSCode: String;
var fileStream: FileStream;
var streamWriter: StreamWriter;
var process: Process;
var processStartInfo: ProcessStartInfo;



// Put class declarations here
// -----------------------------------------------------------------------
class Alpha7
{

    function Scenario7(var7)
    {
	return ++var7;
    }

}

// -----------------------------------------------------------------------

class Alpha8
{
    public const value8: String = "Eight";
    public const value9 = "Nine";

}



// -----------------------------------------------------------------------

var exceptionThrown10 = false;

try
{
   eval ("  class Alpha10            " +
         "  {                       " +
         "	const value10;	    "+
         "  }");
}
catch (error)
{
   exceptionThrown10 = true;
  
}  

// -----------------------------------------------------------------------

var exceptionThrown11 = false;

try
{
   eval ("  class Alpha11            " +
         "  {                        " +
         "	const value11;	     "+
	 "	value11 = 11;	     "+
         "  }");
}
catch (error)
{
   exceptionThrown11 = true;
  
}  

// -----------------------------------------------------------------------

class Alpha12
{
    private const value12 = "private";
    protected const value13 = "protected";
    internal const value14 = "internal";
    public const value15 = "public";
    
    function getPrivateConst()
    {
	return value12;
    }
    function getProtectedConst()
    {
	return value13;
    }
    function getInternalConst()
    {
	return value14;
    }
    function getPublicConst()
    {
	return value15;
    }
}

// -----------------------------------------------------------------------

class Alpha16
{
   
    
    function passConst()
    {
	const value16 = "SixTeen";
	return getConst(value16);
    }
    function getConst(input)
    {
	return input;
    }

}

// -----------------------------------------------------------------------

class Alpha17
{
   
    
    function returnConst()
    {
	const value17 = "SevenTeen";
	return value17;
    }
    
}

// -----------------------------------------------------------------------
class Alpha18
{
	public static const value18 = "EightTeen";
}


// -----------------------------------------------------------------------
var expectedError20, actualError20: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha20                                " +
         "  {                                            " +
         "     public static const value20 = \"Twenty\"; " +
         "  }                                            " +
         "  class Beta20 extends Alpha20                 " +
         "  {  }                                         " +
         "  var x = Beta20.value20;                      ";


@if(!@rotor)
fileStream = new FileStream ("test.js", FileMode.Create, FileAccess.Write);
@else
fileStream = new FileStream ("test.js", FileMode.Create, FileAccess.Write);
@end
streamWriter = new StreamWriter (fileStream);
streamWriter.Write (JSCode);
streamWriter.Close();
fileStream.Close();

// Create a process that will call "jsc test.js"
process = new Process();
processStartInfo = new ProcessStartInfo();

@if(!@rotor)
	processStartInfo.FileName = "jsc";
	processStartInfo.Arguments = "-nologo test.js";
	processStartInfo.UseShellExecute = false;
@else
	processStartInfo.FileName = "clix";
	processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
@end
processStartInfo.RedirectStandardOutput = true;
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError20 = process.StandardOutput.ReadToEnd();

@if(!@rotor)
expectedError20 = "test.js(1,298) : error JS1246: Type 'Beta20' does not have such a static member" + Environment.NewLine + "";
@else
expectedError20 = "test.js(1,298) : error JS1246: Type 'Beta20' does not have such a static member" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError21, actualError21: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha21                                " +
         "  {                                            " +
         "     public static const value21 = \"one\";    " +
         "  }                                            " +
         "  class Beta21 extends Alpha21                 " +
         "  {  }                                         " +
         "  var beta21: Beta21 = new Beta21();           " +
         "  var x;                                       " +
         "  x = beta21.value;                            ";

@if(!@rotor)
fileStream = new FileStream ("test.js", FileMode.Create, FileAccess.Write);
@else
fileStream = new FileStream ("test.js", FileMode.Create, FileAccess.Write);
@end
streamWriter = new StreamWriter (fileStream);
streamWriter.Write (JSCode);
streamWriter.Close();
fileStream.Close();

// Create a process that will call "jsc test.js"
process = new Process();
processStartInfo = new ProcessStartInfo();

@if(!@rotor)
	processStartInfo.FileName = "jsc";
	processStartInfo.Arguments = "-nologo test.js";
	processStartInfo.UseShellExecute = false;
@else
	processStartInfo.FileName = "clix";
	processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
@end
processStartInfo.RedirectStandardOutput = true;
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError21 = process.StandardOutput.ReadToEnd();

@if(!@rotor)
expectedError21 = "test.js(1,388) : error JS1151: Objects of type 'Beta21' do not have such a member" + Environment.NewLine + "";
@else
expectedError21 = "test.js(1,388) : error JS1151: Objects of type 'Beta21' do not have such a member" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError22, actualError22: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha22                                         " +
         "  {                                                     " +
         "     public static const value22 = \"TwentyTwo\";       " +
         "  }                                                     " +
         "  class Beta22 extends Alpha22                          " +
         "  {  }                                                  " +
         "  class Charlie22 extends Beta22                        " +
         "  {  }                                                  " +
         "  var x = Charlie22.value22;                            ";


@if(!@rotor)
fileStream = new FileStream ("test.js", FileMode.Create, FileAccess.Write);
@else
fileStream = new FileStream ("test.js", FileMode.Create, FileAccess.Write);
@end
streamWriter = new StreamWriter (fileStream);
streamWriter.Write (JSCode);
streamWriter.Close();
fileStream.Close();

// Create a process that will call "jsc test.js"
process = new Process();
processStartInfo = new ProcessStartInfo();

@if(!@rotor)
	processStartInfo.FileName = "jsc";
	processStartInfo.Arguments = "-nologo test.js";
	processStartInfo.UseShellExecute = false;
@else
	processStartInfo.FileName = "clix";
	processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
@end
processStartInfo.RedirectStandardOutput = true;
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError22 = process.StandardOutput.ReadToEnd();

@if(!@rotor)
expectedError22 = "test.js(1,467) : error JS1246: Type 'Charlie22' does not have such a static member" + Environment.NewLine + "";
@else
expectedError22 = "test.js(1,467) : error JS1246: Type 'Charlie22' does not have such a static member" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
var expectedError23, actualError23: String;

// First create test.js that will contain the JS code that we want to compile
JSCode = "  class Alpha23                                         " +
         "  {                                                     " +
         "     public static const value23 = \"TwentyTwo\";       " +
         "  }                                                     " +
         "  class Beta23 extends Alpha23                          " +
         "  {  }                                                  " +
         "  class Charlie23 extends Beta23                        " +
         "  {  }                                                  " +
         "  var charlie23: Charlie23 = new Charlie23();           " +
         "  var x = charlie23.value23;                            ";


@if(!@rotor)
fileStream = new FileStream ("test.js", FileMode.Create, FileAccess.Write);
@else
fileStream = new FileStream ("test.js", FileMode.Create, FileAccess.Write);
@end
streamWriter = new StreamWriter (fileStream);
streamWriter.Write (JSCode);
streamWriter.Close();
fileStream.Close();

// Create a process that will call "jsc test.js"
process = new Process();
processStartInfo = new ProcessStartInfo();

@if(!@rotor)
	processStartInfo.FileName = "jsc";
	processStartInfo.Arguments = "-nologo test.js";
	processStartInfo.UseShellExecute = false;
@else
	processStartInfo.FileName = "clix";
	processStartInfo.Arguments = " " + System.Environment.GetEnvironmentVariable("JSCCOMP") + " -nologo test.js";
@end
processStartInfo.RedirectStandardOutput = true;
process.StartInfo = processStartInfo;
process.Start();
process.WaitForExit();
actualError23 = process.StandardOutput.ReadToEnd();

@if(!@rotor)
expectedError23 = "test.js(1,523) : error JS1244: The static member 'value23' must be accessed with the class name" + Environment.NewLine + "";
@else
expectedError23 = "test.js(1,523) : error JS1244: The static member 'value23' must be accessed with the class name" + Environment.NewLine + "";
@end

// -----------------------------------------------------------------------
class Alpha24
{
 	static const value24 = 24;

	function passToFunction()
	{
		
		return Scenario5(value24);
	}  
      
}

// -----------------------------------------------------------------------

class Alpha25
{
 
	static const value25 = 25;
	const value26 = 1;
	function passToFunction()
	{
		
		var bravo25:Bravo25 = new Bravo25();
		return bravo25.updateConst(value25);
	}  
      
}
class Bravo25
{

	function updateConst(input)
	{
		return ++input

	}

}


// -----------------------------------------------------------------------

class Alpha2
{
}

class Beta2 extends Alpha2
{
}

// -----------------------------------------------------------------------
//function definition here
function Scenario5(var5)
{
     return var5 + 1;
}
function Scenario6()
{
     const value6 = "Six";
     return value6;
}

// -----------------------------------------------------------------------
function constants1()
{
   apInitTest ("Constants");

   // -----------------------------------------------------------------------
   apInitScenario ("1. Declare a constant; assign a value during declaration; with type annotation");
   
   const value1: String = "One";
   
   expected = "One";
   actual = "";
   actual = value1;
   if (actual != expected)
      apLogFailInfo ("Error in 1", expected, actual, "");
      
    
   // -----------------------------------------------------------------------
   apInitScenario ("2. Declare a constant; assign a value during declaration; without type annotation");
   
   const value2 = "Two";
   
   expected = "Two";
   actual = "";
   actual = value2;
   if (actual != expected)
      apLogFailInfo ("Error in 2", expected, actual, "");

  // -----------------------------------------------------------------------
   
   apInitScenario ("3. Don't assign a value during declaration");
   
   actual = 0;
   try
   {
   	eval("const value3;");
   }
   catch(e)
   {
	actual=1;
   }

   if (actual==0)
   	apLogFailInfo ("Error in 3", "Compile Error", "No Compile Error", "");

// -----------------------------------------------------------------------

   apInitScenario ("4. Assign a value after it has been initialized in the declaration");
   
   var exceptionThrown4 = false;
   var actualError4 = "";
   var expectedError4 = "ReferenceError: 'value' is read-only";

   try
   {
      eval ("  const value = 4;  " +
            "  value = 5;        ");
   }
   catch (error)
   {
      exceptionThrown4 = true;
      actualError4 = error;
   }

   if (exceptionThrown4 == false)
      apLogFailInfo ("No compile error in 4.", "Should give a compile error", exceptionThrown4, "261834, 300501");
   if (actualError4 != expectedError4)
      apLogFailInfo ("Wrong compile error in 4.", expectedError4, actualError4, "261834, 300501");

   
// -----------------------------------------------------------------------

   apInitScenario ("5. Pass a constant to a function");
   
   
   const value5 = 5;
  
   
   expected = 6;
   actual = "";
   actual = Scenario5(value5);
   if (actual != expected)
      apLogFailInfo ("Error in 5", expected, actual, "");

// -----------------------------------------------------------------------

   apInitScenario ("6. Return a constant that's declared in a function");
   
   expected = "Six";
   actual = "";
   actual = Scenario6();
   if (actual != expected)
      apLogFailInfo ("Error in 6", expected, actual, "");

// -----------------------------------------------------------------------

   apInitScenario ("7. Pass a constant to a method");
      
   
   const value7 = 7;
   
   var alpha7:Alpha7 = new Alpha7();
  
   
   expected = 8;
   actual = "";
   actual = alpha7.Scenario7(value7);
   if (actual != expected)
      apLogFailInfo ("Error in 7", expected, actual, "");

// -----------------------------------------------------------------------

   apInitScenario ("8. Declare a constant as a field; assign a value during declaration With type annotation");
      
    
   var alpha8:Alpha8 = new Alpha8();
  
   
   expected = "Eight";
   actual = "";
   actual = alpha8.value8;
   if (actual != expected)
      apLogFailInfo ("Error in 8", expected, actual, "");

// -----------------------------------------------------------------------

   apInitScenario ("9. Declare a constant as a field; assign a value during declaration Without type annotation");
      
    
   var alpha9:Alpha8 = new Alpha8();
  
   
   expected = "Nine";
   actual = "";
   actual = alpha9.value9;
   if (actual != expected)
      apLogFailInfo ("Error in 9", expected, actual, "");

// -----------------------------------------------------------------------
   apInitScenario ("10. Don't assign a value during field declaration");
   
   if (exceptionThrown10 == false)
      apLogFailInfo ("No compile error in 10", "Should give a compile error",exceptionThrown10,"");
 
// -----------------------------------------------------------------------
   apInitScenario ("11. Assign a value after it has been initialized in the field claration");
   
   if (exceptionThrown11 == false)
      apLogFailInfo ("No compile error in 11", "Should give a compile error",exceptionThrown11,"");
 
// -----------------------------------------------------------------------

   apInitScenario ("12. Private constant");
      
    
   var alpha12:Alpha12 = new Alpha12();
  
   
   expected = "private";
   actual = "";
   actual = alpha12.getPrivateConst();
   if (actual != expected)
      apLogFailInfo ("Error in 12", expected, actual, "");

// -----------------------------------------------------------------------

   apInitScenario ("13. Protected constant");
      
    
   var alpha13:Alpha12 = new Alpha12();
  
   
   expected = "protected";
   actual = "";
   actual = alpha13.getProtectedConst();
   if (actual != expected)
      apLogFailInfo ("Error in 13", expected, actual, "");
// -----------------------------------------------------------------------

   apInitScenario ("14. Internal constant");
      
    
   var alpha14:Alpha12 = new Alpha12();
  
   
   expected = "internal";
   actual = "";
   actual = alpha14.getInternalConst();
   if (actual != expected)
      apLogFailInfo ("Error in 14", expected, actual, "");

// -----------------------------------------------------------------------

   apInitScenario ("15. Public constant");
      
    
   var alpha15:Alpha12 = new Alpha12();
  
   
   expected = "public";
   actual = "";
   actual = alpha15.getPublicConst();
   if (actual != expected)
      apLogFailInfo ("Error in 15", expected, actual, "");


// -----------------------------------------------------------------------

   apInitScenario ("16. Declare a constant in a method; pass it to another method");
      
    
   var alpha16:Alpha16 = new Alpha16();
  
   
   expected = "SixTeen";
   actual = "";
   actual = alpha16.passConst();
   if (actual != expected)
      apLogFailInfo ("Error in 16", expected, actual, "");

// -----------------------------------------------------------------------

   apInitScenario ("17. Declare a constant in a method; use it as a return value");
      
    
   var alpha17:Alpha17 = new Alpha17();
  
   
   expected = "SevenTeen";
   actual = "";
   actual = alpha17.returnConst();
   if (actual != expected)
      apLogFailInfo ("Error in 17", expected, actual, "");

// -----------------------------------------------------------------------

   apInitScenario ("18. Static constant - Access the constant using the class itself");
      
    
   expected = "EightTeen";
   actual = "";
   actual = Alpha18.value18;
   if (actual != expected)
      apLogFailInfo ("Error in 18", expected, actual, "");

// -----------------------------------------------------------------------

   apInitScenario ("19. Static constant - Access the constant using an instance of the class");
      
   expected = "1";
   actual=0;


   var alpha19:Alpha18 = new Alpha18 

   try
   {
   	
	eval("actual = alpha19.value18;");
	
   }
   catch(e)
   {
	actual=1;
   }

   if (actual==0)
      apLogFailInfo ("Error in 19", "Compile Error", "No Compile Error", "");


// -----------------------------------------------------------------------
   apInitScenario ("20. Static constant - Access the constant using the subclass itself");
      
   if (actualError20 != expectedError20)
      apLogFailInfo ("Error in 20", expectedError20, actualError20, "");


// -----------------------------------------------------------------------
   apInitScenario ("21. Static constant - Access the constant using an instance of the subclass");
      
   if (actualError21 != expectedError21)
      apLogFailInfo ("Error in 21", expectedError21, actualError21, "");


// -----------------------------------------------------------------------
   apInitScenario ("22. Static constant - Access the constant using the 3rd level subclass itself");
      
   if (actualError22 != expectedError22)
      apLogFailInfo ("Error in 22", expectedError22, actualError22, "");


// -----------------------------------------------------------------------
   apInitScenario ("23. Static constant - Access the constant using an instance of the 3rd level subclass");
   
   if (actualError23 != expectedError23)
      apLogFailInfo ("Error in 23", expectedError23, actualError23, "");


// -----------------------------------------------------------------------

   apInitScenario ("24. Static constant - Pass a static constant to a function");
      
   var alpha24:Alpha24 = new Alpha24 
   expected = 25;
   actual = "";
   actual = alpha24.passToFunction();
   if (actual != expected)
      apLogFailInfo ("Error in 24", expected, actual, "");

// -----------------------------------------------------------------------

   apInitScenario ("25. Static constant - Pass a static constant to a method");
      
   var alpha25:Alpha25 = new Alpha25 
   expected = 26;
   actual = "";
   actual = alpha25.passToFunction();
   if (actual != expected)
      apLogFailInfo ("Error in 25", expected, actual, "");

// -----------------------------------------------------------------------

   apInitScenario ("26. constant - Access a constant form an undefined Object");
      
   var alpha26 = new Alpha25 
   expected = 1;
   actual = "";
   actual = alpha26.value26;
   if (actual != expected)
      apLogFailInfo ("Error in 26", expected, actual, "");




   apEndTest();
}



constants1();


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
