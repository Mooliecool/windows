//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------
using System.Activities;
using System.Text.RegularExpressions;

namespace Microsoft.Samples.Activities.Statements
{

    // Activity to provide Regex.IsMatch functionality in WF programs
    public sealed class IsMatch: CodeActivity<bool>
    {
        public IsMatch() : base() 
        {
            this.RegexOption = RegexOptions.IgnoreCase | RegexOptions.Compiled;
        }
        [RequiredArgument]        
        public InArgument<string> Pattern { get; set; }

        [RequiredArgument]
        public InArgument<string> Input { get; set; }

        public RegexOptions RegexOption { get; set; }

        protected override bool Execute(CodeActivityContext context)
        {
            // we are using static method because by default the regular expression engine caches the 15 most recently used static regular expressions            

            return Regex.IsMatch(this.Input.Get(context), this.Pattern.Get(context), this.RegexOption);                        
        }
    }
}
