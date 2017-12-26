'-----------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'-----------------------------------------------------------------------
Option Explicit On 
Option Strict On

'=====================================================================
'  File:      FindType.vb
'  Summary:   Command Line utility to help find and display information
'             on .NET types.
'  Classes:   FindType
'             Indented Writer
'  Origin:    .NET Samples Team
'---------------------------------------------------------------------

Imports System
Imports System.Reflection
Imports System.IO
Imports System.Collections
Imports System.Text
Imports System.Runtime.InteropServices
Imports Microsoft.Win32
Imports Microsoft.VisualBasic
Imports System.Globalization

Namespace Microsoft.Samples
    Public NotInheritable Class App
        Private Sub New()
        End Sub

        Public Shared Sub Main(ByVal args() As String)
            Dim ft As New FindType()
            SetOptions(args, ft)
            ft.Search()
        End Sub 'Main


        ' Prints out usage information to the user
        Private Shared Sub PrintUsage()
            Console.WriteLine()

            Console.WriteLine("FindType [SystemOptions] [MatchOptions] [ShowOptions] [MiscOptions] Name")
            Console.WriteLine()
            Console.WriteLine("  where SystemOptions")
            Console.WriteLine("    d:[Directory] - Additional directory to search")

            Console.WriteLine("  where MatchOptions")
            Console.WriteLine("    x - Name = Exact Type Name (including namespace)")
            Console.WriteLine("    n - Name = Namespace ")
            Console.WriteLine("    w - Match the name anywhere in the namespace")

            Console.WriteLine("  where ShowOptions")
            Console.WriteLine("    i - Show Interfaces")
            Console.WriteLine("    f - Show Fields")
            Console.WriteLine("    p - Show Properties")
            Console.WriteLine("    e - Show Events")
            Console.WriteLine("    m - Show Methods")
            Console.WriteLine("    a - Show All Info")
            Console.WriteLine("    l - Show Module Information")

            Console.WriteLine("  where MiscOptions")
            Console.WriteLine("    v = Verbose")
            Console.WriteLine("    r = For every type find display base type information")
            Console.WriteLine("    ? = Prints Usage information")

            Console.WriteLine()

            Console.WriteLine("Examples")
            Console.WriteLine()

            Console.WriteLine(" FindType String")
            Console.WriteLine("   Finds all types that have 'String' as part of their type name" & ControlChars.CrLf)
            Console.WriteLine(" FindType -r String")
            Console.WriteLine("   Finds all types that have 'String' as part of their name")
            Console.WriteLine("   and prints out all base classes" & ControlChars.CrLf)
            Console.WriteLine(" FindType -n System.Reflection")
            Console.WriteLine("   Displays all types in the 'System.Reflection' namespace" & ControlChars.CrLf)
            Console.WriteLine(" FindType -xipm System.String")
            Console.WriteLine("   Displays the interfaces, properties and methods on the 'System.String' type" & ControlChars.CrLf)
            Console.WriteLine(" FindType -d:C:\ -d:""C:\Program Files\Microsoft.NET\FrameworkSDK\Lib"" String")
            Console.WriteLine("   Searches DLLs C:\ and C:\Program Files\Microsoft.NET\FrameworkSDK\Lib")
            Console.WriteLine("   as well as in the current directory " & ControlChars.CrLf)

            Console.WriteLine()
        End Sub 'PrintUsage


        ' Processes all of the options specified in the arguments   
        Private Shared Sub SetOptions(ByVal args() As String, ByVal ft As FindType)
            If args.Length = 0 Then
                PrintUsage()
                Return
            End If

            ft.VerbosePrint = False

            Dim i As Integer
            For i = 0 To args.Length - 1
                Dim curArg As String = args(i)

                Dim backslash As String
                backslash = "\"

                If curArg.ToCharArray()(0) = "-"c Or curArg.ToCharArray()(0) = "/"c Then
                    If Char.ToUpper(curArg.ToCharArray()(1), CultureInfo.InvariantCulture) = "D"c Then
                        If curArg.ToCharArray().Length > 2 And curArg.ToCharArray()(2) = ":"c Then
                            Dim dir As String = curArg.Substring(3).TrimEnd(backslash.ToCharArray()).ToUpper(CultureInfo.InvariantCulture)

                            If dir.Length > 0 Then
                                ft.DirAdd(dir)
                            Else
                                Console.WriteLine("Directory not specified")
                            End If
                        Else
                            Console.WriteLine("Directory not specified")
                        End If
                    Else
                        Dim j As Integer
                        For j = 1 To curArg.ToCharArray().Length - 1
                            Select Case Char.ToUpper(curArg.ToCharArray()(j), CultureInfo.InvariantCulture)
                                Case "X"c
                                    ft.ExactMatchOnly = True
                                Case "R"c
                                    ft.RecurseTypes = True
                                Case "V"c
                                    ft.VerbosePrint = True
                                Case "N"c
                                    ft.MatchOnlyNamespace = True
                                Case "W"c
                                    ft.WideSearch = True
                                Case "I"c
                                    ft.ShowInterfaces = True
                                Case "M"c
                                    ft.ShowMethods = True
                                Case "F"c
                                    ft.ShowFields = True
                                Case "P"c
                                    ft.ShowProperties = True
                                Case "E"c
                                    ft.ShowEvents = True
                                Case "L"c
                                    ft.ShowModuleInfo = True
                                Case "A"c
                                    ft.ShowAll()
                                Case "?"c
                                    PrintUsage()
                                Case "D"c
                                    Console.WriteLine("Directory not specified")
                                Case Else
                                    Console.WriteLine("Invald Option[{0}]", curArg.ToCharArray()(j))
                            End Select
                        Next j
                    End If
                Else
                    ft.ClassAdd(curArg)
                End If
            Next i
        End Sub 'SetOptions
    End Class 'App


    ' Utility class that holds methods that can be used to find and 
    ' display information about types. Use this utility to:
    '          
    ' (1) Find a specific type
    ' (2) Optionally display information about the type including 
    '     interfaces, properties, events and methods
    '          
    ' This utility dumps information directly contained in the type 
    ' specified. In order to get a complete dump of a type and all 
    ' base type information you must use the "recurse" mode of the 
    ' utility.
    Class FindType

        '   Searches based on settings set in the instance
        Public Sub Search()
            If myClassList.Count <> 0 Then
                Dim i As Integer
                For i = 0 To myClassList.Count - 1
                    Search(CStr(myClassList(i)))
                Next i
            End If
        End Sub 'Search


        ' Searchs for the passed string in the type names of all
        ' The .NET Framework assemblies and all DLLs in the specified paths.   
        Public Sub Search(ByVal theSearchString As String)
            '
            ' Search the .NET Framework Directory
            '  
            Dim testAssy As System.Reflection.Assembly = GetType(object).Assembly
            '        Dim key As RegistryKey = Registry.LocalMachine.OpenSubKey("SOFTWARE\Microsoft\.NETFramework")
            Dim dirFrameworks As String = Path.GetDirectoryName(testAssy.Location)
            '        Dim version As String = CType(key.GetValue("Version"), String)
            '        dirFrameworks += version
            myVerboseWriter.WriteLine("Searching System Libraries")

            Dim l As New ArrayList()
            BuildDLLFileList(dirFrameworks, l)
            Dim j As Integer
            For j = 0 To l.Count - 1
                Search(theSearchString, CType(l(j), String))
            Next j

            '
            ' Search the current directory
            '
            myVerboseWriter.WriteLine("Searching the current directory...")
            l = New ArrayList()
            BuildDLLFileList(".", l)
            For j = 0 To l.Count - 1
                Search(theSearchString, CType(l(j), String))
            Next j

            '
            ' Search the specified directories
            '
            Dim dir As Object() = DirList.ToArray()
            Dim i As Integer
            For i = 0 To dir.Length - 1
                myVerboseWriter.WriteLine("Searching directory  {0}...", dir(i))

                l = New ArrayList()
                BuildDLLFileList(CType(dir(i), String), l)

                For j = 0 To l.Count - 1
                    Search(theSearchString, CType(l(j), String))
                Next j
            Next i
        End Sub 'Search


        ' Loads the specified module and searchs through all 
        ' the types defined in it for the type name specified.   
        Public Sub Search(ByVal theSearchString As String, ByVal theModule As String)
            Try
                '
                ' Load the module - expect to fail if it is not a .NET Framework 
                ' module.
                '
                Dim a As [Assembly] = [Assembly].LoadFrom(theModule)
                Dim m As [Module]() = a.GetModules()

                ' We are case insensitive
                theSearchString = theSearchString.ToUpper(CultureInfo.InvariantCulture)

                Dim j As Integer
                For j = 0 To m.Length - 1

                    myVerboseWriter.WriteLine("Searching Module {0}", theModule)

                    If Not (m Is Nothing) Then
                        '
                        ' Get all the types from the module
                        '
                        Dim types As Type() = m(j).GetTypes()

                        Dim i As Integer
                        For i = 0 To types.Length - 1
                            Dim curType As Type = types(i)

                            ' Case insensitive
                            Dim name As String = curType.FullName.ToUpper(CultureInfo.InvariantCulture)

                            '
                            ' How has the user indicated they want to search. Note that 
                            ' even if the user has specified several ways to search we only 
                            ' respect one of them (from most specific to most general)
                            '                  
                            If ExactMatchOnly Then
                                If name = theSearchString Then
                                    DumpType(curType)
                                End If
                            Else
                                If MatchOnlyNamespace Then
                                    If Not (curType.Namespace Is Nothing) Then
                                        If curType.Namespace.ToUpper(CultureInfo.InvariantCulture) = theSearchString Then
                                            DumpType(curType)
                                        End If
                                    End If
                                Else
                                    If WideSearch Then
                                        If curType.Namespace.ToUpper(CultureInfo.InvariantCulture).IndexOf(theSearchString) <> -1 Then
                                            DumpType(curType)
                                        End If
                                    Else
                                        '
                                        ' User has not specified a search criteria - so we have some 
                                        ' defaults:
                                        '   (1) If the search string supplied matches a complete type
                                        '       name then assume they want to get all information about 
                                        '       the type. If they have actually set display options then 
                                        '       respect them.
                                        '   (2) If we are going to dump all information save the show 
                                        '       properties so they can be reset after this type (in case the 
                                        '       user is searching for multiple types).
                                        '
                                        If name = theSearchString Then
                                            Dim oldOptions As Integer = showOptions

                                            If showOptions = 0 Then
                                                ShowAll()
                                            End If

                                            DumpType(curType)

                                            showOptions = oldOptions
                                        Else
                                            If name.IndexOf(theSearchString) <> -1 Then
                                                DumpType(curType)
                                            End If
                                        End If
                                    End If
                                End If
                            End If
                        Next i
                    End If
                Next j
            Catch rcle As ReflectionTypeLoadException

                Dim loadedTypes As Type() = rcle.Types
                Dim exceptions As Exception() = rcle.LoaderExceptions

                Dim exceptionCount As Integer = 0

                Dim i As Integer
                For i = 0 To loadedTypes.Length - 1
                    If loadedTypes(i) Is Nothing Then
                        ' The following line would output the TypeLoadException.
                        ' myWriter.WriteLine("Unable to load a type because {0}", exceptions(exceptionCount))
                        exceptionCount += 1
                    End If
                Next i
            Catch fnfe As FileNotFoundException
                myVerboseWriter.WriteLine(fnfe.Message)
            Catch
            End Try
        End Sub 'Search 

        ' Do an exact match of passed types - the name passed must 
        ' be the same as the fully qualified type name.   
        Public Property ExactMatchOnly() As Boolean
            Get
                Return exactMatchOnlyField
            End Get
            Set(ByVal value As Boolean)
                exactMatchOnlyField = value
            End Set
        End Property
        ' Load all base types and display the same information 
        ' (methods, properties etc) for the base type.   

        Public Property RecurseTypes() As Boolean
            Get
                Return recurseTypesField
            End Get
            Set(ByVal value As Boolean)
                recurseTypesField = value
            End Set
        End Property
        ' Match only against the namespace component of a type
        ' Use this to display all members of a namespace.   

        Public Property MatchOnlyNamespace() As Boolean
            Get
                Return matchOnlyNamespaceField
            End Get
            Set(ByVal value As Boolean)
                matchOnlyNamespaceField = value
            End Set
        End Property
        ' Match anywhere in the name (namespace and type name)

        Public Property WideSearch() As Boolean
            Get
                Return wideSearchField
            End Get
            Set(ByVal value As Boolean)
                wideSearchField = value
            End Set
        End Property
        ' Print verbose information   

        Public Property VerbosePrint() As Boolean
            Get
                Return myVerboseWriter.Print
            End Get
            Set(ByVal value As Boolean)
                myVerboseWriter.Print = value
            End Set
        End Property
        ' Show any interfaces implemented by this type   

        Public Property ShowInterfaces() As Boolean
            Get
                Return (showOptions And SHOW_INTERFACES) <> 0
            End Get
            Set(ByVal value As Boolean)
                If value Then
                    showOptions = showOptions Or SHOW_INTERFACES
                Else
                    showOptions = showOptions And Not SHOW_INTERFACES
                End If
            End Set
        End Property
        ' Show any public fields in the type found   

        Public Property ShowFields() As Boolean
            Get
                Return (showOptions And SHOW_FIELDS) <> 0
            End Get
            Set(ByVal value As Boolean)
                If value Then
                    showOptions = showOptions Or SHOW_FIELDS
                Else
                    showOptions = showOptions And Not SHOW_FIELDS
                End If
            End Set
        End Property
        ' Show any public properties in the type found   

        Public Property ShowProperties() As Boolean
            Get
                Return (showOptions And SHOW_PROPERTIES) <> 0
            End Get
            Set(ByVal value As Boolean)
                If value Then
                    showOptions = showOptions Or SHOW_PROPERTIES
                Else
                    showOptions = showOptions And Not SHOW_PROPERTIES
                End If
            End Set
        End Property
        ' Show any public events in the type found.   

        Public Property ShowEvents() As Boolean
            Get
                Return (showOptions And SHOW_EVENTS) <> 0
            End Get
            Set(ByVal value As Boolean)
                If value Then
                    showOptions = showOptions Or SHOW_EVENTS
                Else
                    showOptions = showOptions And Not SHOW_EVENTS
                End If
            End Set
        End Property
        ' Show any public methods in the type found.   

        Public Property ShowMethods() As Boolean
            Get
                Return (showOptions And SHOW_METHODS) <> 0
            End Get
            Set(ByVal value As Boolean)
                If value Then
                    showOptions = showOptions Or SHOW_METHODS
                Else
                    showOptions = showOptions And Not SHOW_METHODS
                End If
            End Set
        End Property
        ' Show the module information for any type found - this is useful when 
        ' trying to determine what DLL contains what type.   

        Public Property ShowModuleInfo() As Boolean
            Get
                Return (showOptions And SHOW_MODULE_INFO) <> 0
            End Get
            Set(ByVal value As Boolean)
                If value Then
                    showOptions = showOptions Or SHOW_MODULE_INFO
                Else
                    showOptions = showOptions And Not SHOW_MODULE_INFO
                End If
            End Set
        End Property

        ' Sets all display options on.   
        Public Sub ShowAll()
            ShowInterfaces = True
            ShowMethods = True
            ShowFields = True
            ShowProperties = True
            ShowEvents = True
            ShowModuleInfo = True
        End Sub 'ShowAll


        ' Add a directory to the search set
        Public Sub DirAdd(ByVal dir As String)
            DirList.Add(dir)
        End Sub 'DirAdd


        ' Add a class to the search set
        Public Sub ClassAdd(ByVal newClass As String)
            myClassList.Add(newClass)
        End Sub 'ClassAdd


        ' Given a directory this method appends the fullly qualified path name of 
        ' all DLLs in the directory to the list passed in.   
        Private Sub BuildDLLFileList(ByVal dirName As String, ByVal list As ArrayList)
            Try

                If Directory.Exists(dirName) Then
                    Dim e As String() = Directory.GetFiles(dirName, "*.dll")

                    Dim i As Integer
                    For i = 0 To e.Length - 1
                        list.Add(e(i))
                    Next i
                Else
                    myVerboseWriter.WriteLine("Directory [{0}] does not exist!", dirName)
                End If

            Catch Ex As Exception

                '
                ' Trap Exception: System.InvalidOperationException  This occurs when the file is an unmanaged dll
                '
                If Ex.GetType() Is GetType(System.InvalidOperationException) Then
                End If
            End Try
        End Sub 'BuildDLLFileList


        ' A short description of the type.   
        Private Function GetTypeDescription(ByVal aType As Type) As String
            Dim str As String = Nothing

            If aType.IsClass Then
                str = "class"
            End If

            If aType.IsInterface Then
                str = "interface"
            End If

            If aType.IsValueType Then
                str = "struct"
            End If

            If aType.IsArray Then
                str = "array"
            End If

            Return str
        End Function 'GetTypeDescription


        ' Dumps information about the specified type.   
        Private Sub DumpType(ByVal aType As Type)
            Dim baseType As Type = aType.BaseType

            myWriter.WriteLine("{0,-10} {1}", GetTypeDescription(aType), aType)

            If ShowModuleInfo Then
                myWriter.WriteLine("{0,-10} {1}", "Module:", aType.Module.FullyQualifiedName)
            End If

            DumpInterfaces(aType)
            DumpFields(aType)
            DumpProperties(aType)
            DumpEvents(aType)
            DumpMethods(aType)

            If RecurseTypes Then
                myWriter.WriteLine()
            End If

            '
            ' If recursing then pop the indent on the writers so we 
            ' can easily see the nesting for the base type information.
            '                                       
            If RecurseTypes And Not (baseType Is Nothing) Then
                myWriter.PushIndent()
                myVerboseWriter.PushIndent()

                DumpType(baseType)

                myWriter.PopIndent()
                myVerboseWriter.PopIndent()
            End If
        End Sub 'DumpType


        ' Dumps the interfaces implemented by the specified type.   
        Private Sub DumpInterfaces(ByVal aType As Type)
            If Not ShowInterfaces Then
                Return
            End If
            Dim info As Type() = aType.GetInterfaces()

            If info.Length <> 0 Then
                myWriter.WriteLine("{0} {1}", "# Interfaces:", info.Length)

                Dim i As Integer
                For i = 0 To info.Length - 1

                    myWriter.PushIndent()
                    myWriter.WriteLine("interface {0}", info(i).FullName)

                    '
                    ' Only show method information only if requested
                    '
                    If ShowMethods Then
                        myWriter.PushIndent()
                        DumpType(info(i))
                        myWriter.PopIndent()
                    End If
                    myWriter.PopIndent()
                Next i
            End If
        End Sub 'DumpInterfaces


        ' Dumps the public properties directly contained in the specified type   
        Private Sub DumpProperties(ByVal aType As Type)
            If Not ShowProperties Then
                Return
            End If
            Dim pInfo As PropertyInfo() = aType.GetProperties()
            myWriter.WriteLine("Properties")

            Dim found As Boolean = False

            If pInfo.Length <> 0 Then
                Dim curInfo As PropertyInfo = Nothing

                Dim i As Integer
                For i = 0 To pInfo.Length - 1
                    curInfo = pInfo(i)

                    '
                    ' Only display properties declared in this type.
                    '          
                    If curInfo.DeclaringType Is aType Then
                        found = True

                        Dim flags As String = Nothing

                        If curInfo.CanRead And curInfo.CanWrite Then
                            flags = "get; set;"
                        Else
                            If curInfo.CanRead Then
                                flags = "get"
                            Else
                                If curInfo.CanWrite Then
                                    flags = "set"
                                End If
                            End If
                        End If
                        myWriter.WriteLine("  {0,-10} '{1}' ", curInfo, flags)
                    End If
                Next i
            End If

            If Not found Then
                myWriter.WriteLine("  (none)")
            End If
        End Sub 'DumpProperties


        ' Dumps the public events directly contained in the specified type   
        Private Sub DumpEvents(ByVal aType As Type)
            If Not ShowEvents Then
                Return
            End If
            Dim eInfo As EventInfo() = aType.GetEvents()

            myWriter.WriteLine("Events:")
            Dim found As Boolean = False

            If eInfo.Length <> 0 Then
                Dim i As Integer
                For i = 0 To eInfo.Length - 1
                    '
                    ' Only display events declared in this type.
                    '          
                    If eInfo(i).DeclaringType Is aType Then
                        found = True
                        myWriter.WriteLine("  {0}", eInfo(i))
                    End If
                Next i
            End If

            If Not found Then
                myWriter.WriteLine("  (none)")
            End If
        End Sub 'DumpEvents


        ' Dumps the public fields directly contained in the specified type   
        Private Sub DumpFields(ByVal aType As Type)
            If Not ShowFields Then
                Return
            End If
            Dim info As FieldInfo() = aType.GetFields()

            myWriter.WriteLine("Fields:")

            Dim found As Boolean = False

            If info.Length <> 0 Then
                Dim i As Integer
                For i = 0 To info.Length - 1
                    '
                    ' Only display fields declared in this type.
                    '          
                    If info(i).DeclaringType Is aType Then
                        myWriter.WriteLine("  {0}", info(i))
                        found = True
                    End If
                Next i
            End If

            If Not found Then
                myWriter.WriteLine("  (none)")
            End If
        End Sub 'DumpFields


        ' Dumps the public methods directly contained in the specified type. 
        ' Note "special name" methods are not displayed.   
        Private Sub DumpMethods(ByVal aType As Type)
            If Not ShowMethods Then
                Return
            End If
            Dim mInfo As MethodInfo() = aType.GetMethods()

            myWriter.WriteLine("Methods")

            Dim found As Boolean = False

            If mInfo.Length <> 0 Then
                Dim i As Integer
                For i = 0 To mInfo.Length - 1
                    '
                    ' Only display methods declared in this type. Also 
                    ' filter out any methods with special names - these
                    ' cannot be generally called by the user (i.e their 
                    ' functionality is usually exposed in other ways e.g
                    ' property get/set methods are exposed as properties.
                    '          
                    If mInfo(i).DeclaringType Is aType And Not mInfo(i).IsSpecialName Then
                        found = True

                        Dim modifiers As New StringBuilder()

                        If mInfo(i).IsStatic Then
                            modifiers.Append("static ")
                        End If
                        If mInfo(i).IsPublic Then
                            modifiers.Append("public ")
                        End If
                        If mInfo(i).IsFamily Then
                            modifiers.Append("protected ")
                        End If
                        If mInfo(i).IsAssembly Then
                            modifiers.Append("internal ")
                        End If
                        If mInfo(i).IsPrivate Then
                            modifiers.Append("private ")
                        End If
                        myWriter.WriteLine("  {0} {1}", modifiers, mInfo(i))
                    End If
                Next i
            End If

            If Not found Then
                myWriter.WriteLine("  (none)")
            End If
        End Sub 'DumpMethods

        Private SHOW_INTERFACES As Integer = &H1
        Private SHOW_FIELDS As Integer = &H2
        Private SHOW_PROPERTIES As Integer = &H4
        Private SHOW_EVENTS As Integer = &H8
        Private SHOW_METHODS As Integer = &H10
        Private SHOW_MODULE_INFO As Integer = &H20

        Private myVerboseWriter As New IndentedWriter()
        Private myWriter As New IndentedWriter()

        Private exactMatchOnlyField As Boolean = False
        Private recurseTypesField As Boolean = False
        Private matchOnlyNamespaceField As Boolean = False
        Private wideSearchField As Boolean = False

        Private showOptions As Integer = 0

        Private myClassList As New ArrayList()
        Private DirList As New ArrayList()
    End Class 'FindType


    ' Utility class that performs basic text output operations that can be 
    ' indented via "push" and "pop". The text writer used to write 
    ' information out can be changed by setting the appropriate property.
    ' The default text writer is System.Console.Out
    Public Class IndentedWriter

        ' Controls whether printing is performed or not. This can be useful 
        ' for clients that require a "verbose" writer that only displays 
        ' information when the verbose mode is set   

        Public Property Print() As Boolean
            Get
                Return myPrintFlag
            End Get
            Set(ByVal value As Boolean)
                myPrintFlag = value
            End Set
        End Property
        ' Sets the TextWriter to use when writing strings out   

        Public Property TextWriter() As TextWriter
            Get
                Return myTextWriter
            End Get
            Set(ByVal value As TextWriter)
                myTextWriter = value
            End Set
        End Property

        Public Sub PushIndent()
            myIndent += 2
        End Sub 'PushIndent


        Public Sub PopIndent()
            If myIndent > 0 Then
                myIndent -= 2
            End If
        End Sub 'PopIndent


        ' Writes out an empty line   
        Public Sub WriteLine()
            WriteLine("")
        End Sub 'WriteLine


        ' Writes a formatted string   
        Public Sub WriteLine(ByVal format As String, ByVal ParamArray arguments() As Object)
            WriteLine(String.Format(format, arguments))
        End Sub 'WriteLine


        ' Writes out a string (indenting it as required)   
        Public Sub WriteLine(ByVal message As String)
            If myPrintFlag Then
                Dim sb As New StringBuilder()

                Dim i As Integer
                For i = 0 To myIndent - 1
                    sb.Append(" "c)
                Next i
                sb.Append(message)
                myTextWriter.WriteLine(sb.ToString())
            End If
        End Sub 'WriteLine

        Dim myTextWriter As TextWriter = Console.Out
        Dim myPrintFlag As Boolean = True
        Dim myIndent As Integer = 0

    End Class 'IndentedWriter
End Namespace
