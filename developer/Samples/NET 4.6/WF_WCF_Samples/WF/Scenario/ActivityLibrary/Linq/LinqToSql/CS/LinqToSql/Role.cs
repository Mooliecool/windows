//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------
using System.Data.Linq.Mapping;

namespace Microsoft.Samples.Activities.Data
{

    [Table(Name = "Roles")]
    public class Role
    {
        [Column]
        public string Code { get; set; }

        [Column]
        public string Name { get; set; }
    }
}
