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


var iTestID = 53697;



@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function tostr007() {
    var VersionLF="\n";

 @if(@_fast)
    var sExp, sAct, vDate;
 @end
 
    @cc_on
    if (parseFloat(@_jscript_version)>=7)
      VersionLF = "";

    apInitTest( "toStr007 " );

    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec");

//    sExp = VersionLF + "function Math() {\n    [native code]\n}" + VersionLF;
    sExp = "[object Math]";
    sAct = Math+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, non-exec--Math failed ", sExp, sAct,"");


    //----------------------------------------------------------------------------
    apInitScenario("2. built-in, exec, not instanciated");

    sExp = VersionLF + "function Array() {\n    [native code]\n}" + VersionLF;
    sAct = Array+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst--Array failed ", sExp, sAct,"");

    sExp = VersionLF + "function Boolean() {\n    [native code]\n}" + VersionLF;
    sAct = Boolean+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst, exec--Boolean failed ", sExp, sAct, "");

    sExp = VersionLF + "function Date() {\n    [native code]\n}" + VersionLF;
    sAct = Date+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst--Date failed ", sExp, sAct, "");

    sExp = VersionLF + "function Number() {\n    [native code]\n}" + VersionLF;
    sAct = Number+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst--Number failed ", sExp, sAct, "");

    sExp = VersionLF + "function Object() {\n    [native code]\n}" + VersionLF;
    sAct = Object+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst--Object failed ", sExp, sAct, "");

    sExp = VersionLF + "function String() {\n    [native code]\n}" + VersionLF;
    sAct = String+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst--String failed ", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("3. built-in, exec, instanciated");

    sExp = "";
    sAct = (new Array())+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec, instanciated--new Array() failed ", sExp, sAct, "");

    sExp = "false";
    sAct = (new Boolean())+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec, instanciated--new Boolean() failed ", sExp, sAct, "");

    if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
    sExp = "Mon Jan 1 00:00:00 PST 1996";
    sAct = (new Date(96,0,1))+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec, instanciated--new Date(96,0,1) failed ", sExp, sAct, "");
    }

    sExp = "0";
    sAct = (new Number())+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec, instanciated--new Number() failed ", sExp, sAct, "");

    sExp = "[object Object]";
    sAct = (new Object())+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec, instanciated--new Object() failed ", sExp, sAct, "");

    sExp = "";
    sAct = (new String())+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec, instanciated--new String() failed ", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("4. user-defined, not instanciated");


@if(!@aspx)
    sExp = "function obFoo() {}";
@else
    sExp = "expando function obFoo() {}";
@end

    sAct = obFoo+"";

    if (sAct != sExp)
        apLogFailInfo( "user-defined not inst failed ", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("5. user-defined, instanciated");

    sExp = "[object Object]";
    sAct = (new obFoo())+"";

    if (sAct != sExp)
        apLogFailInfo( "user-defined inst failed ", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("8. string");

    sExp = " ";
    sAct = " ";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--single space failed ", sExp, sAct, "");

    sExp = "                                                                   ";
    sAct = "                                                                   ";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--multiple spaces failed ", sExp, sAct, "");

    sExp = "false";
    sAct = "false";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as false failed ", sExp, sAct, "");

    sExp = "0";
    sAct = "0";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--ns 0 failed ", sExp, sAct, "");

    sExp = "1234567890";
    sAct = "1234567890";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--ns > 0 failed ", sExp, sAct, "");

    sExp = " 1234567890";
    sAct = " 1234567890";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--ns > 0, leading space failed ", sExp, sAct, "");

    sExp = "1234567890 ";
    sAct = "1234567890 ";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--ns > 0, trailing space failed ", sExp, sAct, "");

    sExp = "-1234567890";
    sAct = "-1234567890";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--ns < 0 failed ", sExp, sAct, "");

    sExp = "obFoo";
    sAct = "obFoo";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as single word failed ", sExp, sAct, "");

    sExp = " foo";
    sAct = " foo";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as single word, leading space failed ", sExp, sAct, "");

    sExp = "foo ";
    sAct = "foo ";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as single word, trailing space ", sExp, sAct, "");

    sExp = "foo bar";
    sAct = "foo bar";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as multiple word failed ", sExp, sAct, "");

    sExp = " foo bar";
    sAct = " foo bar";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as multiple word, leading space failed ", sExp, sAct, "");

    sExp = "foo bar ";
    sAct = "foo bar ";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as multiple word, trailing space failed ", sExp, sAct, "");

    sExp = "";
    sAct = "";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--zls failed ", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("9. constants");

    sExp = "true";
    sAct = true+"";

    if (sAct != sExp)
        apLogFailInfo( "constants--true failed ", sExp, sAct, "");

    sExp = "false";
    sAct = false+"";

    if (sAct != sExp)
        apLogFailInfo( "constants--false failed ", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("10. null");

    sExp = "null";
    sAct = null+"";

    if (sAct != sExp)
        apLogFailInfo( "null failed ", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("11. undefined");
    
    var obUndef;
    sExp = "undefined";
    sAct = obUndef+"";

    if (sAct != sExp)
        apLogFailInfo( "undefined failed ", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("12. built-in functions");

    sExp = VersionLF + "function escape() {\n    [native code]\n}" + VersionLF;
    sAct = escape+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--escape failed ", sExp, sAct, "");


    sExp = VersionLF + "function eval() {\n    [native code]\n}" + VersionLF;
    sAct = eval+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--eval failed ", sExp, sAct, "");


    sExp = VersionLF + "function isNaN() {\n    [native code]\n}" + VersionLF;
    sAct = isNaN+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--isNaN failed ", sExp, sAct, "");


    sExp = VersionLF + "function parseFloat() {\n    [native code]\n}" + VersionLF;
    sAct = parseFloat+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--parseFloat failed ", sExp, sAct, "");


    sExp = VersionLF + "function parseInt() {\n    [native code]\n}" + VersionLF;
    sAct = parseInt+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--parseInt failed ", sExp, sAct, "");


    sExp = VersionLF + "function unescape() {\n    [native code]\n}" + VersionLF;
    sAct = unescape+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--unescape failed ", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("13. Math methods");

    sExp = VersionLF + "function abs() {\n    [native code]\n}" + VersionLF;
    sAct = Math.abs+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--abs failed ", sExp, sAct, "");


    sExp = VersionLF + "function acos() {\n    [native code]\n}" + VersionLF;
    sAct = Math.acos+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--acos failed ", sExp, sAct, "");


    sExp = VersionLF + "function asin() {\n    [native code]\n}" + VersionLF;
    sAct = Math.asin+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--asin failed ", sExp, sAct, "");

    sExp = VersionLF + "function atan() {\n    [native code]\n}" + VersionLF;
    sAct = Math.atan+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--atan failed ", sExp, sAct, "");


    sExp = VersionLF + "function ceil() {\n    [native code]\n}" + VersionLF;
    sAct = Math.ceil+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--ceil failed ", sExp, sAct, "");


    sExp = VersionLF + "function cos() {\n    [native code]\n}" + VersionLF;
    sAct = Math.cos+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--cos failed ", sExp, sAct, "");


    sExp = VersionLF + "function exp() {\n    [native code]\n}" + VersionLF;
    sAct = Math.exp+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--exp failed ", sExp, sAct, "");


    sExp = VersionLF + "function floor() {\n    [native code]\n}" + VersionLF;
    sAct = Math.floor+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--floor failed ", sExp, sAct, "");


    sExp = VersionLF + "function log() {\n    [native code]\n}" + VersionLF;
    sAct = Math.log+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--log failed ", sExp, sAct, "");


    sExp = VersionLF + "function max() {\n    [native code]\n}" + VersionLF;
    sAct = Math.max+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--max failed ", sExp, sAct, "");


    sExp = VersionLF + "function min() {\n    [native code]\n}" + VersionLF;
    sAct = Math.min+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--min failed ", sExp, sAct, "");


    sExp = VersionLF + "function pow() {\n    [native code]\n}" + VersionLF;
    sAct = Math.pow+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--pow failed ", sExp, sAct, "");


    sExp = VersionLF + "function random() {\n    [native code]\n}" + VersionLF;
    sAct = Math.random+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--random failed ", sExp, sAct, "");


    sExp = VersionLF + "function round() {\n    [native code]\n}" + VersionLF;
    sAct = Math.round+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--round failed ", sExp, sAct, "");


    sExp = VersionLF + "function sin() {\n    [native code]\n}" + VersionLF;
    sAct = Math.sin+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--sin failed ", sExp, sAct, "");


    sExp = VersionLF + "function sqrt() {\n    [native code]\n}" + VersionLF;
    sAct = Math.sqrt+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--sqrt failed ", sExp, sAct, "");


    sExp = VersionLF + "function tan() {\n    [native code]\n}" + VersionLF;
    sAct = Math.tan+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--tan failed ", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("14. string methods");

    sExp = VersionLF + "function anchor() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".anchor+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--anchor failed ", sExp, sAct, "");


    sExp = VersionLF + "function big() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".big+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--big failed ", sExp, sAct, "");


    sExp = VersionLF + "function blink() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".blink+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--blink failed ", sExp, sAct, "");


    sExp = VersionLF + "function bold() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".bold+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--bold failed ", sExp, sAct, "");


    sExp = VersionLF + "function charAt() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".charAt+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--charAt failed ", sExp, sAct, "");


    sExp = VersionLF + "function fixed() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".fixed+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--fixed failed ", sExp, sAct, "");


    sExp = VersionLF + "function fontcolor() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".fontcolor+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--fontcolor failed ", sExp, sAct, "");


    sExp = VersionLF + "function fontsize() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".fontsize+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--fontsize failed ", sExp, sAct, "");


    sExp = VersionLF + "function indexOf() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".indexOf+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--indexOf failed ", sExp, sAct, "");


    sExp = VersionLF + "function italics() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".italics+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--italics failed ", sExp, sAct, "");


    sExp = VersionLF + "function lastIndexOf() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".lastIndexOf+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--lastIndexOf failed ", sExp, sAct, "");


    sExp = VersionLF + "function link() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".link+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--link failed ", sExp, sAct, "");


    sExp = VersionLF + "function small() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".small+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--small failed ", sExp, sAct, "");


    sExp = VersionLF + "function strike() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".strike+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--strike failed ", sExp, sAct, "");


    sExp = VersionLF + "function sub() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".sub+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--sub failed ", sExp, sAct, "");


    sExp = VersionLF + "function substring() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".substring+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--substring failed ", sExp, sAct, "");


    sExp = VersionLF + "function sup() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".sup+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--sup failed ", sExp, sAct, "");


    sExp = VersionLF + "function toLowerCase() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".toLowerCase+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--toLowerCase failed ", sExp, sAct, "");


    sExp = VersionLF + "function toUpperCase() {\n    [native code]\n}" + VersionLF;
    sAct = "somestring".toUpperCase+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--toUpperCase failed ", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("15. Built-in Date methods");

    sExp = VersionLF + "function parse() {\n    [native code]\n}" + VersionLF;
    sAct = Date.parse+"";

    if (sAct != sExp)
        apLogFailInfo( "Built-in Date methods--parse failed ", sExp, sAct, "");


    sExp = VersionLF + "function UTC() {\n    [native code]\n}" + VersionLF;
    sAct = Date.UTC+"";

    if (sAct != sExp)
        apLogFailInfo( "Built-in Date methods--UTC failed ", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("16. Instantiated Date methods");

    vDate = new Date(96,0,1);

    sExp = VersionLF + "function getDate() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.getDate+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getDate failed ", sExp, sAct, "");


    sExp = VersionLF + "function getDay() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.getDay+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getDay failed ", sExp, sAct, "");


    sExp = VersionLF + "function getHours() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.getHours+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getHours failed ", sExp, sAct, "");


    sExp = VersionLF + "function getMinutes() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.getMinutes+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getMinutes failed ", sExp, sAct, "");


    sExp = VersionLF + "function getMonth() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.getMonth+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getMonth failed ", sExp, sAct, "");


    sExp = VersionLF + "function getSeconds() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.getSeconds+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getSeconds failed ", sExp, sAct, "");


    sExp = VersionLF + "function getTime() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.getTime+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getTime failed ", sExp, sAct, "");


    sExp = VersionLF + "function getTimezoneOffset() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.getTimezoneOffset+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getTimezoneOffset failed ", sExp, sAct, "");


    sExp = VersionLF + "function getYear() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.getYear+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getYear failed ", sExp, sAct, "");


    sExp = VersionLF + "function setDate() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.setDate+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setDate failed ", sExp, sAct, "");


    sExp = VersionLF + "function setHours() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.setHours+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setHours failed ", sExp, sAct, "");


    sExp = VersionLF + "function setMinutes() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.setMinutes+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setMinutes failed ", sExp, sAct, "");


    sExp = VersionLF + "function setMonth() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.setMonth+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setMonth failed ", sExp, sAct, "");


    sExp = VersionLF + "function setSeconds() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.setSeconds+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setSeconds failed ", sExp, sAct, "");


    sExp = VersionLF + "function setTime() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.setTime+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setTime failed ", sExp, sAct, "");


    sExp = VersionLF + "function setYear() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.setYear+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setYear failed ", sExp, sAct, "");

    //if (parseFloat(@_jscript_version)>=7)
     // sExp = VersionLF + "function toUTCString() {\n    [native code]\n}" + VersionLF;
    //else
     // sExp = VersionLF + "function toGMTString() {\n    [native code]\n}" + VersionLF;
  if (parseFloat(@_jscript_version)<6)
     	sExp = VersionLF + "function toGMTString() {\n    [native code]\n}" + VersionLF;
    else
	//  sExp = VersionLF + "function toUTCString() {\n    [native code]\n}" + VersionLF;
	{
	@if(@_fast)
		sExp = VersionLF + "function toGMTString() {\n    [native code]\n}" + VersionLF;
	@else
		sExp = VersionLF + "function toUTCString() {\n    [native code]\n}" + VersionLF;
	@end
	}


    sAct = vDate.toGMTString+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--toGMTString failed ", sExp, sAct, "");


    sExp = VersionLF + "function toLocaleString() {\n    [native code]\n}" + VersionLF;
    sAct = vDate.toLocaleString+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--toLocaleString failed ", sExp, sAct, "");


    /***** Browser only method tests
    //----------------------------------------------------------------------------
//    apInitScenario("17. Browser only methods");

    sExp = VersionLF + "function alert() {\n    [native code]\n}" + VersionLF;
    sAct = window.alert+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.alert failed ", sExp, sAct, "");


    sExp = VersionLF + "function back() {\n    [native code]\n}" + VersionLF;
    sAct = history.back+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--history.back failed ", sExp, sAct, "");


    sExp = VersionLF + "function blur() {\n    [native code]\n}" + VersionLF;
    sAct = text.blur+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--text.blur failed ", sExp, sAct, "");


    sExp = VersionLF + "function clear() {\n    [native code]\n}" + VersionLF;
    sAct = document.clear+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--document.clear failed ", sExp, sAct, "");


    sExp = VersionLF + "function clearTimeout() {\n    [native code]\n}" + VersionLF;
    sAct = frame.clearTimeout+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--frame.clearTimeout failed ", sExp, sAct, "");


    sExp = VersionLF + "function click() {\n    [native code]\n}" + VersionLF;
    sAct = button.click+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--button.click failed ", sExp, sAct, "");


    sExp = VersionLF + "function close() {\n    [native code]\n}" + VersionLF;
    sAct = document.close+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--document.close failed ", sExp, sAct, "");


    sExp = VersionLF + "function close() {\n    [native code]\n}" + VersionLF;
    sAct = window.close+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.close failed ", sExp, sAct, "");


    sExp = VersionLF + "function confirm() {\n    [native code]\n}" + VersionLF;
    sAct = window.confirm+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.confirm failed ", sExp, sAct, "");


    sExp = VersionLF + "function focus() {\n    [native code]\n}" + VersionLF;
    sAct = text.focus+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--text.focus failed ", sExp, sAct, "");


    sExp = VersionLF + "function forward() {\n    [native code]\n}" + VersionLF;
    sAct = history.forward+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--history.forward failed ", sExp, sAct, "");


    sExp = VersionLF + "function go() {\n    [native code]\n}" + VersionLF;
    sAct = history.go+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--history.go failed ", sExp, sAct, "");


    sExp = VersionLF + "function open() {\n    [native code]\n}" + VersionLF;
    sAct = document.open+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--document.open failed ", sExp, sAct, "");


    sExp = VersionLF + "function open() {\n    [native code]\n}" + VersionLF;
    sAct = window.open+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.open failed ", sExp, sAct, "");


    sExp = VersionLF + "function prompt() {\n    [native code]\n}" + VersionLF;
    sAct = window.prompt+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.prompt failed ", sExp, sAct, "");


    sExp = VersionLF + "function select() {\n    [native code]\n}" + VersionLF;
    sAct = text.select+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--text.select failed ", sExp, sAct, "");


    sExp = VersionLF + "function setTimeout() {\n    [native code]\n}" + VersionLF;
    sAct = window.setTimeout+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.setTimeout failed ", sExp, sAct, "");


    sExp = VersionLF + "function submit() {\n    [native code]\n}" + VersionLF;
    sAct = form.submit+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--form.submit failed ", sExp, sAct, "");


    sExp = VersionLF + "function write() {\n    [native code]\n}" + VersionLF;
    sAct = document.write+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--document.write failed ", sExp, sAct, "");


    sExp = VersionLF + "function writeln() {\n    [native code]\n}" + VersionLF;
    sAct = document.writeln+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--document.writeln failed ", sExp, sAct, "");
    */

    apEndTest();

}


tostr007();


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
