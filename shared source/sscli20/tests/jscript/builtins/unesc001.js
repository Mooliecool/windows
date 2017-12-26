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


var iTestID = 53881;


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
var g_OnDBCS = false;
var g_lang;

function GetLang()
{
	g_lang = apGlobalObj.apGetLangExt(apGlobalObj.LangHost());
	if(g_lang == "JP" || g_lang == "KO" || g_lang == "CHS" || g_lang == "CHT")
		g_OnDBCS = true;
}

function unesc001() {

    apInitTest( "unesc001 ");
    GetLang();

    var sExp, sAct;
    var sClean="";

    //--------------------------------------------------------------------
    apInitScenario( "1. escaped char 0x00 (unprintable)");
    
    if ( "%00"  != escape(unescape("%00")) )
        apLogFailInfo( "char 0x00 failed", "", "", "");


    //--------------------------------------------------------------------
    apInitScenario( "2. escaped single char 0x01-0x1f (unprintable)");

    sExp = '\b';
    sAct = unescape( "%08" );

    if (sAct != sExp)
        apLogFailInfo( "escaped single char 0x01-0x1f--'\\b' failed", sExp, sAct, "");

    sExp = '\t';
    sAct = unescape( "%09" );

    if (sAct != sExp)
        apLogFailInfo( "escaped single char 0x01-0x1f--'\\t' failed", sExp, sAct, "");

    sExp = '\n';
    sAct = unescape( "%0A" );

    if (sAct != sExp)
        apLogFailInfo( "escaped single char 0x01-0x1f--'\\n' failed", sExp, sAct, "");

    sExp = '\f';
    sAct = unescape( "%0C" );

    if (sAct != sExp)
        apLogFailInfo( "escaped single char 0x01-0x1f--'\\f' failed", sExp, sAct, "");

    sExp = '\r';
    sAct = unescape( "%0D" );

    if (sAct != sExp)
        apLogFailInfo( "escaped single char 0x01-0x1f--'\\r' failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "3. repeated escaped char 0x01-0x1f (unprintable)");

    sExp = '\b\b\b\b\b\b\b\b';
    sAct = unescape( "%08%08%08%08%08%08%08%08" );

    if (sAct != sExp)
        apLogFailInfo( "repeated escaped char 0x01-0x1f--'\\b' failed", sExp, sAct, "");

    sExp = '\t\t\t\t';
    sAct = unescape( "%09%09%09%09" );

    if (sAct != sExp)
        apLogFailInfo( "repeated escaped char 0x01-0x1f--'\\t' failed", sExp, sAct, "");

    sExp = '\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n';
    sAct = unescape( "%0A%0A%0A%0A%0A%0A%0A%0A%0A%0A%0A%0A%0A%0A%0A%0A%0A%0A%0A%0A%0A%0A" );

    if (sAct != sExp)
        apLogFailInfo( "repeated escaped char 0x01-0x1f--'\\n' failed", sExp, sAct, "");

    sExp = '\f\f';
    sAct = unescape( "%0C%0C" );

    if (sAct != sExp)
        apLogFailInfo( "repeated escaped char 0x01-0x1f--'\\f' failed", sExp, sAct, "");

    sExp = '\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r';
    sAct = unescape( "%0D%0D%0D%0D%0D%0D%0D%0D%0D%0D%0D%0D%0D%0D%0D" );

    if (sAct != sExp)
        apLogFailInfo( "repeated escaped char 0x01-0x1f--'\\r' failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "4. escaped chars 0x01-0x1f (unprintable), ordered");

    sExp = '\b\t\n\f\r';
    sAct = unescape( "%08%09%0A%0C%0D" );

    if (sAct != sExp)
        apLogFailInfo( "escaped chars 0x01-0x1f (unprintable), ordered failed", sExp, sAct, "");


    //--------------------------------------------------------------------
    apInitScenario( "5. escaped chars 0x01-0x1f (unprintable), unordered");
 
    sExp = '\t\n\r\b\f';
    sAct = unescape( "%09%0A%0D%08%0C" );

    if (sAct != sExp)
        apLogFailInfo( "escaped chars 0x01-0x1f (unprintable), unordered failed", sExp, sAct, "");


    //---------------Chars 0x20-0x7f------------------------------------------

        //--------------------------------------------------------------------
        apInitScenario( "6. single escaped space");
    
        sExp = " ";
        sAct = unescape( '%20' );

        if (sAct != sExp)
            apLogFailInfo( "single space failed", sExp, sAct, "");


        //--------------------------------------------------------------------
        apInitScenario( "7. multiple escaped spaces");
    
        sExp = "                  ";
        sAct = unescape( "%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20%20" );

        if (sAct != sExp)
            apLogFailInfo( "multiple escaped spaces failed", sExp, sAct, "");


        //--------------------------------------------------------------------
        apInitScenario( "8. single escaped char uc alpha");

        sExp = 'M';
        sAct = unescape( '%4D' );

        if (sAct != sExp)
            apLogFailInfo( "single escaped char uc alpha failed", sExp, sAct, "");

        sExp = 'A';
        sAct = unescape( '%41' );

        if (sAct != sExp)
            apLogFailInfo( "single escaped char uc alpha failed", sExp, sAct, "");


        //--------------------------------------------------------------------
        apInitScenario( "9. single escaped char lc alpha");

        sExp = 's';
        sAct = unescape( '%73' );

        if (sAct != sExp)
            apLogFailInfo( "single escaped char lc alpha failed", sExp, sAct, "");

        sExp = 'i';
        sAct = unescape( '%69' );

        if (sAct != sExp)
            apLogFailInfo( "single escaped char lc alpha failed", sExp, sAct, "");


        //--------------------------------------------------------------------
        apInitScenario( "10. single escaped char misc");

        sExp = '#';
        sAct = unescape( '%23' );

        if (sAct != sExp)
            apLogFailInfo( "single escaped char misc failed", sExp, sAct, "");

        sExp = '!';
        sAct = unescape( '%21' );

        if (sAct != sExp)
            apLogFailInfo( "single escaped char misc failed", sExp, sAct, "");

        sExp = '[';
        sAct = unescape( '%5B' );

        if (sAct != sExp)
            apLogFailInfo( "single escaped char misc failed", sExp, sAct, "");

        sExp = '@';
        sAct = unescape( '%40' );

        if (sAct != sExp)
            apLogFailInfo( "single escaped char misc failed", sExp, sAct, "");


        //--------------------------------------------------------------------
        apInitScenario( "11. single escaped char num");

        sExp = "4";
        sAct = unescape( "%34" );

        if (sAct != sExp)
            apLogFailInfo( "single escaped char num failed", sExp, sAct, "");

        sExp = "0";
        sAct = unescape( "%30" );

        if (sAct != sExp)
            apLogFailInfo( "single escaped char num failed", sExp, sAct, "");

        sExp = "7";
        sAct = unescape( "%37" );

        if (sAct != sExp)
            apLogFailInfo( "single escaped char num failed", sExp, sAct, "");


        //--------------------------------------------------------------------
        apInitScenario( "12. repeated escaped char uc alpha");

        sExp = 'QQQQQQQQQQQQQQQQQQQQQQQQ';
        sAct = unescape( '%51%51%51%51%51%51%51%51%51%51%51%51%51%51%51%51%51%51%51%51%51%51%51%51' );

        if (sAct != sExp)
            apLogFailInfo( "repeated escaped char uc alpha failed", sExp, sAct, "");

        sExp = 'TTTTTTTTTTTTT';
        sAct = unescape( '%54%54%54%54%54%54%54%54%54%54%54%54%54' );

        if (sAct != sExp)
            apLogFailInfo( "repeated escaped char uc alpha failed", sExp, sAct, "");


        //--------------------------------------------------------------------
        apInitScenario( "13. repeated escaped char lc alpha");

        sExp = 'pppppppppppppppppppppppppppppppp';
        sAct = unescape( '%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70%70' );

        if (sAct != sExp)
            apLogFailInfo( "repeated escaped char lc alpha failed", sExp, sAct, "");

        sExp = 'zzzzzzzzzzzzzzzzzzzzzzzz';
        sAct = unescape( '%7A%7A%7A%7A%7A%7A%7A%7A%7A%7A%7A%7A%7A%7A%7A%7A%7A%7A%7A%7A%7A%7A%7A%7A' );

        if (sAct != sExp)
            apLogFailInfo( "repeated escaped char lc alpha failed", sExp, sAct, "");


        //--------------------------------------------------------------------
        apInitScenario( "14. repeated escaped char misc");

        sExp = '##########';
        sAct = unescape( '%23%23%23%23%23%23%23%23%23%23' );

        if (sAct != sExp)
            apLogFailInfo( "repeated escaped char misc failed", sExp, sAct, "");

        sExp = '!!!!!!!!!!!!!!';
        sAct = unescape( '%21%21%21%21%21%21%21%21%21%21%21%21%21%21' );

        if (sAct != sExp)
            apLogFailInfo( "repeated escaped char misc failed", sExp, sAct, "");

        sExp = '[[[[';
        sAct = unescape( '%5B%5B%5B%5B' );

        if (sAct != sExp)
            apLogFailInfo( "repeated escaped char misc failed", sExp, sAct, "");

        sExp = '@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@';
        sAct = unescape( '%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40' );

        if (sAct != sExp)
            apLogFailInfo( "repeated escaped char misc failed", sExp, sAct, "");


        //--------------------------------------------------------------------
        apInitScenario( "15. repeated escaped char num");

        sExp = "11111111111111111111111111111111111";
        sAct = unescape( "%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31%31" );

        if (sAct != sExp)
            apLogFailInfo( "repeated escaped char num failed", sExp, sAct, "");

        sExp = "0000000000000000000000000000";
        sAct = unescape( "%30%30%30%30%30%30%30%30%30%30%30%30%30%30%30%30%30%30%30%30%30%30%30%30%30%30%30%30" );

        if (sAct != sExp)
            apLogFailInfo( "repeated char num failed", sExp, sAct, "");


        //--------------------------------------------------------------------
        apInitScenario( "16. escaped chars 0x20-0x7f, ordered");

        sExp = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
        sAct = unescape( "%20%21%22%23%24%25%26%27%28%29%2A%2B%2C%2D%2E%2F%30%31%32%33%34%35%36%37%38%39%3A%3B%3C%3D%3E%3F%40%41%42%43%44%45%46%47%48%49%4A%4B%4C%4D%4E%4F%50%51%52%53%54%55%56%57%58%59%5A%5B%5D%5E%5F%60%61%62%63%64%65%66%67%68%69%6A%6B%6C%6D%6E%6F%70%71%72%73%74%75%76%77%78%79%7A%7B%7C%7D%7E%7F" );

        if (sAct != sExp)
            apLogFailInfo( "escaped chars 0x20-0x7f, ordered", sExp, sAct, "");


        //--------------------------------------------------------------------
        apInitScenario( "17. escaped chars 0x20-0x7f, unordered");

        sExp = "0tjzL>E!(B#CJHg-;XwU^vI9nQ\'G`Wm&hDA i6k8c]=|YOKN)b3*:xd[,.~2?y/uS+MqlV\"pPR4TZ%7{o$f@e_F5<r1}as";
        sAct = unescape( "%30%74%7F%6A%7A%4C%3E%45%21%28%42%23%43%4A%48%67%2D%3B%58%77%55%5E%76%49%39%6E%51%27%47%60%57%6D%26%68%44%41%20%69%36%6B%38%63%5D%3D%7C%59%4F%4B%4E%29%62%33%2A%3A%78%64%5B%2C%2E%7E%32%3F%79%2F%75%53%2B%4D%71%6C%56%22%70%50%52%34%54%5A%25%37%7B%6F%24%66%40%65%5F%46%35%3C%72%31%7D%61%73" );

        if (sAct != sExp)
            apLogFailInfo( "escaped chars 0x20-0x7f, unordered", sExp, sAct, "");


    //-------------------------------------------------------------------------


    //---------------Chars 0x80-0xff-(Extended)-------------------------------

        //--------------------------------------------------------------------
        apInitScenario( "18. single escaped char 0x80-0xff");
	
	//Start International
        /*
	sExp = '×';
        sAct = unescape( '%D7' );

        if (sAct != sExp)
            apLogFailInfo( "single escaped char 0x80-0xff failed", sExp, sAct, "");

        sExp = 'º';
        sAct = unescape( '%BA' );

        if (sAct != sExp)
            apLogFailInfo( "single escaped char 0x80-0xff failed", sExp, sAct, "");

        sExp = '\u0094';
        sAct = unescape( '%94' );

        if (sAct != sExp)
            apLogFailInfo( "single escaped char 0x80-0xff failed", sExp, sAct, "");
	
	
        sExp = '¬';
        sAct = unescape( '%AC' );

        if (sAct != sExp)
            apLogFailInfo( "single escaped char 0x80-0xff failed", sExp, sAct, "");

	*/
	//End International

        //--------------------------------------------------------------------
        apInitScenario( "19. repeated escaped char misc");

        //Begin International
	/*
	sExp = 'çççççççççççççççççç';
        sAct = unescape( '%E7%E7%E7%E7%E7%E7%E7%E7%E7%E7%E7%E7%E7%E7%E7%E7%E7%E7' );

        if (sAct != sExp)
            apLogFailInfo( "repeated escaped char extended failed", sExp, sAct, "");

        sExp = 'ýýýýýý';
        sAct = unescape( '%FD%FD%FD%FD%FD%FD' );

        if (sAct != sExp)
            apLogFailInfo( "repeated escaped char extended failed", sExp, sAct, "");

        sExp = '¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬¬';
        sAct = unescape( '%AC%AC%AC%AC%AC%AC%AC%AC%AC%AC%AC%AC%AC%AC%AC%AC%AC%AC%AC%AC%AC%AC%AC%AC' );

        if (sAct != sExp)
            apLogFailInfo( "repeated escaped char extended failed", sExp, sAct, "");

        sExp = '¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿';
        sAct = unescape( '%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF%BF' );

        if (sAct != sExp)
            apLogFailInfo( "repeated escaped char extended failed", sExp, sAct, "");

        sExp = '«««';
        sAct = unescape( '%AB%AB%AB' );

        if (sAct != sExp)
            apLogFailInfo( "repeated escaped char extended failed", sExp, sAct, "");
	*/
	//End International

        //--------------------------------------------------------------------
        apInitScenario( "20. escaped chars 0x80-0xff, ordered");

        sExp = "\u0080\u0082\u0083\u0084\u0085\u0086\u0087\u0088\u0089\u008A\u008B\u008C\u008E\u0091\u0092\u0093\u0094\u0095\u0096\u0097\u0098\u0099\u009A\u009B\u009C\u009E\u009F\u0020\u00A1\u00A2\u00A3\u00A4\u00A5\u00A6\u00A7\u00A8\u00A9\u00AA\u00AB\u00AC\u00AD\u00AE\u00AF\u00B0\u00B1\u00B2\u00B3\u00B4\u00B5\u00B6\u00B7\u00B8\u00B9\u00BA\u00BB\u00BC\u00BD\u00BE\u00BF\u00C0\u00C1\u00C2\u00C3\u00C4\u00C5\u00C6\u00C7\u00C8\u00C9\u00CA\u00CB\u00CC\u00CD\u00CE\u00CF\u00D0\u00D1\u00D2\u00D3\u00D4\u00D5\u00D6\u00D7\u00D8\u00D9\u00DA\u00DB\u00DC\u00DD\u00DE\u00DF\u00E0\u00E1\u00E2\u00E3\u00E4\u00E5\u00E6\u00E7\u00E8\u00E9\u00EA\u00EB\u00EC\u00ED\u00EE\u00EF\u00F0\u00F1\u00F2\u00F3\u00F4\u00F5\u00F6\u00F7\u00F8\u00F9\u00FA\u00FB\u00FC\u00FD\u00FE\u00FF";
        sAct = unescape( "%80%82%83%84%85%86%87%88%89%8A%8B%8C%8E%91%92%93%94%95%96%97%98%99%9A%9B%9C%9E%9F%20%A1%A2%A3%A4%A5%A6%A7%A8%A9%AA%AB%AC%AD%AE%AF%B0%B1%B2%B3%B4%B5%B6%B7%B8%B9%BA%BB%BC%BD%BE%BF%C0%C1%C2%C3%C4%C5%C6%C7%C8%C9%CA%CB%CC%CD%CE%CF%D0%D1%D2%D3%D4%D5%D6%D7%D8%D9%DA%DB%DC%DD%DE%DF%E0%E1%E2%E3%E4%E5%E6%E7%E8%E9%EA%EB%EC%ED%EE%EF%F0%F1%F2%F3%F4%F5%F6%F7%F8%F9%FA%FB%FC%FD%FE%FF" );

        if (sAct != sExp)
            apLogFailInfo( "escaped chars 0x80-0xff, ordered", sExp, sAct, "");


        //--------------------------------------------------------------------
        apInitScenario( "21. escaped chars 0x80-0xff, unordered");

        sExp = "\u00BE\u00D1\u00D2\u009E\u0095\u00EF\u00E7\u0083\u00AE\u00B1\u00D9\u00E8\u00B4\u00FE\u00F9\u00A8\u00A2\u00AB\u0098\u00C7\u00FB\u00F6\u00EA\u00DF\u009C\u009F\u00F7\u0097\u009B\u00E0\u00D0\u00E6\u0094\u00DC\u00E4\u00FC\u00A5\u00E9\u00C2\u00BC\u00CC\u008A\u00EC\u0099\u00C8\u00A7\u00B2\u00ED\u00E2\u00F3\u008E\u00E3\u0087\u00C5\u00BA\u00C0\u00BD\u00D8\u009A\u0096\u00FD\u00CA\u0092\u00C6\u00DA\u00CB\u00B0\u00EE\u00AA\u00F2\u00F1\u00B5\u00A4\u0091\u00C4\u0084\u0093\u00A6\u0089\u00D4\u008B\u00C1\u00D3\u00F4\u0086\u00D5\u0080\u00CF\u00FA\u00F0\u00D6\u00A3\u00C3\u00DE\u00AF\u0020\u0088\u00FF\u00B3\u00B8\u00DB\u00BB\u00EB\u00E1\u00C9\u00A1\u00B6\u00F8\u00A9\u00CD\u00CE\u0085\u00E5\u008C\u00BF\u00B9\u00AC\u00AD\u00F5\u00D7\u00B7\u0082\u00DD";

        sAct = unescape( "%BE%D1%D2%9E%95%EF%E7%83%AE%B1%D9%E8%B4%FE%F9%A8%A2%AB%98%C7%FB%F6%EA%DF%9C%9F%F7%97%9B%E0%D0%E6%94%DC%E4%FC%A5%E9%C2%BC%CC%8A%EC%99%C8%A7%B2%ED%E2%F3%8E%E3%87%C5%BA%C0%BD%D8%9A%96%FD%CA%92%C6%DA%CB%B0%EE%AA%F2%F1%B5%A4%91%C4%84%93%A6%89%D4%8B%C1%D3%F4%86%D5%80%CF%FA%F0%D6%A3%C3%DE%AF%20%88%FF%B3%B8%DB%BB%EB%E1%C9%A1%B6%F8%A9%CD%CE%85%E5%8C%BF%B9%AC%AD%F5%D7%B7%82%DD" );

        if (sAct != sExp)
            apLogFailInfo( "escaped chars 0x80-0xff, unordered", sExp, sAct, "");

    apEndTest();

}



unesc001();


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
