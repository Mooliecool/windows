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
	if ( Bug )
	  apLogFailInfo (Desc, Exp, Act,"") ;
	else
	  apLogFailInfo (Desc, Exp, Act,"") ;
  }

function tolst305 ()
  {
     apInitTest( "ToLSt305 ");

     var arrErr  = -2146823257 + "" ;	     
     var dateErr = -2146823282 + "" ;	     
     var numErr  = -2146823287 + "" ;	     

     var Str1 = "" ;
     var Str2 = "" ;
     var Str3 = "" ;

     var rExpObj1 = new RegExp () ;
     var rExpObj2 = new RegExp ("ddd+") ;
     var rExpVar  = "/ddd+/" ; ;

    
     apInitScenario( "Scenario 1: toLocaleString from non-instantiated RegExp object") ;

     Str1 = RegExp.toString () ;
     Str2 = RegExp.toLocaleString () ;	     
     Verify ( Str1, Str2, "toLocaleString.1.1" ) ;

    
     apInitScenario( "Scenario 2: toLocaleString from instantiated RegExp object - explicit");

     Str1 = rExpObj1.toString () ;
     Str2 = rExpObj1.toLocaleString () ;
     Verify ( Str1, Str2, "toLocaleString.2.1" ) ;

     Str1 = rExpObj2.toString () ;
     Str2 = rExpObj2.toLocaleString () ;
     Verify ( Str1, Str2, "toLocaleString.2.2" ) ;

    
     apInitScenario( "Scenario 3: toLocaleString from instantiated RegExp object - implicit");

     Str1 = rExpVar.toString () ;
     Str2 = rExpVar.toLocaleString () ;
     Verify ( Str1, Str2, "toLocaleString.3.1" ) ;

     Str1 = "/ddd+/".toString () ;
     Str2 = "/ddd+/".toLocaleString () ;
     Verify ( Str1, Str2, "toLocaleString.3.2" ) ;

    
     apInitScenario( "Scenario 4: toLocaleString based on Array.prototype.toLocaleString");

@if (!@_fast)
     Str1 = "" ;			     
     RegExp.toLocaleString = Array.prototype.toLocaleString ;
     try
       {
	 Str1 = RegExp.toLocaleString () ;
       }
     catch ( err )
       {
	  Str1 = Str1 + err.number.toString(10) ;
       }
     Verify ( arrErr, Str1, "toLocaleString.4.1" ) ;
@end

     Str1 = "" ;			     
     rExpObj1.toLocaleString = Array.prototype.toLocaleString ;
     try
       {
	 Str1 = rExpObj1.toLocaleString () ;
       }
     catch ( err )
       {
	  Str1 = Str1 + err.number.toString(10) ;
       }
     Verify ( arrErr, Str1, "toLocaleString.4.2" ) ;

     Str1 = "" ;			     
     rExpObj2.toLocaleString = Array.prototype.toLocaleString ;
     try
       {
	 Str1 = rExpObj2.toLocaleString () ;
       }
     catch ( err )
       {
	  Str1 = Str1 + err.number.toString(10) ;
       }
     Verify ( arrErr, Str1, "toLocaleString.4.3" ) ;

@if (!@_fast)
     Str1 = "" ;			     
     rExpVar.toLocaleString = Array.prototype.toLocaleString ;
     try				     
       {				     
	 Str1 = rExpVar.toLocaleString () ;  
       }				     
     catch ( err )			     
       {				     
	  Str1 = Str1 + err.number.toString(10) ;
       }
     Verify ( rExpVar.toString(), Str1, "toLocaleString.4.4" ) ;
@end

    
     apInitScenario( "Scenario 5: toLocaleString based on Date.prototype.toLocaleString");

@if (!@_fast)
     Str1 = "" ;			     
     RegExp.toLocaleString = Date.prototype.toLocaleString ;
     try
       {
	 Str1 = RegExp.toLocaleString () ;
       }
     catch ( err )
       {
	  Str1 = Str1 + err.number.toString(10) ;
       }
     Verify ( dateErr, Str1, "toLocaleString.5.1" ) ;
@end

     Str1 = "" ;			     
     rExpObj1.toLocaleString = Date.prototype.toLocaleString ;
     try
       {
	 Str1 = rExpObj1.toLocaleString () ;
       }
     catch ( err )
       {
	  Str1 = Str1 + err.number.toString(10) ;
       }
     Verify ( dateErr, Str1, "toLocaleString.5.2" ) ;

     Str1 = "" ;			     
     rExpObj2.toLocaleString = Date.prototype.toLocaleString ;
     try
       {
	 Str1 = rExpObj2.toLocaleString () ;
       }
     catch ( err )
       {
	  Str1 = Str1 + err.number.toString(10) ;
       }
     Verify ( dateErr, Str1, "toLocaleString.5.3" ) ;

@if (!@_fast)
     Str1 = "" ;			     
     rExpVar.toLocaleString = Date.prototype.toLocaleString ;
     try
       {
	 Str1 = rExpVar.toLocaleString () ;
       }
     catch ( err )
       {
	  Str1 = Str1 + err.number.toString(10) ;
       }
     Verify ( rExpVar.toString(), Str1, "toLocaleString.5.4" ) ;
@end

    
     apInitScenario( "Scenario 6: toLocaleString based on Number.prototype.toLocaleString");

@if (!@_fast)
     Str1 = "" ;			     
     RegExp.toLocaleString = Number.prototype.toLocaleString ;
     try
       {
	 Str1 = RegExp.toLocaleString () ;
       }
     catch ( err )
       {
	  Str1 = Str1 + err.number.toString(10) ;
       }
     Verify ( numErr, Str1, "toLocaleString.6.1" ) ;
@end

     Str1 = "" ;			     
     rExpObj1.toLocaleString = Number.prototype.toLocaleString ;
     try
       {
	 Str1 = rExpObj1.toLocaleString () ;
       }
     catch ( err )
       {
	  Str1 = Str1 + err.number.toString(10) ;
       }
     Verify ( numErr, Str1, "toLocaleString.6.2" ) ;

     Str1 = "" ;			     
     rExpObj2.toLocaleString = Number.prototype.toLocaleString ;
     try
       {
	 Str1 = rExpObj2.toLocaleString () ;
       }
     catch ( err )
       {
	  Str1 = Str1 + err.number.toString(10) ;
       }
     Verify ( numErr, Str1, "toLocaleString.6.3" ) ;

@if (!@_fast)
     Str1 = "" ;			     
     rExpVar.toLocaleString = Number.prototype.toLocaleString ;
     try
       {
	 Str1 = rExpVar.toLocaleString () ;
       }
     catch ( err )
       {
	  Str1 = Str1 + err.number.toString(10) ;
       }
     Verify ( rExpVar.toString(), Str1, "toLocaleString.6.4" ) ;
@end

    
     apInitScenario( "Scenario 7: toLocaleString based on Object.prototype.toLocaleString");

@if (!@_fast)
     Str1 = "" ;			     
     RegExp.toLocaleString = Object.prototype.toLocaleString ;
     try
       {
	 Str1 = RegExp.toLocaleString () ;
       }
     catch ( err )
       {
	  Str1 = Str1 + err.Object.toString(10) ;
       }				     
     Verify ( RegExp.toString(), Str1, "toLocaleString.7.1" ) ;
@end

     Str1 = "" ;			     
     rExpObj1.toLocaleString = Object.prototype.toLocaleString ;
     try
       {
	 Str1 = rExpObj1.toLocaleString () ;
       }
     catch ( err )
       {
	  Str1 = Str1 + err.Object.toString(10) ;
       }
     Verify ( rExpObj1.toString(), Str1, "toLocaleString.7.2" ) ;

     Str1 = "" ;			     
     rExpObj2.toLocaleString = Object.prototype.toLocaleString ;
     try
       {
	 Str1 = rExpObj2.toLocaleString () ;
       }
     catch ( err )
       {
	  Str1 = Str1 + err.Object.toString(10) ;
       }
     Verify ( rExpObj2.toString(), Str1, "toLocaleString.7.3" ) ;

@if (!@_fast)
     Str1 = "" ;			     
     rExpVar.toLocaleString = Object.prototype.toLocaleString ;
     try
       {
	 Str1 = rExpVar.toLocaleString () ;
       }
     catch ( err )
       {
	  Str1 = Str1 + err.Object.toString(10) ;
       }
     Verify ( rExpVar.toString(), Str1, "toLocaleString.7.4" ) ;
@end

    
     apInitScenario( "Scenario 8: User-defined toString/toLocaleString");

     Str2 = "fromToString" ;		     
     rExpObj1.toString = function () { return Str2 ; } ;

     Str1 = rExpObj1.toLocaleString () ;      
     Verify ( Str2, Str1, "toLocaleString.8.1.1" ) ;  

     Str3 = "fromToLocaleString" ;	     
     rExpObj1.toLocaleString = function () { return Str3 ; } ;

     Str1 = rExpObj1.toLocaleString () ;
     Verify ( Str3, Str1, "toLocaleString.8.1.2" ) ;

     Str1 = rExpObj1.toString () ;	      
     Verify ( Str2, Str1, "toString.8.1.2" ) ;	

     Str2 = "boolTrue"			     
     rExpObj2.toString = function () { return Str2 ; } ;

     Str1 = rExpObj2.toLocaleString () ;      
     Verify ( Str2, Str1, "toLocaleString.8.2.1" ) ;  

     Str3 = "True is the Value" ;	     
     rExpObj2.toLocaleString = function () { return Str3 ; } ;

     Str1 = rExpObj2.toLocaleString () ;
     Verify ( Str3, Str1, "toLocaleString.8.2.2" ) ;

     Str1 = rExpObj2.toString () ;	      
     Verify ( Str2, Str1, "toString.8.2.2" ) ;	

@if (!@_fast)
     Str2 = "/(d)/" ;			     
     rExpVar.toString = function () { return Str2 ; } ;

     Str1 = rExpVar.toLocaleString () ;      
     Verify ( "/ddd+/", Str1, "toLocaleString.8.4.1" ) ;  
					     
     Str3 = "/(*)/" ;			     
     rExpVar.toLocaleString = function () { return Str3 ; } ;

     Str1 = rExpVar.toLocaleString () ;
     Verify ( "/ddd+/", Str1, "toLocaleString.8.4.2" ) ;
@end
     apEndTest();
  }

tolst305();
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
