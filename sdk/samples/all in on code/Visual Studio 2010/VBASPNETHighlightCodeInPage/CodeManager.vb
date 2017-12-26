'*************************** Module Header ***********************************\
'* Module Name:    CodeManager.VB
'* Project:        VBASPNETHighlightCodeInPage
'* Copyright (c) Microsoft Corporation
'*
'* Sometimes we input code like VB or HTML in our post and we need these code 
'* to be highlighted for a better reading experience.This project illustrates how
'* to highlight the code in a page. 
'
'* In this file,we use a Hashtable variable to store the different languages of 
'* code and their related regular expressions with matching options.Then add the
'* style object to the matching string of code.
'
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'*
'\****************************************************************************


''' <summary>
''' The structure of style string and regular expressions. 
''' </summary>
Public Structure RegexStruct
    Public styleObject As String
    Public regex As Regex
End Structure

''' <summary>
''' Matching string and style object.
''' </summary>
Public Class RegExp
    ''' <summary>
    ''' Store the matching string collection.
    ''' </summary>
    Private _regexStructList As New ArrayList()

    ''' <summary>
    ''' </summary>
    ''' <param name="styleObject">Style object</param>
    ''' <param name="reg">Regular expressions</param>
    ''' <param name="regOption">Matching options</param>
    Public Sub Add(ByVal styleObject As String, ByVal reg As String, _
                   ByVal regOption As RegexOptions)
        Dim regexStruct As New RegexStruct()
        regexStruct.styleObject = styleObject
        regexStruct.regex = New Regex(reg, regOption Or RegexOptions.Compiled)
        _regexStructList.Add(regexStruct)
    End Sub
    ''' <summary>
    ''' Return the matching string collection ，read only.
    ''' </summary>
    Public ReadOnly Property regexStructList() As ArrayList
        Get
            Return _regexStructList
        End Get
    End Property
End Class

''' <summary>
''' Highlight the code operation.
''' </summary>
Public Class CodeManager
    ''' <summary>
    ''' Initialize the Hashtable variable which used to store the 
    ''' the different languages of code and their related regular
    ''' expressions with matching options.
    ''' </summary>
    ''' <returns></returns>
    Public Shared Function Init() As Hashtable
        Dim hashTable As New Hashtable()
        Dim regExp As New RegExp()

        ' Add the information of VBScript language to a Hashtable variable.   
        '#Region "VBScript language"
        regExp.Add("str", """([^""\n]*?)""", RegexOptions.None)
        regExp.Add("note", "'[^" & vbCr & vbLf & "]*", RegexOptions.None)
        regExp.Add("kw", "\b(elseif|if|then|else|select|case|end|for|while" _
                   & "|wend|do|loop|until|abs|sgn|hex|oct|sqr|int|fix|round" _
                   & "|log|split|cint|sin|cos|tan|len|mid|left|right|lcase|ucase" _
                   & "|trim|ltrim|rtrim|replace|instr|instrrev|space|string" _
                   & "|strreverse|cstr|clng|cbool|cdate|csng|cdbl|date|time|now" _
                   & "|dateadd|datediff|dateserial|datevalue|year|month|day|hour" _
                   & "|minute|second|timer|timeserial|timevalue|weekday|monthname" _
                   & "|array|asc|chr|filter|inputbox|join|msgbox|lbound|ubound" _
                   & "|redim|randomize|rnd|isempty|mod|execute|not|and|or|xor" _
                   & "|const|dim|erase" & "|class(?!\s*=))\b", _
                   RegexOptions.IgnoreCase)
        hashTable.Add("vbs", regExp)
        '#End Region

        ' Add the information of JavaScript language to a Hashtable variable.   
        '#Region "JavaScript language"
        regExp = New RegExp()
        regExp.Add("str", """[^""\n]*""|'[^'\n]*'", RegexOptions.None)
        regExp.Add("note", "\/\/[^\n\r]*|\/\*[\s\S]*?\*\/", RegexOptions.None)
        regExp.Add("kw", "\b(break|delete|function|return|typeof|case|do|if" _
                   & "|switch|var|catch|else|in|this|void|continue|false|nstanceof" _
                   & "|throw|while|debugger|finally|new|true|with|default|for|null" _
                   & "|try|abstract|double|goto|native|static|boolean|enum|implements" _
                   & "|package|super|byte|export|import|private|synchronized|char" _
                   & "|extends|int|protected|throws|final|interface|public|transient" _
                   & "|const|float|long|short|volatile" _
                   & "|class(?!\s*=))\b", RegexOptions.None)
        hashTable.Add("js", regExp)
        '#End Region

        ' Add the information of SqlServer language to a Hashtable variable.   
        '#Region "SqlServer language"
        regExp = New RegExp()
        regExp.Add("sqlstr", "'([^'\n]*?)*'", RegexOptions.None)
        regExp.Add("note", "--[^\n\r]*|\/\*[\s\S]*?\*\/", RegexOptions.None)
        regExp.Add("sqlconnect", "\b(all|and|between|cross|exists|in|join|like" _
                   & "|not|null|outer|or)" _
                   & vbBack, RegexOptions.IgnoreCase)
        regExp.Add("sqlfunc", "\b(avg|case|checksum|current_timestamp|day|left" _
                   & "|month|replace|year)" _
                   & vbBack, RegexOptions.IgnoreCase)
        regExp.Add("kw", "\b(action|add|alter|after|as|asc|bigint|bit|binary|by" _
                   & "|cascade|char|character|check|column|columns|constraint|create" _
                   & "|current_date|current_time|database|date|datetime|dec|decimal" _
                   & "|default|delete|desc|distinct|double|drop|end|else|escape|file" _
                   & "|first|float|foreign|from|for|full|function|global|grant|group" _
                   & "|having|hour|ignore|index|inner|insert|int|integer|into|if|is" _
                   & "|key|kill|load|local|max|minute|modify|numeric|no|on|option|order" _
                   & "|partial|password|precision|primary|procedure|privileges" _
                   & "|read|real|references|restrict|returns|revoke|rows|second|select" _
                   & "|set|shutdown|smallint|table|temporary|text|then|time" _
                   & "|timestamp|tinyint|to|use|unique|update|values|varchar|varying" _
                   & "|varbinary|with|when|where)\b", RegexOptions.IgnoreCase)
        hashTable.Add("sql", regExp)
        '#End Region

        ' Add the information of C# language to a Hashtable variable.   
        '#Region "C# language"
        regExp = New RegExp()
        regExp.Add("str", """[^""\n]*""", RegexOptions.None)
        regExp.Add("note", "\/\/[^\n\r]*|\/\*[\s\S]*?\*\/", RegexOptions.None)
        regExp.Add("Var", "(?<=\bclass\s+)([_a-z][_a-z0-9]*)(?=\s*[\{:])" _
                   & "|(?<=\=\s*new\s+)([a-z_][a-z0-9_]*)(?=\s*\()" _
                   & "|([a-z][a-z0-9_]*)(?=\s+[a-z_][a-z0-9_]*\s*=\s*new)", _
                   RegexOptions.IgnoreCase)
        regExp.Add("kw", "\b(partial|abstract|event|get|set|value|new|struct|as" _
                   & "|null|switch|base|object|this|bool|false|operator|throw|break" _
                   & "|finally|out|byte|fixed|override|try|case|float|params|typeof" _
                   & "|catch|for|private|uint|char|foreach|protected|ulong|checked" _
                   & "|goto|public|unchecked|if|readonly|unsafe|const|implicit|ref" _
                   & "|ushort|continue|in|return|using|decimal|int|sbyte|virtual" _
                   & "|default|interface|sealed|volatile|delegate|internal|short|void" _
                   & "|do|is|sizeof|while|double|lock|stackalloc|else|long|static" _
                   & "|enum|string|namespace|region|endregion|class(?!\s*=))\b", _
                   RegexOptions.None)
        regExp.Add("kwG", "\b(EventArgs|Page|Label|TextBox|CheckBox|DropDownList" _
                   & "|Control|Button|DayRenderEventArgs|Color(?!\s*=))\b", _
                   RegexOptions.None)
        hashTable.Add("cs", regExp)
        '#End Region

        ' Add the information of VB.NET language to a Hashtable variable.
        '#Region "VB.NET language"
        regExp = New RegExp()
        regExp.Add("str", """[^""\n]*""", RegexOptions.None)
        regExp.Add("note", "'[^\n\r]*", RegexOptions.None)
        regExp.Add("Var", "(?<=\bclass\s+)([_a-z][_a-z0-9]*)(?=\s*[\{:])" _
                   & "|(?<=\=\s*new\s+)([a-z_][a-z0-9_]*)(?=\s*\()" _
                   & "|([a-z][a-z0-9_]*)(?=\s+[a-z_][a-z0-9_]*\s*=\s*new)", _
                   RegexOptions.IgnoreCase)
        regExp.Add("kw", "\b(AddHandler|AddressOf|AndAlso|Alias|And|Ansi|As" _
                   & "|Assembly|Auto|Boolean|Class|CLng|CObj|Const|Char|CInt|CDbl" _
                   & "|ByRef|Byte|ByVal|Call|Case|Catch|CBool|CByte|CChar|CDate|CDec" _
                   & "|CShort|CSng|CStr|CType|Date|Decimal|Declare|Default|Delegate" _
                   & "|Dim|DirectCast|Do|Double|Each|Else|ElseIf|End|Handles|If" _
                   & "|Enum|Erase|Error|Event|Exit|False|Finally|For|Friend|Function" _
                   & "|Get|GetType|GoTo|Let|Lib|Like|Long|Loop|Me|Mod|Module|Nothing" _
                   & "|Implements|Imports|In|Inherits|Integer|Interface|Is|Public" _
                   & "|MustInherit|MustOverride|MyBase|MyClass|Namespace|New|Next|Not" _
                   & "|NotInheritable|NotOverridable|Object|On|Option|Optional|Or|OrElse" _
                   & "|Overloads|Overridable|Overrides|ParamArray|Preserve|Private" _
                   & "|RaiseEvent|ReadOnly|ReDim|RemoveHandler|Resume|Return|Property" _
                   & "|Select|Set|Shadows|Shared|Short|Single|Static|Step|Stop|String" _
                   & "|Structure|Sub|SyncLock|Then|Throw|Protected|TypeOf|Unicode|Try" _
                   & "|To|True|Until|Variant|When|While|With|WithEvents" _
                   & "|WriteOnly|Xor(?!\s*=))\b", RegexOptions.None)
        regExp.Add("kwG", "\b(EventArgs|Page|Label|TextBox|CheckBox|DropDownList" _
                   & "|Control|Button|Nullable|DayRenderEventArgs|Color(?!\s*=))\b", _
                   RegexOptions.None)
        hashTable.Add("vb", regExp)
        '#End Region

        ' Add the information of CCS syntax to a Hashtable variable.
        '#Region "CCS syntax"
        regExp = New RegExp()
        regExp.Add("note", "\/\*[\s\S]*?\*\/", RegexOptions.None)
        regExp.Add("str", "([\s\S]+)", RegexOptions.None)
        regExp.Add("kw", "(\{[^\}]+\})", RegexOptions.None)
        regExp.Add("sqlstr", "([a-z\-]+(?=\s*:))", RegexOptions.IgnoreCase)
        regExp.Add("black", "([\{\}])", RegexOptions.None)
        hashTable.Add("css", regExp)
        '#End Region

        ' Add the information of HTML language to a Hashtable variable.
        '#Region "HTML language"
        regExp = New RegExp()
        regExp.Add("", "<%@\s*page[\s\S]*?language=['""](.*?)[""']", _
                   RegexOptions.IgnoreCase)
        regExp.Add("", "<!--([\s\S]*?)-->", RegexOptions.None)
        regExp.Add("", "(<script[^>]*>)([\s\S]*?)<\/script>", _
                   RegexOptions.IgnoreCase)
        regExp.Add("", "<%(?!@)([\s\S]*?)%>", RegexOptions.None)
        regExp.Add("", "<\?php\b([\s\S]*?)\?>", RegexOptions.IgnoreCase)
        regExp.Add("", "(<style[^>]*>)([\s\S]*?)<\/style>", _
                   RegexOptions.IgnoreCase)
        regExp.Add("", "&([a-z]+;)", RegexOptions.None)
        regExp.Add("", "'.*?'", RegexOptions.None)
        regExp.Add("", """.*?""", RegexOptions.None)
        regExp.Add("", "<([^>]+)>", RegexOptions.None)
        hashTable.Add("html", regExp)
        '#End Region
        Return hashTable
    End Function

    ''' <summary>
    ''' Replace the brackets which contained in quotes or single quotes.
    ''' </summary>
    ''' <param name="m"></param>
    ''' <returns></returns>
    Private Shared Function NoteBrackets(ByVal m As Match) As String
        Return "<span class='gray'>" _
            & m.Groups(0).Value.Replace("<", "&lt;").Replace(">", "&gt;") _
            & "</span>"
    End Function

    ''' <summary>
    ''' Replace the brackets.
    ''' </summary>
    ''' <param name="m"></param>
    ''' <returns></returns>
    Private Shared Function RetrieveBrackets(ByVal m As Match) As String
        Return m.Groups(0).Value.Replace("<", "&lt;").Replace(">", "&gt;")
    End Function


    ''' <summary>
    ''' Highlight the HTML code
    ''' </summary>
    ''' <param name="codeString">The string of code</param>
    ''' <param name="hashTable">The regular expressions collection</param>
    ''' <returns>Highlighted code</returns>
    Public Shared Function HighlightHTMLCode(ByVal codeString As String, _
                                             ByVal hashTable As Hashtable) As String
        Dim lang As String = "VB"
        Dim regExp As RegExp = DirectCast(hashTable("html"), RegExp)
        Dim regex As Regex = CType(regExp.regexStructList(0), RegexStruct).regex, _
            htmlR As Regex = CType(regExp.regexStructList(9), RegexStruct).regex
        Dim match As Match = regex.Match(codeString)
        Dim mc As MatchCollection
        Dim note As New ArrayList(), vb As New ArrayList(), js As New ArrayList(), _
            cs As New ArrayList(), css As New ArrayList()
        Dim blockIndex As Integer = 0

        ' Get the default language of page.
        If match.Groups(1).Value.Trim() <> "" Then
            lang = match.Groups(1).Value.ToUpper().Trim()
        End If
        If lang <> "C#" Then
            lang = "VB"
        End If

        '#Region "Characters replaced"
        codeString = codeString.Replace("\""", "__CharactersQuotes__").
            Replace("\'", "__CharactersSingleQuote__")
        '#End Region

        '#Region "Script tags"
        regex = CType(regExp.regexStructList(2), RegexStruct).regex
        mc = regex.Matches(codeString)
        For Each m As Match In mc
            If m.Groups(1).Value.ToLower().IndexOf("runat") = -1 Then
                ' JavaScript tags.
                If m.Groups(1).Value.ToLower().IndexOf("vbscript") = -1 Then
                    blockIndex = js.Count
                    js.Add(htmlR.Replace(m.Groups(1).Value, AddressOf HTMLEval) _
                           & (If(m.Groups(2).Value.Trim() <> "",
                                 HighlightCode(m.Groups(2).Value,
                                               "js", DirectCast(hashTable("js"), RegExp)), "")) _
                                   & "<span class=""kw"">&lt;/" _
                                   & "<span class=""str"">script</span>&gt;</span> ")
                    codeString = regex.Replace(codeString, "__JS" & blockIndex & "__", 1)
                Else
                    ' VBscript tags.
                    blockIndex = vb.Count
                    vb.Add(htmlR.Replace(m.Groups(1).Value, AddressOf HTMLEval) _
                           & (If(m.Groups(2).Value.Trim() <> "",
                                 HighlightCode(m.Groups(2).Value,
                                               "vbs", DirectCast(hashTable("vbs"), RegExp)), "")) _
                                   & "<span class=""kw"">&lt;/" _
                                   & "<span class=""str"">script</span>&gt;</span> ")
                    codeString = regex.Replace(codeString, "__VB" & blockIndex & "__", 1)
                End If
            Else
                ' C# language tags.
                If lang = "C#" Then
                    If m.Groups(1).Value.ToLower().IndexOf("vb") = -1 Then
                        blockIndex = cs.Count
                        cs.Add(htmlR.Replace(m.Groups(1).Value, AddressOf HTMLEval) _
                               & (If(m.Groups(2).Value.Trim() <> "",
                                     HighlightCode(m.Groups(2).Value,
                                                   "cs", DirectCast(hashTable("cs"), RegExp)), "")) _
                                       & "<span class=""kw"">&lt;/" _
                                       & "<span class=""str"">script</span>&gt;</span> ")
                        codeString = regex.Replace(codeString, "__C#" & blockIndex & "__", 1)
                    Else
                        ' VBScript language tags.
                        blockIndex = vb.Count
                        vb.Add(htmlR.Replace(m.Groups(1).Value, AddressOf HTMLEval) _
                               & (If(m.Groups(2).Value.Trim() <> "",
                                     HighlightCode(m.Groups(2).Value,
                                                   "vbs", DirectCast(hashTable("vbs"), RegExp)), "")) _
                                       & "<span class=""kw"">&lt;/" _
                                       & "<span class=""str"">script</span>&gt;</span> ")
                        codeString = regex.Replace(codeString, "__VB" & blockIndex & "__", 1)
                    End If
                Else
                    If m.Groups(1).Value.ToLower().IndexOf("c#") <> -1 Then
                        blockIndex = cs.Count
                        cs.Add(htmlR.Replace(m.Groups(1).Value, AddressOf HTMLEval) _
                               & (If(m.Groups(2).Value.Trim() <> "",
                                     HighlightCode(m.Groups(2).Value,
                                                   "cs", DirectCast(hashTable("cs"), RegExp)), "")) _
                                       & "<span class=""kw"">&lt;/" _
                                       & "<span class=""str"">script</span>&gt;</span> ")
                        codeString = regex.Replace(codeString, "__C#" & blockIndex & "__", 1)
                    Else
                        blockIndex = vb.Count
                        vb.Add(htmlR.Replace(m.Groups(1).Value, AddressOf HTMLEval) _
                               & (If(m.Groups(2).Value.Trim() <> "",
                                     HighlightCode(m.Groups(2).Value,
                                                   "vbs", DirectCast(hashTable("vbs"), RegExp)), "")) _
                                       & "<span class=""kw"">&lt;/" _
                                       & "<span class=""str"">script</span>&gt;</span> ")
                        codeString = regex.Replace(codeString, "__VB" & blockIndex & "__", 1)
                    End If

                End If
            End If
        Next
        '#End Region

        '#Region "Style tags"
        regex = CType(regExp.regexStructList(5), RegexStruct).regex
        mc = regex.Matches(codeString)
        blockIndex = 0
        For Each m As Match In mc
            css.Add(htmlR.Replace(m.Groups(1).Value, AddressOf HTMLEval) _
                    & (If(m.Groups(2).Value.Trim() <> "",
                          HighlightCode(m.Groups(2).Value,
                                        "css", DirectCast(hashTable("css"), RegExp)), "")) _
                            & "<span class=""kw"">&lt;/" _
                            & "<span class=""str"">style</span>&gt;</span> ")
            codeString = regex.Replace(codeString, "__CSS" & blockIndex & "__", 1)
            blockIndex += 1
        Next
        '#End Region

        '#Region "Comment tags"
        regex = CType(regExp.regexStructList(1), RegexStruct).regex
        mc = regex.Matches(codeString)
        blockIndex = 0
        For Each m As Match In mc
            note.Add("<span class='note'>&lt;!--" _
                     & m.Groups(1).Value.Replace("<", "&lt;") _
                     & "--&gt;</span>")
            codeString = regex.Replace(codeString, "__Comments" _
                                       & blockIndex & "__", 1)
            blockIndex += 1
        Next
        '#End Region

        '#Region "Code which contained in <%%> tags"
        regex = CType(regExp.regexStructList(3), RegexStruct).regex
        mc = regex.Matches(codeString)
        For Each m As Match In mc
            If lang = "VB" Then
                blockIndex = vb.Count
                vb.Add("<span class='declare'>&lt;%</span>" _
                       & (If(m.Groups(1).Value.Trim() <> "",
                             HighlightCode(m.Groups(1).Value,
                                           "vbs", DirectCast(hashTable("vbs"), RegExp)), "")) _
                               & "<span class='declare'>%&gt;</span>")
            Else
                blockIndex = cs.Count
                cs.Add("<span class='declare'>&lt;%</span>" _
                       & (If(m.Groups(1).Value.Trim() <> "",
                             HighlightCode(m.Groups(1).Value,
                                           "cs", DirectCast(hashTable("cs"), RegExp)), "")) _
                               & "<span class='declare'>%&gt;</span>")
            End If
            codeString = regex.Replace(codeString, "__" & lang & blockIndex & "__", 1)
        Next
        '#End Region

        '#Region "Replace '&' character"
        codeString = CType(regExp.regexStructList(6), RegexStruct).
            regex.Replace(codeString, "&amp;$1")
        '#End Region

        '#Region "THML tags"
        codeString = CType(regExp.regexStructList(7), RegexStruct).
            regex.Replace(codeString, AddressOf RetrieveBrackets)
        codeString = CType(regExp.regexStructList(8), RegexStruct).
            regex.Replace(codeString, AddressOf RetrieveBrackets)
        codeString = htmlR.Replace(codeString, AddressOf HTMLEval)
        '#End Region

        '#Region "Replace the string back to original value."
        Dim i As Integer

        ' Comments.
        For i = 0 To note.Count - 1
            codeString = codeString.Replace("__Comments" & i _
                                            & "__", note(i).ToString())
        Next
        codeString = codeString.Replace("__CharactersQuotes__", "\""").
            Replace("__CharactersSingleQuote__", "\'")

        ' CSS.
        For i = 0 To css.Count - 1
            codeString = codeString.Replace("__CSS" & i & "__", css(i).ToString())
        Next

        ' C# language.
        For i = 0 To cs.Count - 1
            codeString = codeString.Replace("__C#" & i & "__", cs(i).ToString())
        Next

        ' VBScript language or vb language.
        For i = 0 To vb.Count - 1
            codeString = codeString.Replace("__VB" & i & "__", vb(i).ToString())
        Next

        ' Javascript language.
        For i = 0 To js.Count - 1
            codeString = codeString.Replace("__JS" & i & "__", js(i).ToString())
        Next
        '#End Region

        Return codeString
    End Function


    ''' <summary>
    ''' Highlight the code depend on the language(except HTML language).
    ''' </summary>
    ''' <param name="codeString">The string of code</param>
    ''' <param name="language">The language of code</param>
    ''' <param name="regExp">The regular expressions object</param>
    ''' <returns>Highlighted code</returns>
    Public Shared Function HighlightCode(ByVal codeString As String, _
                                         ByVal language As String, _
                                         ByVal regExp As RegExp) As String

        language = language.ToLower()
        codeString = codeString.Replace("<!--", "&lt;!--")
        Dim regexStruct As RegexStruct
        Dim styleString As New ArrayList(),
            note As New ArrayList(),
            xmlnote As New ArrayList()

        Dim mc As MatchCollection
        Dim blockIndex As Integer = 0

        '#Region "Characters replaced"
        If language <> "css" Then
            codeString = codeString.
                Replace("\""", "__CharactersQuotes__").
                Replace("\'", "__CharactersSingleQuote__")
        End If
        '#End Region

        '#Region "String replaced"
        If language <> "css" Then
            regexStruct = CType(regExp.regexStructList(0), RegexStruct)
            mc = regexStruct.regex.Matches(codeString)
            For Each m As Match In mc
                styleString.Add("<span class='" _
                                & regexStruct.styleObject & "'>" _
                                & m.Groups(0).Value.Replace("<", "&lt;") & "</span>")
                codeString = regexStruct.regex.Replace(codeString,
                                                       "__StringVariables" _
                                                       & blockIndex _
                                                       & "__", 1)
                blockIndex += 1
            Next
        End If
        '#End Region

        '#Region "XML Comments for C# language replaced"
        blockIndex = 0
        If language = "cs" Then
            Dim regex As New Regex("((?<!/)///(?!/))([^\r\n]*)?"), _
                attri As New Regex("(<[^>]+>)")
            mc = regex.Matches(codeString)
            Dim tmp As String = ""
            For Each m As Match In mc
                tmp = m.Groups(2).Value
                tmp = attri.Replace(tmp, AddressOf NoteBrackets)
                xmlnote.Add("<span class='note'>" _
                            & "<span class='gray'>///</span>" _
                            & tmp & "</span>")
                codeString = regex.Replace(codeString,
                                           "__XMLComments" & blockIndex & "__", 1)
                blockIndex += 1
            Next
        End If
        '#End Region

        '#Region "Comments replaced"
        regexStruct = CType(regExp.
            regexStructList(If(language = "css", 0, 1)), RegexStruct)
        mc = regexStruct.regex.Matches(codeString)
        blockIndex = 0
        For Each m As Match In mc
            note.Add("<span class='" & regexStruct.styleObject _
                     & "'>" & m.Groups(0).Value.Replace("<", "&lt;").Replace(">", "&gt;") _
                     & "</span>")
            codeString = regexStruct.regex.Replace(codeString,
                                                   "__Comments" & blockIndex & "__", 1)
            blockIndex += 1
        Next
        '#End Region

        '#Region "other replaced"
        Dim i As Integer = If(language = "css", 1, 2)
        While i < regExp.regexStructList.Count
            regexStruct = CType(regExp.regexStructList(i), RegexStruct)
            If language = "cs" AndAlso regexStruct.styleObject = "Var" Then
                codeString = regexStruct.regex.Replace(codeString,
                                                       "<span class='Var'>$1$2$3</span>")
            Else
                codeString = regexStruct.regex.Replace(codeString,
                                                       "<span class='" _
                                                       & regexStruct.styleObject _
                                                       & "'>$1</span>")
            End If
            i += 1
        End While
        '#End Region


        '#Region "Replace the string back to original value."
        If language <> "css" Then
            For i = 0 To styleString.Count - 1
                codeString = codeString.Replace("__StringVariables" _
                                                & i & "__", styleString(i).ToString())
            Next
        End If
        If language = "cs" Then
            For i = 0 To xmlnote.Count - 1
                codeString = codeString.Replace("__XMLComments" _
                                                & i & "__", xmlnote(i).ToString())
            Next
        End If
        For i = 0 To note.Count - 1
            codeString = codeString.Replace("__Comments" _
                                            & i & "__", note(i).ToString())
        Next
        If language <> "css" Then
            ' Replace the string which contains comments.
            If codeString.IndexOf("__XMLComments") <> -1 Then
                For i = 0 To styleString.Count - 1
                    For j = 0 To xmlnote.Count - 1
                        codeString = codeString.Replace("__XMLComments" _
                                                        & j _
                                                        & "__",
                                                        ClearHTMLTag(xmlnote(j).ToString()))
                    Next
                Next
            End If
            If codeString.IndexOf("__Comments") <> -1 Then
                For i = 0 To styleString.Count - 1
                    For j = 0 To note.Count - 1
                        codeString = codeString.Replace("__Comments" _
                                                        & j _
                                                        & "__",
                                                        ClearHTMLTag(note(j).ToString()))
                    Next
                Next
            End If
            If codeString.IndexOf("__StringVariables") <> -1 Then
                For i = 0 To styleString.Count - 1
                    codeString = codeString.Replace("__StringVariables" _
                                                    & i _
                                                    & "__",
                                                    ClearHTMLTag(styleString(i).ToString()))
                Next
            End If

            If codeString.IndexOf("__XMLComments") <> -1 Then
                For i = 0 To xmlnote.Count - 1
                    codeString = codeString.Replace("__XMLComments" _
                                                    & i _
                                                    & "__",
                                                    xmlnote(i).ToString())
                Next
            End If
            codeString = codeString.
                Replace("__CharactersQuotes__", "\""").
                Replace("__CharactersSingleQuote__", "\'")
        End If
        '#End Region
        Return codeString
    End Function


    ''' <summary>
    ''' Clear the HTML tags.
    ''' </summary>
    Public Shared Function ClearHTMLTag(ByVal htmlString As String) As String
        ' Clear the script tags.
        htmlString = Regex.Replace(htmlString,
                                   "<script[^>]*?>.*?</script>", "", RegexOptions.IgnoreCase)

        ' Clear the HTML tags.
        htmlString = Regex.Replace(htmlString, "<(.[^>]*)>", "", RegexOptions.IgnoreCase)
        htmlString = Regex.Replace(htmlString, "([\r\n])[\s]+", "", RegexOptions.IgnoreCase)
        htmlString = Regex.Replace(htmlString, "-->", "", RegexOptions.IgnoreCase)
        htmlString = Regex.Replace(htmlString, "<!--.*", "", RegexOptions.IgnoreCase)

        htmlString = Regex.Replace(htmlString, "&(quot|#34);", """", RegexOptions.IgnoreCase)
        htmlString = Regex.Replace(htmlString, "&(amp|#38);", "&", RegexOptions.IgnoreCase)
        htmlString = Regex.Replace(htmlString, "&(lt|#60);", "<", RegexOptions.IgnoreCase)
        htmlString = Regex.Replace(htmlString, "&(gt|#62);", ">", RegexOptions.IgnoreCase)
        htmlString = Regex.Replace(htmlString, "&(nbsp|#160);", " ", RegexOptions.IgnoreCase)
        htmlString = Regex.Replace(htmlString, "&(iexcl|#161);", "¡", RegexOptions.IgnoreCase)
        htmlString = Regex.Replace(htmlString, "&(cent|#162);", "¢", RegexOptions.IgnoreCase)
        htmlString = Regex.Replace(htmlString, "&(pound|#163);", "£", RegexOptions.IgnoreCase)
        htmlString = Regex.Replace(htmlString, "&(copy|#169);", "©", RegexOptions.IgnoreCase)
        htmlString = Regex.Replace(htmlString, "&#(\d+);", "", RegexOptions.IgnoreCase)

        htmlString.Replace("<", "")
        htmlString.Replace(">", "")
        htmlString.Replace(vbCr & vbLf, "")
        htmlString = HttpContext.Current.Server.HtmlEncode(htmlString).Trim()
        Return htmlString
    End Function

    ''' <summary>
    ''' Replace the properties of the THML tags.
    ''' </summary>
    ''' <param name="m">Matching collection</param>
    ''' <returns>Replaced code</returns>
    Private Shared Function HTMLEval(ByVal m As Match) As String
        Dim tmp As String = m.Groups(1).Value
        If tmp.StartsWith("/") Then
            Return "<span class='kw'>&lt;/<span class='str'>" _
                & tmp.Substring(1) _
                & "</span>&gt;</span>"
        ElseIf New Regex("^([_0-9a-z]+)\s*\/$",
                         RegexOptions.IgnoreCase).IsMatch(tmp) Then
            Return "<span class='kw'>&lt;<span class='str'>" _
                & tmp.Substring(0, tmp.Length - 1) _
                & "</span>&gt;</span>"
        ElseIf tmp.ToLower().StartsWith("!doctype") Then
            tmp = "<span class='kw'>" _
                & m.Groups(0).Value.Substring(1) _
                & "</span>"
            tmp = New Regex("\b(html|public)\b",
                            RegexOptions.IgnoreCase).
                        Replace(tmp, "<span class='sqlstr'>$1</span>")
            Return "<span class='kw'>&lt;!" _
                & tmp _
                & "&gt;</span>"
        Else
            Dim regex As New Regex("([a-z_][a-z_0-9\.\-]*)\s*=\s*""([^""]*)""",
                                   RegexOptions.IgnoreCase)
            tmp = regex.Replace(tmp,
                                "<span class=""sqlstr"">$1</span><span class=""kw"">=""$2""</span>")
            regex = New Regex("([a-z_][a-z_0-9\.\-]*)\s*=\s*'([^']*)'",
                              RegexOptions.IgnoreCase)
            tmp = regex.Replace(tmp,
                                "<span class=""sqlstr"">$1</span><span class=""kw"">='$2'</span>")
            regex = New Regex("([a-z_][a-z_0-9\-]*)\s*=\s*(?!['""])(\w+)",
                              RegexOptions.IgnoreCase)
            tmp = regex.Replace(tmp,
                                "<span class=""sqlstr"">$1</span><span class=""kw"">=$2</span>")

            regex = New Regex("^([a-z_0-9\-]+)",
                              RegexOptions.IgnoreCase)
            tmp = regex.Replace(tmp, "<span class='str'>$1</span>")
            If tmp.StartsWith("%@") Then
                Return "<span class='str'><span class='declare'>&lt;%</span>" _
                    & "<span class='kw'>@</span>" _
                    & tmp.Trim(New Char() {"%"c, "@"c}) _
                    & "<span class='declare'>%&gt;</span></span>"
            End If
            Return "<span class='kw'>&lt;" & tmp & "&gt;</span>"
        End If
    End Function
    ''' <summary>
    ''' Replace the \r, \n to <br/>.
    ''' </summary>
    ''' <param name="codeString"></param>
    ''' <returns></returns>
    Public Shared Function Encode(ByVal codeString As String) As String
        codeString = codeString.Replace(vbCr, "").Replace(vbLf, "<br/>")
        Return Regex.Replace(codeString, "(?<!<span)( +)(?!class)",
                             AddressOf GetSpace, RegexOptions.Compiled)
    End Function

    ''' <summary>
    ''' Replace the space.
    ''' </summary>
    ''' <param name="m"></param>
    ''' <returns></returns>
    Public Shared Function GetSpace(ByVal m As Match) As String
        Return m.Groups(1).Value.Replace(" ", "&nbsp;")
    End Function
End Class

