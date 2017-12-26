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


var iTestID = 53763;


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
	@if(@_fast) return a.undef;
	@else return a;
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

function tcatch05() {
	
    apInitTest("tCatch05: Nesting issues");

    //----------------------------------------------------------------------------
	apInitScenario("One try-catch nested in a try");

	try{
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
	}
	catch(e){

	}
	verify(res, "inner try", "");

	//----------------------------------------------------------------------------
	apInitScenario("Five try-catch nested in a try");

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
						catch(e){
@if(@_fast) 			verify(e.number, -2146827864, "");
@else					verify(e.number, -2146823279, "");
@end
			//International
			//verify(e.description, "'a' is undefined", "");
						}
					}
					catch(e){
					}
				}
				catch(e){
				}
			}
			catch(e){
			}
		}
		catch(e){
		}

	}
	catch(e){

	}
	verify(res, "inner try", "");
	
	//----------------------------------------------------------------------------
	apInitScenario("~100 try-catch nested in a try");
	@cc_on
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
@if(@_win32)
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
@end
			res = "inner try";
			throwUndef();
		}
		catch(e){
@if(@_fast) verify(e.number, -2146827864, "");
@else		verify(e.number, -2146823279, "");
@end
			//International
			//verify(e.description, "'a' is undefined", "");
		}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
@if(@_win32)
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
		catch(e){}}
@end
	catch(e){

	}
	verify(res, "inner try", "");

	//----------------------------------------------------------------------------
	apInitScenario("One try-catch-finally nested in a try");

	try{
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
	catch(e){

	}
	finally{
		verify(res, "inner finally", "");
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
						catch(e){
@if(@_fast) 				verify(e.number, -2146827864, "");
@else						verify(e.number, -2146823279, "");
@end
							//International
							//verify(e.description, "'a' is undefined", "");
							res= "inner catch";
						}
						finally{
							verify(res, "inner catch", "");
							res = "inner finally";
						}
					}
					catch(e){}
					finally{}
				}
				catch(e){}
				finally{}
			}
			catch(e){}
			finally{}
		}
		catch(e){}
		finally{}

	}
	catch(e){}
	finally{
		verify(res, "inner finally", "");
		res = "outer finally";
	}
	verify(res, "outer finally", "");
	
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
@if(@_win32)
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
@end
			res = "inner try";
			throwUndef();
		}
		catch(e){
@if(@_fast) verify(e.number, -2146827864, "");
@else		verify(e.number, -2146823279, "");
@end
			//International
			//verify(e.description, "'a' is undefined", "");
			res = "inner catch";
		}
		finally{
			verify(res, "inner catch", "");
			res = "inner finally";
		}
		}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
@if(@_win32)
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
		catch(e){}finally{}}
@end
	catch(e){

	}
	finally{
		verify(res, "inner finally", "");
		res= "outer finally";
	}
	verify(res, "outer finally", "");

	//----------------------------------------------------------------------------
	apInitScenario("One try-catch nested in a catch");

	try{
		throwUndef();
	}
	catch(e){
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
	}
	verify(res, "inner try", "");

	//----------------------------------------------------------------------------
	apInitScenario("Five try-catch nested in a catch");

	try{
		throwUndef();
	}
	catch(e){
		try{
			throwUndef();
		}	
		catch(e){
			try{
				throwUndef();
			}
			catch(e){
				try{
					throwUndef();
				}
				catch(e){
					try{
						throwUndef();
					}
					catch(e){
						try{
							res = "inner try";
							throwUndef();
						}
						catch(e){
@if(@_fast) 				verify(e.number, -2146827864, "");
@else						verify(e.number, -2146823279, "");
@end
							//International
							//verify(e.description, "'a' is undefined", "");
						}
					}
				}
			}
		}

	}
/*
	catch(e){

	}
*/
	verify(res, "inner try", "");
	
	//----------------------------------------------------------------------------
	apInitScenario("~100 try-catch nested in a catch");

		try{
			throwUndef();
		}
		catch(e){
		res = "outer try";
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
@if(@_win32)
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
@end
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
		}}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
@if(@_win32)
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
		}
@end
	}
	verify(res, "inner try", "");
	//----------------------------------------------------------------------------
	apInitScenario("One try-catch-finally nested in a catch");

	try{
		throwUndef();
	}
	catch(e){
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
	finally{
		verify(res, "inner finally", "");
		res = "outer finally";
	}
	verify(res, "outer finally", "");

	//----------------------------------------------------------------------------
	apInitScenario("Five try-catch-finally nested in a catch");

	try{
		throwUndef();
	}
	catch(e){
		try{
			throwUndef();
		}	
		catch(e){
			try{
				throwUndef();
			}
			catch(e){
				try{
					throwUndef();
				}
				catch(e){
					try{
						throwUndef();
					}
					catch(e){
						try{
							res = "inner try";
							throwUndef();
						}
						catch(e){
@if(@_fast) 				verify(e.number, -2146827864, "");
@else						verify(e.number, -2146823279, "");
@end
							//International
							//verify(e.description, "'a' is undefined", "");
						}
						finally{
							verify(res, "inner try", "");
							res = "inner finally1";
						}
					}
					finally{
						verify(res, "inner finally1", "");
						res = "inner finally2";
					}
				}
				finally{
					verify(res, "inner finally2", "");
					res = "inner finally3";
				}
			}
			finally{
				verify(res, "inner finally3", "");
				res = "inner finally4";
			}
		}
		finally{
			verify(res, "inner finally4", "");
			res = "inner finally5";
		}
	}
/*
	catch(e){

	}
*/
	finally{
		verify(res, "inner finally5", "");
		res = "inner finally6";
	}
	verify(res, "inner finally6", "");
	
	//----------------------------------------------------------------------------
	apInitScenario("~100 try-catch-finally nested in a catch");

@if(@_win32)
		try{
			throwUndef();
		}
		catch(e){
		res = "outer try";
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
@end
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
				try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
		try{
			throwUndef();
		}
		catch(e){
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
			verify(res,"inner try", "");
			res = 0;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
@if(@_win32)
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
		finally{
			res++;
		}
		}
	verify(res,101, "");
@else
	verify(res,24,"");
@end

	//----------------------------------------------------------------------------
	apInitScenario("One try-catch nested in a finally");

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
	}
	verify(res, "inner try", "");


	//----------------------------------------------------------------------------
	apInitScenario("Five try-catch nested in a finally");

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
	}
	}
	}
	}
	}	
	verify(res, "inner try", "");

	//----------------------------------------------------------------------------
	apInitScenario("~100 try-catch nested in a finally");

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
		res = 0;
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
	verify(res,99,"");

	apEndTest();
}


tcatch05();


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
