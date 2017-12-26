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


var iTestID = 227043;

/* -------------------------------------------------------------------------
  Test: 	dotnet12 (dotnet12.js)
   
  Product:	JScript
 
  Area:		CLR
 
  Purpose:	Some practice with System.Reflection and System.Refelction.Emit
 
  Scenarios:	
		
  Notes: 	
 ---------------------------------------------------------------------------
  
 
	[0] 	qiongou -Created 11/27/2001
 -------------------------------------------------------------------------*/

/*---------------------------------*/
@if(@aspx)
	</script>
	<%@ import namespace="System.Reflection" %>
	<%@ import namespace="System.Runtime.Remoting" %>
	<%@ import namespace="System.Reflection.Emit" %>
	<script language=jscript runat=server>
@else
	import System;
	import System.Reflection;
	import System.Runtime.Remoting; 
	import System.Reflection.Emit;
@end


/* this is for greeting.dll */
public abstract class Greeting {
	public abstract function SayHello() : String ;
}

/* these two classes for english.dll, but also use for scenario 4 */
public class AmericanGreeting extends Greeting {
	private var msg: String = "Hi";
	public override function SayHello() : String {
		return msg;
	}
}

public class BritishGreeting extends Greeting {
	private var msg: String = "Hello";
	public override function SayHello() : String {
		return msg;
	}
}
var sAct="";
var sExp = "";
var sActErr = "";
var sExpErr = "";
var sErrThr = "";

//***********************************
function dotnet12()
{
	
	apInitTest("dotnet12");
	
	apInitScenario ("1.Reflecting over a Type Hierarchy");
	var o: Object = new String("ABC");
	var t: Type = o.GetType();
	
	try {
		var mem : MemberInfo[] = t.GetMembers(BindingFlags.Public | BindingFlags.Instance);
		var fieldin = 0;
		sExp = "Field"
		for (var index : int in mem)
		{
			sAct = mem[index].MemberType;
			if (sAct == sExp)
				fieldin = 1;
		}
		//if (sAct != sExp ) 
		if (fieldin == 0) //we couldn't depend on the order of sequence mem which may change
			apLogFailInfo ("1.Reflecting over a Type Hierarchy",sExp,sAct,"");
	}
	catch (e ) {
			apLogFailInfo ("1. SecurityException: "+e.description,"","","");
	}
	
	apInitScenario ("4. using reflection to change private member value");
	try {
		var bg : BritishGreeting = new BritishGreeting();
		var t4: Type = bg.GetType();
		var f4: FieldInfo = t4.GetField("msg", BindingFlags.NonPublic | BindingFlags.Instance);
		sExp = "Good morning"
		f4.SetValue(bg, sExp);
		sAct = bg.SayHello();
		if (sAct != sExp )
			apLogFailInfo("4. using reflection to change private member value",sExp,sAct,"");
	}
	catch (e){
			apLogFailInfo("4. exception","",e.description,"");
	}
	
	apInitScenario ("5. Creating New Types at Runtime");

	try {
		
		var ad : AppDomain = AppDomain.CurrentDomain;
		var an : AssemblyName = new AssemblyName();
		an.Name = "MyAssembly";
		var ab: AssemblyBuilder = ad.DefineDynamicAssembly(an, AssemblyBuilderAccess.Run);
		
	
		var modb: ModuleBuilder = ab.DefineDynamicModule("MyModule");
		var tb: TypeBuilder = modb.DefineType("HiThere", TypeAttributes.Public);
		
		
		var mb: MethodBuilder = tb.DefineMethod("SayHi", MethodAttributes.Public, null, null);
		
		
		var ilg : ILGenerator = mb.GetILGenerator();
		ilg.EmitWriteLine("Never");
	
		ilg.Emit(OpCodes.Ret);
		
		
		var t5: Type =tb.CreateType();
		var o5: Object = Activator.CreateInstance(t5);
		t5.GetMethod("SayHi").Invoke(o5,null);
	}
	catch (e) {
		apLogFailInfo("5. exception","",e.description,"");
	}
	
	apEndTest ();
}

	

dotnet12();


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
