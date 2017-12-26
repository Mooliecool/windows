//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Configuration;

namespace Microsoft.Samples.ByteStreamEncoder
{

    [AttributeUsage(AttributeTargets.Property)]
    internal sealed class AddressValidatorAttribute : ConfigurationValidatorAttribute
    {
        public override ConfigurationValidatorBase ValidatorInstance
        {
            get { return new AddressValidator(); }
        }
    }
}
