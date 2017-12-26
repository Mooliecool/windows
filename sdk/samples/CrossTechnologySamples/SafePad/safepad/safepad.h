//---------------------------------------------------------------------------
//  SafePad.H
//---------------------------------------------------------------------------
#ifndef _safepad_h
#define _safepad_h

BOOL RegisterWindows(HINSTANCE);
BOOL CreateWindowInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LPTSTR   GetStringRes (int id);
VOID InitializeRestart();
VOID DisableRestart();
VOID InitializeRecovery();
VOID DisableRecovery();
VOID InitializeReporting();
VOID DisableReporting( BOOL ui );
VOID CrashApplication();
VOID HangApplication();
VOID UpdateMenuState();
VOID FillParentWithChild( HWND parent, HWND child );
VOID CALLBACK ApplicationCrashTimerCallback(
    HWND hWnd,
    UINT message,
    UINT idTimer,
    DWORD dwTime);
DWORD SaveRecoveryDocument( __in PVOID pParam );
VOID HandleFileOpenNew( BOOL opening );
VOID LoadEditor( HANDLE hFrom );
BOOL HandleFileSave( LPTSTR lpszFile );
VOID HandleInitialLoad();
BOOL IsDirty();
VOID SetCaption( LPTSTR caption );  
BOOL CreateEditor( HWND parent );
VOID DisplayMessage( LPTSTR caption, LPTSTR text );
VOID DisplayError( LPTSTR context );
VOID CalculateRecoveryPath();
BOOL ProcessRecoveryFile();

#define APPNAME L"SafePad"
#define EXENAME L"SafePad.exe"
#define READMEFILE L"Readme.txt"
#define RECOVERYFILE L"SafePadRecover.txt"
#define ID_CRASHTIMER 100
#define MAXCAPTION 255
#define EDIT_BUF_SIZE  32768
#define MAXERRMSG 255





// Command IDs for system menus
#define IDM_NEW            100
#define IDM_OPEN           101
#define IDM_SAVE           102
#define IDM_SAVEAS         103
#define IDM_PRINT          104
#define IDM_PRINTSETUP     105
#define IDM_EXIT           106
#define IDM_UNDO           200
#define IDM_CUT            201
#define IDM_COPY           202
#define IDM_PASTE          203
#define IDM_LINK           204
#define IDM_LINKS          205
#define IDM_HELPCONTENTS   300
#define IDM_HELPSEARCH     301
#define IDM_HELPHELP       302
#define IDM_ABOUT          303
#define IDM_HELPTOPICS     304
#define IDM_NO_OP		   999  // used for disabled menu items
#define IDM_CRASH_APP      400
#define IDM_ENABLE_RESTART 401
#define IDM_ENABLE_RECOVERY 402
#define IDM_ENABLE_REPORTING 403
#define IDM_COMMIT_TX 405
#define IDM_ABORT_TX 406
#define IDM_HANG_APP 407


#define IDC_STATIC -1

#define DLG_VERFIRST        400
#define IDC_COMPANY        DLG_VERFIRST
#define IDC_FILEDESC          DLG_VERFIRST+1
#define IDC_PRODVER         DLG_VERFIRST+2
#define IDC_COPYRIGHT       DLG_VERFIRST+3
#define IDC_OSVERSION       DLG_VERFIRST+4
#define IDC_TRADEMARK       DLG_VERFIRST+5
#define DLG_VERLAST         DLG_VERFIRST+5

#define IDC_LABEL           DLG_VERLAST+1


#define IDS_APP_TITLE       500
#define IDS_DISPLAYCHANGED  501
#define IDS_VER_INFO_LANG   502
#define IDS_VERSION_ERROR   503
#define IDS_NO_HELP         504

#endif
