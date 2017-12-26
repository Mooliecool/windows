//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System.Activities;
using System.Activities.Statements;
using System.Collections.Generic;

namespace Microsoft.Samples.EmulatingBreakInWhile
{

    // This activity finds the first reliable vendor from a supplied list of vendors.
    // It iterates through the list and when it finds a vendor above the minimum provided,
    // it breaks the loop.
    public sealed class FindReliableVendor: Activity<Vendor>
    {
        [RequiredArgument]
        public InArgument<IList<Vendor>> Vendors { get; set; }

        [RequiredArgument]
        public InArgument<int> MinimumReliability { get; set; }

		public FindReliableVendor()
		{
            var reliableVendor = new Variable<Vendor> { Name = "ReliableVendor", Default = null };
            var i = new Variable<int>() { Name = "Iteration Variable", Default = 0 };

            this.Implementation = () => new Sequence
            {
                DisplayName = "Finds the first reliable vendor (the first vendor with a reliability higher than the minimum acceptable parameter)",
                Variables = { reliableVendor, i },
                Activities =
                {
                    new WriteLine { Text = new InArgument<string>(env => string.Format("Iterating through the list searching a reliable vendor (min. reliability: {0})", this.MinimumReliability.Get(env).ToString())) },
                    new While(env => i.Get(env) < this.Vendors.Get(env).Count && reliableVendor.Get(env) == null)
                    {
                        DisplayName = "Main loop. Breaks when a reliable vendor is found",
                        Body = new Sequence
                        {                            
                            Activities =
                            {
                                new WriteLine() 
                                { 
                                    Text = new InArgument<string>(env => string.Format("....Checking {0}", this.Vendors.Get(env)[i.Get(env)].Name))
                                },
                                new If
                                {
                                    DisplayName = "Check for a reliable vendor",
                                    Condition = new InArgument<bool>(env => this.Vendors.Get(env)[i.Get(env)].Reliability > this.MinimumReliability.Get(env)),
                                    Then = new Assign<Vendor>
                                    {
                                        To = reliableVendor,
                                        Value = new InArgument<Vendor>(env => this.Vendors.Get(env)[i.Get(env)])
                                    }
                                },
                                new Assign<int>
                                {
                                    DisplayName = "Increment iteration variable",
                                    To = i,
                                    Value = new InArgument<int>(env => i.Get(env) + 1)
                                } 
                            }
                        }
                    },
                    new Assign<Vendor>
                    {
                        DisplayName = "Assign reliableVendor to output argument",
                        To = new OutArgument<Vendor>(env => this.Result.Get(env)),
                        Value = new InArgument<Vendor>(reliableVendor)
                    }
                }
            };
		}
    }
}
