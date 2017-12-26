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


var iTestID = 53709;




@if(!@aspx)
	function obFoo() {};
	function fnRetStr() { return "function return string"; }
@else
	expando function obFoo() {};
	expando function fnRetStr() { return "function return string"; }
@end


function tostr019() {
 @if(@_fast) 
    var conExp, sExp, sAct, vDate;
 @end

    apInitTest( "toStr019 " );

    conExp = "function return string";

    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec");
  @if(! @_fast)
    Math.toString = fnRetStr;
    sExp = conExp
    sAct = Math+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, non-exec--Math failed ", sExp, sAct,"");
@end

    //----------------------------------------------------------------------------
    apInitScenario("2. built-in, exec, not instanciated");
  @if(! @_fast)
    Array.toString = fnRetStr;
    sExp = conExp;
    sAct = Array+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst--Array failed ", sExp, sAct,"");

    Boolean.toString = fnRetStr;
    sExp = conExp;
    sAct = Boolean+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst, exec--Boolean failed ", sExp, sAct, "");

    Date.toString = fnRetStr;
    sExp = conExp;
    sAct = Date+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst--Date failed ", sExp, sAct, "");

    Number.toString = fnRetStr;
    sExp = conExp;
    sAct = Number+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst--Number failed ", sExp, sAct, "");

    Object.toString = fnRetStr;
    sExp = conExp;
    sAct = Object+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst--Object failed ", sExp, sAct, "");

    String.toString = fnRetStr;
    sExp = conExp;
    sAct = String+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst--String failed ", sExp, sAct, "");

@end
    //----------------------------------------------------------------------------
     apInitScenario("3. built-in, exec, instanciated");
    // no way of adding and subsequently accessing a redefined method for a global
    // object constructor without the use of a prototype.


    //----------------------------------------------------------------------------
    apInitScenario("4. user-defined, not instanciated");
  @if(! @_fast)
    obFoo.toString = fnRetStr;
    sExp = conExp;
    sAct = obFoo+"";

    if (sAct != sExp)
        apLogFailInfo( "user-defined not inst failed ", sExp, sAct, "");

@end
    //----------------------------------------------------------------------------
    // apInitScenario("5. user-defined, instanciated");
    // no way of adding and subsequently accessing a redefined method for a global
    // object constructor without the use of a prototype.


    //----------------------------------------------------------------------------
    apInitScenario("6. string");
  @if(! @_fast)
    " ".toString = fnRetStr;
    sExp = " ";
    sAct = " "+"";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--single space failed ", sExp, sAct);

    "                                                                   ".toString = fnRetStr;
    sExp = "                                                                   ";
    sAct = "                                                                   "+"";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--multiple spaces failed ", sExp, sAct);

    "false".toString = fnRetStr;
    sExp = "false";
    sAct = "false"+"";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as false failed ", sExp, sAct);

    "0".toString = fnRetStr;
    sExp = "0";
    sAct = "0"+"";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--ns 0 failed ", sExp, sAct);

    "1234567890".toString = fnRetStr;
    sExp = "1234567890";
    sAct = "1234567890"+"";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--ns > 0 failed ", sExp, sAct);

    " 1234567890".toString = fnRetStr;
    sExp = " 1234567890";
    sAct = " 1234567890"+"";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--ns > 0, leading space failed ", sExp, sAct);

    "1234567890 ".toString = fnRetStr;
    sExp = "1234567890 ";
    sAct = "1234567890 "+"";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--ns > 0, trailing space failed ", sExp, sAct);

    "-1234567890".toString = fnRetStr;
    sExp = "-1234567890";
    sAct = "-1234567890"+"";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--ns < 0 failed ", sExp, sAct);

    "obFoo".toString = fnRetStr;
    sExp = "obFoo";
    sAct = "obFoo"+"";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as single word failed ", sExp, sAct);

    " foo".toString = fnRetStr;
    sExp = " foo";
    sAct = " foo"+"";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as single word, leading space failed ", sExp, sAct);

    "foo ".toString = fnRetStr;
    sExp = "foo ";
    sAct = "foo "+"";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as single word, trailing space ", sExp, sAct);

    "foo bar".toString = fnRetStr;
    sExp = "foo bar";
    sAct = "foo bar"+"";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as multiple word failed ", sExp, sAct);

    " foo bar".toString = fnRetStr;
    sExp = " foo bar";
    sAct = " foo bar"+"";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as multiple word, leading space failed ", sExp, sAct);

    "foo bar ".toString = fnRetStr;
    sExp = "foo bar ";
    sAct = "foo bar "+"";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as multiple word, trailing space failed ", sExp, sAct);

    "".toString = fnRetStr;
    sExp = "";
    sAct = ""+"";

    if (sAct != sExp)
        apLogFailInfo( "literal, string--zls failed ", sExp, sAct);

@end
    //----------------------------------------------------------------------------
    apInitScenario("7. constants");
  @if(! @_fast)
    true.toString = fnRetStr;
    sExp = "true";
    sAct = true+"";

    if (sAct != sExp)
        apLogFailInfo( "constants--true failed ", sExp, sAct, 15139);

    false.toString = fnRetStr;
    sExp = "false";
    sAct = false+"";

    if (sAct != sExp)
        apLogFailInfo( "constants--false failed ", sExp, sAct, 15139);

@end
    //----------------------------------------------------------------------------
    apInitScenario("8. built-in functions");
  @if(! @_fast)
    escape.toString = fnRetStr;
    sExp = conExp;
    sAct = escape+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--escape failed ", sExp, sAct, "");

    eval.toString = fnRetStr;
    sExp = conExp;
    sAct = eval+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--eval failed ", sExp, sAct, "");

    isNaN.toString = fnRetStr;
    sExp = conExp;
    sAct = isNaN+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--isNaN failed ", sExp, sAct, "");

    parseFloat.toString = fnRetStr;
    sExp = conExp;
    sAct = parseFloat+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--parseFloat failed ", sExp, sAct, "");

    parseInt.toString = fnRetStr;
    sExp = conExp;
    sAct = parseInt+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--parseInt failed ", sExp, sAct, "");

    unescape.toString = fnRetStr;
    sExp = conExp;
    sAct = unescape+"";

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--unescape failed ", sExp, sAct, "");
@end

    //----------------------------------------------------------------------------
    apInitScenario("9. Math methods");
  @if(! @_fast)
    Math.abs.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.abs+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--abs failed ", sExp, sAct, "");

    Math.acos.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.acos+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--acos failed ", sExp, sAct, "");

    Math.asin.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.asin+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--asin failed ", sExp, sAct, "");

    Math.atan.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.atan+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--atan failed ", sExp, sAct, "");

    Math.ceil.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.ceil+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--ceil failed ", sExp, sAct, "");

    Math.cos.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.cos+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--cos failed ", sExp, sAct, "");

    Math.exp.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.exp+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--exp failed ", sExp, sAct, "");

    Math.floor.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.floor+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--floor failed ", sExp, sAct, "");

    Math.log.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.log+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--log failed ", sExp, sAct, "");

    Math.max.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.max+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--max failed ", sExp, sAct, "");

    Math.min.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.min+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--min failed ", sExp, sAct, "");

    Math.pow.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.pow+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--pow failed ", sExp, sAct, "");

    Math.random.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.random+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--random failed ", sExp, sAct, "");

    Math.round.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.round+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--round failed ", sExp, sAct, "");

    Math.sin.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.sin+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--sin failed ", sExp, sAct, "");

    Math.sqrt.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.sqrt+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--sqrt failed ", sExp, sAct, "");

    Math.tan.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.tan+"";

    if (sAct != sExp)
        apLogFailInfo( "Math methods--tan failed ", sExp, sAct, "");

@end
    //----------------------------------------------------------------------------
    apInitScenario("10. string methods");
  @if(! @_fast)
    "somestring".anchor.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".anchor+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--anchor failed ", sExp, sAct, "");

    "somestring".big.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".big+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--big failed ", sExp, sAct, "");

    "somestring".blink.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".blink+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--blink failed ", sExp, sAct, "");

    "somestring".bold.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".bold+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--bold failed ", sExp, sAct, "");

    "somestring".charAt.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".charAt+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--charAt failed ", sExp, sAct, "");

    "somestring".fixed.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".fixed+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--fixed failed ", sExp, sAct, "");

    "somestring".fontcolor.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".fontcolor+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--fontcolor failed ", sExp, sAct, "");

    "somestring".fontsize.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".fontsize+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--fontsize failed ", sExp, sAct, "");

    "somestring".indexOf.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".indexOf+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--indexOf failed ", sExp, sAct, "");

    "somestring".italics.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".italics+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--italics failed ", sExp, sAct, "");

    "somestring".lastIndexOf.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".lastIndexOf+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--lastIndexOf failed ", sExp, sAct, "");

    "somestring".link.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".link+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--link failed ", sExp, sAct, "");

    "somestring".small.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".small+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--small failed ", sExp, sAct, "");

    "somestring".strike.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".strike+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--strike failed ", sExp, sAct, "");

    "somestring".sub.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".sub+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--sub failed ", sExp, sAct, "");

    "somestring".substring.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".substring+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--substring failed ", sExp, sAct, "");

    "somestring".sup.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".sup+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--sup failed ", sExp, sAct, "");

    "somestring".toLowerCase.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".toLowerCase+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--toLowerCase failed ", sExp, sAct, "");

    "somestring".toUpperCase.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".toUpperCase+"";

    if (sAct != sExp)
        apLogFailInfo( "string methods--toUpperCase failed ", sExp, sAct, "");
@end

    //----------------------------------------------------------------------------
    apInitScenario("11. Built-in Date methods");
  @if(! @_fast)
    Date.parse.toString = fnRetStr;
    sExp = conExp;
    sAct = Date.parse+"";

    if (sAct != sExp)
        apLogFailInfo( "Built-in Date methods--parse failed ", sExp, sAct, "");

    Date.UTC.toString = fnRetStr;
    sExp = conExp;
    sAct = Date.UTC+"";

    if (sAct != sExp)
        apLogFailInfo( "Built-in Date methods--UTC failed ", sExp, sAct, "");
@end

    //----------------------------------------------------------------------------
    apInitScenario("12. Instantiated Date methods");
  @if(! @_fast)
    vDate = new Date(96,0,1);

    vDate.getDate.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getDate+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getDate failed ", sExp, sAct, "");

    vDate.getDay.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getDay+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getDay failed ", sExp, sAct, "");

    vDate.getHours.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getHours+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getHours failed ", sExp, sAct, "");

    vDate.getMinutes.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getMinutes+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getMinutes failed ", sExp, sAct, "");

    vDate.getMonth.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getMonth+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getMonth failed ", sExp, sAct, "");

    vDate.getSeconds.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getSeconds+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getSeconds failed ", sExp, sAct, "");

    vDate.getTime.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getTime+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getTime failed ", sExp, sAct, "");

    vDate.getTimezoneOffset.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getTimezoneOffset+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getTimezoneOffset failed ", sExp, sAct, "");

    vDate.getYear.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getYear+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getYear failed ", sExp, sAct, "");

    vDate.setDate.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.setDate+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setDate failed ", sExp, sAct, "");

    vDate.setHours.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.setHours+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setHours failed ", sExp, sAct, "");

    vDate.setMinutes.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.setMinutes+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setMinutes failed ", sExp, sAct, "");

    vDate.setMonth.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.setMonth+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setMonth failed ", sExp, sAct, "");

    vDate.setSeconds.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.setSeconds+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setSeconds failed ", sExp, sAct, "");

    vDate.setTime.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.setTime+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setTime failed ", sExp, sAct, "");

    vDate.setYear.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.setYear+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setYear failed ", sExp, sAct, "");

    vDate.toGMTString.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.toGMTString+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--toGMTString failed ", sExp, sAct, "");

    vDate.toLocaleString.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.toLocaleString+"";

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--toLocaleString failed ", sExp, sAct, "");
  @end

    /***** Browser only method tests
    //----------------------------------------------------------------------------
//    apInitScenario("13. Browser only methods");
  
    window.alert.toString = fnRetStr;
    sExp = conExp;
    sAct = window.alert+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.alert failed ", sExp, sAct, "");

    history.back.toString = fnRetStr;
    sExp = conExp;
    sAct = history.back+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--history.back failed ", sExp, sAct, "");

    text.blur.toString = fnRetStr;
    sExp = conExp;
    sAct = text.blur+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--text.blur failed ", sExp, sAct, "");

    document.clear.toString = fnRetStr;
    sExp = conExp;
    sAct = document.clear+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--document.clear failed ", sExp, sAct, "");

    frame.clearTimeout.toString = fnRetStr;
    sExp = conExp;
    sAct = frame.clearTimeout+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--frame.clearTimeout failed ", sExp, sAct, "");

    button.click.toString = fnRetStr;
    sExp = conExp;
    sAct = button.click+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--button.click failed ", sExp, sAct, "");

    document.close.toString = fnRetStr;
    sExp = conExp;
    sAct = document.close+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--document.close failed ", sExp, sAct, "");

    window.close.toString = fnRetStr;
    sExp = conExp;
    sAct = window.close+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.close failed ", sExp, sAct, "");

    window.confirm.toString = fnRetStr;
    sExp = conExp;
    sAct = window.confirm+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.confirm failed ", sExp, sAct, "");

    text.focus.toString = fnRetStr;
    sExp = conExp;
    sAct = text.focus+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--text.focus failed ", sExp, sAct, "");

    history.forward.toString = fnRetStr;
    sExp = conExp;
    sAct = history.forward+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--history.forward failed ", sExp, sAct, "");

    history.go.toString = fnRetStr;
    sExp = conExp;
    sAct = history.go+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--history.go failed ", sExp, sAct, "");

    document.open.toString = fnRetStr;
    sExp = conExp;
    sAct = document.open+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--document.open failed ", sExp, sAct, "");

    window.open.toString = fnRetStr;
    sExp = conExp;
    sAct = window.open+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.open failed ", sExp, sAct, "");

    window.prompt.toString = fnRetStr;
    sExp = conExp;
    sAct = window.prompt+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.prompt failed ", sExp, sAct, "");

    text.select.toString = fnRetStr;
    sExp = conExp;
    sAct = text.select+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--text.select failed ", sExp, sAct, "");

    window.setTimeout.toString = fnRetStr;
    sExp = conExp;
    sAct = window.setTimeout+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.setTimeout failed ", sExp, sAct, "");

    form.submit.toString = fnRetStr;
    sExp = conExp;
    sAct = form.submit+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--form.submit failed ", sExp, sAct, "");

    document.write.toString = fnRetStr;
    sExp = conExp;
    sAct = document.write+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--document.write failed ", sExp, sAct, "");

    document.writeln.toString = fnRetStr;
    sExp = conExp;
    sAct = document.writeln+"";

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--document.writeln failed ", sExp, sAct, "");
*/

    apEndTest();

}


tostr019();


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
