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


var iTestID = 53145;



@if(!@aspx)
	function obFoo() {};
@else
	expando function obFoo() {};
@end

var m_scen = "";

function psflt01a() {
@if(@_fast)
    var sExp,sAct,sCat;
@end
    apInitTest("psFlt01a ");

    //----------------------------------------------------------------------------
    apInitScenario("9. Alpha string");

    m_scen = "alpha string";

    sCat = "single space";
    sAct = parseFloat(" ");

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" -- "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "multiple spaces";
    sAct = parseFloat("                                                                   ");

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" -- "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "as false";
    sAct = parseFloat("false");

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" -- "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "as single word";
    sAct = parseFloat("foo");

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" -- "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "as single word, leading space";
    sAct = parseFloat(" foo");

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" -- "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "as single word, trailing space";
    sAct = parseFloat("foo ");

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" -- "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "as multiple word";
    sAct = parseFloat("foo bar");

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" -- "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "as multiple word, leading space";
    sAct = parseFloat(" foo bar");

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" -- "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "as multiple word, trailing space";
    sAct = parseFloat("foo bar ");

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" -- "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "zls";
    sAct = parseFloat("");

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" -- "+sCat+" failed", Number.NaN, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("10. random float number+string (printable chars only, no numbers)");

    m_scen = "random number+string, printable chars only";
    
    sCat = "100139007.976685xe|^pf%pyCU)C\nr~R\nk\"}~m`je_+Gkulnug\n-f\"hPn''\"oq#M:|a\"q<*RdT}O{XRqkt]b$VP\OGsBlQq?SW.,zYxVBj\"gqC-&NTQ";
    sExp = 100139007.976685;
    sAct = parseFloat("100139007.976685xe|^pf%pyCU)C\nr~R\nk\"}~m`je_+Gkulnug\n-f\"hPn''\"oq#M:|a\"q<*RdT}O{XRqkt]b$VP\OGsBlQq?SW.,zYxVBj\"gqC-&NTQ");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" "+sCat+" failed", sExp, sAct, "");

    sCat = "1412956159.67102\n!_\f$.qd`td\tCN=uE[CZQIJ#Zpv$-&xO+r\t=\CC}DYf#^\zu(aRg<INtpEu% I wOZ_npp%gP>['H?goVZ *:ta.WN|\te^~_Y.[";
    sExp = 1412956159.67102;
    sAct = parseFloat("1412956159.67102\n!_\f$.qd`td\tCN=uE[CZQIJ#Zpv$-&xO+r\t=\CC}DYf#^\zu(aRg<INtpEu% I wOZ_npp%gP>['H?goVZ *:ta.WN|\te^~_Y.[");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" "+sCat+" failed", sExp, sAct, "");

    sCat = "1632894975.61981*nDd#+rizNq<:b-E\"\'\\>sDUIUyQ*	>\"T`}A-MEkEWACm,P}|f\DR.KPk:kyt.A$I:mXt'*$}Itv >bxfgl>qw}q(-:;Qr_tig?;";
    sExp = 1632894975.61981;
    sAct = parseFloat("1632894975.61981*nDd#+rizNq<:b-E\"\'\\>sDUIUyQ*	>\"T`}A-MEkEWACm,P}|f\DR.KPk:kyt.A$I:mXt'*$}Itv >bxfgl>qw}q(-:;Qr_tig?;");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" "+sCat+" failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("11. random float number+string (non-printable chars)");

    m_scen = "random number+string, non-printable chars";

    sCat = "1076232191.74942ÍÚ?´Ãö?¸Á\
ôö?â„¡Í.??.èÎÉ¦«Â.Êº¾•–ˆ³éŒ‚.»¦..Ò—.¬…½ªŽÎÊ.í¼Õëš“¼„.÷ø.?Ú™.?›ü–…À».œ‡¬ðÁªÃêÌÓƒ§";
    sExp = 1076232191.74942;
    sAct = parseFloat("1076232191.74942ÍÚ?´Ãö?¸Á\
ôö?â„¡Í.??.èÎÉ¦«Â.Êº¾•–ˆ³éŒ‚.»¦..Ò—.¬…½ªŽÎÊ.í¼Õëš“¼„.÷ø.?Ú™.?›ü–…À».œ‡¬ðÁªÃêÌÓƒ§");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" "+sCat+" failed", sExp, sAct, "");

    sCat = "2760376319.3573û£—òÙö.?Þ?Ž¼Ñì.?ïÊ÷¹?ºéÊ“ƒÆÃ“’Ý¶¦Äò?.?š÷‚óýÎ¥å.¯Ý.¹ëÛ¯¶‹ßÁÁÙ.ùµ?éÈ€?.òÅ.ÇéÉ.Îüû£.àíàÝƒÕÅ?";
    sExp = 2760376319.3573;
    sAct = parseFloat("2760376319.3573û£—òÙö.?Þ?Ž¼Ñì.?ïÊ÷¹?ºéÊ“ƒÆÃ“’Ý¶¦Äò?.?š÷‚óýÎ¥å.¯Ý.¹ëÛ¯¶‹ßÁÁÙ.ùµ?éÈ€?.òÅ.ÇéÉ.Îüû£.àíàÝƒÕÅ?");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" "+sCat+" failed", sExp, sAct, "");

    sCat = "3596484607.16263..”.šÃÔà¥Ÿ½ÔÀë?‹?Ã‹ »»Ì¬áèƒ¤Èî. §–öµö„ÌÈâïÞ´.í®Ï•¬ëàóˆ¾¥À ¹±¿.¬‘Ó\
Ö¶’õ»¥óý.€š‚Ž×.§•”¦.­˜?ø×¬Ê";
    sExp = 3596484607.16263;
    sAct = parseFloat("3596484607.16263..”.šÃÔà¥Ÿ½ÔÀë?‹?Ã‹ »»Ì¬áèƒ¤Èî. §–öµö„ÌÈâïÞ´.í®Ï•¬ëàóˆ¾¥À ¹±¿.¬‘Ó\
Ö¶’õ»¥óý.€š‚Ž×.§•”¦.­˜?ø×¬Ê");

    if (sAct != sExp)
        apLogFailInfo( m_scen+" "+sCat+" failed", sExp, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("12. random string(printable chars only)+number");

    m_scen = "random string(printable chars only)+number";

    sCat = "xe|^pf%pyCU)C\nr~R\nk\"}~m`je_+Gkulnug\n-f\"hPn''\"oq#M:|a\"q<*RdT}O{XRqkt]b$VP\OGsBlQq?SW.,zYxVBj\"gqC-&NTQ100139007.976685";
    sAct = parseFloat("xe|^pf%pyCU)C\nr~R\nk\"}~m`je_+Gkulnug\n-f\"hPn''\"oq#M:|a\"q<*RdT}O{XRqkt]b$VP\OGsBlQq?SW.,zYxVBj\"gqC-&NTQ100139007.976685");

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "\n!_\f$.qd`td\tCN=uE[CZQIJ#Zpv$-&xO+r\t=\CC}DYf#^\zu(aRg<INtpEu% I wOZ_npp%gP>['H?goVZ *:ta.WN|\te^~_Y.[1412956159.67102";
    sAct = parseFloat("\n!_\f$.qd`td\tCN=uE[CZQIJ#Zpv$-&xO+r\t=\CC}DYf#^\zu(aRg<INtpEu% I wOZ_npp%gP>['H?goVZ *:ta.WN|\te^~_Y.[1412956159.67102");

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "*nDd#+rizNq<:b-E\"\'\\>sDUIUyQ*	>\"T`}A-MEkEWACm,P}|f\DR.KPk:kyt.A$I:mXt'*$}Itv >bxfgl>qw}q(-:;Qr_tig?;1632894975.61981";
    sAct = parseFloat("*nDd#+rizNq<:b-E\"\'\\>sDUIUyQ*	>\"T`}A-MEkEWACm,P}|f\DR.KPk:kyt.A$I:mXt'*$}Itv >bxfgl>qw}q(-:;Qr_tig?;1632894975.61981");

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("13. random string(non-printable chars)+number");

    m_scen = "random string(non-printable chars)+number";

    sCat = "ÍÚ?´Ãö?¸Á\
ôö?â„¡Í.??.èÎÉ¦«Â.Êº¾•–ˆ³éŒ‚.»¦..Ò—.¬…½ªŽÎÊ.í¼Õëš“¼„.÷ø.?Ú™.?›ü–…À».œ‡¬ðÁªÃêÌÓƒ§1076232191.74942";
    sAct = parseFloat("ÍÚ?´Ãö?¸Á\
ôö?â„¡Í.??.èÎÉ¦«Â.Êº¾•–ˆ³éŒ‚.»¦..Ò—.¬…½ªŽÎÊ.í¼Õëš“¼„.÷ø.?Ú™.?›ü–…À».œ‡¬ðÁªÃêÌÓƒ§1076232191.74942");

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "û£—òÙö.?Þ?Ž¼Ñì.?ïÊ÷¹?ºéÊ“ƒÆÃ“’Ý¶¦Äò?.?š÷‚óýÎ¥å.¯Ý.¹ëÛ¯¶‹ßÁÁÙ.ùµ?éÈ€?.òÅ.ÇéÉ.Îüû£.àíàÝƒÕÅ?760376319.3573";
    sAct = parseFloat("û£—òÙö.?Þ?Ž¼Ñì.?ïÊ÷¹?ºéÊ“ƒÆÃ“’Ý¶¦Äò?.?š÷‚óýÎ¥å.¯Ý.¹ëÛ¯¶‹ßÁÁÙ.ùµ?éÈ€?.òÅ.ÇéÉ.Îüû£.àíàÝƒÕÅ?760376319.3573");

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "..”.šÃÔà¥Ÿ½ÔÀë?‹?Ã‹ »»Ì¬áèƒ¤Èî. §–öµö„ÌÈâïÞ´.í®Ï•¬ëàóˆ¾¥À ¹±¿.¬‘Ó\
Ö¶’õ»¥óý.€š‚Ž×.§•”¦.­˜?ø×¬Ê3596484607.16263";
    sAct = parseFloat("..”.šÃÔà¥Ÿ½ÔÀë?‹?Ã‹ »»Ì¬áèƒ¤Èî. §–öµö„ÌÈâïÞ´.í®Ï•¬ëàóˆ¾¥À ¹±¿.¬‘Ó\
Ö¶’õ»¥óý.€š‚Ž×.§•”¦.­˜?ø×¬Ê3596484607.16263");

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("14. objects, built-in, non-exec");

    m_scen = "objects, built-in, non-exec";

    sCat = "Math";
    sAct = parseFloat(Math);

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("15. objects, built-in, exec, not inst");

    m_scen = "objects, built-in, exec, not inst";

    sCat = "Array";
    sAct = parseFloat(Array);

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "Boolean";
    sAct = parseFloat(Boolean);

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "Date";
    sAct = parseFloat(Date);

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "Number";
    sAct = parseFloat(Number);

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "Object";
    sAct = parseFloat(Object);

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("16. objects, built-in, exec, inst");

    m_scen = "objects, built-in, exec, inst";

    sCat = "new Array()";
    sAct = parseFloat(new Array());

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "new Boolean()";
    sAct = parseFloat(new Boolean());

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "new Date()";
    sAct = parseFloat(new Date());

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "new Number()";
    sExp = 0;
    sAct = parseFloat(new Number());

    if (sAct != sExp)
        apLogFailInfo( m_scen+" "+sCat+" failed", sExp, sAct, "");

    sCat = "new Object()";
    sAct = parseFloat(new Object());

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("17. objects, user-def, not inst");

    m_scen = "objects, user-def, not inst";

    sCat = "obFoo";
    sAct = parseFloat(obFoo);

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("18. objects, user-def, inst");

    m_scen = "objects, user-def, inst";

    sCat = "new obFoo()";
    sAct = parseFloat(new obFoo());

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("19. constants");

    m_scen = "constants";

    sCat = "true";
    sAct = parseFloat(true);

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");

    sCat = "false";
    sAct = parseFloat(false);

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("20. null");

    m_scen = "null";

    sCat = "";
    sAct = parseFloat(null);

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");


    //----------------------------------------------------------------------------
    apInitScenario("21. undefined");

    m_scen = "undefined";

    var obUndef;
    sCat = "";
    sAct = parseFloat(obUndef);

    if (!isNaN(sAct))
        apLogFailInfo( m_scen+" "+sCat+" failed", Number.NaN, sAct, "");

    apEndTest();
}


psflt01a();


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
