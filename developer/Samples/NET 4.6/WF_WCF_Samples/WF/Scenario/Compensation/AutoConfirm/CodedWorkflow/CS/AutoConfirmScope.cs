//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;
using System.Activities.Statements;
using Microsoft.VisualBasic.Activities;

namespace Microsoft.Samples.Compensation.AutoConfirmSample
{
    public sealed class AutoConfirmScope : Activity
    {
        public AutoConfirmScope()
        {
            base.Implementation = this.InternalImplementation;
        }

        public Activity Body { get; set; }

        private Activity InternalImplementation()
        {
            Variable<CompensationToken> token = new Variable<CompensationToken>() { Name = "Token" };

            return new TryCatch()
            {
                Variables = { token },
                Try = new CompensableActivity()
                {
                    Result = token,
                    Body = this.Body,
                },
                Finally = new If()
                {
                    //Checks to see if the CompensableActivity completed and has not already been compensated
                    Condition = new VisualBasicValue<bool> { ExpressionText = "Token Is Nothing" },
                    Else = new Confirm() { Target = token },
                }
            };
        }
    }
}
