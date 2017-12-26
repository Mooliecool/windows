// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==
// ===========================================================================
// File: errors.h
//
// Contains a list of all the warnings and errors in the compiler.
// ===========================================================================

#ifndef ERRORDEF
#error Must define ERRORDEF macro before including errors.h
#endif

// NOTE:  Warnings, errors, and fatal errors are determined by the
//        warning level value as follows:
//
//          0   = ERROR_ERROR
//          >0  = ERROR_WARNING
//          -1  = ERROR_FATAL
//
//     Error  Warning
//     Number, Level,      Error name,                    Resource name
ERRORDEF( 1001, -1,  FTL_InternalError,                  IDS_InternalError)
ERRORDEF( 1002, -1,  FTL_NoMemory,                       IDS_NoMemory)
ERRORDEF( 1003,  0,  ERR_MissingOptionArg,               IDS_MissingOptionArg)
ERRORDEF( 1004, -1,  FTL_ComPlusInit,                    IDS_ComPlusInit)
ERRORDEF( 1005, -1,  FTL_FileTooBig,                     IDS_FileTooBig)
ERRORDEF( 1006,  0,  ERR_DuplicateResponseFile,          IDS_DuplicateResponseFile)
ERRORDEF( 1007,  0,  ERR_OpenResponseFile,               IDS_OpenResponseFile)
ERRORDEF( 1008,  0,  ERR_NoFileSpec,                     IDS_NoFileSpec)
ERRORDEF( 1009,  0,  ERR_CantOpenFileWrite,              IDS_CantOpenFileWrite)
ERRORDEF( 1010,  0,  ERR_SwitchNeedsString,              IDS_SwitchNeedsString)
ERRORDEF( 1011,  0,  ERR_CantOpenBinaryAsText,           IDS_CantOpenBinaryAsText)
ERRORDEF( 1012,  0,  ERR_BadOptionValue,                 IDS_BadOptionValue)
ERRORDEF( 1013,  0,  ERR_BadSwitch,                      IDS_BadSwitch)
ERRORDEF( 1014, -1,  FTL_InitError,                      IDS_InitError)
ERRORDEF( 1015, -1,  FTL_NoMessagesDLL,                  0)
ERRORDEF( 1016,  0,  ERR_NoInputs,                       IDS_NoInputs)
ERRORDEF( 1017,  0,  ERR_NoOutput,                       IDS_NoOutput)
ERRORDEF( 1018, -1,  FTL_RequiredFileNotFound,           IDS_RequiredFileNotFound)
ERRORDEF( 1019,  0,  ERR_MetaDataError,                  IDS_MetaDataError)
ERRORDEF( 1020,  1,  WRN_IgnoringAssembly,               IDS_IgnoringAssembly)
ERRORDEF( 1021,  1,  WRN_OptionConflicts,                IDS_OptionConflicts)
ERRORDEF( 1022,  0,  ERR_CantReadResource,               IDS_CantReadResource)
ERRORDEF( 1023,  0,  ERR_CantEmbedResource,              IDS_CantEmbedResource)
//ERRORDEF( 1024,  0,  ERR_FileNotInDir,                   IDS_FileNotInDir)  // Cut as part of multi-dir assemblies
ERRORDEF( 1025,  0,  ERR_InvalidFileDefInComType,        IDS_InvalidFileDefInComType)
ERRORDEF( 1026,  0,  ERR_InvalidVersionFormat,           IDS_InvalidVersionString)
ERRORDEF( 1027,  0,  ERR_InvalidOSString,                IDS_InvalidOSString)
ERRORDEF( 1028,  0,  ERR_NeedPrivateKey,                 IDS_NeedPrivateKey)
ERRORDEF( 1029,  0,  ERR_CryptoNoKeyContainer,           IDS_CryptoNoKeyContainer)
ERRORDEF( 1030,  0,  ERR_CryptoFailed,                   IDS_CryptoFailed)
ERRORDEF( 1031,  0,  ERR_CantReadIcon,                   IDS_CantReadIcon)
ERRORDEF( 1032,  0,  ERR_AutoResGen,                     IDS_AutoResGen)
ERRORDEF( 1033,  0,  ERR_DuplicateCA,                    IDS_DuplicateCA)
ERRORDEF( 1034,  0,  ERR_CantRenameAssembly,             IDS_CantRenameAssembly)
ERRORDEF( 1035,  0,  ERR_NoMainOnDlls,                   IDS_NoMainOnDlls)
ERRORDEF( 1036,  0,  ERR_AppNeedsMain,                   IDS_AppNeedsMain)
ERRORDEF( 1037,  0,  ERR_NoMainFound,                    IDS_NoMainFound)
//ERRORDEF( 1038,  0,  ERR_OutAndInstall,                  IDS_OutAndInstall)
ERRORDEF( 1039, -1,  FTL_FusionInit,                     IDS_FusionInit)
ERRORDEF( 1040,  0,  ERR_FusionInstallFailed,            IDS_FusionInstallFailed)
ERRORDEF( 1041,  0,  ERR_BadMainFound,                   IDS_BadMainFound)
ERRORDEF( 1042,  0,  ERR_CantAddExes,                    IDS_CantAddExes)
ERRORDEF( 1043,  0,  ERR_SameOutAndSource,               IDS_SameOutAndSource)
ERRORDEF( 1044,  0,  ERR_CryptoFileFailed,               IDS_CryptoFileFailed)
ERRORDEF( 1045,  0,  ERR_FileNameTooLong,                IDS_FileNameTooLong)
ERRORDEF( 1046,  0,  ERR_DupResourceIdent,               IDS_DupResourceIdent)
ERRORDEF( 1047,  0,  ERR_ModuleImportError,              IDS_ModuleImportError)
ERRORDEF( 1048,  0,  ERR_AssemblyModuleImportError,      IDS_AssemblyModuleImportError)
ERRORDEF( 1049,  1,  WRN_InvalidTime,                    IDS_InvalidTime)
ERRORDEF( 1050,  1,  WRN_FeatureDeprecated,              IDS_FeatureDeprecated)
ERRORDEF( 1051,  0,  ERR_EmitCAFailed,                   IDS_EmitCAFailed)
ERRORDEF( 1052,  0,  ERR_ParentNotAnAssembly,            IDS_ParentNotAnAssembly)
ERRORDEF( 1053,  1,  WRN_InvalidVersionString,           IDS_InvalidVersionFormat)
ERRORDEF( 1054,  0,  ERR_InvalidVersionString,           IDS_InvalidVersionFormat)
ERRORDEF( 1055,  0,  ERR_RefNotStrong,                   IDS_RefNotStrong)
ERRORDEF( 1056,  1,  WRN_RefHasCulture,                  IDS_RefHasCulture)
ERRORDEF( 1057,  0,  ERR_ExeHasCulture,                  IDS_ExeHasCulture)
ERRORDEF( 1058,  0,  ERR_CantAddAssembly,                IDS_CantAddAssembly)
ERRORDEF( 1059,  0,  ERR_UnknownError,                   IDS_UnknownError)
ERRORDEF( 1060,  0,  ERR_CryptoHashFailed,               IDS_CryptoHashFailed)
ERRORDEF( 1061,  0,  ERR_BadOptionValueHR,               IDS_BadOptionValueHR)
ERRORDEF( 1062,  1,  WRN_IgnoringDuplicateSource,        IDS_IgnoringDuplicateSource)
ERRORDEF( 1063,  0,  ERR_DuplicateExportedType,          IDS_DuplicateExportedType)
ERRORDEF( 1065,  0,  FTL_InputFileNameTooLong,           IDS_InputFileNameTooLong)
ERRORDEF( 1066,  0,  ERR_IllegalOptionChar,              IDS_IllegalOptionChar)
ERRORDEF( 1067,  0,  ERR_BinaryFile,                     IDS_BinaryFile)
ERRORDEF( 1068,  0,  ERR_DuplicateModule,                IDS_DuplicateModule)
ERRORDEF( 1069, -1,  FTL_OutputFileExists,               IDS_OutputFileExists)
ERRORDEF( 1070,  0,  ERR_AgnosticToMachine,              IDS_AgnosticToMachineModule)


ERRORDEF( 1072,  0,  ERR_ConflictingMachine,             IDS_ConflictingMachineModule)
ERRORDEF( 1073,  1,  WRN_ConflictingMachine,             IDS_ConflictingMachineAssembly)
ERRORDEF( 1074,  0,  ERR_ModuleNameDifferent,            IDS_ModuleNameDifferent)
ERRORDEF( 1075,  1,  WRN_DelaySignWithNoKey,             IDS_DelaySignWithNoKey)

ERRORDEF( 1076,  0,  ERR_DuplicateTypeForwarders,        IDS_DuplicateTypeForwarders)
ERRORDEF( 1077,  0,  ERR_TypeFwderMatchesDeclared,       IDS_TypeFwderMatchesDeclared)

#undef ERRORDEF
