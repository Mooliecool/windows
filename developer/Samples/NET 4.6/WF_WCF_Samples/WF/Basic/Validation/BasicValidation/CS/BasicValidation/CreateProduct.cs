//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System.Activities;

namespace Microsoft.Samples.BasicValidation
{

    public sealed class CreateProduct : CodeActivity
    {       
        public double Price { get; set; }
        public double Cost { get; set; }

        //[RequiredArgument] attribute will generate a validation error if the Description argument is not set
        [RequiredArgument]
        public InArgument<string> Description { get; set; }
                
        protected override void CacheMetadata(CodeActivityMetadata metadata)
        {
            base.CacheMetadata(metadata);
            //Determine when the activity has been configured in an invalid way
            if (this.Cost > this.Price)
            {
                //Add a validation error with a custom message
                metadata.AddValidationError("The Cost must be less than or equal to the Price");
            }
        }

        protected override void Execute(CodeActivityContext context)
        {
            // not needed for the sample
        }
    }
}
