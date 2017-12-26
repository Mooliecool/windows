//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  STRIDs should be defined in the following ranges:
//
//   1     -  100    - non localized STRID (main common compiler DLLs)
//   101   -  500    - non localized STRID (main specific compiler DLLs)
//   1000  - 1008    - localized STRID (intl common compiler DLLs)
//   1009  - 1022    - localized STRID (intl specific compiler DLLs)
//   1023  - 1030    - localized STRID (intl common compiler DLLs)
//   1031  - 1100    - localized STRID (intl specific compiler DLLs)
//   1101  - 1199    - localized STRID (intl common compiler DLLs)
//   1200  - 29999   - localized STRID (intl specific compiler DLLs)
//
//-------------------------------------------------------------------------------------------------

// Make sure to only use the ranges for the specific DLL's,
// not the ones in common.
#define IDS_HELP_COLUMN_SPACE           101 // Non-localized

#define IDS_FATALERROR                  2001
#define IDS_ERROR                       2002
#define IDS_WARNING                     2003
#define IDS_ERR_INITERROR               2004
#define IDS_ERR_FILENOTFOUND            2005
#define IDS_ERR_NOMEMORY                2006
#define IDS_ERR_NORESPONSEFILE          2007
#define IDS_WRN_BADSWITCH               2009
#define IDS_ERR_NOSOURCES               2011
#define IDS_ERR_ARGUMENTREQUIRED        2013
#define IDS_ERR_DUPLICATERESPONSEFILE   2014
#define IDS_WRN_FILEALREADYINCLUDED     2015
#define IDS_ERR_LIBNOTFOUND             2016
#define IDS_ERR_CANTOPENFILEWRITE       2017
#define IDS_ERR_INVALIDSWITCHVALUE      2019
#define IDS_ERR_BINARYFILE              2020
#define IDS_ERR_BADCODEPAGE             2021
#define IDS_ERR_COMPILEFAILED           2022
#define IDS_ERR_SWITCHNEEDSBOOL         2024
#define IDS_ERR_INTLLIBNOTFOUND         2025
#define IDS_ERR_MAXIMUMERRORS           2026
#define IDS_ERR_ICONFILEANDWIN32RES     2029
#define IDS_WRN_RESERVEDREFERENCE       2030
#define IDS_WRN_NOCONFIGINRESPONSEFILE  2031
#define IDS_WRN_INVALIDWARNINGID        2032
#define IDS_ERR_WATSONSENDNOTOPTEDIN    2033
#define IDS_WRN_SWITCHNOBOOL            2034
#define IDS_ERR_NOSOURCESOUT            2035
#define IDS_ERR_NEEDMODULE              2036
#define IDS_ERR_INVALIDASSEMBLYNAME     2037
#define IDS_ERR_CONFLICTINGMANIFESTSWITCHES 2038
#define IDS_WRN_IGNOREMODULEMANIFEST    2039
#define IDS_ERR_NODEFAULTMANIFEST       2040
#define IDS_ERR_INVALIDSWITCHVALUE1     2041
#define IDS_ERR_VBCORENETMODULECONFLICT 2042

#define IDS_BANNER1                     10001
// Don't use see bug 60618
// #define IDS_BANNER1PART2                10002
#define IDS_BANNER2                     10003
#define IDS_HELP_BANNER                 10004

#define IDS_GROUP_OUTPUT_FILE           13005
#define IDS_GROUP_INPUT_FILES           13006
#define IDS_GROUP_RESOURCES             13007
#define IDS_GROUP_CODEGEN               13008
#define IDS_GROUP_WARNING               13009
#define IDS_GROUP_LANGUAGE              13010
#define IDS_GROUP_MISC                  13011
#define IDS_GROUP_ADVANCED              13012
#define IDS_GROUP_DEBUG                 13013

#define IDS_ARG_FILE                    13100
#define IDS_ARG_FILELIST                13101
#define IDS_ARG_NUMBER                  13102
#define IDS_ARG_STRING                  13103
#define IDS_ARG_WILDCARD                13104
#define IDS_ARG_SYMLIST                 13105
#define IDS_ARG_PATHLIST                13106
#define IDS_ARG_RESINFO                 13107
#define IDS_ARG_CLASS                   13108
#define IDS_ARG_IMPORTLIST              13109
#define IDS_ARG_PATH                    13110
#define IDS_NUMBERLIST                  13111
#define IDS_ARG_ASSEMBLY_NAME           13112
#define IDS_ARG_VERSION                 13113

#define IDS_HELP_SHORTFORM              13200

#define IDS_HELP_OUT                    13302
#define IDS_HELP_TARGET_EXE             13303
#define IDS_HELP_TARGET_WIN             13304
#define IDS_HELP_TARGET_LIB             13305
#define IDS_HELP_TARGET_MOD             13306
#define IDS_HELP_TARGET_APPCTR          13307
#define IDS_HELP_TARGET_WINMDOBJ        13308
#define IDS_HELP_RECURSE                13310
#define IDS_HELP_MAIN                   13311
#define IDS_HELP_REFERENCE              13312
#define IDS_HELP_RESOURCE               13314
#define IDS_HELP_RESINFO                13315
#define IDS_HELP_LINKRESOURCE           13316
#define IDS_HELP_WIN32ICON              13317
#define IDS_HELP_WIN32RESOURCE          13318
#define IDS_HELP_DEBUG                  13320
#define IDS_HELP_OPTIMIZE               13321
#define IDS_HELP_LANGUAGE               13323
#define IDS_HELP_ROOTNAMESPACE          13324
#define IDS_HELP_IMPORTS                13325
#define IDS_HELP_DEFINE                 13326
#define IDS_HELP_REMOVEINT              13327
#define IDS_HELP_OPTCOMPARE_BIN         13328
#define IDS_HELP_OPTCOMPARE_TXT         13329
#define IDS_HELP_OPTEXPLICIT            13330
#define IDS_HELP_OPTSTRICT              13331
#define IDS_HELP_OPTINFER               13332
#define IDS_HELP_RESPONSE               13333
#define IDS_HELP_NOLOGO                 13334
#define IDS_HELP_HELP                   13335
#define IDS_HELP_BUGREPORT              13336
#define IDS_HELP_TIME                   13337
#define IDS_HELP_VERBOSE                13338
#define IDS_HELP_BASEADDRESS            13340
// UNAVAILABLE                          13341
#define IDS_HELP_LIBPATH                13342
#define IDS_HELP_KEYFILE                13343
#define IDS_HELP_KEYCONTAINER           13344
#define IDS_HELP_DEBUGFULL              13345
#define IDS_HELP_DEBUGPDBONLY           13346
#define IDS_HELP_NOWARN                 13347
#define IDS_HELP_WARNASERROR            13348
#define IDS_HELP_DELAYSIGN              13349
#define IDS_HELP_ADDMODULE              13350
#define IDS_HELP_UTF8OUTPUT             13351
#define IDS_HELP_QUIET                  13352
#define IDS_HELP_SDKPATH                13353
#define IDS_HELP_STARLITE               13354
#define IDS_HELP_DOC                    13355
#define IDS_HELP_NOCONFIG               13356
#define IDS_HELP_CODEPAGE               13357
#define IDS_HELP_FILEALIGN              13358
#define IDS_HELP_DOC_TO_FILE            13359
#define IDS_HELP_PLATFORM               13360
#define IDS_HELP_ERRORREPORT            13361
// UNAVAILABLE                          13362
#define IDS_HELP_NOSTDLIB               13363
#define IDS_HELP_NOWARNLIST             13364
#define IDS_HELP_WARNASERRORLIST        13365
#define IDS_HELP_OPTSTRICTCUST          13366
#define IDS_HELP_MODULEASSEMBLYNAME     13367
#define IDS_HELP_WIN32MANIFEST          13368
#define IDS_HELP_WIN32NOMANIFEST        13369
#define IDS_HELP_VBRUNTIME              13370
#define IDS_HELP_VBRUNTIME_FILE         13371
#define IDS_HELP_LANGVERSION            13372
#define IDS_HELP_LINKREFERENCE          13373
#define IDS_HELP_HIGHENTROPYVA          13374
#define IDS_HELP_SUBSYSTEMVERSION       13375

#define IDS_REPROTITLE                  12000
#define IDS_REPROVER                    12001
#define IDS_REPROOS                     12002
#define IDS_REPROUSER                   12003
#define IDS_REPROCOMMANDLINE            12004
#define IDS_REPROSOURCEFILE             12005
#define IDS_REPRODIAGS                  12006
#define IDS_REPROISSUETYPE              12007
#define IDS_REPRODESCRIPTION            12008
#define IDS_REPROCORRECTBEHAVIOR        12009
#define IDS_REPROURTVER                 12010
#define IDS_BUGREPORT1                  12040
#define IDS_BUGREPORT2                  12041
#define IDS_BUGREPORT3                  12042
#define IDS_ENTERDESC                   12050
#define IDS_ENTERCORRECT                12051
#define IDS_MSG_ADDSOURCEFILE           12052
#define IDS_MSG_ADDREFERENCE            12053
#define IDS_MSG_ADDRESOURCEFILE         12054
#define IDS_MSG_ADDIMPORT               12056
#define IDS_MSG_COMPILING               12057
#define IDS_MSG_COMPILE_OK              12058
#define IDS_MSG_COMPILE_BAD1            12059
#define IDS_MSG_COMPILE_BAD2            12060
#define IDS_MSG_ELAPSED_TIME            12061
#define IDS_MSG_COMPILE_WARN1           12062
#define IDS_MSG_COMPILE_WARN2           12063
#define IDS_MSG_COMPILE_WARN1BAD1       12064
#define IDS_MSG_COMPILE_WARN2BAD1       12065
#define IDS_MSG_COMPILE_WARN1BAD2       12066
#define IDS_MSG_COMPILE_WARN2BAD2       12067
#define IDS_MSG_ADDMODULE               12068
#define IDS_MSG_ADDLINKREFERENCE        12069


