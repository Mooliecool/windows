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


var iTestID = 226779;

//////////////////////////////////////////////////////////////////////////////
//		This testcase tests the usage of assembly attributes
//
//
//
//
//
import System
import System.Reflection


[assembly: AssemblyVersion("1.0.*")]
[assembly: AssemblyCulture("")]	

[assembly: AssemblyProduct("my product")]
[assembly: AssemblyTrademark("stoopid (tm)")]
[assembly: AssemblyInformationalVersionAttribute("funky version string")]
[assembly: AssemblyCompany("xxx inc.")]
[assembly: AssemblyCopyright("mine! gimmie!")]

@if(!@rotor)
[assembly: AssemblyFileVersionAttribute("file version attribute")]
[assembly: AssemblyKeyName("")]
@end

[assembly: System.Reflection.AssemblyTitle("Microsoft JScript Compiler")]
[assembly: AssemblyDescription("why ask why?")]
[assembly: AssemblyConfiguration("fried")]
[assembly: AssemblyDefaultAliasAttribute("default alias")]

[assembly: AssemblyDelaySign(false)]



function asmattr01() {

  var res

  apInitTest("AsmAttr01");

  apInitScenario("verify attributes persisted");

  var asm:int;
  var ad = AppDomain.CurrentDomain.GetAssemblies();
  for (asm = 0; asm < ad.Length; asm++){
	if (ad[asm].FullName.ToLower().IndexOf("asmattr01") >= 0) break
  }

  var attribs = AppDomain.CurrentDomain.GetAssemblies()[asm].GetCustomAttributes(true)
  var i:int

//  var a
//  for (a = 0; a < attribs.Length; a++){
//  print(a + "  : " + attribs[a].GetType())
//  }

  i=0
  while (attribs[i].GetType().ToString() != "System.Reflection.AssemblyProductAttribute") i++
  if (attribs[i].Product != "my product") apLogFailInfo("wrong value", "my product", attribs[i].Product, "")

  i=0
  while (attribs[i].GetType().ToString() != "System.Reflection.AssemblyTrademarkAttribute") i++
  if (attribs[i].Trademark != "stoopid (tm)") apLogFailInfo("wrong value", "stoopid (tm)", attribs[i].Trademark, "")

  i=0
  while (attribs[i].GetType().ToString() != "System.Reflection.AssemblyInformationalVersionAttribute") i++
  if (attribs[i].InformationalVersion != "funky version string") apLogFailInfo("wrong value", "funky version string", attribs[i].InformationalVersion, "")

  i=0
  while (attribs[i].GetType().ToString() != "System.Reflection.AssemblyCompanyAttribute") i++
  if (attribs[i].Company != "xxx inc.") apLogFailInfo("wrong value", "richiem inc.", attribs[i].Company, "")

  i=0
  while (attribs[i].GetType().ToString() != "System.Reflection.AssemblyCopyrightAttribute") i++
  if (attribs[i].Copyright != "mine! gimmie!") apLogFailInfo("wrong value", "mine! gimmie!", attribs[i].Copyright, "")
@if(!@rotor)
  i=0
  while (attribs[i].GetType().ToString() != "System.Reflection.AssemblyFileVersionAttribute") i++
  if (attribs[i].Version != "file version attribute") apLogFailInfo("wrong value", "file version attribute", attribs[i].Version, "")
@end
  i=0
  while (attribs[i].GetType().ToString() != "System.Reflection.AssemblyTitleAttribute") i++
  if (attribs[i].Title != "Microsoft JScript Compiler") apLogFailInfo("wrong value", "Microsoft JScript Compiler", attribs[i].Title, "")

  i=0
  while (attribs[i].GetType().ToString() != "System.Reflection.AssemblyDescriptionAttribute") i++
  if (attribs[i].Description != "why ask why?") apLogFailInfo("wrong value", "why ask why?", attribs[i].Description, "")

  i=0
  while (attribs[i].GetType().ToString() != "System.Reflection.AssemblyConfigurationAttribute") i++
  if (attribs[i].Configuration != "fried") apLogFailInfo("wrong value", "fried", attribs[i].Configuration, "")

  i=0
  while (attribs[i].GetType().ToString() != "System.Reflection.AssemblyDefaultAliasAttribute") i++
  if (attribs[i].DefaultAlias != "default alias") apLogFailInfo("wrong value", "default alias", attribs[i].DefaultAlias, "")


  apEndTest();


}

asmattr01();


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
