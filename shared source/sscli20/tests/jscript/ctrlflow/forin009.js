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


var iTestID = 52767;



@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function forin009() 
{
   apInitTest("forIn009 ");

// Added to handle fast mode in JScript 7.0
@cc_on 
    @if (@_fast)  
        var ob;
        var cMem;
        var tMem;
        var n;
        var i;    
     
@end       


   //----------------------------------------------------------------------------
   apInitScenario("1. built-in, non-exec"); 

   ob = Math; 

   /*
      Added to handle fast mode in JScript 7.0
      "Expando properties are forbidden on the built-in objects"
      "Predefined properties of built-in objects are marked {DontEnum, DontDelete, ReadOnly}"
   */
   @cc_on 
      @if (@_fast)
            tMem = 0;
        @else 
            tMem = 5;
   @end       

   n = 0;
   i = null;
   
   // For more info, please check VS bug 190534.
   @cc_on 
   @if (!@_fast)
      ob.foo = ob[1234567] = ob["bar"] = ob.bin = ob[-1.23e45] = null;       
   @else 
      ob[1234567] = ob["bar"] = ob[-1.23e45] = null;
   @end 

   for (i in Math) 
      n++

   if (n != tMem) 
      apLogFailInfo( "built-in, non-executable--Math failed",tMem,n,"");


   //----------------------------------------------------------------------------
   apInitScenario("2. built-in, exec, not instanciated");

   // all should have members: length, arguments, caller, and prototype

   ob=Array; 
   cMem=0;

   /*
      Added to handle strict mode in JScript 7.0
      "Expando properties are forbidden on the built-in objects"
      "Predefined properties of built-in objects are marked {DontEnum, DontDelete, ReadOnly}"
   */
   @cc_on 
      @if (@_fast)  
            tMem = 0;
      @else
            tMem = cMem+5; 
       
   @end       

   n=1; 
   i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in Array) 
      n++
   n--;

   if (n != tMem) 
      apLogFailInfo( "built-in, exec, not instantiated--Array failed", tMem, n,"");

   ob=Boolean; 
   cMem=0;

   /*
      Added to handle strict mode in JScript 7.0
      "Expando properties are forbidden on the built-in objects"
      "Predefined properties of built-in objects are marked {DontEnum, DontDelete, ReadOnly}"
   */
   @cc_on 
      @if (@_fast)  
            tMem = 0;
      @else
            tMem = cMem+5; 
       
   @end       

   n=1; 
   i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in Boolean) n++
      n--;

   if (n != tMem) apLogFailInfo( "built-in, exec, not instantiated--Boolean failed", tMem, n,"");

   ob=Date; 
   cMem=0;

   /*
      Added to handle strict mode in JScript 7.0
      "Expando properties are forbidden on the built-in objects"
      "Predefined properties of built-in objects are marked {DontEnum, DontDelete, ReadOnly}"
   */
   @cc_on 
      @if (@_fast)  
            tMem = 0;
      @else
            tMem = cMem+5; 
       
   @end       

   n=1; 
   i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in Date) n++
      n--;

   if (n != tMem) apLogFailInfo( "built-in, exec, not instantiated--Date failed", tMem, n,"");

   ob=Number; 
   cMem=0;

   /*
      Added to handle strict mode in JScript 7.0
      "Expando properties are forbidden on the built-in objects"
      "Predefined properties of built-in objects are marked {DontEnum, DontDelete, ReadOnly}"
   */
   @cc_on 
      @if (@_fast)  
            tMem = 0;
      @else
            tMem = cMem+5; 
       
   @end       

   n=1; 
   i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in Number) n++
      n--;

   if (n != tMem) apLogFailInfo( "built-in, exec, not instantiated--Number failed", tMem, n,"");

   ob=Object; 
   cMem=0;

   /*
      Added to handle strict mode in JScript 7.0
      "Expando properties are forbidden on the built-in objects"
      "Predefined properties of built-in objects are marked {DontEnum, DontDelete, ReadOnly}"
   */
   @cc_on 
      @if (@_fast)  
            tMem = 0;
      @else
            tMem = cMem+5; 
       
   @end       

   n=1; 
   i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in Object) n++
      n--;

   if (n != tMem) apLogFailInfo( "built-in, exec, not instantiated--Object failed", tMem, n,"");


   //----------------------------------------------------------------------------
   apInitScenario("3. user-defined, not instanciated");

   // should have members: length, arguments, caller, and prototype

   ob=obFoo; cMem=0;
   tMem=cMem+5; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in obFoo) n++
      n--;

   if (n != tMem) apLogFailInfo( "user-defined, not instantiated failed", tMem, n,"");


   //----------------------------------------------------------------------------
   apInitScenario("4. string");

   ob=" "; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in " ") n++
      n--;

   if (n != tMem) apLogFailInfo( "string--single space failed", tMem, n,"");

   ob="                                                                   "; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in "                                                                   ") n++
      n--;

   if (n != tMem) apLogFailInfo( "string--multiple spaces failed", tMem, n,"");

   ob="foo"; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in "foo") n++
      n--;

   if (n != tMem) apLogFailInfo( "string--as single word failed", tMem, n,"");

   ob=" foo"; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in " foo") n++
      n--;

   if (n != tMem) apLogFailInfo( "string--as single word, leading space failed", tMem, n,"");

   ob="foo "; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in "foo ") n++
      n--;

   if (n != tMem) apLogFailInfo( "string--as single word, trailing space failed", tMem, n,"");

   ob="foo bar"; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in "foo bar") n++
      n--;

   if (n != tMem) apLogFailInfo( "string--as multiple word failed", tMem, n,"");

   ob=" foo bar"; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in " foo bar") n++
      n--;

   if (n != tMem) apLogFailInfo( "string--as multiple word, leading space failed", tMem, n,"");

   ob="foo bar "; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in "foo bar ") n++
      n--;

   if (n != tMem) apLogFailInfo( "string--as multiple word, trailing space failed", tMem, n,"");

   ob=""; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in "") n++
      n--;

   if (n != tMem) apLogFailInfo( "string--zls failed", tMem, n,"");


   apEndTest();
}



forin009();


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
