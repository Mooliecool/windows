//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Configuration;

namespace Microsoft.Samples.ByteStreamEncoder
{

    [AttributeUsage(AttributeTargets.Property)]
    sealed class ServiceModelEnumValidatorAttribute : ConfigurationValidatorAttribute
    {
        Type enumHelperType;

        public ServiceModelEnumValidatorAttribute(Type enumHelperType)
        {
            this.EnumHelperType = enumHelperType;
        }

        public Type EnumHelperType
        {
            get { return this.enumHelperType; }
            set { this.enumHelperType = value; }
        }

        public override ConfigurationValidatorBase ValidatorInstance
        {
            get { return new ServiceModelEnumValidator(enumHelperType); }
        }
    }
}
