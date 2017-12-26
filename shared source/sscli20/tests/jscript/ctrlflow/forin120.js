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


var iTestID = 52802;


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

function verify(scen, ob, nob, cMem) 
{
    var tMem = cMem+5, n = 1;

// Added to handle strict mode in JScript 7.0
@cc_on 
    @if (@_fast)  
        var pOb;        
     
@end       

    pOb = new Object();
    pOb[0] = ob;

    pOb[0].a=pOb[0].b=pOb[0].c=pOb[0].d=pOb[0].e=null;

    for (var i in pOb[0]) {
        pOb[0][i]=n;
		n++;
	}
    n--;

    if (typeof ob == "string")
    {
        if (n != 0)
            apLogFailInfo( scen+"--"+nob+" failed ",tMem,n,"");
        return;
    }            

    var tExpect = factorial(tMem)/factorial(cMem);
	var tActual = pOb[0].a*pOb[0].b*pOb[0].c*pOb[0].d*pOb[0].e;

    if (n != tMem || (tActual != tExpect)) 
        apLogFailInfo( scen+"--"+nob+" failed ",tMem,n,"");
}


@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function forin120() {

    apInitTest("forIn120 ");

    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec");

// Fast mode does not allow the user to add expando properties to built-in objects
@if(!@_fast) 
    verify("built-in, non-executable",Math,"Math",0);
@end


    //----------------------------------------------------------------------------
    apInitScenario("2. built-in, exec, not instanciated");

// Fast mode does not allow the user to add expando properties to built-in objects
@if(!@_fast) 
    verify("built-in, exec, not instantiated",Array,"Array",0);
    verify("built-in, exec, not instantiated",Boolean,"Boolean",0);
    verify("built-in, exec, not instantiated",Date,"Date",0);
    verify("built-in, exec, not instantiated",Number,"Number",0);
    verify("built-in, exec, not instantiated",Object,"Object",0);
@end


    //----------------------------------------------------------------------------
    apInitScenario("3. built-in, exec, instanciated");

    verify("built-in, exec, instantiated",new Array(),"new Array()",0);
    verify("built-in, exec, instantiated",new Boolean(),"new Boolean()",0);
    verify("built-in, exec, instantiated",new Date(),"new Date()",0);
    verify("built-in, exec, instantiated",new Number(),"new Number()",0);
    verify("built-in, exec, instantiated",new Object(),"new Object()",0);


    //----------------------------------------------------------------------------
    apInitScenario("4. user-defined, not instanciated");

    // should have members: length, arguments, caller, and prototype

    verify("user-defined, not instantiated", obFoo, "obFoo", 0);


    //----------------------------------------------------------------------------
    apInitScenario("5. user-defined, instanciated");

    verify("user-defined, instantiated", new obFoo(), "new obFoo()", 0);


    //----------------------------------------------------------------------------
    apInitScenario("6. string");

    verify("string"," ","single space",0);
    verify("string","                                                                   ","multiple spaces",0);
    verify("string","foo","as single word",0);
    verify("string"," foo","as single word, leading space",0);
    verify("string","foo ","as single word, trailing space",0);
    verify("string","foo bar","as multiple word",0);
    verify("string"," foo bar","as multiple word, leading space",0);
    verify("string","foo bar ","as multiple word, trailing space",0);
    verify("string","","zls",0);


    apEndTest();

}




forin120();


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
