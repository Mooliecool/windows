/*
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
 * ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Copyright (c) Microsoft Corporation.  All Rights Reserved.
 */

#include <windows.h>
#include <tchar.h>
#include <strsafe.h> // Functions defined inline, no need to explicitly link to strsafe.lib
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <ktmw32.h>
#include <shlobj.h>
#include <knownfolders.h>
#include <werapi.h>
#include "SafePad.h"

// Global Variables:
HINSTANCE hInstance;                        // Current app instance
HWND hWndMain, hWndEdit;                    // Handles to main window and edit control
HANDLE hMainThread;                         // Handle to main thread - allows us to resolve hangs
HANDLE hActiveFile = INVALID_HANDLE_VALUE;  // Handle to active (open) file
TCHAR szAppName[100];                       // Name of the app
TCHAR szCaption[100];                       // Title bar text
TCHAR EditorBuffer[EDIT_BUF_SIZE];          // Editor text buffer
TCHAR ActiveFilename[MAX_PATH];             // Name of active (open) file
TCHAR RecoveryFullPath[MAX_PATH];           // Calculated recovery path
TCHAR RecoveryFolderPath[MAX_PATH];         // Recovery folder

BOOL RestartEnabled = FALSE;
BOOL RecoveryEnabled = FALSE;
BOOL ReportingEnabled = FALSE;
BOOL CrashTimerExpired = FALSE;

DWORD recoveryPingInterval = RECOVERY_DEFAULT_PING_INTERVAL;
DWORD recoveryOptionFlags = 0;
DWORD RestartTimeIn = 61000;

// Main Windows application entry point
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    MSG msg;
    HACCEL hAccelTable;
    HRESULT hRes;

    hInstance = hInstance; // Store instance handle in our global variable

    // Initialize global strings with safe string handling
    hRes = StringCchCopyN(szAppName, ARRAYSIZE(szAppName), APPNAME, sizeof(APPNAME));
    if (FAILED(hRes)) 
    {
        return FALSE; // Insufficient buffer space
    }
    LoadString (hInstance, IDS_APP_TITLE, szCaption, ARRAYSIZE(szCaption));
    hAccelTable = LoadAccelerators (hInstance, szAppName);
    UpdateMenuState();
    
    // Perform application initialization - register window and spin up an instance
    if (!RegisterWindows(hInstance) || !CreateWindowInstance(hInstance, nCmdShow)) 
    {
        return FALSE;
    }
    
    // First, turn off error reporting for now
    DisableReporting( FALSE );

    // Take care of initial loads of file
    HandleInitialLoad();
    SetCaption( ActiveFilename );

    // Stash off a handle to the main UI thread;
    // We may need to use this later to cancel all
    // pending IO
    hMainThread = GetCurrentThread();
    
    // Core message loop
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        if (!TranslateAccelerator (msg.hwnd, hAccelTable, &msg)) 
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

// Create and register application's window class(es)
BOOL RegisterWindows(HINSTANCE hInstance)
{
    WNDCLASS  wc;

    // Fill in window class structure with parameters that describe
    // the main window.
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC)WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon (hInstance, szAppName);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = szAppName;
    wc.lpszClassName = szAppName;

    // Register the window class and return success/failure code.
    return RegisterClassW(&wc);
}

// Spin up this instance of the application's window class
BOOL CreateWindowInstance(HINSTANCE hInstance, int nCmdShow)
{
    DWORD dwMainStyle = WS_OVERLAPPEDWINDOW;
    hWndMain = CreateWindow(szAppName, szCaption, dwMainStyle,
                            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
                            NULL, NULL, hInstance, NULL);

    // Make sure we were able to create a new window - e.g. OS
    // may have run out of window handles, etc.
    if (!hWndMain)
    {
        return FALSE;
    }

    // Populate main window with the actual editor
    if (!CreateEditor( hWndMain ))
    {
        DisplayMessage( L"Unrecoverable Error", L"Unable to create editor - aborting." );
        DestroyWindow (hWndMain);
        return FALSE;
    }

    // Start timer so that we can enable the crash app menu item when enough 
    // time has elapsed (10 seconds) since process start
    SetTimer( hWndMain, ID_CRASHTIMER, RestartTimeIn, (TIMERPROC)ApplicationCrashTimerCallback );

    // We're going to exclude this app from error reporting by default, though only 
    // for instances started by the current user (the second parameter)
    WerAddExcludedApplication( EXENAME, FALSE );
    
    // And away we go ...
    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain);

    return TRUE;
}

// Main Window procedure for handling WM_ events dispatched by the message pump
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int menuId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;
    POINT pnt;
    HMENU hMenu;
    BOOL bGotHelp;
    
    switch (message) {

        // Handle system menu messages
        case WM_COMMAND:
            menuId = LOWORD(wParam);

            UpdateMenuState();

            //Parse the menu selections:
            switch (menuId) {   

                // Handle application exit
                case IDM_EXIT:
                    DestroyWindow (hWnd);
                    break;

                // Crash the application, to test
                // the various app recovery and restart APIs
                // NOTE: simply dividing by zero without using the temp
                // result of the calculation allows the compiler to 
                // aggressively optimize the problem away
                case IDM_CRASH_APP:
                    CrashApplication();
                    break;

                case IDM_HANG_APP:
                    HangApplication();
                    break;

                // Enable application restart for SafePad
                case IDM_ENABLE_RESTART:
                    if (!RestartEnabled)
                    {
                        RestartEnabled = TRUE;
                        InitializeRestart();
                    }
                    else
                    {
                        RestartEnabled = FALSE;
                        DisableRestart();
                    }
                    UpdateMenuState();
                    break;

                case IDM_ENABLE_RECOVERY:
                    if (!RecoveryEnabled)
                    {
                        RecoveryEnabled = TRUE;
                        InitializeRecovery();
                    }
                    else
                    {
                        RecoveryEnabled = FALSE;
                        DisableRecovery();
                    }
                    UpdateMenuState();
                    break;

                case IDM_ENABLE_REPORTING:
                    if (!ReportingEnabled)
                    {
                        ReportingEnabled = TRUE;
                        InitializeReporting();
                    }
                    else
                    {
                        ReportingEnabled = FALSE;
                        DisableReporting( TRUE );
                    }
                    UpdateMenuState();
                    break;

                case IDM_OPEN:
                    HandleFileOpenNew( TRUE );  // open existing file
                    break;

                case IDM_SAVE:
                    HandleFileSave( ActiveFilename );
                    break;

                case IDM_NEW:
                    HandleFileOpenNew( FALSE ); // open new file
                    break;

                // Default case for unimplemented menus
                case IDM_NO_OP:
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;

        case EN_CHANGE:
            // Update the menu state, based on buffer now being dirty
            if (IsDirty())
                UpdateMenuState();
            break;

        case WM_DESTROY:
            // Cleanup various sytem resources
            KillTimer( hWndMain, ID_CRASHTIMER );
            if ( INVALID_HANDLE_VALUE != hActiveFile )
                CloseHandle( hActiveFile );
            PostQuitMessage(0);
            break;

        case WM_WINDOWPOSCHANGED:
            FillParentWithChild( hWndMain, hWndEdit);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Fetches a reource string from the compiled resources
LPTSTR GetStringRes (int id)
{
    static WCHAR buffer[MAX_PATH];

    buffer[0]=0;
    LoadString (GetModuleHandle (NULL), id, buffer, MAX_PATH);
    return buffer;
}

// Intentionally cause the application to crash
// Note that we have to actually use the illegal arithmetic operation
// in a call or the compiler can and will optimize it away
VOID CrashApplication()
{
    
    int i = 0;

    // Expected: divide by zero exception
    DestroyWindow((HWND) (4/i));
}

// Hang the application, to invoke the OS' 5 second
// timeout hang rule
VOID HangApplication()
{
    while (TRUE) {}
}


// Setup the recovery and restart functionality
// NOTE: even if an application is registered for restarting, 
// the system will not restart the app after a crash unless the app
// has been running for at least 10 seconds - this is an attempt to 
// avoid endless startup crash/restart loops
VOID InitializeRestart()
{
    LPTSTR lpszCaption = L"Application Restart Registration";
    
    // NOTE: passing in NULL for the restart cmd-line parameter 
    // actually unregisters the application from restart.
    HRESULT res = RegisterApplicationRestart( ActiveFilename, 0 );

    if (FAILED(res))
    {
        DisplayMessage( lpszCaption, L"Application restart registration failed!" );
        return;
    }
    DisplayMessage( lpszCaption , L"Application has been registered to be restarted if it crashes.\n\nPlease allow at least 10 seconds to elapse before crashing application." );

    return;
}

// Unregister application from system's restart list
VOID DisableRestart()
{
    LPTSTR lpszCaption = L"Application Restart Registration";

    // Passing in NULL for the cmd-line for restart actually means
    // the app should be unregistered
    HRESULT res = RegisterApplicationRestart(NULL, 0);
    if (FAILED(res))
    {
        DisplayMessage( lpszCaption, L"Application could not be removed from the restart list." );
        return;
    }

    DisplayMessage( lpszCaption, L"Application has been removed from the restart list." );
    return;
}

// Setup recovery information and mechanisms
VOID InitializeRecovery()
{
    RegisterApplicationRecoveryCallback( (APPLICATION_RECOVERY_CALLBACK) SaveRecoveryDocument, NULL, recoveryPingInterval, recoveryOptionFlags );
    DisplayMessage( L"Application Restart", L"Application is now setup for document recovery." );
 
    return;
}

// Teardown recovery infrastructure
VOID DisableRecovery()
{
    UnregisterApplicationRecoveryCallback();
    DisplayMessage( L"Application Restart", L"Application will no longer retain document recovery information." );
    return;
}

// Initialize reporting by removing this app from exclusion list
VOID InitializeReporting()
{
    // Configure our reporting options - in our case, don't post
    // the heap to winqual, and let's attach the actual text file being edited
    WerSetFlags( WER_FAULT_REPORTING_FLAG_NOHEAP );
    WerRegisterFile( ActiveFilename, WerRegFileTypeUserDocument, 0 );

    // Make sure we are no longer on the "excluded" list
    WerRemoveExcludedApplication( EXENAME, FALSE );

    DisplayMessage( L"Error Reporting", L"Reporting has been enabled for this application." );
    return;
}

// Disable error reporting for this app by adding .exe name
// to exclusion list
VOID DisableReporting( BOOL ui )
{
    WerAddExcludedApplication( EXENAME, FALSE );
    if (ui)
        DisplayMessage( L"Error Reporting", L"Reporting has been disabled for this application." );
    return;
}

// Helper to update check/enabled states of various menu items
VOID UpdateMenuState()
{
    HMENU hMenu = GetMenu( hWndMain );
    CheckMenuItem( hMenu, IDM_ENABLE_RESTART, (RestartEnabled ? MF_CHECKED : MF_UNCHECKED) ); 
    CheckMenuItem( hMenu, IDM_ENABLE_RECOVERY, (RecoveryEnabled ? MF_CHECKED : MF_UNCHECKED) ); 
    CheckMenuItem( hMenu, IDM_ENABLE_REPORTING, (ReportingEnabled ? MF_CHECKED : MF_UNCHECKED) ); 
    EnableMenuItem( hMenu, IDM_CRASH_APP, (CrashTimerExpired ? MF_ENABLED : MF_GRAYED) ); 
    EnableMenuItem( hMenu, IDM_HANG_APP, (CrashTimerExpired ? MF_ENABLED : MF_GRAYED) ); 
    EnableMenuItem( hMenu, IDM_SAVE, ( IsDirty() ? MF_ENABLED : MF_GRAYED) ); 
    DrawMenuBar( hWndMain );

    return;
}

// Calculates location/size necessary for child to fill parent window
VOID FillParentWithChild( HWND hParent, HWND hChild )
{
    // Determine desired size for child control	
    RECT rClient;
    GetClientRect(hParent,&rClient);

    SetWindowPos( hChild, HWND_TOP, 0, 0, rClient.right, rClient.bottom, 0 );
    return;
}

// Called when 10 seconds have elapsed since application
// was registered from restarting
VOID CALLBACK ApplicationCrashTimerCallback(
    HWND hWnd,
    UINT message,
    UINT idTimer,
    DWORD dwTime)
{
    // Update the menu to reflect that crash application
    // will now achieve desired effect
    CrashTimerExpired = TRUE;
    UpdateMenuState();

    return;
}




// Core logic for opening an existing or new file
VOID HandleFileOpenNew( BOOL opening )
{
    HANDLE hTempFile;
    
    if (opening)
    {
        // Spin up the new Vista Open File dialog
        OPENFILENAME ofn;
        TCHAR szFile[MAX_PATH];
        
        ZeroMemory( &ofn, sizeof(ofn) );
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hWndMain;
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        
        ofn.nMaxFile = EDIT_BUF_SIZE;
        ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        
        if (GetOpenFileName(&ofn)==TRUE)
        {
            StringCchCopy(ActiveFilename, MAX_PATH, ofn.lpstrFile);
            
            hTempFile = CreateFile( ActiveFilename,
                                    GENERIC_READ,
                                    FILE_SHARE_WRITE,
                                    NULL, 
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL );
        }
        else
        {
            // User cancelled or closed the Open dialog box
            return;
        }
    }
    else
    {
        TCHAR szFile[MAX_PATH];
        int count = 1;
        
        do
        {
            // Generate Unique Filename
            StringCchPrintf(szFile, MAX_PATH, L"NewFile%d.txt", count++);
            
            hTempFile = CreateFile( szFile,
                                    GENERIC_READ,
                                    FILE_SHARE_WRITE,
                                    NULL, 
                                    CREATE_NEW,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL );
        } while (hTempFile == INVALID_HANDLE_VALUE);
        
        StringCchCopy(ActiveFilename, MAX_PATH, szFile);
    }
    
    if (hTempFile == INVALID_HANDLE_VALUE)
    {
        if (opening)
            DisplayError( L"File open failed" );
        else
            DisplayError( L"File creation failed" );
        return;
    }
    else
    {
        // Close any open file and reload buffer with new file
        if ( INVALID_HANDLE_VALUE != hActiveFile )
            CloseHandle( hActiveFile );
        hActiveFile = hTempFile;
        LoadEditor( hActiveFile );
    }
    
    // Update various state
    SetCaption( ActiveFilename );
    UpdateMenuState();
    CalculateRecoveryPath();
    
    // If we've already registered for restart, we need to re-register
    if (RestartEnabled)
        RegisterApplicationRestart( ActiveFilename, 0 );
    
    return;
}

// Loads the editor's backing buffer with contents of file
VOID LoadEditor( HANDLE hFrom )
{
    // Default encoding is ANSI
    BOOL isUnicode = false;
    
    // Clean editor buffer
    ZeroMemory(&EditorBuffer, EDIT_BUF_SIZE);
    
    // Read the first two bytes of the file to determine encoding
    DWORD bytesRead;
    if ( !ReadFile( hFrom, &EditorBuffer, 2, &bytesRead, 0) )
    {
        DisplayError( L"Unable to load file" );
        return;
    }
    
    // Move file pointer back to beginning of file
    SetFilePointer(hFrom, 0, NULL, FILE_BEGIN);
    
    // Check if file is unicode
    if (bytesRead > 0)
    {
        // UFT - 16
        if (EditorBuffer[0] == 0xFFFE || EditorBuffer[0] == 0xFEFF)
            isUnicode = true;
        
        // UTF - 32
        else if ((EditorBuffer[0] == 0xFFEE && EditorBuffer[1] == 0x0000) ||
             (EditorBuffer[0] == 0x0000 && EditorBuffer[1] == 0xFEFF)) 
        isUnicode = true;
        
        // UTF - 8
        else if (EditorBuffer[0] == 0xEFBB && EditorBuffer[1] & 0xFF00 == 0xBF00)
            isUnicode = true;
    }
    
    // Read file directly into editor buffer if the encoding is unicode
    if (isUnicode == true)
    {
        if ( !ReadFile( hFrom, &EditorBuffer, EDIT_BUF_SIZE, &bytesRead, 0) )
        {
            DisplayError( L"Unable to load file" );
            return;
        }
    }
    else
    {
        // Load into ANSI string
        CHAR LoadBuffer[EDIT_BUF_SIZE];
        ZeroMemory(&LoadBuffer, EDIT_BUF_SIZE);
        
        if ( !ReadFile( hFrom, &LoadBuffer, EDIT_BUF_SIZE, &bytesRead, 0) )
        {
            DisplayError( L"Unable to load file" );
            return;
        }
        
        // Convert ANSI to Unicode
        int lenLoadBuffer = lstrlenA(LoadBuffer);
        MultiByteToWideChar(CP_ACP, 0, LoadBuffer, lenLoadBuffer, EditorBuffer, lenLoadBuffer);
    }
    
    
    // Copy buffer into EDIT control's private backing buffer
    SendMessage( hWndEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)&EditorBuffer );
    
    return;
}

// Handles a file specified at the command line. It differs
// from HandleFileOpenNew in that it will open the file if it
// already exists, and creates a new file with that name if not 
// (i.e. there is no error condition)
VOID HandleInitialLoad()
{
    // Parse command line
    int nArgs;
    LPTSTR lpCmdLine = GetCommandLine();

    LPTSTR *szArgs = CommandLineToArgvW( lpCmdLine, &nArgs );
    
    if (nArgs > 2)
    {
        DisplayMessage( L"Error", L"Unexpected number of arguments" );
    }
    else if (nArgs > 1)
    {
        StringCchCopy(ActiveFilename, MAX_PATH, szArgs[1]);
    }
    else
    {
        StringCchCopy(ActiveFilename, MAX_PATH, READMEFILE);
    }

    // Open the actual file - either the default readme or the 
    // file specified at the command line. Note that the command line
    // could come from the actual user, or the restart logic
    hActiveFile = CreateFile( ActiveFilename, 
        GENERIC_READ,
        FILE_SHARE_WRITE,
        NULL, 
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL );
    if ( INVALID_HANDLE_VALUE == hActiveFile )
    {
        DisplayError( L"File could not be opened or created" );
        return;
    }

    // Now check if we need to do a recovery:
    // Figure out which file to open (or use a blank)
    // in the event of needing recovery
    CalculateRecoveryPath();

    // If there's no recovery file, load from the newly opened file
    if ( !ProcessRecoveryFile() )
        LoadEditor( hActiveFile );

    return;
}

BOOL ProcessRecoveryFile()
{
    HANDLE hRecoveryFile = INVALID_HANDLE_VALUE;

    // Test to see if there's a matching recovery file
    // for the currently opened file
    hRecoveryFile = CreateFile( (LPCTSTR) &RecoveryFullPath,
        GENERIC_READ | GENERIC_WRITE,
        0, NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL );
    
    // If a matching recovery file exists, load the editor with its contents instead,
    // then delete the recovery file
    if ( INVALID_HANDLE_VALUE != hRecoveryFile )
    {
        DisplayMessage( L"Document Recovery", L"Recovering document from previous SafePad session." );
        LoadEditor( hRecoveryFile );
        CloseHandle( hRecoveryFile );
        DeleteFile( (LPCTSTR)&RecoveryFullPath );
        return TRUE;
    }

    // No recovery file found
    return FALSE;
}

// Logic for saving a file (either for recovery or the main active file)
BOOL HandleFileSave( LPTSTR lpszFile )
{
    DWORD written;
    DWORD dwBufferSize = SendMessage( hWndEdit, WM_GETTEXT, (WPARAM) EDIT_BUF_SIZE, (LPARAM) &EditorBuffer ) * sizeof(TCHAR);

    // Pop open a new handle to our file, so we can overwrite everything
    HANDLE hSaveFile = CreateFile( lpszFile, 
        GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CREATE_ALWAYS, 0, NULL);
    if ( INVALID_HANDLE_VALUE == hSaveFile )
    {
        DisplayError( (LPTSTR)&RecoveryFullPath );
        return FALSE;
    }
    
    //TODO: spin up save dialog if hActiveFile is null

    // Write out the buffer
    if (!WriteFile( hSaveFile, &EditorBuffer, dwBufferSize, &written, 0))
    {
        DisplayError( L"Unable to write to file" );
        return FALSE;
    }
    CloseHandle( hSaveFile );
    return TRUE;
}

// Helper to check dirty state of edit control
BOOL IsDirty()
{
    return ( SendMessage( hWndEdit, EM_GETMODIFY, 0, 0 ) == 0 ? FALSE : TRUE );
}

// Creates new caption text based on active filename
VOID SetCaption( LPTSTR caption )
{
    TCHAR buffer[MAXCAPTION];
    LPTSTR pBuffer = (LPTSTR)&buffer;

    // Note: ignoring HRESULTS - truncated caption is worst case
    StringCchCopy( pBuffer, MAXCAPTION, caption );
    StringCchCat( pBuffer, MAXCAPTION, L" - SafePad" );

    SetWindowText( hWndMain, pBuffer );
}

// Creates and sets up the editor control
BOOL CreateEditor( HWND parent )
{
    // Create and position editor control
    DWORD dwEditStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE;
    hWndEdit = CreateWindow(TEXT("edit"), NULL, dwEditStyle, 
        0, 0, 0, 0,
        parent, 0, hInstance, NULL);
    FillParentWithChild( parent, hWndEdit );

    return TRUE;
}

// Callback called by the OS when the app has "crashed" - either encountered
// an unhandled exception or hung
DWORD SaveRecoveryDocument( PVOID pParam )
{
    BOOL saved;

    // First, kill any pending IO operations, in case this is getting called
    // in case of a hang; note that specifically in the case of hangs (and 
    // certain fatal exceptions like stack overflow) the calling thread
    // will NOT be the main thread
    CancelSynchronousIo( hMainThread );

    // Ensure that the recovery path actually exists (CreateFile will
    // fail otherwise). Note we use the 'Ex' variant to force
    // all intermediate folders to be created as well
    SHCreateDirectoryEx( hWndMain, (LPCTSTR)&RecoveryFolderPath, NULL );
    saved = HandleFileSave( (LPTSTR)&RecoveryFullPath );

    // Tell OS whether recovery failed or suceeded
    if (!saved)
        ApplicationRecoveryFinished( FALSE );
    else
        ApplicationRecoveryFinished( TRUE );
    CloseHandle( hActiveFile );
    return 0;
}

// Create path for recovery file, in the form
// %User%\%Documents%\SafePad Recovery\<filename>.recover
VOID CalculateRecoveryPath()
{
    LPTSTR rootKnownFolderPath;

    // We use the new Vista KnownFolder APIs to
    // fetch the path for 'My Documents', passing a null
    // user token to indicate we want the current user's folder set
    HRESULT res = SHGetKnownFolderPath( FOLDERID_Documents, 0, NULL, 
        &rootKnownFolderPath);
    if (FAILED(res))
    {
        // Make sure it's an empty string - we'll use the
        // current directory
        RecoveryFullPath[0] = '\0';
        return;
    }

    // Build-up recovery folder and full paths
    StringCchPrintf( RecoveryFolderPath, MAX_PATH, L"%s\\SafePad Recovery", rootKnownFolderPath );
    StringCchPrintf( RecoveryFullPath, MAX_PATH, L"%s\\%s.recover", &RecoveryFolderPath, ActiveFilename );
    CoTaskMemFree(rootKnownFolderPath);

    return;
}

VOID DisplayMessage( LPTSTR caption, LPTSTR text )
{
    MessageBox( hWndMain, text, caption, MB_OK );
    return;
}

VOID DisplayError( LPTSTR context )
{
    TCHAR szCompleteMsg[MAXERRMSG];
    LPTSTR lpMsgBuf;
    DWORD errcode = GetLastError();
    
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, errcode, 0,
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    StringCchPrintf( szCompleteMsg, MAXERRMSG, L"%s: %s", context, lpMsgBuf );
    DisplayMessage( L"Error" , szCompleteMsg );
    LocalFree(lpMsgBuf);

    return;
}

