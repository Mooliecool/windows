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


var iTestID = 53883;


// universal trim function
// defaults to trimming tabs, newlines, and spaces.
function trim (sIn, sTChars)
{
    if (!sIn.length) return sIn;
    if (sTChars == undefined) sTChars = "\t\n ";

    var iSt=-1, iEnd=sIn.length;

    while ( ++iSt<iEnd && sTChars.indexOf(sIn.charAt(iSt)) != -1 ) {}
    while ( --iEnd>iSt && sTChars.indexOf(sIn.charAt(iEnd)) != -1 ) {}

    return sIn.substring(iSt,iEnd+1);
}


// Equvilant to VBA's Chr function
function chr (num)
{
    if (num > 255 || num < 0) return null;

    var sHex = "0123456789abcdef";
    return unescape( "%"+sHex.charAt(num>>4&0x0f)+sHex.charAt(num&0x0f) );
}


// Equivilant to VBA's Asc function
function asc (str)
{
//    return parseInt( escape(str.charAt(0), 0).substring(1,3), 16 );
    return str.charCodeAt(0);
}


// Returns a "proper form" string from the input string sIn.
// The standard token delimiters are tab, newline, space, period, hypen,
// comma, & double/single quotes.  These can be overridden with the optional
// opDlm parameter and added to if the opDlm param contains a '+' char
// as the first char.
function proper (sIn, opDlm)
{
    var i;
    var std = "\t\n .-,\"\'";
    if (opDlm == undefined) opDlm = std;
    var sDlm = opDlm.charAt(0)=='+' ? std+opDlm.substring(1,opDlm.length) : opDlm;
    var ch, ich=0, cb=sIn.length, s=sIn.toLowerCase();

    for (i=0; i<cb; i++)
    {
        ch = s.charAt(i);
        if (sDlm.indexOf(ch)>=0)
            ich = i+1;
        else
            if (ich==i)
                s = s.substring(0,i)+ch.toUpperCase()+s.substring(i+1,cb);
    }
    
    return s;
}


// Equiv to VBA's String function
function bldStr (cb, str)
{
    if ( cb==undefined || 0 == cb || 0 == (""+str).length ) return "";
	if ( str==undefined ) str = " ";

    var sBld = (str + "");

    while (sBld.length * 2 < cb) sBld += sBld;

    if (sBld.length < cb)
        sBld += bldStr(cb - sBld.length, str);
    
    return sBld;
}


// Pads the base arg with as many pad arg's to make a string who's length is equal to n.
// sPad is optional, if not present, a space is used by default.
// Requires:  bldStr
function padStr (base, n, pad)
{
	var cb = n - ("" + base).length;

	if (pad == undefined) pad = " ";

	if (cb > 0 )
	    return bldStr(cb, pad) + base;

    return ""+base;
}


// Formats a string into cbBlk sized sections, delimited by sPad, from the right
// if fRight flag is set, from the left if not.  All args are optional except sSrc.
function fmtSBlock (sSrc, cbBlk, sPad, fRight)
{
    var i;
    // Default parameters
    if (fRight == undefined) fRight = false;
    if (sPad == undefined) sPad = " ";
    if (cbBlk == undefined || 0==cbBlk) cbBlk = sSrc.length;

    var cbMod = fRight ? sSrc.length%cbBlk : 0;

    var sRet = sSrc.substring(0,cbMod);
    
    for (i=0; i<sSrc.length-cbMod; i+=cbBlk)
        sRet += (sPad + sSrc.substring(i+cbMod,i+cbMod+cbBlk));

    return sRet.substring(!cbMod*sPad.length,sRet.length);
}


// Returns the string arg's chars randomly aranged in cbChunk sized chuncks
// cbChunk is optional, defaults to 1
function unsort (sIn, cbChunk)
{
    if (cbChunk == undefined) cbChunk = 1;
	var sOut="", iStrt=0, iEnd=0;
    while (sIn.length)
    {
        iStrt = Math.floor(Math.random()*Math.ceil(sIn.length/cbChunk))*cbChunk;
        iEnd = iStrt+cbChunk;
        sOut += sIn.substring(iStrt,iEnd);
        sIn = sIn.substring(0,iStrt) + (iEnd<sIn.length ? sIn.substring(iEnd, sIn.length) : "");
    }
    return sOut;
}


// Parameterized random string generator
// cb is the number of characters you want the string to return [optional, defaults to 100].
// bfType is a bit mask specifying the characters you want the string to contain [optional, 
// defaults to alphanumeric].  sDomain [optional] is a user supplied source string.
// Requires: chr
function genRndStr (cb, bfType, sDomain)
{
    var i;
    var sSource = "";
    if ( sDomain != undefined && typeof sDomain == "string")
        sSource = sDomain;

    if (bfType == undefined || bfType > 0)
    {
        if (cb == undefined) cb = 100;
        if (bfType == undefined) bfType = 0x01|0x02|0x04;

        var rgfWhichChars = new Array(256);
        for (i=1; i<rgfWhichChars.length; i++)
            rgfWhichChars[i] = 0;

        if (bfType & 0x01) // ucase alpha
            for (i=0x41; i<=0x5A; i++) rgfWhichChars[i]=1;

        if (bfType & 0x02) // lcase alpha
            for (i=0x61; i<=0x7A; i++) rgfWhichChars[i]=1;

        if (bfType & 0x04) // numbers
            for (i=0x30; i<=0x39; i++) rgfWhichChars[i]=1;

        if (bfType & 0x08) // all other printable chars (non-extended)
        {
            for (i=0x21; i<=0x2E; i++) rgfWhichChars[i]=1;
            for (i=0x3A; i<=0x40; i++) rgfWhichChars[i]=1;
            for (i=0x5B; i<=0x60; i++) rgfWhichChars[i]=1;
            for (i=0x7B; i<=0x7E; i++) rgfWhichChars[i]=1;
        }
        if (bfType & 0x10) // space
            rgfWhichChars[0x20]=1;

        if (bfType & 0x20) // tab
            rgfWhichChars[0x09]=1;

        if (bfType & 0x40) // newline
            rgfWhichChars[0x0A]=1;

        if (bfType & 0x80) // all other special chars
        {
            rgfWhichChars[0x08]=1; // backspace
            rgfWhichChars[0x0C]=1; // formfeed
            rgfWhichChars[0x0D]=1; // carriage return
        }
        if (bfType & 0x100) // non-printable non-extended chars
        {
            for (i=0x01; i<=0x07; i++) rgfWhichChars[i]=1;
                                           rgfWhichChars[0x0B]=1;
            for (i=0x0E; i<=0x1F; i++) rgfWhichChars[i]=1;
                                           rgfWhichChars[0x7F]=1;
        }
        if (bfType & 0x200) // extended chars (0x7f-0xff)
            for (i=0x80; i<=0xff; i++) rgfWhichChars[i]=1;

        // Create restricted char string
        for (i=0x00; i<0xff; i++)
            if (rgfWhichChars[i]) sSource += chr(i);
    }

    // Create random result string from restricted char string
    var sRet = "";
    for (i=0; i<cb; i++)
        sRet += sSource.charAt( Math.floor(Math.random()*sSource.length) );

    return sRet;
}


function GetLang()
{
   var lang
   lang = apGlobalObj.apGetLangExt(apGlobalObj.LangHost());
   return lang;
}

var g_OnDBCS = false;
var g_lang;


function OnDbcs()
{
	if(g_lang == "JP" || g_lang == "KO" || g_lang == "CHS" || g_lang == "CHT")
	{
		g_OnDBCS = true;
		return true;
	}
	return false;
}


function unesc003() {
	

    apInitTest( "unesc003 ");

    g_lang = GetLang();
    OnDbcs();

    var sExp, sAct;
    var sClean="";

    //--------------------------------------------------------------------
    apInitScenario( "1. repeated mixed escape char 0x01-0x1f (unprintable)");

    sExp = '\b\b\b\b\b\b\b\b';
    sAct = unescape( "%08\b\b%08%08\b%08\b" );

    if (sAct != sExp)
        apLogFailInfo( "repeated mixed escape char 0x01-0x1f--'\\b' failed", sExp, sAct, "");

    sExp = '\t\t\t\t';
    sAct = unescape( "\t%09%09\t" );

    if (sAct != sExp)
        apLogFailInfo( "repeated mixed escape char 0x01-0x1f--'\\t' failed", sExp, sAct, "");

    sExp = '\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n';
    sAct = unescape( "%0A%0A\n%0A%0A%0A%0A\n\n\n%0A%0A\n%0A\n\n\n%0A%0A\n\n\n" );

    if (sAct != sExp)
        apLogFailInfo( "repeated mixed escape char 0x01-0x1f--'\\n' failed", sExp, sAct, "");

    sExp = '\f\n';
    sAct = unescape( "%0C\n" );

    if (sAct != sExp)
        apLogFailInfo( "repeated mixed escape char 0x01-0x1f--'\\f' failed", sExp, sAct, "");

    sExp = '\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r';
    sAct = unescape( "\r\r%0D\r\r%0D%0D\r%0D%0D%0D%0D%0D\r%0D" );

    if (sAct != sExp)
        apLogFailInfo( "repeated mixed escape char 0x01-0x1f--'\\r' failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "2. mixed escape chars 0x01-0x1f (unprintable), ordered");

    sExp = '\b\t\n\f\r';
    sAct = unescape( "%08\t%0A\f\r" );

    if (sAct != sExp)
        apLogFailInfo( "mixed escape chars 0x01-0x1f (unprintable), ordered failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "3. mixed escape chars 0x01-0x1f (unprintable), unordered");
 
    sExp = '\t\n\r\b\f';
    sAct = unescape( "\t\n%0D%08%0C" );

    if (sAct != sExp)
        apLogFailInfo( "mixed escape chars 0x01-0x1f (unprintable), unordered failed", sExp, sAct, "");


    //---------------Chars 0x20-0x7f------------------------------------------

        //--------------------------------------------------------------------
        apInitScenario( "4. multiple mixed escape spaces");
    
        sExp = "                  ";
        sAct = unescape( " %20 %20%20%20%20%20  %20%20%20    %20" );

        if (sAct != sExp)
            apLogFailInfo( "multiple mixed escape spaces failed", sExp, sAct, "");


        //--------------------------------------------------------------------
        apInitScenario( "5. repeated mixed escape char uc alpha");

        sExp = 'QQQQQQQQQQQQQQQQQQQQQQQQ';
        sAct = unescape( 'QQ%51%51%51QQQQ%51%51%51QQQQQQQQQ%51%51%51' );

        if (sAct != sExp)
            apLogFailInfo( "repeated mixed escape char uc alpha failed", sExp, sAct, "");

        sExp = 'TTTTTTTTTTTTT';
        sAct = unescape( 'TT%54TTT%54%54TTTTT' );

        if (sAct != sExp)
            apLogFailInfo( "repeated escaped char uc alpha failed", sExp, sAct, "");


        //--------------------------------------------------------------------
        apInitScenario( "6. repeated mixed escape char lc alpha");

        sExp = 'pppppppppppppppppppppppppppppppp';
        sAct = unescape( '%70%70pppppp%70pppppppppp%70%70%70%70%70%70%70%70%70p%70%70%70' );

        if (sAct != sExp)
            apLogFailInfo( "repeated mixed escape char lc alpha failed", sExp, sAct, "");

        sExp = 'zzzzzzzzzzzzzzzzzzzzzzzz';
        sAct = unescape( 'zzz%7A%7Az%7A%7A%7A%7A%7A%7A%7Az%7A%7A%7A%7A%7A%7A%7A%7A%7Az' );

        if (sAct != sExp)
            apLogFailInfo( "repeated mixed escape char lc alpha failed", sExp, sAct, "");


        //--------------------------------------------------------------------
        apInitScenario( "7. repeated mixed escape char misc");

        sExp = '##########';
        sAct = unescape( '%23#%23%23%23%23%23%23%23%23' );

        if (sAct != sExp)
            apLogFailInfo( "repeated mixed escape char misc failed", sExp, sAct, "");

        sExp = '!!!!!!!!!!!!!!';
        sAct = unescape( '%21!!!!%21%21!!!%21!%21%21' );

        if (sAct != sExp)
            apLogFailInfo( "repeated mixed escape char misc failed", sExp, sAct, "");

        sExp = '[[[[';
        sAct = unescape( '[[[%5B' );

        if (sAct != sExp)
            apLogFailInfo( "repeated mixed escape char misc failed", sExp, sAct, "");

        sExp = '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@';
        sAct = unescape( '@@@@@@@%40%40@@@@@@@%40@@@@@@@@@@@@@@@@@%40%40@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@' );

        if (sAct != sExp)
            apLogFailInfo( "repeated mixed escape char misc failed", sExp, sAct, "");


        //--------------------------------------------------------------------
        apInitScenario( "8. repeated mixed escape char num");

        sExp = "11111111111111111111111111111111111";
        sAct = unescape( "11111111%31%31%31%31%31%31%311111111111%31%3111111%31%31%31" );

        if (sAct != sExp)
            apLogFailInfo( "repeated mixed escape char num failed", sExp, sAct, "");

        sExp = "0000000000000000000000000000";
        sAct = unescape( "%30%30%30%30000%30%30%30%30%30%30%30%30%30%30%300%3000000%3000" );

        if (sAct != sExp)
            apLogFailInfo( "repeated mixed escape char num failed", sExp, sAct, "");


    //-------------------------------------------------------------------------


    //---------------Chars 0x80-0xff-(Extended)-------------------------------

        //--------------------------------------------------------------------

        apInitScenario( "9. repeated mixed escape char misc");


	if(!g_OnDBCS)
	{
        	sExp = 'çççççççççççççççççç';
	        sAct = unescape( 'ç%E7ç%E7%E7%E7%E7çççç%E7%E7çç%E7%E7%E7' );

	        if (sAct != sExp)
        	    apLogFailInfo( "repeated mixed escape char extended failed", sExp, sAct, "");

	        sExp = 'ýýýýýý';
        	sAct = unescape( 'ýýýý%FD%FD' );

	        if (sAct != sExp)
        	    apLogFailInfo( "repeated mixed escape char extended failed", sExp, sAct, "");

	        sExp = '¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬';
        	sAct = unescape( '%AC%AC%AC%AC%AC%AC%AC%AC%AC¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬' );

	        if (sAct != sExp)
        	    apLogFailInfo( "repeated mixed escape char extended failed", sExp, sAct, "");

	        sExp = '¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿';
        	sAct = unescape( '%BF%BF%BF%BF¿¿¿¿¿%BF%BF¿¿¿¿¿%BF%BF¿¿¿¿%BF%BF%BF¿' );

	        if (sAct != sExp)
        	    apLogFailInfo( "repeated mixed escape char extended failed", sExp, sAct, "");

	        sExp = '«««';
        	sAct = unescape( '««%AB' );

	        if (sAct != sExp)
        	    apLogFailInfo( "repeated mixed escape char extended failed", sExp, sAct, "");

	}
	
	//DBCS Safe tests 
	else
	{
        	sExp = 'çççççççççççççççççç';
	        sAct = unescape( 'çç\u9257\u9257çç\u9257\u9257\u9257\u9257\u9257' );

	        if (sAct != sExp)
        	    apLogFailInfo( "repeated mixed escape char extended failed", sExp, sAct, "");

	        sExp = 'ýýýýýý';
        	sAct = unescape( 'ýýýý\uF8F1\uF8F1' );

	        if (sAct != sExp)
        	    apLogFailInfo( "repeated mixed escape char extended failed", sExp, sAct, "");

	        sExp = '¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬';
        	sAct = unescape( '\uFF6C\uFF6C\uFF6C\uFF6C\uFF6C\uFF6C\uFF6C\uFF6C\uFF6C¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬' );

	        if (sAct != sExp)
        	    apLogFailInfo( "repeated mixed escape char extended failed", sExp, sAct, "");

	        sExp = '¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿';
        	sAct = unescape( '\uFF7F\uFF7F\uFF7F\uFF7F¿¿¿¿¿\uFF7F\uFF7F¿¿¿¿¿\uFF7F\uFF7F¿¿¿¿\uFF7F\uFF7F\uFF7F¿' );

	        if (sAct != sExp)
        	    apLogFailInfo( "repeated mixed escape char extended failed", sExp, sAct, "");

	        sExp = '«««';
        	sAct = unescape( '««\uFF6B' );

	        if (sAct != sExp)
        	    apLogFailInfo( "repeated mixed escape char extended failed", sExp, sAct, "");
	}
	
    //-------------------------------------------------------------------------


    apEndTest();

}



unesc003();


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
