//------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------

using System.ComponentModel;

namespace Microsoft.Samples.SwitchUsage
{
    [TypeConverter(typeof(PersonConverter))]
    public class Person
    {
        public string Name { get; set; }
        public int Age { get; set; }

        public Person()
        {
            this.Age = 15;
        }

        public Person(string name, int age)
        {
            this.Name = name;
            this.Age = age;
        }

        public override bool Equals(object obj)
        {
            Person person = obj as Person;

            if (person != null)
            {
                return string.Equals(this.Name, person.Name);
            }

            return false;
        }

        public override int GetHashCode()
        {
            if (this.Name != null)
            {
                return this.Name.GetHashCode();
            }

            return 0;
        }
    }
}
