//------------------------------------------------------------------------------
// <copyright file="INameCreationService.cs" company="Microsoft">
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

namespace System.ComponentModel.Design.Serialization {

    using System;
    using System.Collections;
    using System.ComponentModel.Design;

    /// <devdoc>
    ///     This service may be provided by a designer loader to provide
    ///     a way for the designer to fabricate new names for objects.
    ///     If this service isn't available the designer will choose a 
    ///     default implementation.
    /// </devdoc>
    public interface INameCreationService {
    
        /// <devdoc>
        ///     Creates a new name that is unique to all the components
        ///     in the given container.  The name will be used to create
        ///     an object of the given data type, so the service may
        ///     derive a name from the data type's name.  The container
        ///     parameter can be null if no container search is needed.
        /// </devdoc>
        string CreateName(IContainer container, Type dataType);
        
        /// <devdoc>
        ///     Determines if the given name is valid.  A name 
        ///     creation service may have rules defining a valid
        ///     name, and this method allows the sevice to enforce
        ///     those rules.
        /// </devdoc>
        bool IsValidName(string name);
    
        /// <devdoc>
        ///     Determines if the given name is valid.  A name 
        ///     creation service may have rules defining a valid
        ///     name, and this method allows the sevice to enforce
        ///     those rules.  It is similar to IsValidName, except
        ///     that this method will throw an exception if the
        ///     name is invalid.  This allows implementors to provide
        ///     rich information in the exception message.
        /// </devdoc>
        void ValidateName(string name);
    }
}

