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
// File: palconfig.cpp
//
// PAL APIs related to PAL configuration and settings
// ===========================================================================


#include <rotor_palrt.h>

#define CONFIG_DIRECTORY L"rotor"

/* PAL_GetMachineConfigurationDirectoryW
 * -------------------------------------
 * Locates the machine configuration directory for the PAL. This
 * is a "rotor" subdirectory of the location of the PAL. The
 * directory is created the first time this function is called
 * if it doesn't currently exist.
 *
 * Parameters:
 *      OUT LPWSTR lpDirectoryName: The path to the machine
 *                                  configuration directory
 *      IN UINT cbDirectoryName: The size in bytes of the buffer
 *                               pointed to by lpDirectoryName
 *
 * Returns: Whether lpDirectoryName was filled in with the
 *          location of the machine configuration directory.
 */
EXTERN_C
BOOL
PALAPI
PAL_GetMachineConfigurationDirectoryW(
                                      OUT LPWSTR lpDirectoryName,
                                      IN UINT cbDirectoryName)
{
    static BOOL bCreated = FALSE;
    BOOL b = FALSE;
    DWORD dw;

    dw = PAL_GetPALDirectoryW(lpDirectoryName, cbDirectoryName);
    if (dw == 0 || dw >= cbDirectoryName) {
        goto LExit;
    }

    // Append "rotor" to the path, which already has a trailing
    // separator.
    if (wcslen(lpDirectoryName) + sizeof(CONFIG_DIRECTORY)/sizeof(WCHAR) >= 
        cbDirectoryName) {
        goto LExit;
    }
    wcscat(lpDirectoryName, CONFIG_DIRECTORY);

    // Try to create the directory on the first call.
    if (!bCreated) {
        CreateDirectoryW(lpDirectoryName, NULL);
        bCreated = TRUE;
    }

    b = TRUE;

LExit:
    return b;
}

