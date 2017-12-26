//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------
using System.Activities;
using System.Text.RegularExpressions;

namespace Microsoft.Samples.Activities.Statements
{

    // Activity to provide Regex.Replace functionality in WF programs
    public sealed class Replace : CodeActivity<string>
    {        
        public Replace() : base()
        {
            this.RegexOption = RegexOptions.IgnoreCase | RegexOptions.Compiled;
        }

        [RequiredArgument]
        public InArgument<string> Pattern { get; set; }

        [RequiredArgument]
        public InArgument<string> Input { get; set; }

        public RegexOptions RegexOption { get; set; }
        
        public InArgument<string> Replacement { get; set; }
        
        public MatchEvaluator MatchEvaluator { get; set; }

        protected override void CacheMetadata(CodeActivityMetadata metadata)
        {          
            if (this.MatchEvaluator == null && (this.Replacement == null || this.Replacement.Expression == null))
            {
                metadata.AddValidationError("'Replacement' or 'MatchEvaluator' arguments in Replace not set.");
            }

            base.CacheMetadata(metadata);
        }

        protected override string Execute(CodeActivityContext context)
        {
            // we are using static method because by default the regular expression engine caches the 15 most recently used static regular expressions            
            
            if (this.Replacement.Get(context) != null)
            {
                return Regex.Replace(this.Input.Get(context), this.Pattern.Get(context), this.Replacement.Get(context), this.RegexOption);
            }
            else            
            {
                return Regex.Replace(this.Input.Get(context), this.Pattern.Get(context), this.MatchEvaluator, this.RegexOption);
            }
        }
    }
}
