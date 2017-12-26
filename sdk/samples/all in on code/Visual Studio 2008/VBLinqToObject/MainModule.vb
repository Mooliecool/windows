'****************************** Module Header ******************************\
' Module Name:  Program.cs
' Project:      VBLinqToObject
' Copyright (c) Microsoft Corporation.
' 
' This example illustrates how to write Linq to Object queries using Visual 
' VB.NET. First, it builds a class named Person. Person inculdes the ID, 
' Name and Age properties. Then the example creates a list of Person which 
' will be used as the datasource. In the example, you will see the basic 
' Linq operations like select, update, orderby, max, average, etc.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' History:
' * 5/8/2009 02:00 PM Lingzhi Sun Created
'***************************************************************************/


Module MainModule

    Sub Main()

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Build the Person list that serves as the data source.
        '

        Dim persons As New List(Of Person)

        persons.Add(New Person(1, "Ereka", 20))
        persons.Add(New Person(2, "Marion", 18))
        persons.Add(New Person(3, "Grace", 21))
        persons.Add(New Person(4, "David", 19))
        persons.Add(New Person(5, "Michael", 20))
        persons.Add(New Person(6, "Stephen", 21))
        persons.Add(New Person(7, "Sam", 22))
        persons.Add(New Person(8, "Cael", 19))
        persons.Add(New Person(9, "Eric", 20))


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Query a person in the data source.
        '

        Dim ereka = (From p In persons _
                    Where p.Name = "Ereka" _
                    Select p).First()

        Console.WriteLine("Ereka's age is {0}", ereka.Age)


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Perform the Update operation on the person's age.
        '

        ereka.Age = 21

        Console.WriteLine("Ereka's age is updated to 21")


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Sort the data in the data source.
        '

        ' Order the persons by age
        Dim query1 = From p In persons _
                     Order By p.Age _
                     Select p

        Console.WriteLine("ID" & vbTab & "Name" & vbTab & vbTab & "Age")

        For Each p In query1.ToList()
            Console.WriteLine("{0}" & vbTab & "{1}" & vbTab & vbTab & "{2}", _
                              p.PersonID, p.Name, p.Age)
        Next


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Print the average, max, min age of the persons.
        '

        Dim avgAge As Double = (From p In persons _
                                Select p.Age).Average()
        Console.WriteLine("The average age of the persons is {0:f2}", avgAge)

        Dim maxAge As Double = (From p In persons _
                                Select p.Age).Max()
        Console.WriteLine("The maximum age of the persons is {0}", maxAge)

        Dim minAge As Double = (From p In persons _
                                Select p.Age).Min()
        Console.WriteLine("The minimum age of the persons is {0}", minAge)


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Count the persons who age is larger than 20.
        '

        Dim query2 = From p In persons _
                     Where p.Age > 20 _
                     Select p

        Dim count As Integer = query2.Count()

        Console.WriteLine("{0} persons are older than 20:", count)

        For i = 0 To count - 1
            Console.WriteLine(query2.ElementAt(i).Name)
        Next

    End Sub

End Module
