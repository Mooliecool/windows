//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

//This activity has two interesting characteristics:
//1.	It has some required arguments and some optional ones.
//2.	It allows the user to choose between providing two different sets of arguments. 

using System.Activities;

namespace Microsoft.Samples.OverloadGroups
{
    class CreateLocation: Activity
    {
        [RequiredArgument]
        public InArgument<string> Name { get; set; }
       
        public InArgument<string> Description { get; set; }

        [RequiredArgument]
        [OverloadGroup("G1")]
        public InArgument<int> Latitude { get; set; }
        
        [RequiredArgument]
        [OverloadGroup("G1")]
        public InArgument<int> Longitude { get; set; }

        [RequiredArgument]
        [OverloadGroup("G2")]
        [OverloadGroup("G3")]
        public InArgument<string> Street { get; set; }

        [RequiredArgument]
        [OverloadGroup("G2")]
        public InArgument<string> City { get; set; }

        [RequiredArgument]
        [OverloadGroup("G2")]
        public InArgument<string> State { get; set; }
        
        [RequiredArgument]
        [OverloadGroup("G3")]
        public InArgument<int> Zip { get; set; }                
    }
}
