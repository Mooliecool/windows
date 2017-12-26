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
#include <rotor_pal.h>
#include <palstartup.h>

#define BINPLACE_ERR 77
#define BINPLACE_OK 0

BOOL fUsage;
BOOL fVerbose;
BOOL fTestMode;
BOOL fSetupModeAllFiles;
BOOL fSetupModeScriptFile;
BOOL fKeepAttributes;
BOOL fDontLog;
BOOL fMakeErrorOnDumpCopy;
BOOL fDontExit;
BOOL fSignCode;
BOOL fChangeAsmsToRetailForSymbols;
BOOL fSrcControl;
BOOL fDbgControl;


LPSTR CurrentImageName;
LPSTR PlaceFileName;
LPSTR PlaceRootName;
LPSTR DumpOverride;
LPSTR NormalPlaceSubdir;
LPSTR CommandScriptName;
LPSTR SymbolFilePath;
LPSTR PrivateSymbolFilePath;
LPSTR BinplaceLcDir;
LPSTR LcFilePart;

FILE *PlaceFile;
FILE *CommandScriptFile;
CHAR* gDelayLoadModule;
CHAR* gDelayLoadHandler;
CHAR gFullFileName[MAX_PATH+1];
UCHAR SetupFilePath[ MAX_PATH+1 ];
UCHAR DebugFilePath[ MAX_PATH+1 ];
UCHAR PlaceFilePath[ MAX_PATH+1 ];
UCHAR DefaultSymbolFilePath[ MAX_PATH+1 ];
UCHAR szAltPlaceRoot[ MAX_PATH+1 ];


struct {
    int   RC;
    CHAR  **Argv;
    int   Argc;
} ImageCheck;

#define DEFAULT_PLACE_FILE    "\\public\\sdk\\lib\\placefil.txt"
#define DEFAULT_NTROOT        "\\nt"
#define DEFAULT_NTDRIVE       "c:"
#define DEFAULT_DUMP          "dump"
#define DEFAULT_LCDIR         "LcINF"

typedef struct _CLASS_TABLE {
    LPSTR ClassName;
    LPSTR ClassLocation;
} CLASS_TABLE, *PCLASS_TABLE;

BOOL
PlaceTheFile();


BOOL
CopyTheFile(
           LPSTR SourceFileName,
           LPSTR SourceFilePart,
           LPSTR DestinationSubdir,
           LPSTR DestinationFilePart
           );

BOOL
BinplaceCopyPdb (
                LPSTR DestinationFile,
                LPSTR SourceFileName,       // Used for redist case
                BOOL  CopyFromSourceOnly,
                BOOL  StripPrivate
                );
BOOL
BinplaceCopyIldb (
                 LPSTR DestinationFile,
                 LPSTR SourceFileName,       // Used for redist case
                 BOOL  CopyFromSourceOnly,
                 BOOL  StripPrivate
                 );

BOOL
BinplaceCopyManifest (
                 LPSTR DestinationFile,
                 LPSTR SourceFileName,       // Used for redist case
                 BOOL  CopyFromSourceOnly,
                 BOOL  StripPrivate
                 );


BOOL
SourceIsNewer(
             IN LPSTR SourceFile,
             IN LPSTR TargetFile
             );

BOOL
SetupModeRetailFile(
                   IN  LPSTR FullFileName,
                   IN  LPSTR FileNamePart,
                   OUT PBOOL PutInDump
                   );

__inline BOOL
SearchOneDirectory(
                  IN  LPSTR Directory,
                  IN  LPSTR FileToFind,
                  IN  LPSTR SourceFullName,
                  IN  LPSTR SourceFilePart,
                  OUT PBOOL FoundInTree
                  )
{
    //
    // This was way too slow. Just say we didn't find the file.
    //
    *FoundInTree = FALSE;
    return(TRUE);
}

BOOL
FileExists(
          IN  LPCSTR FileName,
          OUT PWIN32_FIND_DATA FindData
          );



CLASS_TABLE CommonClassTable[] = {
    {"retail",  "."},
    {"windows", "."},
    {NULL,NULL}
};

CLASS_TABLE i386SpecificClassTable[] = {
    {"hal","system32"},
    {"printer","system32\\spool\\drivers\\w32x86"},
    {"prtprocs","system32\\spool\\prtprocs\\w32x86"},
    {NULL,NULL}
};


CLASS_TABLE ia64SpecificClassTable[] = {
    {"hal",".."},
    {"printer","system32\\spool\\drivers\\w32ia64"},
    {"prtprocs","system32\\spool\\prtprocs\\w32ia64"},
    {NULL,NULL}
    };

//
// Names of sections in layout.inx
//
LPCSTR szSourceDisksFiles = "SourceDisksFiles";
LPCSTR szSourceDisksX86   = "SourceDisksFiles.x86";
LPCSTR szSourceDisksIA64  = "SourceDisksFiles.ia64";

typedef struct _PLACE_FILE_RECORD {
    LPSTR FileNameEntry;
    LPSTR FileClass;
} PLACE_FILE_RECORD, *PPLACE_FILE_RECORD;

int MaxNumberOfRecords;
int NumberOfRecords;
PPLACE_FILE_RECORD PlaceFileRecords;

int __cdecl
pfcomp(
      const void *e1,
      const void *e2
      )
{
    PPLACE_FILE_RECORD p1;
    PPLACE_FILE_RECORD p2;

    p1 = (PPLACE_FILE_RECORD)e1;
    p2 = (PPLACE_FILE_RECORD)e2;

    return (strcmp(p1->FileNameEntry,p2->FileNameEntry));
}

CHAR PlaceFileDir[4096];
CHAR PlaceFileClass[4096];
CHAR PlaceFileEntry[4096];

BOOL
SortPlaceFileRecord()
{
    int cfield;
    PPLACE_FILE_RECORD NewRecords;

    NumberOfRecords = 0;
    MaxNumberOfRecords = 0;

    //
    // get space for 6k records. Grow if need to.
    //
    MaxNumberOfRecords = 7000;
    PlaceFileRecords = (PPLACE_FILE_RECORD) malloc( sizeof(*PlaceFileRecords)*MaxNumberOfRecords );
    if ( !PlaceFileRecords ) {
        return FALSE;
    }

    if (fseek(PlaceFile,0,SEEK_SET)) {
        free(PlaceFileRecords);
        PlaceFileRecords = NULL;
        return FALSE;
    }
    
    while (fgets(PlaceFileDir,sizeof(PlaceFileDir),PlaceFile)) {

        PlaceFileEntry[0] = '\0';
        PlaceFileClass[0] = '\0';

        cfield = sscanf(
                       PlaceFileDir,
//                       "%s %[A-Za-z0-9.,_!@#\\$+=%^&()~ -]s",
                       "%s %s",
                       PlaceFileEntry,
                       PlaceFileClass
                       );

        if (cfield <= 0 || PlaceFileEntry[0] == ';') {
            continue;
        }

        PlaceFileRecords[NumberOfRecords].FileNameEntry = (LPSTR) malloc( strlen(PlaceFileEntry)+1 );
        PlaceFileRecords[NumberOfRecords].FileClass = (LPSTR) malloc( strlen(PlaceFileClass)+1 );
        if (!PlaceFileRecords[NumberOfRecords].FileClass || !PlaceFileRecords[NumberOfRecords].FileNameEntry) {
            if (PlaceFileRecords[NumberOfRecords].FileClass)
                free(PlaceFileRecords[NumberOfRecords].FileClass);
            if (PlaceFileRecords[NumberOfRecords].FileNameEntry)
                free(PlaceFileRecords[NumberOfRecords].FileNameEntry);
            free(PlaceFileRecords);
            PlaceFileRecords = NULL;
            return FALSE;
        }
        strcpy(PlaceFileRecords[NumberOfRecords].FileNameEntry,PlaceFileEntry);
        strcpy(PlaceFileRecords[NumberOfRecords].FileClass,PlaceFileClass);
        NumberOfRecords++;
        if ( NumberOfRecords >= MaxNumberOfRecords ) {
            MaxNumberOfRecords += 200;
            NewRecords = (PPLACE_FILE_RECORD) realloc(
                                                     PlaceFileRecords,
                                                     sizeof(*PlaceFileRecords)*MaxNumberOfRecords
                                                     );
            if ( !NewRecords ) {
                PlaceFileRecords = NULL;
                return FALSE;
            }
            PlaceFileRecords = NewRecords;
        }
    }
    qsort((void *)PlaceFileRecords,(size_t)NumberOfRecords,(size_t)sizeof(*PlaceFileRecords),pfcomp);
    return TRUE;
}

PPLACE_FILE_RECORD
LookupPlaceFileRecord(
                     LPSTR FileName
                     )
{
    LONG High;
    LONG Low;
    LONG Middle = 0;
    LONG Result;

    //
    // Lookup the name using a binary search.
    //

    if ( !PlaceFileRecords ) {
        return NULL;
    }

    Low = 0;
    High = NumberOfRecords - 1;
    while (High >= Low) {

        //
        // Compute the next probe index and compare the import name
        // with the export name entry.
        //

        Middle = (Low + High) >> 1;
        Result = _stricmp(FileName, PlaceFileRecords[Middle].FileNameEntry);

        if (Result < 0) {
            High = Middle - 1;

        } else if (Result > 0) {
            Low = Middle + 1;

        } else {
            break;
        }
    }

    if (High < Low) {
        return NULL;
    } else {
        return &PlaceFileRecords[Middle];
    }
}

BOOL
MakeSureDirectoryPathExists(
    LPCSTR DirPath
    )
{
    LPSTR p, DirCopy;
    DWORD dw;

    // Make a copy of the string for editing.

    DirCopy = (LPSTR)malloc(strlen(DirPath) + 1);

    if (!DirCopy) {
        return FALSE;
    }

    strcpy(DirCopy, DirPath);

    p = DirCopy;

    //  If the second character in the path is "\", then this is a UNC
    //  path, and we should skip forward until we reach the 2nd \ in the path.

    if ((*p == '\\') && (*(p+1) == '\\')) {
        p++;            // Skip over the first \ in the name.
        p++;            // Skip over the second \ in the name.

        //  Skip until we hit the first "\" (\\Server\).

        while (*p && *p != '\\') {
            p = CharNext(p);
        }

        // Advance over it.

        if (*p) {
            p++;
        }

        //  Skip until we hit the second "\" (\\Server\Share\).

        while (*p && *p != '\\') {
            p = CharNext(p);
        }

        // Advance over it also.

        if (*p) {
            p++;
        }

    } else
    // Not a UNC.  See if it's <drive>:
    if (*(p+1) == ':' ) {

        p++;
        p++;

        // If it exists, skip over the root specifier

        if (*p && (*p == '\\')) {
            p++;
        }
    }

    while( *p ) {
        if ( *p == '\\' ) {
            *p = '\0';
            dw = GetFileAttributesA(DirCopy);
            // Nothing exists with this name.  Try to make the directory name and error if unable to.
            if ( dw == 0xffffffff ) {
                if ( !CreateDirectory(DirCopy,NULL) ) {
                    if( GetLastError() != ERROR_ALREADY_EXISTS ) {
                        free(DirCopy);
                        return FALSE;
                    }
                }
            } else {
                if ( (dw & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY ) {
                    // Something exists with this name, but it's not a directory... Error
                    free(DirCopy);
                    return FALSE;
                }
            }

            *p = '\\';
        }
        p = CharNext(p);
    }

    free(DirCopy);
    return TRUE;
}


int __cdecl
main(
    int argc,
    char **argv
    )
{
    char c, *p, *OverrideFlags, *s, **newargv;
    int i, n;

    fUsage = FALSE;
    fVerbose = FALSE;
    fTestMode = FALSE;
    fKeepAttributes = FALSE;
    fDontExit = FALSE;
    fSrcControl = FALSE;
    fDbgControl = FALSE;
    NormalPlaceSubdir = NULL;

    if (argc < 2) {
        goto showUsage;
    }


    if (!(PlaceFileName = getenv( "BINPLACE_PLACEFILE" ))) {
        if ((PlaceFileName = getenv("_NTDRIVE")) == NULL) {
            PlaceFileName = DEFAULT_NTDRIVE;
        }
        strcpy((PCHAR) PlaceFilePath, PlaceFileName);
        if ((PlaceFileName = getenv("_NTROOT")) == NULL) {
            PlaceFileName = DEFAULT_NTROOT;
        }
        strcat((PCHAR) PlaceFilePath, PlaceFileName);
        strcat((PCHAR) PlaceFilePath, DEFAULT_PLACE_FILE);
        PlaceFileName = (PCHAR) PlaceFilePath;
    }


    if (!(BinplaceLcDir = getenv( "BINPLACE_LCDIR" ))) {
        BinplaceLcDir = DEFAULT_LCDIR;
    }



    CurrentImageName = NULL;

    OverrideFlags = getenv( "BINPLACE_OVERRIDE_FLAGS" );
    if (OverrideFlags != NULL) {
        s = OverrideFlags;
        n = 0;
        while (*s) {
            while (*s && *s <= ' ')
                s += 1;
            if (*s) {
                n += 1;
                while (*s > ' ')
                    s += 1;

                if (*s)
                    *s++ = '\0';
            }
        }

        if (n) {
            newargv = malloc( (argc + n + 1) * sizeof( char * ) );
            memcpy( &newargv[n], argv, argc * sizeof( char * ) );
            argv = newargv;
            argv[ 0 ] = argv[ n ];
            argc += n;
            s = OverrideFlags;
            for (i=1; i<=n; i++) {
                while (*s && *s <= ' ')
                    s += 1;
                argv[ i ] = s;
                while (*s++)
                    ;
            }
        }
    }

    while (--argc) {
        p = *++argv;
        if (*p == '-') {
            if (_stricmp(p + 1, "ChangeAsmsToRetailForSymbols") == 0) {
                fChangeAsmsToRetailForSymbols = TRUE;
            } else {
              while ((c = *++p))
                switch (toupper( c )) {
                    case '?':
                        fUsage = TRUE;
                        break;


                    case 'B':
                        argc--, argv++;
                        NormalPlaceSubdir = *argv;
                        break;

                    case 'C':
                        if (*(p+1) == 'I' || *(p+1) == 'i') {
                            char *q;

                            argc--, argv++;
                            p = *argv;
                            ImageCheck.RC = atoi(p);
                            if (!ImageCheck.RC) {
                                fprintf( stderr, "BINPLACE : error BNP0000: Invalid return code for -CI option\n");
                                fUsage = TRUE;
                            }
                            while (*p++ != ',');
                            q = p;
                            ImageCheck.Argc = 0;
                            while (*p != '\0')
                                if (*p++ == ',') ImageCheck.Argc++;
                            // last option plus extra args for Image file and Argv NULL
                            ImageCheck.Argc += 3;
                            ImageCheck.Argv = malloc( ImageCheck.Argc * sizeof( void * ) );
                            for ( i = 0; i <= ImageCheck.Argc - 3; i++) {
                                ImageCheck.Argv[i] = q;
                                while (*q != ',' && *q != '\0') q++;
                                *q++ = '\0';
                            }
                            p--;
                            ImageCheck.Argv[ImageCheck.Argc-1] = NULL;
                        } else {
                        }
                        break;

                    case 'D':
                        {
                            argc--, argv++;
                            DumpOverride = *argv;
                        }
                        break;

                    case 'E':
                        fDontExit = TRUE;
                        break;

                    case 'F':
                        break;


                    case 'K':
                        fKeepAttributes = TRUE;
                        break;


                    case 'M':
                        fMakeErrorOnDumpCopy = TRUE;
                        break;

                    case 'N':
                        argc--, argv++;
                        break;

                    case 'O':
                        argc--, argv++;
                        if (PlaceRootName != NULL) {
                            strcpy(szAltPlaceRoot,PlaceRootName);
                            strcat(szAltPlaceRoot,"\\");
                            strcat(szAltPlaceRoot,*argv);
                            PlaceRootName = szAltPlaceRoot;
                        }
                        break;

                    case 'P':
                        argc--, argv++;
                        PlaceFileName = *argv;
                        break;

                    case 'Q':
                        fDontLog = TRUE;
                        break;

                    case 'R':
                        argc--, argv++;
                        PlaceRootName = *argv;
                        break;

                    case 'S':
                        argc--, argv++;
                        SymbolFilePath = *argv;
                        break;

                    case 'T':
                        fTestMode = TRUE;
                        break;


                    case 'V':
                        fVerbose = TRUE;
                        break;

                    case 'W':
                        break;

                    case 'X':
                        break;

                    case 'Y':
                        break;



                    default:
                        fprintf( stderr, "BINPLACE : error BNP0000: Invalid switch - /%c\n", c );
                        fUsage = TRUE;
                        break;
                }
            }

            if ( fUsage ) {
                showUsage:
                fputs(
                     "usage: binplace [switches] image-names... \n"
                     "where: [-?] display this message\n"
                     "       [-b subdir] put file in subdirectory of normal place\n"
                     "       [-d dump-override]\n"
                     "       [-e] don't exit if a file in list could not be binplaced\n"
                     "       [-k] keep attributes (don't turn off archive)\n"
                     "       [-o place-root-subdir] alternate project subdirectory\n"
                     "       [-p place-file]\n"
                     "       [-q] suppress writing to log file %BINPLACE_LOG%\n"
                     "       [-r place-root]\n"
                     "       [-s Symbol file path] split symbols from image files\n"
                     "       [-t] test mode\n"
                     "       [-v] verbose output\n"
                     "       [-ci <rc,app,-arg0,-argv1,-argn>]\n"
                     "            rc=application error return code,\n"
                     "            app=application used to check images,\n"
                     "            -arg0..-argn=application options\n"
                     "\n"
                     "BINPLACE looks for the following environment variable names:\n"
                     "   BINPLACE_OVERRIDE_FLAGS - may contain additional switches\n"
                     "   BINPLACE_PLACEFILE - default value for -p flag\n"
                     "\n"
                     ,stderr
                     );

                exit(BINPLACE_ERR);
            }
        } else {
            WIN32_FIND_DATA FindData;
            HANDLE h;

            if (!PlaceRootName) {
                // If there's no root, just exit.
                exit(BINPLACE_OK);
            }


            h = FindFirstFile(p,&FindData);
            if (h != INVALID_HANDLE_VALUE) {
                FindClose(h);
                if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    if ( fVerbose ) {
                        fprintf(stdout,"BINPLACE : warning BNP0000: ignoring directory %s\n",p);
                    }
                    continue;
                }
            }

            CurrentImageName = p;


            //
            // If the master place file has not been opened, open
            // it up.
            //

            if ( !PlaceFile ) {
                PlaceFile = fopen(PlaceFileName, "rt");
                if (!PlaceFile) {
                    fprintf(stderr,"BINPLACE : fatal error BNP0000: fopen of placefile %s failed %d\n",PlaceFileName,GetLastError());
                    exit(BINPLACE_ERR);
                }
            }


            if (PlaceRootName == NULL) {
                fprintf(stderr,"BINPLACE : fatal error BNP0000: Place Root not defined - exiting.\n");
                exit(BINPLACE_ERR);
            }

            // If the SymbolFilePath has not been set, make a default value.
            if (!SymbolFilePath) {
                strcpy(DefaultSymbolFilePath, PlaceRootName);
                strcat(DefaultSymbolFilePath, "\\symbols");
                SymbolFilePath = DefaultSymbolFilePath;
            }

            if ( !PlaceTheFile() ) {
                if (fDontExit) {
                    fprintf(stderr,"BINPLACE : error BNP0000: Unable to place file %s.\n",CurrentImageName);
                } else {
                    fprintf(stderr,"BINPLACE : fatal error BNP0000: Unable to place file %s - exiting.\n",CurrentImageName);
                    exit(BINPLACE_ERR);
                }
            }
        }
    }

    exit(BINPLACE_OK);
    return BINPLACE_OK;
}

BOOL
PlaceTheFile()
{
    CHAR FullFileName[MAX_PATH+1];
    LPSTR PlaceFileNewName;
    LPSTR FilePart;
    LPSTR Separator;
    LPSTR PlaceFileClassPart;
    DWORD cb;
    int cfield;
    PCLASS_TABLE ClassTablePointer;
    BOOLEAN ClassMatch;
    BOOL    fCopyResult = FALSE;
    LPSTR Extension;
    BOOL PutInDebug = FALSE;
    BOOL PutInLcDir = FALSE;

    cb = GetFullPathName(CurrentImageName,MAX_PATH+1,FullFileName,&FilePart);

    if (!cb || cb > MAX_PATH+1) {
        fprintf(stderr,"BINPLACE : fatal error BNP0000: GetFullPathName failed %d\n",GetLastError());
        return FALSE;
    }


    if (fVerbose) {
        fprintf(stdout,"BINPLACE : warning BNP0000: Looking at file %s\n",FilePart);
    }

    Extension = strrchr(FilePart,'.');
    if (Extension) {
        if (!_stricmp(Extension,".DBG")) {
            PutInDebug = TRUE;
        }
        else if (!_stricmp(Extension,".LC")) {
            PutInLcDir = TRUE;
        }
    }

    if (!DumpOverride) {

        if (fseek(PlaceFile,0,SEEK_SET)) {
            fprintf(stderr,"BINPLACE : fatal error BNP0000: fseek(PlaceFile,0,SEEK_SET) failed %d\n",GetLastError());
            return FALSE;
        }
        while (fgets(PlaceFileDir,sizeof(PlaceFileDir),PlaceFile)) {

            PlaceFileEntry[0] = '\0';
            PlaceFileClass[0] = '\0';

            cfield = sscanf(
                           PlaceFileDir,
//                           "%s %[A-Za-z0-9.,_!@#\\$+=%^&()~ -]s",
                           "%s %s",
                           PlaceFileEntry,
                           PlaceFileClass
                           );

            if (cfield <= 0 || PlaceFileEntry[0] == ';') {
                continue;
            }

            if ((PlaceFileNewName = strchr(PlaceFileEntry,'!'))) {
                *PlaceFileNewName++ = '\0';
            }

            if (!_stricmp(FilePart,PlaceFileEntry)) {
                //
                // now that we have the file and class, search the
                // class tables for the directory.
                //
                Separator = PlaceFileClass - 1;
                while (Separator) {

                    PlaceFileClassPart = Separator+1;
                    Separator = strchr(PlaceFileClassPart,':');
                    if (Separator) {
                        *Separator = '\0';
                    }

                    //
                    // If the class is "retail" and we're in Setup mode,
                    // handle this file specially. Setup mode is used to
                    // incrementally binplace files into an existing installation.
                    //
                    SetupFilePath[0] = '\0';

                    PlaceFileDir[0]='\0';
                    ClassMatch = FALSE;
                    ClassTablePointer = &CommonClassTable[0];
                    while (ClassTablePointer->ClassName) {
                        if (!_stricmp(ClassTablePointer->ClassName,PlaceFileClassPart)) {
                            strcpy(PlaceFileDir,ClassTablePointer->ClassLocation);
                            ClassMatch = TRUE;

                            break;
                        }

                        ClassTablePointer++;
                    }

                    if (!ClassMatch) {
                        //
                        // Search Specific classes
                        //
                        // We need to support cross compiling here.
                        LPTSTR platform;

                        ClassTablePointer = &i386SpecificClassTable[0];
                        if ((platform = getenv("IA64")) != NULL) {
                            ClassTablePointer = &ia64SpecificClassTable[0];
                        }
                        while (ClassTablePointer->ClassName) {

                            if (!_stricmp(ClassTablePointer->ClassName,PlaceFileClassPart)) {
                                strcpy(PlaceFileDir,ClassTablePointer->ClassLocation);
                                ClassMatch = TRUE;
                                break;
                            }

                            ClassTablePointer++;
                        }
                    }

                    if (!ClassMatch) {

                        char * asterisk;

                        //
                        // Still not found in class table. Use the class as the
                        // directory
                        //

                        if ( fVerbose ) {
                            fprintf(stderr,"BINPLACE : warning BNP0000: Class %s Not found in Class Tables\n",PlaceFileClassPart);
                        }
                        if ((asterisk = strchr(PlaceFileClassPart, '*'))) {
                            //
                            // Expand * to platform
                            //
                            LPTSTR platform;
                            ULONG PlatformSize;
                            LPTSTR PlatformPath;

                            PlatformSize = 4;
                            PlatformPath = TEXT("i386");
                            if ((platform = getenv("IA64")) != NULL) {
                                PlatformPath = TEXT("ia64");
                            } else if ((platform = getenv("AMD64")) != NULL) {
                                PlatformSize = 5;
                                PlatformPath = TEXT("amd64");
                            }

                            strncpy(PlaceFileDir,PlaceFileClassPart, (int)(asterisk - PlaceFileClassPart));
                            strcpy(PlaceFileDir + (asterisk - PlaceFileClassPart), PlatformPath);
                            strcpy(PlaceFileDir + (asterisk - PlaceFileClassPart) + PlatformSize, asterisk + 1);

                        } else {
                            strcpy(PlaceFileDir,PlaceFileClassPart);
                        }
                    }

                    if (SetupFilePath[0] == '\0') {
                        strcpy(SetupFilePath, PlaceFileDir);
                        strcat(SetupFilePath, "\\");
                        strcat(SetupFilePath, FilePart);
                    }

                    if (NormalPlaceSubdir) {
                        strcat(PlaceFileDir,"\\");
                        strcat(PlaceFileDir,NormalPlaceSubdir);
                    }

                    fCopyResult = CopyTheFile(FullFileName,FilePart,PlaceFileDir,PlaceFileNewName);
                    if (!fCopyResult) {
                        break;
                    }
                }

                return(fCopyResult);
            }
        }
    }

    if (fMakeErrorOnDumpCopy) {
        fprintf(stderr, "BINPLACE : error BNP0000: File '%s' is not listed in '%s'. Copying to dump.\n", FullFileName, PlaceFileName);
    }

    return CopyTheFile(
               FullFileName,
               FilePart,
               PutInDebug ? "Symbols" : (PutInLcDir ? BinplaceLcDir : (DumpOverride ? DumpOverride : DEFAULT_DUMP)),
               NULL
               );

}

BOOL
CopyTheFile(
           LPSTR SourceFileName,
           LPSTR SourceFilePart,
           LPSTR DestinationSubdir,
           LPSTR DestinationFilePart
           )
{
    CHAR DestinationFile[MAX_PATH+1];
    CHAR TmpDestinationDir[MAX_PATH];
    CHAR *TmpSymbolFilePath;

    if ( !PlaceRootName ) {
        fprintf(stderr,"BINPLACE : warning BNP0000: PlaceRoot is not specified\n");
        return FALSE;
    }


    strcpy(DestinationFile,PlaceRootName);
    strcat(DestinationFile,"\\");
    strcat(DestinationFile,DestinationSubdir);
    strcat(DestinationFile,"\\");

    strcpy (TmpDestinationDir, DestinationFile);


    if (!MakeSureDirectoryPathExists(DestinationFile)) {
        fprintf(stderr, "BINPLACE : error BNP0000: Unable to create directory path '%s' (%u)\n",
                DestinationFile, GetLastError()
               );
    }

    if (DestinationFilePart) {
        strcat(DestinationFile,DestinationFilePart);
    } else {
        strcat(DestinationFile,SourceFilePart);
    }

    if (
        (fVerbose || fTestMode)) {
        fprintf(stdout,"BINPLACE : warning BNP0000: place %s in %s\n",SourceFileName,DestinationFile);
    }

    {
        if (SourceIsNewer(SourceFileName,DestinationFile)) {
            fprintf(stdout, "binplace %s\n", SourceFileName);
        }
    }

    if (!fTestMode) {
        //
        // In Setup mode, copy the file only if it's newer than
        // the one that's already there.
        //
            if (SourceIsNewer(SourceFileName,DestinationFile)) {
                if (fVerbose) {
                    fprintf(stdout,"BINPLACE : warning BNP0000: copy %s to %s\n",SourceFileName,DestinationFile);
                }
            } else {
                return(TRUE);
            }

        SetFileAttributes(DestinationFile,FILE_ATTRIBUTE_NORMAL);


        if ( !CopyFile(SourceFileName,DestinationFile, FALSE)) {
            fprintf(stderr,"BINPLACE : warning BNP0000: CopyFile(%s,%s) failed %d\n",SourceFileName,DestinationFile,GetLastError());

            return FALSE;
        }

        if (!fKeepAttributes)
            SetFileAttributes(DestinationFile,FILE_ATTRIBUTE_NORMAL);

        {
            TmpSymbolFilePath = SymbolFilePath;
        }

        BinplaceCopyPdb(TmpSymbolFilePath, SourceFileName, TRUE, FALSE);
        // Copy the ILDB to be next to the exe
        BinplaceCopyIldb(TmpDestinationDir, SourceFileName, TRUE, FALSE);
        // Copy the manifest to be next to the exe
        BinplaceCopyManifest(TmpDestinationDir, SourceFileName, TRUE, FALSE);

    } else {
    }

    return TRUE;
}



BOOL
SourceIsNewer(
             IN LPSTR SourceFile,
             IN LPSTR TargetFile
             )
{
    return TRUE;
}




BOOL
BinplaceCopyPdb (
                LPSTR DestinationFile,
                LPSTR SourceFileName,
                BOOL CopyFromSourceOnly,
                BOOL StripPrivate
                )
{
    CHAR Drive[_MAX_DRIVE];
    CHAR Dir[_MAX_DIR];
    CHAR Filename[_MAX_FNAME];
    CHAR Ext[_MAX_EXT];
    CHAR SourcePdbName[_MAX_PATH];
    CHAR DestinationPdbName[_MAX_PATH];

    _splitpath(SourceFileName, Drive, Dir, Filename, Ext);
    _makepath(SourcePdbName, Drive, Dir, Filename, ".pdb");
    if (GetFileAttributesA(SourcePdbName) != 0xffffffff) {
        //
        // Source file exists
        //
        strcpy(DestinationPdbName, DestinationFile);
        strcat(DestinationPdbName, "\\");
        strcat(DestinationPdbName, &Ext[1]);
        strcat(DestinationPdbName, "\\");
        strcat(DestinationPdbName, Filename);
        strcat(DestinationPdbName, ".pdb");

        if (fVerbose || fTestMode) {
            fprintf(stdout,"BINPLACE : warning BNP0000: place %s in %s\n", SourcePdbName, DestinationPdbName);
        }

        if (!MakeSureDirectoryPathExists(DestinationPdbName)) {
            return FALSE;
        }
        if (!CopyFileA(SourcePdbName, DestinationPdbName, FALSE)) {
            return FALSE;
        }
        SetFileAttributes(DestinationPdbName,FILE_ATTRIBUTE_NORMAL);
        return TRUE;
    }
    return FALSE;
}

BOOL
BinplaceCopyIldb (
                LPSTR DestinationFile,
                LPSTR SourceFileName,
                BOOL CopyFromSourceOnly,
                BOOL StripPrivate
                )
{
    CHAR Drive[_MAX_DRIVE];
    CHAR Dir[_MAX_DIR];
    CHAR Filename[_MAX_FNAME];
    CHAR Ext[_MAX_EXT];
    CHAR SourceIldbName[_MAX_PATH];
    CHAR DestinationIldbName[_MAX_PATH];

    _splitpath(SourceFileName, Drive, Dir, Filename, Ext);
    _makepath(SourceIldbName, Drive, Dir, Filename, ".ildb");
    if (GetFileAttributesA(SourceIldbName) != 0xffffffff) {
                //
                // Source file exists
                //
                strcpy(DestinationIldbName, DestinationFile);
                strcat(DestinationIldbName, "\\");
                strcat(DestinationIldbName, Filename);
                strcat(DestinationIldbName, ".ildb");

        if (fVerbose || fTestMode) {
            fprintf(stdout,"BINPLACE : warning BNP0000: place %s in %s\n", SourceIldbName, DestinationIldbName);
        }

        if (!MakeSureDirectoryPathExists(DestinationIldbName)) {
            return FALSE;
        }
        if (!CopyFileA(SourceIldbName, DestinationIldbName, FALSE)) {
            return FALSE;
        }
        SetFileAttributes(DestinationIldbName,FILE_ATTRIBUTE_NORMAL);
        return TRUE;
    }
    return FALSE;
}

BOOL
BinplaceCopyManifest (
                LPSTR DestinationFile,
                LPSTR SourceFileName,
                BOOL CopyFromSourceOnly,
                BOOL StripPrivate
                )
{
    CHAR Drive[_MAX_DRIVE];
    CHAR Dir[_MAX_DIR];
    CHAR Filename[_MAX_FNAME];
    CHAR Ext[_MAX_EXT];
    CHAR SourceManifestName[_MAX_PATH];
    CHAR DestinationManifestName[_MAX_PATH];

    _splitpath(SourceFileName, Drive, Dir, Filename, Ext);
    _makepath(SourceManifestName, Drive, Dir, Filename, Ext);
	strcat(SourceManifestName, ".manifest");

    if (GetFileAttributesA(SourceManifestName) != 0xffffffff) {
        //
        // Source file exists
        //
        strcpy(DestinationManifestName, DestinationFile);
        strcat(DestinationManifestName, "\\");
        strcat(DestinationManifestName, Filename);
        strcat(DestinationManifestName, Ext);
        strcat(DestinationManifestName, ".manifest");

        if (fVerbose || fTestMode) {
            fprintf(stdout,"BINPLACE : warning BNP0000: place %s in %s\n", SourceManifestName, DestinationManifestName);
        }

        if (!MakeSureDirectoryPathExists(DestinationManifestName)) {
            return FALSE;
        }
        if (!CopyFileA(SourceManifestName, DestinationManifestName, FALSE)) {
            return FALSE;
        }
        SetFileAttributes(DestinationManifestName,FILE_ATTRIBUTE_NORMAL);
        return TRUE;
    }
    return FALSE;
}

BOOL
FileExists(
          IN  LPCSTR FileName,
          OUT PWIN32_FIND_DATA FindData
          )
{
    BOOL Found;
    HANDLE FindHandle;


    FindHandle = FindFirstFile(FileName,FindData);
    if (FindHandle == INVALID_HANDLE_VALUE) {
        Found = FALSE;
    } else {
        FindClose(FindHandle);
        Found = TRUE;
    }

    return(Found);
}


