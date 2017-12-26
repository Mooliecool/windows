/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:      CSLinqToObject
* Copyright (c) Microsoft Corporation.
* 
* This example illustrates how to write Linq to Object queries using Visual 
* C#. First, it builds a class named Person. Person inculdes the ID, Name and 
* Age properties. Then the example creates a list of Person which will be  
* used as the datasource. In the example, you will see the basic Linq 
* operations like select, update, orderby, max, average, etc.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 3/14/2009 09:00 PM Colbert Zhou Created
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
#endregion


class Program
{
    static void Main(string[] args)
    {
        /////////////////////////////////////////////////////////////////////
        // Build the Person list that serves as the data source.
        // 

        List<Person> persons = new List<Person>();

        persons.Add(new Person(1, "Ereka", 20));
        persons.Add(new Person(2, "Marion", 18));
        persons.Add(new Person(3, "Grace", 21));
        persons.Add(new Person(4, "David", 19));
        persons.Add(new Person(5, "Michael", 20));
        persons.Add(new Person(6, "Stephen", 21));
        persons.Add(new Person(7, "Sam", 22));
        persons.Add(new Person(8, "Cael", 19));
        persons.Add(new Person(9, "Eric", 20));


        /////////////////////////////////////////////////////////////////////
        // Query a person in the data source.
        // 

        var ereka = (from p in persons
                      where p.Name == "Ereka"
                      select p).First();
        // [-or-]
        // Use extension method + lambda expression directly 
        //var ereka = persons.Where(p => p.Name == "Ereka").First();

        Console.WriteLine("Ereka's age is {0}", ereka.Age);


        /////////////////////////////////////////////////////////////////////
        // Perform the Update operation on the person's age.
        // 

        ereka.Age = 21;
        Console.WriteLine("Ereka's age is updated to 21");


        /////////////////////////////////////////////////////////////////////
        // Sort the data in the data source.
        // 

        // Order the persons by age
        var query1 = from p in persons
                    orderby p.Age
                    select p;

        Console.WriteLine("ID\tName\t\tAge");

        foreach (Person p in query1.ToList<Person>())
        {
            Console.WriteLine("{0}\t{1}\t\t{2}", p.PersonID, p.Name, p.Age);
        }
        

        /////////////////////////////////////////////////////////////////////
        // Print the average, max, min age of the persons.
        // 

        double avgAge = (from p in persons
                         select p.Age).Average();
        Console.WriteLine("The average age of the persons is {0:f2}", avgAge);

        double maxAge = (from p in persons
                         select p.Age).Max();
        Console.WriteLine("The maximum age of the persons is {0}", maxAge);

        double minAge = (from p in persons
                         select p.Age).Min();
        Console.WriteLine("The minimum age of the persons is {0}", minAge);


        /////////////////////////////////////////////////////////////////////
        // Count the persons who age is larger than 20
        // 

        var query2 = from p in persons
                     where p.Age > 20
                     select p;

        int count = query2.Count();
        Console.WriteLine("{0} persons are older than 20:", count);
        for (int i = 0; i < count; i++)
        {
            Console.WriteLine(query2.ElementAt(i).Name);
        }
    }
}