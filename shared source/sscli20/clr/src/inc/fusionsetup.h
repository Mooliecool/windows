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
/*============================================================
**
** Header:  FusionSetup.h
**
** Purpose: Defines that are used in both managed and unmanged
**          code
**
** Date:  Jan 1, 2001
**
===========================================================*/
#ifndef _FUSIONSETUP_H
#define _FUSIONSETUP_H

#define DYNAMIC_DIRECTORY                   L"DYNAMIC_DIRECTORY"
#define LOADER_OPTIMIZATION                 L"LOADER_OPTIMIZATION"
#define LICENSE_FILE                        L"LICENSE_FILE"
#define APPENV_BASE                         L"APPBASE"
#define APPENV_RELATIVEPATH                 L"RELPATH"
#define APPENV_GLOBALPATH                   L"CORPATH"
#define APPENV_DEVPATH                      L"DEVPATH"
#define APPENV_RUNFROMSOURCE                L"__FUSION_RUN_FROM_SOURCE__"
#define CONFIGURATION_EXTENSION             L".config"
#define MACHINE_CONFIGURATION_FILE          L"config\\machine.config"

#ifndef ACTAG_HOST_CONFIG_FILE
#define ACTAG_HOST_CONFIG_FILE              L"HOST_CONFIG"
#endif 

// These are defines instead of enums because they are
// used to change FusionSetup.cs as well as being used in
// unmanaged code
#define  LOADER_APPLICATION_BASE            0
#define  LOADER_CONFIGURATION_BASE          1
#define  LOADER_DYNAMIC_BASE                2
#define  LOADER_DEVPATH                     3
#define  LOADER_APPLICATION_NAME            4
#define  LOADER_PRIVATE_PATH                5
#define  LOADER_PRIVATE_BIN_PATH_PROBE      6
#define  LOADER_SHADOW_COPY_DIRECTORIES     7
#define  LOADER_SHADOW_COPY_FILES           8
#define  LOADER_CACHE_PATH                  9
#define  LOADER_LICENSE_FILE               10
#define  LOADER_DISALLOW_PUBLISHER_POLICY  11
#define  LOADER_DISALLOW_CODE_DOWNLOAD     12
#define  LOADER_DISALLOW_BINDING_REDIRECTS 13
#define  LOADER_DISALLOW_APPBASE_PROBING   14
#define  LOADER_CONFIGURATION_BYTES        15
#define  LOADER_MAXIMUM                    16

#endif
