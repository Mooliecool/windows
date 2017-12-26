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


var iTestID = 53703;


function fnRetStr() { return "function return string"; }


@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

function tostr013() {
 @if(@_fast)
    var conExp, sExp, sAct, vDate;
 @end

    apInitTest( "toStr013 " );

    conExp = "function return string";

    //----------------------------------------------------------------------------
    apInitScenario("1. built-in, non-exec");
@if(!@_fast)
    Math.toString = fnRetStr;
    sExp = conExp
    sAct = Math.toString();

    if (sAct != sExp)
        apLogFailInfo( "built-in, non-exec--Math failed ", sExp, sAct,"");
@end

    //----------------------------------------------------------------------------
    apInitScenario("2. built-in, exec, not instanciated");
@if(!@_fast)
    Array.toString = fnRetStr;
    sExp = conExp;
    sAct = Array.toString();

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst--Array failed ", sExp, sAct,"");

    Boolean.toString = fnRetStr;
    sExp = conExp;
    sAct = Boolean.toString();

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst, exec--Boolean failed ", sExp, sAct, "");

    Date.toString = fnRetStr;
    sExp = conExp;
    sAct = Date.toString();

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst--Date failed ", sExp, sAct, "");

    Number.toString = fnRetStr;
    sExp = conExp;
    sAct = Number.toString();

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst--Number failed ", sExp, sAct, "");

    Object.toString = fnRetStr;
    sExp = conExp;
    sAct = Object.toString();

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst--Object failed ", sExp, sAct, "");

    String.toString = fnRetStr;
    sExp = conExp;
    sAct = String.toString();

    if (sAct != sExp)
        apLogFailInfo( "built-in, exec not inst--String failed ", sExp, sAct, "");

@end
    //----------------------------------------------------------------------------
    apInitScenario("3. built-in, exec, instanciated");
    // no way of adding and subsequently accessing a redefined method for a global
    // object constructor without the use of a prototype.


    //----------------------------------------------------------------------------
    apInitScenario("4. user-defined, not instanciated");
@if(!@_fast)
    obFoo.toString = fnRetStr;
    sExp = conExp;
    sAct = obFoo.toString();

    if (sAct != sExp)
        apLogFailInfo( "user-defined not inst failed ", sExp, sAct, "");
@end

    //----------------------------------------------------------------------------
    apInitScenario("5. user-defined, instanciated");
    // no way of adding and subsequently accessing a redefined method for a global
    // object constructor without the use of a prototype.


    //----------------------------------------------------------------------------
    apInitScenario("8. string");
@if(!@_fast)
    " ".toString = fnRetStr;
    sExp = " ";
    sAct = " ".toString();

    if (sAct != sExp)
        apLogFailInfo( "literal, string--single space failed ", sExp, sAct, 16045);

    "                                                                   ".toString = fnRetStr;
    sExp = "                                                                   ";
    sAct = "                                                                   ".toString();

    if (sAct != sExp)
        apLogFailInfo( "literal, string--multiple spaces failed ", sExp, sAct, 16045);

    "false".toString = fnRetStr;
    sExp = "false";
    sAct = "false".toString();

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as false failed ", sExp, sAct, 16045);

    "0".toString = fnRetStr;
    sExp = "0";
    sAct = "0".toString();

    if (sAct != sExp)
        apLogFailInfo( "literal, string--ns 0 failed ", sExp, sAct, 16045);

    "1234567890".toString = fnRetStr;
    sExp = "1234567890";
    sAct = "1234567890".toString();

    if (sAct != sExp)
        apLogFailInfo( "literal, string--ns > 0 failed ", sExp, sAct, 16045);

    " 1234567890".toString = fnRetStr;
    sExp = " 1234567890";
    sAct = " 1234567890".toString();

    if (sAct != sExp)
        apLogFailInfo( "literal, string--ns > 0, leading space failed ", sExp, sAct, 16045);

    "1234567890 ".toString = fnRetStr;
    sExp = "1234567890 ";
    sAct = "1234567890 ".toString();

    if (sAct != sExp)
        apLogFailInfo( "literal, string--ns > 0, trailing space failed ", sExp, sAct, 16045);

    "-1234567890".toString = fnRetStr;
    sExp = "-1234567890";
    sAct = "-1234567890".toString();

    if (sAct != sExp)
        apLogFailInfo( "literal, string--ns < 0 failed ", sExp, sAct, 16045);

    "obFoo".toString = fnRetStr;
    sExp = "obFoo";
    sAct = "obFoo".toString();

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as single word failed ", sExp, sAct, 16045);

    " foo".toString = fnRetStr;
    sExp = " foo";
    sAct = " foo".toString();

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as single word, leading space failed ", sExp, sAct, 16045);

    "foo ".toString = fnRetStr;
    sExp = "foo ";
    sAct = "foo ".toString();

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as single word, trailing space ", sExp, sAct, 16045);

    "foo bar".toString = fnRetStr;
    sExp = "foo bar";
    sAct = "foo bar".toString();

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as multiple word failed ", sExp, sAct, 16045);

    " foo bar".toString = fnRetStr;
    sExp = " foo bar";
    sAct = " foo bar".toString();

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as multiple word, leading space failed ", sExp, sAct, 16045);

    "foo bar ".toString = fnRetStr;
    sExp = "foo bar ";
    sAct = "foo bar ".toString();

    if (sAct != sExp)
        apLogFailInfo( "literal, string--as multiple word, trailing space failed ", sExp, sAct, 16045);

    "".toString = fnRetStr;
    sExp = "";
    sAct = "".toString();

    if (sAct != sExp)
        apLogFailInfo( "literal, string--zls failed ", sExp, sAct, 16045);
@end

    //----------------------------------------------------------------------------
    apInitScenario("9. constants");
@if(!@_fast)
    true.toString = fnRetStr;
    sExp = "true";
    sAct = true.toString();

    if (sAct != sExp)
        apLogFailInfo( "constants--true failed ", sExp, sAct, 15139);

    false.toString = fnRetStr;
    sExp = "false";
    sAct = false.toString();

    if (sAct != sExp)
        apLogFailInfo( "constants--false failed ", sExp, sAct, 15139);
@end
    //----------------------------------------------------------------------------
    apInitScenario("12. built-in functions");
@if(!@_fast)
    escape.toString = fnRetStr;
    sExp = conExp;
    sAct = escape.toString();

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--escape failed ", sExp, sAct, "");

    eval.toString = fnRetStr;
    sExp = conExp;
    sAct = eval.toString();

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--eval failed ", sExp, sAct, "");

    isNaN.toString = fnRetStr;
    sExp = conExp;
    sAct = isNaN.toString();

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--isNaN failed ", sExp, sAct, "");

    parseFloat.toString = fnRetStr;
    sExp = conExp;
    sAct = parseFloat.toString();

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--parseFloat failed ", sExp, sAct, "");

    parseInt.toString = fnRetStr;
    sExp = conExp;
    sAct = parseInt.toString();

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--parseInt failed ", sExp, sAct, "");

    unescape.toString = fnRetStr;
    sExp = conExp;
    sAct = unescape.toString();

    if (sAct != sExp)
        apLogFailInfo( "built-in functions--unescape failed ", sExp, sAct, "");

@end
    //----------------------------------------------------------------------------
    apInitScenario("13. Math methods");
@if(!@_fast)
    Math.abs.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.abs.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--abs failed ", sExp, sAct, "");

    Math.acos.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.acos.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--acos failed ", sExp, sAct, "");

    Math.asin.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.asin.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--asin failed ", sExp, sAct, "");

    Math.atan.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.atan.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--atan failed ", sExp, sAct, "");

    Math.ceil.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.ceil.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--ceil failed ", sExp, sAct, "");

    Math.cos.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.cos.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--cos failed ", sExp, sAct, "");

    Math.exp.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.exp.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--exp failed ", sExp, sAct, "");

    Math.floor.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.floor.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--floor failed ", sExp, sAct, "");

    Math.log.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.log.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--log failed ", sExp, sAct, "");

    Math.max.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.max.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--max failed ", sExp, sAct, "");

    Math.min.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.min.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--min failed ", sExp, sAct, "");

    Math.pow.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.pow.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--pow failed ", sExp, sAct, "");

    Math.random.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.random.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--random failed ", sExp, sAct, "");

    Math.round.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.round.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--round failed ", sExp, sAct, "");

    Math.sin.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.sin.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--sin failed ", sExp, sAct, "");

    Math.sqrt.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.sqrt.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--sqrt failed ", sExp, sAct, "");

    Math.tan.toString = fnRetStr;
    sExp = conExp;
    sAct = Math.tan.toString();

    if (sAct != sExp)
        apLogFailInfo( "Math methods--tan failed ", sExp, sAct, "");

@end
    //----------------------------------------------------------------------------
    apInitScenario("14. string methods");
@if(!@_fast)
    "somestring".anchor.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".anchor.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--anchor failed ", sExp, sAct, "");

    "somestring".big.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".big.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--big failed ", sExp, sAct, "");

    "somestring".blink.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".blink.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--blink failed ", sExp, sAct, "");

    "somestring".bold.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".bold.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--bold failed ", sExp, sAct, "");

    "somestring".charAt.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".charAt.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--charAt failed ", sExp, sAct, "");

    "somestring".fixed.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".fixed.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--fixed failed ", sExp, sAct, "");

    "somestring".fontcolor.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".fontcolor.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--fontcolor failed ", sExp, sAct, "");

    "somestring".fontsize.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".fontsize.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--fontsize failed ", sExp, sAct, "");

    "somestring".indexOf.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".indexOf.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--indexOf failed ", sExp, sAct, "");

    "somestring".italics.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".italics.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--italics failed ", sExp, sAct, "");

    "somestring".lastIndexOf.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".lastIndexOf.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--lastIndexOf failed ", sExp, sAct, "");

    "somestring".link.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".link.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--link failed ", sExp, sAct, "");

    "somestring".small.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".small.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--small failed ", sExp, sAct, "");

    "somestring".strike.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".strike.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--strike failed ", sExp, sAct, "");

    "somestring".sub.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".sub.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--sub failed ", sExp, sAct, "");

    "somestring".substring.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".substring.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--substring failed ", sExp, sAct, "");

    "somestring".sup.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".sup.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--sup failed ", sExp, sAct, "");

    "somestring".toLowerCase.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".toLowerCase.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--toLowerCase failed ", sExp, sAct, "");

    "somestring".toUpperCase.toString = fnRetStr;
    sExp = conExp;
    sAct = "somestring".toUpperCase.toString();

    if (sAct != sExp)
        apLogFailInfo( "string methods--toUpperCase failed ", sExp, sAct, "");

@end
    //----------------------------------------------------------------------------
    apInitScenario("15. Built-in Date methods");
@if(!@_fast)
    Date.parse.toString = fnRetStr;
    sExp = conExp;
    sAct = Date.parse.toString();

    if (sAct != sExp)
        apLogFailInfo( "Built-in Date methods--parse failed ", sExp, sAct, "");

    Date.UTC.toString = fnRetStr;
    sExp = conExp;
    sAct = Date.UTC.toString();

    if (sAct != sExp)
        apLogFailInfo( "Built-in Date methods--UTC failed ", sExp, sAct, "");

@end
    //----------------------------------------------------------------------------
    apInitScenario("16. Instantiated Date methods");
@if(!@_fast)
    vDate = new Date(96,0,1);

    vDate.getDate.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getDate.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getDate failed ", sExp, sAct, "");

    vDate.getDay.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getDay.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getDay failed ", sExp, sAct, "");

    vDate.getHours.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getHours.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getHours failed ", sExp, sAct, "");

    vDate.getMinutes.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getMinutes.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getMinutes failed ", sExp, sAct, "");

    vDate.getMonth.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getMonth.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getMonth failed ", sExp, sAct, "");

    vDate.getSeconds.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getSeconds.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getSeconds failed ", sExp, sAct, "");

    vDate.getTime.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getTime.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getTime failed ", sExp, sAct, "");

    vDate.getTimezoneOffset.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getTimezoneOffset.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getTimezoneOffset failed ", sExp, sAct, "");

    vDate.getYear.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.getYear.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--getYear failed ", sExp, sAct, "");

    vDate.setDate.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.setDate.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setDate failed ", sExp, sAct, "");

    vDate.setHours.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.setHours.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setHours failed ", sExp, sAct, "");

    vDate.setMinutes.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.setMinutes.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setMinutes failed ", sExp, sAct, "");

    vDate.setMonth.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.setMonth.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setMonth failed ", sExp, sAct, "");

    vDate.setSeconds.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.setSeconds.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setSeconds failed ", sExp, sAct, "");

    vDate.setTime.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.setTime.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setTime failed ", sExp, sAct, "");

    vDate.setYear.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.setYear.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--setYear failed ", sExp, sAct, "");

    vDate.toGMTString.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.toGMTString.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--toGMTString failed ", sExp, sAct, "");

    vDate.toLocaleString.toString = fnRetStr;
    sExp = conExp;
    sAct = vDate.toLocaleString.toString();

    if (sAct != sExp)
        apLogFailInfo( "Instantiated Date methods--toLocaleString failed ", sExp, sAct, "");
@end

    /***** Browser only method tests
    //----------------------------------------------------------------------------
//    apInitScenario("17. Browser only methods");

    window.alert.toString = fnRetStr;
    sExp = conExp;
    sAct = window.alert.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.alert failed ", sExp, sAct, "");

    history.back.toString = fnRetStr;
    sExp = conExp;
    sAct = history.back.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--history.back failed ", sExp, sAct, "");

    text.blur.toString = fnRetStr;
    sExp = conExp;
    sAct = text.blur.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--text.blur failed ", sExp, sAct, "");

    document.clear.toString = fnRetStr;
    sExp = conExp;
    sAct = document.clear.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--document.clear failed ", sExp, sAct, "");

    frame.clearTimeout.toString = fnRetStr;
    sExp = conExp;
    sAct = frame.clearTimeout.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--frame.clearTimeout failed ", sExp, sAct, "");

    button.click.toString = fnRetStr;
    sExp = conExp;
    sAct = button.click.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--button.click failed ", sExp, sAct, "");

    document.close.toString = fnRetStr;
    sExp = conExp;
    sAct = document.close.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--document.close failed ", sExp, sAct, "");

    window.close.toString = fnRetStr;
    sExp = conExp;
    sAct = window.close.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.close failed ", sExp, sAct, "");

    window.confirm.toString = fnRetStr;
    sExp = conExp;
    sAct = window.confirm.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.confirm failed ", sExp, sAct, "");

    text.focus.toString = fnRetStr;
    sExp = conExp;
    sAct = text.focus.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--text.focus failed ", sExp, sAct, "");

    history.forward.toString = fnRetStr;
    sExp = conExp;
    sAct = history.forward.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--history.forward failed ", sExp, sAct, "");

    history.go.toString = fnRetStr;
    sExp = conExp;
    sAct = history.go.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--history.go failed ", sExp, sAct, "");

    document.open.toString = fnRetStr;
    sExp = conExp;
    sAct = document.open.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--document.open failed ", sExp, sAct, "");

    window.open.toString = fnRetStr;
    sExp = conExp;
    sAct = window.open.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.open failed ", sExp, sAct, "");

    window.prompt.toString = fnRetStr;
    sExp = conExp;
    sAct = window.prompt.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.prompt failed ", sExp, sAct, "");

    text.select.toString = fnRetStr;
    sExp = conExp;
    sAct = text.select.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--text.select failed ", sExp, sAct, "");

    window.setTimeout.toString = fnRetStr;
    sExp = conExp;
    sAct = window.setTimeout.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--window.setTimeout failed ", sExp, sAct, "");

    form.submit.toString = fnRetStr;
    sExp = conExp;
    sAct = form.submit.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--form.submit failed ", sExp, sAct, "");

    document.write.toString = fnRetStr;
    sExp = conExp;
    sAct = document.write.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--document.write failed ", sExp, sAct, "");

    document.writeln.toString = fnRetStr;
    sExp = conExp;
    sAct = document.writeln.toString();

    if (sAct != sExp)
        apLogFailInfo( "Browser only methods--document.writeln failed ", sExp, sAct, "");
    */

    apEndTest();

}


tostr013();


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
