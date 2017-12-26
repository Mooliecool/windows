//------------------------------------------------------------------------------
// <copyright file="IPersistComponentSettings.cs" company="Microsoft">
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
    /// <devdoc>
    ///     Components should implement this interface if they want to persist custom settings 
    ///     in a hosting application. This interface allows the application author to tell a control
    ///     whether to persist, when to load, save etc.
    /// </devdoc>
    public interface IPersistComponentSettings {

        /// <devdoc>
        ///     Indicates to the implementor that settings should be persisted.
        /// </devdoc>
        bool SaveSettings { get; set; }

        /// <devdoc>
        ///     Unique key that identifies an individual instance of a settings group(s). This key is needed
        ///     to identify which instance of a component owns a given group(s) of settings. Usually, the component
        ///     will frame its own key, but this property allows the hosting application to override it if necessary.
        /// </devdoc>
        string SettingsKey { get; set; }

        /// <devdoc>
        ///     Tells the component to load its settings.
        /// </devdoc>
        void LoadComponentSettings();

        /// <devdoc>
        ///     Tells the component to save its settings.
        /// </devdoc>
        void SaveComponentSettings();

        /// <devdoc>
        ///     Tells the component to reset its settings. Typically, the component can call Reset on its settings class(es).
        /// </devdoc>
        void ResetComponentSettings();
    }
}

