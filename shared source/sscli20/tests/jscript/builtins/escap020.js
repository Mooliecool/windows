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


var iTestID = 52606;


// Decimal number to binary string conversion
function bin (num, fAll)
{
	num = parseInt(num);
	if (fAll==null) fAll = false;
    var sBin="";

    for (var i=0; (num>>>i || fAll) && i<32; i++)
        sBin = (num>>>i&1) + sBin;

    return sBin;
}


// Decimal number to hexidecimal string conversion
function hex (num)
{
    num = parseInt(num);
    
    var sHex = "0123456789ABCDEF".charAt(num&0x0f);

    if (num>>>4) sHex = hex(num>>>4) + sHex;
    
    return sHex;
}


// Decimal to octal conversion
function oct (num)
{
    if (num < 0)
        return "-"+oct(Math.abs(num));

    if (num < 8)
        return ""+num;
    else
        return oct(Math.floor(num/8)) + num%8+"";
}


// Hex or oct to decimal conversion
function dec (num)
{
    if (typeof num != "string") return num;
    if (num.charAt(0)!= "0") num = "0x"+num;
    return eval(num);
}


// bin string to dec number conversion
function decBin (sNum)
{
    var val = 0, cb = sNum.length;
    for (var i=0; i<cb; i++)
        if (sNum.charAt(i)=="1") val |= 1<<(cb-1-i);
    return val;
}

// hex string to dec number conversion
// Handles signed int min (0x80000000) to signed int max (0x7fffffff), for
// everything else the behaviour is undefined.  The prefixes "0x"|"0X" are optional.
function decHex (sNum)
{
    var val = 0, cb = sNum.length;
    sNum = sNum.toLowerCase();
    for (var i=(sNum.substring(0,2)=="0x" ? 2 : 0); i<cb; i++)
        val |= "0123456789abcdef".indexOf(sNum.charAt(i))<<(4*(cb-1-i));
    return val;
}

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
function bldMask(start, end) {
    if (bldMask.length<2) {
        end = start;
        start = 0;
    }
    if (end>0xff) return "";
    var ret="";
    for (var i=start; i<=end; i++)
        ret += padStr(hex(i),2,"0");
    return ret;
}
function runningonFE() {
    var ret = 0;

    if (apGlobalObj.apGetLangExt(apGlobalObj.LangHost()) == "JP")
       ret = 1;
    if (apGlobalObj.apGetLangExt(apGlobalObj.LangHost()) == "KO")
       ret = 1;
    if (apGlobalObj.apGetLangExt(apGlobalObj.LangHost()) == "CHS")
       ret = 1;
    if (apGlobalObj.apGetLangExt(apGlobalObj.LangHost()) == "CHT")
       ret = 1;

    return ret;
}

function escap020() {

    apInitTest( "escap020 ");

    var stkOb = new Object();
    stkOb[12] = null;

    var sExp, sAct;
    var sMask, sEscRnd, afEscRnd, iPos;

    var sEsc = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";  // chars 0x20-0x7f
    var afEsc = "111111111100100000000000001111110000000000000000000000000001111010000000000000000000000000011111";

    //--------------------------------------------------------------------
    apInitScenario( "1. zls, null mask");

    sExp = "";
    stkOb[12] = sExp;
    sAct = escape( stkOb[12] );
    if (sAct != sExp)
        apLogFailInfo( "zls, null mask failed", sExp, sAct, "");

    
    //--------------------------------------------------------------------
    apInitScenario( "2. char 0x00 (unprintable), null mask");
    
    stkOb[12] = unescape("%00");
    if ( "%00" != escape(stkOb[12]) )
        apLogFailInfo( "char 0x00 failed, null mask", "", "", "");


    //--------------------------------------------------------------------
    apInitScenario( "3. single char 0x01-0x1f (unprintable), null mask");

    // ten random chars
    for (var i=0; i<10; i++) {
        iPos = Math.floor(Math.random()*((0x1f-0x01)+1))
        sExp = "%"+padStr(hex(iPos+0x01),2,"0");

        stkOb[12] = unescape( sExp );
        sAct = escape( stkOb[12] );

        if (sAct != sExp)
            apLogFailInfo( "single char 0x01-0x1f, null mask failed", sExp, sAct, "");
    }


    //--------------------------------------------------------------------
    apInitScenario( "4. repeated single char 0x01-0x1f (unprintable), null mask");

    // five random repeated char strings
    var s = "";
    for (var i=0; i<5; i++) {
        sExp = "";
        iPos = Math.floor(Math.random()*((0x1f-0x01)+1))
        s = "%"+padStr(hex(iPos+0x01),2,"0");

        for (var j=0; j<10; j++) sExp += s;

        stkOb[12] = unescape( sExp );
        sAct = escape( stkOb[12] );

        if (sAct != sExp)
            apLogFailInfo( "repeated single char 0x01-0x1f, null mask failed", sExp, sAct, "");
    }


    //--------------------------------------------------------------------
    apInitScenario( "5. chars 0x01-0x1f (unprintable), ordered, null mask");

    sExp=sAct="";
    for (i=0x01; i<=0x1f; i++)
        sExp += "%"+padStr(hex(i),2,"0");

    stkOb[12] = unescape( sExp );
    sAct = escape( stkOb[12] );

    if (sAct != sExp)
        apLogFailInfo( "chars 0x01-0x1f, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "6. chars 0x01-0x1f (unprintable), unordered, null mask");

    sMask = unsort( bldMask(0x01,0x1f), 2), sExp="", sAct="";
    for (var c=0; c<sMask.length; c+=2) {
        sExp += "%"+sMask.substring(c, c+2);
    }

    stkOb[12] = unescape( sExp );
    sAct = escape( stkOb[12] );

    if (sAct != sExp)
        apLogFailInfo( "chars 0x01-0x1f (unprintable), unordered, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "7. single char 0x20-0x7f, null mask");

    // ten random chars
    for (var i=0; i<10; i++) {
        iPos = Math.floor(Math.random()*((0x7f-0x20)+1))
        if (afEsc.charAt(iPos)=="1")
            sExp = "%"+padStr(hex(iPos+0x20),2,"0");
            else sExp = sEsc.charAt(iPos);

        stkOb[12] = sEsc.charAt(iPos);
        sAct = escape( stkOb[12] );

        if (sAct != sExp)
            apLogFailInfo( "single char 0x20-0x7f, null mask failed", sExp, sAct, "");
    }


    //--------------------------------------------------------------------
    apInitScenario( "8. repeated single char 0x20-0x7f, null mask");

    // five random repeated char strings
    var s="", sActTmp="";
    for (var i=0; i<5; i++) {
        sExp=sActTmp="";
        iPos = Math.floor(Math.random()*((0x7f-0x20)+1))
        if (afEsc.charAt(iPos)=="1")
            s = "%"+padStr(hex(iPos+0x20),2,"0");
            else s = sEsc.charAt(iPos);

        for (var j=0; j<10; j++) { sExp+=s; sActTmp+=sEsc.charAt(iPos); };

        stkOb[12] = sActTmp;
        sAct = escape( stkOb[12] );

        if (sAct != sExp)
            apLogFailInfo( "repeated single char 0x20-0x7f, null mask failed", sExp, sAct, "");
    }


    //--------------------------------------------------------------------
    apInitScenario( "9. chars 0x20-0x7f, ordered, null mask");

    sExp=sAct="";
    for (i=0x20 ; i<=0x7f; i++) {
        iPos = i-0x20;
        if (afEsc.charAt(iPos)=="1")
            sExp += "%"+padStr(hex(i),2,"0");
        else sExp += sEsc.charAt(iPos);
    }

    stkOb[12] = sEsc;
    sAct = escape( stkOb[12] );

    if (sAct != sExp)
        apLogFailInfo( "chars 0x20-0x7f, ordered, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "10. chars 0x20-0x7f, unordered, null mask");

    sMask = unsort( bldMask(0x20, 0x7f), 2 ), sEscRnd="", afEscRnd="", iPos=0;
    for (var c=0; c<sMask.length; c+=2) {
        iPos = eval("0x"+sMask.substring(c, c+2))-0x20;
        sEscRnd += sEsc.charAt(iPos);
        afEscRnd += afEsc.charAt(iPos);
    }

    sExp=sAct="";
    for (i=0x20 ; i<=0x7f; i++) {
        iPos = i-0x20;
        if (afEscRnd.charAt(iPos)=="1")
            sExp += "%"+sMask.substring(iPos*2,iPos*2+2);
        else sExp += sEscRnd.charAt(iPos);
    }

    stkOb[12] = sEscRnd;
    sAct = escape( stkOb[12] );

    if (sAct != sExp)
        apLogFailInfo( "chars 0x20-0x7f, unordered, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    if (runningonFE() == 0) {
    apInitScenario( "11. single char 0x80-0xff (extended), null mask");

    // ten random chars
    for (var i=0; i<10; i++) {
        iPos = Math.floor(Math.random()*((0xff-0x80)+1))
        sExp = "%"+padStr(hex(iPos+0x80),2,"0");

        stkOb[12] = unescape( sExp );
        sAct = escape( stkOb[12] );

        if (sAct != sExp)
            apLogFailInfo( "single char 0x80-0xff (extended), null mask failed", sExp, sAct, "");
    }


    //--------------------------------------------------------------------
    apInitScenario( "12. repeated single char 0x80-0xff (extended), null mask");

    // five random repeated char strings
    s = "";
    for (var i=0; i<5; i++) {
        sExp = "";
        iPos = Math.floor(Math.random()*((0xff-0x80)+1))
        s = "%"+padStr(hex(iPos+0x80),2,"0");

        for (var j=0; j<10; j++) sExp += s;

        stkOb[12] = unescape( sExp );
        sAct = escape( stkOb[12] );

        if (sAct != sExp)
            apLogFailInfo( "single char 0x80-0xff (extended), null mask failed", sExp, sAct, "");
    }


    //--------------------------------------------------------------------
    apInitScenario( "13. chars 0x80-0xff (extended), ordered, null mask");

    sExp=sAct="";
    for (i=0x80; i<=0xff; i++)
        sExp += "%"+padStr(hex(i),2,"0");

    stkOb[12] = unescape( sExp );
    sAct = escape( stkOb[12] );

    if (sAct != sExp)
        apLogFailInfo( "chars 0x80-0xff (extended), null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "14. chars 0x80-0xff (extended), unordered, null mask");

    sMask = unsort( bldMask(0x80,0xff), 2), sExp="", sAct="";
    for (var c=0; c<sMask.length; c+=2) {
        sExp += "%"+sMask.substring(c, c+2);
    }

    stkOb[12] = unescape( sExp );
    sAct = escape( stkOb[12] );

    if (sAct != sExp)
        apLogFailInfo( "chars 0x80-0xff (extended), unordered, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "15. chars 0x01-0xff, ordered, null mask");

    var sRnd="", sCur="";
    sExp=sAct="";
    for (var i=0x01; i<=0xff; i++) {
        if (i>=0x20 && i<=0x7f && afEsc.charAt(i-0x20)=="0") {
            sCur = sEsc.charAt(i-0x20);
            sRnd += sCur;
        }
        else {
            sCur = "%"+padStr(hex(i),2,"0");
            sRnd += unescape( sCur );
        }
        sExp += sCur;
    }

    stkOb[12] = sRnd;
    sAct = escape( stkOb[12] );

    if (sAct != sExp)
        apLogFailInfo( "chars 0x00-0xff, ordered, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "16. chars 0x01-0xff, unordered, null mask");

    var sRnd="";
    sMask = unsort( bldMask(0x01,0xff), 2), iPos=0, sExp=sAct="";
    for (var c=0; c<sMask.length; c+=2) {
        iPos = eval("0x"+sMask.substring(c, c+2));
        if (iPos>=0x20 && iPos<=0x7f && afEsc.charAt(iPos-0x20)=="0") {
            sCur = sEsc.charAt(iPos-0x20);
            sRnd += sCur;
        }
        else {
            sCur = "%"+sMask.substring(c, c+2);
            sRnd += unescape( sCur );
        }
        sExp += sCur;
    }

    stkOb[12] = sRnd;
    sAct = escape( stkOb[12] );

    if (sAct != sExp)
        apLogFailInfo( "chars 0x01-0xff, unordered, null mask failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "17. huge string, chars 0x01-0xff, unordered, null mask");

    // use random strings created from last test, joined together to make huge string
    for (var i=0; i<12; i++) {
        sExp += sExp;
        sRnd += sRnd;
    }

    stkOb[12] = sRnd;
    sAct = escape( stkOb[12] );

    if (sAct != sExp)
        apLogFailInfo( "huge string, chars 0x01-0xff, unordered, null mask failed", "","", "");

   } // end if not on FE platform
   else{
    /**/apInitScenario("skipped scenario")
    /**/apInitScenario("skipped scenario")
    /**/apInitScenario("skipped scenario")
    /**/apInitScenario("skipped scenario")
    /**/apInitScenario("skipped scenario")
    /**/apInitScenario("skipped scenario")
    /**/apInitScenario("skipped scenario")
    }

    apEndTest();

}


escap020();


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
