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

namespace Microsoft.JScript{

    using Microsoft.JScript.Vsa;
    using System;
    using System.Collections;
    using System.Globalization;
    using System.Reflection;
    using Microsoft.Vsa;
    using System.Text;

    public sealed class JSScanner{
      // scanner main data
      private string strSourceCode;
      private int startPos;
      private int endPos;
      private int currentPos;
      private int currentLine;
      private int startLinePos;

      // token information
      private Context currentToken;
      private String escapedString;
      private StringBuilder identifier;
      private int idLastPosOnBuilder;

      // flags
      private bool gotEndOfLine;
      private bool IsAuthoring;
      private bool peekModeOn;
      private bool scanForDebugger;

      // keyword table
      private JSKeyword[] keywords;
      private static readonly JSKeyword[] s_Keywords = JSKeyword.InitKeywords();

      // pre process information
      private bool preProcessorOn;
      private int matchIf;
      private Object preProcessorValue;
      private SimpleHashtable ppTable;

      // environmental information
      private DocumentContext currentDocument;
      private Globals globals;

      public JSScanner(){
        this.keywords = JSScanner.s_Keywords;
        this.strSourceCode = null;
        this.startPos = 0;
        this.endPos = 0;
        this.currentPos = 0;
        this.currentLine = 1;
        this.startLinePos = 0;
        this.currentToken = null;
        this.escapedString = null;
        this.identifier = new StringBuilder(128);
        this.idLastPosOnBuilder = 0;
        this.gotEndOfLine = false;
        this.IsAuthoring = false;
        this.peekModeOn = false;
        this.preProcessorOn = false;
        this.matchIf = 0;
        this.ppTable = null;
        this.currentDocument = null;
        this.globals = null;
        this.scanForDebugger = false;
      }

      public JSScanner(Context sourceContext){
        this.IsAuthoring = false;
        this.peekModeOn = false;
        this.keywords = s_Keywords;
        this.preProcessorOn = false;
        this.matchIf = 0;
        this.ppTable = null;
        this.SetSource(sourceContext);
        this.currentDocument = null;
        this.globals = sourceContext.document.engine.Globals;
      }

      public void SetAuthoringMode(bool mode){
        this.IsAuthoring = mode;
      }

      public void SetSource(Context sourceContext){
        this.strSourceCode = sourceContext.source_string;
        this.startPos = sourceContext.startPos;
        this.startLinePos = sourceContext.startLinePos;
        this.endPos = (0 < sourceContext.endPos && sourceContext.endPos < this.strSourceCode.Length) ?
                                                                              sourceContext.endPos :
                                                                              this.strSourceCode.Length;
        this.currentToken = sourceContext;
        this.escapedString = null;
        this.identifier = new StringBuilder(128);
        this.idLastPosOnBuilder = 0;
        this.currentPos = this.startPos;
        this.currentLine = (sourceContext.lineNumber > 0) ? sourceContext.lineNumber : 1;
        this.gotEndOfLine = false;
        this.scanForDebugger = sourceContext.document != null && sourceContext.document.engine != null && VsaEngine.executeForJSEE;
      }

      internal JSToken PeekToken(){
        int currentPos = this.currentPos;
        int currentLine = this.currentLine;
        int startLinePos = this.startLinePos;
        bool gotEndOfLine = this.gotEndOfLine;
        int idLastPosOnBuilder = this.idLastPosOnBuilder;
        this.peekModeOn = true;
        JSToken token = JSToken.None;
        // temporary switch the token
        Context currentToken = this.currentToken;
        this.currentToken = this.currentToken.Clone();
        try{
          GetNextToken();
          token = this.currentToken.token;
        }finally{
          this.currentToken = currentToken;
          this.currentPos = currentPos;
          this.currentLine = currentLine;
          this.startLinePos = startLinePos;
          this.gotEndOfLine = gotEndOfLine;
          this.identifier.Length = 0;
          this.idLastPosOnBuilder = idLastPosOnBuilder;
          this.peekModeOn = false;
          this.escapedString = null;
        }
        return token;
      }

      public void GetNextToken(){
        JSToken token = JSToken.None;
        this.gotEndOfLine = false;
        try{
          int currentLine = this.currentLine;
        nextToken:
          this.SkipBlanks();
          this.currentToken.startPos = this.currentPos;
          this.currentToken.lineNumber = this.currentLine;
          this.currentToken.startLinePos = this.startLinePos;
          char c = GetChar(this.currentPos++);
          switch (c){
            case (char)0:
              if (this.currentPos >= this.endPos){
                this.currentPos--;
                token = JSToken.EndOfFile;
                if (this.matchIf > 0){
                  this.currentToken.endLineNumber = this.currentLine;
                  this.currentToken.endLinePos = this.startLinePos;
                  this.currentToken.endPos = this.currentPos;
                  HandleError(JSError.NoCcEnd);
                }
                break;
              }
              goto nextToken;
            case '=':
              token = JSToken.Assign;
              if ('=' == GetChar(this.currentPos)){
                this.currentPos++;
                token = JSToken.Equal;
                if ('=' == GetChar(this.currentPos)){
                  this.currentPos++;
                  token = JSToken.StrictEqual;
                }
              }
              break;
            case '>':
              token = JSToken.GreaterThan;
              if ('>' == GetChar(this.currentPos)){
                this.currentPos++;
                token = JSToken.RightShift;
                if ('>' == GetChar(this.currentPos)){
                  this.currentPos++;
                  token = JSToken.UnsignedRightShift;
                }
              }
              if ('=' == GetChar(this.currentPos)){
                this.currentPos++;
                switch (token){
                case JSToken.GreaterThan:
                  token = JSToken.GreaterThanEqual;
                  break;
                case JSToken.RightShift:
                  token = JSToken.RightShiftAssign;
                  break;
                case JSToken.UnsignedRightShift:
                  token = JSToken.UnsignedRightShiftAssign;
                  break;
                }
              }
              break;
            case '<':
              token = JSToken.LessThan;
              if ('<' == GetChar(this.currentPos)){
                this.currentPos++;
                token = JSToken.LeftShift;
              }
              if ('=' == GetChar(this.currentPos)){
                this.currentPos++;
                if (token == JSToken.LessThan)
                  token = JSToken.LessThanEqual;
                else
                  token = JSToken.LeftShiftAssign;
              }
              break;
            case '!':
              token = JSToken.LogicalNot;
              if ('=' == GetChar(this.currentPos)){
                this.currentPos++;
                token = JSToken.NotEqual;
                if ('=' == GetChar(this.currentPos)){
                  this.currentPos++;
                  token = JSToken.StrictNotEqual;
                }
              }
              break;
            case ',':
              token = JSToken.Comma;
              break;
            case '~':
              token = JSToken.BitwiseNot;
              break;
            case '?':
              token = JSToken.ConditionalIf;
              break;
            case ':':
              token = JSToken.Colon;
              if (':' == GetChar(this.currentPos)){
                this.currentPos++;
                token = JSToken.DoubleColon;
              }
              break;
            case '.':
              token = JSToken.AccessField;
              c = GetChar(this.currentPos);
              if (JSScanner.IsDigit(c)){
                token = ScanNumber('.');
              }else if ('.' == c){
                c = GetChar(this.currentPos+1);
                if ('.' == c){
                  this.currentPos += 2;
                  token = JSToken.ParamArray;
                }
              }
              break;
            case '&':
              token = JSToken.BitwiseAnd;
              c = GetChar(this.currentPos);
              if ('&' == c){
                this.currentPos++;
                token = JSToken.LogicalAnd;
              }else if ('=' == c){
                this.currentPos++;
                token = JSToken.BitwiseAndAssign;
              }
              break;
            case '|':
              token = JSToken.BitwiseOr;
              c = GetChar(this.currentPos);
              if ('|' == c){
                this.currentPos++;
                token = JSToken.LogicalOr;
              }else if ('=' == c){
                this.currentPos++;
                token = JSToken.BitwiseOrAssign;
              }
              break;
            case '+':
              token = JSToken.Plus;
              c = GetChar(this.currentPos);
              if ('+' == c){
                this.currentPos++;
                token = JSToken.Increment;
              }else if ('=' == c){
                this.currentPos++;
                token = JSToken.PlusAssign;
              }
              break;
            case '-':
              token = JSToken.Minus;
              c = GetChar(this.currentPos);
              if ('-' == c){
                this.currentPos++;
                token = JSToken.Decrement;
              }else if ('=' == c){
                this.currentPos++;
                token = JSToken.MinusAssign;
              }
              break;
            case '*':
              token = JSToken.Multiply;
              if ('=' == GetChar(this.currentPos)){
                this.currentPos++;
                token = JSToken.MultiplyAssign;
              }
              break;
            case '\\':
              this.currentPos--;
              if (IsIdentifierStartChar(ref c)){
                this.currentPos++;
                ScanIdentifier();
                token = JSToken.Identifier;
                break;
              }
              this.currentPos++; // move on
              c = GetChar(this.currentPos);
              if ('a' <= c && c <= 'z'){
                JSKeyword keyword =  this.keywords[c - 'a'];
                if (null != keyword){
                  this.currentToken.startPos++;
                  token = ScanKeyword(keyword);
                  if (token != JSToken.Identifier){
                    token = JSToken.Identifier;
                    break;
                  }
                  this.currentToken.startPos--;
                }
              }
              this.currentPos = this.currentToken.startPos+1;
              HandleError(JSError.IllegalChar);
              break;
            case '/':
              token = JSToken.Divide;
              c = GetChar(this.currentPos);
              bool preProcess = false;
              switch (c){
                case '/':
                  if (GetChar(++this.currentPos) == '@' && !this.peekModeOn){
                    // we got //@
                    if (!this.preProcessorOn){
                      // preprocessing was not on, check if it's going to be turned on
                      if ('c' == GetChar(++this.currentPos) &&
                          'c' == GetChar(++this.currentPos) &&
                          '_' == GetChar(++this.currentPos) &&
                          'o' == GetChar(++this.currentPos) &&
                          'n' == GetChar(++this.currentPos)){
                        char nextChar = GetChar(this.currentPos + 1);
                        if (!JSScanner.IsDigit(nextChar) && !JSScanner.IsAsciiLetter(nextChar) && !JSScanner.IsUnicodeLetter(nextChar)){
                          SetPreProcessorOn();
                          this.currentPos++;
                          goto nextToken;
                        }
                      }
                      // it will fall into a "standard" comment processing
                    }else{
                      // if it is a space we got '//@ ' that has to be ignored
                      if (JSScanner.IsBlankSpace(GetChar(++this.currentPos)))
                        goto nextToken;
                      preProcess = true;
                      break;
                    }
                  }
                  SkipSingleLineComment();
                  if (this.IsAuthoring){
                    token = JSToken.Comment;
                    break; // just break out and return
                  }else
                    goto nextToken; // read another token this last one was a comment
                case '*':
                  if (GetChar(++this.currentPos) == '@' && !this.peekModeOn){
                    if (!this.preProcessorOn){
                      if ('c' == GetChar(++this.currentPos) &&
                          'c' == GetChar(++this.currentPos) &&
                          '_' == GetChar(++this.currentPos) &&
                          'o' == GetChar(++this.currentPos) &&
                          'n' == GetChar(++this.currentPos)){
                        char nextChar = GetChar(this.currentPos + 1);
                        if (!JSScanner.IsDigit(nextChar) && !JSScanner.IsAsciiLetter(nextChar) && !JSScanner.IsUnicodeLetter(nextChar)){
                          SetPreProcessorOn();
                          this.currentPos++;
                          goto nextToken;
                        }
                      }
                    }else{
                      // if it is a space we got '/*@ ' that has to be ignored
                      if (JSScanner.IsBlankSpace(GetChar(++this.currentPos)))
                        goto nextToken;
                      preProcess = true;
                      break;
                    }
                  }

                  SkipMultiLineComment();
                  if (this.IsAuthoring){
                    if (this.currentPos > this.endPos){
                      token = JSToken.UnterminatedComment;
                      this.currentPos = this.endPos;
                    }else
                      token = JSToken.Comment;
                    break;
                  }else
                    goto nextToken; // read another token this last one was a comment
                case '=':
                  this.currentPos++;
                  token = JSToken.DivideAssign;
                  break;
              }
              if (preProcess)
                goto case '@';

              break;
            case '^':
              token = JSToken.BitwiseXor;
              if ('=' == GetChar(this.currentPos)){
                this.currentPos++;
                token = JSToken.BitwiseXorAssign;
              }
              break;
            case '%':
              token = JSToken.Modulo;
              if ('=' == GetChar(this.currentPos)){
                this.currentPos++;
                token = JSToken.ModuloAssign;
              }
              break;
            case '(':
              token = JSToken.LeftParen;
              break;
            case ')':
              token = JSToken.RightParen;
              break;
            case '{':
              token = JSToken.LeftCurly;
              break;
            case '}':
              token = JSToken.RightCurly;
              break;
            case '[':
              token = JSToken.LeftBracket;
              break;
            case ']':
              token = JSToken.RightBracket;
              break;
            case ';':
              token = JSToken.Semicolon;
              break;
            case '"':
              goto case '\'';
            case '\'':
              token = JSToken.StringLiteral;
              ScanString(c);
              break;

            case '\r':
              if (GetChar(this.currentPos) == '\n')
                this.currentPos++;
              this.currentLine++;
              this.startLinePos = this.currentPos;
              goto nextToken;
            case '\n':
              this.currentLine++;
              this.startLinePos = this.currentPos;
              goto nextToken;
            case (char)0x2028:
              this.currentLine++;
              this.startLinePos = this.currentPos;
              goto nextToken;
            case (char)0x2029:
              this.currentLine++;
              this.startLinePos = this.currentPos;
              goto nextToken;

            //The sick beauty of pre processing
            case '@':
              if (this.scanForDebugger)
                HandleError(JSError.CcInvalidInDebugger);
              if (this.peekModeOn){
                this.currentToken.token = JSToken.PreProcessDirective;
                break;
              }
              int startPos = this.currentPos;
              this.currentToken.startPos = startPos;
              this.currentToken.lineNumber = this.currentLine;
              this.currentToken.startLinePos = this.startLinePos;
              ScanIdentifier();
              switch (this.currentPos - startPos){
              case 0:
                // Ignore '@*/'.
                if (this.preProcessorOn && '*' == GetChar(this.currentPos) && '/' == GetChar(++this.currentPos))
                    this.currentPos++;
                else
                    HandleError(JSError.IllegalChar);
                goto nextToken;
              case 2:
                if ('i' == this.strSourceCode[startPos] &&
                    'f' == this.strSourceCode[startPos + 1]){
                  if (!this.preProcessorOn)
                    SetPreProcessorOn();
                  this.matchIf++;
                  if (!PPTestCond())
                    PPSkipToNextCondition(true);
                  goto nextToken;
                }
                goto default;
              case 3:
                if ('s' == this.strSourceCode[startPos] &&
                    'e' == this.strSourceCode[startPos + 1] &&
                    't' == this.strSourceCode[startPos + 2]){
                  if (!this.preProcessorOn)
                      SetPreProcessorOn();
                  PPScanSet();
                  goto nextToken;
                }else if ('e' == this.strSourceCode[startPos] &&
                         'n' == this.strSourceCode[startPos + 1] &&
                         'd' == this.strSourceCode[startPos + 2]){
                  if (0 >= this.matchIf)
                    HandleError(JSError.CcInvalidEnd);
                else
                  this.matchIf--;
                  goto nextToken;
                }
                goto default;
              case 4:
                if ('e' == this.strSourceCode[startPos] &&
                    'l' == this.strSourceCode[startPos + 1] &&
                    's' == this.strSourceCode[startPos + 2] &&
                    'e' == this.strSourceCode[startPos + 3]){
                  if (0 >= this.matchIf)
                    HandleError(JSError.CcInvalidElse);
                  else
                    PPSkipToNextCondition(false);
                  goto nextToken;
                }else if ('e' == this.strSourceCode[startPos] &&
                         'l' == this.strSourceCode[startPos + 1] &&
                         'i' == this.strSourceCode[startPos + 2] &&
                         'f' == this.strSourceCode[startPos + 3]){
                  if (0 >= this.matchIf)
                    HandleError(JSError.CcInvalidElif);
                  else
                    PPSkipToNextCondition(false);
                  goto nextToken;
                }
                goto default;
              case 5:
                if ('c' == GetChar(startPos) &&
                    'c' == GetChar(startPos + 1) &&
                    '_' == GetChar(startPos + 2) &&
                    'o' == GetChar(startPos + 3) &&
                    'n' == GetChar(startPos + 4)){
                  if (!this.preProcessorOn)
                    SetPreProcessorOn();
                  goto nextToken;
                }
                goto default;
              default:
                if (!this.preProcessorOn){
                  HandleError(JSError.CcOff);
                  goto nextToken;
                }
                Object value = this.ppTable[this.strSourceCode.Substring(startPos, this.currentPos - startPos)];
                if (null == value)
                  this.preProcessorValue = Double.NaN;
                else
                  this.preProcessorValue = value;
                token = JSToken.PreProcessorConstant;
                break;
              }
              break;

            case '$':
              goto case '_';
            case '_':
              ScanIdentifier();
              token = JSToken.Identifier;
              break;
            default:
              if ('a' <= c && c <= 'z'){
                JSKeyword keyword =  this.keywords[c - 'a'];
                if (null != keyword)
                  token = ScanKeyword(keyword);
                else{
                  token = JSToken.Identifier;
                  ScanIdentifier();
                }
              }else if (JSScanner.IsDigit(c)){
                token = ScanNumber(c);
              }else if ('A' <= c && c <= 'Z' || JSScanner.IsUnicodeLetter(c)){
                token = JSToken.Identifier;
                ScanIdentifier();
              }else{
                HandleError(JSError.IllegalChar);
                goto nextToken;
              }
              break;
          }
          this.currentToken.endLineNumber = this.currentLine;
          this.currentToken.endLinePos = this.startLinePos;
          this.currentToken.endPos = this.currentPos;
          this.gotEndOfLine = (this.currentLine > currentLine || token == JSToken.EndOfFile) ? true : false;
          if (this.gotEndOfLine && token == JSToken.StringLiteral && this.currentToken.lineNumber == currentLine)
            this.gotEndOfLine = false;
        }catch(IndexOutOfRangeException){
          token = JSToken.None;
          this.currentToken.endPos = this.currentPos;
          this.currentToken.endLineNumber = this.currentLine;
          this.currentToken.endLinePos = this.startLinePos;
          throw new ScannerException(JSError.ErrEOF);
        }
        this.currentToken.token = token;
      }

      private char GetChar(int index){
        if (index < this.endPos)
          return this.strSourceCode[index];
        else
          return (char)0;
      }

      public int GetCurrentPosition(bool absolute){
        return this.currentPos;
      }

      public int GetCurrentLine(){
        return this.currentLine;
      }

      public int GetStartLinePosition(){
        return this.startLinePos;
      }

      public string GetStringLiteral(){
        return this.escapedString;
      }

      public string GetSourceCode(){
        return this.strSourceCode;
      }

      public bool GotEndOfLine() {
        return this.gotEndOfLine;
      }

      internal String GetIdentifier(){
        String id = null;
        if (this.identifier.Length > 0){
          id = this.identifier.ToString();
          this.identifier.Length = 0;
        }else
          id = this.currentToken.GetCode();
        if (id.Length > 500) //The EE sometimes gets into trouble if presented with a name > 1023 bytes, make this less likely
          id = id.Substring(0, 500) + id.GetHashCode().ToString(CultureInfo.InvariantCulture);
        return id;
      }

      private void ScanIdentifier(){
        for(;;){
          char c = GetChar(this.currentPos);
          if (!IsIdentifierPartChar(c))
            break;
          ++this.currentPos;
        }
        if (this.idLastPosOnBuilder > 0){
          this.identifier.Append(this.strSourceCode.Substring(this.idLastPosOnBuilder, this.currentPos - this.idLastPosOnBuilder));
          this.idLastPosOnBuilder = 0;
        }
      }

      private JSToken ScanKeyword(JSKeyword keyword){
        for(;;){
          char c = GetChar(this.currentPos);
          if ('a' <= c && c <= 'z'){
            this.currentPos++;
            continue;
          }else{
            if (IsIdentifierPartChar(c)){
              ScanIdentifier();
              return JSToken.Identifier;
            }
            break;
          }
        }
        return keyword.GetKeyword(this.currentToken, this.currentPos - this.currentToken.startPos);
      }

      private JSToken ScanNumber(char leadChar){
        bool noMoreDot = '.' == leadChar;
        JSToken token = noMoreDot ? JSToken.NumericLiteral : JSToken.IntegerLiteral;
        bool exponent = false;
        char c;

        if ('0' == leadChar){
          c = GetChar(this.currentPos);
          if ('x' == c || 'X' == c){
            if (!JSScanner.IsHexDigit(GetChar(this.currentPos + 1)))
              HandleError(JSError.BadHexDigit);
            while (JSScanner.IsHexDigit(GetChar(++this.currentPos)));
            return token;
          }
        }

        for (;;){
          c = GetChar(this.currentPos);
          if (!JSScanner.IsDigit(c)){
            if ('.' == c){
              if (noMoreDot)
                break;
              else{
                noMoreDot = true;
                token = JSToken.NumericLiteral;
              }
            }else if ('e' == c || 'E' == c){
              if (exponent)
                break;
              else{
                exponent = true;
                token = JSToken.NumericLiteral;
              }
            }else if ('+' == c || '-' == c){
              char e = GetChar(this.currentPos - 1);
              if ('e' != e && 'E' != e)
                break;
            }else
              break;
          }
          this.currentPos++;
        }

        c = GetChar(this.currentPos - 1);
        if ('+' == c || '-' == c){
          this.currentPos--;
          c = GetChar(this.currentPos - 1);
        }
        if ('e' == c || 'E' == c){
          this.currentPos--;
        }

        return token;
      }

      internal String ScanRegExp(){
        int pos = this.currentPos;
        bool isEscape = false;
        char c;
        while (!IsEndLineOrEOF(c = GetChar(this.currentPos++), 0)) {
          if (isEscape)
            isEscape = false;
          else{
            if (c == '/') {
              if (pos == this.currentPos) return null;
              this.currentToken.endPos = this.currentPos;
              this.currentToken.endLinePos = this.startLinePos;
              this.currentToken.endLineNumber = this.currentLine;
              return this.strSourceCode.Substring(
                this.currentToken.startPos + 1,
                this.currentToken.endPos - this.currentToken.startPos - 2);
            }
            if (c == '\\') isEscape = true;
          }
        }
        // reset and return null. Assume it is not a reg exp
        this.currentPos = pos;
        return null;
      }

      internal String ScanRegExpFlags(){
        int pos = this.currentPos;
        while (JSScanner.IsAsciiLetter(GetChar(this.currentPos)))
          this.currentPos++;
        if (pos != this.currentPos){
          this.currentToken.endPos = this.currentPos;
          this.currentToken.endLineNumber = this.currentLine;
          this.currentToken.endLinePos = this.startLinePos;
          return this.strSourceCode.Substring(pos, this.currentToken.endPos - pos);
        }
        return null;
      }

      //--------------------------------------------------------------------------------------------------
      // ScanString
      //
      //  Scan a string dealing with escape sequences.
      //  On exit this.escapedString will contain the string with all escape sequences replaced
      //  On exit this.currentPos must be at the next char to scan after the string
      //  This method wiil report an error when the string is unterminated or for a bad escape sequence
      //--------------------------------------------------------------------------------------------------
      private void ScanString(char cStringTerminator){
        char ch;
        int start = this.currentPos;
        this.escapedString = null;
        StringBuilder result = null;
        do{
          ch = GetChar(this.currentPos++);

          if (ch != '\\'){
            // this is the common non escape case
            if (IsLineTerminator(ch, 0)){
              HandleError(JSError.UnterminatedString);
              this.currentPos--;
              //this.currentLine++;
              //this.startLinePos = this.currentPos;
              break;
            }else if ((char)0 == ch){
              this.currentPos--;
              HandleError(JSError.UnterminatedString);
              break;
            }
          }else{
            // ESCAPE CASE

            // got an escape of some sort. Have to use the StringBuilder
            if (null == result)
              result = new StringBuilder(128);

            // start points to the first position that has not been written to the StringBuilder.
            // The first time we get in here that position is the beginning of the string, after that
            // is the character immediately following the escape sequence
            if (this.currentPos - start - 1 > 0) {
              // append all the non escape chars to the string builder
              result.Append(this.strSourceCode, start, this.currentPos - start - 1);
            }

            // state variable to be reset
            bool seqOfThree = false;
            int esc = 0;

            ch = GetChar(this.currentPos++);
            switch(ch) {

            case '\r':
              if('\n' == GetChar(this.currentPos)){
                this.currentPos++;
              }
              goto case '\n';
            case '\n':
            case (char)0x2028:
            case (char)0x2029:
              this.currentLine++;
              this.startLinePos = this.currentPos;
              break;

            // classic single char escape sequences
            case 'b':
              result.Append((char)8);
              break;
            case 't':
              result.Append((char)9);
              break;
            case 'n':
              result.Append((char)10);
              break;
            case 'v':
              result.Append((char)11);
              break;
            case 'f':
              result.Append((char)12);
              break;
            case 'r':
              result.Append((char)13);
              break;
            case '"':
              result.Append('"');
              ch = (char)0; // so it does not exit the loop
              break;
            case '\'':
              result.Append('\'');
              ch = (char)0; // so it does not exit the loop
              break;
            case '\\':
              result.Append('\\');
              break;

            // hexadecimal escape sequence /xHH
            case 'x':
              ch = GetChar(this.currentPos++);
              if (unchecked((UInt32)(ch - '0')) <= '9' - '0')
                esc = (ch - '0') << 4;
              else if (unchecked((UInt32)(ch -'A')) <= 'F' - 'A')
                esc = (ch + 10 - 'A') << 4;
              else if (unchecked((UInt32)(ch - 'a')) <= 'f' - 'a')
                esc = (ch + 10 - 'a') << 4;
              else{
                HandleError(JSError.BadHexDigit);
                if (ch != cStringTerminator)
                  --this.currentPos; // do not skip over this char we have to read it back
                break;
              }
              ch = GetChar(this.currentPos++);
              if (unchecked((UInt32)(ch - '0')) <= '9' - '0')
                esc |= (ch - '0');
              else if (unchecked((UInt32)(ch - 'A')) <= 'F' - 'A')
                esc |= (ch + 10 - 'A');
              else if (unchecked((UInt32)(ch - 'a')) <= 'f' - 'a')
                esc |= (ch + 10 - 'a');
              else{
                HandleError(JSError.BadHexDigit);
                if (ch != cStringTerminator)
                  --this.currentPos; // do not skip over this char we have to read it back
                break;
              }
              result.Append((char)esc);
              break;

            // unicode escape sequence /uHHHH
            case 'u':
              ch = GetChar(this.currentPos++);
              if (unchecked((UInt32)(ch - '0')) <= '9' - '0')
                esc = (ch - '0') << 12;
              else if (unchecked((UInt32)(ch - 'A')) <= 'F' - 'A')
                esc = (ch + 10 - 'A') << 12;
              else if (unchecked((UInt32)(ch - 'a')) <= 'f' - 'a')
                esc = (ch + 10 - 'a') << 12;
              else{
                HandleError(JSError.BadHexDigit);
                if (ch != cStringTerminator)
                  --this.currentPos; // do not skip over this char we have to read it back
                break;
              }
              ch = GetChar(this.currentPos++);
              if (unchecked((UInt32)(ch - '0')) <= '9' - '0')
                esc |= (ch - '0') << 8;
              else if (unchecked((UInt32)(ch - 'A')) <= 'F' - 'A')
                esc |= (ch + 10 - 'A') << 8;
              else if (unchecked((UInt32)(ch - 'a')) <= 'f' - 'a')
                esc |= (ch + 10 - 'a') << 8;
              else{
                HandleError(JSError.BadHexDigit);
                if (ch != cStringTerminator)
                  --this.currentPos; // do not skip over this char we have to read it back
                break;
              }
              ch = GetChar(this.currentPos++);
              if (unchecked((UInt32)(ch - '0')) <= '9' - '0')
                esc |= (ch - '0') << 4;
              else if (unchecked((UInt32)(ch - 'A')) <= 'F' - 'A')
                esc |= (ch + 10 - 'A') << 4;
              else if (unchecked((UInt32)(ch - 'a')) <= 'f' - 'a')
                esc |= (ch + 10 - 'a') << 4;
              else{
                HandleError(JSError.BadHexDigit);
                if (ch != cStringTerminator)
                  --this.currentPos; // do not skip over this char we have to read it back
                break;
              }
              ch = GetChar(this.currentPos++);
              if (unchecked((UInt32)(ch - '0')) <= '9' - '0')
                esc |= (ch - '0');
              else if (unchecked((UInt32)(ch - 'A')) <= 'F' - 'A')
                esc |= (ch + 10 - 'A');
              else if (unchecked((UInt32)(ch - 'a')) <= 'f' - 'a')
                esc |= (ch + 10 - 'a');
              else{
                HandleError(JSError.BadHexDigit);
                if (ch != cStringTerminator)
                  --this.currentPos; // do not skip over this char we have to read it back
                break;
              }
              result.Append((char)esc);
              break;

            case '0':
            case '1':
            case '2':
            case '3':
              seqOfThree = true;
              esc = (ch - '0') << 6;
              goto case '4';
            case '4':
            case '5':
            case '6':
            case '7':
              // esc is reset at the beginning of the loop and it is used to check that we did not go through the cases 1, 2 or 3
              if (!seqOfThree)
                esc = (ch - '0') << 3;

              ch = GetChar(this.currentPos++);
              if (unchecked((UInt32)(ch - '0')) <= '7' - '0'){
                if (seqOfThree){
                  esc |= (ch - '0') << 3;
                  ch = GetChar(this.currentPos++);
                  if (unchecked((UInt32)(ch - '0')) <= '7' - '0'){
                    esc |= ch - '0';
                    result.Append((char)esc);
                  }else{
                    result.Append((char)(esc >> 3));
                    if (ch != cStringTerminator)
                      --this.currentPos; // do not skip over this char we have to read it back
                  }
                }else{
                  esc |= ch - '0';
                  result.Append((char)esc);
                }
              }else{
                if (seqOfThree)
                  result.Append((char)(esc >> 6));
                else
                  result.Append((char)(esc >> 3));
                if (ch != cStringTerminator)
                  --this.currentPos; // do not skip over this char we have to read it back
              }
              break;

            default:
              // not an octal number, ignore the escape '/' and simply append the current char
              result.Append(ch);
              break;
            }
            start = this.currentPos;
          }
        }while (ch != cStringTerminator);

        // update this.escapedString
        if (null != result){
          if (this.currentPos - start - 1 > 0){
            // append all the non escape chars to the string builder
            result.Append(this.strSourceCode, start, this.currentPos - start - 1);
          }
          this.escapedString = result.ToString();
        }else{
          if (this.currentPos <= this.currentToken.startPos + 2)
            this.escapedString = "";
          else
            this.escapedString = this.currentToken.source_string.Substring(this.currentToken.startPos + 1, this.currentPos - this.currentToken.startPos - 2);
        }
      }

      private void SkipSingleLineComment(){
        while(!IsEndLineOrEOF(GetChar(this.currentPos++), 0));
        if (this.IsAuthoring){
          this.currentToken.endPos = this.currentPos;
          this.currentToken.endLineNumber = this.currentLine;
          this.currentToken.endLinePos = this.startLinePos;
          this.gotEndOfLine = true;
        }
        this.currentLine++;
        this.startLinePos = this.currentPos;
      }

      // this method is public because it's used from the authoring code
      public int SkipMultiLineComment(){
        for(;;){
          char c = GetChar(this.currentPos);

          while ('*' ==  c){
            c = GetChar(++this.currentPos);
            if ('/' == c){
              this.currentPos++;
              return this.currentPos;
            }else if ((char)0 == c)
              break;
            else if (IsLineTerminator(c, 1)){
              c = GetChar(++this.currentPos);
              this.currentLine++;
              this.startLinePos = this.currentPos + 1;
            }
          }

          if ((char)0 == c && this.currentPos >= this.endPos)
            break;

          if (IsLineTerminator(c, 1)){
            this.currentLine++;
            this.startLinePos = this.currentPos + 1;
          }

          ++this.currentPos;
        }

        // if we are here we got EOF
        if (!this.IsAuthoring){
          this.currentToken.endPos = --this.currentPos;
          this.currentToken.endLinePos = this.startLinePos;
          this.currentToken.endLineNumber = this.currentLine;
          throw new ScannerException(JSError.NoCommentEnd);
        }
        return this.currentPos;
      }

      private void SkipBlanks(){
        char c = GetChar(this.currentPos);
        while(JSScanner.IsBlankSpace(c))
          c = GetChar(++this.currentPos);
      }

      private static bool IsBlankSpace(char c){
        switch (c){
        case (char)0x09:
        case (char)0x0B:
        case (char)0x0C:
        case (char)0x20:
        case (char)0xA0:
          return true;
        default:
          if (c >= 128)
            return Char.GetUnicodeCategory(c) == UnicodeCategory.SpaceSeparator;
          else
            return false;
        }
      }

      private bool IsLineTerminator(char c, int increment){
        switch (c){
          case (char)0x0D:
            // treat 0x0D0x0A as a single character
            if (0x0A == GetChar(this.currentPos + increment))
              this.currentPos++;
            return true;
          case (char)0x0A:
            return true;
          case (char)0x2028:
            return true;
          case (char)0x2029:
            return true;
          default:
            return false;
        }
      }

      private bool IsEndLineOrEOF(char c, int increment){
        return IsLineTerminator(c, increment) || (char)0 == c && this.currentPos >= this.endPos;
      }

      private int GetHexValue(char hex){
        int hexValue;
        if ('0' <= hex && hex <= '9')
          hexValue = hex - '0';
        else if ('a' <= hex && hex <= 'f')
          hexValue = hex - 'a' + 10;
        else
          hexValue = hex - 'A' + 10;
        return hexValue;
      }

      internal bool IsIdentifierPartChar(char c){
        if (this.IsIdentifierStartChar(ref c))
          return true;
        if ('0' <= c && c <= '9')
          return true;
        if (c < 128)
          return false;
        UnicodeCategory ccat = Char.GetUnicodeCategory(c);
        switch (ccat){
          case UnicodeCategory.NonSpacingMark:
          case UnicodeCategory.SpacingCombiningMark:
          case UnicodeCategory.DecimalDigitNumber:
          case UnicodeCategory.ConnectorPunctuation:
            return true;
          default:
            return false;
        }
      }

      internal bool IsIdentifierStartChar(ref char c){
        bool isEscapeChar = false;
        if ('\\' == c){
          if ('u' == GetChar(this.currentPos + 1)){
            char h1 = GetChar(this.currentPos + 2);
            if (IsHexDigit(h1)){
              char h2 = GetChar(this.currentPos + 3);
              if (IsHexDigit(h2)){
                char h3 = GetChar(this.currentPos + 4);
                if (IsHexDigit(h3)){
                  char h4 = GetChar(this.currentPos + 5);
                  if (IsHexDigit(h4)){
                    isEscapeChar = true;
                    c = (char)(GetHexValue(h1) << 12 | GetHexValue(h2) << 8 | GetHexValue(h3) << 4 | GetHexValue(h4));
                  }
                }
              }
            }
          }
        }
        if ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || '_' == c || '$' == c)
          goto isIdentifierChar;
        if (c < 128)
          return false;
        UnicodeCategory ccat = Char.GetUnicodeCategory(c);
        switch (ccat){
          case UnicodeCategory.UppercaseLetter:
          case UnicodeCategory.LowercaseLetter:
          case UnicodeCategory.TitlecaseLetter:
          case UnicodeCategory.ModifierLetter:
          case UnicodeCategory.OtherLetter:
          case UnicodeCategory.LetterNumber:
            goto isIdentifierChar;
          default:
            return false;
        }
      isIdentifierChar:
        if (isEscapeChar){
          int startPos = (this.idLastPosOnBuilder > 0) ? this.idLastPosOnBuilder : this.currentToken.startPos;
          if (this.currentPos - startPos > 0)
            this.identifier.Append(this.strSourceCode.Substring(startPos, this.currentPos - startPos));
          this.identifier.Append(c);
          this.currentPos += 5;
          this.idLastPosOnBuilder = this.currentPos + 1;
        }
        return true;
      }

      internal static bool IsDigit(char c){
        return '0' <= c && c <= '9';
      }

      internal static bool IsHexDigit(char c){
        return JSScanner.IsDigit(c) || 'A' <= c && c <= 'F' || 'a' <= c && c <= 'f';
      }


      internal static bool IsAsciiLetter(char c){
        return 'A' <= c && c <= 'Z' || 'a' <= c && c <= 'z';
      }

      internal static bool IsUnicodeLetter(char c){
        return c >= 128 && Char.IsLetter(c);
      }

      private void SetPreProcessorOn(){
        this.preProcessorOn = true;
        this.ppTable = new SimpleHashtable(16);

        // define constants

        this.ppTable["_debug"] = this.globals.engine.GenerateDebugInfo;
        this.ppTable["_fast"] = ((IActivationObject)this.globals.ScopeStack.Peek()).GetGlobalScope().fast;
        this.ppTable["_jscript"] = true;
        this.ppTable["_jscript_build"]= GlobalObject.ScriptEngineBuildVersion();
        this.ppTable["_jscript_version"] = Convert.ToNumber(GlobalObject.ScriptEngineMajorVersion() + "." + GlobalObject.ScriptEngineMinorVersion());
        this.ppTable["_microsoft"] = true;

        // define command-line symbols
        Hashtable userDefines = (Hashtable)this.globals.engine.GetOption("defines");
        if (userDefines != null)
          foreach (DictionaryEntry def in userDefines)
            this.ppTable[def.Key] = def.Value;
      }

      private bool PPTestCond(){
        this.SkipBlanks();

        if ('(' != GetChar(this.currentPos)){
          this.currentToken.startPos = this.currentPos - 1;
          this.currentToken.lineNumber = this.currentLine;
          this.currentToken.startLinePos = this.startLinePos;;
          HandleError(JSError.NoLeftParen);
        }else
          this.currentPos++;
        Object value = PPScanExpr();
        if (')' != GetChar(this.currentPos++)){
          this.currentToken.startPos = this.currentPos - 1;
          this.currentToken.lineNumber = this.currentLine;
          this.currentToken.startLinePos = this.startLinePos;;
          HandleError(JSError.NoRightParen);
          this.currentPos--;
        }

        return Convert.ToBoolean(value);
      }

      private void PPSkipToNextCondition(bool checkCondition){
        int ifCount = 0;
        for (;;){
          char c = GetChar(this.currentPos++);
          switch (c){
          case (char)0:
            if (this.currentPos >= this.endPos){
              this.currentPos--;
              this.currentToken.endPos = this.currentPos;
              this.currentToken.endLineNumber = this.currentLine;
              this.currentToken.endLinePos = this.startLinePos;
              HandleError(JSError.NoCcEnd);
              throw new ScannerException(JSError.ErrEOF);
            }
            break;

          case '\r':
            if (GetChar(this.currentPos) == '\n'){
              this.currentPos++;
            }
            this.currentLine++;
            this.startLinePos = this.currentPos;
            break;
          case '\n':
            this.currentLine++;
            this.startLinePos = this.currentPos;
            break;
          case (char)0x2028:
            this.currentLine++;
            this.startLinePos = this.currentPos;
            break;
          case (char)0x2029:
            this.currentLine++;
            this.startLinePos = this.currentPos;
            break;

          case '@':
            this.currentToken.startPos = this.currentPos;
            this.currentToken.lineNumber = this.currentLine;
            this.currentToken.startLinePos = this.startLinePos;
            ScanIdentifier();
            switch (this.currentPos - this.currentToken.startPos){
              case 2:
                if ('i' == this.strSourceCode[this.currentToken.startPos] &&
                    'f' == this.strSourceCode[this.currentToken.startPos + 1]){
                  ifCount++;
                }
                break;
              case 3:
                if ('e' == this.strSourceCode[this.currentToken.startPos] &&
                    'n' == this.strSourceCode[this.currentToken.startPos + 1] &&
                    'd' == this.strSourceCode[this.currentToken.startPos + 2]){
                  if (ifCount == 0){
                    this.matchIf--;
                    return;
                  }
                  ifCount--;
                }
                break;
              case 4:
                if ('e' == this.strSourceCode[this.currentToken.startPos] &&
                    'l' == this.strSourceCode[this.currentToken.startPos + 1] &&
                    's' == this.strSourceCode[this.currentToken.startPos + 2] &&
                    'e' == this.strSourceCode[this.currentToken.startPos + 3]){
                  if (0 == ifCount && checkCondition)
                    return;
                }else if ('e' == this.strSourceCode[this.currentToken.startPos] &&
                         'l' == this.strSourceCode[this.currentToken.startPos + 1] &&
                         'i' == this.strSourceCode[this.currentToken.startPos + 2] &&
                         'f' == this.strSourceCode[this.currentToken.startPos + 3]){
                  if (0 == ifCount && checkCondition && PPTestCond())
                    return;
                }
                break;
            }
            break;
          }
        }
      }

      private void PPScanSet(){
        this.SkipBlanks();
        if ('@' != GetChar(this.currentPos++)){
          HandleError(JSError.NoAt);
          this.currentPos--;
        }
        int startIdentifier = this.currentPos;
        ScanIdentifier();
        int length = this.currentPos - startIdentifier;

        string id = null;
        if (length == 0){
          this.currentToken.startPos = this.currentPos - 1;
          this.currentToken.lineNumber = this.currentLine;
          this.currentToken.startLinePos = this.startLinePos;
          HandleError(JSError.NoIdentifier);
          id = "#_Missing CC Identifier_#";
        }else
          id = this.strSourceCode.Substring(startIdentifier, length);

        this.SkipBlanks();

        char c = GetChar(this.currentPos++);
        if ('(' == c){
          if (id.Equals("position")){
            PPRemapPositionInfo();
          }else if (id.Equals("option")){
            PPLanguageOption();
          }else if (id.Equals("debug")){
            PPDebugDirective();
          }else{
            this.currentToken.startPos = this.currentPos - 1;
            this.currentToken.lineNumber = this.currentLine;
            this.currentToken.startLinePos = this.startLinePos;
            HandleError(JSError.NoEqual);
            this.currentPos--;
          }
        }else{
          if ('=' != c){
            this.currentToken.startPos = this.currentPos - 1;
            this.currentToken.lineNumber = this.currentLine;
            this.currentToken.startLinePos = this.startLinePos;
            HandleError(JSError.NoEqual);
            this.currentPos--;
          }

          Object value = PPScanConstant();
          this.ppTable[id] = value;
        }
      }

      private Object PPScanExpr(){
        OpListItem opsStack = new OpListItem(JSToken.None, OpPrec.precNone, null); // dummy element
        ConstantListItem termStack = new ConstantListItem(PPScanConstant(), null);
        Object value;
        for (;;){
          GetNextToken();
          if (JSScanner.IsPPOperator(this.currentToken.token)){

            OpPrec prec = JSScanner.GetPPOperatorPrecedence(this.currentToken.token);
            while (prec < opsStack._prec){
              //Console.Out.WriteLine("lower prec or same and left assoc");
              value = PPGetValue(opsStack._operator, termStack.prev.term, termStack.term);

              // pop the operator stack
              opsStack = opsStack._prev;
              // pop the term stack twice
              termStack = termStack.prev.prev;
              // push node onto the stack
              termStack = new ConstantListItem(value, termStack);
            }
            // the current operator has higher precedence that every scanned operators on the stack, or
            // it has the same precedence as the one at the top of the stack and it is right associative
            // push the operator onto the operators stack
            opsStack = new OpListItem(this.currentToken.token, prec, opsStack);
            // push new term
            termStack = new ConstantListItem(PPScanConstant(), termStack);
          }else{
            //Console.Out.WriteLine("unwinding stack");
            // there are still operators to be processed
            while (opsStack._operator != JSToken.None){
              // make the ast operator node
              value = PPGetValue(opsStack._operator, termStack.prev.term, termStack.term);

              // pop the operator stack
              opsStack = opsStack._prev;
              // pop the term stack twice
              termStack = termStack.prev.prev;

              // push node onto the stack
              termStack = new ConstantListItem(value, termStack);
            }

            // reset position into stream
            this.currentPos = this.currentToken.startPos;

            break; // done
          }
        }

        Debug.Assert(termStack.prev == null);
        return termStack.term;
      }

      private void PPRemapPositionInfo(){
        GetNextToken();
        String filename = null; // this.currentToken.document.documentName;
        int line = 0;
        int col = -1;
        bool endDirective = false;
        while(JSToken.RightParen != this.currentToken.token){
          if (JSToken.Identifier == this.currentToken.token){
            if (this.currentToken.Equals("file")){
              if (null == this.currentDocument){
                if (null == filename){
                  GetNextToken();
                  if (JSToken.Assign != this.currentToken.token){
                    HandleError(JSError.InvalidPositionDirective);
                    goto ignoreDirective;
                  }
                  GetNextToken();
                  if (JSToken.StringLiteral != this.currentToken.token){
                    HandleError(JSError.InvalidPositionDirective);
                    goto ignoreDirective;
                  }
                  filename = GetStringLiteral();
                  if (filename == this.currentToken.document.documentName){
                    filename = null;
                    HandleError(JSError.InvalidPositionDirective);
                    goto ignoreDirective;
                  }
                }else{
                  HandleError(JSError.InvalidPositionDirective);
                  goto ignoreDirective;
                }
              }else{
                HandleError(JSError.CannotNestPositionDirective);
                goto ignoreDirective;
              }
            }else if (this.currentToken.Equals("line")){
              if (null == this.currentDocument){
                if (line == 0){
                  GetNextToken();
                  if (JSToken.Assign != this.currentToken.token){
                    HandleError(JSError.InvalidPositionDirective);
                    goto ignoreDirective;
                  }
                  GetNextToken();
                  if (JSToken.IntegerLiteral != this.currentToken.token){
                    HandleError(JSError.InvalidPositionDirective);
                    goto ignoreDirective;
                  }
                  String number = this.currentToken.GetCode();
                  double i = Convert.ToNumber(number, true, true, Missing.Value);
                  if ((double)(int)i == i && i > 0)
                    line = (int)i;
                  else{
                    line = 1;
                    HandleError(JSError.InvalidPositionDirective);
                    goto ignoreDirective;
                  }
                }else{
                  HandleError(JSError.InvalidPositionDirective);
                  goto ignoreDirective;
                }
              }else{
                HandleError(JSError.CannotNestPositionDirective);
                goto ignoreDirective;
              }
            }else if (this.currentToken.Equals("column")){
              if (null == this.currentDocument){
                if (col == -1){
                  GetNextToken();
                  if (JSToken.Assign != this.currentToken.token){
                    HandleError(JSError.InvalidPositionDirective);
                    goto ignoreDirective;
                  }
                  GetNextToken();
                  if (JSToken.IntegerLiteral != this.currentToken.token){
                    HandleError(JSError.InvalidPositionDirective);
                    goto ignoreDirective;
                  }
                  String number = this.currentToken.GetCode();
                  double i = Convert.ToNumber(number, true, true, Missing.Value);
                  if ((double)(int)i == i && i >= 0)
                    col = (int)i;
                  else{
                    col = 0;
                    HandleError(JSError.InvalidPositionDirective);
                    goto ignoreDirective;
                  }
                }else{
                  HandleError(JSError.InvalidPositionDirective);
                  goto ignoreDirective;
                }
              }else{
                HandleError(JSError.CannotNestPositionDirective);
                goto ignoreDirective;
              }
            }else if (this.currentToken.Equals("end")){
              // end of the position mapping directive. Put back the old document
              if (null != this.currentDocument){
                // make sure this is the end of it and if not just go to the end of this directive and ignore it
                GetNextToken();
                if (JSToken.RightParen != this.currentToken.token){
                  HandleError(JSError.InvalidPositionDirective);
                  goto ignoreDirective;
                }else{
                  // restore the main document
                  this.currentToken.document = this.currentDocument;
                  this.currentDocument = null;
                  endDirective = true;
                  break;
                }
              }else{
                // we got an end with no position ever started. Report an error and ignore
                HandleError(JSError.WrongDirective);
                goto ignoreDirective;
              }
            }else{
              HandleError(JSError.InvalidPositionDirective);
              goto ignoreDirective;
            }

            GetNextToken();
            if (JSToken.RightParen == this.currentToken.token)
              break;
            else if (JSToken.Semicolon == this.currentToken.token)
              GetNextToken();
            continue;
          }

          HandleError(JSError.InvalidPositionDirective);
      ignoreDirective:
          // ignore the directive
          while (JSToken.RightParen != this.currentToken.token && JSToken.EndOfFile != this.currentToken.token)
            GetNextToken();
          break;
        }
        this.SkipBlanks();
        if (';' == GetChar(this.currentPos)){
          ++this.currentPos;
          this.SkipBlanks();
        }
            
        // the next char must be an end of line or we skip to it
        if (this.currentPos < this.endPos && !IsLineTerminator(GetChar(this.currentPos++), 0)){
          HandleError(JSError.MustBeEOL);
          while (this.currentPos < this.endPos && !IsLineTerminator(GetChar(this.currentPos++), 0));
        }
        this.currentLine++;
        this.startLinePos = this.currentPos;

        // set up or restore the document context
        if (!endDirective){ 
          if (null == filename && line == 0 && col == -1)
            HandleError(JSError.InvalidPositionDirective);
          else{
            if (filename == null) filename = this.currentToken.document.documentName;
            if (line == 0) line = 1;
            if (col == -1) col = 0;
            this.currentDocument = this.currentToken.document;
            this.currentToken.document = new DocumentContext(filename, line, col, this.currentLine, this.currentDocument.sourceItem);
          }
        }
      }

      private void PPDebugDirective(){
        GetNextToken();
        bool debugOn = false;
        if (JSToken.Identifier == this.currentToken.token){
          if (this.currentToken.Equals("off"))
            debugOn = false;
          else if (this.currentToken.Equals("on"))
            debugOn = true;
          else{
            HandleError(JSError.InvalidDebugDirective);
            goto ignoreDirective;
          }
          GetNextToken();
          if (JSToken.RightParen != this.currentToken.token){
            HandleError(JSError.InvalidDebugDirective);
            goto ignoreDirective;
          }
        }else{
          HandleError(JSError.InvalidDebugDirective);
          goto ignoreDirective;
        }
        this.currentToken.document.debugOn = debugOn && this.globals.engine.GenerateDebugInfo;
        this.ppTable["_debug"] = debugOn;

      ignoreDirective:
          // ignore the directive
        while (JSToken.RightParen != this.currentToken.token)
          GetNextToken();

        this.SkipBlanks();
        if (';' == GetChar(this.currentPos)){
          ++this.currentPos;
          this.SkipBlanks();
        }
            
        // the next char must be an end of line or we skip to it
        if (!IsLineTerminator(GetChar(this.currentPos++), 0)){
          HandleError(JSError.MustBeEOL);
          while (!IsLineTerminator(GetChar(this.currentPos++), 0));
        }
        this.currentLine++;
        this.startLinePos = this.currentPos;
      }

      private void PPLanguageOption(){
        GetNextToken();
        /*
        if (JSToken.Identifier == this.currentToken.token && this.globals != null){
          if (this.currentToken.Equals("fast")){
            ((GlobalScope)this.globals.ScopeStack.Peek()).SetFast();
            this.ppTable["_fast"] = true;
          }else if (this.currentToken.Equals("versionsafe")){
            this.globals.engine.versionSafe = true;
          }else{
            HandleError(JSError.InvalidLanguageOption);
          }
        }else
        */  HandleError(JSError.InvalidLanguageOption);
        GetNextToken();
        Context errorToken = null;
        while (JSToken.RightParen != this.currentToken.token){
          if (null == errorToken)
            errorToken = this.currentToken.Clone();
          else
            errorToken.UpdateWith(this.currentToken);
          GetNextToken();
        }
        if (null != errorToken)
          HandleError(JSError.NoRightParen);
      }

      private Object PPScanConstant(){
        Object value;
        GetNextToken();
        switch (this.currentToken.token){
          case JSToken.Plus:
            value = Convert.ToNumber(PPScanConstant());
            break;
          case JSToken.Minus:
            value = -Convert.ToNumber(PPScanConstant());
            break;
          case JSToken.BitwiseNot:
            value = ~Convert.ToInt32(PPScanConstant());
            break;
          case JSToken.LogicalNot:
            value = !Convert.ToBoolean(PPScanConstant());
            break;
          case JSToken.IntegerLiteral:
            value = Convert.ToNumber(this.currentToken.GetCode(), true, true, Missing.Value);
            break;
          case JSToken.NumericLiteral:
            value = Convert.ToNumber(this.currentToken.GetCode(), false, false, Missing.Value);
            break;
          case JSToken.PreProcessorConstant:
            value = this.preProcessorValue;
            break;
          case JSToken.True:
            value = true;
            break;
          case JSToken.False:
            value = false;
            break;
          case JSToken.LeftParen:
            value = PPScanExpr();
            GetNextToken();
            if (JSToken.RightParen != this.currentToken.token){
              this.currentToken.endPos = this.currentToken.startPos + 1;
              this.currentToken.endLineNumber = this.currentLine;
              this.currentToken.endLinePos = this.startLinePos;
              HandleError(JSError.NoRightParen);
              this.currentPos = this.currentToken.startPos;
            }
            break;
          default:
            HandleError(JSError.NotConst);
            this.currentPos = this.currentToken.startPos;
            value = true;
            break;
        }
        return value;
      }

      Object PPGetValue(JSToken op, Object op1, Object op2){
        switch (op){
          case JSToken.Plus:
            return Convert.ToNumber(op1) + Convert.ToNumber(op2);
          case JSToken.Minus:
            return Convert.ToNumber(op1) - Convert.ToNumber(op2);
          case JSToken.LogicalOr:
            return Convert.ToBoolean(op1) || Convert.ToBoolean(op2);
          case JSToken.LogicalAnd:
            return Convert.ToBoolean(op1) && Convert.ToBoolean(op2);
          case JSToken.BitwiseOr:
            return Convert.ToInt32(op1) | Convert.ToInt32(op2);
          case JSToken.BitwiseXor:
            return Convert.ToInt32(op1) ^ Convert.ToInt32(op2);
          case JSToken.BitwiseAnd:
            return Convert.ToInt32(op1) & Convert.ToInt32(op2);
          case JSToken.Equal:
            return Convert.ToNumber(op1) == Convert.ToNumber(op2);
          case JSToken.NotEqual:
            return Convert.ToNumber(op1) != Convert.ToNumber(op2);
          case JSToken.StrictEqual:
            return op1 == op2;
          case JSToken.StrictNotEqual:
            return op1 != op2;
          case JSToken.GreaterThan:
            return Convert.ToNumber(op1) > Convert.ToNumber(op2);
          case JSToken.LessThan:
            return Convert.ToNumber(op1) < Convert.ToNumber(op2);
          case JSToken.LessThanEqual:
            return Convert.ToNumber(op1) <= Convert.ToNumber(op2);
          case JSToken.GreaterThanEqual:
            return Convert.ToNumber(op1) >= Convert.ToNumber(op2);
          case JSToken.LeftShift:
            return Convert.ToInt32(op1) << (Convert.ToInt32(op2) & 0x1F);
          case JSToken.RightShift:
            return Convert.ToInt32(op1) >> (Convert.ToInt32(op2) & 0x1F);
          case JSToken.UnsignedRightShift:
            return ((uint)Convert.ToInt32(op1)) >> (Convert.ToInt32(op2) & 0x1F);
          case JSToken.Multiply:
            return Convert.ToNumber(op1) * Convert.ToNumber(op2);
          case JSToken.Divide:
            return Convert.ToNumber(op1) / Convert.ToNumber(op2);
          case JSToken.Modulo:
            return Convert.ToInt32(op1) % Convert.ToInt32(op2);
          default:
            Debug.Assert(false);
            break;
        }
        return null;
      }

      internal Object GetPreProcessorValue(){
        return this.preProcessorValue;
      }

      private void HandleError(JSError error){
        if (!this.IsAuthoring)
          this.currentToken.HandleError(error);
      }

      public static bool IsOperator(JSToken token){
        return JSToken.FirstOp <= token && token <= JSToken.LastOp;
      }

      internal static bool IsAssignmentOperator(JSToken token){
        return JSToken.Assign <= token && token <= JSToken.LastAssign;
      }

      internal static bool CanStartStatement(JSToken token){
        return JSToken.If <= token && token <= JSToken.Function;
      }


      internal static bool CanParseAsExpression(JSToken token){
        return JSToken.FirstBinaryOp <= token && token <= JSToken.LastOp || JSToken.LeftParen <= token && token <= JSToken.AccessField;
      }

      internal static bool IsRightAssociativeOperator(JSToken token){
        return JSToken.Assign <= token && token <= JSToken.ConditionalIf;
      }

      public static bool IsKeyword(JSToken token){
        switch(token){
          case JSToken.If:
          case JSToken.For:
          case JSToken.Do:
          case JSToken.While:
          case JSToken.Continue:
          case JSToken.Break:
          case JSToken.Return:
          case JSToken.Import:
          case JSToken.With:
          case JSToken.Switch:
          case JSToken.Throw:
          case JSToken.Try:
          case JSToken.Package:
          case JSToken.Abstract:
          case JSToken.Public:
          case JSToken.Static:
          case JSToken.Private:
          case JSToken.Protected:
          case JSToken.Var:
          case JSToken.Class:
          case JSToken.Function:
          case JSToken.Null:
          case JSToken.True:
          case JSToken.False:
          case JSToken.This:
          case JSToken.Final:
          case JSToken.Const:
          case JSToken.Delete:
          case JSToken.Void:
          case JSToken.Typeof:
          case JSToken.Instanceof:
          case JSToken.In:
          case JSToken.Case:
          case JSToken.Catch:
          case JSToken.Debugger:
          case JSToken.Default:
          case JSToken.Else:
          case JSToken.Export:
          case JSToken.Extends:
          case JSToken.Finally:
          case JSToken.Get:
          case JSToken.Implements:
          case JSToken.Interface:
          case JSToken.New:
          case JSToken.Set:
          case JSToken.Super:
          case JSToken.Boolean:
          case JSToken.Byte:
          case JSToken.Char:
          case JSToken.Double:
          case JSToken.Enum:
          case JSToken.Float:
          case JSToken.Goto:
          case JSToken.Int:
          case JSToken.Long:
          case JSToken.Native:
          case JSToken.Short:
          case JSToken.Synchronized:
          case JSToken.Transient:
          case JSToken.Throws:
          case JSToken.Volatile:
            return true;
        }
        return false;
      }

      // This function return whether an operator is processable in ParseExpression.
      // Comma is out of this list and so are the unary ops
      internal static bool IsProcessableOperator(JSToken token){
        return JSToken.FirstBinaryOp <= token && token <= JSToken.ConditionalIf;
      }

      // This function return whether an operator is processable in pre processing.
      internal static bool IsPPOperator(JSToken token){
        return JSToken.FirstBinaryOp <= token && token <= JSToken.LastPPOperator;
      }

      private static readonly OpPrec[] s_OperatorsPrec = InitOperatorsPrec();
      private static readonly OpPrec[] s_PPOperatorsPrec = InitPPOperatorsPrec();

      internal static OpPrec GetOperatorPrecedence(JSToken token){
        return JSScanner.s_OperatorsPrec[token - JSToken.FirstBinaryOp];
      }

      internal static OpPrec GetPPOperatorPrecedence(JSToken token) {
        return JSScanner.s_PPOperatorsPrec[token - JSToken.FirstBinaryOp];
      }

      private static OpPrec[] InitOperatorsPrec(){
        OpPrec[] operatorsPrec = new OpPrec[JSToken.ConditionalIf - JSToken.FirstBinaryOp + 1];

        operatorsPrec[JSToken.Plus - JSToken.FirstBinaryOp] = OpPrec.precAdditive;
        operatorsPrec[JSToken.Minus - JSToken.FirstBinaryOp] = OpPrec.precAdditive;

        operatorsPrec[JSToken.LogicalOr - JSToken.FirstBinaryOp] = OpPrec.precLogicalOr;
        operatorsPrec[JSToken.LogicalAnd - JSToken.FirstBinaryOp] = OpPrec.precLogicalAnd;
        operatorsPrec[JSToken.BitwiseOr - JSToken.FirstBinaryOp] = OpPrec.precBitwiseOr;
        operatorsPrec[JSToken.BitwiseXor - JSToken.FirstBinaryOp] = OpPrec.precBitwiseXor;
        operatorsPrec[JSToken.BitwiseAnd - JSToken.FirstBinaryOp] = OpPrec.precBitwiseAnd;

        operatorsPrec[JSToken.Equal - JSToken.FirstBinaryOp] = OpPrec.precEquality;
        operatorsPrec[JSToken.NotEqual - JSToken.FirstBinaryOp] = OpPrec.precEquality;
        operatorsPrec[JSToken.StrictEqual - JSToken.FirstBinaryOp] = OpPrec.precEquality;
        operatorsPrec[JSToken.StrictNotEqual - JSToken.FirstBinaryOp] = OpPrec.precEquality;

        operatorsPrec[JSToken.Instanceof - JSToken.FirstBinaryOp] = OpPrec.precRelational;
        operatorsPrec[JSToken.In - JSToken.FirstBinaryOp] = OpPrec.precRelational;
        operatorsPrec[JSToken.GreaterThan - JSToken.FirstBinaryOp] = OpPrec.precRelational;
        operatorsPrec[JSToken.LessThan - JSToken.FirstBinaryOp] = OpPrec.precRelational;
        operatorsPrec[JSToken.LessThanEqual - JSToken.FirstBinaryOp] = OpPrec.precRelational;
        operatorsPrec[JSToken.GreaterThanEqual - JSToken.FirstBinaryOp] = OpPrec.precRelational;

        operatorsPrec[JSToken.LeftShift - JSToken.FirstBinaryOp] = OpPrec.precShift;
        operatorsPrec[JSToken.RightShift - JSToken.FirstBinaryOp] = OpPrec.precShift;
        operatorsPrec[JSToken.UnsignedRightShift - JSToken.FirstBinaryOp] = OpPrec.precShift;

        operatorsPrec[JSToken.Multiply - JSToken.FirstBinaryOp] = OpPrec.precMultiplicative;
        operatorsPrec[JSToken.Divide - JSToken.FirstBinaryOp] = OpPrec.precMultiplicative;
        operatorsPrec[JSToken.Modulo - JSToken.FirstBinaryOp] = OpPrec.precMultiplicative;

        operatorsPrec[JSToken.Assign - JSToken.FirstBinaryOp] = OpPrec.precAssignment;
        operatorsPrec[JSToken.PlusAssign - JSToken.FirstBinaryOp] = OpPrec.precAssignment;
        operatorsPrec[JSToken.MinusAssign - JSToken.FirstBinaryOp] = OpPrec.precAssignment;
        operatorsPrec[JSToken.MultiplyAssign - JSToken.FirstBinaryOp] = OpPrec.precAssignment;
        operatorsPrec[JSToken.DivideAssign - JSToken.FirstBinaryOp] = OpPrec.precAssignment;
        operatorsPrec[JSToken.BitwiseAndAssign - JSToken.FirstBinaryOp] = OpPrec.precAssignment;
        operatorsPrec[JSToken.BitwiseOrAssign - JSToken.FirstBinaryOp] = OpPrec.precAssignment;
        operatorsPrec[JSToken.BitwiseXorAssign - JSToken.FirstBinaryOp] = OpPrec.precAssignment;
        operatorsPrec[JSToken.ModuloAssign - JSToken.FirstBinaryOp] = OpPrec.precAssignment;
        operatorsPrec[JSToken.LeftShiftAssign - JSToken.FirstBinaryOp] = OpPrec.precAssignment;
        operatorsPrec[JSToken.RightShiftAssign - JSToken.FirstBinaryOp] = OpPrec.precAssignment;
        operatorsPrec[JSToken.UnsignedRightShiftAssign - JSToken.FirstBinaryOp] = OpPrec.precAssignment;

        operatorsPrec[JSToken.ConditionalIf - JSToken.FirstBinaryOp] = OpPrec.precConditional;

        return operatorsPrec;
      }

      private static OpPrec[] InitPPOperatorsPrec() {
        OpPrec[] operatorsPrec = new OpPrec[JSToken.LastPPOperator - JSToken.FirstBinaryOp + 1];

        operatorsPrec[JSToken.Plus - JSToken.FirstBinaryOp] = OpPrec.precAdditive;
        operatorsPrec[JSToken.Minus - JSToken.FirstBinaryOp] = OpPrec.precAdditive;

        operatorsPrec[JSToken.LogicalOr - JSToken.FirstBinaryOp] = OpPrec.precLogicalOr;
        operatorsPrec[JSToken.LogicalAnd - JSToken.FirstBinaryOp] = OpPrec.precLogicalAnd;
        operatorsPrec[JSToken.BitwiseOr - JSToken.FirstBinaryOp] = OpPrec.precBitwiseOr;
        operatorsPrec[JSToken.BitwiseXor - JSToken.FirstBinaryOp] = OpPrec.precBitwiseXor;
        operatorsPrec[JSToken.BitwiseAnd - JSToken.FirstBinaryOp] = OpPrec.precBitwiseAnd;

        operatorsPrec[JSToken.Equal - JSToken.FirstBinaryOp] = OpPrec.precEquality;
        operatorsPrec[JSToken.NotEqual - JSToken.FirstBinaryOp] = OpPrec.precEquality;
        operatorsPrec[JSToken.StrictEqual - JSToken.FirstBinaryOp] = OpPrec.precEquality;
        operatorsPrec[JSToken.StrictNotEqual - JSToken.FirstBinaryOp] = OpPrec.precEquality;

        operatorsPrec[JSToken.GreaterThan - JSToken.FirstBinaryOp] = OpPrec.precRelational;
        operatorsPrec[JSToken.LessThan - JSToken.FirstBinaryOp] = OpPrec.precRelational;
        operatorsPrec[JSToken.LessThanEqual - JSToken.FirstBinaryOp] = OpPrec.precRelational;
        operatorsPrec[JSToken.GreaterThanEqual - JSToken.FirstBinaryOp] = OpPrec.precRelational;

        operatorsPrec[JSToken.LeftShift - JSToken.FirstBinaryOp] = OpPrec.precShift;
        operatorsPrec[JSToken.RightShift - JSToken.FirstBinaryOp] = OpPrec.precShift;
        operatorsPrec[JSToken.UnsignedRightShift - JSToken.FirstBinaryOp] = OpPrec.precShift;

        operatorsPrec[JSToken.Multiply - JSToken.FirstBinaryOp] = OpPrec.precMultiplicative;
        operatorsPrec[JSToken.Divide - JSToken.FirstBinaryOp] = OpPrec.precMultiplicative;
        operatorsPrec[JSToken.Modulo - JSToken.FirstBinaryOp] = OpPrec.precMultiplicative;

        return operatorsPrec;
      }
    }
}

