'/************************************* Module Header **************************************\
'* Module Name:	ScriptControl.vb
'* Project:		VBCodeDOM
'* Copyright (c) Microsoft Corporation.
'* 
'* The VBCodeDOM project demonstrates how to use the .NET CodeDOM mechanism to enable
'* dynamic souce code generation and compilation at runtime.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* History:
'* * 9/12/2009 1:00 PM Jie Wang Created
'\******************************************************************************************/

#Region "Using directives"
Imports System
Imports System.IO
Imports System.Text
Imports System.CodeDom
Imports System.CodeDom.Compiler
Imports System.Reflection
Imports System.Collections.Generic
Imports Microsoft.CSharp
Imports Microsoft.VisualBasic
Imports Microsoft.JScript
#End Region


Public NotInheritable Class ScriptControl
    Inherits MarshalByRefObject

    ''' <summary>
    ''' Languages supported by <see cref="ScriptControl" />.
    ''' </summary>
    Public Enum Language
        CSharp
        VisualBasic
        JScript
    End Enum

    Private Const ScriptAppDomainFriendlyName As String = "ScriptDomain"
    Private Const ContainerNamespace As String = "ScriptContainerNamespace"
    Private Const ContainerClassName As String = "ScriptContainer"
    Private Const ScriptMethodName As String = "RunScript"
    Private Const LanguageVersion As String = "v3.5"

#Region ".ctor"

    ''' <summary>
    ''' Creates an instance of ScriptControl.
    ''' </summary>
    Public Sub New()
        Me.TargetLanguage = Language.VisualBasic
        Me.RunInSeparateDomain = True
        Me.m_assemblyReferences = New List(Of String)
        Me.m_namespaceImports = New List(Of String)
    End Sub

    ''' <summary>
    ''' Creates an instance of ScriptControl.
    ''' </summary>
    ''' <param name="script">Script text.</param>
    ''' <param name="language">Script language.</param>
    Public Sub New(ByVal script As String, ByVal language As Language)
        Me.New()
        Me.Script = script
        Me.TargetLanguage = language
    End Sub

#End Region

#Region "Private members"

    ''' <summary>
    ''' List of namespace to be imported
    ''' </summary>
    Private m_namespaceImports As List(Of String)

    ''' <summary>
    ''' List of assemblies to be referenced
    ''' </summary>
    Private m_assemblyReferences As List(Of String)

    Private m_runInSeparateDomain As Boolean

    Private m_script As String

    Private m_lang As Language

#End Region

#Region "Public interface"

    ''' <summary>
    ''' Runs the script stored in the <see cref="Script"/> property.
    ''' </summary>
    ''' <returns>The object returned by the script. If the script doesn't return anything, 
    ''' an instance of System.Object is returned by default.</returns>
    Public Function Run() As Object
        Dim r As Object

        If Me.RunInSeparateDomain Then  ' We will run the script in a new AppDomain...
            ' Create a new domain for running the script.
            Dim scriptDomain As AppDomain = AppDomain.CreateDomain(ScriptAppDomainFriendlyName)
            ' Create an instance of ScriptControl inside the new AppDomain.
            Dim sc As ScriptControl = CType( _
                scriptDomain.CreateInstanceAndUnwrap(Assembly.GetExecutingAssembly().FullName, _
                Me.GetType().FullName), ScriptControl)

            ' Set the property values of the ScriptControl in the new AppDomain.
            ' Making the values identical with current instance.
            sc.TargetLanguage = Me.TargetLanguage
            sc.Script = Me.Script

            ' Add assembly references.
            For i As Integer = 0 To Me.AssemblyReferences.Count - 1
                sc.AddAssemblyReference(Me.AssemblyReferences(i))
            Next

            ' Add namespace imports.
            For i As Integer = 0 To Me.CodeNamespaceImports.Count - 1
                sc.AddNamespaceImport(Me.CodeNamespaceImports(i))
            Next

            ' Except this one, other wise the call will end up as an infinite loop.
            sc.RunInSeparateDomain = False
            ' Call the Run method in the remote AppDomain and get the result.
            r = sc.Run()
            ' We're done with the new AppDomain, unload it.
            AppDomain.Unload(scriptDomain)
        Else
            ' We will run the script in current AppDomain, call RunInternal directly.
            r = Me.RunInternal()
        End If

        Return r
    End Function

    ''' <summary>
    ''' Adds a namespace import.
    ''' </summary>
    Public Sub AddNamespaceImport(ByVal ns As String)
        If Me.CodeNamespaceImports.IndexOf(ns) < 0 Then
            Me.CodeNamespaceImports.Add(ns)
        End If
    End Sub

    ''' <summary>
    ''' Removes a namespace import.
    ''' </summary>
    Public Sub RemoveNamespaceImport(ByVal ns As String)
        Me.CodeNamespaceImports.Remove(ns)
    End Sub

    ''' <summary>
    ''' Adds an assembly reference.
    ''' </summary>
    Public Sub AddAssemblyReference(ByVal asm As String)
        If Me.AssemblyReferences.IndexOf(asm) < 0 Then
            Me.AssemblyReferences.Add(asm)
        End If
    End Sub

    ''' <summary>
    ''' Remove an assembly reference.
    ''' </summary>
    Public Sub RemoveAssemblyReference(ByVal asm As String)
        Me.AssemblyReferences.Remove(asm)
    End Sub

    ''' <summary>
    ''' Gets or sets the language
    ''' </summary>
    Public Property TargetLanguage() As Language
        Get
            Return Me.m_lang
        End Get
        Set(ByVal value As Language)
            If Me.m_lang <> value Then
                Me.m_lang = value
            End If
        End Set
    End Property

    ''' <summary>
    ''' Gets or sets the script.
    ''' </summary>
    Public Property Script() As String
        Get
            Return Me.m_script
        End Get
        Set(ByVal value As String)
            If Not Object.Equals(Me.m_script, value) Then
                Me.m_script = value
            End If
        End Set
    End Property

    ''' <summary>
    ''' Gets or sets whether the script should be run in a separate <see cref="AppDomain"/>.
    ''' </summary>
    Public Property RunInSeparateDomain() As Boolean
        Get
            Return Me.m_runInSeparateDomain
        End Get
        Set(ByVal value As Boolean)
            If Me.m_runInSeparateDomain <> value Then
                Me.m_runInSeparateDomain = value
            End If
        End Set
    End Property

    ''' <summary>
    ''' Gets a list of namespaces to be imported.
    ''' </summary>
    Public ReadOnly Property CodeNamespaceImports() As List(Of String)
        Get
            Return Me.m_namespaceImports
        End Get
    End Property

    ''' <summary>
    ''' Gets a list of assemblies to be referenced.
    ''' </summary>
    Public ReadOnly Property AssemblyReferences() As List(Of String)
        Get
            Return Me.m_assemblyReferences
        End Get
    End Property

#End Region

#Region "Internal support methods"

    ''' <summary>
    ''' The inner implementation of <see cref="Run"/>.
    ''' </summary>
    Private Function RunInternal() As Object
        ' Build the script into a class and compile it into an in-memory assembly.
        Dim r As CompilerResults = CompileCode(BuildClass(Me.Script))
        Dim asm As Assembly = r.CompiledAssembly

        ' Now extract the method containing the script using reflection...
        Dim modules As [Module]() = asm.GetModules(False)
        Dim types As Type() = modules(0).GetTypes()

        For i As Integer = 0 To types.Length - 1
            Dim t As Type = types(i)
            If t.IsClass AndAlso t.Name = ContainerClassName Then   ' The class we're looking for.
                Dim mis As MethodInfo() = t.GetMethods()
                For j As Integer = 0 To mis.Length - 1
                    Dim mi As MethodInfo = mis(j)
                    If mi.Name = ScriptMethodName Then      ' The method we're looking for.
                        Return mi.Invoke(Nothing, Nothing)  ' Call the method and return the result.
                    End If
                Next
            End If
        Next

        ' Should never be here.
        Throw New ApplicationException("Script method not found.")
    End Function

    ''' <summary>
    ''' Build the Class source code using CodeDOM.
    ''' </summary>
    ''' <param name="snippet">Script text to be built into the class.</param>
    ''' <returns>CodeDOM generated source.</returns>
    Private Function BuildClass(ByVal snippet As String) As String
        Dim ns As New CodeNamespace(ContainerNamespace)

        ' Import namespaces
        For i As Integer = 0 To Me.CodeNamespaceImports.Count - 1
            ns.Imports.Add(New CodeNamespaceImport(Me.CodeNamespaceImports(i)))
        Next

        ' Create the class declaration.
        Dim cls As New CodeTypeDeclaration
        cls.IsClass = True
        cls.Name = ContainerClassName
        cls.Attributes = MemberAttributes.Public

        ' Create the method
        Dim method As New CodeMemberMethod
        method.Name = ScriptMethodName
        method.ReturnType = New CodeTypeReference(GetType(Object))   ' It will return an object.
        method.Attributes = MemberAttributes.Static Or MemberAttributes.Public
        method.Statements.Add(New CodeSnippetExpression(snippet))   ' Add script code into the method.
        ' Since we don't know if the script will return something or not, we will add this 
        ' return statement to the end of the script code. This will not affect the script 
        ' if the script does return something because in this case our return statement 
        ' will never be executed. However, if the script doesn't include a return to the end 
        ' of its code path, our return statement will return an instance of System.Object by 
        ' default. Otherwise, a compile error will occur: not all code paths return a value.
        method.Statements.Add(New CodeMethodReturnStatement(New CodeObjectCreateExpression(GetType(Object), New CodeExpression() {})))

        cls.Members.Add(method) ' Add method to the class.
        ns.Types.Add(cls)       ' Add class to the namespace.

        Dim sb As New StringBuilder
        Dim sw As StringWriter = Nothing
        Dim provider As CodeDomProvider = Nothing

        Try
            sw = New StringWriter(sb)
            provider = GetProvider()
            Dim generator As ICodeGenerator = provider.CreateGenerator(sw)
            ' Generate the code and write to the stream.
            generator.GenerateCodeFromNamespace(ns, sw, New CodeGeneratorOptions())
            sw.Flush()
            sw.Close()
        Finally
            If provider IsNot Nothing Then provider.Dispose()
            If sw IsNot Nothing Then sw.Dispose()
        End Try

        Return sb.ToString()
    End Function

    ''' <summary>
    ''' Returns an instance of CodeDomProvider according to the target language.
    ''' </summary>
    Private Function GetProvider() As CodeDomProvider
        Dim cpOptions As New Dictionary(Of String, String)

        Select Case Me.TargetLanguage
            Case Language.CSharp
                cpOptions.Add("CompilerVersion", LanguageVersion)
                Return New CSharpCodeProvider(cpOptions)

            Case Language.VisualBasic
                cpOptions.Add("CompilerVersion", LanguageVersion)
                Return New VBCodeProvider(cpOptions)

            Case Language.JScript
                Return New JScriptCodeProvider

            Case Else
                Throw New NotSupportedException(String.Format("Target language {0} is not supported.", Me.TargetLanguage))
        End Select
    End Function

    ''' <summary>
    ''' Returns an instance of <see cref="CompilerParameters"/> containing the compiler parameters.
    ''' </summary>
    Private Function CreateCompilerParameters() As CompilerParameters
        Dim cp As New CompilerParameters
        cp.CompilerOptions = "/target:library"
        cp.IncludeDebugInformation = True
        cp.GenerateExecutable = False
        cp.GenerateInMemory = True
        For i As Integer = 0 To Me.AssemblyReferences.Count - 1
            cp.ReferencedAssemblies.Add(Me.AssemblyReferences(i))
        Next
        Return cp
    End Function

    ''' <summary>
    ''' Compiles the source code into assembly.
    ''' </summary>
    ''' <param name="source">Full source generated by <see cref="BuildClass"/> method.</param>
    Private Function CompileCode(ByVal source As String) As CompilerResults
        Dim provider As CodeDomProvider = GetProvider()
        Dim cp As CompilerParameters = CreateCompilerParameters()
        Dim cr As CompilerResults = provider.CompileAssemblyFromSource(cp, source)

        If cr.Errors.Count > 0 Then
            Dim sb As New StringBuilder

            For Each cplErr As CompilerError In cr.Errors
                sb.AppendLine("Compile Error: " + cplErr.ErrorText)
                sb.AppendLine()
            Next

            Throw New ApplicationException(sb.ToString())
        End If

        Return cr
    End Function

#End Region

End Class
