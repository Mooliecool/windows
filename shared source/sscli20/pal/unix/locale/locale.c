/*++


 Copyright (c) 2006 Microsoft Corporation.  All rights reserved.

 The use and distribution terms for this software are contained in the file
 named license.txt, which can be found in the root of this distribution.
 By using this software in any fashion, you are agreeing to be bound by the
 terms of this license.

 You must not remove this notice, or any other, from this software.


Module Name:

    locale.c

Abstract:

    Implementation of locale API functions requested by the PAL.

Revision History:

--*/

#include "pal/palinternal.h"
#include "pal/dbgmsg.h"
#include "pal/locale.h"
#include <locale.h>
#include <time.h>

SET_DEFAULT_DEBUG_CHANNEL(LOCALE);

LCID GetLCIDFromUnixLocaleName(const LPSTR lpUnixLocaleName);
LPSTR GetUnixLocaleNameFromLCID(const LCID localeID);

/*
 * This structure is used to build a mapping between UNIX-style locales and
 * Windows style language IDs.  In this structure, the 'locale' variable is
 * the UNIX-like locale name.  The langID variable is the Windows language ID.
 */
struct LANGID_map_entry
{
  CHAR *locale;
  LANGID langID;
};

const static struct LANGID_map_entry LANGID_map[]=
{
  {"", 0x0000}, /* Language Neutral */
  {"", 0x007f}, /* invariant locale */
  {"", 0x0400}, /* Process or User Default Language */
  {"", 0x0800}, /* System Default Language */
  {"", 0x0436}, /* Afrikaans */
  {"", 0x041c}, /* Albanian */
  {"", 0x0401}, /* Arabic (Saudi Arabia) */
  {"", 0x0801}, /* Arabic (Iraq) */
  {"", 0x0c01}, /* Arabic (Egypt) */
  {"", 0x1001}, /* Arabic (Libya) */
  {"", 0x1401}, /* Arabic (Algeria) */
  {"", 0x1801}, /* Arabic (Morocco) */
  {"", 0x1c01}, /* Arabic (Tunisia) */
  {"", 0x2001}, /* Arabic (Oman) */
  {"", 0x2401}, /* Arabic (Yemen) */
  {"", 0x2801}, /* Arabic (Syria) */
  {"", 0x2c01}, /* Arabic (Jordan) */
  {"", 0x3001}, /* Arabic (Lebanon) */
  {"", 0x3401}, /* Arabic (Kuwait) */
  {"", 0x3801}, /* Arabic (U.A.E.) */
  {"", 0x3c01}, /* Arabic (Bahrain) */
  {"", 0x4001}, /* Arabic (Qatar) */
  {"", 0x042b}, /* Windows 2000: Armenian. This is Unicode only. */
  {"", 0x044d}, /* Windows 2000: Assamese. This is Unicode only. */
  {"", 0x042c}, /* Azeri (Latin) */
  {"", 0x082c}, /* Azeri (Cyrillic) */
  {"", 0x042d}, /* Basque */
  {"", 0x0423}, /* Belarussian */
  {"", 0x0445}, /* Windows 2000: Bengali. This is Unicode only. */
  {"", 0x0402}, /* Bulgarian */
  {"", 0x0455}, /* Burmese */
  {"", 0x0403}, /* Catalan */
  {"", 0x0404}, /* Chinese (Taiwan) */
  {"", 0x0804}, /* Chinese (PRC) */
  {"", 0x0c04}, /* Chinese (Hong Kong SAR, PRC) */
  {"", 0x1004}, /* Chinese (Singapore) */
  {"", 0x1404}, /* Chinese (Macau SAR) */
  {"", 0x041a}, /* Croatian */
  {"", 0x0405}, /* Czech */
  {"", 0x0406}, /* Danish */
  {"", 0x0413}, /* Dutch (Netherlands) */
  {"", 0x0813}, /* Dutch (Belgium) */
  {ISO_NAME("en_US", "8859", "1"), 0x0409}, /* English (United States) */
  {"en_UK", 0x0809}, /* English (United Kingdom) */
  {"", 0x0c09}, /* English (Australian) */
  {"en_CA", 0x1009}, /* English (Canadian) */
  {"", 0x1409}, /* English (New Zealand) */
  {"", 0x1809}, /* English (Ireland) */
  {"", 0x1c09}, /* English (South Africa) */
  {"", 0x2009}, /* English (Jamaica) */
  {"", 0x2409}, /* English (Caribbean) */
  {"", 0x2809}, /* English (Belize) */
  {"", 0x2c09}, /* English (Trinidad) */
  {"", 0x3009}, /* English (Zimbabwe) */
  {"", 0x3409}, /* English (Philippines) */
  {"", 0x0425}, /* Estonian */
  {"", 0x0438}, /* Faeroese */
  {"", 0x0429}, /* Farsi */
  {"", 0x040b}, /* Finnish */
  {"fr_FR", 0x040c}, /* French (Standard) */
  {"", 0x080c}, /* French (Belgian) */
  {"fr_CA", 0x0c0c}, /* French (Canadian) */
  {"", 0x100c}, /* French (Switzerland) */
  {"", 0x140c}, /* French (Luxembourg) */
  {"", 0x180c}, /* French (Monaco) */
  {"", 0x0437}, /* Windows 2000: Georgian. This is Unicode only. */
  {"", 0x0407}, /* German (Standard) */
  {"", 0x0807}, /* German (Switzerland) */
  {"", 0x0c07}, /* German (Austria) */
  {"", 0x1007}, /* German (Luxembourg) */
  {"", 0x1407}, /* German (Liechtenstein) */
  {"", 0x0408}, /* Greek */
  {"", 0x0447}, /* Windows 2000: Gujarati. This is Unicode only. */
  {"", 0x040d}, /* Hebrew */
  {"", 0x0439}, /* Windows 2000: Hindi. This is Unicode only. */
  {"", 0x040e}, /* Hungarian */
  {"", 0x040f}, /* Icelandic */
  {"", 0x0421}, /* Indonesian */
  {"", 0x0410}, /* Italian (Standard) */
  {"", 0x0810}, /* Italian (Switzerland) */
  {"", 0x0411}, /* Japanese */
  {"", 0x044b}, /* Windows 2000: Kannada. This is Unicode only. */
  {"", 0x0860}, /* Kashmiri (India) */
  {"", 0x043f}, /* Kazakh */
  {"", 0x0457}, /* Windows 2000: Konkani. This is Unicode only. */
  {"", 0x0412}, /* Korean */
  {"", 0x0812}, /* Korean (Johab) */
  {"", 0x0426}, /* Latvian */
  {"", 0x0427}, /* Lithuanian */
  {"", 0x0827}, /* Lithuanian (Classic) */
  {"", 0x042f}, /* Macedonian */
  {"", 0x043e}, /* Malay (Malaysian) */
  {"", 0x083e}, /* Malay (Brunel Durassalam) */
  {"", 0x044c}, /* Windows 2000: Malayalam. This is Unicode only. */
  {"", 0x0458}, /* Manipuri */
  {"", 0x044e}, /* Windows 2000: Marathi. This is Unicode only. */
  {"", 0x0861}, /* Windows 2000: Nepali (India). This is Unicode only. */
  {"", 0x0414}, /* Norwegian (Bokmal) */
  {"", 0x0814}, /* Norwegian (Nynorsk) */
  {"", 0x0448}, /* Windows 2000: Oriya. This is Unicode only. */
  {"", 0x0415}, /* Polish */
  {"", 0x0416}, /* Portuguese (Brazil) */
  {"", 0x0816}, /* Portuguese (Standard) */
  {"", 0x0446}, /* Windows 2000: Punjabi. This is Unicode only. */
  {"", 0x0418}, /* Romanian */
  {"", 0x0419}, /* Russian */
  {"", 0x044f}, /* Windows 2000: Sanskrit. This is Unicode only. */
  {"", 0x0c1a}, /* Serbian (Cyrillic) */
  {"", 0x081a}, /* Serbian (Latin) */
  {"", 0x0459}, /* Sindhi */
  {"", 0x041b}, /* Slovak */
  {"", 0x0424}, /* Slovenian */
  {"", 0x040a}, /* Spanish (Traditional Sort) */
  {"", 0x080a}, /* Spanish (Mexican) */
  {"", 0x0c0a}, /* Spanish (Modern Sort) */
  {"", 0x100a}, /* Spanish (Guatemala) */
  {"", 0x140a}, /* Spanish (Costa Rica) */
  {"", 0x180a}, /* Spanish (Panama) */
  {"", 0x1c0a}, /* Spanish (Dominican Republic) */
  {"", 0x200a}, /* Spanish (Venezuela) */
  {"", 0x240a}, /* Spanish (Colombia) */
  {"", 0x280a}, /* Spanish (Peru) */
  {"", 0x2c0a}, /* Spanish (Argentina) */
  {"", 0x300a}, /* Spanish (Ecuador) */
  {"", 0x340a}, /* Spanish (Chile) */
  {"", 0x380a}, /* Spanish (Uruguay) */
  {"", 0x3c0a}, /* Spanish (Paraguay) */
  {"", 0x400a}, /* Spanish (Bolivia) */
  {"", 0x440a}, /* Spanish (El Salvador) */
  {"", 0x480a}, /* Spanish (Honduras) */
  {"", 0x4c0a}, /* Spanish (Nicaragua) */
  {"", 0x500a}, /* Spanish (Puerto Rico) */
  {"", 0x0430}, /* Sutu */
  {"", 0x0441}, /* Swahili (Kenya) */
  {"", 0x041d}, /* Swedish */
  {"", 0x081d}, /* Swedish (Finland) */
  {"", 0x0449}, /* Windows 2000: Tamil. This is Unicode only. */
  {"", 0x0444}, /* Tatar (Tatarstan) */
  {"", 0x044a}, /* Windows 2000: Telugu. This is Unicode only. */
  {"", 0x041e}, /* Thai */
  {"", 0x041f}, /* Turkish */
  {"", 0x0422}, /* Ukrainian */
  {"", 0x0420}, /* Urdu (Pakistan) */
  {"", 0x0820}, /* Urdu (India) */
  {"", 0x0443}, /* Uzbek (Latin) */
  {"", 0x0843}, /* Uzbek (Cyrillic) */
  {"", 0x042a}, /* Vietnamese */
};

/*++
Function:
  PAL_setlocale

See MSDN doc.

--*/
char * __cdecl PAL_setlocale(int category, const char * locale)
{
    char * pret = NULL;
    int native_category = 0;

    PERF_ENTRY(setlocale);
    ENTRY("PAL_setlocale(category=%d, locale=%s)\n",
          category, locale ? locale : "(NULL)");

    switch (category)
    {
    case PAL_LC_ALL:
        native_category = LC_ALL;
        break;
    case PAL_LC_COLLATE:
        native_category = LC_COLLATE;
        break;
    case PAL_LC_CTYPE:
        native_category = LC_CTYPE;
        break;
    case PAL_LC_MONETARY:
        native_category = LC_MONETARY;
        break;
    case PAL_LC_NUMERIC:
        native_category = LC_NUMERIC;
        break;
    case PAL_LC_TIME:
        native_category = LC_TIME;
        break;
    default:
        ERROR( "Unknown category %d\n", category );     
        goto EXIT;
        break;
    }

#if !HAVE_CFSTRING
    if (NULL == locale)
    {
        /* Need the read lock */
        if (!CODEPAGEAcquireReadLock())
        {
            /* Could not get the readlock */  
            SetLastError(ERROR_INTERNAL_ERROR);
            goto EXIT;
        }
    }
    else
    {
        /* Need a write lock. */
        if ( !CODEPAGEAcquireWriteLock() )
        {
            /* Could not get a write lock */  
            SetLastError(ERROR_INTERNAL_ERROR);
            goto EXIT;
        }    
    }
#endif // !HAVE_CFSTRING

    pret = setlocale(native_category, locale);

#if !HAVE_CFSTRING
    if( !CODEPAGEReleaseLock() )
    {
        ERROR( "Unable to release the readwrite lock\n" );
    }
#endif // !HAVE_CFSTRING

EXIT:
    LOGEXIT("PAL_setlocale returns: %s\n", pret ? pret : "NULL");
    PERF_EXIT(setlocale);
    return pret;
}

/*++
Function:
GetLCIDFromUnixLocaleName

Return:
Non-zero value on success, else returns 0 and sets the error to
ERROR_INVALID_PARAMETER.

--*/
LCID GetLCIDFromUnixLocaleName(const LPSTR lpUnixLocaleName)
{
  struct LANGID_map_entry map_entry;
  INT nNumOfLangids = sizeof(LANGID_map)/sizeof(map_entry);
  LCID localeID = (LCID)0;
  INT i;

  /*
   * Iterate through the map entries to find the LCID for the given locale
   * name.
   */
  for(i = 0; i < nNumOfLangids; i++)
    {
      if(strcmp(LANGID_map[i].locale, lpUnixLocaleName) == 0)
    {
      localeID = MAKELCID(LANGID_map[i].langID, SORT_DEFAULT);
      break;
    }
    }

  if(i == nNumOfLangids)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
    }

  /*hardcode the locale to US_ENGLISH for user default locale*/
  if(!strcmp(lpUnixLocaleName,"C"))
      localeID  = 0x0409;
  return localeID;
}


/*++
Function:
GetUnixLocaleNameFromLCID

Return:
Non-null character pointer to the locale name.  If the argument is invalid,
then a null pointer is returned and the error is set to
ERROR_INVALID_PARAMETER.
--*/
LPSTR GetUnixLocaleNameFromLCID(const LCID localeID)
{
  LPSTR lpLocaleName = NULL;
  struct LANGID_map_entry map_entry;
  INT nNumOfLangids = sizeof(LANGID_map)/sizeof(map_entry);
  LANGID langID = LANGIDFROMLCID(localeID);
  INT i;


  /*
   * Iterate through the map entries to find the UNIX-like locale name
   * for the given locale id.
   */
  for(i = 0; i < nNumOfLangids; i++)
  {
      if(LANGID_map[i].langID == langID)
     {
         lpLocaleName = LANGID_map[i].locale;
         break;
      }
  }

  /*hardcode the locale to US_ENGLISH for user default*/
  if(localeID == LOCALE_USER_DEFAULT)
  {
      lpLocaleName = ISO_NAME("en_US", "8859", "1");
  }

  if(lpLocaleName == NULL)
  {
      SetLastError(ERROR_INVALID_PARAMETER);
  }
  return lpLocaleName;
}

/*++
Function:
  GetSystemDefaultLangID

See MSDN doc.
--*/
LANGID
PALAPI
GetSystemDefaultLangID(
               void)
{
    LANGID langID = (LANGID)0; /* return value */
    LPSTR lpCurrentLocale;
    struct LANGID_map_entry map_entry;
    INT i;
    INT nNumOfLangids = sizeof(LANGID_map)/sizeof(map_entry);

    PERF_ENTRY(GetSystemDefaultLangID);
    ENTRY("GetSystemDefaultLangID()\n");

#if !HAVE_CFSTRING
      if ( !CODEPAGEAcquireReadLock() )
      {
          /* Could not get a read lock */  
          SetLastError(ERROR_INTERNAL_ERROR);
          goto EXIT;
      }    
#endif // !HAVE_CFSTRING

    /* get current locale */
    if((lpCurrentLocale = setlocale(LC_CTYPE,0)) == NULL)
    {
       ASSERT("setlocale failed to give currentlocale\n");
       SetLastError(ERROR_INTERNAL_ERROR);
       goto ReleaseLock;
    }

    /*
     * Iterate through the map entries to find the LCID for the given locale
     * name.
     */
    for(i = 0; i < nNumOfLangids; i++)
    {
      if(strcmp(LANGID_map[i].locale,lpCurrentLocale ) == 0)
      {
        langID = LANGID_map[i].langID;
        break;
      }
    }

    if(i == nNumOfLangids)
    {
      WARN("Cannot find langid in the map entries,"
            "hardcoding to US_ENGLISH");
      /* hardcode  system default langid to US_ENGLISH */
      langID = 0x0409;
    }

ReleaseLock:
#if !HAVE_CFSTRING
      if( !CODEPAGEReleaseLock() )
      {
          ERROR( "Unable to release the readwrite lock\n" );
      }

EXIT:	  
#endif // !HAVE_CFSTRING

    LOGEXIT("GetSystemDefaultLangID returns LANGID %x\n", langID);
    PERF_EXIT(GetSystemDefaultLangID);
    return langID;
}



/*++
Function:
  GetUserDefaultLangID

See MSDN doc.
--*/
LANGID
PALAPI
GetUserDefaultLangID(
             void)
{
    LANGID langID = (LANGID)0; /* return value */
    PERF_ENTRY(GetUserDefaultLangID);
    ENTRY("GetUserDefaultLangID()\n");
    langID = GetSystemDefaultLangID();
    LOGEXIT("GetUserDefaultLangID returns LANGID %x\n", langID);
    PERF_EXIT(GetUserDefaultLangID);
    return langID;
}



/*++
Function:
  SetThreadLocale

See MSDN doc.
--*/
BOOL
PALAPI
SetThreadLocale(
        IN LCID Locale)
{
  BOOL bReturnValue = FALSE;
  LPSTR lpLocaleName;
  DWORD dMask =  0xF0000 ;

  PERF_ENTRY(SetThreadLocale);
  ENTRY("SetThreadLocale(Locale=%#x)\n", Locale);

  /* check if the sorting bit is set or not*/

  if(Locale & dMask)
  {
      /*The sorting bit is set to a value other than SORT_DEFAULT
       * which we dont support at this stage*/
      ASSERT("Error Locale(%#x) parameter is invalid\n",Locale);
      SetLastError(ERROR_INVALID_PARAMETER);
      goto EXIT;
  }

  lpLocaleName = GetUnixLocaleNameFromLCID(Locale);

  if((lpLocaleName != NULL) && (strcmp(lpLocaleName, "") != 0))
  {
#if !HAVE_CFSTRING
      if ( !CODEPAGEAcquireWriteLock() )
      {
          /* Could not get a write lock */
          SetLastError(ERROR_INTERNAL_ERROR);
          goto EXIT;
      }
#endif // !HAVE_CFSTRING

      if(setlocale(LC_CTYPE,lpLocaleName))
      {
          bReturnValue = TRUE;
      }
      else
      {
          ASSERT("setlocale failed for localename %s\n",lpLocaleName);
          SetLastError(ERROR_INTERNAL_ERROR);
      }

#if !HAVE_CFSTRING
      if( !CODEPAGEReleaseLock() )
      {
          ERROR( "Unable to release the readwrite lock\n" );
      }
#endif // !HAVE_CFSTRING

  }
  else
  {
      SetLastError(ERROR_INVALID_PARAMETER);
  }
EXIT:
  LOGEXIT("SetThreadLocale returning BOOL %d)\n", bReturnValue);
  PERF_EXIT(SetThreadLocale);
  return bReturnValue;
}


/*++
Function:
  GetThreadLocale

See MSDN doc.
--*/
LCID
PALAPI
GetThreadLocale(
        void)
{
  LCID lcID = (LCID)0; /* return value */
  LPSTR lpCurrentLocale;

  PERF_ENTRY(GetThreadLocale);
  ENTRY("GetThreadLocale()\n");

#if !HAVE_CFSTRING
  if ( !CODEPAGEAcquireReadLock() )
  {
      /* Could not get a read lock */
      SetLastError(ERROR_INTERNAL_ERROR);
      goto EXIT;
  }
#endif // !HAVE_CFSTRING

  /* get current locale */
  if((lpCurrentLocale = setlocale(LC_CTYPE,0)) != NULL)
  {
    lcID = GetLCIDFromUnixLocaleName(lpCurrentLocale);
  }
  else
  {
    ASSERT("setlocale failed to give currentlocale\n");
    SetLastError(ERROR_INTERNAL_ERROR);
  }

#if !HAVE_CFSTRING
  if( !CODEPAGEReleaseLock() )
  {
      ERROR( "Unable to release the readwrite lock\n" );
  }

EXIT:
#endif // !HAVE_CFSTRING

  LOGEXIT("GetThreadLocale returning %d\n", lcID);
  PERF_EXIT(GetThreadLocale);
  return lcID;
}


/*++
Function:
  GetUserDefaultLCID

See MSDN doc.
--*/
LCID
PALAPI
GetUserDefaultLCID(
           void)
{
    LPSTR lpCurrentLocale;
    LCID lcID = (LCID)0; /* return value */

    PERF_ENTRY(GetUserDefaultLCID);
    ENTRY("GetUserDefaultLCID()\n");

#if !HAVE_CFSTRING
    if ( !CODEPAGEAcquireReadLock() )
    {
	/* Could not get a read lock */
	SetLastError(ERROR_INTERNAL_ERROR);
	goto EXIT;
    }
#endif // !HAVE_CFSTRING

    /* get current locale */
    if((lpCurrentLocale = setlocale(LC_CTYPE,NULL)) == NULL)
    {
       ASSERT("setlocale failed to give currentlocale\n");
       SetLastError(ERROR_INTERNAL_ERROR);
    }

    lcID = GetLCIDFromUnixLocaleName(lpCurrentLocale);

#if !HAVE_CFSTRING
    if( !CODEPAGEReleaseLock() )
    {
	ERROR( "Unable to release the readwrite lock\n" );
    }
#endif // !HAVE_CFSTRING

    /* harcoded value for lcid in case the app is not internationalized and 
       does not have a valid lcid*/
    if(lcID == 0)
    {
       /* hardcoded value for user default langid taken from the langid map */
        lcID = MAKELCID(0x0400, SORT_DEFAULT);
    }

#if !HAVE_CFSTRING
EXIT:
#endif // !HAVE_CFSTRING
    LOGEXIT("GetUserDefaultLCID returns LCID %x\n", lcID);
    PERF_EXIT(GetUserDefaultLCID);
    return lcID;
}


/*++
Function:
  IsValidLocale

See MSDN doc.
--*/
BOOL
PALAPI
IsValidLocale(
          IN LCID Locale,
          IN DWORD dwFlags)
{
  BOOL bReturnValue = FALSE;
  LPSTR lpBuf = NULL;
  LPSTR lpCurrentLocale = NULL;
  LPSTR lpLocaleName = NULL;
  DWORD dMask =  0xF0000 ;

  PERF_ENTRY(IsValidLocale);
  ENTRY("IsValidLocale(Locale=%#x, dwFlags=%#x)\n", Locale, dwFlags);

   /* check if the sorting bit is set or not*/

  if((Locale & dMask) || (Locale == LOCALE_USER_DEFAULT))
  {
      /*if (Locale & dMask) is true then sorting bit is
       * set to a value other than SORT_DEFAULT which
       * we dont support at this stage*/
      LOGEXIT ("IsValidLocale returns BOOL %d\n",bReturnValue);
      PERF_EXIT(IsValidLocale);
      return bReturnValue;
  }

  /*
   * First, lets do the check to see if the locale is supported.
   */
  if((dwFlags & LCID_SUPPORTED) || (dwFlags & LCID_INSTALLED))
  {
      lpLocaleName = GetUnixLocaleNameFromLCID(Locale);

      if((lpLocaleName != NULL) && (strcmp(lpLocaleName, "") != 0))
      {
          bReturnValue = TRUE;
      }
  }

  /*
   * Now lets check if the locale is installed, if needed.
   */
  if((dwFlags & LCID_INSTALLED) && bReturnValue)
  {
#if !HAVE_CFSTRING
      /* Need a write lock. */
      if ( !CODEPAGEAcquireWriteLock() )
      {
          /* Could not get a write lock */  
          SetLastError(ERROR_INTERNAL_ERROR);
          goto EXIT;
      }    
#endif // !HAVE_CFSTRING

      /*
       * To actually test if a locale is installed, we will check the output
       * of the setlocale() function.
       */
      if((lpCurrentLocale = setlocale(LC_CTYPE, 0)) != NULL)
      {
          lpBuf = strdup(lpCurrentLocale);
          
          if (lpBuf != NULL)
          {
              if(setlocale(LC_CTYPE, lpLocaleName) == NULL)
              {
                  bReturnValue = FALSE;
              }
              else
              {
                  /* reset locale */
                  setlocale(LC_CTYPE, lpBuf);
              }

              free(lpBuf);
          }
          else
          {
              /* Could not allocate memory. */
              bReturnValue = FALSE;
              ERROR("Could not allocate memory for saving current locale.\n");
              SetLastError(ERROR_INTERNAL_ERROR);
          }
      }
      else
      {
          /* setlocale failed... */
          bReturnValue = FALSE;
          ASSERT("setlocale failed\n");
          SetLastError(ERROR_INTERNAL_ERROR);
      }

#if !HAVE_CFSTRING
      if( !CODEPAGEReleaseLock() )
      {
          ERROR( "Unable to release the readwrite lock\n" );
      }
#endif // !HAVE_CFSTRING
  }

#if !HAVE_CFSTRING
EXIT:
#endif // !HAVE_CFSTRING
  LOGEXIT ("IsValidLocale returns BOOL %d\n",bReturnValue);
  PERF_EXIT(IsValidLocale);
  return bReturnValue;
}


/*++
Function:
  GetUserDefaultLCID

See MSDN doc.
--*/
int
PALAPI
GetCalendarInfoW(
         IN LCID Locale,
         IN CALID Calendar,
         IN CALTYPE CalType,
         OUT LPWSTR lpCalData,
         IN int cchData,
         OUT LPDWORD lpValue)
{

   int nRetValue = sizeof(DWORD);
   PERF_ENTRY(GetCalendarInfoW);
   ENTRY("GetCalendarInfoW(Locale=%#x, Calendar=%d, CalType=%d, "
         "lpCalData=%p, cchData=%d, lpValue=%p)\n",
          Locale, Calendar, CalType, lpCalData, cchData, lpValue);

   if((lpValue != NULL) && (Locale == LOCALE_USER_DEFAULT)
       && (lpCalData == NULL) &&(cchData == 0) &&
      (CalType == (CAL_ITWODIGITYEARMAX|CAL_RETURN_NUMBER)))
   {
       switch(Calendar)
       {
           case CAL_GREGORIAN :
                     /* Time and dates in FreeBSD are represented internally as the
                      * number of secondssince the 1st of January 1970 (the Unix
                      * "epoch"). Currently, that figure is stored as a 32 bit
                      * integer, and will run out part way through 2038*/

                        *lpValue = 2038;
                        goto done;

           case CAL_GREGORIAN_US:
                     /* Time and dates in FreeBSD are represented internally as the
                      * number of secondssince the 1st of January 1970 (the Unix
                      * "epoch"). Currently, that figure is stored as a 32 bit
                      * integer, and will run out part way through 2038*/

                        *lpValue = 2038;
                        goto done;

           case CAL_JAPAN:
                        *lpValue = 99;
                        goto done;

           case CAL_TAIWAN:
                        *lpValue = 99;
                        goto done;

           case CAL_KOREA:
                        *lpValue = 4362;
                        goto done;

           case CAL_HIJRI:
                        *lpValue = 1451;
                        goto done;

           case CAL_THAI:
                        *lpValue = 2572;
                        goto done;

           case CAL_HEBREW:
                        *lpValue = 5790;
                        goto done;

           case CAL_GREGORIAN_ME_FRENCH:
                        *lpValue = 2038;
                        goto done;

           case CAL_GREGORIAN_ARABIC:
                        *lpValue = 2038;
                        goto done;

           case CAL_GREGORIAN_XLIT_ENGLISH:
                        *lpValue = 2038;
                        goto done;

           case CAL_GREGORIAN_XLIT_FRENCH:
                        *lpValue = 2038;
                        goto done;

           default:
                    ERROR("Error Calendar(%d) parameter is invalid\n",Calendar);
                    SetLastError(ERROR_INVALID_PARAMETER);
                    nRetValue =0;
                    goto done;
       }
   }
   else
   {
       ASSERT("Error some parameters are invalid\n");
       SetLastError(ERROR_INVALID_PARAMETER);
       nRetValue =0;
   }
done:
   LOGEXIT ("GetCalendarInfoW returns int %d\n",nRetValue);
   PERF_EXIT(GetCalendarInfoW);
   return nRetValue;
}



int
PALAPI
GetDateFormatW(
           IN LCID Locale,
           IN DWORD dwFlags,
           IN CONST SYSTEMTIME *lpDate,
           IN LPCWSTR lpFormat,
           OUT LPWSTR lpDateStr,
           IN int cchDate)
{
    LCID localeID = (LCID)0;
    WCHAR GG_string[] = {'g','g','\0'};

    PERF_ENTRY(GetDateFormatW);
    ENTRY("GetDateFormatW(Locale=%#x, dwFlags=%#x, lpDate=%p, lpFormat=%p (%S), "
          "lpDateStr=%p, cchDate=%d)\n",
          Locale, dwFlags, lpDate, lpFormat ? lpFormat : W16_NULLSTRING, lpFormat ? lpFormat : W16_NULLSTRING,
          lpDateStr, cchDate);

    localeID = MAKELCID(GetSystemDefaultLangID(), SORT_DEFAULT);


    /* The implementation of this function is not reqd as of now
     *as this is called only for CAL_TAIWAN Calendar and we are supporting
     *only CAL_GREGORIAN*/
    if(((Locale == localeID) || (Locale == 0x0404)) && (dwFlags & DATE_USE_ALT_CALENDAR) && (lpDate == NULL)
       && lpFormat ? (PAL_wcsncmp(lpFormat, GG_string, PAL_wcslen(lpFormat)) == 0) : FALSE)
    {
        /*Need to make a call to strftime() with appropriate params when functionality
          to use alternate calendars is implemented */
        ERROR("Not Implemented\n");
    }
    else
    {
        ASSERT("One of the input parameters is invalid\n");
        SetLastError(ERROR_INVALID_PARAMETER);
    }
    LOGEXIT ("GetDateFormatW returns int 0\n");
    PERF_EXIT(GetDateFormatW);

    return 0;
}


/*++
Function:
  CompareStringA

See MSDN doc.
--*/
int
PALAPI
CompareStringA(
    IN LCID     Locale,
    IN DWORD    dwCmpFlags,
    IN LPCSTR   lpString1,
    IN int      cchCount1,
    IN LPCSTR   lpString2,
    IN int      cchCount2)
{
    INT nRetVal =0;  /*return Value*/
    INT nStrLen =0;

    PERF_ENTRY(CompareStringA);
    ENTRY("CompareStringA(Locale=%#x, dwCmpFlags=%#x,lpString1 = %p (%s), "
          "cchCount1 =%d,lpString2 = %p (%s),cchCount2 =%d )\n",
          Locale, dwCmpFlags, lpString1, lpString1, cchCount1,lpString2,lpString2, cchCount2 );

    if ( Locale != 0x0409 )
    {
        ASSERT("Error Locale(%#x) parameter is invalid\n",Locale);
        SetLastError(ERROR_INVALID_PARAMETER);
        LOGEXIT ("CompareStringA returns int 0\n");
        PERF_EXIT(CompareStringA);
        return 0;
    }

    if( (dwCmpFlags & ~NORM_IGNOREWIDTH) != NORM_IGNORECASE)
    {
        ASSERT("Error dwCmpFlags(%#x) parameter is invalid\n",dwCmpFlags);
        SetLastError(ERROR_INVALID_PARAMETER);
        LOGEXIT ("CompareStringA returns int 0\n");
        PERF_EXIT(CompareStringA);
        return 0;
    }

    if ( !lpString1 || !lpString2 )
    {
        ERROR("One of the two params %p and %p is invalid\n",lpString1,lpString2);
        SetLastError( ERROR_INVALID_PARAMETER );
        LOGEXIT ("CompareStringA returns 0\n" );
        PERF_EXIT(CompareStringA);
        return 0;
    }

    if(cchCount1 == 0 && cchCount2 == 0 )
    {
        LOGEXIT ("CompareStringA returns int %d\n", CSTR_EQUAL );
        PERF_EXIT(CompareStringA);
        return CSTR_EQUAL;
    }

    if ( cchCount1 == 0 )
    {
        LOGEXIT ("CompareStringA returns int %d\n", CSTR_LESS_THAN );
        PERF_EXIT(CompareStringA);
        return CSTR_LESS_THAN;
    }
    if ( cchCount2 == 0 )
    {
        LOGEXIT ("CompareStringA returns int %d\n", CSTR_GREATER_THAN );
        PERF_EXIT(CompareStringA);
        return CSTR_GREATER_THAN;
    }

    if( cchCount1 == -1)
    {
        cchCount1 = strlen( lpString1 );
    }
    if( cchCount2 == -1 )
    {
        cchCount2 = strlen( lpString2 );
    }

    /*take the length of the smaller of the 2 strings*/
    nStrLen = ( ( cchCount1 > cchCount2 ) ? cchCount2 : cchCount1 );
    nRetVal = _strnicmp( lpString1, lpString2, nStrLen );

    if(nRetVal ==0)
    {
        if(cchCount1>cchCount2)
        {
            nRetVal = 1;
        }
        else if (cchCount1 < cchCount2)
        {
            nRetVal = -1;
        }
    }

    if ( nRetVal == 0 )
    {
        LOGEXIT ("CompareStringA returns int %d\n", CSTR_EQUAL );        
        PERF_EXIT(CompareStringA);
        return CSTR_EQUAL;
    }
    else if ( nRetVal > 0 )
    {
        LOGEXIT ("CompareStringA returns int %d\n", CSTR_EQUAL );        
        PERF_EXIT(CompareStringA);
        return CSTR_GREATER_THAN;
    }
    else
    {
        LOGEXIT ("CompareStringA returns int %d\n", CSTR_LESS_THAN );
        PERF_EXIT(CompareStringA);
        return CSTR_LESS_THAN;
    }
}


/*++
Function:
  CompareStringW

See MSDN doc.
--*/
int
PALAPI
CompareStringW(
    IN LCID     Locale,
    IN DWORD    dwCmpFlags,
    IN LPCWSTR  lpString1,
    IN int      cchCount1,
    IN LPCWSTR  lpString2,
    IN int      cchCount2)
{
    INT nRetVal =0;  /*return Value*/
    INT nStrLen =0;

    PERF_ENTRY(CompareStringW);
    ENTRY("CompareStringW(Locale=%#x, dwCmpFlags=%#x,lpString1 = %p (%S), "
          "cchCount1 =%d,lpString2 = %p (%S),cchCount2 =%d )\n",
          Locale, dwCmpFlags, lpString1, lpString1, cchCount1,lpString2,lpString2, cchCount2 );

    if ( Locale != 0x0409 )
    {
        ASSERT("Error Locale(%#x) parameter is invalid\n",Locale);
        SetLastError(ERROR_INVALID_PARAMETER);
        LOGEXIT ("CompareStringW returns int 0\n");
        PERF_EXIT(CompareStringW);
        return 0;
    }

    if( (dwCmpFlags & ~NORM_IGNOREWIDTH) != NORM_IGNORECASE)
    {
        ASSERT("Error dwCmpFlags(%#x) parameter is invalid\n",dwCmpFlags);
        SetLastError(ERROR_INVALID_PARAMETER);
        LOGEXIT ("CompareStringW returns int 0\n");
        PERF_EXIT(CompareStringW);
        return 0;
    }

    if ( !lpString1 || !lpString2 )
    {
        ERROR("One of the two params %p and %p is Invalid\n",lpString1,lpString2);
        SetLastError( ERROR_INVALID_PARAMETER );
        LOGEXIT ("CompareStringW returns 0\n" );
        PERF_EXIT(CompareStringW);
        return 0;
    }

    if(cchCount1 == 0 && cchCount2 == 0 )
    {
        LOGEXIT ("CompareStringW returns int %d\n", CSTR_EQUAL );
        PERF_EXIT(CompareStringW);
        return CSTR_EQUAL;
    }

    if ( cchCount1 == 0 )
    {
        LOGEXIT ("CompareStringW returns int %d\n", CSTR_LESS_THAN );
        PERF_EXIT(CompareStringW);
        return CSTR_LESS_THAN;
    }
    if ( cchCount2 == 0 )
    {
        LOGEXIT ("CompareStringW returns int %d\n", CSTR_GREATER_THAN );
        PERF_EXIT(CompareStringW);
        return CSTR_GREATER_THAN;
    }

    if( cchCount1 == -1)
    {
        cchCount1 = PAL_wcslen( lpString1 );
    }
    if( cchCount2 == -1 )
    {
        cchCount2 = PAL_wcslen( lpString2 );
    }

    /*take the length of the smaller of the 2 strings*/
    nStrLen = ( ( cchCount1 > cchCount2 ) ? cchCount2 : cchCount1 );
    nRetVal = _wcsnicmp( lpString1, lpString2, nStrLen );

    if(nRetVal ==0)
    {
        if(cchCount1>cchCount2)
        {
            nRetVal = 1;
        }
        else if (cchCount1 < cchCount2)
        {
            nRetVal = -1;
        }
    }

    if ( nRetVal == 0 )
    {
        LOGEXIT ("CompareStringW returns int %d\n", CSTR_EQUAL );        
        PERF_EXIT(CompareStringW);
        return CSTR_EQUAL;
    }
    else if ( nRetVal > 0 )
    {
        LOGEXIT ("CompareStringW returns int %d\n", CSTR_EQUAL );        
        PERF_EXIT(CompareStringW);
        return CSTR_GREATER_THAN;
    }
    else
    {
        LOGEXIT ("CompareStringW returns int %d\n", CSTR_LESS_THAN );
        PERF_EXIT(CompareStringW);
        return CSTR_LESS_THAN;
    }
}


/*++
Function:
  GetLocaleInfoW

See MSDN doc.
--*/
int
PALAPI
GetLocaleInfoW(
    IN LCID     Locale,
    IN LCTYPE   LCType,
    OUT LPWSTR  lpLCData,
    IN int      cchData)
{
    INT nRetval =0; /*return value*/
    struct lconv * LCConv;
    char * InputStr;

    PERF_ENTRY(GetLocaleInfoW);
    ENTRY("GetLocaleInfoW(Locale=%#x, LCType=%#x,lpLCData = %p,cchData =%d)\n",
          Locale, LCType, lpLCData, cchData);
    
    if(Locale != LOCALE_NEUTRAL && Locale != LOCALE_US_ENGLISH)
    {
        ASSERT("Error Locale(%#x) parameter is invalid\n",Locale);
        SetLastError(ERROR_INVALID_PARAMETER);
        goto EXIT;
    }

    LCConv = localeconv();
    /* Harcoding most of the  values for US_ENGLISH
     *as these values are not defined for FreeBSD 4.5 */
    switch(LCType)
    {
        case LOCALE_SDECIMAL:
            InputStr = LCConv->decimal_point;
            break;
        case LOCALE_STHOUSAND:
            /*InputStr = LCConv->thousands_sep;*/
            InputStr = ",";
            break;
        case LOCALE_ILZERO:
             /*harcoded to return leading zeros
             *in decimal fields*/
            InputStr = "1";
            break;
        case LOCALE_SCURRENCY:
            /*InputStr = LCConv->currency_symbol;*/
            InputStr = "$";
            break;
        case LOCALE_SMONDECIMALSEP:
            /*InputStr = LCConv->mon_decimal_point;*/
            InputStr = ".";
            break;
        case LOCALE_SMONTHOUSANDSEP:
            /*InputStr = LCConv->mon_thousands_sep;*/
            InputStr = ",";
            break;
        default :
            ASSERT("Error LCType(%#x) parameter is invalid\n",LCType);
            SetLastError(ERROR_INVALID_PARAMETER);
            goto EXIT;

    }
     
    /*if output buffer size is zero return the
      *size of buffer required*/
     if(cchData == 0)
     {
         nRetval = MultiByteToWideChar(CP_ACP,0,InputStr,-1,NULL,0);
     }
     else
     {
        nRetval = MultiByteToWideChar(CP_ACP,0,InputStr, -1,lpLCData,cchData);     
     }

     if (!nRetval && (ERROR_INSUFFICIENT_BUFFER != GetLastError()))
     {
         ASSERT("MultiByteToWideChar failed.  Error is %d\n", GetLastError());
     }
EXIT:    
    LOGEXIT ("GetLocaleInfoW returns int %d\n", nRetval);
    PERF_EXIT(GetLocaleInfoW);
    return nRetval;
}
