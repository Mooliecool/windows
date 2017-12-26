//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;
using System.IO;

namespace Microsoft.Samples.Activities.Statements
{
    
    public sealed class CreateTextWriter : CodeActivity<TextWriter>
    {
        [RequiredArgument]
        public string Filename { get; set; }

        protected override TextWriter Execute(CodeActivityContext context)
        {
            TextWriter writer = new StreamWriter(this.Filename);
            return writer;
        }
    }
}
