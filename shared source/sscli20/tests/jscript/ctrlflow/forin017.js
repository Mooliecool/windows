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


var iTestID = 52775;


// Replacement random number generator (auto-seeding)
function rand ()
{
   @if(@_fast) 
   {
      var hold = -1;
      var t, d;
      
      if (hold == -1) // Only used for initial seed
      {
          t = (d = new Date()).getTime() % 4294967295;
          t = ((t&0xFF000000)>>>24) | (t&0x00FF0000>>8) |
              ((t&0x0000FF00)<<8) | ((t&0x000000FF)<<24);        
          t = ((t&0x0F000F00)>>4) | ((t&0x00F000F0)<<4) | (t&0xF00FF00F);
          t = ((t&0x30303030)>>2) | ((t&0x0C0C0C0C)<<2) | (t&0xC3C3C3C3);
          t = ((t&0x44444444)>>1) | ((t&0x22222222)<<1) | (t&0x99999999);
          hold = t;
      }
      // LNC RNG Algorithm
      hold = (Math.abs(hold)*214013+2531011) % 4294967295;

      return ((hold>>>12)&0xffff)/0xffff;
   }
   @else
   {
      if (!rand.hold) // Only used for initial seed
      {
          var t = (rand.d=new Date()).getTime() % 4294967295;
          t = ((t&0xFF000000)>>>24) | (t&0x00FF0000>>8) |
              ((t&0x0000FF00)<<8) | ((t&0x000000FF)<<24);        
          t = ((t&0x0F000F00)>>4) | ((t&0x00F000F0)<<4) | (t&0xF00FF00F);
          t = ((t&0x30303030)>>2) | ((t&0x0C0C0C0C)<<2) | (t&0xC3C3C3C3);
          t = ((t&0x44444444)>>1) | ((t&0x22222222)<<1) | (t&0x99999999);
          rand.hold = t;
      }
      // LNC RNG Algorithm
      rand.hold = (Math.abs(rand.hold)*214013+2531011) % 4294967295;

      return ((rand.hold>>>12)&0xffff)/0xffff;
   }   
   @end
}


// Returns the avg of a param array of numbers
function avg ()
{
    // arguments[] array not accepted in fast mode
    @if(!@_fast) 
        var argv=avg.arguments, tot=0;
        for ( var i = 0; i < argv.length; i++ )
            tot += argv[i];
        return tot/argv.length;
    @end     
}


// Returns the factorial of the number passed in
function factorial (val)
{
    var res=1;
    for (var i=2; i<=val; i++)
        res*=i;
    return res;
}


// Equiv to the VBA Sgn function
function sgn (val)
{
    if (val > 0) return 1;
    if (val < 0) return -1;

    return 0;
}


// trims off any fractional values--necessary to handle numbers greater than INT_MAX
function fix (num)
{
    if (typeof num != "number") num = parseFloat(num);
    if (num < Math.pow(2,31))
        return parseInt( num );
    
    var i = (""+num).indexOf(".");
    if ( (""+num).indexOf("e") == -1 && i != -1 )
        num = (""+num).substring(0,i);

    return parseFloat( num );
}


@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function forin017() 
{
   apInitTest("forIn017 ");

// Added to handle strict mode in JScript 7.0
@cc_on 
    @if (@_fast)  
        var ob, cMem, tMem, n, tExpect, tActual
     
@end       

   var i;


   //----------------------------------------------------------------------------
   apInitScenario("1. built-in, non-exec"); 
   
   ob = Math; cMem=0;
   tMem=cMem+5; n=1; i=null;
    
   // For more info, please check VS bug 190534.
   @cc_on 
   @if (!@_fast)
      ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;       
   @else 
      ob[1234567]=ob["bar"]=ob[-1.23e45]=null;
   @end 
    
   for (i in Math)
      Math[i]=n++;
      n--;

   tExpect = factorial(tMem)/factorial(cMem);

   // For more info, please check VS bug 190534.
   @cc_on 
   @if (!@_fast)
      tActual = ob.foo*ob[1234567]*ob["bar"]*ob.bin*ob[-1.23e45];       
   @else 
      tActual = ob[1234567]*ob["bar"]*ob[-1.23e45];
   @end 
   
   @if(@_fast) 
      if (n != 0)
         apLogFailInfo ("Error in 1.", 0, n, "");    
   @else
      if (n != tMem || (tActual != tExpect)) 
         apLogFailInfo( "built-in, non-executable--Math failed",tMem,n,"");
   @end         


   //----------------------------------------------------------------------------
   apInitScenario("2. built-in, exec, not instanciated");

   // all should have members: length, arguments, caller, and prototype

   ob=Array; cMem=0;
   tMem=cMem+5; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in Array)
      Array[i]=n++;
      n--;

   tExpect = factorial(tMem)/factorial(cMem);
   tActual = ob.foo*ob[1234567]*ob["bar"]*ob.bin*ob[-1.23e45];

   @if(@_fast) 
      if (n != 0)
         apLogFailInfo ("Error in 2.1", 0, n, "");    
   @else
      if (n != tMem || (tActual != tExpect)) 
         apLogFailInfo( "built-in, exec, not instantiated--Array failed", tMem, n,"");
   @end         
   
   //-----------------------------------------
   
   ob=Boolean; cMem=0;
   tMem=cMem+5; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in Boolean)
      Boolean[i]=n++;
      n--;

   tExpect = factorial(tMem)/factorial(cMem);
   tActual = ob.foo*ob[1234567]*ob["bar"]*ob.bin*ob[-1.23e45];

   @if(@_fast) 
      if (n != 0)
         apLogFailInfo ("Error in 2.2", 0, n, "");    
   @else
      if (n != tMem || (tActual != tExpect)) 
         apLogFailInfo( "built-in, exec, not instantiated--Boolean failed", tMem, n,"");
   @end         

   //-----------------------------------------

   ob=Date; cMem=0;
   tMem=cMem+5; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in Date)
      Date[i]=n++;
      n--;

   tExpect = factorial(tMem)/factorial(cMem);
   tActual = ob.foo*ob[1234567]*ob["bar"]*ob.bin*ob[-1.23e45];

   @if(@_fast) 
      if (n != 0)
         apLogFailInfo ("Error in 2.3", 0, n, "");    
   @else
      if (n != tMem || (tActual != tExpect)) 
         apLogFailInfo( "built-in, exec, not instantiated--Date failed", tMem, n,"");
   @end         
   
   //-----------------------------------------

   ob=Number; cMem=0;
   tMem=cMem+5; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in Number)
      Number[i]=n++;
      n--;

   tExpect = factorial(tMem)/factorial(cMem);
   tActual = ob.foo*ob[1234567]*ob["bar"]*ob.bin*ob[-1.23e45];

   @if(@_fast) 
      if (n != 0)
         apLogFailInfo ("Error in 2.4", 0, n, "");    
   @else
      if (n != tMem || (tActual != tExpect)) 
         apLogFailInfo( "built-in, exec, not instantiated--Number failed", tMem, n,"");
   @end         

   //-----------------------------------------

   ob=Object; cMem=0;
   tMem=cMem+5; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in Object)
      Object[i]=n++;
      n--;

   tExpect = factorial(tMem)/factorial(cMem);
   tActual = ob.foo*ob[1234567]*ob["bar"]*ob.bin*ob[-1.23e45];

   @if(@_fast) 
      if (n != 0)
         apLogFailInfo ("Error in 2.5", 0, n, "");    
   @else
      if (n != tMem || (tActual != tExpect)) 
         apLogFailInfo( "built-in, exec, not instantiated--Object failed", tMem, n,"");
   @end         


   //----------------------------------------------------------------------------
   apInitScenario("3. user-defined, not instanciated");

   // should have members: length, arguments, caller, and prototype

   ob=obFoo; cMem=0;
   tMem=cMem+5; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in obFoo)
      obFoo[i]=n++;
      n--;

   tExpect = factorial(tMem)/factorial(cMem);
   tActual = ob.foo*ob[1234567]*ob["bar"]*ob.bin*ob[-1.23e45];

   if (n != tMem || (tActual != tExpect)) 
      apLogFailInfo( "user-defined, not instantiated failed", tMem, n,"");


   //----------------------------------------------------------------------------
   apInitScenario("4. string");

   ob=" "; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in " ")
      " "[i]=n++;
      n--;

   if (n != 0) apLogFailInfo( "string--single space failed", tMem, n,"");

   ob="                                                                   "; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in "                                                                   ")
      "                                                                   "[i]=n++;
      n--;

   if (n != 0) apLogFailInfo( "string--multiple spaces failed", tMem, n,"");

   ob="foo"; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in "foo")
      "foo"[i]=n++;
      n--;

   if (n != 0) apLogFailInfo( "string--as single word failed", tMem, n,"");

   ob=" foo"; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in " foo")
      " foo"[i]=n++;
      n--;

   if (n != 0) apLogFailInfo( "string--as single word, leading space failed", tMem, n,"");

   ob="foo "; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in "foo ")
      "foo "[i]=n++;
      n--;

   if (n != 0) apLogFailInfo( "string--as single word, trailing space failed", tMem, n,"");

   ob="foo bar"; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in "foo bar")
      "foo bar"[i]=n++;
      n--;

   if (n != 0) apLogFailInfo( "string--as multiple word failed", tMem, n,"");

   ob=" foo bar"; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in " foo bar")
      " foo bar"[i]=n++;
      n--;

   if (n != 0) apLogFailInfo( "string--as multiple word, leading space failed", tMem, n,"");

   ob="foo bar "; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in "foo bar ")
      "foo bar "[i]=n++;
      n--;

   if (n != 0) apLogFailInfo( "string--as multiple word, trailing space failed", tMem, n,"");

   ob=""; cMem=0;
   tMem=cMem; n=1; i=null;
   ob.foo=ob[1234567]=ob["bar"]=ob.bin=ob[-1.23e45]=null;

   for (i in "")
      ""[i]=n++;
      n--;

   if (n != 0) apLogFailInfo( "string--zls failed", tMem, n,"");


   apEndTest();
}



forin017();


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
