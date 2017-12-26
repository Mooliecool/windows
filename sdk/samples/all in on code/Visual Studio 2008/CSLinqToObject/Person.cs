using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


class Person
{
    public Person(int id, string name, int age)
    {
        this.id = id;
        this.name = name;
        this.age = age;
    }

    private int id;

    /// <summary>
    /// Person ID
    /// </summary>
    public int PersonID
    {
        get { return this.id; }
        set { this.id = value; }
    }

    private string name;

    /// <summary>
    /// Person name
    /// </summary>
    public string Name
    {
        get { return this.name; }
        set { this.name = value; }
    }

    private int age;

    /// <summary>
    /// Age that ranges from 1 to 100
    /// </summary>
    public int Age
    {
        get { return this.age; }
        set
        {
            if (value > 0 && value <= 100)
                this.age = value;
            else
                throw new Exception("Age is out of scope [1,100]");
        }
    }
}