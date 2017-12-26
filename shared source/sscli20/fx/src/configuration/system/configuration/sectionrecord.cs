//------------------------------------------------------------------------------
// <copyright file="SectionRecord.cs" company="Microsoft">
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
    using System.Configuration.Internal;
    using System.Collections;
    using System.Collections.Specialized;
    using System.Collections.Generic;
    using System.Configuration;
    using System.Text;
    using System.Threading;
    using System.Reflection;
    using System.Xml;

    [System.Diagnostics.DebuggerDisplay("SectionRecord {ConfigKey}")]
    internal class SectionRecord {
        //
        // Flags constants
        //

        //
        // Runtime flags below 0x10000
        //

        // locked by parent input, either because a parent section is locked,
        // a parent section locks all children, or a location input for this 
        // configPath has allowOverride=false.
        private const int Flag_Locked                               = 0x00000001;

        // lock children of this section
        private const int Flag_LockChildren                         = 0x00000002;

        // propagation of FactoryRecord.IsFactoryTrustedWithoutAptca
        private const int Flag_IsResultTrustedWithoutAptca          = 0x00000004;

        // propagation of FactoryRecord.RequirePermission
        private const int Flag_RequirePermission                    = 0x00000008;

        //
        // Designtime flags at or above 0x00010000
        //

        // the section has been added to the update list
        private const int Flag_AddUpdate                            = 0x00010000;

        // result can be null, so we use this object to indicate whether it has been evaluated
        static object                           s_unevaluated = new object();

        private SafeBitVector32                _flags;

        // config key
        private string                          _configKey;

        // The input from location sections
        // This list is ordered to keep oldest ancestors at the front
        private List<SectionInput>              _locationInputs;    

        // The input from this file
        private SectionInput                    _fileInput;

        // the cached result of evaluating this section
        private object                          _result;
        
        // the cached result of evaluating this section after GetRuntimeObject is called
        private object                          _resultRuntimeObject;

        // errors not associated with a particular input
        // this it NOT the sum of all errors
        private List<ConfigurationException>    _errors;


        internal SectionRecord(string configKey) {
            _configKey = configKey;
            _result = s_unevaluated;
            _resultRuntimeObject = s_unevaluated;
        }

        internal string ConfigKey {
            get {return _configKey;}
        }

        internal bool Locked {
            get {return _flags[Flag_Locked];}
            set {_flags[Flag_Locked] = value;}
        }

        internal bool LockChildren {
            get {return _flags[Flag_LockChildren];}
            set {_flags[Flag_LockChildren] = value;}
        }

        internal bool IsResultTrustedWithoutAptca {
            get {return _flags[Flag_IsResultTrustedWithoutAptca];}
            set {_flags[Flag_IsResultTrustedWithoutAptca] = value;}
        }

        internal bool RequirePermission {
            get {return _flags[Flag_RequirePermission];}
            set {_flags[Flag_RequirePermission] = value;}
        }

        internal bool AddUpdate {
            get {return _flags[Flag_AddUpdate];}
            set {_flags[Flag_AddUpdate] = value;}
        }

        internal bool HasLocationInputs {
            get {
                return _locationInputs != null && _locationInputs.Count > 0;
            }
        }

        internal List<SectionInput> LocationInputs {
            get {return _locationInputs;}
        }

        internal SectionInput LastLocationInput {
            get {
                if (HasLocationInputs) {
                    return _locationInputs[_locationInputs.Count - 1];
                }
                else {
                    return null;
                }
            }
        }

        internal void 
        AddLocationInput(SectionInput sectionInput) {
            if (_locationInputs == null) {
                _locationInputs = new List<SectionInput>(1);
            }

            // The list of locationSections is traversed from child to parent,
            // so insert at the beginning of the list.
            _locationInputs.Insert(0, sectionInput);

            // If LockChildren is set, lock both ourselves and children
            if (!sectionInput.HasErrors && sectionInput.SectionXmlInfo.LockChildren) {
                Locked = true;
                LockChildren = true;
            }
        }

        internal bool HasFileInput {
            get {
                return _fileInput != null;
            }
        }

        internal SectionInput FileInput {
            get { return _fileInput; }
        }

        // AddFileInput
        internal void AddFileInput(SectionInput sectionInput) {
            _fileInput = sectionInput;

            // If LockChildren is set, lock our children
            if (!sectionInput.HasErrors && sectionInput.SectionXmlInfo.LockChildren) {
                LockChildren = true;
            }
        }

        internal void RemoveFileInput() {
            if (_fileInput != null) {
                _fileInput = null;

                // Reset LockChildren flag to the value provided by 
                // location input or inherited sections.
                LockChildren = Locked;
            }
        }

        internal bool HasInput {
            get {
                return HasLocationInputs || HasFileInput;
            }
        }

        internal void ClearRawXml() {
            if (HasLocationInputs) {
                foreach (SectionInput locationInput in LocationInputs) {
                    locationInput.SectionXmlInfo.RawXml = null;
                }
            }

            if (HasFileInput) {
                FileInput.SectionXmlInfo.RawXml = null;
            }
        }

        internal bool HasResult {
            get {return _result != s_unevaluated;}
        }

        internal bool HasResultRuntimeObject {
            get {return _resultRuntimeObject != s_unevaluated;}
        }

        internal object Result {
            get {
                // Useful assert, but it fires in the debugger when using automatic property evaluation
                // Debug.Assert(_result != s_unevaluated, "_result != s_unevaluated");

                return _result;
            }

            set {_result = value;}
        }

        internal object ResultRuntimeObject {
            get {
                // Useful assert, but it fires in the debugger when using automatic property evaluation
                // Debug.Assert(_resultRuntimeObject != s_unevaluated, "_resultRuntimeObject != s_unevaluated");

                return _resultRuntimeObject;
            }

            set {
                _resultRuntimeObject = value;
            }
        }

        internal void ClearResult() {
            if (_fileInput != null) {
                _fileInput.ClearResult();
            }

            if (_locationInputs != null) {
                foreach (SectionInput input in _locationInputs) {
                    input.ClearResult();
                }
            }

            _result = s_unevaluated;
            _resultRuntimeObject = s_unevaluated;
        }

        //
        // Error handling.
        //

        private List<ConfigurationException> GetAllErrors() {
            List<ConfigurationException> allErrors = null;

            ErrorsHelper.AddErrors(ref allErrors, _errors);

            if (HasLocationInputs) {
                foreach (SectionInput input in LocationInputs) {
                    ErrorsHelper.AddErrors(ref allErrors, input.Errors);
                }
            }

            if (HasFileInput) {
                ErrorsHelper.AddErrors(ref allErrors, FileInput.Errors);
            }

            return allErrors;
        }

        internal bool HasErrors {
            get {
                if (ErrorsHelper.GetHasErrors(_errors)) {
                    return true;
                }

                if (HasLocationInputs) {
                    foreach (SectionInput input in LocationInputs) {
                        if (input.HasErrors) {
                            return true;
                        }
                    }
                }

                if (HasFileInput) {
                    if (FileInput.HasErrors) {
                        return true;
                    }
                }

                return false;
            }
        }

        internal void ThrowOnErrors() {
            if (HasErrors) {
                throw new ConfigurationErrorsException(GetAllErrors());
            }
        }
    }
}

