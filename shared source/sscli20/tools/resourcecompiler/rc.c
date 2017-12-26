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
// File: rc.c
// Purpose: Compiles .rc files to .satellite files
// ===========================================================================

#include "rc.h"
#include "palstartup.h"

#define BUF_SIZE (4097*2)  //Number comes from resource compiler mini spec
#define PATH_BUF_SIZE _MAX_PATH
#define UNKNOWN_LINE_NUMBER -1

#ifdef __GNUC__
/* By default, cc treats .rc files as something to be linked in and
 * therefore won't run the preprocessor on them.  The '-x c' option
 * tells cc to treat the following file as a C file.
 * On Darwin, we don't want to use the cpp-precomp preprocessor.
 */
#if __APPLE_CC__
#define COMPILER_PREPROCESSOR_FLAGS        "-E -no-cpp-precomp -DRC_INVOKED=1 -DFEATURE_PAL=1 -x c"
#else   // __APPLE_CC__
#define COMPILER_PREPROCESSOR_FLAGS        "-E -DRC_INVOKED=1 -DFEATURE_PAL=1 -x c"
#endif  // __APPLE_CC__
#ifdef PLATFORM_UNIX
#define COMPILER_NAME "gcc"
#else
#define COMPILER_NAME "gcc.exe"
#endif
#define LINEMARKER_PREFIX '#'
#define DEFINE_PREFIX   "-D"
#define INCLUDE_PREFIX  "-I"
#define TARGET_PREFIX   "-o"
#else // __GNUC__
#define COMPILER_PREPROCESSOR_FLAGS             "/nologo /EP /DRC_INVOKED=1 /DFEATURE_PAL=1"
#define COMPILER_NAME "cl.exe"
#define LINEMARKER_PREFIX "#line"
#define DEFINE_PREFIX   "/D"
#define INCLUDE_PREFIX  "/I"
#endif // __GNUC__

#ifdef PLATFORM_UNIX
#define PATH_SEPARATOR_CHAR '/'
#define PATH_SEPARATOR_STRING "/"
#else
#define PATH_SEPARATOR_CHAR '\\'
#define PATH_SEPARATOR_STRING "\\"
#endif

int getEscapeCharacter (char * input, char * output);
LPSTR FormatString(LPSTR inputString);
LPSTR convertToUTF8(LPSTR inputString);
int __cdecl steCompare(const void *first, const void *second);
void ProcessParameters(int argc, LPSTR argv[], LPSTR * compilerOptions);
void printUsage(LPSTR message);
void crashAndBurn(LPSTR message, int lineNumber, LPSTR file, LPSTR badline);
void updateLineNumber(LPSTR line, int * lineNumber, LPSTR * lineFile);
void preProcessFile(LPSTR compilerOptions);
void clearBlanksAndLineUpdates(FILE * file, int * lineNumber, 
                               LPSTR * lineFile, 
                               LPSTR * textline, BOOL checkCurrent);
BOOL startsWithLineMarker(const char *line);
BOOL __stdcall ConsoleControlHandler(DWORD dwCtrlType); 
LPSTR changeExtension(LPSTR filename, LPSTR oldExt, LPSTR newExt);
void removeDefinition(FILE * file, 
                      LPSTR textline,
                      BOOL linesBeforeBegin, 
                      int * lineNumber,
                      LPSTR * lineFile);
int LexOneLine(char *p);
ULONG Expr_Eval(void);


BOOL verbose = FALSE;
BOOL warn = FALSE;
BOOL nulls = FALSE;
int codepage = 1252;

LPSTR inFilename;
LPSTR outFilename = NULL;
LPSTR strippedFilename;
FILE * strippedrc;
FILE * outfile;


//MAIN

int __cdecl main(int argc, LPSTR argv[])
{
  int lineNumber = 0;
  LPSTR filenameStart;
  LPSTR lineFile = NULL;
  LPSTR textline;
  LPSTR alloctextline;
  LPSTR textlinetotok;
  LPSTR firstword;

  LPSTR stringID;
  UINT stringIDint;
  UINT32 filelenCur;
  LPSTR stringValue;
  UINT stringLen;
  LPSTR stringValueFormatted;
  LPSTR stringValueOut;
  STRING_TABLE_ENTRY  * stringtable = NULL;
  int stringTableCounter = 0;
  char * pchEndQuote;
  char * pchStartQuote;

  LPSTR compilerOptions;

  UINT16 uID;
  UINT32 stringoffset;

  int i;
  int lastValue = -1;
  BOOL OutFileWritten = FALSE;
  LONG exprResult;

  alloctextline = (LPSTR)malloc(BUF_SIZE);
  if(alloctextline == NULL){
      crashAndBurn("Out of memory - alloctextline", 
                   UNKNOWN_LINE_NUMBER, "", "");
  }

  SetConsoleCtrlHandler(ConsoleControlHandler, TRUE);

  argv++;
  argc--;
  if (argc == 0){
    printUsage("");
  }

  compilerOptions = (LPSTR)malloc(sizeof(" " COMPILER_PREPROCESSOR_FLAGS " "));

  if (compilerOptions == NULL)
    crashAndBurn("Out of memory - compilerOptions",
                 UNKNOWN_LINE_NUMBER, "", "");

  strcpy(compilerOptions, " " COMPILER_PREPROCESSOR_FLAGS " ");

  ProcessParameters(argc, argv, &compilerOptions);

  if (inFilename == NULL){
    printUsage("A filename must be specified\n");
  }
  else{
    printf("\n\n%s\n", inFilename);
  }

  if (verbose){
    printf("Preprocessing resource file...\n");
  }

  filenameStart = strrchr(inFilename, PATH_SEPARATOR_CHAR);
  if (filenameStart == NULL) {
    filenameStart = inFilename;
  } else {
    filenameStart++;
  }

  lineFile = (LPSTR)malloc(strlen(filenameStart) + 1);
  if (lineFile == NULL) {
      crashAndBurn("Out of memory - string allocation",
                   UNKNOWN_LINE_NUMBER, "", "");
  }
  strcpy(lineFile, filenameStart);

  {
    // The pre processed file is created in the temp directory
    char szTempName[MAX_PATH];
    char szTempPath[MAX_PATH];

    if (!GetTempPathA(MAX_PATH, szTempPath)) {
      crashAndBurn("GetTempPath failed!", UNKNOWN_LINE_NUMBER,"","");
    }

    if (!GetTempFileNameA(szTempPath, "RC", 0, szTempName)) {
      crashAndBurn("GetTempFileNameA failed!", UNKNOWN_LINE_NUMBER,"","");
    }
    
    strippedFilename = (LPSTR)malloc(strlen(szTempName) + 1);
    if (strippedFilename == NULL) {
      crashAndBurn("Out of memory - strippedFileName",
                   UNKNOWN_LINE_NUMBER, "", "");
    }

    strcpy(strippedFilename, szTempName);
  }

  preProcessFile(compilerOptions);

  free(compilerOptions);
  free(inFilename);

  if (verbose)
    printf("Reading Preprocessed File %s\n", strippedFilename);

  //Open stripped input file
  strippedrc = fopen(strippedFilename, "r");
  if (strippedrc == NULL){
    crashAndBurn("Preprocessor failure", UNKNOWN_LINE_NUMBER, "", "");
  }

  textline=alloctextline;
  while (fgets(textline, BUF_SIZE, strippedrc) != NULL){

    lineNumber++;

    clearBlanksAndLineUpdates(strippedrc, &lineNumber, &lineFile,
                              &textline, TRUE);
    if (textline == NULL){
      break;
    }

    textline[(strlen(textline)-1)] = '\0';  //Remove trailing newline

    textlinetotok = (LPSTR)malloc(strlen(textline) + 1);

    if(textlinetotok == NULL){
      crashAndBurn("Out of memory - textlinetotok", 
                   UNKNOWN_LINE_NUMBER, "", "");
    }

    strcpy(textlinetotok, textline);

    firstword = strtok(textlinetotok, " \t\n");
    assert(firstword != NULL);  //BLANK LINES WERE REMOVED
    
    if (!strcmp(firstword, "STRINGTABLE")){

      if (verbose)
       printf("StringTable resource found on line %i in \n\t%s, processing\n", 
               lineNumber, lineFile);
      
      clearBlanksAndLineUpdates(strippedrc,&lineNumber, 
                                &lineFile, &textline, FALSE);
      if (textline == NULL){
        crashAndBurn("Unexpected end of file in Stringtable", 
                     lineNumber, lineFile, "");
      }

      free(textlinetotok);

      //MAKE SURE BEGIN EXISTS
      textlinetotok = (LPSTR)malloc(strlen(textline) + 1);
      if (textlinetotok == NULL)
        crashAndBurn("Out of memory -textlinetotok", 
                     UNKNOWN_LINE_NUMBER,"",""); 
      textlinetotok = strcpy(textlinetotok, textline);

      firstword = strtok(textlinetotok, " \t\n");
      assert(firstword != NULL);  //BLANK LINES WERE REMOVED

      if ( (strcmp(firstword,"BEGIN")) ){  //NO BEGIN, CHECK FOR {
        textline = strchr(textline, '{');
        if (textline == NULL){
          crashAndBurn("No BEGIN or { following STRINGTABLE", 
                       lineNumber, lineFile, textline);
        }
        textline++;
      }
      else {
        textline = alloctextline;
        if(fgets(textline, BUF_SIZE, strippedrc) == NULL){
          crashAndBurn("end of file before END in stringtable", 
                       lineNumber, lineFile, "");
        }
        else{
          lineNumber++;
        }
      }
      free(textlinetotok);
        
      //process entries
      while(1){
        clearBlanksAndLineUpdates(strippedrc, 
                                  &lineNumber, &lineFile, &textline, TRUE);
        if (textline == NULL){
          crashAndBurn("end of file before END in stringtable", 
                       lineNumber, lineFile, "");
        }

        textlinetotok = (LPSTR)malloc(strlen(textline) + 1);
        if (textlinetotok == NULL)
          crashAndBurn("Out of memory -textlinetotok", 
                       UNKNOWN_LINE_NUMBER,"",""); 
        textlinetotok = strcpy(textlinetotok, textline);

        firstword = strtok(textlinetotok, " \t\n");
        assert(firstword != NULL);  //BLANK LINES WERE REMOVED
        
        if (!strcmp(firstword,"END")|| !strcmp(firstword, "}")){
          free(textlinetotok);
          break;
        }

        if (stringtable == NULL){  //First time through loop, no memory yet.
          stringtable = (STRING_TABLE_ENTRY*)malloc(sizeof(STRING_TABLE_ENTRY));
          if (stringtable == NULL){
            crashAndBurn("Out of memory - stringtable", 
                         UNKNOWN_LINE_NUMBER, "", "");
          }
        }
        else {  //Make room for additional entry
          stringtable = (STRING_TABLE_ENTRY*)realloc(stringtable, 
                                (stringTableCounter + 1) *
                                sizeof(STRING_TABLE_ENTRY));
          if (stringtable == NULL){
            crashAndBurn("Out of memory - stringtable", 
                         UNKNOWN_LINE_NUMBER, "", "");
          }
        }

        //GET FRESH COPY OF TEXTLINE TO TOKENIZE WITH DIFFERENT SPACERS
        free(textlinetotok);

        textlinetotok = (LPSTR)malloc(strlen(textline) + 1);
        if (textlinetotok == NULL)
          crashAndBurn("Out of memory -textlinetotok", 
                       UNKNOWN_LINE_NUMBER,"",""); 
        textlinetotok = strcpy(textlinetotok, textline) ;
        
        stringID = strtok(textlinetotok, "\"");
        if (stringID == NULL)
          crashAndBurn("Bad Format in StringTable", 
                       lineNumber, lineFile, textline);

        if (strstr(stringID, "L") != NULL)
          *(strstr(stringID, "L")) = '\0';

        if(!LexOneLine(stringID))
          crashAndBurn("StringID in bad form", 
                       lineNumber, lineFile, textline);
        
        // Note: we are indexing only using the low 16 bits
        exprResult = Expr_Eval();
        if (exprResult == -1) {
           // Expr_Eval returns -1 if there is no valid number
          crashAndBurn("No StringID found", 
                       lineNumber, lineFile, textline);
        }
        stringIDint = (UINT16) exprResult;
        free(textlinetotok);
        
        // Cannot use strtok because it will ignore quotes in resources
        // that appear as  "this has a ""quoted"" string"
        pchStartQuote = strchr(textline, '\"');
        pchEndQuote = strrchr(textline, '\"');
        stringValue = NULL;
        if (pchEndQuote != NULL && pchEndQuote != pchStartQuote)
        {
            stringLen = pchEndQuote - pchStartQuote + 1;
            stringValue = (LPSTR)malloc(stringLen + 1);
            if (stringValue == NULL)
              crashAndBurn("Out of memory - stringValue", 
                           lineNumber, lineFile, textline); 
            stringValue = strncpy(stringValue, pchStartQuote, stringLen);
            stringValue[stringLen-1] = '\0';
        }

        if (stringValue == NULL){
          //STRING VALUE WAS NOT ON SAME LINE AS STRING ID
          //SEARCH UPCOMING LINES TO SEE IF WE CAN FIND IT
          
          int oldLineNumber = lineNumber;

          clearBlanksAndLineUpdates(strippedrc,
                                    &lineNumber, &lineFile, &textline, FALSE);
          if (textline == NULL){
            crashAndBurn("Unexpected end of file in stringtable", 
                         lineNumber, lineFile, "");
          }

          if (strchr(textline, '\"') != NULL){
            // Cannot use strtok because it will ignore quotes in resources
            // that appear as  "this has a ""quoted"" string"
            pchStartQuote = strchr(textline, '\"');
            pchEndQuote = strrchr(textline, '\"');
            stringValue = NULL;
            if (pchEndQuote != NULL && pchEndQuote != pchStartQuote)
            {
                stringLen = pchEndQuote - pchStartQuote + 1;
                stringValue = (LPSTR)malloc(stringLen + 1);
                if (stringValue == NULL)
                  crashAndBurn("Out of memory - stringValue", 
                               oldLineNumber, lineFile, textline); 
                stringValue = strncpy(stringValue, pchStartQuote, stringLen);
                stringValue[stringLen-1] = '\0';
            }
          }
          else{ 
            /// Found a line which is not blank, but does not have
            /// a string surrounded by quotes in it
            crashAndBurn("StringID with no StringValue in STRINGTABLE",
                         oldLineNumber, lineFile, "");
          }
        }

        stringValueFormatted = FormatString(stringValue + 1); // skip over leading quote
        if (stringValueFormatted == NULL){
          crashAndBurn("Unknown escape sequence in String Value",
                       lineNumber,lineFile, textline);
        }       

        stringValueOut = convertToUTF8(stringValueFormatted);
        free(stringValueFormatted);
        free(stringValue);

        stringtable[stringTableCounter].StringID = stringIDint;
        stringtable[stringTableCounter].StringValue = stringValueOut;
        stringTableCounter++;

        // If there is a '}' after the string and new line char, end section
        if (pchEndQuote) {
          char *	pchNewLine = strchr(pchEndQuote + 1, '\n');
          char *	pchBrace = strchr(pchEndQuote + 1, '}');

          if (pchNewLine != NULL && pchBrace != NULL && pchNewLine > pchBrace)
            break;
        }

        textline = alloctextline; 
        if(fgets(textline, BUF_SIZE, strippedrc) == NULL){
          crashAndBurn("end of file before END in stringtable", 
                       lineNumber, lineFile, "");
        }
        else{
          lineNumber++;
        }
        
      }

    }
    
    else if (!strcmp(firstword, "#pragma")){
      LPSTR pragma;

      pragma = strtok(NULL, " (");

      if(!strcmp(pragma, "code_page")){
        LPSTR cpString;

        cpString = strtok(NULL, ")");
        codepage = strtoul(cpString, NULL, 0); 
        
        if(codepage == 0){ //couldn't conver to int
          crashAndBurn("Bad Format in code_page pragma", 
                       lineNumber, lineFile, textline);
        }

        if (!IsValidCodePage(codepage)){
          if (warn)
            printf("WARNING, codepage %i is invalid\n", codepage);
          else
            crashAndBurn("Invalid Code Page Value",
                         lineNumber, lineFile, textline);
        }
        if(verbose)
          printf("Codepage pragma found, updating codepage to %i\n", codepage);
      }
      else if(!strcmp(pragma, "once")){
        //ignore
      }
      else if(!strcmp(pragma, "GCC")){
        //ignore
      }
      else{
        crashAndBurn("unknown pragma", lineNumber, lineFile, textline);
      }
      free(textlinetotok);
    }
    else if(!strcmp(firstword, "typedef")){
      int i;
      ////SKIP ALL LINES UNTIL FIRST {, THEN STOP WHEN FIND } THAT MATCHES

      printf("WARNING...TYPEDEF in %s:\n %s\n", lineFile, textline);
     
      free(textlinetotok);
      textline = alloctextline; 
      fgets(textline, BUF_SIZE, strippedrc);
      lineNumber++;
      
      while (strstr(textline, "{") == NULL){ //Skipping to first {
        
        if (fgets(textline, BUF_SIZE, strippedrc) == NULL){
          crashAndBurn("Unexpected end of file", ++lineNumber, lineFile, "");
        }
        else{
          lineNumber++;
        }
      }

      textline = strstr(textline, "{");
      textline++;
      
      for (i = 1; i != 0;){
        //i is the number of { without }
        
        lineNumber++;
        if (strstr(textline, "{") != NULL){
          textline = strstr(textline, "{");
          textline++;
          i++;
        }
        else if (strstr(textline, "}") != NULL){
          textline = strstr(textline, "}");
          textline++;
          i--;
        }
        else{
          textline = alloctextline;
          if(fgets(textline, BUF_SIZE, strippedrc) == NULL){
            crashAndBurn("Unexpected end of file", ++lineNumber, lineFile, "");
          }
          lineNumber++;
          i = i;
        }
      }
    }
    else {  ///SOME OTHER RESOURCE
      LPSTR secondword = strtok(NULL, " \t\n");
      if (secondword == NULL)
        secondword = "";

      if(!strcmp(firstword, "FONT")
         || !strcmp(firstword, "AUTO3STATE")
         || !strcmp(firstword, "AUTORADIOBUTTON")
         || !strcmp(firstword, "COMBOBOX")
         || !strcmp(firstword, "CONTROL")
         || !strcmp(firstword, "CTEXT")
         || !strcmp(firstword, "DEFPUSHBUTTON")
         || !strcmp(firstword, "EDITTEXT")
         || !strcmp(firstword, "PUSHBOX")
         || !strcmp(firstword, "PUSHBUTTON")
         || !strcmp(firstword, "RADIOBUTTON")
         || !strcmp(firstword, "RTEXT")
         || !strcmp(firstword, "SCROLLBAR")
         || !strcmp(firstword, "STATE3")
         || !strcmp(firstword, "CAPTION")
         || !strcmp(firstword, "LANGUAGE")
         || !strcmp(firstword, "MENUITEM")
         || !strcmp(firstword, "STYLE")
         ){
        if (verbose) //These resources are one line of text in resource file
          printf("Resource %s found on line % i in \n\t%s, ignoring\n", 
                 firstword, lineNumber, lineFile);
        free(textlinetotok);
        continue;
      }

      else if(!strcmp(secondword, "TYPELIB")
              || !strcmp(secondword, "typelib")
              || !strcmp(secondword, "ICON")
              || !strcmp(secondword, "BITMAP")
              || !strcmp(secondword, "CURSOR")
              || !strcmp(secondword, "REGISTRY")
              || !strcmp(secondword, "FONT")
              || !strcmp(secondword, "MESSAGETABLE")){
        if (verbose) //These resources are one line of text in resource file
          printf("Resource %s found on line % i in \n\t%s, ignoring\n", 
                 secondword, lineNumber, lineFile);
        free(textlinetotok);
        continue;
      }

      else if (!strcmp(secondword, "VERSIONINFO")
          || !strcmp(secondword, "TOOLBAR")
          || !strcmp(secondword, "MENU")
          || !strcmp(secondword, "ACCELERATORS")
          || !strcmp(secondword, "DIALOG")
          || !strcmp(secondword, "TEXTINCLUDE")
          || !strcmp(secondword, "DIALOGEX")
          || !strcmp(secondword, "MENUEX")
          || !strcmp(secondword, "RCDATA")
          || !strcmp(firstword, "POPUP")){
        
        // THESE RESOURCES TAKE UP MORE THEN ONE LINE, DELIMITED BY
        // BEGIN/END TAGS OR BRACES { }

        //      int i;

        if (!strcmp(firstword, "POPUP")){
          secondword = firstword;
        }

        if (verbose){
          printf("Resource %s found on line % i in \n\t%s, ignoring\n", 
                 secondword, lineNumber, lineFile);
        }

        removeDefinition(strippedrc, textline, TRUE, &lineNumber, &lineFile);
        free(textlinetotok);
      }            
    
      else { //USER DEFINED RESOURCE
        
        // OF TWO FORMS, EITHER ONE LINE WITH FILENAME SPECIFYING RESOURCE
        // OR MULTIPLE LINES, WITH RESOURCE IN SCRIPT SURROUNDED BY BEGIN/END
        // TAGS OR BRACES { }

        LPSTR thirdword;


        printf("UNKNOWN resource %s found on line % i in \n\t%s, ignoring\n",
               secondword, lineNumber, lineFile);

        thirdword = strtok(NULL, " \t\n");

        if (thirdword != NULL){ // RESOURCE DEFINED IN OTHER FILE, ONE LINE
          free(textlinetotok);
          continue;
        }
        else { // RESOURCE IN RESOURCE SCRIPT, SKIP DEFINITION
          
          textline = alloctextline;
          fgets(textline, BUF_SIZE, strippedrc);
          lineNumber++;

          removeDefinition(strippedrc, textline, FALSE, &lineNumber, &lineFile);
          free(textlinetotok);
        }
      }
    }
    textline = alloctextline;
  }

  free(alloctextline);

  qsort((void *)stringtable, stringTableCounter,    //sort array by id 
        sizeof(STRING_TABLE_ENTRY), steCompare);

  //Open output file
  if (verbose){
    printf("Opening %s for writing\n",outFilename);
  }
  outfile = fopen(outFilename,"w+b");
  if (outfile == NULL){
    crashAndBurn("Couldn't open file for output",UNKNOWN_LINE_NUMBER, "", "");
  }

  filelenCur = 0;
  for(i=0; i<stringTableCounter; i++){

    if (stringtable[i].StringID == lastValue){  //CHECK FOR DOUBLES
      LPSTR errorMessage;

      errorMessage = (LPSTR)malloc(sizeof("Duplicate StringID for Values: ") + 
                            strlen(stringtable[i-1].StringValue) + 
                            sizeof(", ") + 
                            strlen(stringtable[i].StringValue) - 1);   
      if (errorMessage == NULL){
        crashAndBurn("Out of memory",
                     UNKNOWN_LINE_NUMBER, "", "");
      }             

      errorMessage = strcpy(errorMessage, "Duplicate StringID for Values: ");
      errorMessage = strcat(errorMessage, stringtable[i-1].StringValue);
      errorMessage = strcat(errorMessage, ", ");
      errorMessage = strcat(errorMessage, stringtable[i].StringValue);
      
      crashAndBurn(errorMessage, UNKNOWN_LINE_NUMBER, "", "");
      
      //ERROR MESSAGE SHOWS VALUES WHICH SHARE AN ID

    }

    //PRINT OUT TO FILE
    OutFileWritten = TRUE;
    // Save string length
    stringtable[i].StringLen = strlen(stringtable[i].StringValue);

    // Write string data
    filelenCur += fwrite(stringtable[i].StringValue, 1, stringtable[i].StringLen, outfile);

    lastValue = stringtable[i].StringID;
  }
  for(i=0; i<stringTableCounter; i++){
    // Done with the StringValue, free it up
    free(stringtable[i].StringValue);
  }

  // If odd # of bytes written, pad with an extra to make the table entries
  // WORD aligned.
  if (filelenCur & 1)
  {
      char  ch = 0;

      fwrite(&ch, 1, sizeof(ch), outfile);
  }

  // Write out the table
  filelenCur = 0;
  for(i=0; i<stringTableCounter; i++){
        uID = VAL16(stringtable[i].StringID);
        stringoffset = VAL32(filelenCur);
        filelenCur += stringtable[i].StringLen;
        // Resource ID
        fwrite(&uID, 1, sizeof(uID), outfile);
        // File offset
        fwrite(&stringoffset, 1, sizeof(stringoffset), outfile);
    }
  uID = VAL16(-1);
  stringoffset = VAL32(filelenCur);
  // Resource ID
  fwrite(&uID, 1, sizeof(uID), outfile);
  // File offset
  fwrite(&stringoffset, 1, sizeof(stringoffset), outfile);

  if (verbose){
    printf("Output file is: %s\n\n", outFilename);
  }

  // CLOSE BOTH OUTPUT FILE AND PREPROCESSOR (.i) FILE

  if (strippedrc != NULL){
    fclose(strippedrc);
  }
  if (outfile != NULL){
    fclose(outfile);
  }
  if (!OutFileWritten) {
      if (verbose) {
          printf("Removing output file - no STRINGTABLEs were found\n");
      }
      DeleteFileA(outFilename);
  }

  //REMOVE .i FILE

  if(DeleteFileA(strippedFilename)){
    if(verbose){
      printf("Removing %s\n", strippedFilename);
    }
  }

  if (lineFile != NULL) {
    free(lineFile);
  }

  if (stringtable) {
    free(stringtable);
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
//        ConsoleControlHandler
//
// Parameters:
//   dwCtrlType: the type of signal to be handled
//
// Returns:
//   false to indicate the program should exit
//
///////////////////////////////////////////////////////////////////////////////
BOOL 
__stdcall
ConsoleControlHandler(DWORD dwCtrlType)
{     
  if (strippedrc != NULL){
    fclose(strippedrc);
  }
  if (outfile != NULL){
    fclose(outfile);
  }
  DeleteFileA(strippedFilename);
  DeleteFileA(outFilename);
  
  return FALSE;
} 

///////////////////////////////////////////////////////////////////////////////
//  
//        removeDefinition
//
///////////////////////////////////////////////////////////////////////////////

void removeDefinition(FILE * file, 
                      LPSTR textline,
                      BOOL linesBeforeBegin, 
                      int * lineNumber,
                      LPSTR * lineFile)
{
  // LPSTR textline = malloc(BUF_SIZE);
  int i;


  if(linesBeforeBegin){
    ////SKIP ALL LINES UNTIL FIRST BEGIN OR { 
    ////THEN STOP WHEN FIND END or }THAT MATCHES
    
    while ((strstr(textline, "BEGIN") == NULL) && 
           (strstr(textline, "{") == NULL)){
      if (fgets(textline, BUF_SIZE, strippedrc) == NULL){
        crashAndBurn("Unexpected end of file", 
                     ++(*lineNumber), (*lineFile), "");
      }
      else {
        (*lineNumber)++;
      }
    }
  }
  else if ((strstr(textline, "BEGIN") == NULL) && 
           (strstr(textline, "{") == NULL)){
    crashAndBurn("Bad Format in User Defined Resource",
                 *lineNumber, *lineFile, textline);
  }
  
  if(strstr(textline, "{") != NULL){  //Brace broke the loop
    textline = strstr(textline, "{");
    textline++;
  }
  else{  //BEGIN broke the loop
    if(fgets(textline, BUF_SIZE, strippedrc) == NULL){
      crashAndBurn("Unexpected end of file", 
                   ++(*lineNumber), (*lineFile), "");
    }
    else{
      (*lineNumber)++;
    }
  }       
  
  for (i = 1; i != 0;){
    LPSTR word;
    LPSTR textlinetotok;

    textlinetotok = (LPSTR)malloc(strlen(textline) + 1);
    if (textlinetotok == NULL)
      crashAndBurn("Out of memory -textlinetotok", 
                   UNKNOWN_LINE_NUMBER,"",""); 
    textlinetotok = strcpy(textlinetotok, textline);

    word = strtok(textlinetotok, " \t\n");
    if (word == NULL)
      word = "";

    if (textline[0] == '\0')
            break;
    //i is the number of opens not yet closed
    if (!strcmp(word, "BEGIN")){
      i++;
      if(fgets(textline, BUF_SIZE, strippedrc) == NULL){
        crashAndBurn("Unexpected end of file", 
                           ++(*lineNumber), (*lineFile), "");
      }
      else{
        (*lineNumber)++;
      }
    }
    else if (strstr(textline, "{") != NULL){
      i++;
      textline = strstr(textline, "{");
      textline++;
    }
    else if (!strcmp(word, "END")){
      i--;
      if(i != 0){
        if(fgets(textline, BUF_SIZE, strippedrc) == NULL){
          crashAndBurn("Unexpected end of file", 
                       ++(*lineNumber), (*lineFile), "");
        }
      }
      else{
        (*lineNumber)++;
      }
    }
    else if (strstr(textline, "}") != NULL){
      i--;
      textline = strstr(textline, "}");
      textline++;
    }
    else{ 
      if(fgets(textline, BUF_SIZE, strippedrc) == NULL){
        crashAndBurn("Unexpected end of file", 
                     ++(*lineNumber), (*lineFile), "");
      }
      else{
        (*lineNumber)++;
      }
    }
    free(textlinetotok);
  }
}
///////////////////////////////////////////////////////////////////////////////
//
//        crashAndBurn
//
// Parameters:
//   message:    Message to be displayed to stderr
//   lineNumber: Line number in .rc file in which error occured, or
//               UNKNOWN_LINE_NUMBER if not line number specific or line
//               number unknown.
//   file:       File in which the error occured, if using known line number
//   badline:    The line of text which caused the error, if using known line
//               number.
//
///////////////////////////////////////////////////////////////////////////////

void crashAndBurn(LPSTR message, int lineNumber, LPSTR file, LPSTR badline){

  if (strippedrc != NULL){
    fclose(strippedrc);
  }
  if (outfile != NULL){
    fclose(outfile);
  }
  if (strippedFilename)
  {
    DeleteFileA(strippedFilename);
  }
  if (outFilename)
  {
    DeleteFileA(outFilename);
  }

  fprintf(stderr, "resourcecompiler: fatal error RC0001 : ");

  if(lineNumber != UNKNOWN_LINE_NUMBER){
    fprintf(stderr, "on line %d in %s: \n", lineNumber, file);
    if (badline != NULL && badline[0] != '\0') {
      fprintf(stderr, "resourcecompiler: %s\n", badline);
    }
  }
  fprintf(stderr, "resourcecompiler: %s\n", message);
  exit(1);
}


///////////////////////////////////////////////////////////////////////////////
//
//        ProcessParameters
//
// Parameters:
//   argc:            Number of arguments to be processed
//   argv[]:          Array of parameters
//   compilerOptions: Options to be passed to the compiler for preprocessing
//
///////////////////////////////////////////////////////////////////////////////

void ProcessParameters(int argc, LPSTR argv[], LPSTR * compilerOptions)
{
  char * p;

  while(argc){
    if (argc == 1){
      if (!strcmp(argv[0], "/?") ||
          !strcmp(argv[0], "-?")  ||
          !strcmp(argv[0], "/h") ||
          !strcmp(argv[0], "-h")){
          printUsage("");
      }
#if PLATFORM_UNIX
      // Allow absolute paths
      else if (**argv == '-') {
#else   // PLATFORM_UNIX
      else if(**argv == '/' || **argv == '-'){
#endif  // PLATFORM_UNIX
        //LAST ARGUMENT SHOULD NOT BE OPTION, SHOULD BE FILENAME
        printUsage("A filename must be specified\n");
      }
      
      else{ //One arguement is filename
        FILE * inFile;
        FILE * inFile2;
        LPSTR inFileAppend;

        inFilename = (LPSTR)malloc(strlen(argv[0]) + 1);
        if (inFilename == NULL){
           crashAndBurn("Out of memory - inFilename", 
                       UNKNOWN_LINE_NUMBER, "", "");
        }
        
        strcpy(inFilename, argv[0]);

        inFileAppend = (LPSTR)malloc(strlen(argv[0]) + sizeof(".rc"));
        if (inFileAppend == NULL){
          crashAndBurn("Out of memory - inFileAppend", 
                       UNKNOWN_LINE_NUMBER, "", "");
        }

        inFileAppend = strcpy(inFileAppend, argv[0]);
        inFileAppend = strcat(inFileAppend, ".rc");

        inFile = fopen(inFilename, "r");
        inFile2 = fopen(inFileAppend, "r");

        if(inFile == NULL && inFile2 == NULL){
          LPSTR errorMessage;
          
          errorMessage = (LPSTR)malloc(sizeof("Cannot Open File: ") +
                                + strlen(inFilename));
          if (errorMessage == NULL)
            crashAndBurn("Out of memory - errorMessage", 
                         UNKNOWN_LINE_NUMBER, "", "");
          
          errorMessage = strcpy(errorMessage, "Cannot Open File: ");
          errorMessage = strcat(errorMessage, inFilename);
      
          free(inFileAppend);
          free(inFilename);    
          crashAndBurn(errorMessage, UNKNOWN_LINE_NUMBER, "", "");
        }
        else{
          //success!

          if (inFile2 != NULL){
            fclose(inFile2);        
            if (inFile == NULL){
              free(inFilename);
              inFilename = inFileAppend;
            }
            else {
              free(inFileAppend);
            }
          }

          if (inFile != NULL){
            fclose(inFile);
            free(inFileAppend);
          }

          if (outFilename == NULL){
            outFilename = changeExtension(inFilename, ".rc", ".satellite");
          }
        }
      }
    }
    else if ( (**argv == '/') || (**argv == '-')){
      for(p = *argv + 1; *p != '\0'; p++){
        switch(tolower(*p)){
        case '/':
        case '-':
          break;
        case '?': 
        case 'h':
          printUsage("");
          break;
        case 'c':
           p++;
           if(*p == '\0')
             {
               argv++;
               argc--;
               p = *argv;
             }

           codepage = strtoul(p, NULL, 16); 
           if (!IsValidCodePage(codepage)){
             if (warn)
               printf("WARNING, codepage %i is invalid\n", codepage);
             else
               crashAndBurn("Invalid Code Page Value", 
                            UNKNOWN_LINE_NUMBER, "", "");
           } 

           p += strlen(p) - 1;
           break;

        case 'd':
          p++;

          if(*p == '\0')
             {
               argv++;
               argc--;
               p = *argv;
             }


          *compilerOptions = (LPSTR)realloc(*compilerOptions, 
                                    strlen(*compilerOptions) + 
                    sizeof(DEFINE_PREFIX) + 
                                    strlen(p) +
                                    sizeof(" ") - 1);
          if (*compilerOptions == NULL){
            crashAndBurn("Out of memory - compilerOptions",  
                         UNKNOWN_LINE_NUMBER, "", "");
          }
      *compilerOptions = strcat(*compilerOptions, DEFINE_PREFIX);
          *compilerOptions = strcat(*compilerOptions, p);
          *compilerOptions = strcat(*compilerOptions, " ");

          p += strlen(p) - 1;
          break;

        case 'f':
          if (*(++p) != 'o'){
            printUsage("'f' must be followed by 'o' for renaming output\n");
          }
         else{
           p++;
           outFilename = p;
           if(!strcmp(outFilename,""))
             {
               argv++;
               argc--;
               p = *argv;
             }
           
           outFilename = p;
           
           p += strlen(outFilename) - 1;
           if (verbose)
             printf("Set output file name to %s\n", outFilename);
           break;
         }
        case 'i':
          p++;
        
          if(*p == '\0')
          {
            argv++;
            argc--;
            p = *argv;
          }

          *compilerOptions = (LPSTR)realloc(*compilerOptions,
                                    strlen(*compilerOptions) +
                                    sizeof(INCLUDE_PREFIX) +
                                    strlen(p) +
                                    strlen(" "));
          if (*compilerOptions == NULL){
            crashAndBurn("Out of memory - compilerOptions",
                         UNKNOWN_LINE_NUMBER, "", "");
          }
          *compilerOptions = strcat(*compilerOptions, INCLUDE_PREFIX);
          *compilerOptions = strcat(*compilerOptions, p);
          *compilerOptions = strcat(*compilerOptions, " ");

          p += strlen(p) - 1;
          break;

        case 'l':
          p++;
          
          if(*p == '\0')
          {
            argv++;
            argc--;
            p = *argv;
          }
          
          //THIS ARGUMENT IS NOT USED BY RC!!  
          //Codepage is set by #pragma(codepage) or \c
          
          p += strlen(p) - 1;
          break;
        case 'n':
          break;
        case 'r':
          break;
        case 'u':
          p++;
          
          if(*p == '\0'){
            argv++;
            argc--;
            p = *argv;
          }
          
          *compilerOptions = (LPSTR)realloc(*compilerOptions, 
                                    strlen(*compilerOptions) + 
                                    sizeof("/U") + 
                                    strlen(p) +
                                    strlen(" "));
          if (*compilerOptions == NULL){
            crashAndBurn("Out of memory - compilerOptions", 
                         UNKNOWN_LINE_NUMBER, "", "");
          }
          *compilerOptions = strcat(*compilerOptions, "/U");
          *compilerOptions = strcat(*compilerOptions, p);
          *compilerOptions = strcat(*compilerOptions, " ");

          p += strlen(p) - 1;
          break;
        case 'v':
          verbose = TRUE;
          printf("Processing Arguments...\n");
          break;
        case 'w':
          warn = TRUE;
          break;
        case 'x':
          *compilerOptions = (LPSTR)realloc(*compilerOptions, 
                                    strlen(*compilerOptions) + 
                                    sizeof("/X "));
          if (*compilerOptions == NULL){
            crashAndBurn("Out of memory - compilerOptions", 
                         UNKNOWN_LINE_NUMBER, "", "");
          }
          *compilerOptions = strcat(*compilerOptions, "/X ");

          break;
        case 'z':
          //ARGUMENT IGNORED, PROVIDED FOR COMPATIBILITY

           p++;
           
           if(*p == '\0')
             {
               argv++;
               argc--;
               p = *argv;
             }

           p += strlen(p) - 1;
           break;
        default:
          printUsage("Unknown argument\n");
          break;
        }
      }
    }
    else{ //ARGUMENT THAT DOESN'T START WITH / OR - BUT IS NOT LAST
      printUsage("Unknown argument, no - or /\n");
    }
    argc--;
    argv++;
  }
}
///////////////////////////////////////////////////////////////////////////////
//  
//        printUsage
//
// Prints the Usage Statement for rc
//
// Parameters:
//   message: A message to be printed before the usage statemnt, often 
//            indicating what caused the usage statement to be printed  
//
///////////////////////////////////////////////////////////////////////////////

void printUsage(LPSTR message){
  printf(message);

  printf("Usage: resourcecompiler [options] inputfile\n");
  printf("\nOptions: \n\n");
  printf("\t/?  Displays a list of resource compiler command-line options.\n");
  printf("\t/d  Defines a symbol for the preprocessor that you can test\n");
  printf("\t    with the ifdef directive\n");
  printf("\t/fo Rename output file\n");
  printf("\t/h  Displays a list of command-line options\n");
  printf("\t/i  Searches the specified directory before searching the\n");
  printf("\t    directories specified by the INCLUDE enviornment variable\n");
  printf("\t/c  specifies the codepage to be used for compilation\n");
  printf("\t/n  Null terminates all strings in the string table\n");
  printf("\t/r  Ignored. Provided for compatibility with existing\n");
  printf("\t    makefiles\n");
  printf("\t/l  Ignored. Provided for compatibility with existing\n");  
  printf("\t    makefiles\n");

  printf("\t/z  Ignored. Provided for compatibility with existing\n");  
  printf("\t    makefiles\n");

  printf("\t/u  Undefines a symbol for the preprocessor\n");
  printf("\t/v  Displays messages that report on the progress of the\n");
  printf("\t    compiler\n");
  printf("\t/w  Warns if invalid codepage (default is error)\n");
  printf("\t/x  Prevents RC from checking INCLUDE enviornment variable\n"); 
  printf("\t    when searching for header files or resource files\n\n");

  exit(1);
}

///////////////////////////////////////////////////////////////////////////////
//  
//        preProcessFile
//
// Calls the C preprocessor on the input file to produce the stripped file.
//
// Parameters:
//   compilerOptions: Command Line arguments to be passed to the compiler
//
///////////////////////////////////////////////////////////////////////////////
void preProcessFile(LPSTR compilerOptions)
{
  LPSTR commandLine;
  PROCESS_INFORMATION procInfo;
  STARTUPINFOA startInfo;
  DWORD exitCode;
  HANDLE hPreFile;
  SECURITY_ATTRIBUTES secAttr;

  commandLine = (LPSTR)malloc(strlen(COMPILER_NAME) +
               strlen(compilerOptions) +
               strlen(inFilename) +
               strlen("\"\"") + 1);
  if (commandLine == NULL){
    crashAndBurn("Out of memory - commandLine", UNKNOWN_LINE_NUMBER, "", "");
  }

  commandLine = strcpy(commandLine, COMPILER_NAME);

  strcat(commandLine, compilerOptions);
  strcat(commandLine, "\"");
  strcat(commandLine, inFilename);
  strcat(commandLine, "\"");

  memset(&startInfo, 0, sizeof(startInfo));
  startInfo.cb = sizeof(startInfo);

  memset(&secAttr, 0, sizeof(secAttr));
  secAttr.nLength = sizeof(secAttr);
  secAttr.bInheritHandle = TRUE;

  if ((hPreFile = CreateFileA(strippedFilename, GENERIC_WRITE, 0, &secAttr,
      CREATE_ALWAYS, 0, NULL)) == INVALID_HANDLE_VALUE) {
    crashAndBurn("Failed to create temporary file for preprocessor",
		 UNKNOWN_LINE_NUMBER,"","");
  }

  startInfo.dwFlags = STARTF_USESTDHANDLES;
  startInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
  startInfo.hStdOutput = hPreFile;
  startInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);

  if (verbose){
    printf("\n\n\nC compiler command line is:  %s\n\n\n", commandLine);
  }

  if(!CreateProcessA(NULL,
                     commandLine,
                     NULL,
                     NULL, 
                     TRUE, 
                     0,
                     NULL, 
                     NULL, 
                     &startInfo,
                     &procInfo)){
    crashAndBurn("Failed to spawn preprocessor", UNKNOWN_LINE_NUMBER,"","");
  }

  WaitForSingleObject( procInfo.hProcess, INFINITE );

  GetExitCodeProcess(procInfo.hProcess, &exitCode);

  if (exitCode != 0){
    crashAndBurn("Preprocessor Failed", UNKNOWN_LINE_NUMBER, "", "");
  }

  CloseHandle(procInfo.hProcess);
  CloseHandle(procInfo.hThread);
  CloseHandle(hPreFile);
  free(commandLine);
}


///////////////////////////////////////////////////////////////////////////////
//  
//        convertToUTF8
//
// Parameters:
//   inputString:  pointer to string of characters to be converted
//
// Returns:
//   Pointer to a string converted into UTF8
//
///////////////////////////////////////////////////////////////////////////////

LPSTR convertToUTF8(LPSTR inputString)
{
  
  int i;
  int error;
  int numOfWideChars;
  int numOfOutChars;
  LPWSTR wideString;
  LPSTR outputString;

  numOfWideChars = MultiByteToWideChar(codepage, 0, 
                                       inputString, 
                                       strlen(inputString), 
                                       NULL, 0);
  
  wideString = (LPWSTR)malloc(numOfWideChars*sizeof(WCHAR));
  if (wideString == NULL){
    crashAndBurn("Out of memory - wideString", UNKNOWN_LINE_NUMBER, "", "");
  }

  i = MultiByteToWideChar(codepage,0,inputString, 
                          strlen(inputString),wideString, 
                          numOfWideChars);
  
  if (i == 0){
    error = GetLastError();
    if (error == ERROR_INSUFFICIENT_BUFFER)
      crashAndBurn("Converting to UTF8 - INSUFFICIENT_BUFFER\n", 
                   UNKNOWN_LINE_NUMBER, "", "");
    else if (error == ERROR_INVALID_FLAGS)
      crashAndBurn("Converting to UTF8 - INVALID_FLAGS\n", 
                   UNKNOWN_LINE_NUMBER, "", "");
    else if (error == ERROR_INVALID_PARAMETER)
      crashAndBurn("Converting to UTF8 - INVALID_PARAMETER 1\n", 
                   UNKNOWN_LINE_NUMBER, "", "");
    else if (error == ERROR_NO_UNICODE_TRANSLATION )
      crashAndBurn("Converting to UTF8 - NO_UNICODE_TRANSLATION\n",  
                   UNKNOWN_LINE_NUMBER, "", "");
    else
      crashAndBurn("Error converting to UTF8\n",  
                   UNKNOWN_LINE_NUMBER, "", "");
  }


  numOfOutChars = WideCharToMultiByte(CP_UTF8, 0, wideString, 
                                      numOfWideChars, NULL, 0, NULL, NULL);
  

  outputString = (LPSTR)malloc(numOfOutChars + 1);
  if (outputString == NULL){
    crashAndBurn("Out of memory - outputString", UNKNOWN_LINE_NUMBER, "", "");
  }

 
  i = WideCharToMultiByte(CP_UTF8, 0, wideString, 
                          numOfWideChars, outputString, 
                          numOfOutChars, NULL, NULL);
  
  if (i == 0){
    error = GetLastError();
    if (error == ERROR_INSUFFICIENT_BUFFER)
      crashAndBurn("Converting to UTF8 - INSUFFICIENT_BUFFER\n",  
                   UNKNOWN_LINE_NUMBER, "", "");
    else if (error == ERROR_INVALID_FLAGS)
      crashAndBurn("Converting to UTF8 - INVALID_FLAGS\n",  
                   UNKNOWN_LINE_NUMBER, "", "");
    else if (error == ERROR_INVALID_PARAMETER)
      crashAndBurn("Converting to UTF8 - INVALID_PARAMETER 1\n",  
                   UNKNOWN_LINE_NUMBER, "", "");
    else
      crashAndBurn("Error converting to UTF8\n",  
                   UNKNOWN_LINE_NUMBER, "", "");
  }
  
  outputString[numOfOutChars] = '\0';

  free(wideString);
  return outputString;
}

///////////////////////////////////////////////////////////////////////////////
//  
//        FormatString
//
// Parameters:
//   inputString:  pointer to string of characters to be formatted
//
// Returns:
//  all character sequences interpretable as escape characters are 
//  replaced by the corresponding character
//
///////////////////////////////////////////////////////////////////////////////

LPSTR FormatString(LPSTR inputString)
{
  LPSTR formattedString;
  int i,j, stringLength;
  int numOfCharsUsed;
  char escchar;

  // Gets longer only when % is present
  formattedString = (LPSTR)malloc(strlen(inputString) + 1);

  if (formattedString == NULL){
    crashAndBurn("Out of memory - formattedString", 
                 UNKNOWN_LINE_NUMBER, "", "");
  }

  stringLength = (int)strlen(inputString);

  for (i = 0, j = 0; i < stringLength; i++, j++){
      
    if ((int) inputString[i] == '\\'){
      //replace with escape character
      numOfCharsUsed = getEscapeCharacter(&inputString[i+1], &escchar);
      if (numOfCharsUsed == 0){
        free(formattedString);
        return NULL;
      }
      i += numOfCharsUsed;
      inputString[i] = escchar;
    } 

    if (inputString[i] == '\"' && inputString[i+1] == '\"') {
      // replace "" with "
      formattedString[j] = inputString[i];
      i++;
    }
    else {
      //use character given
      formattedString[j] = inputString[i];
    }
  }
  formattedString[j] = '\0';
  
  return formattedString;
}

///////////////////////////////////////////////////////////////////////////////
//  
//       changeExtension
//
// Changes the extension from old to new, or appends new filename didn't
// have extension old
//
// Parameters:
//   filename: filename on which to change extension
//   oldExt:      old extension
//   newExt:      new extension
//
// Returns:
//   Pointer to a string containing the new filename
// 
///////////////////////////////////////////////////////////////////////////////
LPSTR changeExtension(LPSTR filename, LPSTR oldExt, LPSTR newExt)
{

  LPSTR returnValue;
  LPSTR tempString;

  returnValue = (LPSTR)malloc(strlen(filename) + strlen(newExt));
  if (returnValue == NULL)
    crashAndBurn("Out of memory - returnValue", UNKNOWN_LINE_NUMBER, "", "");

  returnValue = strcpy(returnValue, filename);

  tempString = strstr(returnValue, oldExt);
  if (tempString != NULL)
    tempString[0] = '\0';
  returnValue = strcat(returnValue, newExt);

  return(returnValue);

}


///////////////////////////////////////////////////////////////////////////////
//  
//        steCompare
//      (for use with qsort)
//
// Parameters:
//   first:  pointer to first STRING_TABLE_ENTRY to be compared
//   second: pointer to second STRING_TABLE_ENTRY to be compared
//
// Returns:
//   negative value if first->StringID < second->StringID
//   zero if first.StringID = second.StringID
//   positive value if first->StringID > second->StringID
//
///////////////////////////////////////////////////////////////////////////////

int __cdecl steCompare(const void *first, const void *second){

  int result = 0;

  if (((STRING_TABLE_ENTRY * )first)->StringID > 
      ((STRING_TABLE_ENTRY * )second)->StringID)
    result = 1;
  else if (((STRING_TABLE_ENTRY * )first)->StringID == 
           ((STRING_TABLE_ENTRY * )second)->StringID)
    result = 0;
  else
    result = -1;

  return result;
}

///////////////////////////////////////////////////////////////////////////////
//  
//        updateLineNumber
//
// Parameters:
//   line:       line of the format #line linenumber filename.  To be used to
//               determine new values for lineNumber and lineFile.
//   lineNumber: pointer to the int which holds current line number
//   lineFile:   pointer to the LPSTR which holds the current file 
//               corresponding to lineNumber
//
///////////////////////////////////////////////////////////////////////////////

void updateLineNumber(LPSTR line, int * lineNumber, LPSTR * lineFile){
  
  LPSTR lineNumberString;
  LPSTR linetotok;
  LPSTR fileToken;
  LPSTR fileTokenCopy;

  linetotok = (LPSTR)malloc(strlen(line) + 1);
  if (linetotok == NULL)
    crashAndBurn("Out of memory - linetotok", UNKNOWN_LINE_NUMBER, "", "");
  strcpy(linetotok, line);

  strtok(linetotok, " "); ///removes #line

  lineNumberString = strtok(NULL, "\"");
  *lineNumber = strtoul(lineNumberString, NULL, 0) - 1;
  
  fileToken = strtok(NULL, "\"");
  fileTokenCopy = (LPSTR)malloc (strlen(fileToken) + 1);
  if (fileTokenCopy == NULL)
   crashAndBurn("Out of memory - fileTokenCopy", UNKNOWN_LINE_NUMBER, "", "");
  strcpy(fileTokenCopy, fileToken);

  if (*lineFile != NULL) {
    free(*lineFile);
  }
  *lineFile = FormatString(fileTokenCopy);
  free(fileTokenCopy);

  if (*lineFile == NULL){
    printf(line);
    crashAndBurn("Error in format of preprocessed file",
                 UNKNOWN_LINE_NUMBER, "", "");
  }

  if (*lineFile == NULL){
    crashAndBurn("Out of memory - lineFile", UNKNOWN_LINE_NUMBER, "", "");
  }

  free(linetotok);
}
///////////////////////////////////////////////////////////////////////////////
//  
//        clearBlanksAndLineUpdates
//
// Parameters:
//   file:         pointer to file from which the lines are to be taken
//   lineNumber:   pointer to the int which holds current line number
//   lineFile:     pointer to the LPSTR which holds the current file 
//                 corresponding to lineNumber
//   textline:     buffer for lines read from file.  if checkCurrent is true, 
//                 will be read as first line to be examined, at return, 
//                 will hold the value of the first non blank or 
//                 line updating line.
//   checkCurrent: if true, the value of textline when the function is called
//                 will be checked for blankness or line update.  otherwise the
//                 initial string in textline will be ignored
//
///////////////////////////////////////////////////////////////////////////////
void clearBlanksAndLineUpdates(FILE * file, 
                               int * lineNumber, 
                               LPSTR * lineFile, 
                               LPSTR * textline,
                               BOOL checkCurrent)
{
  //LOOP TO REMOVE EFFECTIVELY BLANK LINES, AND #line LINES
  do{

    //IF checkCurrent WAS SET TO TRUE, DON'T READ LINE FIRST TIME

    if(!checkCurrent){
      if(fgets(*textline, BUF_SIZE, file) == NULL){
        *textline = NULL;
        break;
      }
      else{
        (*lineNumber)++;
      }
    }
    else{
      checkCurrent = FALSE;
    }

    // Only recognize the prefix at the start of the line
    if(startsWithLineMarker(*textline))
      updateLineNumber(*textline, lineNumber, lineFile);

  }while((strspn(*textline, " \t\n") == strlen(*textline))
         || startsWithLineMarker(*textline));

}

///////////////////////////////////////////////////////////////////////////////
//  
//        startsWithLineMarker
//
// Parameters:
//   line:  a line in a preprocessed file
//
// Returns:
//   Whether the line starts with a preprocessor line directive
//
///////////////////////////////////////////////////////////////////////////////


BOOL startsWithLineMarker(const char *line)
{
#ifdef __GNUC__
    return (line[0] == LINEMARKER_PREFIX && (line[1] == ' ' || isdigit(line[1])));
#else
    return (strstr(line, (const char*)LINEMARKER_PREFIX) == line);
#endif
}

///////////////////////////////////////////////////////////////////////////////
//  
//        getEscapeCharacter
//
// Parameters:
//   input:  pointer to string of characters to be interpreted as escape 
//           sequence
//   output: pointer to location in which to store escape character
//
// Returns:
//   Number of characters used to form escape sequence if successfull,
//   0 if string could not be interpreted
//
///////////////////////////////////////////////////////////////////////////////


int getEscapeCharacter (char * input, char * output){
 
  int i;
  int value;
  int c;

  switch(*input){
  case 'x':
    for (i = 0, value = 0; isxdigit(input[i]); i++){
      c = input[i];
      if (isupper(c)){
        c -= ('A' - 10);
      }
      else if (islower(c)){
        c -= ('a' - 10);
      }
      else{
        c -= '0';
      }
      value = (16 * value) + c;
    }
    if(!i){
      fprintf(stderr, "Bad Hex value while converting escape character\n");
      return 0;
    }
    else if (value < 255){
      fprintf(stderr, 
              "Out of range Hex value while converting escape character\n");
      return 0;
    }

    *output = (char) value;
    return i + 1;
    
  case '0':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
    for ( i = 0, value = 0; 
         input[i] <= '7' && input[i] >= '0' && i <= 3;
         i++){
      value = (8 * value) + (input[i] - '0');
    }
    if (!i){
      printf("Bad Octal value while converting escape character\n");
      return 0;
    }
    else if (value > 255){
      printf("Out of range Octal value while converting escape character\n");
      return 0;
    }
    
    *output = (char) value;
    return i + 1;

  case 'a':
    *output = '\a';
    return 1;
  case 'b':
    *output = '\b';
    return 1;
  case 'f':
    *output = '\f';
    return 1;
  case 'n':
    *output = '\n';
    return 1;
  case 'r':
    *output = '\r';
    return 1;
  case 't':
    *output = '\t';
    return 1;
  case 'v':
    *output = '\v';
    return 1;
  case '?':
  case '\\':
  case '\'':
    *output = *input;
    return 1;

  default: 
    return 0;
  }
}
