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
//@set @rotor=true;

import System;

var NULL_DISPATCH = null;
var apGlobalObj;
var apPlatform;
var lFailCount;
var lInScenario;

 





function addZeros (str1, num2, loc3)
  {
     var sTmp = "" ;
     var iTmp = 0 ;

     sTmp = str1 ;
     for ( iTmp = 0 ; iTmp < num2 ; iTmp++ )
       if ( loc3 == 1 )
	 sTmp = sTmp + "0" ;
       else
	 sTmp = "0" + sTmp ;

    return sTmp ;
  }




function addNegSign (str1, fNeg, sNeg)
  {
     if ( fNeg == 0 )			   
       return  "(" + str1 + ")" ;	   
					   
     if ( fNeg == 1 )			   
       return sNeg + str1 ;		   
					   
     if ( fNeg == 2 )			   
	return sNeg + " " + str1 ;

     if ( fNeg == 3 )
	return str1 + sNeg ;

     if ( fNeg == 4 )
	return str1 + " " + sNeg ;
  }




function chgLeading (str1, sDec, iLead)   
  {					  
     if ( iLead == 1 )			  
       {				  
	 if ( str1.substr(0, sDec.length) == sDec ) 
	   return "0" + str1 ;
	 else
	   return str1 ;
       }
     else				  
       if ( str1.charAt(0) == "0" && str1.substr(1, sDec.length) == sDec )
	 return str1.substr(1, str1.length-1) ;  
       else				  
	 return str1 ;
  }



function numToString (num1, num2, fNeg, sNeg, sDec, iLead)
  {					   
     var str2 = "" ;			   
     var str1 = "" ;			   
     var iTmp = 0 ;			   
     var cTmp = "" ;			   
     var negNum = false ;		   
					   
     if ( num1 < 0 )			   
       {
	 num1 *= (-1) ; 		   
	 negNum = true ;
       }

     str1 = num1.toString () ;		   
     while ( iTmp < str1.length && cTmp != '.' ) 
       {
	   cTmp = str1.charAt(iTmp) ;
           iTmp++ ;
       }

     if ( cTmp != '.' ) 		   
       {				   
	 if ( num2 > 0 )		   
	   str1 = addZeros (str1+sDec, num2, 1) ;
       }
     else
       {				   
	 iTmp-- ;			   
	 str2 = str1.substr(iTmp+1, str1.length-iTmp-1) ;  
	 str1 = str1.substring(0, iTmp);   

	 if ( num2 == 0 )		   
	   {				   
	      if  ( str2.charCodeAt(0) > 52 )
		str1 = (Math.ceil(num1)).toString () ;	
	   }
	 else
	   {
	     str1 = str1 + sDec ;	   
					   
	     if (str2.length <= num2 )	   
	       {			   
		  str1 = str1 + str2 ;	   

		  if ( str2.length < num2 )
		    str1 = addZeros (str1, num2-str2.length, 1) ;
	       }
	     else			   
	       {			   
		  if  ( str2.charCodeAt(num2) < 53 ) 
		     str1 = str1 + str2.substr(0, num2) ; 
		  else			   
		    {			   
		       for ( iTmp = 0 ; iTmp < num2 ; iTmp++ )
			   num1 *= 10 ;    
		       num1 = Math.ceil(num1) ; 

		       str1 = num1.toString () ; 
		       if ( str1.length < num2 ) 
			 str1 = addZeros (str1, num2-str1.length+1, 0) ;
						 
		       str2 = str1.substr (str1.length-num2, num2) ;
		       str1 = str1.substr (0, str1.length-num2) + sDec + str2 ;
		    }			  
	       }
	   }
       }

     str1 = chgLeading (str1, sDec, iLead) ;  

     cTmp = "0" ;			  
     iTmp = 0 ;
     while ( iTmp < str1.length && ( cTmp == "0" || cTmp == sDec ) )
       {
	  cTmp = str1.charAt(iTmp) ;
	  iTmp++ ;
       }
     if ( cTmp != "0" && cTmp != sDec && negNum ) 
       str1 = addNegSign (str1, fNeg, sNeg) ;

     return str1 ;
  }

function toGroup (str1, num2, sep3)
  {
    var lenStr = 0 ;
    var sTmp = "" ;

    lenStr = str1.length ;
    if ( num2 == 0 || num2 >= lenStr )
       return str1 ;

    sTmp = str1.substring (0, lenStr - num2 ) ;
    return toGroup (sTmp, num2, sep3) + sep3 + str1.substr(lenStr - num2, num2) ;
  }
 




var lGreg = "1" 			    
var eGreg = "2" 			    
var jEra  = "3" 			    
var tEra  = "4" 			    
var kEra  = "5" 			    



var defiTime = "1" ;			    
var deflDate = "dddd, MMMM dd, yyyy" ;	    
var defs1159 = "AM" ;			    
var defs2359 = "PM" ;			    
var defsDate = "/" ;			    
var defssDate= "M/d/yyyy" ;		    
var defsTFmt = "hh:mm:ss tt" ;		    
var defsTime = ":" ;			    
var defiCalendarType = lGreg ;		    

var defiDigits	 = "2" ;		    
var defiLzero	 = "1" ;		    
var defiNegNum	 = "1" ;		    
var defsdec	 = "." ;		    
var defsGroup	 = "3;0" ;		    
var defsList	 = "," ;		    
var defsNegSign  = "-" ;		    
var defsThousand = "," ;		    
					    


var keyiTime = "iTime" ;		    
var keylDate = "sLongDate" ;		    
var keys1159 = "s1159" ;		    
var keys2359 = "s2359" ;		    
var keysDate = "sDate" ;		    
var keyssDate= "sShortDate" ;		    
var keysTFmt = "sTimeFormat" ;		    
var keysTime = "sTime" ;		    

var keyiDigits	 = "iDigits" ;		    
var keyiLzero	 = "iLzero" ;		    
var keyiNegNum	 = "iNegNumber" ;	    
var keysdec	 = "sDecimal" ; 	    
var keysGroup	 = "sGrouping" ;	    
var keysList	 = "sList" ;		    
var keysNegSign  = "sNegativeSign" ;	    
var keysThousand = "sThousand" ;	    
					    
var keyiCalendarType = "ICalendarType"	    



var curiTime = "" ;
var curlDate = "" ;
var curs1159 = "" ;
var curs2359 = "" ;
var cursDate = "" ;
var curssDate= "" ;
var cursTFmt = "" ;
var cursTime = "" ;

var curiDigits	 = "" ;
var curiLzero	 = "" ;
var curiNegNum	 = "" ;
var cursdec	 = "" ;
var cursGroup	 = "" ;
var cursList	 = "" ;
var cursNegSign  = "" ;
var cursThousand = "" ;

var curiCalendarType = ""
//var apGlobalObj = 0 ;			    
//apGlobalObj = 0 ;			    

@if (@_jscript_version >= 7)		    

defsGroup = "3" ;
import System.Globalization ;
import System.Threading;

class tstCulture extends CultureInfo
{
  private var m_dtFInfo : DateTimeFormatInfo ;
  private var m_noFInfo : NumberFormatInfo ;

  function tstCulture(cCulture : String)
    {
       super (cCulture) ;
       m_dtFInfo = null ;
       m_noFInfo = null ;
    }

  function get DateTimeFormat () : DateTimeFormatInfo
    {
       if  ( m_dtFInfo == null )
	  m_dtFInfo = super.DateTimeFormat ;

       return m_dtFInfo;
     }

  function SetDateTimeFormat (dtFInfo : DateTimeFormatInfo) : void
      {
	this.m_dtFInfo = dtFInfo ;
      }

  function get NumberFormat () : NumberFormatInfo
    {
       if  ( m_noFInfo == null )
	  m_noFInfo = super.NumberFormat ;

       return m_noFInfo;
     }

  function SetNumberFormat (noFInfo : NumberFormatInfo) : void
      {
	this.m_noFInfo = noFInfo ;
      }
}

public class intlHelper
{
  public function SetDateFormat (keyFmt: String, strFmt : String) : void
    {
	  var varCult : tstCulture = new tstCulture (getCurCulture ()) ;
	  var dtFInfo : DateTimeFormatInfo = Thread.CurrentThread.CurrentCulture.DateTimeFormat.Clone();

	  if ( keyFmt == "sLongDate" )
	       dtFInfo.LongDatePattern = strFmt ;

	  if ( keyFmt == "sShortDate" )
	       dtFInfo.ShortDatePattern = strFmt ;

	  if ( keyFmt == "sDate" )
	       dtFInfo.DateSeparator = strFmt ;

	  if ( keyFmt == "s1159" )
	     dtFInfo.AMDesignator = strFmt ;

	  if ( keyFmt == "s2359" )
	     dtFInfo.PMDesignator = strFmt ;

	  if ( keyFmt == "sTime" )
	     dtFInfo.TimeSeparator = strFmt ;

	  if ( keyFmt == "sTimeFormat" )
	     dtFInfo.LongTimePattern = strFmt ;

	  varCult.SetDateTimeFormat(dtFInfo) ;
	  Thread.CurrentThread.CurrentCulture = varCult ;
    }

  public function SetNumberFormat (keyFmt: String, strFmt : String) : void
    {
	  var varCult : tstCulture = new tstCulture (getCurCulture ()) ;
	  var noFInfo : NumberFormatInfo = Thread.CurrentThread.CurrentCulture.NumberFormat.Clone();

	  if ( keyFmt == "iDigits" )
	       noFInfo.NumberDecimalDigits = parseInt(strFmt) ;

    //	  if ( keyFmt == "iLzero" )		      

	  if ( keyFmt == "iNegNumber" ) 	      
	     noFInfo.NumberNegativePattern = parseInt(strFmt) ;

	  if ( keyFmt == "sDecimal" )
	     noFInfo.NumberDecimalSeparator = strFmt ;

	  if ( keyFmt == "sGrouping" )		      
	     {					      
		 var i : int = 0 ;
		 var j : int = 0 ;
		 var sTmp : String = strFmt ;
						      
		 i = sTmp.indexOf (";") ;	      
		 while ( i != -1)
		   {
		      j++ ;
		      sTmp = sTmp.substr(i+1, sTmp.length-i)
		      i = sTmp.indexOf (";")
		   }
		 var arr1 : int [] = new int[j+1] ;   
		 

		 sTmp = strFmt ;
		 i = sTmp.indexOf (";") ;
		 j = 0 ;
		 while ( i != -1)		      
		   {				      
		      arr1[j] = parseInt(sTmp.substr(0, i)) ;
		      j++ ;
		      sTmp = sTmp.substr(i+1, sTmp.length-i)
		      i = sTmp.indexOf (";")
		   }
		 arr1[j] = parseInt(sTmp) ;   

		 noFInfo.NumberGroupSizes = arr1 ;
	     }

      //  if ( keyFmt == "sList" )		      

	  if ( keyFmt == "sNegativeSign" )
	       noFInfo.NegativeSign = strFmt ;

	  if ( keyFmt == "sThousand" )
	     noFInfo.NumberGroupSeparator = strFmt ;

	  varCult.SetNumberFormat(noFInfo) ;
	  Thread.CurrentThread.CurrentCulture = varCult ;
    }

  private function getCurCulture () : String
    {
       var Lcid = 1033 ;		 
       
       Lcid = apGetLocale();

       switch ( Lcid )
	{
	  case 1025:			  
	    return "ar-SA" ;
	  case 1027:			  
	    return "ca-ES" ;
	  case 1028:			  
	    return "zh-TW" ;
	  case 1029:			  
	    return "cs-CZ" ;
	  case 1030:			  
	    return "da-DK" ;
	  case 1031:			  
	    return "de-DE" ;
	  case 1032:			  
	    return "el-GR" ;
	  case 1035:			  
	    return "fi-FI" ;
	  case 1036:			  
	    return "fr-FR" ;
	  case 1037:			  
	    return "he-IL" ;
	  case 1038:			  
	    return "hu-HU" ;
	  case 1039:			  
	    return "is-IS" ;
	  case 1040:			  
	    return "it-IT" ;
	  case 1041:			  
	    return "ja-JP" ;
	  case 1042:			  
	    return "ko-KR" ;
	  case 1043:			  
	    return "nl-NL" ;
	  case 1044:			  
	    return "nb-NO" ;
	  case 1045:			  
	    return "pl-PL" ;
	  case 1046:			  
	    return "pt-BR" ;
	  case 1049:			  
	    return "ru-RU" ;
	  case 1051:			  
	    return "sk-SK" ;
	  case 1053:			  
	    return "sv-SE" ;
	  case 1055:			  
	    return "tr-TR" ;
	  case 1060:			  
	    return "sl-SI" ;
    //	  case 1069:			  
    
	  case 2052:			  
	    return "zh-CN" ;
	  case 2058:			  
	    return "es-MX" ;
	  case 2070:			  
	    return "pt-PT" ;
	  case 3082:			  
	    return "es-ES" ;
	  default:
	    return "en-US" ;
	}
    }
}
@end







function apfSetIntlValue (Setting, Valu)    
  {					    
@if (@_jscript_version >= 7)
     var h = new intlHelper() ;
     if ( ( Setting.indexOf("Date") != -1 ) || ( Setting.indexOf("Time") != -1 ) ||
	  ( Setting.indexOf("59") != -1 ) )  
       h.SetDateFormat(Setting, Valu) ;
     else
       h.SetNumberFormat(Setting, Valu) ;   
     return true ;
@else
    return apGlobalObj.osSetIntlValue(Setting, Valu) ; 
@end
  }					    
					    
function apstGetIntlValue(Setting, Valu)    
  {					    
    return apGlobalObj.osGetIntlValue (Setting, Valu) ; 
  }					    
					    

function apSetIntlDateDefaults ()	    
  {					    
    var intTmp = 1 ;			    
					    
    curiCalendarType = apstGetIntlValue(keyiCalendarType, curiCalendarType) ;
    if ( curiCalendarType != defiCalendarType )
      intTmp *= apfSetIntlValue (keyiCalendarType, defiCalendarType) ;

    curiTime = apstGetIntlValue (keyiTime, curiTime) ;
    if ( curiTime != defiTime )
      intTmp *= apfSetIntlValue (keyiTime, defiTime) ;

    curlDate = apstGetIntlValue (keylDate, curlDate) ;
    if ( curlDate != deflDate )
      intTmp *= apfSetIntlValue (keylDate, deflDate) ;

    curs1159 = apstGetIntlValue (keys1159, curs1159) ;
    if ( curs1159 != defs1159 )
      intTmp *= apfSetIntlValue (keys1159, defs1159) ;

    curs2359 = apstGetIntlValue (keys2359, curs2359) ;
    if ( curs2359 != defs2359 )
      intTmp *= apfSetIntlValue (keys2359, defs2359) ;

    cursDate = apstGetIntlValue (keysDate, cursDate) ;
    if ( cursDate != defsDate )
      intTmp *= apfSetIntlValue (keysDate, defsDate) ;

    curssDate = apstGetIntlValue (keyssDate, curssDate) ;
    if ( curssDate != defssDate )
      intTmp *= apfSetIntlValue (keyssDate, defssDate) ;

    cursTFmt = apstGetIntlValue (keysTFmt, cursTFmt) ;
    if ( cursTFmt != defsTFmt )
      intTmp *= apfSetIntlValue (keysTFmt, defsTFmt) ;

    cursTime = apstGetIntlValue (keysTime, cursTime) ;
    if ( cursTime != defsTime )
      intTmp *= apfSetIntlValue (keysTime, defsTime) ;

    return intTmp ;
  }

function apSetIntlNumDefaults ()	    
  {					    
    var intTmp = 1 ;			    
					    
    curiDigits = apstGetIntlValue (keyiDigits, curiDigits) ;
    if ( curiDigits != defiDigits )
      intTmp *= apfSetIntlValue (keyiDigits, defiDigits) ;

    curiLzero = apstGetIntlValue (keyiLzero, curiLzero) ;
    if ( curiLzero != defiLzero )
      intTmp *= apfSetIntlValue (keyiLzero, defiLzero) ;

    curiNegNum = apstGetIntlValue (keyiNegNum, curiNegNum) ;
    if ( curiNegNum != defiNegNum )
      intTmp *= apfSetIntlValue (keyiNegNum, defiNegNum) ;

    cursdec = apstGetIntlValue (keysdec, cursdec) ;
    if ( cursdec != defsdec )
      intTmp *= apfSetIntlValue (keysdec, defsdec) ;

    cursGroup = apstGetIntlValue (keysGroup, cursGroup) ;
    if ( cursGroup != defsGroup )
      intTmp *= apfSetIntlValue (keysGroup, defsGroup) ;

    cursList = apstGetIntlValue (keysList, cursList) ;
    if ( cursList != defsList )
      intTmp *= apfSetIntlValue (keysList, defsList) ;

    cursNegSign = apstGetIntlValue (keysNegSign, cursNegSign) ;
    if ( cursNegSign != defsNegSign )
      intTmp *= apfSetIntlValue (keysNegSign, defsNegSign) ;

    cursThousand = apstGetIntlValue (keysThousand, cursThousand) ;
    if ( cursThousand != defsThousand )
      intTmp *= apfSetIntlValue (keysThousand, defsThousand) ;

    return intTmp ;
  }

function apRestoreIntlDateDefaults ()
  {					    
    var intTmp = 1 ;			    
					    
    intTmp *= apfSetIntlValue (keyiCalendarType , curiCalendarType) ;
    intTmp *= apfSetIntlValue (keyiTime , curiTime) ;
    intTmp *= apfSetIntlValue (keylDate , curlDate) ;
    intTmp *= apfSetIntlValue (keys1159 , curs1159) ;
    intTmp *= apfSetIntlValue (keys2359 , curs2359) ;
    intTmp *= apfSetIntlValue (keysDate , cursDate) ;
    intTmp *= apfSetIntlValue (keyssDate, curssDate) ;
    intTmp *= apfSetIntlValue (keysTFmt , cursTFmt) ;
    intTmp *= apfSetIntlValue (keysTime , cursTime) ;
					    
    return intTmp ;			    
  }

function apRestoreIntlNumDefaults ()
  {					    
    var intTmp = 1 ;			    
					    
    intTmp *= apfSetIntlValue (keyiDigits  , curiDigits) ;
    intTmp *= apfSetIntlValue (keyiLzero   , curiLzero) ;
    intTmp *= apfSetIntlValue (keyiNegNum  , curiNegNum) ;
    intTmp *= apfSetIntlValue (keysdec	   , cursdec) ;
    intTmp *= apfSetIntlValue (keysGroup   , cursGroup) ;
    intTmp *= apfSetIntlValue (keysList    , cursList) ;
    intTmp *= apfSetIntlValue (keysNegSign , cursNegSign) ;
    intTmp *= apfSetIntlValue (keysThousand, cursThousand) ;
					    
    return intTmp ;			    
  }

function getshortNameOfMonth (index)	    
  {					    
     var sTmp = "" ;			    
					    
     sTmp = apstGetIntlValue(keyiCalendarType, "") ;
					    
     if ( index == "Jan" || index == 1 )
       {
	  if ( sTmp == eGreg )		    
	    return "Jan" ;
	  else
	    return apstGetIntlValue("SAbbrevMonthName1", "") ;
       }

     if ( index == "Feb" || index == 2 )
       {
	  if ( sTmp == eGreg )		    
	    return "Feb" ;
	  else
	    return apstGetIntlValue("SAbbrevMonthName2", "") ;
       }

     if ( index == "Mar" || index == 3 )
       {
	  if ( sTmp == eGreg )		    
	    return "Mar" ;
	  else
	    return apstGetIntlValue("SAbbrevMonthName3", "") ;
       }

     if ( index == "Apr" || index == 4 )
       {
	  if ( sTmp == eGreg )		    
	    return "Apr" ;
	  else
	    return apstGetIntlValue("SAbbrevMonthName4", "") ;
       }

     if ( index == "May" || index == 5 )
       {
	  if ( sTmp == eGreg )		    
	    return "May" ;
	  else
	    return apstGetIntlValue("SAbbrevMonthName5", "") ;
       }

     if ( index == "Jun" || index == 6 )
       {
	  if ( sTmp == eGreg )		    
	    return "Jun" ;
	  else
	    return apstGetIntlValue("SAbbrevMonthName6", "") ;
       }

     if ( index == "Jul" || index == 7 )
       {
	  if ( sTmp == eGreg )		    
	    return "Jul" ;
	  else
	    return apstGetIntlValue("SAbbrevMonthName7", "") ;
       }

     if ( index == "Aug" || index == 8 )
       {
	  if ( sTmp == eGreg )		    
	    return "Aug" ;
	  else
	    return apstGetIntlValue("SAbbrevMonthName8", "") ;
       }

     if ( index == "Sep" || index == 9 )
       {
	  if ( sTmp == eGreg )		    
	    return "Sep" ;
	  else
	    return apstGetIntlValue("SAbbrevMonthName9", "") ;
       }

     if ( index == "Oct" || index == 10 )
       {
	  if ( sTmp == eGreg )		    
	    return "Oct" ;
	  else
	    return apstGetIntlValue("SAbbrevMonthName10", "") ;
       }

     if ( index == "Nov" || index == 11 )
       {
	  if ( sTmp == eGreg )
	    return "Nov" ;
	  else
	    return apstGetIntlValue("SAbbrevMonthName11", "") ;
       }

     if ( sTmp == eGreg )
       return "Dec" ;
     else
       return apstGetIntlValue("SAbbrevMonthName12", "") ;
  }

function getlongNameOfMonth (index)	    
  {					    
     var sTmp = "" ;			    
					    
     sTmp = apstGetIntlValue(keyiCalendarType, "") ;
					    
     if ( index == "Jan" || index == 1 )
       {
	  if ( sTmp == eGreg )		    
	    return "January" ;
	  else
	    return apstGetIntlValue("SMonthName1", "") ;
       }

     if ( index == "Feb" || index == 2 )
       {
	  if ( sTmp == eGreg )
	    return "February" ;
	  else
	    return apstGetIntlValue("SMonthName2", "") ;
       }
     
     if ( index == "Mar" || index == 3 )
       {
	  if ( sTmp == eGreg )
	    return "March" ;
	  else
	    return apstGetIntlValue("SMonthName3", "") ;
       }

     if ( index == "Apr" || index == 4 )
       {
	  if ( sTmp == eGreg )
	    return "April" ;
	  else
	    return apstGetIntlValue("SMonthName4", "") ;
       }
     
     if ( index == "May" || index == 5 )
       {
	  if ( sTmp == eGreg )
	    return "May" ;
	  else
	    return apstGetIntlValue("SMonthName5", "") ;
       }
     
     if ( index == "Jun" || index == 6 )
       {
	  if ( sTmp == eGreg )
	    return "June" ;
	  else
	    return apstGetIntlValue("SMonthName6", "") ;
       }
     
     if ( index == "Jul" || index == 7 )
       {
	  if ( sTmp == eGreg )
	    return "July" ;
	  else
	    return apstGetIntlValue("SMonthName7", "") ;
       }

     if ( index == "Aug" || index == 8 )
       {
	  if ( sTmp == eGreg )
	    return "August" ;
	  else
	    return apstGetIntlValue("SMonthName8", "") ;
       }

     if ( index == "Sep" || index == 9 )
       {
	  if ( sTmp == eGreg )
	    return "September" ;
	  else
	    return apstGetIntlValue("SMonthName9", "") ;
       }
     
     if ( index == "Oct" || index == 10 )
       {
	  if ( sTmp == eGreg )
	    return "October" ;
	  else
	    return apstGetIntlValue("SMonthName10", "") ;
       }
     
     if ( index == "Nov" || index == 11 )
       {
	  if ( sTmp == eGreg )
	    return "November" ;
	  else
	    return apstGetIntlValue("SMonthName11", "") ;
       }

     if ( sTmp == eGreg )
       return "December" ;
     else
       return apstGetIntlValue("SMonthName12", "") ;
  }

function getshortNameOfDay (index)	    
  {					    
     var sTmp = "" ;			    
					    
     sTmp = apstGetIntlValue(keyiCalendarType, "") ;
					    
     if ( index == "Mon" || index == 1 )
       {
	  if ( sTmp == eGreg )		    
	    return "Mon" ;
	  else
	    return apstGetIntlValue("SAbbrevDayName1", "") ;
       }

     if ( index == "Tue" || index == 2 )
       {
	  if ( sTmp == eGreg )
	    return "Tue" ;
	  else
	    return apstGetIntlValue("SAbbrevDayName2", "") ;
       }
     
     if ( index == "Wed" || index == 3 )
       {
	  if ( sTmp == eGreg )
	    return "Wed" ;
	  else
	    return apstGetIntlValue("SAbbrevDayName3", "") ;
       }

     if ( index == "Thu" || index == 4 )
       {
	  if ( sTmp == eGreg )
	    return "Thu" ;
	  else
	    return apstGetIntlValue("SAbbrevDayName4", "") ;
       }
     
     if ( index == "Fri" || index == 5 )
       {
	  if ( sTmp == eGreg )
	    return "Fri" ;
	  else
	    return apstGetIntlValue("SAbbrevDayName5", "") ;
       }
     
     if ( index == "Sat" || index == 6 )
       {
	  if ( sTmp == eGreg )
	    return "Sat" ;
	  else
	    return apstGetIntlValue("SAbbrevDayName6", "") ;
       }
     
     if ( sTmp == eGreg )
       return "Sun" ;
     else
       return apstGetIntlValue("SAbbrevDayName7", "") ;
  }

function getlongNameOfDay (index)	    
  {					    
     var sTmp = "" ;			    
					    
     sTmp = apstGetIntlValue(keyiCalendarType, "") ;
					    
     if ( index == "Mon" || index == 1 )
       {
	  if ( sTmp == eGreg )		    
	    return "Monday" ;
	  else
	    return apstGetIntlValue("SDayName1", "") ;
       }

     if ( index == "Tue" || index == 2 )
       {
	  if ( sTmp == eGreg )
	    return "Tuesday" ;
	  else
	    return apstGetIntlValue("SDayName2", "") ;
       }
     
     if ( index == "Wed" || index == 3 )
       {
	  if ( sTmp == eGreg )
	    return "Wednesday" ;
	  else
	    return apstGetIntlValue("SDayName3", "") ;
       }

     if ( index == "Thu" || index == 4 )
       {
	  if ( sTmp == eGreg )
	    return "Thursday" ;
	  else
	    return apstGetIntlValue("SDayName4", "") ;
       }
     
     if ( index == "Fri" || index == 5 )
       {
	  if ( sTmp == eGreg )
	    return "Friday" ;
	  else
	    return apstGetIntlValue("SDayName5", "") ;
       }
     
     if ( index == "Sat" || index == 6 )
       {
	  if ( sTmp == eGreg )
	    return "Saturday" ;
	  else
	    return apstGetIntlValue("SDayName6", "") ;
       }
     
     if ( sTmp == eGreg )
       return "Sunday" ;
     else
       return apstGetIntlValue("SDayName7", "") ;
  }

function getEraType ()			    
  {					    
    var lcid = 0 ;			    

    lcid = apGlobalObj.LangHost () ;	    
    if ( lcid == 1041 )
       return jEra ;			    
    if ( lcid == 1028 )
       return tEra ;			    
    if ( lcid == 1042 )
       return kEra ;			    

    return lGreg ;			    
  }

function getEraYear (obj1, str2)	    
  {					    
     var nYear = 0 ;			    
     nYear = obj1.getFullYear () ;

     if ( str2 == jEra )		    
       {				    
	  var nMonth = obj1.getMonth () ;
	  var nDate = obj1.getDate () ; ;

	  if ( nYear > 1989 || ( nYear == 1989 && ( nMonth > 0 || (nMonth == 0 && nDate > 7 ) ) ) )
	    nYear = nYear - 1989 + 1 ;	    
	  else
	    if ( nYear > 1926 || ( nYear == 1926 && ( nMonth > 11 || (nMonth == 11 && nDate > 24 ) ) ) )
	      nYear = nYear - 1926 + 1 ;    
	    else
	      if ( nYear > 1912 || ( nYear == 1912 && ( nMonth > 6 || (nMonth == 6 && nDate > 29 ) ) ) )
		nYear = nYear - 1912 + 1 ;  
	      else
		if ( nYear > 1868 || ( nYear == 1868 && ( nMonth > 8 || (nMonth == 8 && nDate > 7 ) ) ) )
		  nYear = nYear - 1868 + 1 ;
       }				    
     else
       if ( str2 == kEra )		    
	 nYear = nYear + 2333 ; 	    
       else
	 if ( str2 == tEra )		    
	   if ( nYear > 1911 )		    
	     nYear = nYear - 1911 ;

     if ( str2 == jEra || str2 == kEra || str2 == tEra )
       {
	 if ( nYear < 10 )		    
	   return "0" + nYear.toString () ; 
	 else
	   return nYear.toString () ;
       }
     else
       {
	 if ( nYear < 10 )
	   return "000" + nYear.toString () ; 
	 else
	   if ( nYear < 100 )
	     return "00" + nYear.toString () ;
	   else
	     if ( nYear < 1000 )
	       return "0" + nYear.toString () ;
	     else
	       return nYear.toString () ;
       }
  }
 


function Verify (Exp, Act, Desc, Bug)
  {
      if ( Act != Exp )
	{
	  if ( Bug )
	    apLogFailInfo (Desc, Exp, Act,"") ;
	  else
	    apLogFailInfo (Desc, Exp, Act,"") ;
	  return false ;
	}
      else
	return true ;
  }

var SV = ScriptEngineMajorVersion() + (ScriptEngineMinorVersion() / 10);

function tolst202 ()
  {
     apInitTest( "ToLSt202 ");

     if ( apSetIntlNumDefaults () )	   
       {				   
	  if ( apfSetIntlValue (keysThousand, " ") == 0 )
	     Verify ("1", "0", "apfSetIntlValue failed.1","") ;
	  else
	     Scenarios (" ", "1") ;	   
					   
	  if ( apfSetIntlValue (keysThousand, "<>") == 0 )
	     Verify ("1", "0", "apfSetIntlValue failed.2","") ;
	  else
	     Scenarios ("<>", "2") ;	   
					   
	  if ( apfSetIntlValue (keysThousand, "| %") == 0 )
	     Verify ("1", "0", "apfSetIntlValue failed.3","") ;
	  else
	     Scenarios ("| %", "3") ;	   

	  if ( apfSetIntlValue (keysThousand, ";") == 0 )
	     Verify ("1", "0", "apfSetIntlValue failed.4","") ;
	  else
	     Scenarios (";", "4") ;	   

	  if ( apfSetIntlValue (keysThousand, "  ") == 0 )
	     Verify ("1", "0", "apfSetIntlValue failed.5","") ;
	  else
	     Scenarios ("  ", "5") ;	   

	  if ( apfSetIntlValue (keysThousand, "Ths") == 0 )
	     Verify ("1", "0", "apfSetIntlValue failed.6","") ;
	  else
	     Scenarios ("Ths", "5") ;	   

	  apRestoreIntlNumDefaults () ;    
       }				   
     else
       Verify ("1", "0", "apSetIntlNumDefaults failed","") ;

     apEndTest();
  }








function Scenarios (sGrp, iCase)
  {
     var Str1 = "" ;
     var Str2 = "" ;
     var iTmp = 0 ;

     var numObj1 = new Number () ;
     var numObj2 = new Number (15) ;
     var numVar = 527 ;

    
     apInitScenario( "Scenario 1: toLocaleString on positive whole numbers") ;

     Str1 = numObj1.toLocaleString () ;    
     Verify ( "0.00", Str1, "toLocaleString.1.1."+iCase ) ;

     Str1 = numObj2.toLocaleString () ;
     Verify ( "15.00", Str1, "toLocaleString.1.2."+iCase ) ;

     Str1 = numVar.toLocaleString () ;
     Verify ( "527.00", Str1, "toLocaleString.1.3."+iCase ) ;

     Str1 = (884).toLocaleString () ;
     Verify ( "884.00", Str1, "toLocaleString.1.4."+iCase ) ;

     Str1 = (numObj2 * 9).toLocaleString () ;
     Verify ( "135.00", Str1, "toLocaleString.1.5."+iCase ) ;

     numObj1 = 1000 ;			   
     numObj2 = 16572 ;
     numVar = 439918 ;
     Str1 = numObj1.toLocaleString () ;
     Verify ( "1"+sGrp+"000.00", Str1, "toLocaleString.1.6."+iCase ) ;

     Str1 = numObj2.toLocaleString () ;
     Verify ( "16"+sGrp+"572.00", Str1, "toLocaleString.1.7."+iCase ) ;

     Str1 = numVar.toLocaleString () ;
     Verify ( "439"+sGrp+"918.00", Str1, "toLocaleString.1.8."+iCase ) ;

     Str1 = (28184).toLocaleString () ;
     Verify ( "28"+sGrp+"184.00", Str1, "toLocaleString.1.9."+iCase ) ;

     Str1 = (numObj1 * 76).toLocaleString () ;
     Verify ( "76"+sGrp+"000.00", Str1, "toLocaleString.1.10."+iCase ) ;

     numObj1 = 1000000 ;		   
     numObj2 = 17946562 ;
     numVar = 10000 * 10000 * 10000 * 10000 ;
     Str1 = numObj1.toLocaleString () ;
     Verify ( "1"+sGrp+"000"+sGrp+"000.00", Str1, "toLocaleString.1.11."+iCase ) ;

     Str1 = numObj2.toLocaleString () ;
     Verify ( "17"+sGrp+"946"+sGrp+"562.00", Str1, "toLocaleString.1.12."+iCase ) ;

     Str1 = numVar.toLocaleString () ;
     if ( Str1 != "1E+16" && Str1 != "1e+16" )
	Verify ( "10"+sGrp+"000"+sGrp+"000"+sGrp+"000"+sGrp+"000"+sGrp+"000.00", Str1, "toLocaleString.1.13."+iCase ) ;

     Str1 = (301982160).toLocaleString () ;
     Verify ( "301"+sGrp+"982"+sGrp+"160.00", Str1, "toLocaleString.1.14."+iCase ) ;

     Str1 = (numVar * 25).toLocaleString () ;
     if ( Str1 != "2.5E+17" && Str1 != "2.5e+17" )
	Verify ( "250"+sGrp+"000"+sGrp+"000"+sGrp+"000"+sGrp+"000"+sGrp+"000.00", Str1, "toLocaleString.1.15."+iCase ) ;

     numObj1 = 20e+45 ; 		   
     numObj2 = 3E+80 ;
     numVar = 40E+140 ;
     Str1 = numObj1.toLocaleString () ;
     if ( Str1 != "2E+46" && Str1 != "2e+46" )
       {
	 Str2 = "20" ;
	 for ( iTmp = 0 ; iTmp < 15 ; iTmp ++ )
	    Str2 += sGrp+"000" ;
	 Verify ( Str2+".00", Str1, "toLocaleString.1.16."+iCase ) ;
       }

     Str1 = numObj2.toLocaleString () ;
     if ( Str1 != "3E+80" && Str1 != "3e+80" )
       {
	 Str2 = "300" ;
	 for ( iTmp = 0 ; iTmp < 26 ; iTmp ++ )
	    Str2 += sGrp+"000" ;
	 Verify ( Str2+".00", Str1, "toLocaleString.1.17."+iCase ) ;
       }

     Str1 = numVar.toLocaleString () ;
     Str2 = "4" ;			   
     if ( Str1 != "4E+141" && Str1 != "4e+141" )
       {
	 for ( iTmp = 0 ; iTmp < 47 ; iTmp++ )
	   Str2 = Str2 + sGrp + "000" ;
	 Str2 = Str2 + ".00" ;
	 if ( Str1 != Str2 )
	   Verify ( "4E+141", Str1, "toLocaleString.1.18."+iCase ) ;
       }

     Str1 = (1e+60).toLocaleString () ;
     Str2 = "1" ;
     if ( Str1 != "1e+60" && Str1 != "1E+60" )
       {
	 for ( iTmp = 0 ; iTmp < 20 ; iTmp ++ )
	    Str2 += sGrp+"000" ;
	 Verify ( Str2+".00", Str1, "toLocaleString.1.19."+iCase ) ;
       }

     Str1 = (numVar * 10).toLocaleString () ;
     if ( Str1 != "4E+142" && Str1 != "4e+142" )
       {
	 Str2 = "40" ;			   
	 for ( iTmp = 0 ; iTmp < 47 ; iTmp++ )
	   Str2 = Str2 + sGrp + "000" ;
	 Str2 = Str2 + ".00" ;
	 if ( Str1 != Str2 )
	   Verify ( "4E+142", Str1, "toLocaleString.1.20."+iCase ) ;
       }

    
     apInitScenario( "Scenario 2: toLocaleString on negative whole numbers") ;

     numObj1 = -83 ;			   
     numObj2 = -1 ;
     numVar = -812 ;
     Str1 = numObj1.toLocaleString () ;
     Verify ( "-83.00", Str1, "toLocaleString.2.1."+iCase ) ;

     Str1 = numObj2.toLocaleString () ;
     Verify ( "-1.00", Str1, "toLocaleString.2.2."+iCase ) ;

     Str1 = numVar.toLocaleString () ;
     Verify ( "-812.00", Str1, "toLocaleString.2.3."+iCase ) ;

     Str1 = (-36).toLocaleString () ;
     Verify ( "-36.00", Str1, "toLocaleString.2.4."+iCase ) ;

     Str1 = (numVar - 50).toLocaleString () ;
     Verify ( "-862.00", Str1, "toLocaleString.2.5."+iCase ) ;

     numObj1 = -819934 ;		   
     numObj2 = -28561 ;
     numVar = -1100 ;
     Str1 = numObj1.toLocaleString () ;
     Verify ( "-819"+sGrp+"934.00", Str1, "toLocaleString.2.6."+iCase ) ;

     Str1 = numObj2.toLocaleString () ;
     Verify ( "-28"+sGrp+"561.00", Str1, "toLocaleString.2.7."+iCase ) ;

     Str1 = numVar.toLocaleString () ;
     Verify ( "-1"+sGrp+"100.00", Str1, "toLocaleString.2.8."+iCase ) ;

     Str1 = (-6219).toLocaleString () ;
     Verify ( "-6"+sGrp+"219.00", Str1, "toLocaleString.2.9."+iCase ) ;

     Str1 = (numObj1 + 25000).toLocaleString () ;
     Verify ( "-794"+sGrp+"934.00", Str1, "toLocaleString.2.10."+iCase ) ;

     numObj1 = -1001000 ;		   
     numObj2 = -201664971 ;
     numVar = -40000 * 20000 * 30000 ;
     Str1 = numObj1.toLocaleString () ;
     Verify ( "-1"+sGrp+"001"+sGrp+"000.00", Str1, "toLocaleString.2.11."+iCase ) ;

     Str1 = numObj2.toLocaleString () ;
     Verify ( "-201"+sGrp+"664"+sGrp+"971.00", Str1, "toLocaleString.2.12."+iCase ) ;

     Str1 = numVar.toLocaleString () ;
     Verify ( "-24"+sGrp+"000"+sGrp+"000"+sGrp+"000"+sGrp+"000.00", Str1, "toLocaleString.2.13."+iCase ) ;

     Str1 = (-52073901433).toLocaleString () ;
     Verify ( "-52"+sGrp+"073"+sGrp+"901"+sGrp+"433.00", Str1, "toLocaleString.2.14."+iCase ) ;

     Str1 = (numObj2 * 15).toLocaleString () ;
     Verify ( "-3"+sGrp+"024"+sGrp+"974"+sGrp+"565.00", Str1, "toLocaleString.2.15."+iCase ) ;

     numObj1 = -9e+145 ;		   
     numObj2 = -60E+36 ;
     numVar = -7E+75 ;
     Str1 = numObj1.toLocaleString () ;
     Str2 = "-90" ;			   
     if ( Str1 != "-9E+145" && Str1 != "-9e+145" )
       {
	 for ( iTmp = 0 ; iTmp < 48 ; iTmp++ )
	   Str2 = Str2 + sGrp + "000" ;
	 Str2 = Str2 + ".00" ;
	 if ( Str1 != Str2 )
	   Verify ( "-9E+145", Str1, "toLocaleString.2.16."+iCase ) ;
       }

     Str1 = numObj2.toLocaleString () ;
     Str2 = "-60" ;
     if ( Str1 != "-6e+37" && Str1 != "-6E+37" )
       {
	 for ( iTmp = 0 ; iTmp < 12 ; iTmp ++ )
	    Str2 += sGrp+"000" ;
	 Verify ( Str2+".00", Str1, "toLocaleString.2.17."+iCase ) ;
       }

     Str1 = numVar.toLocaleString () ;
     if ( Str1 != "-7E+75" && Str1 != "-7e+75" )
       {
	 Str2 = "-7"
	 for ( iTmp = 0 ; iTmp < 25 ; iTmp ++ )
	    Str2 += sGrp+"000" ;
	 Verify ( Str2+".00", Str1, "toLocaleString.2.18."+iCase ) ;
       }

     Str1 = (-5e+138).toLocaleString () ;
     if ( Str1 != "-5E+138" && Str1 != "-5e+138" )
       {
	 Str2 = "-5" ;			   
	 for ( iTmp = 0 ; iTmp < 46 ; iTmp++ )
	   Str2 = Str2 + sGrp + "000" ;
	 Str2 = Str2 + ".00" ;
	 Verify ( Str2, Str1, "toLocaleString.2.19."+iCase ) ;
       }

     Str1 = (numObj2 * 20).toLocaleString () ;
     if ( Str1 != "-1.2E+39" && Str1 != "-1.2e+39" )
       {
	 Str2 = "-1"+sGrp+"200" ;
	 for ( iTmp = 0 ; iTmp < 12 ; iTmp ++ )
	    Str2 += sGrp+"000" ;
	 Verify ( Str2+".00", Str1, "toLocaleString.2.20."+iCase ) ;
       }

    
     apInitScenario( "Scenario 3: toLocaleString on positive decimal numbers") ;

     numObj1 = 0.13 ;			   
     numObj2 = 211.2566137 ;
     numVar = 39.5121 ;
     Str1 = numObj1.toLocaleString () ;
     Verify ( "0.13", Str1, "toLocaleString.3.1."+iCase ) ;

     Str1 = numObj2.toLocaleString () ;
     Verify ( "211.26", Str1, "toLocaleString.3.2."+iCase ) ;

     Str1 = numVar.toLocaleString () ;
     Verify ( "39.51", Str1, "toLocaleString.3.3."+iCase ) ;

     Str1 = (81.1).toLocaleString () ;
     Verify ( "81.10", Str1, "toLocaleString.3.4."+iCase ) ;

     Str1 = (numObj1 + 416).toLocaleString () ;
     Verify ( "416.13", Str1, "toLocaleString.3.5."+iCase ) ;

     numObj1 = 1993.4 ; 		   
     numObj2 = 47984.16644 ;
     numVar = 316647.834 ;
     Str1 = numObj1.toLocaleString () ;
     Verify ( "1"+sGrp+"993.40", Str1, "toLocaleString.3.6."+iCase ) ;

     Str1 = numObj2.toLocaleString () ;
     Verify ( "47"+sGrp+"984.17", Str1, "toLocaleString.3.7."+iCase ) ;

     Str1 = numVar.toLocaleString () ;
     Verify ( "316"+sGrp+"647.83", Str1, "toLocaleString.3.8."+iCase ) ;

     Str1 = (66471.31).toLocaleString () ;
     Verify ( "66"+sGrp+"471.31", Str1, "toLocaleString.3.9."+iCase ) ;

     Str1 = (numVar - 2503.734).toLocaleString () ;
     Verify ( "314"+sGrp+"144.10", Str1, "toLocaleString.3.10."+iCase ) ;

     numObj1 = 9110441.8644137 ;	   
     numObj2 = 7860155312.1 ;
     numVar = 41358612.137 ;
     Str1 = numObj1.toLocaleString () ;
     Verify ( "9"+sGrp+"110"+sGrp+"441.86", Str1, "toLocaleString.3.11."+iCase ) ;

     Str1 = numObj2.toLocaleString () ;
     Verify ( "7"+sGrp+"860"+sGrp+"155"+sGrp+"312.10", Str1, "toLocaleString.3.12."+iCase ) ;

     Str1 = numVar.toLocaleString () ;
     Verify ( "41"+sGrp+"358"+sGrp+"612.14", Str1, "toLocaleString.3.13."+iCase ) ;

     Str1 = (62464007831.3191).toLocaleString () ;
     Verify ( "62"+sGrp+"464"+sGrp+"007"+sGrp+"831.32", Str1, "toLocaleString.3.14."+iCase ) ;

     Str1 = (153.14*153.14*153.14*153.14*153.14*153.14).toLocaleString () ;
     if ( Str1 != "12"+sGrp+"898"+sGrp+"281"+sGrp+"664"+sGrp+"351.61" )
       Verify ( "12"+sGrp+"898"+sGrp+"281"+sGrp+"664"+sGrp+"351.60", Str1, "toLocaleString.3.15."+iCase ) ;

     numObj1 = 1.98e+102 ;		     
     numObj2 = 5.3E+113 ;
     numVar = 8.661E+125 ;
     Str1 = numObj1.toLocaleString () ;
     if ( Str1 != "1.98E+102" && Str1 != "1.98e+102" )
       {
	 Str2 = "1" + sGrp + "980" ;	     
	 for ( iTmp = 0 ; iTmp < 33 ; iTmp++ )
	   Str2 = Str2 + sGrp + "000" ;
	 Str2 = Str2 + ".00" ;
	 Verify ( Str2, Str1, "toLocaleString.3.16."+iCase ) ;
       }

     Str1 = numObj2.toLocaleString () ;
     if ( Str1 != "5.3E+113" && Str1 != "5.3e+113" )
       {
	 Str2 = "530" ;			     
	 for ( iTmp = 0 ; iTmp < 37 ; iTmp++ )
	   Str2 = Str2 + sGrp + "000" ;
	 Str2 = Str2 + ".00" ;
	 Verify ( Str2, Str1, "toLocaleString.3.17."+iCase ) ;
       }

     Str1 = numVar.toLocaleString () ;
     if ( Str1 != "8.661E+125" && Str1 != "8.661e+125" )
       {
	 Str2 = "866" + sGrp + "100" ;	     
	 for ( iTmp = 0 ; iTmp < 40 ; iTmp++ )
	   Str2 = Str2 + sGrp + "000" ;
	 Str2 = Str2 + ".00" ;
	 Verify ( Str2, Str1, "toLocaleString.3.18."+iCase ) ;
       }

     Str1 = (4.0147e+131).toLocaleString () ;
     if ( Str1 != "4.0147E+131" && Str1 != "4.0147e+131" )
       {
	 Str2 = "401" + sGrp + "470" ;	     
	 for ( iTmp = 0 ; iTmp < 42 ; iTmp++ )
	   Str2 = Str2 + sGrp + "000" ;
	 Str2 = Str2 + ".00" ;
	 Verify ( Str2, Str1, "toLocaleString.3.19."+iCase ) ;
       }

     Str1 = (2.655319E+148 * 10).toLocaleString () ;
if (SV < 5.6) {
     Str2 = "265" + sGrp + "531" + sGrp + "900" + sGrp + "000" + sGrp + "000" + sGrp + "000" ;    
}
else {
     Str2 = "265" + sGrp + "531" + sGrp + "900" + sGrp + "000" + sGrp + "000" + sGrp + "020" ;    
}
     if ( Str1 != "2.655319E+149" && Str1 != "2.655319e+149" )
       {
	 for ( iTmp = 0 ; iTmp < 44 ; iTmp++ )
	   Str2 = Str2 + sGrp + "000" ;
	 Str2 = Str2 + ".00" ;
	 if ( Str1 != Str2 )
	 Verify ( Str2, Str1, "toLocaleString.3.20."+iCase ) ;
       }

    
     apInitScenario( "Scenario 4: toLocaleString on negative decimal numbers") ;

     numObj1 = -6.1377 ;		    
     numObj2 = -73.16652211 ;
     numVar = -121.593 ;
     Str1 = numObj1.toLocaleString () ;
     Verify ( "-6.14", Str1, "toLocaleString.4.1."+iCase ) ;

     Str1 = numObj2.toLocaleString () ;
     Verify ( "-73.17", Str1, "toLocaleString.4.2."+iCase ) ;

     Str1 = numVar.toLocaleString () ;
     Verify ( "-121.59", Str1, "toLocaleString.4.3."+iCase ) ;

     Str1 = (-0.0093).toLocaleString () ;
     Verify ( "-0.01", Str1, "toLocaleString.4.4."+iCase ) ;

     Str1 = (numVar + 63.1724).toLocaleString () ;
     Verify ( "-58.42", Str1, "toLocaleString.4.5."+iCase ) ;

     numObj1 = -44991.7 ;		    
     numObj2 = -477148.0992 ;
     numVar = -8422.6 ;
     Str1 = numObj1.toLocaleString () ;
     Verify ( "-44"+sGrp+"991.70", Str1, "toLocaleString.4.6."+iCase ) ;

     Str1 = numObj2.toLocaleString () ;
     Verify ( "-477"+sGrp+"148.10", Str1, "toLocaleString.4.7."+iCase ) ;

     Str1 = numVar.toLocaleString () ;
     Verify ( "-8"+sGrp+"422.60", Str1, "toLocaleString.4.8."+iCase ) ;

     Str1 = (-131746.22593).toLocaleString () ;
     Verify ( "-131"+sGrp+"746.23", Str1, "toLocaleString.4.9."+iCase ) ;

     Str1 = (numVar + numObj2).toLocaleString () ;
     Verify ( "-485"+sGrp+"570.70", Str1, "toLocaleString.4.10."+iCase ) ;

     numObj1 = -73144681440.11998 ;	    
     numObj2 = -102135510.617 ;
     numVar = -3121685.4 ;
     Str1 = numObj1.toLocaleString () ;
     Verify ( "-73"+sGrp+"144"+sGrp+"681"+sGrp+"440.12", Str1, "toLocaleString.4.11."+iCase ) ;

     Str1 = numObj2.toLocaleString () ;
     Verify ( "-102"+sGrp+"135"+sGrp+"510.62", Str1, "toLocaleString.4.12."+iCase ) ;

     Str1 = numVar.toLocaleString () ;
     Verify ( "-3"+sGrp+"121"+sGrp+"685.40", Str1, "toLocaleString.4.13."+iCase ) ;

     Str1 = (-2910162464.007831).toLocaleString () ;
     Verify ( "-2"+sGrp+"910"+sGrp+"162"+sGrp+"464.01", Str1, "toLocaleString.4.14."+iCase ) ;

     Str1 = (-332261943343.91394 - 72138491340.1348311).toLocaleString () ;
     Verify ( "-404"+sGrp+"400"+sGrp+"434"+sGrp+"684.05", Str1, "toLocaleString.4.15."+iCase ) ;

     numObj1 = -6.17784e+149 ;		    
     numObj2 = -4.326E+137 ;
     numVar = -3.1e+124 ;
     Str1 = numObj1.toLocaleString () ;
     if ( Str1 != "-6.17784E+149" && Str1 != "-6.17784e+149" )
       {
	 Str2 = "-617" + sGrp + "784" ;	    
	 for ( iTmp = 0 ; iTmp < 48 ; iTmp++ )
	   Str2 = Str2 + sGrp + "000" ;
	 Str2 = Str2 + ".00" ;
	 Verify ( Str2, Str1, "toLocaleString.4.16."+iCase ) ;
       }

     Str1 = numObj2.toLocaleString () ;
     if ( Str1 != "-4.326E+137" && Str1 != "-4.326e+137" )
       {
	 Str2 = "-432" + sGrp + "600" ;	    
	 for ( iTmp = 0 ; iTmp < 44 ; iTmp++ )
	   Str2 = Str2 + sGrp + "000" ;
	 Str2 = Str2 + ".00" ;
	 Verify ( Str2, Str1, "toLocaleString.4.17."+iCase ) ;
       }

     Str1 = numVar.toLocaleString () ;
     if ( Str1 != "-3.1E+124" && Str1 != "-3.1e+124" )
       {
	 Str2 = "-31" ;			    
	 for ( iTmp = 0 ; iTmp < 41 ; iTmp++ )
	   Str2 = Str2 + sGrp + "000" ;
	 Str2 = Str2 + ".00" ;
	 Verify ( Str2, Str1, "toLocaleString.4.18."+iCase ) ;
       }

     Str1 = (-9.0073e+118).toLocaleString () ;
     if ( Str1 != "-9.0073E+118" && Str1 != "-9.0073e+118" )
       {
	 Str2 = "-90" + sGrp + "073" ;	    
	 for ( iTmp = 0 ; iTmp < 38 ; iTmp++ )
	   Str2 = Str2 + sGrp + "000" ;
	 Str2 = Str2 + ".00" ;
	 if ( Str1 != Str2 )
	   Verify ( Str2, Str1, "toLocaleString.4.19."+iCase ) ;
       }

     Str1 = (-7.175319E+102 * 100).toLocaleString () ;
     if ( Str1 != "-7.175319E+104" && Str1 != "-7.175319e+104" )
       {
	 Str2 = "-717" + sGrp + "531" + sGrp + "900" ;	 
	 for ( iTmp = 0 ; iTmp < 32 ; iTmp++ )
	   Str2 = Str2 + sGrp + "000" ;
	 Str2 = Str2 + ".00" ;
	 Verify ( Str2, Str1, "toLocaleString.4.20."+iCase ) ;
       }

     return ;
  }

tolst202();
if(lFailCount >= 0) System.Environment.ExitCode = lFailCount;
else System.Environment.ExitCode = 1;

function apInitTest(stTestName) {
    lFailCount = 0;

    apGlobalObj = new Object();
    apGlobalObj.apGetPlatform = function Funca() { return "Rotor" }
    apGlobalObj.LangHost = function Funcb() { return 1033;}
    apGlobalObj.apGetLangExt = function Funcc(num) { return "EN"; }

    apPlatform = apGlobalObj.apGetPlatform();
    var sVer = "1.0";  
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
