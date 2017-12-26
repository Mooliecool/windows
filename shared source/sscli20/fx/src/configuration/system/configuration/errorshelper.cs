//------------------------------------------------------------------------------
// <copyright file="FactoryRecord.cs" company="Microsoft">
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

namespace System.Configuration {
    using System.Collections;
    using System.Collections.Specialized;
    using System.Collections.Generic;

    static internal class ErrorsHelper {

        static internal int GetErrorCount(List<ConfigurationException> errors) {
            return (errors != null) ? errors.Count : 0;
        }

        static internal bool GetHasErrors(List<ConfigurationException> errors) {
            return GetErrorCount(errors) > 0;
        }

        static internal void AddError(ref List<ConfigurationException> errors, ConfigurationException e) {
            Debug.Assert(e != null, "e != null");

            // Create on demand
            if (errors == null) {
                errors = new List<ConfigurationException>();
            }

            ConfigurationErrorsException ce = e as ConfigurationErrorsException;
            if (ce == null) {
                errors.Add(e);
            }
            else {
                ICollection<ConfigurationException> col = ce.ErrorsGeneric;
                if (col.Count == 1) {
                    errors.Add(e);
                }
                else {
                    errors.AddRange(col);
                }
            }
        }

        static internal void AddErrors(ref List<ConfigurationException> errors, ICollection<ConfigurationException> coll) {
            if (coll == null || coll.Count == 0) {
                // Nothing to do here, bail
                return;
            }

            foreach (ConfigurationException e in coll) {
                AddError(ref errors, e);
            }
        }

        static internal ConfigurationErrorsException GetErrorsException(List<ConfigurationException> errors) {
            if (errors == null) {
                return null;
            }

            Debug.Assert(errors.Count != 0, "errors.Count != 0");
            return new ConfigurationErrorsException(errors);
        }

        static internal void ThrowOnErrors(List<ConfigurationException> errors) {
            ConfigurationErrorsException e = GetErrorsException(errors);
            if (e != null) {
                throw e;
            }
        }
    }
}
