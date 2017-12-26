'****************************** Module Header ******************************\
' Module Name:  Program.cs
' Project:      VBXPath
' Copyright (c) Microsoft Corporation.
' 
' This sample project shows how to use XPathDocument class to load the XML file
' and manipulate. It includes two main parts, XPathNavigator usage and XPath
' Expression usage. The first part shows how to use XPathNavigator to navigate 
' through the whole document, read its content. The second part shows how to 
' used XPath expression to filter information and select it out.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/

#Region "Imports directives"

Imports System.Xml.XPath

#End Region


Module MainModule

    Sub Main()
        'Initialize XPathDocument and XPathNavigator
        Dim nav As XPathNavigator
        Dim xPathNavigator As XPathNavigator = New XPathDocument("books.xml").CreateNavigator

        'Navigate through the document
        xPathNavigator.MoveToRoot()
        xPathNavigator.MoveToFirstChild()
        If ((xPathNavigator.NodeType = XPathNodeType.Element) AndAlso xPathNavigator.HasChildren) Then
            xPathNavigator.MoveToFirstChild()
            Do
                If xPathNavigator.HasAttributes Then
                    Console.WriteLine(("Book ID: " & xPathNavigator.GetAttribute("id", "")))
                End If
                If xPathNavigator.HasChildren Then
                    xPathNavigator.MoveToFirstChild()
                    Do
                        Console.Write(ChrW(9) & "{0}:" & ChrW(9) & "{1}" & ChrW(13) & ChrW(10), xPathNavigator.Name, xPathNavigator.Value)
                    Loop While xPathNavigator.MoveToNext
                    xPathNavigator.MoveToParent()
                End If
            Loop While xPathNavigator.MoveToNext
        End If

        'Use XPath Expression to select out the book with ID bk103
        Console.WriteLine("Use XPath Expression to select out the book with ID bk103:")
        Dim expression As XPathExpression = xPathNavigator.Compile("/catalog/book[@id='bk103']")
        Dim iterator As XPathNodeIterator = xPathNavigator.Select(expression)
        Do While iterator.MoveNext
            nav = iterator.Current.Clone
            Console.WriteLine(("Book ID: " & nav.GetAttribute("id", "")))
            If nav.HasChildren Then
                nav.MoveToFirstChild()
                Do
                    Console.Write(ChrW(9) & "{0}:" & ChrW(9) & "{1}" & ChrW(13) & ChrW(10), nav.Name, nav.Value)
                Loop While nav.MoveToNext
            End If
        Loop

        'Use XPath Expression to select out all books whose price are more than 10
        Console.WriteLine(ChrW(13) & ChrW(10) & "Use XPath Expression to select out all books whose price are more than 10:")
        expression = xPathNavigator.Compile("/catalog/book[price>10]")
        iterator = xPathNavigator.Select(expression)
        Do While iterator.MoveNext
            nav = iterator.Current.Clone
            Console.WriteLine(("Book ID: " & nav.GetAttribute("id", "")))
            If nav.HasChildren Then
                nav.MoveToFirstChild()
                Do
                    If (nav.Name = "title") Then
                        Console.Write(ChrW(9) & "{0}:" & ChrW(9) & "{1}" & ChrW(13) & ChrW(10), nav.Name, nav.Value)
                    End If
                    If (nav.Name = "price") Then
                        Console.Write(ChrW(9) & "{0}:" & ChrW(9) & "{1}" & ChrW(13) & ChrW(10), nav.Name, nav.Value)
                    End If
                Loop While nav.MoveToNext
            End If
        Loop

        'Use XPath Expression to calculate the average price of all books.
        Console.WriteLine(ChrW(13) & ChrW(10) & "Use XPath Expression to calculate the average price of all books:")
        expression = xPathNavigator.Compile("sum(/catalog/book/price) div count(/catalog/book/price)")
        Dim averagePrice As String = xPathNavigator.Evaluate(expression).ToString
        Console.WriteLine("The average price of the books are {0}", averagePrice)

        'End. Read a char to exit
        Console.WriteLine("Input any key to quit the sample application")
        Console.ReadLine()

    End Sub

End Module
