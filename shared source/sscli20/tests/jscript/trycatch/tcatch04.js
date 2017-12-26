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


var iTestID = 53762;


function verify(sAct, sExp, sMes, bugNum){
	if(bugNum==null) bugNum = "";
	if (sAct != sExp)
        apLogFailInfo(sMes, sExp, sAct, bugNum);
}

var x, a, b;
var res1 = new Object();
var res2 = new Object();

function cObj() { this.test = "pass"; }

function retTry(){
	try{return "in try";}
	catch(e){return "in catch";}
}

function retCatch(){
	try{
		x = a.err1;
		return "in try";
	}
	catch(e){return "in catch";}
}

function retFinally(){
	try{return "in try";}
	catch(e){return "in catch";}
	finally{return "in finally";}
}

function retTryFinally(){
	try{return "in try";}
	finally{return "in finally";}
}

try{x = a.undef;}
catch(e){
	res1.number = e.number;
	res1.description = e.description;
}

try{x = b();}
catch(e){
	res2.number = e.number;
	res2.description = e.description;
}finally{res2.result = "in finally";}

function tcatch04() {
	var i, j, k, res, loc_id, depth, err;
	loc_id = apGetLocale();

    apInitTest("tCatch04: Scope issues"); 

    //----------------------------------------------------------------------------
    apInitScenario("4.01:  Try in global scope");
	
    verify(res2.result,"in finally","res2.result:  4.011","");

	@if(@_jscript_version < 7)
		verify(res1.number,-2146823281,"res1.number:  4.012","");
		verify(res2.number,-2146823281,"res2.number:  4.013","");
		if(loc_id==1033){
			verify(res1.description,"'a.undef' is null or not an object","res1.description:  4.014","");
			verify(res2.description,"Object expected","res2.description:  4.015","");
		}
	@else
		verify(res1.number,-2146827864,"res1.number:  4.012","");
		verify(res2.number,-2146823286,"res2.number:  4.016","");
		if(loc_id==1033){
			verify(res1.description,"Object required","res1.description:  4.017","");
			verify(res2.description,"Function expected","res2.description:  4.018","");
		}
	@end

	//----------------------------------------------------------------------------
	apInitScenario("4.02:  Try in a local function");

	res= "unchanged";
	try{eval("f");}
	catch(e){
		@if(@_fast)
			verify(e.number,-2146827153,"wrong error:  4.0201","");
			if(loc_id==1033) verify(e.description,"Variable 'f' has not been declared","wrong error msg:  4.0202","");
			res = "in catch";
		@else
			verify(e.number,-2146823279,"wrong error:  4.0203","");
			if(loc_id==1033) verify(e.description,"'f' is undefined","wrong error msg:  4.0204","");
			res = "in catch";
		@end
	}
	verify(res, "in catch", "catch not performed:  4.0205","");

	res= "unchanged";
	try{eval("f");}
	catch(e){
		@if(@_fast)
			verify(e.number,-2146827153,"wrong error:  4.0206","");
			if(loc_id==1033) verify(e.description,"Variable 'f' has not been declared","wrong error msg:  4.0207","");
			res = "in catch";
		@else
			verify(e.number,-2146823279,"wrong error:  4.0208","");
			if(loc_id==1033) verify(e.description,"'f' is undefined","wrong error msg:  4.0209","");
			res = "in catch";
		@end
	}
	finally{
		verify(res, "in catch", "catch not performed:  4.0210","");
		res = "in finally";
	}
	verify(res, "in finally", "finally not performed:  4.0211","");

	//----------------------------------------------------------------------------
	apInitScenario("4.03:  Try nested deep within function calls");
	@if(@_win32)
		depth = 100;
	@else
		depth = 15;
	@end

	function rec(a){
		if(a == 0){
			try{eval("1+");}
			catch(e){
				@if(@_jscript_version<7)
					verify(e.number, -2146827286,"wrong error:  4.031","");
					if(loc_id==1033) verify(e.description, "Syntax error", "wrong error msg:  4.032","");
				@else
					verify(e.number, -2146827093,"wrong error:  4.033","");
					if(loc_id==1033) verify(e.description, "Expected expression", "wrong error msg:  4034","");
				@end
			}
		}
		else{rec(a-1);}
	}
	rec(depth);

	//----------------------------------------------------------------------------
	apInitScenario("4.04:  Try in an if block");

	if(true){
		try{eval(":");}
		catch(e){
			@if(@_jscript_version<7)
				verify(e.number, -2146827286,"wrong error:  4.041","");
				if(loc_id==1033) verify(e.description,"Syntax error","wrong error msg:  4.042","");
			@else
				verify(e.number, -2146827093,"wrong error:  4.043","");
				if(loc_id==1033) verify(e.description,"Expected expression","wrong error msg:  4.044","");
			@end
		}

	}

	if(true){
		try{eval(":");}
		catch(e){
			@if(@_jscript_version<7)
				verify(e.number,-2146827286,"wrong error:  4.045","");
				if(loc_id==1033) verify(e.description,"Syntax error","wrong error msg:  4.046","");
			@else
				verify(e.number,-2146827093,"wrong error:  4.047","");
				if(loc_id==1033) verify(e.description,"Expected expression","wrong error msg:  4.048","");		
			@end
		}
		finally{res = "in finally";}
		verify(res, "in finally","finally not executed:  4.049","");
	}

	//----------------------------------------------------------------------------
	apInitScenario("4.05:  Try in an while block");

	i = 0;
	while(true){
		if(i++ > depth) break;
		try{eval(":");}
		catch(e){
			@if(@_jscript_version<7)
				verify(e.number,-2146827286,"wrong error:  4.051","");
				if(loc_id==1033) verify(e.description,"Syntax error","wrong error msg:  4.052","");
			@else
				verify(e.number,-2146827093,"wrong error:  4.053","");
				if(loc_id==1033) verify(e.description, "Expected expression","wrong error msg:  4.054","");
			@end
		}
	}

	i = 0;
	while(true){
		if(i++ > depth) break;
		try{eval(":");}
		catch(e){
			@if(@_jscript_version<7)
				verify(e.number,-2146827286,"wrong error:  4.055","");
				if(loc_id==1033) verify(e.description,"Syntax error","wrong error msg:  4.056","");
			@else
				verify(e.number,-2146827093,"wrong error:  4.057","");
				if(loc_id==1033) verify(e.description,"Expected expression","wrong error msg:  4.058","");	
			@end
		}
		finally{res = "in finally";}
		verify(res, "in finally","finally note executed:  4.059","");
	}

	//----------------------------------------------------------------------------
	apInitScenario("4.06:  Try in an for loop");

	for(i=0; i<depth; i++){
		try{eval(":");}
		catch(e){
			@if(@_jscript_version<7)
				verify(e.number,-2146827286,"wrong error:  4.061","");
				if(loc_id==1033) verify(e.description,"Syntax error","wrong error msg:  4.062","");
			@else
				verify(e.number,-2146827093,"wrong error:  4.063","");
				if(loc_id==1033) verify(e.description,"Expected expression","wrong error msg:  4.064","");
			@end
		}

	}

	for(i=0; i<depth; i++){
		try{eval(":");}
		catch(e){
			@if(@_jscript_version<7)
				verify(e.number,-2146827286,"wrong error:  4.065","");
				if(loc_id==1033) verify(e.description,"Syntax error","wrong error msg:  4.066","");
			@else
				verify(e.number,-2146827093,"wrong error:  4.067","");
				if(loc_id==1033) verify(e.description,"Expected expression","wrong error msg:  4.068","");
			@end
		}
		finally{res = "in finally";}
		verify(res, "in finally","finally not executed:  4.069","");
	}

	//----------------------------------------------------------------------------
	apInitScenario("4.07:  Try in a switch block");

	i = 0;
	switch(i){
		
		case 0:
		try{eval(":");}
		catch(e){
			@if(@_jscript_version<7)
				verify(e.number,-2146827286,"wrong error:  4.0701","");
				if(loc_id==1033) verify(e.description,"Syntax error","wrong error msg:  4.0702","");
			@else
				verify(e.number,-2146827093,"wrong error:  4.0703","");
				if(loc_id==1033) verify(e.description,"Expected expression","wrong error msg:  4.0704","");
			@end
		}
		res = "out";
		break;
		default:
			res = "in";
			break;
	}
	verify(res,"out","default performed:  4.075","");

	i=0;
	switch(i){
		
		case 0:
		try{eval(":");}
		catch(e){
			@if(@_jscript_version<7)
				verify(e.number,-2146827286,"wrong error:  4.0706","");
				if(loc_id==1033) verify(e.description,"Syntax error","wrong error msg:  4.0707","");
			@else
				verify(e.number,-2146827093,"wrong error:  4.0708","");
				if(loc_id==1033) verify(e.description,"Expected expression","wrong error msg:  4.0709","");
			@end
		}
		finally{res = "in finally";}
		verify(res,"in finally","finally not executed:  4.0710","");
		res = "out"
		break;
		default:
			res = "in";
			break;
	}
	verify(res,"out","default performed:  4.0711","");

	//----------------------------------------------------------------------------
	apInitScenario("4.08:  Return from a try, catch, and finally block");

	verify(retTry(),"in try","retTry:  4.081","");
	verify(retCatch(),"in catch","retCatch:  4.082","");
	verify(retFinally(),"in finally","retFinally:  4.083",211204);
	verify(retTryFinally(),"in finally","retTryFinally:  4.084","");

	//----------------------------------------------------------------------------
	apInitScenario("4.09:  Try in an eval");

	x = eval("try{ eval('1+');} catch(e){e;}");
	@if(@_jscript_version<7)
		verify(x.number,-2146827286,"wrong error:  4.091","");
		if(loc_id==1033) verify(x.description,"Syntax error","wrong error msg:  4.092","");
	@else
		verify(x.number,-2146827093,"wrong error:  4.093","");
		if(loc_id==1033) verify(x.description,"Expected expression","wrong error msg:  4.094","");
	@end

	x=eval("function a(){try{eval('1+');} catch(e){return e;}} a();", "unsafe");
	@if(@_jscript_version<7)
		verify(x.number,-2146827286,"wrong error:  4.095","");
		if(loc_id==1033) verify(x.description,"Syntax error","wrong error msg:  4.096","");
	@else
		verify(x.number,-2146827093,"wrong error:  4.097","");
		if(loc_id==1033) verify(x.description,"Expected expression","wrong error msg:  4.098","");
	@end

	//----------------------------------------------------------------------------
	apInitScenario("4.10:  Try in a with");

	err = new Error();
	with(err){
		description = "hi";
		try{x = k.undef;}
		catch(e){
			@if(@_jscript_version < 7)
				if(loc_id==1033) verify(e.description,"'k.undef' is null or not an object","wrong error msg:  4.101","");
				verify(e.number,-2146823281,"wrong error:  4.102","");
			@else
				if(loc_id==1033) verify(e.description,"Object required","wrong error msg:  4.101","");
				verify(e.number,-2146827864,"wrong error:  4.102","");
			@end
		}
		verify(description,"hi","expando on Error object:  4.103","");
	}

	//----------------------------------------------------------------------------
	apInitScenario("4.11:  Try in a for...in");

	err = new Error();
	for(i in err){
		try{x = k();}
		catch(e){
			for(j in e){
				if(j == "number")
					verify(eval("e." + j),-2146823281,"wrong error:  4.111","")
				else{
				   if((loc_id==1033)&&(j == "message"))
				      verify(eval("e." + j),"Object expected","wrong error msg:  4.112","")
				}
				
			}
		}
	}

	apEndTest();
}


tcatch04();


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
