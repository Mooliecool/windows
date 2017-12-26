//------------------------------------------------------------------------------
// <copyright file="LongValidator.cs" company="Microsoft">
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
using System.Collections;
using System.IO;
using System.Reflection;
using System.Security.Permissions;
using System.Xml;
using System.Collections.Specialized;
using System.Globalization;
using System.ComponentModel;
using System.Security;
using System.Text;
using System.Text.RegularExpressions;

namespace System.Configuration {

    public class LongValidator : ConfigurationValidatorBase {
        private enum ValidationFlags {
            None = 0x0000,
            ExclusiveRange = 0x0001,   // If set the value must be outside of the range instead of inside
        }

        private ValidationFlags _flags = ValidationFlags.None;
        private long _minValue = long.MinValue;
        private long _maxValue = long.MaxValue;
        private long _resolution = 1;

        public LongValidator(long minValue, long maxValue)
            : this(minValue, maxValue, false, 1) {
        }
        
        public LongValidator(long minValue, long maxValue, bool rangeIsExclusive)
            : this(minValue, maxValue, rangeIsExclusive, 1) {
        }
        public LongValidator(long minValue, long maxValue, bool rangeIsExclusive, long resolution) {
            if (resolution <= 0) {
                throw new ArgumentOutOfRangeException("resolution");
            }

            if (minValue > maxValue) {
                throw new ArgumentOutOfRangeException("minValue", SR.GetString(SR.Validator_min_greater_than_max));
            }

            _minValue = minValue;
            _maxValue = maxValue;
            _resolution = resolution;

            _flags = rangeIsExclusive ? ValidationFlags.ExclusiveRange : ValidationFlags.None;
        }

        public override bool CanValidate(Type type) {
            return (type == typeof(long));
        }
        public override void Validate(object value) {
            ValidatorUtils.HelperParamValidation(value, typeof(long));

            ValidatorUtils.ValidateScalar((long)value,
                                            _minValue,
                                            _maxValue,
                                            _resolution,
                                            _flags == ValidationFlags.ExclusiveRange);
        }
    }
}
