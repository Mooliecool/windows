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
//*****************************************************************************
// WinWrap.h
//
// This file contains wrapper functions for Win32 API's that take strings.
// Support on each platform works as follows:
//      OS          Behavior
//      ---------   -------------------------------------------------------
//      NT          Fully supports both W and A funtions.
//      Win 9x      Supports on A functions, stubs out the W functions but
//                      then fails silently on you with no warning.
//      CE          Only has the W entry points.
//
// The Common Language Runtime internally uses UNICODE as the internal state 
// and string format.  This file will undef the mapping macros so that one 
// cannot mistakingly call a method that isn't going to work.  Instead, you 
// have to call the correct wrapper API.
//
//*****************************************************************************
#ifndef __WIN_WRAP_H__
#define __WIN_WRAP_H__


//********** Macros. **********************************************************

#define HIWORD64        HIWORD

#define SAFEDELARRAY(p) if ((p) != NULL) { delete [] p; (p) = NULL; }

//********** Includes. ********************************************************

#include <crtwrap.h>
#include <windows.h>
#include <wincrypt.h>
#include <specstrings.h>


#include "palclr.h"




//
// Win CE only supports the wide entry points, no ANSI.  So we redefine
// the wrappers right back to the *W entry points as macros.  This way no
// client code needs a wrapper on CE.
//
// _X86_ includes only 32-bit Windows on Intel.  Given every other platform
// we currently port to besides this platform is UNICODE, it makes no sense
// to force them to have both bound (for example, 32-bit Alpha).  Bug 2757.
//

// crypt.h
#define WszCryptAcquireContext CryptAcquireContextW

// winbase.h
#define WszGetBinaryType GetBinaryTypeW
#define WszGetShortPathName GetShortPathNameW
#define WszGetLongPathName GetLongPathNameW
#define WszGetEnvironmentStrings   GetEnvironmentStringsW
#define WszFreeEnvironmentStrings   FreeEnvironmentStringsW
#define WszFormatMessage   FormatMessageW
#define WszCreateMailslot   CreateMailslotW
#define WszEncryptFile   EncryptFileW
#define WszDecryptFile   DecryptFileW
#define WszOpenRaw   OpenRawW
#define WszQueryRecoveryAgents   QueryRecoveryAgentsW
#define Wszlstrcmp   lstrcmpW
#define Wszlstrcmpi   lstrcmpiW
#define Wszlstrcpy lstrcpyW
#define Wszlstrcat lstrcatW
#define WszCreateMutex CreateMutexW
#define WszOpenMutex OpenMutexW
#define WszCreateEvent CreateEventW
#define WszOpenEvent OpenEventW
#define WszCreateWaitableTimer CreateWaitableTimerW
#define WszOpenWaitableTimer OpenWaitableTimerW
#define WszCreateFileMapping CreateFileMappingW
#define WszOpenFileMapping OpenFileMappingW
#define WszGetLogicalDriveStrings GetLogicalDriveStringsW
#define WszLoadLibrary LoadLibraryW
#define WszLoadLibraryEx LoadLibraryExW
#define WszGetModuleFileName GetModuleFileNameW
#define WszGetModuleHandle GetModuleHandleW
#define WszOOMSafeGetCurrentModuleHandle(szAnsiString, wszUnicodeString) GetModuleHandleW(wszUnicodeString)
#define WszFatalAppExit FatalAppExitW
#define WszGetStartupInfo GetStartupInfoW
#define WszGetCommandLine GetCommandLineW
#define WszGetEnvironmentVariable GetEnvironmentVariableW
#define WszSetEnvironmentVariable SetEnvironmentVariableW
#define WszExpandEnvironmentStrings ExpandEnvironmentStringsW
#define WszOutputDebugString OutputDebugStringW
#define WszFindResource FindResourceW
#define WszFindResourceEx FindResourceExW
#define WszEnumResourceTypes EnumResourceTypesW
#define WszEnumResourceNames EnumResourceNamesW
#define WszEnumResourceLanguages EnumResourceLanguagesW
#define WszBeginUpdateResource BeginUpdateResourceW
#define WszUpdateResource UpdateResourceW
#define WszEndUpdateResource EndUpdateResourceW
#define WszGlobalAddAtom GlobalAddAtomW
#define WszGlobalFindAtom GlobalFindAtomW
#define WszGlobalGetAtomName GlobalGetAtomNameW
#define WszAddAtom AddAtomW
#define WszFindAtom FindAtomW
#define WszGetAtomName GetAtomNameW
#define WszGetProfileInt GetProfileIntW
#define WszGetProfileString GetProfileStringW
#define WszWriteProfileString WriteProfileStringW
#define WszGetProfileSection GetProfileSectionW
#define WszWriteProfileSection WriteProfileSectionW
#define WszGetPrivateProfileInt GetPrivateProfileIntW
#define WszGetPrivateProfileString GetPrivateProfileStringW
#define WszWritePrivateProfileString WritePrivateProfileStringW
#define WszGetPrivateProfileSection GetPrivateProfileSectionW
#define WszWritePrivateProfileSection WritePrivateProfileSectionW
#define WszGetPrivateProfileSectionNames GetPrivateProfileSectionNamesW
#define WszGetPrivateProfileStruct GetPrivateProfileStructW
#define WszWritePrivateProfileStruct WritePrivateProfileStructW
#define WszGetDriveType GetDriveTypeW
#define WszGetSystemDirectory GetSystemDirectoryW
#define WszGetTempPath GetTempPathW
#define WszGetTempFileName GetTempFileNameW
#define WszGetWindowsDirectory GetWindowsDirectoryW
#define WszSetCurrentDirectory SetCurrentDirectoryW
#define WszGetCurrentDirectory GetCurrentDirectoryW
#define WszGetDiskFreeSpace GetDiskFreeSpaceW
#define WszGetDiskFreeSpaceEx GetDiskFreeSpaceExW
#define WszCreateDirectory CreateDirectoryW
#define WszCreateDirectoryEx CreateDirectoryExW
#define WszRemoveDirectory RemoveDirectoryW
#define WszGetFullPathName GetFullPathNameW
#define WszDefineDosDevice DefineDosDeviceW
#define WszQueryDosDevice QueryDosDeviceW
#define WszCreateFile CreateFileW
#define WszSetFileAttributes SetFileAttributesW
#define WszGetFileAttributes GetFileAttributesW
#define WszGetFileAttributesEx GetFileAttributesExW
#define WszGetCompressedFileSize GetCompressedFileSizeW
#define WszDeleteFile DeleteFileW
#define WszFindFirstFileEx FindFirstFileExW
#define WszFindFirstFile FindFirstFileW
#define WszFindNextFile FindNextFileW
#define WszSearchPath SearchPathW
#define WszCopyFile CopyFileW
#define WszCopyFileEx CopyFileExW
#define WszMoveFile MoveFileW
#define WszMoveFileEx MoveFileExW
#define WszMoveFileWithProgress MoveFileWithProgressW
#define WszCreateSymbolicLink CreateSymbolicLinkW
#define WszQuerySymbolicLink QuerySymbolicLinkW
#define WszCreateHardLink CreateHardLinkW
#define WszCreateNamedPipe CreateNamedPipeW
#define WszGetNamedPipeHandleState GetNamedPipeHandleStateW
#define WszCallNamedPipe CallNamedPipeW
#define WszWaitNamedPipe WaitNamedPipeW
#define WszSetVolumeLabel SetVolumeLabelW
#define WszGetVolumeInformation GetVolumeInformationW
#define WszClearEventLog ClearEventLogW
#define WszBackupEventLog BackupEventLogW
#define WszOpenEventLog OpenEventLogW
#define WszRegisterEventSource RegisterEventSourceW
#define WszOpenBackupEventLog OpenBackupEventLogW
#define WszReadEventLog ReadEventLogW
#define WszReportEvent ReportEventW
#define WszAccessCheckAndAuditAlarm AccessCheckAndAuditAlarmW
#define WszAccessCheckByTypeAndAuditAlarm AccessCheckByTypeAndAuditAlarmW
#define WszAccessCheckByTypeResultListAndAuditAlarm AccessCheckByTypeResultListAndAuditAlarmW
#define WszObjectOpenAuditAlarm ObjectOpenAuditAlarmW
#define WszObjectPrivilegeAuditAlarm ObjectPrivilegeAuditAlarmW
#define WszObjectCloseAuditAlarm ObjectCloseAuditAlarmW
#define WszObjectDeleteAuditAlarm ObjectDeleteAuditAlarmW
#define WszPrivilegedServiceAuditAlarm PrivilegedServiceAuditAlarmW
#define WszSetFileSecurity SetFileSecurityW
#define WszGetFileSecurity GetFileSecurityW
#define WszFindFirstChangeNotification FindFirstChangeNotificationW
#define WszIsBadStringPtr IsBadStringPtrW
#define WszLookupAccountSid LookupAccountSidW
#define WszLookupAccountName LookupAccountNameW
#define WszLookupPrivilegeValue LookupPrivilegeValueW
#define WszLookupPrivilegeName LookupPrivilegeNameW
#define WszLookupPrivilegeDisplayName LookupPrivilegeDisplayNameW
#define WszBuildCommDCB BuildCommDCBW
#define WszBuildCommDCBAndTimeouts BuildCommDCBAndTimeoutsW
#define WszCommConfigDialog CommConfigDialogW
#define WszGetDefaultCommConfig GetDefaultCommConfigW
#define WszSetDefaultCommConfig SetDefaultCommConfigW
#define WszGetComputerName GetComputerNameW
#define WszSetComputerName SetComputerNameW
#define WszGetUserName GetUserNameW
#define WszLogonUser LogonUserW
#define WszCreateProcessAsUser CreateProcessAsUserW
#define WszGetCurrentHwProfile GetCurrentHwProfileW
#define WszGetVersionEx GetVersionExW
#define WszCreateJobObject CreateJobObjectW
#define WszOpenJobObject OpenJobObjectW


// winuser.h
#define WszMAKEINTRESOURCE MAKEINTRESOURCEW
#define Wszwvsprintf wvsprintfW
// #define Wszwsprintf wsprintfW
// #define Wszwnsprintf wnsprintfW
#define WszLoadKeyboardLayout LoadKeyboardLayoutW
#define WszGetKeyboardLayoutName GetKeyboardLayoutNameW
#define WszCreateDesktop CreateDesktopW
#define WszOpenDesktop OpenDesktopW
#define WszEnumDesktops EnumDesktopsW
#define WszCreateWindowStation CreateWindowStationW
#define WszOpenWindowStation OpenWindowStationW
#define WszEnumWindowStations EnumWindowStationsW
#define WszGetUserObjectInformation GetUserObjectInformationW
#define WszSetUserObjectInformation SetUserObjectInformationW
#define WszRegisterWindowMessage RegisterWindowMessageW
#define WszSIZEZOOMSHOW SIZEZOOMSHOWW
#define WszWS_TILEDWINDOW WS_TILEDWINDOWW
#define WszGetMessage GetMessageW
#define WszDispatchMessage DispatchMessageW
#define WszPostMessage PostMessageW
#define WszPeekMessage PeekMessageW
#define WszSendMessage SendMessageW
#define WszSendMessageTimeout SendMessageTimeoutW
#define WszSendNotifyMessage SendNotifyMessageW
#define WszSendMessageCallback SendMessageCallbackW
#define WszBroadcastSystemMessage BroadcastSystemMessageW
#define WszRegisterDeviceNotification RegisterDeviceNotificationW
#define WszPostMessage PostMessageW
#define WszPostThreadMessage PostThreadMessageW
#define WszPostAppMessage PostAppMessageW
#define WszDefWindowProc DefWindowProcW
#define WszCallWindowProc CallWindowProcW
#define WszRegisterClass RegisterClassW
#define WszUnregisterClass UnregisterClassW
#define WszGetClassInfo GetClassInfoW
#define WszRegisterClassEx RegisterClassExW
#define WszGetClassInfoEx GetClassInfoExW
#define WszCreateWindowEx CreateWindowExW
#define WszCreateWindow CreateWindowW
#define WszCreateDialogParam CreateDialogParamW
#define WszCreateDialogIndirectParam CreateDialogIndirectParamW
#define WszCreateDialog CreateDialogW
#define WszCreateDialogIndirect CreateDialogIndirectW
#define WszDialogBoxParam DialogBoxParamW
#define WszDialogBoxIndirectParam DialogBoxIndirectParamW
#define WszDialogBox DialogBoxW
#define WszDialogBoxIndirect DialogBoxIndirectW
#define WszSetDlgItemText SetDlgItemTextW
#define WszGetDlgItemText GetDlgItemTextW
#define WszSendDlgItemMessage SendDlgItemMessageW
#define WszDefDlgProc DefDlgProcW
#define WszCallMsgFilter CallMsgFilterW
#define WszRegisterClipboardFormat RegisterClipboardFormatW
#define WszGetClipboardFormatName GetClipboardFormatNameW
#define WszCharToOem CharToOemW
#define WszOemToChar OemToCharW
#define WszCharToOemBuff CharToOemBuffW
#define WszOemToCharBuff OemToCharBuffW
#define WszCharUpper CharUpperW
#define WszCharUpperBuff CharUpperBuffW
#define WszCharLower CharLowerW
#define WszCharLowerBuff CharLowerBuffW
#define WszCharNext CharNextW
#define WszIsCharAlpha IsCharAlphaW
#define WszIsCharAlphaNumeric IsCharAlphaNumericW
#define WszIsCharUpper IsCharUpperW
#define WszIsCharLower IsCharLowerW
#define WszGetKeyNameText GetKeyNameTextW
#define WszVkKeyScan VkKeyScanW
#define WszVkKeyScanEx VkKeyScanExW
#define WszMapVirtualKey MapVirtualKeyW
#define WszMapVirtualKeyEx MapVirtualKeyExW
#define WszLoadAccelerators LoadAcceleratorsW
#define WszCreateAcceleratorTable CreateAcceleratorTableW
#define WszCopyAcceleratorTable CopyAcceleratorTableW
#define WszTranslateAccelerator TranslateAcceleratorW
#define WszLoadMenu LoadMenuW
#define WszLoadMenuIndirect LoadMenuIndirectW
#define WszChangeMenu ChangeMenuW
#define WszGetMenuString GetMenuStringW
#define WszInsertMenu InsertMenuW
#define WszAppendMenu AppendMenuW
#define WszModifyMenu ModifyMenuW
#define WszInsertMenuItem InsertMenuItemW
#define WszGetMenuItemInfo GetMenuItemInfoW
#define WszSetMenuItemInfo SetMenuItemInfoW
#define WszDrawText DrawTextW
#define WszDrawTextEx DrawTextExW
#define WszGrayString GrayStringW
#define WszDrawState DrawStateW
#define WszTabbedTextOut TabbedTextOutW
#define WszGetTabbedTextExtent GetTabbedTextExtentW
#define WszSetProp SetPropW
#define WszGetProp GetPropW
#define WszRemoveProp RemovePropW
#define WszEnumPropsEx EnumPropsExW
#define WszEnumProps EnumPropsW
#define WszSetWindowText SetWindowTextW
#define WszGetWindowText GetWindowTextW
#define WszGetWindowTextLength GetWindowTextLengthW
#define WszMessageBox MessageBoxW
#define WszMessageBoxEx MessageBoxExW
#define WszMessageBoxIndirect MessageBoxIndirectW
#define WszGetWindowLong GetWindowLongW
#define WszSetWindowLong SetWindowLongW
#define WszSetWindowLongPtr SetWindowLongPtrW
#define WszGetWindowLongPtr GetWindowLongPtrW
#define WszGetClassLong GetClassLongW
#define WszSetClassLong SetClassLongW
#define WszFindWindow FindWindowW
#define WszFindWindowEx FindWindowExW
#define WszGetClassName GetClassNameW
#define WszSetWindowsHook SetWindowsHookW
#define WszSetWindowsHook SetWindowsHookW
#define WszSetWindowsHookEx SetWindowsHookExW
#define WszLoadBitmap LoadBitmapW
#define WszLoadCursor LoadCursorW
#define WszLoadCursorFromFile LoadCursorFromFileW
#define WszLoadIcon LoadIconW
#define WszLoadImage LoadImageW
#define WszLoadString LoadStringW
#define WszIsDialogMessage IsDialogMessageW
#define WszDlgDirList DlgDirListW
#define WszDlgDirSelectEx DlgDirSelectExW
#define WszDlgDirListComboBox DlgDirListComboBoxW
#define WszDlgDirSelectComboBoxEx DlgDirSelectComboBoxExW
#define WszDefFrameProc DefFrameProcW
#define WszDefMDIChildProc DefMDIChildProcW
#define WszCreateMDIWindow CreateMDIWindowW
#define WszWinHelp WinHelpW
#define WszChangeDisplaySettings ChangeDisplaySettingsW
#define WszChangeDisplaySettingsEx ChangeDisplaySettingsExW
#define WszEnumDisplaySettings EnumDisplaySettingsW
#define WszEnumDisplayDevices EnumDisplayDevicesW
#define WszSystemParametersInfo SystemParametersInfoW
#define WszGetMonitorInfo GetMonitorInfoW
#define WszGetWindowModuleFileName GetWindowModuleFileNameW
#define WszRealGetWindowClass RealGetWindowClassW
#define WszGetAltTabInfo GetAltTabInfoW
#define WszRegOpenKeyEx RegOpenKeyExW
#define WszRegOpenKey(hKey, wszSubKey, phkRes) RegOpenKeyExW(hKey, wszSubKey, 0, KEY_ALL_ACCESS, phkRes)
#define WszRegQueryValueEx RegQueryValueExW
#define WszRegQueryValueExTrue RegQueryValueExW
#define WszRegQueryStringValueEx RegQueryValueExW
#define WszRegDeleteKey RegDeleteKeyW
#define WszRegCreateKeyEx RegCreateKeyExW
#define WszRegSetValueEx RegSetValueExW
#define WszRegDeleteValue RegDeleteValueW
#define WszRegLoadKey RegLoadKeyW
#define WszRegUnLoadKey RegUnLoadKeyW
#define WszRegRestoreKey RegRestoreKeyW
#define WszRegReplaceKey RegReplaceKeyW
#define WszRegQueryInfoKey RegQueryInfoKeyW
#define WszRegEnumValue RegEnumValueW
#define WszRegEnumKeyEx RegEnumKeyExW
#define WszGetCalendarInfo GetCalendarInfoW
#define WszGetDateFormat GetDateFormatW
#define WszGetTimeFormat GetTimeFormatW
#define WszLCMapString LCMapStringW
#define WszMultiByteToWideChar MultiByteToWideChar
#define WszWideCharToMultiByte WideCharToMultiByte
#define WszCreateSemaphore CreateSemaphoreW

#define WszQueryActCtxW QueryActCtxW



#ifndef _T
#define _T(str) L ## str
#endif


// on win98 and higher
#define Wszlstrlen      lstrlenW
#define Wszlstrcpy      lstrcpyW
#define Wszlstrcat      lstrcatW


            

//*****************************************************************************
// Prototypes for API's.
//*****************************************************************************


BOOL OnUnicodeSystem();
void ForceUnicodeWrappers();

extern DWORD DBCS_MAXWID;
#ifdef __cplusplus
inline DWORD GetMaxDBCSCharByteSize()
{
    // contract.h not visible here
    __annotation(L"LEAF " L"GetMaxDBCSCharByteSize");

    _ASSERTE(DBCS_MAXWID != 0);
    return (DBCS_MAXWID); 
}
#else
#define GetMaxDBCSCharByteSize() (DBCS_MAXWID)
#endif

HRESULT WszConvertToUnicode(LPCSTR pszIn, LONG cbIn, __deref_inout_opt LPWSTR* lpwszOut,
    __inout_opt ULONG* lpcchOut, BOOL fAlloc);

HRESULT WszConvertToAnsi(LPCWSTR pwszIn, __deref_inout_opt LPSTR* lpszOut,
    ULONG cbOutMax, __inout_opt ULONG* lpcbOut, BOOL fAlloc, BOOL bBestFitMapping=FALSE);

BOOL RunningInteractive();



#ifndef Wsz_mbstowcs
#define Wsz_mbstowcs(szOut, szIn, iSize) WszMultiByteToWideChar(CP_ACP, 0, szIn, -1, szOut, iSize)
#endif


#ifndef Wsz_wcstombs
#define Wsz_wcstombs(szOut, szIn, iSize) WszWideCharToMultiByte(CP_ACP, 0, szIn, -1, szOut, iSize, 0, 0)
#endif

// For all platforms:

BOOL
WszCreateProcess(
    LPCWSTR lpApplicationName,
    LPCWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
    );

DWORD
WszGetWorkingSet(
    VOID
    );


    #undef InterlockedExchangePointer
    #define InterlockedExchangePointer(dst, src) (PVOID)(size_t)InterlockedExchange((LPLONG)(size_t)(PVOID*)dst, (LONG)(size_t)(PVOID)src)

#endif  // __WIN_WRAP_H__
