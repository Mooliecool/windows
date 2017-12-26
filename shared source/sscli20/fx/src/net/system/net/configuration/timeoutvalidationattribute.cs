//------------------------------------------------------------------------------
// <copyright file="TimeoutValidator.cs" company="Microsoft">
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

namespace System.Net {
    using System;
    using System.Configuration;
    using System.ComponentModel;
    
    internal sealed class TimeoutValidator : ConfigurationValidatorBase
    {
        bool _zeroValid = false;

        internal TimeoutValidator(bool zeroValid) {
            _zeroValid = zeroValid;
        }

        public override bool CanValidate( Type type ) {
            return ( type == typeof( int ) || type == typeof( long ) );
        }

        public override void Validate( object value ) {
            if (value == null)
                return;
            
            int timeout = (int)value;
            
            if (_zeroValid && timeout == 0)
                return;
            
            if (timeout <= 0 && timeout != System.Threading.Timeout.Infinite) {
                throw new ConfigurationErrorsException(SR.GetString(SR.net_io_timeout_use_gt_zero));
            }
        }
    }
}
