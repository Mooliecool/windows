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


var iTestID = 73350;


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
/* -------------------------------------------------------------------------
  Lib:		hlpUri
   
		
 
  Component:	JScript
 
  Area: 	URI
 
  Purpose:	Library routines for testing URI encoding and decoding
		methods
 ---------------------------------------------------------------------------
  
 
	[00]	21-Oct-1999	    Angelach: Created
 -------------------------------------------------------------------------*/

// see if the incomming number is representing a uriReserved
// character or #

function isReserved (num)
  {
      if ( num == 35 || num == 36 || num == 38 || num == 43 ||
	   num == 44 || num == 47 || num == 58 || num == 59 ||
	   num == 61 || num == 63 || num == 64 )
	 return true ;
       else
	 return false ;
  }

// see if the incomming number is representing an English alphabet

function isAlpha (num)
  {
      if ( ( num >= 65 && num <= 90 ) || ( num >= 97 && num <= 122  ) )
	 return true ;
       else
	 return false ;
  }

// see if the incomming number is representing a decimal digit

function isNumber (num)
  {
      if ( num >= 48 && num <= 57 )
	 return true ;
       else
	 return false ;
  }

// see if the incomming number is representing a uriMark

function isMark (num)
  {
      if ( num == 33 || num == 39 || num == 40 || num == 41 ||
	   num == 42 || num == 45 || num == 46 || num == 95 || num == 126 )
	 return true ;
       else
	 return false ;
  }

// converting a (pair of) number(s) to an escape sequence.  This function is
// created through observing results from encodeURI:
// num1 is the primary number: expected to be in the range of [0x00, 0xDBFF]
// and [0xE000, 0xFFFF]
// num1 is the secondary number: when num1 is in the range of [0xD800, 0xDBFF],
// it is expected to be in the range of [0xDC00, 0xDFFF]; otherwise, this
// number is ignored
// if num3 is 1, num1 will be checked to see if it is a uriReserved number

function getEncoding (num1, num2, num3)
  {
      var iTmp ;
      var sTmp = "" ;
					    // these are characters that won't be
      if ( isAlpha (num1) || isNumber(num1) || isMark(num1) )
	   return chr(num1) ;		    // these are characters that won't be

      if ( num3 == 1 )
	if ( isReserved(num1) ) 	    // these are uriReserved characters
	  return chr(num1) ;		    // that won't be encoded by encodeURI

      if ( num1 < 0x10 )		    // char in [0, 0x7f] is
	  sTmp = "%0" + hex (num1) ;	    // represented by 0zzzzzzz
      else				    // which is its ASCII value
	if ( num1 < 0x80 )		    // in hex
	   sTmp = "%" + hex (num1) ;	    // char in [0x80, 07ff] is
	else				    // represented by 2 octets:
	  if ( num1 < 0x800 )		    // 110yyyyy 10zzzzzz
	    sTmp = "%" + hex (0xc0+Math.floor(num1/0x40)) + "%" + hex(0x80+(num1 % 0x40)) ;
	  else				    // char in [0x800, 0xd7ff] or [0xe000, 0xffff]
	    if ( num1 < 0xD800 || num1 >= 0xE000 )  // is represented by 3
	      { 			    // octets: 1110xxxx 10yyyyyy 10zzzzzz
		 iTmp = Math.floor(num1 / 0x40) ;
		 sTmp = "%" + hex (0xE0 + Math.floor(iTmp/0x40)) ;
		 sTmp = sTmp + "%" + hex (0x80 + (iTmp % 0x40)) ;
		 sTmp = sTmp + "%" + hex (0x80 + (num1 % 0x40)) ;
	       }			    // char in [0xd800, 0xdbff] followed by
	     else			    // char in [0xdc00, 0xdfff] are represented
	       {			    // by 4 octets: 11110uuu 10uuwwwww 10xxyyyy
		  iTmp = num1 - 0xD7C0 ;    // and 10zzzzzzzz
		  sTmp = "%F" + hex (Math.floor(iTmp / 0x100)) ;
		  sTmp = sTmp + "%" + hex (Math.floor((iTmp - Math.floor(iTmp/0x100) * 0x100)/0x04) + 0x80) ;
		  sTmp = sTmp + "%" + hex(((num1-0xD800) % 0x04)*0x10 + ((Math.floor((num2 - 0xDC00)/0x40)) % 0x10) + 0x80) ;
		  sTmp = sTmp + "%" + hex(num2 % 0x40 + 0x80)
               }

       return sTmp ;
  }

// return true if Exp and Act are identical; otherwise, false is returned
//  this is needed because of posponed bug 314108
@if(@aspx) expando @end   function Verify (Exp, Act, Desc, Bug){
      if ( Act != Exp )
	{
	  if ( Bug )
	    apLogFailInfo (Desc, Exp, Act, Bug) ;
	  else
	    apLogFailInfo (Desc, Exp, Act, "") ;

	  return false ;
	}
      else
	return true ;
  }
/* -------------------------------------------------------------------------
  Test: 	EURIC001
   
		
 
  Component:	JScript
 
  Major Area:	Global method
 
  Test Area:	encodeURIComponent
 
  Keyword:	encodeURIComponent, fromCharCode
 
 ---------------------------------------------------------------------------
  Purpose:	Verify the functionality of encodeURIComponent
 
  Scenarios:	1.  encodeURIComponent with an empty string

		2.  encodeURIComponent with an English alphabet

		3.  encodeURIComponent with more than one alphabets

		4.  encodeURIComponent with a decimal digit

		5.  encodeURIComponent with more than one decimal digits

		6.  encodeURIComponent with a #

		7.  encodeURIComponent with more than one #'s

		8.  encodeURIComponent with a uriMark ( -|_|.|!|~|*|'|(|) )

		9.  encodeURIComponent with more than one uriMarks

		10. encodeURIComponent with a uriReserved Char ( ;|/|?|:|@|&|=|+|$|, )

		11. encodeURIComponent with more than one Reserved Chars

		12. encodeURIComponent with a charcter that needs to be encoded

		13. encodeURIComponent with more than one chars that need to be encoded

		14. encodeURIComponent with different types of characters

		15. encodeURIComponent with constant

		16. encodeURIComponent with build-in object - not instantiated

		17. encodeURIComponent with build-in object - instantiated

		18. encodeURIComponent with user-defined object

		19. encodeURIComponent with argument other than string

  Abstract:	Call encodeURIComponent and compare its output with expected value;
		verify that encodeURIComponent returns corrected encoded string.  Also
		call decodeURIComponent with output from encodeURIComponent; verify that output
		from encodeURIComponent can be dcoded.
 ---------------------------------------------------------------------------
  Category:	Functionality
 
  Product:	JScript
 
  Related Files: lib.js, string.js, hlpUri.js
 
  Notes:
 ---------------------------------------------------------------------------
  
 
	[00]	21-Oct-1999	    Angelach: Created Test
 -------------------------------------------------------------------------*/

function euric001 ()
  {
     apInitTest( "eURIc001 ");

     var cPer = "%" ;

     var Str1 = "" ;
     var Str2 = "" ;
     var Str3 = "" ;
     var Str4 = "" ;
     var iTmp = 0 ;
     var jTmp = 0 ;

     var funcObj = new Function ("x", "y", "return(x+y)") ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 1: encodeURIComponent with an empty string");

     Str2 = encodeURIComponent ("") ;
     Verify ( "", Str2, "encodeURIComponent.1.1" ) ; // no encoding is expected

     Str1 = "" ;
     Str2 = encodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "encodeURIComponent.1.2" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 2: encodeURIComponent with an alphabet letter");

     Str2 = encodeURIComponent ("e") ;
     Verify ( "e", Str2, "encodeURIComponent.2.1" ) ;// no encoding is expected

     Str2 = encodeURIComponent ("O") ;
     Verify ( "O", Str2, "encodeURIComponent.2.2" ) ;

     Str1 = "R" ;
     Str2 = encodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "encodeURIComponent.2.3" ) ;

     Str1 = "y" ;
     Str2 = encodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "encodeURIComponent.2.4" ) ;

     Str2 = encodeURIComponent ("h".toUpperCase()) ;
     Verify ( "H", Str2, "encodeURIComponent.2.5" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(115)) ;
     Verify ( "s", Str2, "encodeURIComponent.2.6" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 3: encodeURIComponent with more than 1 alphabet letters");

     Str2 = encodeURIComponent ("dkfienvcl") ;	     // no encoding is expected
     Verify ( "dkfienvcl", Str2, "encodeURIComponent.3.1" ) ;

     Str2 = encodeURIComponent ("gggggggggggggggggggggggggggggggggg") ;
     Verify ( "gggggggggggggggggggggggggggggggggg", Str2, "encodeURIComponent.3.2" ) ;

     Str2 = encodeURIComponent ("YIKLPWMEHSAQ") ;
     Verify ( "YIKLPWMEHSAQ", Str2, "encodeURIComponent.3.3" ) ;

     Str2 = encodeURIComponent ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXX") ;
     Verify ( "XXXXXXXXXXXXXXXXXXXXXXXXXXXXX", Str2, "encodeURIComponent.3.4" ) ;

     Str2 = encodeURIComponent ("zZyYxXwWvVuUtTsSrRqqQppPooOnnNmmMllLkKKjJJiIIhHHgGGffFFeeEEddDDccCCbbBBaaaAA") ;
     Verify ( "zZyYxXwWvVuUtTsSrRqqQppPooOnnNmmMllLkKKjJJiIIhHHgGGffFFeeEEddDDccCCbbBBaaaAA", Str2, "encodeURIComponent.3.5" ) ;

     Str1 = "A" ;
     for ( iTmp = 66 ; iTmp < 91 ; iTmp++ ) // all upper case alphabets
       {
          Str1 = Str1.concat(String.fromCharCode(iTmp)) ;
	  Str2 = encodeURIComponent (Str1) ;

	  Verify ( Str1, Str2, "encodeURIComponent.3.6."+iTmp )
	}

     for ( iTmp = 97 ; iTmp < 123 ; iTmp++ ) // lower case alphabets
       {
          Str1 = Str1.concat(String.fromCharCode(iTmp)) ;
	  Str2 = encodeURIComponent (Str1) ;

	  Verify ( Str1, Str2, "encodeURIComponent.3.7."+iTmp )
        } 

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 4: encodeURIComponent with a decimal digit");

     Str2 = encodeURIComponent ("7") ;		     // no encoding is expected
     Verify ( "7", Str2, "encodeURIComponent.4.1" ) ;

     Str1 = "0" ;
     Str2 = encodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "encodeURIComponent.4.3" ) ;

     iTmp = 2 ;
     Str2 = encodeURIComponent (iTmp.toString ()) ;
     Verify ( "2", Str2, "encodeURIComponent.4.4" ) ;

     iTmp = 7 ;
     Str2 = encodeURIComponent (iTmp.toString ()) ;
     Verify ( "7", Str2, "encodeURIComponent.4.5" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 5: encodeURIComponent with more than 1 decimal digits");

     Str2 = encodeURIComponent ("11111") ;
     Verify ( "11111", Str2, "encodeURIComponent.5.1" ) ;

     Str2 = encodeURIComponent ("0192837465") ;
     Verify ( "0192837465", Str2, "encodeURIComponent.5.2" ) ;

     Str1 = "888888888888888888888888888888888888888888888888888888888888888"
     Str2 = encodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "encodeURIComponent.5.3" ) ;

     Str1 = "083467485100000398459273889731111784834195694899999999999999348884586245459258468701736534545904"
     Str2 = encodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "encodeURIComponent.5.4" ) ;

     iTmp = 13 ;
     Str2 = encodeURIComponent (iTmp) ;
     Verify ( "13", Str2, "encodeURIComponent.5.5" ) ;

     Str2 = encodeURIComponent (0x7FE) ;
     Verify ( "2046", Str2, "encodeURIComponent.5.6" ) ;

     iTmp = 0xD872 ;
     Str1 = 0xD872 + "" ;
     Str2 = encodeURIComponent (iTmp) ;
     Verify ( Str1, Str2, "encodeURIComponent.5.7" ) ;

     iTmp = 0x8200FE15 ;
     Str1 = 0x8200FE15 + "" ;
     Str2 = encodeURIComponent (iTmp) ;
     Verify ( Str1, Str2, "encodeURIComponent.5.8" ) ;

     Str1 = "" ;
     for ( iTmp = 0 ; iTmp < 389 ; iTmp++ )
       {
	  jTmp = iTmp % 10 ;

	  Str1 = Str1.concat(jTmp.toString ()) ;
	  Str2 = encodeURIComponent (Str1) ;

	  Verify ( Str1, Str2, "encodeURIComponent.5.9."+iTmp )
	}

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 6: encodeURIComponent with a #");

     Str2 = encodeURIComponent ("#") ;		     // encoding is expected
     Verify ( "%23", Str2, "encodeURIComponent.6.1" ) ;

     Str1 = "#" ;
     Str2 = encodeURIComponent (Str1) ;
     Verify ( "%23", Str2, "encodeURIComponent.6.2" ) ;

     Str2 = encodeURIComponent (unescape("#")) ;
     Verify ( "%23", Str2, "encodeURIComponent.6.3" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 7: encodeURIComponent with more than 1 #'s");

     Str2 = encodeURIComponent ("#####################################") ;
     Str1 = ""
     for ( iTmp = 0 ; iTmp < 37 ; iTmp++ )
	Str1 = Str1 + "%23" ;
     Verify ( Str1, Str2, "encodeURIComponent.7.1" ) ;

     Str1 = "##"
     Str2 = encodeURIComponent (Str1) ;
     Verify ( "%23%23", Str2, "encodeURIComponent.7.2" ) ;

     Str1 = "#" ;
     Str3 = "%23"
     for ( iTmp = 0 ; iTmp < 500 ; iTmp++ )
       {
	  Str1 = Str1 + ("#") ;
	  Str3 = Str3 + "%23" ;
	  Str2 = encodeURIComponent (Str1) ;

	  Verify ( Str3, Str2, "encodeURIComponent.7.3."+iTmp )
	}

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 8: encodeURIComponent with an uriMark");

     Str2 = encodeURIComponent ("(") ;		     // no encoding is expected
     Verify ( "(", Str2, "encodeURIComponent.8.1" ) ;

     Str1 = "!" ;
     Str2 = encodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "encodeURIComponent.8.2" ) ;

     Str2 = encodeURIComponent ("-_.!~*'()".charAt(2)) ;
     Verify ( ".", Str2, "encodeURIComponent.8.3" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 9: encodeURIComponent with more than 1 uriMarks");

     Str2 = encodeURIComponent ("---------------------------------------") ;
     Verify ( "---------------------------------------", Str2, "encodeURIComponent.9.1" ) ;

     Str1 = encodeURIComponent ("*********************") ;
     Str2 = encodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "encodeURIComponent.9.2" ) ;

     Str2 = encodeURIComponent ("   ~~~~~~~~   ".substring(3, 11)) ;
     Verify ( "~~~~~~~~", Str2, "encodeURIComponent.9.3" ) ;

     Str2 = encodeURIComponent ("('--___!.!.!.**~~~**')") ;
     Verify ( "('--___!.!.!.**~~~**')", Str2, "encodeURIComponent.9.4" ) ;

     Str1 = "-_.!~*'())('*~!._--_.!~*'())('*~!._--_.!~*'())('*~!._--_.!~*'())('*~!._-"
     Str2 = encodeURIComponent (Str1) ;
     Verify ( Str1, Str2, "encodeURIComponent.9.5" ) ;

     Str2 = encodeURIComponent ("1234567890 '!!-((~___))*....... ?".substr(11, 20)) ;
     Verify ( "'!!-((~___))*.......", Str2, "encodeURIComponent.9.6" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 10: encodeURIComponent with an uriReserved character");

     Str2 = encodeURIComponent ("+") ;		     // encoding is expected
     Verify ( "%2B", Str2, "encodeURIComponent.10.1" ) ;

     Str1 = "$" ;
     Str2 = encodeURIComponent (Str1) ;
     Verify ( "%24", Str2, "encodeURIComponent.10.2" ) ;

     Str2 = encodeURIComponent (";%^$=+abc".charAt(0)) ;
     Verify ( "%3B", Str2, "encodeURIComponent.10.3" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 11: encodeURIComponent with more than 1 uriReserved characters");

     Str2 = encodeURIComponent ("//") ;
     Verify ( "%2F%2F", Str2, "encodeURIComponent.11.1" ) ;
     Str4 = decodeURIComponent (Str2) ;
     Verify ( "//", Str4, "decodeURIComponent.11.1" ) ;

     Str2 = encodeURIComponent ("@@@@@@@@@@@@@@@@@@@@@@@@@") ;
     Str3 = "%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40%40" ;
     Verify ( Str3, Str2, "encodeURIComponent.11.2" ) ;
     Str4 = decodeURIComponent (Str2) ;
     Verify ( "@@@@@@@@@@@@@@@@@@@@@@@@@", Str4, "decodeURIComponent.11.2" ) ;

     Str2 = encodeURIComponent (":::??????????????::".substring(3, 17)) ;
     Str3 = "%3F%3F%3F%3F%3F%3F%3F%3F%3F%3F%3F%3F%3F%3F" ;
     Verify ( Str3, Str2, "encodeURIComponent.11.3" ) ;
     Str4 = decodeURIComponent (Str2) ;
     Verify ( "??????????????", Str4, "decodeURIComponent.11.3" ) ;

     Str2 = encodeURIComponent ("+=:;/,?@+$&;?&=+") ;
     Str3 = "%2B%3D%3A%3B%2F%2C%3F%40%2B%24%26%3B%3F%26%3D%2B" ;
     Verify ( Str3, Str2, "encodeURIComponent.11.4" ) ;
     Str4 = decodeURIComponent (Str2) ;
     Verify ( "+=:;/,?@+$&;?&=+", Str4, "decodeURIComponent.11.4" ) ;

     Str1 = ",,,,+++++&&&&&&:::::::////////"
     Str2 = encodeURIComponent (Str1) ;
     Str3 = "%2C%2C%2C%2C%2B%2B%2B%2B%2B%26%26%26%26%26%26%3A%3A%3A%3A%3A%3A%3A%2F%2F%2F%2F%2F%2F%2F%2F" ;
     Verify ( Str3, Str2, "encodeURIComponent.11.5" ) ;
     Str4 = decodeURIComponent (Str2) ;
     Verify ( Str1, Str4, "decodeURIComponent.11.5" ) ;

     Str1 = "%12AB  Ca$=@?;$/6}} +?=:&@?=+,***"
     Str2 = encodeURIComponent (Str1.substr(9, 7)) ;
     Str3 = "%24%3D%40%3F%3B%24%2F" ;
     Verify ( Str3, Str2, "encodeURIComponent.11.6" ) ;
     Str4 = decodeURIComponent (Str2) ;
     Verify ( "$=@?;$/", Str4, "decodeURIComponent.11.6" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 12: encodeURIComponent with non alpha-numeric, #, uirMark, or uirReserved");

     Str2 = encodeURIComponent (" ") ;	    // encoding is expected
     Verify ( "%20", Str2, "encodeURIComponent.12.1" ) ; // asc code of " " in hex
     Str3 = decodeURIComponent (Str2) ;     // because encoding result of ehar's
     Verify ( " ", Str3, "decodeURIComponent.12.1" ) ;	 // in the range of [0, 127] is the
					    // ascii value of the character
     Str2 = encodeURIComponent ("\t") ;
     Verify ( "%09", Str2, "encodeURIComponent.12.2" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( "\t", Str3, "decodeURIComponent.12.2" ) ;

     Str2 = encodeURIComponent (cPer) ;
     Verify ( "%25", Str2, "encodeURIComponent.12.3" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( "%", Str3, "decodeURIComponent.12.3" ) ;

     Str4 = "\r" ;
     Str2 = encodeURIComponent (Str4) ;
     Str1 = "%0D" ;
     Verify ( Str1, Str2, "encodeURIComponent.12.4" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "decodeURIComponent.12.4" ) ;

     Str4 = String.fromCharCode(123) ;
     Str2 = encodeURIComponent (Str4) ;
     Str1 = cPer + hex(123) ;		    // convert 123 into hex and add
     Verify ( Str1, Str2, "encodeURIComponent.12.5" ) ; // a leading %
     Str3 = decodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "decodeURIComponent.12.5" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(3)) ;  // unprintable character
     Str1 = getEncoding (3) ;		      // expected encoded uri
     Verify ( Str1, Str2, "encodeURIComponent.12.6" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(3), Str3, "decodeURIComponent.12.6" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(127)) ;
     Str1 = getEncoding (127) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.7" ) ; // compare result with expected
     Str3 = decodeURIComponent (Str2) ; 	     // value; also check with decodeURIComponent
     Verify ( String.fromCharCode(127), Str3, "decodeURIComponent.12.7" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(128)) ; // in the range of [128, 2047]
     Str1 = getEncoding (128) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.8" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(128), Str3, "decodeURIComponent.12.8" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(209)) ;
     Str1 = getEncoding (209) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.9" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(209), Str3, "decodeURIComponent.12.9" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(256)) ;
     Str1 = getEncoding (256) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.10" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(256), Str3, "decodeURIComponent.12.10" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(541)) ;
     Str1 = getEncoding (541) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.11" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(541), Str3, "decodeURIComponent.12.11" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(894)) ;
     Str1 = getEncoding (894) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.12" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(894), Str3, "decodeURIComponent.12.12" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(1379)) ;
     Str1 = getEncoding (1379) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.13" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(1379), Str3, "decodeURIComponent.12.13" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(1686)) ;
     Str1 = getEncoding (1686) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.14" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(1686), Str3, "decodeURIComponent.12.14" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(0x801)) ;// in the range of
     Str1 = getEncoding (0x801) ;		    // [2048, 55295]
     Verify ( Str1, Str2, "encodeURIComponent.12.15" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(0x801), Str3, "decodeURIComponent.12.15" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(2792)) ;
     Str1 = getEncoding (2792) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.16" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(2792), Str3, "decodeURIComponent.12.16" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(5484)) ;
     Str1 = getEncoding (5484) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.17" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(5484), Str3, "decodeURIComponent.12.17" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(10868)) ;
     Str1 = getEncoding (10868) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.18" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(10868), Str3, "decodeURIComponent.12.18" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(21736)) ;
     Str1 = getEncoding (21736) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.19" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(21736), Str3, "decodeURIComponent.12.19" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(42472)) ;
     Str1 = getEncoding (42472) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.20" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(42472), Str3, "decodeURIComponent.12.20" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(40017)) ;
     Str1 = getEncoding (40017) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.21" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(40017), Str3, "decodeURIComponent.12.21" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(55280)) ;
     Str1 = getEncoding (55280) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.22" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(55280), Str3, "decodeURIComponent.12.22" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(57350)) ;// in the range of
     Str1 = getEncoding (57350) ;		    // [57344, 65535]
     Verify ( Str1, Str2, "encodeURIComponent.12.23" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(57350), Str3, "decodeURIComponent.12.23" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(60429)) ;
     Str1 = getEncoding (60429) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.24" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(60429), Str3, "decodeURIComponent.12.24" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(63471)) ;
     Str1 = getEncoding (63471) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.25" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(63471), Str3, "decodeURIComponent.12.25" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(0xFFFD)) ;
     Str1 = getEncoding (0xFFFD) ;
     Verify ( Str1, Str2, "encodeURIComponent.12.26" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(0xFFFD), Str3, "decodeURIComponent.12.26" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 13: encodeURIComponent with more than 1 'encodeable' characters");

     Str2 = encodeURIComponent ("\n<>\f") ;	   // in the range of [0, 127]
     Verify ( "%0A%3C%3E%0C", Str2, "encodeURIComponent.13.1" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( "\n<>\f", Str3, "decodeURIComponent.13.1" ) ;

     Str2 = encodeURIComponent ("[[[[[[[[[[[[[[[") ;
     Str1 = "" ;
     for ( iTmp = 0 ; iTmp < 15 ; iTmp++ )
	Str1 = Str1 + "%5B" ;
     Verify ( Str1, Str2, "encodeURIComponent.13.2" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( "[[[[[[[[[[[[[[[", Str3, "decodeURIComponent.13.2" ) ;

//   Str2 = encodeURIComponent ("ˆ•Òß˜") ;	  // 710 382 181 210 223 732
     Str2 = encodeURIComponent (String.fromCharCode(710, 382, 181, 210, 223, 732)) ;
     Str1 = getEncoding (710)+getEncoding(382)+getEncoding(181)+getEncoding (210) ;
     Str1 = Str1+getEncoding(223)+getEncoding(732) ;
     Verify ( Str1, Str2, "encodeURIComponent.13.3" ) ; // in the range of [128, 2047]
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(710, 382, 181, 210, 223, 732), Str3, "decodeURIComponent.13.3" ) ;

     Str4 = String.fromCharCode(0xa4, 0xa4, 0xb0, 0xea) ;
     Str1 = getEncoding(0xa4) + getEncoding(0xa4) + getEncoding(0xb0) + getEncoding(0xea) ;
     Str2 = encodeURIComponent(Str4) ;
     Verify ( Str1, Str2, "encodeURIComponent.13.4" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "decodeURIComponent.13.4" ) ;

     Str4 = "" ;
     Str1 = "" ;
     for ( iTmp = 0 ; iTmp < 16 ; iTmp++ )
       {
	  Str4 = Str4 + String.fromCharCode(235) ;
	  Str1 = Str1 + getEncoding(235) ;
       }
     Str2 = encodeURIComponent (Str4) ;
     Verify ( Str1, Str2, "encodeURIComponent.13.5" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "decodeURIComponent.13.5" ) ;

     Str2 = encodeURIComponent (String.fromCharCode(2039, 1573, 865, 412, 128)) ;
     Str1 = getEncoding(2039)+getEncoding(1573)+getEncoding(865)+getEncoding(412)+getEncoding(128) ;
     Verify ( Str1, Str2, "encodeURIComponent.13.6" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( String.fromCharCode(2039, 1573, 865, 412, 128), Str3, "decodeURIComponent.13.5" ) ;

     Str4 = "" ;
     Str1 = "" ;
     for ( iTmp = 0 ; iTmp < 30 ; iTmp++ )
       {
	  Str4 = Str4 + String.fromCharCode(2000) ;
	  Str1 = Str1 + getEncoding(2000) ;
       }
     Str2 = encodeURIComponent (Str4) ;
     Verify ( Str1, Str2, "encodeURIComponent.13.7" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "decodeURIComponent.13.7" ) ;

     Str4 = String.fromCharCode(49999, 17640, 5549, 28200, 8231, 2717) ;
     Str2 = encodeURIComponent (Str4) ; 	     // in the range of [2048, 55295]
     Str1 = getEncoding(49999)+getEncoding(17640)+getEncoding(5549) ;
     Str1 = Str1+getEncoding(28200)+getEncoding(8231)+getEncoding(2717) ;
     Verify ( Str1, Str2, "encodeURIComponent.13.8" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "decodeURIComponent.13.8" ) ;

     Str4 = "" ;
     Str1 = "" ;
     for ( iTmp = 0 ; iTmp < 20 ; iTmp++ )
       {
	  Str4 = Str4 + String.fromCharCode(6199) ;
	  Str1 = Str1 + getEncoding(6199) ;
       }
     Str2 = encodeURIComponent (Str4) ;
     Verify ( Str1, Str2, "encodeURIComponent.13.9" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "decodeURIComponent.13.9" ) ;

     Str4 = String.fromCharCode(8732, 2100, 45724, 11436, 22862, 5718) ;
     Str2 = encodeURIComponent (Str4) ;
     Str1 = getEncoding(8732)+getEncoding(2100)+getEncoding(45724) ;
     Str1 = Str1+getEncoding(11436)+getEncoding(22862)+getEncoding(5718) ;
     Verify ( Str1, Str2, "encodeURIComponent.13.10" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "decodeURIComponent.13.10" ) ;

     Str4 = "" ;
     Str1 = "" ;
     for ( iTmp = 0 ; iTmp < 12 ; iTmp++ )
       {
	  Str4 = Str4 + String.fromCharCode(56287, 57021) ;
	  Str1 = Str1 + getEncoding(56287, 57021) ;
       }
     Str2 = encodeURIComponent (Str4) ;
     Verify ( Str1, Str2, "encodeURIComponent.13.11" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "decodeURIComponent.13.11" ) ;

     Str4 = String.fromCharCode(55296, 56320, 55979, 57003, 55637, 56661, 56319, 57343, 56206, 57231) ;
     Str2 = encodeURIComponent (Str4) ; 	     // in the range of [55296, 56319][56320, 57343]
     Str1 = getEncoding(55296, 56320)+getEncoding(55979, 57003) ;
     Str1 = Str1+getEncoding(55637, 56661)+getEncoding(56319, 57343) ;
     Str1 = Str1+getEncoding(56206, 57231) ;
     Verify ( Str1, Str2, "encodeURIComponent.13.12" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "decodeURIComponent.13.12" ) ;

     Str4 = String.fromCharCode(57345, 64212, 60189, 65528, 62643, 58996) ;
     Str2 = encodeURIComponent (Str4) ; 	     // in the range of [57344, 65535]
     Str1 = getEncoding(57345)+getEncoding(64212)+getEncoding(60189) ;
     Str1 = Str1+getEncoding(65528)+getEncoding(62643)+getEncoding(58996) ;
     Verify ( Str1, Str2, "encodeURIComponent.13.13" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "decodeURIComponent.13.13" ) ;

     Str4 = String.fromCharCode(10, 120, 1211, 230, 31376, 2, 125, 4111, 65300, 55435, 56435, 2668) ;
     Str2 = encodeURIComponent (Str4) ; 	     // mixed characters
     Str1 = getEncoding(10)+getEncoding(120)+getEncoding(1211)+getEncoding(230) ;
     Str1 = Str1+getEncoding(31376)+getEncoding(2)+getEncoding(125)+getEncoding(4111) ;
     Str1 = Str1+getEncoding(65300)+getEncoding(55435, 56435)+getEncoding(2668) ;
     Verify ( Str1, Str2, "encodeURIComponent.13.14" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "decodeURIComponent.13.14" ) ;

     for ( iTmp = 0 ; iTmp < 1024 ; iTmp++ )
       {
	 Str4 = String.fromCharCode(2100+iTmp*51, 57350+iTmp*7, 130+iTmp, 55296+iTmp, 57343-iTmp) ;
	 Str2 = encodeURIComponent (Str4) ;
	 Str1 = getEncoding(2100+iTmp*51) + getEncoding(57350+iTmp*7)
	 Str1 = Str1 + getEncoding(130+iTmp) + getEncoding(55296+iTmp, 57343-iTmp) ;
	 Verify ( Str1, Str2, "encodeURIComponent.13.15." + iTmp ) ;
	 Str3 = decodeURIComponent (Str2) ;
	 Verify ( Str4, Str3, "decodeURIComponent.13.15." + iTmp) ;
       }

     Str4 = "%0F%C2%B8%D5%E7%F4" ;
     Str2 = encodeURIComponent (Str4) ;
     Str1 = "%250F%25C2%25B8%25D5%25E7%25F4" ;
     Verify ( Str1, Str2, "encodeURIComponent.13.16" ) ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "decodeURIComponent.13.16") ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 14: encodeURIComponent with different types of characters");

     Str4 = "http://ms" + String.fromCharCode(222, 1899) + "dn.microsoft.com/scripting/" ;
     Str4 = Str4 + String.fromCharCode(135, 55932, 56884, 6, 60844) ;
     Str2 = encodeURIComponent (Str4) ;
     Str1 = "http%3A%2F%2Fms" + getEncoding(222) + getEncoding(1899) ;
     Str1 = Str1 + "dn.microsoft.com%2Fscripting%2F" + getEncoding(135) ;
     Str1 = Str1 + getEncoding(55932, 56884) + getEncoding(6) + getEncoding(60844) ;
     Verify ( Str1, Str2, "encodeURIComponent.14.1") ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "decodeURIComponent.14.1") ;

     Str4 = String.fromCharCode(22195, 57349, 1, 276, 56309, 57303, 29, 22651, 1988) ;
     Str4 = Str4 + "=test+ing.com" ;
     Str2 = encodeURIComponent (Str4) ;
     Str1 = getEncoding(22195) + getEncoding(57349) + getEncoding(1) + getEncoding(276) ;
     Str1 = Str1 + getEncoding(56309, 57303) + getEncoding(29) + getEncoding(22651) ;
     Str1 = Str1 + getEncoding(1988) + "%3Dtest%2Bing.com" ;
     Verify ( Str1, Str2, "encodeURIComponent.14.2") ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "decodeURIComponent.14.2") ;

     Str4 = "ftp&;@$" + String.fromCharCode(1080, 55555, 56666, 27, 41993) ;
     Str4 = Str4 + "," + String.fromCharCode(59972, 426, 30892) + "the end?" ;
     Str2 = encodeURIComponent (Str4) ;
     Str1 = "ftp%26%3B%40%24" + getEncoding(1080) + getEncoding(55555, 56666) ;
     Str1 = Str1 + getEncoding(27) + getEncoding(41993) + "%2C" + getEncoding(59972) ;
     Str1 = Str1 + getEncoding(426) + getEncoding(30892) + "the%20end%3F" ;
     Verify ( Str1, Str2, "encodeURIComponent.14.3") ;
     Str3 = decodeURIComponent (Str2) ;
     Verify ( Str4, Str3, "decodeURIComponent.14.3") ;

     Str4 = "" ;
     Str1 = "" ;
     for ( iTmp = 0 ; iTmp < 128 ; iTmp++ )
       {
	 Str4 = Str4 + String.fromCharCode(57350+iTmp*34, iTmp, 2100+iTmp*364, 55300+iTmp*8) ;
	 Str4 = Str4 + String.fromCharCode(57340-iTmp*2, 55300+iTmp*2, 57340-iTmp*6) ;
	 Str4 = Str4 + String.fromCharCode(2047-iTmp*12, 57350+iTmp*16, 2047-iTmp*15) ;
	 Str4 = Str4 + String.fromCharCode(57350+iTmp*52, 2100+iTmp*130, 2100+iTmp*260) ;
	 Str4 = Str4 + String.fromCharCode(2100+iTmp*26, 57350+iTmp*46, 57350+iTmp*10) ;
	 Str4 = Str4 + String.fromCharCode(55300+iTmp*4, 57340-iTmp*3, 2100+iTmp*78) ;
	 Str4 = Str4 + String.fromCharCode(2100+iTmp*312, 2100+iTmp*104, 2047-iTmp*3) ;
	 Str4 = Str4 + String.fromCharCode(2047-iTmp*5, 57350+iTmp*64, 57350+iTmp*40) ;
	 Str4 = Str4 + String.fromCharCode(55300+iTmp*3, 57340-iTmp*8, 2100+iTmp*208) ;
	 Str4 = Str4 + String.fromCharCode(2100+iTmp*52, 2047-iTmp*10, 2100+iTmp*156) ;
	 Str4 = Str4 + String.fromCharCode(55300+iTmp*6, 57340-iTmp*4, 2100+iTmp*416) ;
	 Str4 = Str4 + String.fromCharCode(2100+iTmp*182, 57350+iTmp*22, 57350+iTmp*58) ;
	 Str4 = Str4 + String.fromCharCode(57350+iTmp*28, 57350+iTmp*4) ;
	 Str2 = encodeURIComponent (Str4) ;

	 Str1 = Str1 + getEncoding(57350+iTmp*34) + getEncoding(iTmp) + getEncoding(2100+iTmp*364) ;
	 Str1 = Str1 + getEncoding(55300+iTmp*8, 57340-iTmp*2) + getEncoding(55300+iTmp*2, 57340-iTmp*6)  ;
	 Str1 = Str1 + getEncoding(2047-iTmp*12) + getEncoding(57350+iTmp*16) + getEncoding(2047-iTmp*15) ;
	 Str1 = Str1 + getEncoding(57350+iTmp*52) + getEncoding(2100+iTmp*130) ;
	 Str1 = Str1 + getEncoding(2100+iTmp*260) + getEncoding(2100+iTmp*26) ;
	 Str1 = Str1 + getEncoding(57350+iTmp*46) + getEncoding(57350+iTmp*10) ;
	 Str1 = Str1 + getEncoding(55300+iTmp*4, 57340-iTmp*3) + getEncoding(2100+iTmp*78) ;
	 Str1 = Str1 + getEncoding(2100+iTmp*312) + getEncoding(2100+iTmp*104) ;
	 Str1 = Str1 + getEncoding(2047-iTmp*3) + getEncoding(2047-iTmp*5) ;
	 Str1 = Str1 + getEncoding(57350+iTmp*64) + getEncoding(57350+iTmp*40) ;
	 Str1 = Str1 + getEncoding(55300+iTmp*3, 57340-iTmp*8) + getEncoding(2100+iTmp*208) ;
	 Str1 = Str1 + getEncoding(2100+iTmp*52) + getEncoding(2047-iTmp*10) ;
	 Str1 = Str1 + getEncoding(2100+iTmp*156) + getEncoding(55300+iTmp*6, 57340-iTmp*4) ;
	 Str1 = Str1 + getEncoding(2100+iTmp*416) + getEncoding (2100+iTmp*182) ;
	 Str1 = Str1 + getEncoding(57350+iTmp*22) + getEncoding(57350+iTmp*58) ;
	 Str1 = Str1 + getEncoding(57350+iTmp*28) + getEncoding (57350+iTmp*4) ;
	 Verify ( Str1, Str2, "encodeURIComponent.14.4." + iTmp ) ;

	 Str3 = decodeURIComponent (Str2) ;
	 Verify ( Str4, Str3, "decodeURIComponent.14.4." + iTmp) ;
       }

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 15: encodeURIComponent with constant");

     Str2 = encodeURIComponent (true) ;
     Verify ( "true", Str2, "encodeURIComponent.15.1" ) ; // no encoding is expected

     Str2 = encodeURIComponent (false) ;
     Verify ( "false", Str2, "encodeURIComponent.15.2" ) ;

     Str2 = encodeURIComponent (null) ;
     Verify ( "null", Str2, "encodeURIComponent.15.3" ) ;

     //----------------------------------------------------------------------
     apInitScenario( "Scenario 16: encodeURIComponent with build-in object - not instantiated");

     Str2 = encodeURIComponent(Array) ;
@if (@_jscript_version < 7.0)
     Str1 = "%0Afunction%20Array()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@else
     Str1 = "function%20Array()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
@end
     Verify ( Str1, Str2, "encodeURIComponent.16.1" ) ;

     Str2 = encodeURIComponent (Boolean) ;
@if (@_jscript_version < 7.0)
     Str1 = "%0Afunction%20Boolean()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@else
     Str1 = "function%20Boolean()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D"
@end
     Verify ( Str1, Str2, "encodeURIComponent.16.2" ) ;

     Str2 = encodeURIComponent (Date) ;
@if (@_jscript_version < 7.0)
     Str1 = "%0Afunction%20Date()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@else
     Str1 = "function%20Date()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
@end
     Verify ( Str1, Str2, "encodeURIComponent.16.3" ) ;

     Str2 = encodeURIComponent (Enumerator) ;
@if (@_jscript_version < 7.0)
     Str1 = "%0Afunction%20Enumerator()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@else
     Str1 = "function%20Enumerator()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
@end
     Verify ( Str1, Str2, "encodeURIComponent.16.4" ) ;

     Str2 = encodeURIComponent (Error) ;
@if (@_jscript_version < 7.0)
     Str1 = "%0Afunction%20Error()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@else
     Str1 = "function%20Error()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
@end
     Verify ( Str1, Str2, "encodeURIComponent.16.5" ) ;

     Str2 = encodeURIComponent (Function) ;
@if (@_jscript_version < 7.0)
     Str1 = "%0Afunction%20Function()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@else
     Str1 = "function%20Function()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
@end
     Verify ( Str1, Str2, "encodeURIComponent.16.6" ) ;

     Str2 = encodeURIComponent (Math) ;
     Str1 = "%5Bobject%20Math%5D" ;
     Verify ( Str1, Str2, "encodeURIComponent.16.7" ) ;

     Str2 = encodeURIComponent (Number) ;
@if (@_jscript_version < 7.0)
     Str1 = "%0Afunction%20Number()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@else
     Str1 = "function%20Number()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
@end
     Verify ( Str1, Str2, "encodeURIComponent.16.8" ) ;

     Str2 = encodeURIComponent (RegExp) ;
@if (@_jscript_version < 7.0)
     Str1 = "%0Afunction%20RegExp()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@else
     Str1 = "function%20RegExp()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D";
@end
     Verify ( Str1, Str2, "encodeURIComponent.16.9" ) ;

     Str2 = encodeURIComponent (String) ;
@if (@_jscript_version < 7.0)
     Str1 = "%0Afunction%20String()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D%0A" ;
@else
     Str1 = "function%20String()%20%7B%0A%20%20%20%20%5Bnative%20code%5D%0A%7D"
@end
     Verify ( Str1, Str2, "encodeURIComponent.16.10" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 17: encodeURIComponent with build-in object - instantiated");

     Str2 = encodeURIComponent( new Array (10) ) ;
     Str1 = "%2C%2C%2C%2C%2C%2C%2C%2C%2C" ;
     Verify ( Str1, Str2, "encodeURIComponent.17.1" ) ;

     Str2 = encodeURIComponent( new Array (2, 3, 4) ) ;
     Str1 = "2%2C3%2C4" ;
     Verify ( Str1, Str2, "encodeURIComponent.17.2" ) ;

     Str2 = encodeURIComponent (new Boolean (true)) ;
     Str1 = "true" ;
     Verify ( Str1, Str2, "encodeURIComponent.17.3" ) ;

     if( new Date("12/25/2002 12:00:00 PST").getTimezoneOffset()/60 == 8) {
     Str2 = encodeURIComponent (new Date(25542)) ;
     Str1 = "Wed%20Dec%2031%2016%3A00%3A25%20PST%201969" ;
     Verify ( Str1, Str2, "encodeURIComponent.17.4" ) ;
     }

     Str2 = encodeURIComponent (new Enumerator () ) ;
     Str1 = "%5Bobject%20Object%5D" ;
     Verify ( Str1, Str2, "encodeURIComponent.17.5" ) ;

     Str2 = encodeURIComponent (ScriptEngine () ) ;
     Str1 = "JScript" ;
     Verify ( Str1, Str2, "encodeURIComponent.17.5" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 18: encodeURIComponent with user-defined object");

     Str2 = encodeURIComponent (funcObj) ;
@if (@_jscript_version < 7.0)
     Str1 = "function%20anonymous(x%2C%20y)%20%7B%0Areturn(x%2By)%0A%7D"
@else
     Str1 = "function%20anonymous(x%2C%20y)%20%7B%0Areturn(x%2By)%0A%7D";
@end
     Verify ( Str1, Str2, "encodeURIComponent.18.1" ) ;

     Str2 = encodeURIComponent (new Verify ("a", "a", "c")) ;
     Str1 = "%5Bobject%20Object%5D" ;
     Verify ( Str1, Str2, "encodeURIComponent.18.2" ) ;

     Str2 = encodeURIComponent (funcObj(3, 5)) ;
     Str1 = "8"
     Verify ( Str1, Str2, "encodeURIComponent.18.3" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 19: encodeURIComponent with argument other than string");

     Str2 = encodeURIComponent () ;		     // no argument
     Str1 = "undefined" ;
     Verify ( Str1, Str2, "encodeURIComponent.19.1" ) ;

     Str2 = encodeURIComponent (65535) ;	     // numeric value
     Str1 = "65535" ;
     Verify ( Str1, Str2, "encodeURIComponent.19.2" ) ;

     Str2 = encodeURIComponent (4e+100*10) ;
     Str1 = "4e%2B101" ;
     Verify ( Str1, Str2, "encodeURIComponent.19.3" ) ;

     Str2 = encodeURIComponent (Math.ceil(179.73)) ;
     Str1 = "180" ;
     Verify ( Str1, Str2, "encodeURIComponent.19.4" ) ;

     Str2 = encodeURIComponent (iTmp) ;
     Str1 = iTmp + "" ;
     Verify ( Str1, Str2, "encodeURIComponent.19.5" ) ;

    //----------------------------------------------------------------------
     apInitScenario( "Scenario 20: encodeURIComponent repeatedly many times");

     Str1 = String.fromCharCode (4232, 154, 56164, 56641, 55149, 11) ;
     Str1 = Str1 + "// Calling encodeURIComponent many times $90+$. & it works correctly *** " ;
     for ( iTmp = 0 ; iTmp < 100 ; iTmp++ )
       {
	  Str2 = encodeURIComponent (Str1) ;
	  Str3 = decodeURIComponent (Str2) ;
	  Verify ( Str1, Str3, "encodeURIComponent.20." + iTmp ) ;
       }


     apEndTest();
  }


euric001();


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
