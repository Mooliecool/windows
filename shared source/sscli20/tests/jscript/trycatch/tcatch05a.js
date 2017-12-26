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


var iTestID = 141556;


@if(@_fast)
   var a, res, res1;
@end
var undef;
function verify(sAct, sExp, sMes){
	if (sAct != sExp)
        apLogFailInfo( "Scenario failed: "+sMes, sExp, sAct, "");
}
function throwOverflow(){
		return throwOverflow();
}
function throwUndef(){
@if(@_fast)	return a.undef;
@else		return a;
@end
}
function throwObj(){
	return a();
}
function throwNum(){
	throw 123.456;
}
function throwString(){
	throw "Test string";
}
function custObj(){
	this.prop1 = "test";
}

function tcatch05a() {

    apInitTest("tCatch05a: Nesting issues");

	apInitScenario("One try-catch-finally nested in a finally");

	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
		res = "outer try";
		try{
			res = "inner try";
			throwUndef();
		}
		catch(e){
@if(@_fast) verify(e.number, -2146827864, "");
@else		verify(e.number, -2146823279, "");
@end
			//International
			//verify(e.description, "'a' is undefined", "");
		}
		finally{
			verify(res, "inner try", "");
			res = "inner finally";
		}
	}
	verify(res, "inner finally", "");


	//----------------------------------------------------------------------------
	apInitScenario("Five try-catch-finally nested in a finally");

	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
		res = "outer try";
		try{
			res1 = "inner try";
			throwUndef();
		}
		catch(e){
@if(@_fast) verify(e.number, -2146827864, "");
@else		verify(e.number, -2146823279, "");
@end
			//International
			//verify(e.description, "'a' is undefined", "");
		}
		finally{
			res =0;
		}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}	
	verify(res1, "inner try", "");
	verify(res, 5, "");

	//----------------------------------------------------------------------------
	apInitScenario("~100 try-catch-finally nested in a finally");

	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
	try{
		throwUndef();
	}
	catch(e){
		
	}
	finally{
		res = "outer try";
		try{
			res1 = "inner try";
			throwUndef();
		}
		catch(e){
@if(@_fast) verify(e.number, -2146827864, "");
@else		verify(e.number, -2146823279, "");
@end
			//International	
			//verify(e.description, "'a' is undefined", "");
		}
		finally{
			res =0;
		}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}	
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}		
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}		
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}		
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}		
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}		
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}		
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}		
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}		
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}
	res++;
	}			
	verify(res1, "inner try", "");
	verify(res, 100, "");

	//----------------------------------------------------------------------------
	apInitScenario("One try-finally nested in a try");

	try{
		res = "outer try";
		try{
			res = "inner try";
			throwUndef();
		}
		finally{
			verify(res, "inner try", "");
			res = "inner finally";
		}
	}
	catch(e){
		verify(res, "inner finally", "");
		res = "outer catch";
@if(@_fast) verify(e.number, -2146827864, "");
@else		verify(e.number, -2146823279, "");
@end
		//International
		//verify(e.description, "'a' is undefined", "");
	}
	finally{
		verify(res, "outer catch", "");
		res = "outer finally";
	}
	verify(res, "outer finally", "");

	//----------------------------------------------------------------------------
	apInitScenario("Five try-catch-finally nested in a try");

	try{
		res = "outer try";
		try{
			try{
				try{
					try{
						try{
							res = "inner try";
							throwUndef();
						}
						finally{
							verify(res, "inner try","");
							res = "inner finally";
						}
					}
					finally{}
				}
				finally{}
			}
			finally{}
		}
		finally{}

	}
	catch(e){
		verify(res, "inner finally", "");
		res = "outer catch";
@if(@_fast) verify(e.number, -2146827864, "");
@else		verify(e.number, -2146823279, "");
@end
		//International
		//verify(e.description, "'a' is undefined", "");
	}
	finally{
		
	}
	verify(res, "outer catch", "");
	
	//----------------------------------------------------------------------------
	apInitScenario("~100 try-catch-finally nested in a try");

	try{
		res = "outer try";
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
		try{
			res = "inner try";
			throwUndef();
		}
		finally{
			verify(res, "inner try", "");
			res = "inner finally";
		}
		}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
		finally{}}
	catch(e){
@if(@_fast) verify(e.number, -2146827864, "");
@else		verify(e.number, -2146823279, "");
@end
		//International
		//verify(e.description, "'a' is undefined", "");
		verify(res, "inner finally","");
		res = "outer catch";
	}
	finally{
		verify(res, "outer catch", "");
		res= "outer finally";
	}
	verify(res, "outer finally", "");

	//----------------------------------------------------------------------------

	apInitScenario("Try with ~100 catches - not yet running");
/*
	res = 0;
	try{
		x = new Error;
		x.number = 123.123;
		x.description = "asd";
		throw x;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Number){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof Date){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof String){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Function){
		res++;
	}
	catch(e: e instanceof Error){
		verify(e.number, 123.123,"");
		//International
		//verify(e.description, "asd","");
		res++;
		end = "in correct catch";
	}
	finally{
		verify(res, 1,"");
		verify(end, "in correct catch", "");
		res = "end of test";
	}
	verify(res, "end of test", "");
*/

  apInitScenario("cascading errors though 5 nested finallys");

  var lev = 0
  try{
    try{
      try{
        try{
          try{
            throw("111")
          }catch(e){
            if (e != "111") apLogFailInfo("unexpected error", "111", e, "")
            throw("112")
          }finally{
            lev++
          }
        }catch(e){
          if (e != "112") apLogFailInfo("unexpected error", "112", e, "")
          throw("113")
        }finally{
          lev++
        }
      }catch(e){
        if (e != "113") apLogFailInfo("unexpected error", "113", e, "")
        throw("114")
      }finally{
        lev++
      }
    }catch(e){
      if (e != "114") apLogFailInfo("unexpected error", "114", e, "")
      throw("115")
    }finally{
      lev++
    }
  }catch(e){
    if (e != "115") apLogFailInfo("unexpected error", "115", e, "")
  }finally{
    lev++
  }
  if (lev != 5) apLogFailInfo("a finally was skipped", 5, lev, "")



	apEndTest();
}


tcatch05a();


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
