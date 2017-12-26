//------------------------------------------------------------------------------
// <copyright file="XPathScanner.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

using System.Diagnostics;
//using System.Globalization;
//using System.Text;

namespace System.Xml.Xsl.XPath {
    using Res           = System.Xml.Utils.Res;

    internal enum LexKind {
        Comma                 = ',',
        Slash                 = '/',
        At                    = '@',
        Dot                   = '.',
        LParens               = '(',
        RParens               = ')',
        LBracket              = '[',
        RBracket              = ']',
        LBrace                = '{',
        RBrace                = '}',
        Star                  = '*',
        Plus                  = '+',
        Minus                 = '-',
        Eq                    = '=',
        Lt                    = '<',
        Gt                    = '>',
        Bang                  = '!',
        Dollar                = '$',
        Union                 = '|',

        Ne                    = 'N',   // !=
        Le                    = 'L',   // <=
        Ge                    = 'G',   // >=
        DotDot                = 'D',   // ..
        SlashSlash            = 'S',   // //
        Name                  = 'n',   // Name
        String                = 's',   // String literal
        Number                = 'd',   // Numeric literal
        Axis                  = 'a',   // Axis
        Unknown               = 'U',   // Unknown char
        Eof                   = 'E',
    };

    sealed internal class XPathScanner {
        private string  xpathExpr;
        private int     curIndex;
        private char    curChar;
        private LexKind kind;
        private string  name;
        private string  prefix;
        private string  stringValue;
        private double  numberValue = double.NaN;
        private bool    canBeFunction;
        private int     lexStart;
        private int     prevLexEnd;

        private XmlCharType xmlCharType = XmlCharType.Instance;

        public XPathScanner(string xpathExpr) : this(xpathExpr, 0) {}

        public XPathScanner(string xpathExpr, int startFrom) {
            Debug.Assert(xpathExpr != null);
            this.xpathExpr = xpathExpr;
            SetSourceIndex(startFrom);
            NextLex();
        }

        public string   Source      { get { return xpathExpr;   } }
        public LexKind  Kind        { get { return kind;        } }
        public int      LexStart    { get { return lexStart;    } }
        public int      LexSize     { get { return curIndex - lexStart; } }
        public int      PrevLexEnd  { get { return prevLexEnd;  } }

        private void SetSourceIndex(int index) {
            curIndex = index - 1;
            NextChar();
        }

        private bool NextChar() {
            Debug.Assert(-1 <= curIndex && curIndex < xpathExpr.Length);
            curIndex++;
            if (curIndex < xpathExpr.Length) {
                curChar = xpathExpr[curIndex];
                return true;
            } else {
                Debug.Assert(curIndex == xpathExpr.Length);
                curChar = '\0';
                return false;
            }
        }

        public string Name {
            get {
                Debug.Assert(kind == LexKind.Name || kind == LexKind.Axis);
                Debug.Assert(name != null);
                return name;
            }
        }

        public string Prefix {
            get {
                Debug.Assert(kind == LexKind.Name);
                Debug.Assert(prefix != null);
                return prefix;
            }
        }

        public bool IsKeyword(string keyword) {
            return (
                kind == LexKind.Name &&
                prefix.Length == 0   &&
                name.Equals(keyword)
            );
        }

        public string RawValue {
            get {
                if (kind == LexKind.Eof) {
                    return LexKindToString(kind);
                } else {
                    return xpathExpr.Substring(lexStart, curIndex - lexStart);
                }
            }
        }

        public string StringValue {
            get {
                Debug.Assert(kind == LexKind.String);
                Debug.Assert(stringValue != null);
                return stringValue;
            }
        }

        public double NumberValue {
            get {
                Debug.Assert(kind == LexKind.Number);
                Debug.Assert(numberValue != double.NaN);
                return numberValue;
            }
        }

        // To parse PathExpr we need a way to distinct name from function.
        // THis distinction can't be done without context: "or (1 != 0)" this this a function or 'or' in OrExp
        public bool CanBeFunction {
            get {
                Debug.Assert(kind == LexKind.Name);
                return canBeFunction;
            }
        }

        void SkipSpace() {
            while (xmlCharType.IsWhiteSpace(curChar) && NextChar()) {
            }
        }

        public bool NextLex() {
            prevLexEnd = curIndex;
            SkipSpace();
            lexStart = curIndex;
            switch (curChar) {
            case '\0':
                kind = LexKind.Eof;
                return false;
            case ',': case '@': case '(': case ')':
            case '|': case '*': case '[': case ']':
            case '+': case '-': case '=': case '#':
            case '$': case '{': case '}':
                kind = (LexKind)curChar;
                NextChar();
                break;
            case '<':
                kind = LexKind.Lt;
                NextChar();
                if (curChar == '=') {
                    kind = LexKind.Le;
                    NextChar();
                }
                break;
            case '>':
                kind = LexKind.Gt;
                NextChar();
                if (curChar == '=') {
                    kind = LexKind.Ge;
                    NextChar();
                }
                break;
            case '!':
                kind = LexKind.Bang;
                NextChar();
                if (curChar == '=') {
                    kind = LexKind.Ne;
                    NextChar();
                }
                break;
            case '.':
                kind = LexKind.Dot;
                NextChar();
                if (curChar == '.') {
                    kind = LexKind.DotDot;
                    NextChar();
                } else if (xmlCharType.IsDigit(curChar)) {
                    ScanFraction();
                }
                break;
            case '/':
                kind = LexKind.Slash;
                NextChar();
                if (curChar == '/') {
                    kind = LexKind.SlashSlash;
                    NextChar();
                }
                break;
            case '"':
            case '\'':
                ScanString();
                break;
            default:
                if (xmlCharType.IsDigit(curChar)) {
                    ScanNumber();
                } else if (xmlCharType.IsStartNCNameChar(curChar)) {
                    kind = LexKind.Name;
                    this.name   = ScanNCName();
                    this.prefix = string.Empty;
                    int saveSourceIndex = curIndex;
                    // "foo:bar" is one lexem not three because it doesn't allow spaces in between
                    // We should distinct it from "foo::" and need process "foo ::" as well
                    if (curChar == ':') {
                        NextChar();
                        // can be "foo:bar" or "foo::"
                        if (curChar == ':') {   // "foo::"
                            NextChar();
                            kind = LexKind.Axis;
                        } else {                // "foo:*", "foo:bar" or "foo: "
                            if (curChar == '*') {
                                NextChar();
                                this.prefix = this.name;
                                this.name = "*";
                            } else if (xmlCharType.IsStartNCNameChar(curChar)) {
                                this.prefix = this.name;
                                this.name = ScanNCName();
                            } else {
                                // this lex is something like "foo:?". Let's it be recognized as name "foo"
                                // and leave ":-" to be scaned late as unknown lex.
                                SetSourceIndex(saveSourceIndex);
                            }
                        }
                    } else {
                        SkipSpace();
                        if (curChar == ':') {
                            NextChar();
                            // it can be "foo ::" or just "foo :"
                            if (curChar == ':') {
                                NextChar();
                                kind = LexKind.Axis;
                            } else {
                                // this lex is something like "foo :?". Let's it be recognized as name "foo"
                                // and leave ":-" to be scaned late as unknown lex.
                                SetSourceIndex(saveSourceIndex);
                            }
                        }
                    }
                    // look ahead for '('. I don't want curIndex to be moved by SkipSpace() here to be able to detect presize lexem size.
                    saveSourceIndex = curIndex;
                    SkipSpace();
                    this.canBeFunction = (curChar == '(');
                    SetSourceIndex(saveSourceIndex);
                } else {
                    kind = LexKind.Unknown;
                    NextChar();
                }
                break;
            }
            return true;
        }

        private void ScanNumber() {
            Debug.Assert(xmlCharType.IsDigit(curChar));
            int start = curIndex;
            while (xmlCharType.IsDigit(curChar)) {
                NextChar();
            }
            if (curChar == '.') {
                NextChar();
                while (xmlCharType.IsDigit(curChar)) {
                    NextChar();
                }
            }
            if ((curChar & (~0x20)) == 'E') {
                NextChar();
                if (curChar == '+' || curChar == '-') {
                    NextChar();
                }
                while (xmlCharType.IsDigit(curChar)) {
                    NextChar();
                }
                throw CreateException(Res.XPath_ScientificNotation);
            }
            this.kind        = LexKind.Number;
            this.numberValue = XPathConvert.StringToDouble(xpathExpr.Substring(start, curIndex - start));
        }

        private void ScanFraction() {
            Debug.Assert(xmlCharType.IsDigit(curChar));
            int start = curIndex - 1;
            Debug.Assert(0 <= start && xpathExpr[start] == '.');
            while (xmlCharType.IsDigit(curChar)) {
                NextChar();
            }
            this.kind        = LexKind.Number;
            this.numberValue = XPathConvert.StringToDouble(xpathExpr.Substring(start, curIndex - start));
        }

        private void ScanString() {
            char endChar = curChar;
            int  start   = curIndex + 1;

            do {
                if (!NextChar()) {
                    throw CreateException(Res.XPath_UnclosedString);
                }
            } while (curChar != endChar);

            this.kind        = LexKind.String;
            this.stringValue = xpathExpr.Substring(start, curIndex - start);
            NextChar();
        }

        private string ScanNCName() {
            Debug.Assert(xmlCharType.IsStartNCNameChar(curChar));
            int start = curIndex;
            while (xmlCharType.IsNCNameChar(curChar)) {
                NextChar();
            }
            return xpathExpr.Substring(start, curIndex - start);
        }

        public void PassToken(LexKind t) {
            CheckToken(t);
            NextLex();
        }

        public void CheckToken(LexKind t) {
            if (kind != t) {
                if (t == LexKind.Eof) {
                    throw CreateException(Res.XPath_EofExpected, RawValue);
                } else {
                    throw CreateException(Res.XPath_TokenExpected, LexKindToString(t), RawValue);
                }
            }
        }

        public string LexKindToString(LexKind t) {
            const string OneCharLexemes = ",/@.()[]{}*+-=<>!$|";

            if (OneCharLexemes.IndexOf((char)t) >= 0) {
                return ((char)t).ToString();
            }

            switch (t) {
            case LexKind.Ne         : return "!=";
            case LexKind.Le         : return "<=";
            case LexKind.Ge         : return ">=";
            case LexKind.DotDot     : return "..";
            case LexKind.SlashSlash : return "//";
            case LexKind.Name       : return "<name>";
            case LexKind.String     : return "<string literal>";
            case LexKind.Number     : return "<number literal>";
            case LexKind.Axis       : return "<axis>";
            case LexKind.Unknown    : return "<unknown>";
            case LexKind.Eof        : return "<eof>";
            default:
                Debug.Fail("Must not get here");
                return string.Empty;
            }
        }

        public XPathCompileException CreateException(string resId, params string[] args) {
            return new XPathCompileException(xpathExpr, lexStart, curIndex, resId, args);
        }
    }
}
