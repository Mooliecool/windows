//------------------------------------------------------------------------------
// <copyright file="PositiveTimeSpanValidator.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

using System;
using System.Configuration;
using System.ComponentModel;

namespace System.Configuration {

    public class PositiveTimeSpanValidator : ConfigurationValidatorBase {
        public override bool CanValidate(Type type) {
            return (type == typeof(TimeSpan));
        }
        public override void Validate(object value) {
            if (value == null) {
                throw new ArgumentNullException("value");
            }

            if (((TimeSpan)value) <= TimeSpan.Zero) {
                throw new ArgumentException(SR.GetString(SR.Validator_timespan_value_must_be_positive));
            }
        }
    }
}
