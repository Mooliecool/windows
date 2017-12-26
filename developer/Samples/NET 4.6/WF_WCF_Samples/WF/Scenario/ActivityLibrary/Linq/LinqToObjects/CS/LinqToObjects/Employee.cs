//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------
namespace Microsoft.Samples.Activities.Statements
{
    public class Employee
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public string Role { get; set; }
        public string Location { get; set; }

        public Employee()
        {
        }

        public Employee(int id, string name, string role, string location)
        {
            this.Id = id;
            this.Name = name;
            this.Role = role;
            this.Location = location;
        }

        public override string ToString()
        {
            return string.Format("{0} {1} ({2} at {3})", Id, Name, Role, Location);
        }
    }
}