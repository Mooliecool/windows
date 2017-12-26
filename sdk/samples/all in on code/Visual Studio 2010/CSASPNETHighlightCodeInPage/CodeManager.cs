/**************************** Module Header ********************************\
* Module Name:    CodeManager.cs
* Project:        CSASPNETHighlightCodeInPage
* Copyright (c) Microsoft Corporation
*
* Sometimes we input code like C# or HTML in our post and we need these code 
* to be highlighted for a better reading experience.This project illustrates how
* to highlight the code in a page. 

* In this file,we use a Hashtable variable to store the different languages of 
* code and their related regular expressions with matching options.Then add the
* style object to the matching string of code.

* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\***************************************************************************/

using System.Collections;
using System.Text.RegularExpressions;
using System.Web;

namespace CSASPNETHighlightCodeInPage
{
    /// <summary>
    /// The structure of style string and regular expressions. 
    /// </summary>
    public struct RegexStruct
    {
        public string styleObject;
        public Regex regex;
    }

    /// <summary>
    /// Matching string and style object.
    /// </summary>
    public class RegExp
    {
        /// <summary>
        /// Store the matching string collection.
        /// </summary>
        private ArrayList _regexStructList = new ArrayList();

        /// <summary>
        /// </summary>
        /// <param name="styleObject">Style object</param>
        /// <param name="reg">Regular expressions</param>
        /// <param name="regOption">Matching options</param>
        public void Add(string styleObject, string reg, RegexOptions regOption)
        {
            RegexStruct regexStruct = new RegexStruct();
            regexStruct.styleObject = styleObject;
            regexStruct.regex = new Regex(reg, regOption | RegexOptions.Compiled);
            _regexStructList.Add(regexStruct);
        }
        /// <summary>
        /// Return the matching string collection ，read only.
        /// </summary>
        public ArrayList regexStructList
        {
            get { return _regexStructList; }
        }
    }

    /// <summary>
    /// Highlight the code operation.
    /// </summary>
    public class CodeManager
    {
        /// <summary>
        /// Initialize the Hashtable variable which used to store the 
        /// the different languages of code and their related regular
        /// expressions with matching options.
        /// </summary>
        /// <returns></returns>
        public static Hashtable Init()
        {
            Hashtable hashTable = new Hashtable();
            RegExp regExp = new RegExp();

            // Add the information of VBScript language to a Hashtable variable.   
            #region VBScript language
            regExp.Add("str", "\"([^\"\\n]*?)\"", RegexOptions.None);
            regExp.Add("note", "'[^\r\n]*", RegexOptions.None);
            regExp.Add("kw", @"\b(elseif|if|then|else|select|case|end|for|while"
                + "|wend|do|loop|until|abs|sgn|hex|oct|sqr|int|fix|round"
                + "|log|split|cint|sin|cos|tan|len|mid|left|right|lcase|ucase"
                + "|trim|ltrim|rtrim|replace|instr|instrrev|space|string"
                + "|strreverse|cstr|clng|cbool|cdate|csng|cdbl|date|time|now"
                + "|dateadd|datediff|dateserial|datevalue|year|month|day|hour"
                + "|minute|second|timer|timeserial|timevalue|weekday|monthname"
                + "|array|asc|chr|filter|inputbox|join|msgbox|lbound|ubound"
                + "|redim|randomize|rnd|isempty|mod|execute|not|and|or|xor"
                + "|const|dim|erase"
                + @"|class(?!\s*=))\b", RegexOptions.IgnoreCase);
            hashTable.Add("vbs", regExp);
            #endregion

            // Add the information of JavaScript language to a Hashtable variable.   
            #region JavaScript language
            regExp = new RegExp();
            regExp.Add("str", "\"[^\"\\n]*\"|'[^'\\n]*'", RegexOptions.None);
            regExp.Add("note", @"\/\/[^\n\r]*|\/\*[\s\S]*?\*\/", RegexOptions.None);
            regExp.Add("kw", @"\b(break|delete|function|return|typeof|case|do|if"
                + "|switch|var|catch|else|in|this|void|continue|false|nstanceof"
                + "|throw|while|debugger|finally|new|true|with|default|for|null"
                + "|try|abstract|double|goto|native|static|boolean|enum|implements"
                + "|package|super|byte|export|import|private|synchronized|char"
                + "|extends|int|protected|throws|final|interface|public|transient"
                + "|const|float|long|short|volatile"
                + @"|class(?!\s*=))\b", RegexOptions.None);
            hashTable.Add("js", regExp);
            #endregion

            // Add the information of SqlServer language to a Hashtable variable.   
            #region SqlServer language
            regExp = new RegExp();
            regExp.Add("sqlstr", "'([^'\\n]*?)*'", RegexOptions.None);
            regExp.Add("note", @"--[^\n\r]*|\/\*[\s\S]*?\*\/", RegexOptions.None);
            regExp.Add("sqlconnect", @"\b(all|and|between|cross|exists|in|join|like"
                + "|not|null|outer|or)\b", RegexOptions.IgnoreCase);
            regExp.Add("sqlfunc", @"\b(avg|case|checksum|current_timestamp|day|left"
                + "|month|replace|year)\b", RegexOptions.IgnoreCase);
            regExp.Add("kw", @"\b(action|add|alter|after|as|asc|bigint|bit|binary|by"
                + "|cascade|char|character|check|column|columns|constraint|create"
                + "|current_date|current_time|database|date|datetime|dec|decimal"
                + "|default|delete|desc|distinct|double|drop|end|else|escape|file"
                + "|first|float|foreign|from|for|full|function|global|grant|group"
                + "|having|hour|ignore|index|inner|insert|int|integer|into|if|is"
                + "|key|kill|load|local|max|minute|modify|numeric|no|on|option|order"
                + "|partial|password|precision|primary|procedure|privileges"
                + "|read|real|references|restrict|returns|revoke|rows|second|select"
                + "|set|shutdown|smallint|table|temporary|text|then|time"
                + "|timestamp|tinyint|to|use|unique|update|values|varchar|varying"
                + @"|varbinary|with|when|where)\b", RegexOptions.IgnoreCase);
            hashTable.Add("sql", regExp);
            #endregion

            // Add the information of C# language to a Hashtable variable.   
            #region  C# language
            regExp = new RegExp();
            regExp.Add("str", "\"[^\"\\n]*\"", RegexOptions.None);
            regExp.Add("note", @"\/\/[^\n\r]*|\/\*[\s\S]*?\*\/", RegexOptions.None);
            regExp.Add("Var", @"(?<=\bclass\s+)([_a-z][_a-z0-9]*)(?=\s*[\{:])"
                + @"|(?<=\=\s*new\s+)([a-z_][a-z0-9_]*)(?=\s*\()"
                + @"|([a-z][a-z0-9_]*)(?=\s+[a-z_][a-z0-9_]*\s*=\s*new)",
                RegexOptions.IgnoreCase);
            regExp.Add("kw", @"\b(partial|abstract|event|get|set|value|new|struct|as"
                + "|null|switch|base|object|this|bool|false|operator|throw|break"
                + "|finally|out|byte|fixed|override|try|case|float|params|typeof"
                + "|catch|for|private|uint|char|foreach|protected|ulong|checked"
                + "|goto|public|unchecked|if|readonly|unsafe|const|implicit|ref"
                + "|ushort|continue|in|return|using|decimal|int|sbyte|virtual"
                + "|default|interface|sealed|volatile|delegate|internal|short|void"
                + "|do|is|sizeof|while|double|lock|stackalloc|else|long|static"
                + @"|enum|string|namespace|region|endregion|class(?!\s*=))\b",
                RegexOptions.None);
            regExp.Add("kwG", @"\b(EventArgs|Page|Label|TextBox|CheckBox|DropDownList"
                + @"|Control|Button|DayRenderEventArgs|Color(?!\s*=))\b",
                RegexOptions.None);
            hashTable.Add("cs", regExp);
            #endregion

            // Add the information of VB.NET language to a Hashtable variable.
            #region  VB.NET language
            regExp = new RegExp();
            regExp.Add("str", "\"[^\"\\n]*\"", RegexOptions.None);
            regExp.Add("note", @"'[^\n\r]*", RegexOptions.None);
            regExp.Add("Var", @"(?<=\bclass\s+)([_a-z][_a-z0-9]*)(?=\s*[\{:])"
                + @"|(?<=\=\s*new\s+)([a-z_][a-z0-9_]*)(?=\s*\()"
                + @"|([a-z][a-z0-9_]*)(?=\s+[a-z_][a-z0-9_]*\s*=\s*new)",
                RegexOptions.IgnoreCase);
            regExp.Add("kw", @"\b(AddHandler|AddressOf|AndAlso|Alias|And|Ansi|As"
                + "|Assembly|Auto|Boolean|Class|CLng|CObj|Const|Char|CInt|CDbl"
                + "|ByRef|Byte|ByVal|Call|Case|Catch|CBool|CByte|CChar|CDate|CDec"
                + "|CShort|CSng|CStr|CType|Date|Decimal|Declare|Default|Delegate"
                + "|Dim|DirectCast|Do|Double|Each|Else|ElseIf|End|Handles|If"
                + "|Enum|Erase|Error|Event|Exit|False|Finally|For|Friend|Function"
                + "|Get|GetType|GoTo|Let|Lib|Like|Long|Loop|Me|Mod|Module|Nothing"
                + "|Implements|Imports|In|Inherits|Integer|Interface|Is|Public"
                + "|MustInherit|MustOverride|MyBase|MyClass|Namespace|New|Next|Not"
                + "|NotInheritable|NotOverridable|Object|On|Option|Optional|Or|OrElse"
                + "|Overloads|Overridable|Overrides|ParamArray|Preserve|Private"
                + "|RaiseEvent|ReadOnly|ReDim|RemoveHandler|Resume|Return|Property"
                + "|Select|Set|Shadows|Shared|Short|Single|Static|Step|Stop|String"
                + "|Structure|Sub|SyncLock|Then|Throw|Protected|TypeOf|Unicode|Try"
                + "|To|True|Until|Variant|When|While|With|WithEvents"
                + @"|WriteOnly|Xor(?!\s*=))\b", RegexOptions.None);
            regExp.Add("kwG", @"\b(EventArgs|Page|Label|TextBox|CheckBox|DropDownList"
                + @"|Control|Button|Nullable|DayRenderEventArgs|Color(?!\s*=))\b",
                RegexOptions.None);
            hashTable.Add("vb", regExp);
            #endregion

            // Add the information of CCS syntax to a Hashtable variable.
            #region CCS syntax
            regExp = new RegExp();
            regExp.Add("note", @"\/\*[\s\S]*?\*\/", RegexOptions.None);
            regExp.Add("str", @"([\s\S]+)", RegexOptions.None);
            regExp.Add("kw", @"(\{[^\}]+\})", RegexOptions.None);
            regExp.Add("sqlstr", @"([a-z\-]+(?=\s*:))", RegexOptions.IgnoreCase);
            regExp.Add("black", @"([\{\}])", RegexOptions.None);
            hashTable.Add("css", regExp);
            #endregion

            // Add the information of HTML language to a Hashtable variable.
            #region  HTML language
            regExp = new RegExp();
            regExp.Add("", "<%@\\s*page[\\s\\S]*?language=['\"](.*?)[\"']",
                RegexOptions.IgnoreCase);
            regExp.Add("", @"<!--([\s\S]*?)-->", RegexOptions.None);
            regExp.Add("", @"(<script[^>]*>)([\s\S]*?)<\/script>",
                RegexOptions.IgnoreCase);
            regExp.Add("", @"<%(?!@)([\s\S]*?)%>", RegexOptions.None);
            regExp.Add("", @"<\?php\b([\s\S]*?)\?>", RegexOptions.IgnoreCase);
            regExp.Add("", @"(<style[^>]*>)([\s\S]*?)<\/style>",
                RegexOptions.IgnoreCase);
            regExp.Add("", @"&([a-z]+;)", RegexOptions.None);
            regExp.Add("", @"'.*?'", RegexOptions.None);
            regExp.Add("", "\".*?\"", RegexOptions.None);
            regExp.Add("", @"<([^>]+)>", RegexOptions.None);
            hashTable.Add("html", regExp);
            #endregion
            return hashTable;
        }

        /// <summary>
        /// Replace the brackets which contained in quotes or single quotes.
        /// </summary>
        /// <param name="m"></param>
        /// <returns></returns>
        private static string NoteBrackets(Match m)
        {
            return "<span class='gray'>"
                + m.Groups[0].Value.Replace("<", "&lt;").Replace(">", "&gt;")
                + "</span>";
        }

        /// <summary>
        /// Replace the brackets.
        /// </summary>
        /// <param name="m"></param>
        /// <returns></returns>
        private static string RetrieveBrackets(Match m)
        {
            return m.Groups[0].Value.Replace("<", "&lt;").Replace(">", "&gt;");
        }


        /// <summary>
        /// Highlight the HTML code
        /// </summary>
        /// <param name="codeString">The string of code</param>
        /// <param name="hashTable">The regular expressions collection</param>
        /// <returns>Highlighted code</returns>
        public static string HighlightHTMLCode(string codeString,
            Hashtable hashTable)
        {
            string lang = "VB";
            RegExp regExp = (RegExp)hashTable["html"];
            Regex regex = ((RegexStruct)regExp.regexStructList[0]).regex,
                htmlR = ((RegexStruct)regExp.regexStructList[9]).regex;
            Match match = regex.Match(codeString);
            MatchCollection mc;
            ArrayList note = new ArrayList(),
                vb = new ArrayList(),
                js = new ArrayList(),
                cs = new ArrayList(),
                css = new ArrayList();
            int blockIndex = 0;

            // Get the default language of page.
            if (match.Groups[1].Value.Trim() != "") lang
                = match.Groups[1].Value.ToUpper().Trim();
            if (lang != "C#") lang = "VB";

            #region Characters replaced
            codeString = codeString.Replace("\\\"", "__CharactersQuotes__")
                .Replace("\\'", "__CharactersSingleQuote__");
            #endregion

            #region Script tags
            regex = ((RegexStruct)regExp.regexStructList[2]).regex;
            mc = regex.Matches(codeString);
            foreach (Match m in mc)
            {
                if (m.Groups[1].Value.ToLower().IndexOf("runat") == -1)
                {
                    // JavaScript tags.
                    if (m.Groups[1].Value.ToLower().
                        IndexOf("vbscript") == -1)
                    {
                        blockIndex = js.Count;
                        js.Add(htmlR.Replace(m.Groups[1].Value, HTMLEval) +
                            (m.Groups[2].Value.Trim() != "" ?
                            HighlightCode(m.Groups[2].Value, "js",
                            (RegExp)hashTable["js"]) : "")
                           + "<span class=\"kw\">&lt;/"
                           + "<span class=\"str\">script</span>&gt;</span> ");
                        codeString = regex.Replace(codeString, 
                            "__JS" + blockIndex + "__", 1);
                    }
                    else
                    {
                        // VBscript tags.
                        blockIndex = vb.Count;
                        vb.Add(htmlR.Replace(m.Groups[1].Value, HTMLEval) +
                            (m.Groups[2].Value.Trim() != "" ?
                            HighlightCode(m.Groups[2].Value, "vbs",
                            (RegExp)hashTable["vbs"]) : "")
                           + "<span class=\"kw\">&lt;/"
                           + "<span class=\"str\">script</span>&gt;</span> ");
                        codeString = regex.Replace(codeString, 
                            "__VB" + blockIndex + "__", 1);
                    }
                }
                else
                {
                    // C# language tags.
                    if (lang == "C#")
                    {
                        if (m.Groups[1].Value.ToLower().IndexOf("vb") == -1)
                        {
                            blockIndex = cs.Count;
                            cs.Add(htmlR.Replace(m.Groups[1].Value, HTMLEval) +
                                (m.Groups[2].Value.Trim() != "" ?
                                HighlightCode(m.Groups[2].Value, "cs",
                                (RegExp)hashTable["cs"]) : "")
                                + "<span class=\"kw\">&lt;/"
                                + "<span class=\"str\">script</span>&gt;</span> ");
                            codeString = regex.Replace(codeString,
                                "__C#" + blockIndex + "__", 1);
                        }
                        else
                        {
                            // VBScript language tags.
                            blockIndex = vb.Count;
                            vb.Add(htmlR.Replace(m.Groups[1].Value, HTMLEval) +
                                (m.Groups[2].Value.Trim() != "" ?
                                HighlightCode(m.Groups[2].Value, "vbs", 
                                (RegExp)hashTable["vbs"]) : "")
                                + "<span class=\"kw\">&lt;/"
                                + "<span class=\"str\">script</span>&gt;</span> ");
                            codeString = regex.Replace(codeString,
                                "__VB" + blockIndex + "__", 1);
                        }
                    }
                    else
                    {
                        if (m.Groups[1].Value.ToLower().IndexOf("c#") != -1)
                        {
                            blockIndex = cs.Count;
                            cs.Add(htmlR.Replace(m.Groups[1].Value, HTMLEval) +
                                (m.Groups[2].Value.Trim() != "" ?
                                HighlightCode(m.Groups[2].Value, "cs", 
                                (RegExp)hashTable["cs"]) : "")
                                + "<span class=\"kw\">&lt;/"
                                + "<span class=\"str\">script</span>&gt;</span> ");
                            codeString = regex.Replace(codeString,
                                "__C#" + blockIndex + "__", 1);
                        }
                        else
                        {
                            blockIndex = vb.Count;
                            vb.Add(htmlR.Replace(m.Groups[1].Value, HTMLEval) +
                                (m.Groups[2].Value.Trim() != "" ?
                                HighlightCode(m.Groups[2].Value, "vbs",
                                (RegExp)hashTable["vbs"]) : "")
                                + "<span class=\"kw\">&lt;/"
                                + "<span class=\"str\">script</span>&gt;</span> ");
                            codeString = regex.Replace(codeString,
                                "__VB" + blockIndex + "__", 1);
                        }
                    }

                }
            }
            #endregion

            #region Style tags
            regex = ((RegexStruct)regExp.regexStructList[5]).regex;
            mc = regex.Matches(codeString);
            blockIndex = 0;
            foreach (Match m in mc)
            {
                css.Add(htmlR.Replace(m.Groups[1].Value, HTMLEval) +
                        (m.Groups[2].Value.Trim() != "" ?
                        HighlightCode(m.Groups[2].Value, "css", 
                        (RegExp)hashTable["css"]) : "")
                       + "<span class=\"kw\">&lt;/"
                       + "<span class=\"str\">style</span>&gt;</span> ");
                codeString = regex.Replace(codeString,
                    "__CSS" + blockIndex + "__", 1);
                blockIndex++;
            }
            #endregion

            #region Comment tags
            regex = ((RegexStruct)regExp.regexStructList[1]).regex;
            mc = regex.Matches(codeString);
            blockIndex = 0;
            foreach (Match m in mc)
            {
                note.Add("<span class='note'>&lt;!--"
                    + m.Groups[1].Value.Replace("<", "&lt;") + "--&gt;</span>");
                codeString = regex.Replace(codeString,
                    "__Comments" + blockIndex + "__", 1);
                blockIndex++;
            }
            #endregion

            #region Code which contained in <%%> tags
            regex = ((RegexStruct)regExp.regexStructList[3]).regex;
            mc = regex.Matches(codeString);
            foreach (Match m in mc)
            {
                if (lang == "VB")
                {
                    blockIndex = vb.Count;
                    vb.Add("<span class='declare'>&lt;%</span>" +
                            (m.Groups[1].Value.Trim() != "" ?
                            HighlightCode(m.Groups[1].Value, "vbs",
                            (RegExp)hashTable["vbs"]) : "")
                            + "<span class='declare'>%&gt;</span>");
                }
                else
                {
                    blockIndex = cs.Count;
                    cs.Add("<span class='declare'>&lt;%</span>" +
                            (m.Groups[1].Value.Trim() != "" ?
                            HighlightCode(m.Groups[1].Value, "cs",
                            (RegExp)hashTable["cs"]) : "")
                            + "<span class='declare'>%&gt;</span>");
                }
                codeString = regex.Replace(codeString,
                    "__" + lang + blockIndex + "__", 1);
            }
            #endregion

            #region Replace '&' character
            codeString = ((RegexStruct)regExp.regexStructList[6]).regex
                .Replace(codeString, "&amp;$1");
            #endregion

            #region THML tags
            codeString = ((RegexStruct)regExp.regexStructList[7]).regex
                .Replace(codeString, RetrieveBrackets);
            codeString = ((RegexStruct)regExp.regexStructList[8]).regex
                .Replace(codeString, RetrieveBrackets);
            codeString = htmlR.Replace(codeString, HTMLEval);
            #endregion

            #region Replace the string back to original value.
            int i;

            // Comments.
            for (i = 0; i < note.Count; i++) codeString = codeString.Replace("__Comments"
                + i + "__", note[i].ToString());
            codeString = codeString.Replace("__CharactersQuotes__", "\\\"").
                Replace("__CharactersSingleQuote__", "\\'");

            // CSS.
            for (i = 0; i < css.Count; i++) codeString = codeString.Replace("__CSS"
                + i + "__", css[i].ToString());

            // C# language.
            for (i = 0; i < cs.Count; i++) codeString = codeString.Replace("__C#"
                + i + "__", cs[i].ToString());

            // VBScript language or vb language.
            for (i = 0; i < vb.Count; i++) codeString = codeString.Replace("__VB"
                + i + "__", vb[i].ToString());

            // Javascript language.
            for (i = 0; i < js.Count; i++) codeString = codeString.Replace("__JS"
                + i + "__", js[i].ToString());
            #endregion

            return codeString;
        }


        /// <summary>
        /// Highlight the code depend on the language(except HTML language).
        /// </summary>
        /// <param name="codeString">The string of code</param>
        /// <param name="language">The language of code</param>
        /// <param name="regExp">The regular expressions object</param>
        /// <returns>Highlighted code</returns>
        public static string HighlightCode(string codeString, 
            string language, RegExp regExp)
        {

            language = language.ToLower();
            codeString = codeString.Replace("<!--", "&lt;!--");
            RegexStruct regexStruct;
            ArrayList styleString = new ArrayList(),
                note = new ArrayList(),
                xmlnote = new ArrayList();
            MatchCollection mc;
            int blockIndex = 0;

            #region Characters replaced
            if (language != "css")
                codeString = codeString.Replace("\\\"", "__CharactersQuotes__")
                .Replace("\\'", "__CharactersSingleQuote__");
            #endregion

            #region String replaced
            if (language != "css")
            {
                regexStruct = (RegexStruct)regExp.regexStructList[0];
                mc = regexStruct.regex.Matches(codeString);
                foreach (Match m in mc)
                {
                    styleString.Add("<span class='" + regexStruct.styleObject + "'>"
                        + m.Groups[0].Value.Replace("<", "&lt;")
                        + "</span>");
                    codeString = regexStruct.regex.Replace(codeString, 
                        "__StringVariables"+ blockIndex + "__", 1);
                    blockIndex++;
                }
            }
            #endregion

            #region XML Comments for C# language replaced
            blockIndex = 0;
            if (language == "cs")
            {
                Regex regex = new Regex(@"((?<!/)///(?!/))([^\r\n]*)?"),
                    attri = new Regex(@"(<[^>]+>)");
                mc = regex.Matches(codeString);
                string tmp = "";
                foreach (Match m in mc)
                {
                    tmp = m.Groups[2].Value;
                    tmp = attri.Replace(tmp, NoteBrackets);
                    xmlnote.Add("<span class='note'>"
                        + "<span class='gray'>///</span>"
                        + tmp + "</span>");
                    codeString = regex.Replace(codeString,
                        "__XMLComments" + blockIndex + "__", 1);
                    blockIndex++;
                }
            }
            #endregion

            #region Comments replaced
            regexStruct = (RegexStruct)regExp.regexStructList[language == "css" ? 0 : 1];
            mc = regexStruct.regex.Matches(codeString);
            blockIndex = 0;
            foreach (Match m in mc)
            {
                note.Add("<span class='" + regexStruct.styleObject + "'>"
                    + m.Groups[0].Value.Replace("<", "&lt;")
                    .Replace(">", "&gt;")
                    + "</span>");
                codeString = regexStruct.regex.Replace(codeString, 
                    "__Comments" + blockIndex + "__", 1);
                blockIndex++;
            }
            #endregion

            #region other replaced
            int i = language == "css" ? 1 : 2;
            for (; i < regExp.regexStructList.Count; i++)
            {
                regexStruct = (RegexStruct)regExp.regexStructList[i];
                if (language == "cs" && regexStruct.styleObject == "Var") 
                    codeString = regexStruct.regex.Replace(codeString, 
                        "<span class='Var'>$1$2$3</span>");
                else
                    codeString = regexStruct.regex.Replace(codeString, "<span class='"
                        + regexStruct.styleObject+ "'>$1</span>");
            }
            #endregion


            #region Replace the string back to original value.
            if (language != "css") for (i = 0; i < styleString.Count; i++)
                    codeString = codeString.Replace("__StringVariables" +
                i + "__", styleString[i].ToString());
            if (language == "cs") for (i = 0; i < xmlnote.Count; i++)
                    codeString = codeString.Replace("__XMLComments" +
                i + "__", xmlnote[i].ToString());
            for (i = 0; i < note.Count; i++)
                codeString = codeString.Replace("__Comments"
                + i + "__", note[i].ToString());
            if (language != "css")
            {
                // Replace the string which contains comments.
                if (codeString.IndexOf("__XMLComments") != -1)
                    for (i = 0; i < styleString.Count; i++)
                        for (i = 0; i < xmlnote.Count; i++)
                            codeString = codeString.Replace("__XMLComments" + i
                                + "__",ClearHTMLTag(xmlnote[i].ToString()));
                if (codeString.IndexOf("__Comments") != -1)
                    for (i = 0; i < styleString.Count; i++)
                        for (i = 0; i < note.Count; i++)
                            codeString = codeString.Replace("__Comments" + i
                                + "__",ClearHTMLTag(note[i].ToString()));
                if (codeString.IndexOf("__StringVariables") != -1)
                    for (i = 0; i < styleString.Count; i++)
                        codeString = codeString.Replace("__StringVariables" + i
                            + "__", ClearHTMLTag(styleString[i].ToString()));

                if (codeString.IndexOf("__XMLComments") != -1)
                    for (i = 0; i < xmlnote.Count; i++)
                        codeString = codeString.Replace("__XMLComments" + i
                            + "__", xmlnote[i].ToString());
                codeString = codeString.Replace("__CharactersQuotes__", "\\\"")
                    .Replace("__CharactersSingleQuote__", "\\'");
            }
            #endregion
            return codeString;
        }


        /// <summary>
        /// Clear the HTML tags.
        /// </summary>
        public static string ClearHTMLTag(string htmlString)
        {
            // Clear the script tags.
            htmlString = Regex.Replace(htmlString,
                @"<script[^>]*?>.*?</script>",
                "", RegexOptions.IgnoreCase);

            // Clear the HTML tags.
            htmlString = Regex.Replace(htmlString, @"<(.[^>]*)>",
                "", RegexOptions.IgnoreCase);
            htmlString = Regex.Replace(htmlString, @"([\r\n])[\s]+",
                "", RegexOptions.IgnoreCase);
            htmlString = Regex.Replace(htmlString, @"-->",
                "", RegexOptions.IgnoreCase);
            htmlString = Regex.Replace(htmlString, @"<!--.*",
                "", RegexOptions.IgnoreCase);

            htmlString = Regex.Replace(htmlString, @"&(quot|#34);",
                "\"", RegexOptions.IgnoreCase);
            htmlString = Regex.Replace(htmlString, @"&(amp|#38);",
                "&", RegexOptions.IgnoreCase);
            htmlString = Regex.Replace(htmlString, @"&(lt|#60);",
                "<", RegexOptions.IgnoreCase);
            htmlString = Regex.Replace(htmlString, @"&(gt|#62);",
                ">", RegexOptions.IgnoreCase);
            htmlString = Regex.Replace(htmlString, @"&(nbsp|#160);",
                " ", RegexOptions.IgnoreCase);
            htmlString = Regex.Replace(htmlString, @"&(iexcl|#161);",
                "\xa1", RegexOptions.IgnoreCase);
            htmlString = Regex.Replace(htmlString, @"&(cent|#162);",
                "\xa2", RegexOptions.IgnoreCase);
            htmlString = Regex.Replace(htmlString, @"&(pound|#163);",
                "\xa3", RegexOptions.IgnoreCase);
            htmlString = Regex.Replace(htmlString, @"&(copy|#169);",
                "\xa9", RegexOptions.IgnoreCase);
            htmlString = Regex.Replace(htmlString, @"&#(\d+);",
                "", RegexOptions.IgnoreCase);

            htmlString.Replace("<", "");
            htmlString.Replace(">", "");
            htmlString.Replace("\r\n", "");
            htmlString = HttpContext.Current.Server
                .HtmlEncode(htmlString).Trim();
            return htmlString;
        }

        /// <summary>
        /// Replace the properties of the THML tags.
        /// </summary>
        /// <param name="m">Matching collection</param>
        /// <returns>Replaced code</returns>
        private static string HTMLEval(Match m)
        {
            string tmp = m.Groups[1].Value;
            if (tmp.StartsWith("/"))
                return "<span class='kw'>&lt;/<span class='str'>"
                    + tmp.Substring(1) + "</span>&gt;</span>";
            else if (new Regex(@"^([_0-9a-z]+)\s*\/$",
                RegexOptions.IgnoreCase).IsMatch(tmp))
                return "<span class='kw'>&lt;<span class='str'>"
                    + tmp.Substring(0, tmp.Length - 1)
                    + "</span>&gt;</span>";
            else if (tmp.ToLower().StartsWith("!doctype"))
            {
                tmp = "<span class='kw'>" + m.Groups[0].Value.Substring(1)
                    + "</span>";
                tmp = new Regex(@"\b(html|public)\b",
                    RegexOptions.IgnoreCase).Replace(tmp, 
                    "<span class='sqlstr'>$1</span>");
                return "<span class='kw'>&lt;!" + tmp + "&gt;</span>";
            }
            else
            {
                Regex regex = new Regex("([a-z_][a-z_0-9\\.\\-]*)\\s*=\\s*\"([^\"]*)\"",
                    RegexOptions.IgnoreCase);
                tmp = regex.Replace(tmp,
                    "<span class=\"sqlstr\">$1</span><span class=\"kw\">=\"$2\"</span>");
                regex = new Regex("([a-z_][a-z_0-9\\.\\-]*)\\s*=\\s*'([^']*)'",
                    RegexOptions.IgnoreCase);
                tmp = regex.Replace(tmp,
                    "<span class=\"sqlstr\">$1</span><span class=\"kw\">='$2'</span>");
                regex = new Regex("([a-z_][a-z_0-9\\-]*)\\s*=\\s*(?!['\"])(\\w+)",
                    RegexOptions.IgnoreCase);
                tmp = regex.Replace(tmp,
                    "<span class=\"sqlstr\">$1</span><span class=\"kw\">=$2</span>");

                regex = new Regex(@"^([a-z_0-9\-]+)", RegexOptions.IgnoreCase);
                tmp = regex.Replace(tmp, "<span class='str'>$1</span>");
                if (tmp.StartsWith("%@"))
                    return "<span class='str'><span class='declare'>&lt;%</span>"
                        + "<span class='kw'>@</span>"
                        + tmp.Trim(new char[] { '%', '@' })
                        + "<span class='declare'>%&gt;</span></span>";
                return "<span class='kw'>&lt;" + tmp + "&gt;</span>";
            }
        }
        /// <summary>
        /// Replace the \r, \n to <br/>.
        /// </summary>
        /// <param name="codeString"></param>
        /// <returns></returns>
        public static string Encode(string codeString)
        {
            codeString = codeString.Replace("\r", "").Replace("\n", "<br/>");
            return Regex.Replace(codeString, "(?<!<span)( +)(?!class)", 
                GetSpace, RegexOptions.Compiled);
        }

        /// <summary>
        /// Replace the space to &nbsp;.
        /// </summary>
        /// <param name="m"></param>
        /// <returns></returns>
        public static string GetSpace(Match m)
        {
            return m.Groups[1].Value.Replace(" ", "&nbsp;");
        }
    }
}