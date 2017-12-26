'---------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'  This source code is intended only as a supplement to Microsoft
'  Development Tools and/or on-line documentation.  See these other
'  materials for detailed information regarding Microsoft code samples.
' 
'  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'  PARTICULAR PURPOSE.
'---------------------------------------------------------------------
Imports System
Imports System.CodeDom
Imports System.CodeDom.Compiler
Imports Microsoft.CSharp
Imports Microsoft.VisualBasic
Imports System.IO


' <summary>
' Class to generate the generic code
' </summary>

Public Class GenerateList
    'Static variables
    Private provider As CodeDomProvider
    'Compile unit instance 
    Private compileUnit As CodeCompileUnit
    'Generator options object to set code options
    Private options As CodeGeneratorOptions
    'Target location of generated files
    Private path As String = String.Empty
    'Generic type parameters
    Private keyTypeParameter, itemTypeParameter As CodeTypeParameter
    'Generic type references
    Private keyType, itemType, nodeType As CodeTypeReference


    'Constructor to initialize private fields
    Public Sub New(ByVal prov As CodeDomProvider, ByVal filePath As String)
        provider = prov
        compileUnit = New CodeCompileUnit()
        path = filePath

        'Generator options object to set code options
        options = New CodeGeneratorOptions()

        'Verbatim ordering turned on - code generated in the order added
        options.VerbatimOrder = True

        'String for code indentation
        options.IndentString = "     "
        options.BlankLinesBetweenMembers = False

        keyTypeParameter = New CodeTypeParameter("K")
        'Add constraint IComparable
        '(Of K as ICompaorable)
        keyTypeParameter.Constraints.Add(GetType(IComparable))
        keyType = New CodeTypeReference(keyTypeParameter)

        itemTypeParameter = New CodeTypeParameter("T")
        'Add constructor constraint to itemType
        '(Of T as new)
        itemTypeParameter.HasConstructorConstraint = True
        itemType = New CodeTypeReference(itemTypeParameter)

        ' Build a reference to Node(Of K, T)
        nodeType = New CodeTypeReference("Node", _
                                         New CodeTypeReference(keyTypeParameter), _
                                         New CodeTypeReference(itemTypeParameter))

    End Sub 'New


    'Generate the four files
    Public Sub GenerateCode()
        GenerateNode()
        GenerateLinkedList()
        GenerateListInterface()
        GenerateMain()

    End Sub 'GenerateCode


    'Generate code for class Node 
    Private Sub GenerateNode()
        Dim ns As New CodeNamespace("GenericList")
        compileUnit.Namespaces.Add(ns)

        'Create class Node
        Dim nodeClass As New CodeTypeDeclaration("Node")

        'Add types to generic class 
        nodeClass.TypeParameters.Add(keyTypeParameter)
        nodeClass.TypeParameters.Add(itemTypeParameter)

        'Add class to namespace
        ns.Types.Add(nodeClass)

        'Add field 
        'public Key as K
        Dim keyField As CodeMemberField = GeneratePublicField("Key", keyType)

        'Add start region directive 
        '#Region "Public Fields"
        keyField.StartDirectives.Add(New CodeRegionDirective(CodeRegionMode.Start, "Public Fields"))
        nodeClass.Members.Add(keyField)

        'Add field
        'public item as T
        Dim itemField As CodeMemberField = GeneratePublicField("Item", itemType)
        nodeClass.Members.Add(itemField)

        'Add field 
        'public NextNode as Node(Of K, T)
        Dim nextNodeField As CodeMemberField = GeneratePublicField("NextNode", nodeType)

        'Add end region directive
        '#End Region
        nextNodeField.EndDirectives.Add(New CodeRegionDirective(CodeRegionMode.End, "End"))
        nodeClass.Members.Add(nextNodeField)

        'Add default constructor

        Dim defConstructorColl As New CodeStatementCollection()

        'Key = K.default
        defConstructorColl.Add(New CodeAssignStatement( _
                New CodeFieldReferenceExpression( _
                New CodeThisReferenceExpression(), "Key"), _
                New CodeDefaultValueExpression(keyType)))

        'item = new T()
        defConstructorColl.Add( _
            New CodeAssignStatement( _
            New CodeFieldReferenceExpression( _
            New CodeThisReferenceExpression(), "Item"), _
            New CodeObjectCreateExpression(itemType, New CodeExpression() {})))

        'NextNode = Nothing
        defConstructorColl.Add(New CodeAssignStatement( _
          New CodeFieldReferenceExpression(New CodeThisReferenceExpression(), _
          "NextNode"), New CodePrimitiveExpression(Nothing)))

        Dim defConstructor As CodeConstructor = _
                GeneratePublicConstructor("Node", _
                Nothing, Nothing, defConstructorColl)
        nodeClass.Members.Add(defConstructor)

        'Add parameterized constructor
        Dim parmConstructorColl As New CodeStatementCollection()

        'Key = keyPar
        parmConstructorColl.Add(New CodeAssignStatement(New CodeFieldReferenceExpression(New CodeThisReferenceExpression(), "Key"), New CodeArgumentReferenceExpression("keyPar")))

        'Item = itemPar
        parmConstructorColl.Add(New CodeAssignStatement(New CodeFieldReferenceExpression(New CodeThisReferenceExpression(), "Item"), New CodeArgumentReferenceExpression("itemPar")))

        'NextNode = nextNodePar
        parmConstructorColl.Add(New CodeAssignStatement(New CodeFieldReferenceExpression(New CodeThisReferenceExpression(), "NextNode"), New CodeArgumentReferenceExpression("nextNodePar")))
        Dim parmConstructor As CodeConstructor = GeneratePublicConstructor("Node", New CodeTypeReference() {keyType, itemType, nodeType}, New String() {"keyPar", "itemPar", "nextNodePar"}, parmConstructorColl)
        nodeClass.Members.Add(parmConstructor)
        Dim t As StreamWriter = New StreamWriter(path + "\Node." + provider.FileExtension, False)
        provider.GenerateCodeFromNamespace(ns, t, options)
        t.Close()

    End Sub 'GenerateNode


    'Code to generate a public method
    Private Function GeneratePublicConstructor(ByVal name As String, ByVal parameters() As CodeTypeReference, ByVal parameterNames() As String, ByVal statements As CodeStatementCollection) As CodeConstructor
        Dim constructor As New CodeConstructor()
        constructor.Attributes = constructor.Attributes And Not MemberAttributes.AccessMask Or MemberAttributes.Public
        constructor.Name = name

        Dim i As Integer
        If Not parameters Is Nothing Then
            For i = 0 To parameters.Length - 1
                constructor.Parameters.Add(New CodeParameterDeclarationExpression(parameters(i), parameterNames(i)))
            Next i
        End If
        constructor.Statements.AddRange(statements)
        Return constructor

    End Function 'GeneratePublicConstructor


    'Generate code for class LinkedList
    Private Sub GenerateLinkedList()
        Dim ns As New CodeNamespace("GenericList")
        compileUnit.Namespaces.Add(ns)

        'Create class LinkedList(Of K, T)
        Dim listClassPartOne As New CodeTypeDeclaration("LinkedList")

        'Add type parameters to class
        listClassPartOne.TypeParameters.Add(keyTypeParameter)
        listClassPartOne.TypeParameters.Add(itemTypeParameter)

        'Make class partial 
        listClassPartOne.IsPartial = True

        ns.Types.Add(listClassPartOne)

        'Add field
        'Dim header as Node(Of K, T)
        Dim headerField As New CodeMemberField()

        headerField.Name = "header"
        headerField.Attributes = MemberAttributes.Private
        headerField.Type = nodeType
        listClassPartOne.Members.Add(headerField)

        'Default constructor
        Dim listDefConstructor As New CodeConstructor()

        listDefConstructor.Name = "LinkedList"
        listDefConstructor.Attributes = MemberAttributes.Public

        'header
        Dim nodeRefExp As New CodeFieldReferenceExpression(New CodeThisReferenceExpression(), "header")

        'header = new Node(Of K, T)
        Dim varInitStmt As New CodeAssignStatement(nodeRefExp, New CodeObjectCreateExpression(nodeType, New CodeExpression() {}))

        listDefConstructor.Statements.Add(varInitStmt)
        listClassPartOne.Members.Add(listDefConstructor)

        'Write to file
        Dim t As StreamWriter = New StreamWriter(path + "\List." + provider.FileExtension, False)
        provider.GenerateCodeFromNamespace(ns, t, options)
        t.Close()

    End Sub 'GenerateLinkedList


    'Generate the list interface
    Private Function GetListInterface() As CodeTypeDeclaration
        'Create generic interface
        'interface ILinkedList(Of K, T)
        Dim linkedListInterface As New CodeTypeDeclaration("ILinkedList")

        'Add region start
        linkedListInterface.StartDirectives.Add(New CodeRegionDirective(CodeRegionMode.Start, "Generic interface"))

        'Add end region
        linkedListInterface.EndDirectives.Add(New CodeRegionDirective(CodeRegionMode.End, "Generic interface"))

        'Make interface
        linkedListInterface.IsInterface = True

        'Add types to generic interface
        linkedListInterface.TypeParameters.Add(keyTypeParameter)
        linkedListInterface.TypeParameters.Add(itemTypeParameter)

        'Add method
        'Function Find(key as K) As T
        Dim findInterfaceMethod As CodeMemberMethod = GeneratePublicMethod("Find", itemType, New CodeTypeReference() {keyType}, New String() {"key"}, New CodeStatementCollection())
        linkedListInterface.Members.Add(findInterfaceMethod)

        'Add Method AddHead
        'public Sub AddHead(key As K, item As T)
        Dim addHeadInterfaceMethod As CodeMemberMethod = GeneratePublicMethod("AddHead", Nothing, New CodeTypeReference() {keyType, itemType}, New String() {"key", "item"}, New CodeStatementCollection())
        linkedListInterface.Members.Add(addHeadInterfaceMethod)

        Return linkedListInterface

    End Function 'GetListInterface


    'Generate code for implementation of List interface
    Private Sub GenerateListInterface()
        Dim ns As New CodeNamespace("GenericList")
        compileUnit.Namespaces.Add(ns)

        ns.Types.Add(GetListInterface())

        'Partial class LinkedList
        Dim listClassPartTwo As New CodeTypeDeclaration("LinkedList")

        'Add type parameters to class
        listClassPartTwo.TypeParameters.Add(keyTypeParameter)
        listClassPartTwo.TypeParameters.Add(itemTypeParameter)
        listClassPartTwo.BaseTypes.Add(GetType(Object))
        Dim listInterface As New CodeTypeReference("ILinkedList", _
                                                   New CodeTypeReference(keyTypeParameter), _
                                                   New CodeTypeReference(itemTypeParameter))
        listClassPartTwo.BaseTypes.Add(listInterface)
        listClassPartTwo.IsPartial = True

        'Add Method AddHead
        Dim addHeadColl As New CodeStatementCollection()

        'header.NextNode
        Dim headerNextNodeRefExprn As New CodeFieldReferenceExpression(New CodeFieldReferenceExpression(New CodeThisReferenceExpression(), "header"), "NextNode")


        'new Node(Of K, T)(key, item, header.NextNode)
        Dim newExpr As New CodeObjectCreateExpression(nodeType, New CodeExpression() {New CodeArgumentReferenceExpression("key"), New CodeArgumentReferenceExpression("item"), headerNextNodeRefExprn})

        'Dim newNode As Node(Of K, T) = new Node(Of K, T)(key, item, header.NextNode)
        Dim newNodeDecStmt As New CodeVariableDeclarationStatement(nodeType, "newNode", newExpr)

        'Header.NextNode = newNode
        Dim assignStmt As New CodeAssignStatement(headerNextNodeRefExprn, New CodeVariableReferenceExpression("newNode"))

        addHeadColl.Add(newNodeDecStmt)
        addHeadColl.Add(assignStmt)

        'Generate AddHead
        Dim addHeadMethod As CodeMemberMethod = GeneratePublicMethod("AddHead", Nothing, New CodeTypeReference() {keyType, itemType}, New String() {"key", "item"}, addHeadColl)
        addHeadMethod.ImplementationTypes.Add(listInterface)
        listClassPartTwo.Members.Add(addHeadMethod)

        'Add method Find
        Dim findColl As New CodeStatementCollection()

        'current.Key
        Dim fieldRefExpr As New CodeFieldReferenceExpression(New CodeVariableReferenceExpression("current"), "Key")

        'current.Key.CompareTo(key) 
        Dim compareToMethodInvStmt As New CodeMethodInvokeExpression(fieldRefExpr, "CompareTo", New CodeExpression() {New CodeArgumentReferenceExpression("key")})

        '(current.Key.CompareTo(key) = 0)
        Dim ifCondExpr As New CodeBinaryOperatorExpression(compareToMethodInvStmt, CodeBinaryOperatorType.ValueEquality, New CodePrimitiveExpression(0))

        'return current.Item
        Dim retStmt As New CodeMethodReturnStatement(New CodeFieldReferenceExpression(New CodeVariableReferenceExpression("current"), "Item"))

        'if ((current.Key.CompareTo(key) = 0))
        Dim ifStatement As New CodeConditionStatement(ifCondExpr, New CodeStatement() {retStmt})

        'header
        Dim headerRefExpr As New CodeFieldReferenceExpression(New CodeThisReferenceExpression(), "header")

        'Dim current As Node(Of K, T) = header
        Dim indexDeclrnStmt As New CodeVariableDeclarationStatement(nodeType, "current", headerRefExpr)

        'current <> Nothing
        Dim condExpr As New CodeBinaryOperatorExpression(New CodeVariableReferenceExpression("current"), CodeBinaryOperatorType.IdentityInequality, New CodePrimitiveExpression(Nothing))

        'current = current.NextNode
        Dim incrementStmt As New CodeAssignStatement(New CodeVariableReferenceExpression("current"), New CodeFieldReferenceExpression(New CodeVariableReferenceExpression("current"), "NextNode"))

        'for each of the nodes
        Dim forStatement As New CodeIterationStatement(indexDeclrnStmt, condExpr, incrementStmt, New CodeStatement() {ifStatement})


        findColl.Add(forStatement)

        'return header.Item
        Dim retHdrStmt As New CodeMethodReturnStatement(New CodeFieldReferenceExpression(New CodeFieldReferenceExpression(New CodeThisReferenceExpression(), "header"), "Item"))
        findColl.Add(retHdrStmt)


        'Find method
        Dim findMethod As CodeMemberMethod = GeneratePublicMethod("Find", itemType, New CodeTypeReference() {keyType}, New String() {"key"}, findColl)
        findMethod.ImplementationTypes.Add(listInterface)
        listClassPartTwo.Members.Add(findMethod)

        'Add property
        Dim itemProperty As New CodeMemberProperty()
        itemProperty.Attributes = itemProperty.Attributes And Not MemberAttributes.AccessMask Or MemberAttributes.Public
        itemProperty.Parameters.Add(New CodeParameterDeclarationExpression(keyType, "key"))
        itemProperty.Name = "Item"
        itemProperty.GetStatements.Add(New CodeMethodReturnStatement(New CodeMethodInvokeExpression(New CodeMethodReferenceExpression(New CodeThisReferenceExpression(), "Find"), New CodeArgumentReferenceExpression("key"))))

        itemProperty.Type = itemType
        listClassPartTwo.Members.Add(itemProperty)

        ns.Types.Add(listClassPartTwo)


        Dim tw As StreamWriter = New StreamWriter(path + "\ListInterface." + provider.FileExtension, False)
        provider.GenerateCodeFromNamespace(ns, tw, options)
        tw.Close()

    End Sub 'GenerateListInterface


    'Code to generate Main method
    Private Sub GenerateMain()
        'Generate namespace
        Dim ns As New CodeNamespace("GenericList")

        'Generate MainClass
        Dim mainClass As New CodeTypeDeclaration("GenericList")
        ns.Types.Add(mainClass)

        mainClass.Members.Add(GetGenericMethod())

        'Generate main method

        'Generate the entry point for the project
        Dim mainMethod As New CodeEntryPointMethod()
        mainMethod.Name = "Main"
        mainMethod.Attributes = mainMethod.Attributes And Not MemberAttributes.AccessMask Or MemberAttributes.Public Or MemberAttributes.Static
        mainClass.Members.Add(mainMethod)

        'Dim x as Double = 12.4
        mainMethod.Statements.Add(New CodeVariableDeclarationStatement(New CodeTypeReference(GetType(Double)), "x", New CodePrimitiveExpression(12.4)))

        'Dim y As double = 10
        mainMethod.Statements.Add(New CodeVariableDeclarationStatement(New CodeTypeReference(GetType(Double)), "y", New CodePrimitiveExpression(10)))

        Dim linkedListType As CodeTypeReference

        linkedListType = New CodeTypeReference("LinkedList", _
                                               New CodeTypeReference(GetType(Integer)), _
                                               New CodeTypeReference(GetType(Object)))

        'Dim newList As LinkedList(Of Integer, Object) = new LinkedList(Of Integer, Object)()
        mainMethod.Statements.Add(New CodeVariableDeclarationStatement(linkedListType, "newList", New CodeObjectCreateExpression(linkedListType, New CodeExpression() {})))

        'newList.AddHead(1,x)
        mainMethod.Statements.Add(New CodeMethodInvokeExpression(New CodeVariableReferenceExpression("newList"), "AddHead", New CodeExpression() {New CodePrimitiveExpression(1), New CodeVariableReferenceExpression("x")}))

        'newList.AddHead(1,x)
        mainMethod.Statements.Add(New CodeMethodInvokeExpression(New CodeVariableReferenceExpression("newList"), "AddHead", New CodeExpression() {New CodePrimitiveExpression(2), New CodeVariableReferenceExpression("y")}))

        ' Global reference 
        ' Call CodeTypeReference with CodeTypeReferenceOptions.GlobalReference set
        Dim globalExp As New CodeTypeReferenceExpression(New CodeTypeReference("GenericList.GenericList", CodeTypeReferenceOptions.GlobalReference))

        'isGreater(Of Double)(x,y)
        Dim methodInvokeExp As New CodeMethodInvokeExpression( _
                        New CodeMethodReferenceExpression(globalExp, "isGreater", New CodeTypeReference(GetType(Double))), _
                        New CodeVariableReferenceExpression("x"), _
                        New CodeVariableReferenceExpression("y"))

        ' Dim entry1greater As Boolean = isGreater(Of Double)(x,y)
        mainMethod.Statements.Add(New CodeVariableDeclarationStatement(GetType(Boolean), "entry1greater", methodInvokeExp))


        Dim condStatement As New CodeConditionStatement(New CodeVariableReferenceExpression("entry1greater"), New CodeStatement() {})

        'newList(1)
        Dim newList1Expr As New CodeIndexerExpression(New CodeVariableReferenceExpression("newList"), New CodeExpression() {New CodePrimitiveExpression(1)})

        'newList(2)
        Dim newList2Expr As New CodeIndexerExpression(New CodeVariableReferenceExpression("newList"), New CodeExpression() {New CodePrimitiveExpression(2)})

        '"Item 1 has higher value: {0}", newList(1)
        'Dim newlist1GreaterExpr As New CodeBinaryOperatorExpression(New CodePrimitiveExpression("Item 1 has higher value: {0}"), CodeBinaryOperatorType.Add, newList1Expr)

        '"Item 2 has higher value: {0}", newList(2)
        'Dim newList2GreaterExpr As New CodeBinaryOperatorExpression(CodeBinaryOperatorType.Add, newList2Expr)

        'Console.WriteLine("Item 1 has higher value: "+ newList(1))
        condStatement.TrueStatements.Add(New CodeMethodInvokeExpression(New CodeTypeReferenceExpression(GetType(Console)), "WriteLine", New CodeExpression() {New CodePrimitiveExpression("Item 1 has higher value: {0}"), newList1Expr}))

        'Console.WriteLine("Item 2 has higher value: "+ newList(2))
        condStatement.FalseStatements.Add(New CodeMethodInvokeExpression(New CodeTypeReferenceExpression(GetType(Console)), "WriteLine", New CodeExpression() {New CodePrimitiveExpression("Item 2 has higher value: {0}"), newList2Expr}))

        mainMethod.Statements.Add(condStatement)

        Dim t As StreamWriter = New StreamWriter(path + "\Main." + provider.FileExtension, False)
        provider.GenerateCodeFromNamespace(ns, t, options)
        t.Close()

    End Sub 'GenerateMain


    'Code to generate a public method
    Private Function GeneratePublicMethod(ByVal name As String, ByVal returnType As CodeTypeReference, ByVal parameters() As CodeTypeReference, ByVal parameterNames() As String, ByVal statements As CodeStatementCollection) As CodeMemberMethod
        Dim method As New CodeMemberMethod()
        method.Attributes = method.Attributes And Not MemberAttributes.AccessMask Or MemberAttributes.Public
        method.Name = name

        If Not (returnType Is Nothing) Then
            method.ReturnType = returnType
        End If
        Dim i As Integer
        For i = 0 To parameters.Length - 1
            method.Parameters.Add(New CodeParameterDeclarationExpression(parameters(i), parameterNames(i)))
        Next i
        method.Statements.AddRange(statements)

        Return method

    End Function 'GeneratePublicMethod


    'Code to generate a field
    Private Function GeneratePublicField(ByVal name As String, ByVal fieldType As CodeTypeReference) As CodeMemberField
        Dim field As New CodeMemberField()

        field.Name = name
        field.Attributes = MemberAttributes.Public
        field.Type = fieldType

        Return field

    End Function 'GeneratePublicField


    'Generate a generic method
    Private Function GetGenericMethod() As CodeMemberMethod
        'Generate generic method

        'Parameter type T for generic method 
        Dim methodTypeParameter As New CodeTypeParameter("T")

        'Add constraint for generic type
        methodTypeParameter.Constraints.Add(GetType(IComparable))
        Dim methodType As New CodeTypeReference(methodTypeParameter)

        'return true
        Dim returnTrueStmt As New CodeMethodReturnStatement(New CodePrimitiveExpression(True))

        'return false
        Dim returnFalseStmt As New CodeMethodReturnStatement(New CodePrimitiveExpression(False))

        'item1.CompareTo(item2)
        Dim methodInvokeExpr As New CodeMethodInvokeExpression(New CodeMethodReferenceExpression(New CodeArgumentReferenceExpression("item1"), "CompareTo"), New CodeExpression() {New CodeArgumentReferenceExpression("item2")})

        '(item1.CompareTo(item2) > 0)
        Dim condExpr As New CodeBinaryOperatorExpression(methodInvokeExpr, CodeBinaryOperatorType.GreaterThan, New CodePrimitiveExpression(0))

        'Add to statement collection
        Dim isGreaterColl As New CodeStatementCollection()
        isGreaterColl.Add(New CodeConditionStatement(condExpr, New CodeStatement() {returnTrueStmt}, New CodeStatement() {returnFalseStmt}))

        'Generate method
        Dim isGreaterMethod As CodeMemberMethod = GeneratePublicMethod("isGreater", New CodeTypeReference(GetType(Boolean)), New CodeTypeReference() {methodType, methodType}, New String() {"item1", "item2"}, isGreaterColl)

        'Add type parameter to method to make it generic
        isGreaterMethod.TypeParameters.Add(methodTypeParameter)

        'make method static
        isGreaterMethod.Attributes = isGreaterMethod.Attributes Or MemberAttributes.Static

        Return isGreaterMethod

    End Function 'GetGenericMethod
End Class 'GenerateList 