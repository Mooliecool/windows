'******************************** Module Header ***************************************************'
' Module Name:  Condition.vb
' Project:      VBDynamicallyBuildLambdaExpressionWithField
' Copyright (c) Microsoft Corporation.
'
' The Condition.vb file defines a LINQ operator concerning with System.Linq.Expressions namespace.
'
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
' EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
' MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'**************************************************************************************************'

Imports System.Linq.Expressions

Public Module DynamicQuery
#Region "Condition Classes"

    Public MustInherit Class Condition

#Region "Fields"

        ' Used to ensure we get the same instance of a particular ParameterExpression
        ' across multiple queries
        Private Shared paramTable As New Dictionary(Of String, ParameterExpression)()

        ' The expression tree which will be passed to the LINQ to SQL runtime
        Protected Friend LambdaExpr As LambdaExpression

        ' Enumerates all the different comparisons which can be performed.
        Public Enum Compare As Integer
            [Or] = ExpressionType.Or
            [And] = ExpressionType.And
            [Xor] = ExpressionType.ExclusiveOr
            [Not] = ExpressionType.Not
            Equal = ExpressionType.Equal
            [Like] = ExpressionType.TypeIs + 1
            NotEqual = ExpressionType.NotEqual
            [OrElse] = ExpressionType.OrElse
            [AndAlso] = ExpressionType.AndAlso
            LessThan = ExpressionType.LessThan
            GreaterThan = ExpressionType.GreaterThan
            LessThanOrEqual = ExpressionType.LessThanOrEqual
            GreaterThanOrEqual = ExpressionType.GreaterThanOrEqual
        End Enum
#End Region

        ''' <summary>
        ''' Constructs a Condition with T as the element type and S as the value's type
        ''' </summary>
        Public Shared Function Create(Of T, S)(ByVal dataSource As IEnumerable(Of T), ByVal propertyName As String, ByVal condType As Compare, ByVal value As S) As Condition(Of T, S)

            Return New Condition(Of T, S)(propertyName, condType, value)
        End Function

        ''' <summary>
        ''' Constructs a Condition with T as the element type and valueType as the value's type
        ''' This is useful for situations where you won't know the value's type until runtime.
        ''' </summary>
        Public Shared Function Create(Of T)(ByVal dataSource As IEnumerable(Of T), ByVal propertyName As String, ByVal condType As Compare, ByVal value As Object, ByVal valueType As Type) As Condition(Of T)

            Return New Condition(Of T)(propertyName, condType, value, valueType)
        End Function


        ''' <summary>
        ''' Creates a Condition which combines two other Conditions
        ''' </summary>
        ''' <typeparam name="T">The type the condition will execute against</typeparam>
        ''' <param name="cond1">The first Condition</param>
        ''' <param name="condType">The operator to use on the conditions</param>
        ''' <param name="cond2">The second Condition</param>
        ''' <returns>A new Condition which combines two Conditions into one according to the 
        ''' specified operator</returns>
        ''' <remarks></remarks>
        Public Shared Function Combine(Of T)(ByVal cond1 As Condition(Of T), ByVal condType As Compare, ByVal cond2 As Condition(Of T)) As Condition(Of T)
            Return Condition(Of T).Combine(cond1, condType, cond2)
        End Function

        ''' <summary>
        ''' Combines multiple conditions according to the specified operator
        ''' </summary>
        Public Shared Function Combine(Of T)(ByVal cond1 As Condition(Of T), ByVal condType As Compare, ByVal ParamArray conditions() As Condition(Of T)) As Condition(Of T)
            Return Condition(Of T).Combine(cond1, condType, conditions)
        End Function


#Region "Protected Methods"
        ''' <summary>
        ''' Combines two Expressions according to the specified operator (condType)
        ''' </summary>
        Protected Shared Function CombineExpression(ByVal left As Expression, ByVal condType As Compare, ByVal right As Expression) As Expression

            'Join the Expressions based on the operator
            Select Case condType
                Case Compare.Or
                    Return Expression.Or(left, right)
                Case Compare.And
                    Return Expression.And(left, right)
                Case Compare.Xor
                    Return Expression.ExclusiveOr(left, right)
                Case Compare.Equal
                    Return Expression.Equal(left, right)
                Case Compare.OrElse
                    Return Expression.OrElse(left, right)
                Case Compare.AndAlso
                    Return Expression.AndAlso(left, right)
                Case Compare.NotEqual
                    Return Expression.NotEqual(left, right)
                Case Compare.LessThan
                    Return Expression.LessThan(left, right)
                Case Compare.GreaterThan
                    Return Expression.GreaterThan(left, right)
                Case Compare.LessThanOrEqual
                    Return Expression.LessThanOrEqual(left, right)
                Case Compare.GreaterThanOrEqual
                    Return Expression.GreaterThanOrEqual(left, right)
                Case Compare.Like

                    'For the Like operator we encode a call to the LikeString method in the VB runtime
                    Dim m = GetType(CompilerServices.Operators).GetMethod("LikeString")

                    Return Expression.Call(m, left, right, Expression.Constant(CompareMethod.Binary))

                Case Else
                    Throw New ArgumentException("Not a valid Condition Type", "condType", Nothing)
            End Select
        End Function

        ''' <summary>
        ''' Since both type parameters must be the same, we can turn what would normally
        ''' be a Func(Of T, T, Boolean) into a Func(Of T, Boolean)
        ''' </summary>
        Protected Shared Function CombineFunc(Of T)(ByVal d1 As Func(Of T, Boolean), ByVal condType As Compare, ByVal d2 As Func(Of T, Boolean)) As Func(Of T, Boolean)

            'Return a delegate which combines delegates d1 and d2
            Select Case condType
                Case Compare.Or
                    Return Function(x) d1(x) Or d2(x)
                Case Compare.And
                    Return Function(x) d1(x) And d2(x)
                Case Compare.Xor
                    Return Function(x) d1(x) Xor d2(x)
                Case Compare.Equal
                    Return Function(x) d1(x) = d2(x)
                Case Compare.OrElse
                    Return Function(x) d1(x) OrElse d2(x)
                Case Compare.AndAlso
                    Return Function(x) d1(x) AndAlso d2(x)
                Case Compare.NotEqual
                    Return Function(x) d1(x) <> d2(x)
                Case Compare.LessThan
                    Return Function(x) Integer.Parse(d1(x).ToString()) < Integer.Parse(d2(x).ToString())
                Case Compare.GreaterThan
                    Return Function(x) Integer.Parse(d1(x).ToString()) > Integer.Parse(d2(x).ToString())
                Case Compare.LessThanOrEqual
                    Return Function(x) Integer.Parse(d1(x).ToString()) <= Integer.Parse(d2(x).ToString())
                Case Compare.GreaterThanOrEqual
                    Return Function(x) Integer.Parse(d1(x).ToString()) >= Integer.Parse(d2(x).ToString())
                Case Else
                    Throw New ArgumentException("Not a valid Condition Type", "condType")
            End Select
        End Function

        ''' <summary>
        ''' Guarantees that we get the same instance of a ParameterExpression for a given type t.
        ''' </summary>
        Protected Shared Function GetParamInstance(ByVal dataType As Type) As ParameterExpression

            'Parameters are matched by reference, not by name, so we cache the instances in a Dictionary.
            If Not (paramTable.ContainsKey(dataType.Name)) Then
                paramTable.Add(dataType.Name, Expression.Parameter(dataType, dataType.Name))
            End If

            Return paramTable(dataType.Name)
        End Function
#End Region

    End Class

    Public Class Condition(Of T)
        Inherits Condition

        'Delegate that contains a compiled expression tree which can be run locally
        Friend del As Func(Of T, Boolean)

#Region "Constructors"

        Friend Sub New()
        End Sub

        Friend Sub New(ByVal propName As String, ByVal condType As Compare, ByVal value As Object, ByVal valueType As Type)
            ' Split the string to handle nested property access
            Dim s = propName.Split("."c)

            ' Get the PropertyInfo instance for propName
            Dim pInfo = GetType(T).GetProperty(s(0))
            Dim paramExpr = GetParamInstance(GetType(T))
            Dim callExpr = Expression.MakeMemberAccess(paramExpr, pInfo)

            ' For each member specified, construct the additional MemberAccessExpression
            ' For example, if the user says "myCustomer.Order.OrderID = 4" we need an
            ' additional MemberAccessExpression for "Order.OrderID = 4"
            For i = 1 To s.GetUpperBound(0)
                pInfo = pInfo.PropertyType.GetProperty(s(i))
                callExpr = Expression.MakeMemberAccess(callExpr, pInfo)
            Next i

            ' ConstantExpression representing the value on the left side of the operator
            Dim valueExpr = Expression.Constant(value, valueType)

            Dim b As Expression = CombineExpression(callExpr, condType, valueExpr)
            LambdaExpr = Expression.Lambda(Of Func(Of T, Boolean))(b, New ParameterExpression() {paramExpr})

            ' Compile the lambda expression into a delegate
            del = CType(LambdaExpr.Compile(), Func(Of T, Boolean))
        End Sub

#End Region

#Region "Methods"

        ' Combines two conditions according to the specified operator
        Friend Shared Function Combine(ByVal cond1 As Condition(Of T), ByVal condType As Compare, ByVal cond2 As Condition(Of T)) As Condition(Of T)
            Dim c As New Condition(Of T)()

            Dim b As Expression = CombineExpression(cond1.LambdaExpr.Body, condType, cond2.LambdaExpr.Body)

            Dim paramExpr = New ParameterExpression() {GetParamInstance(GetType(T))}

            ' Create the LambdaExpression and compile the delegate
            c.LambdaExpr = Expression.Lambda(Of Func(Of T, Boolean))(b, paramExpr)
            c.del = Condition.CombineFunc(cond1.del, condType, cond2.del)

            Return c
        End Function

        ' Combines multiple conditions according to the specified operator
        Friend Shared Function Combine(ByVal cond1 As Condition(Of T), ByVal condType As Compare, ByVal ParamArray conditions() As Condition(Of T)) As Condition(Of T)
            Dim finalCond = cond1
            For Each c In conditions
                finalCond = Condition.Combine(finalCond, condType, c)
            Next c

            Return finalCond
        End Function

        ' Run query locally instead of remotely
        Public Function Matches(ByVal row As T) As Boolean
            Return del(row) ' Passes the row into the delegate to see if it's a match
        End Function

#End Region

#Region "Overloaded Operators"

        ' Overloaded operators - allows syntax like "(condition1 Or condition2) And condition3"
        Public Shared Operator And(ByVal c1 As Condition(Of T), ByVal c2 As Condition(Of T)) As Condition(Of T)
            Return Condition.Combine(c1, Compare.And, c2)
        End Operator

        Public Shared Operator Or(ByVal c1 As Condition(Of T), ByVal c2 As Condition(Of T)) As Condition(Of T)
            Return Condition.Combine(c1, Compare.Or, c2)
        End Operator

        Public Shared Operator Xor(ByVal c1 As Condition(Of T), ByVal c2 As Condition(Of T)) As Condition(Of T)
            Return Condition.Combine(c1, Compare.Xor, c2)
        End Operator

#End Region

    End Class

    ' Represents a condition like "object.Property = value"
    ' In this case object is of type T, and value is of type S
    '
    ' Even though most of the logic for this is already in the base class, 
    ' defining a second generic parameter means the user doesn't have to
    ' pass in a System.Type - it can just be inferred.
    Public Class Condition(Of T, S)
        Inherits Condition(Of T)
        Friend Sub New(ByVal propName As String, ByVal condType As Compare, ByVal value As S)
            MyBase.New(propName, condType, value, GetType(S))
        End Sub
    End Class

#End Region

#Region "Extension Methods"

    ''' <summary>
    ''' Filters an IQueryable(Of T) according to the specified condition
    ''' </summary>
    <System.Runtime.CompilerServices.Extension()> _
    Public Function Where(Of T)(ByVal source As IQueryable(Of T), ByVal condition_Renamed As Condition(Of T)) As IQueryable(Of T)

        Dim callExpr = Expression.Call(GetType(Queryable), "Where", New Type() {source.ElementType}, source.Expression, Expression.Quote(condition_Renamed.LambdaExpr))

        Return CType(source.Provider.CreateQuery(callExpr), IQueryable(Of T))
    End Function

    ''' <summary>
    ''' Filters an IEnumerable(Of T) according to the specified condition
    ''' </summary>
    <System.Runtime.CompilerServices.Extension()> _
    Public Function Where(Of T)(ByVal source As IEnumerable(Of T), ByVal condition_Renamed As Condition(Of T)) As IEnumerable(Of T)
        Return source.Where(condition_Renamed.del)
    End Function

    ''' <summary>
    '''  Extension method that can be called off any type that implements IEnumerable(Of T), 
    '''  which constructs a Condition with T as the element type and S as the value's type
    ''' </summary>
    <System.Runtime.CompilerServices.Extension()> _
    Public Function CreateCondition(Of T, S)(ByVal dataSource As IEnumerable(Of T), ByVal propName As String, ByVal condType As DynamicQuery.Condition.Compare, ByVal value As S) As Condition(Of T, S)

        Return Condition.Create(dataSource, propName, condType, value)
    End Function
#End Region
End Module
