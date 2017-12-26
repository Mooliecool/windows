//------------------------------------------------------------------------------
// <copyright file="ITypeResolutionService.cs" company="Microsoft">
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

/*
 */
namespace System.ComponentModel.Design {
    using System;
    using System.Reflection;

    /// <devdoc>
    ///    <para>
    ///         The type resolution service is used to load types at design time.
    ///    </para>
    /// </devdoc>
    public interface ITypeResolutionService {

        /// <devdoc>
        ///     Retrieves the requested assembly.
        /// </devdoc>    
        Assembly GetAssembly(AssemblyName name);
    
        /// <devdoc>
        ///     Retrieves the requested assembly.
        /// </devdoc>    
        Assembly GetAssembly(AssemblyName name, bool throwOnError);
    
        /// <devdoc>
        ///     Loads a type with the given name.
        /// </devdoc>
        Type GetType(string name);
    
        /// <devdoc>
        ///     Loads a type with the given name.
        /// </devdoc>
        Type GetType(string name, bool throwOnError);
    
        /// <devdoc>
        ///     Loads a type with the given name.
        /// </devdoc>
        Type GetType(string name, bool throwOnError, bool ignoreCase);
    
        /// <devdoc>
        ///     References the given assembly name.  Once an assembly has
        ///     been referenced types may be loaded from it without
        ///     qualifying them with the assembly.
        /// </devdoc>
        void ReferenceAssembly(AssemblyName name);

        /// <devdoc>
        ///    <para>
        ///       Returns the path to the file name from which the assembly was loaded.
        ///    </para>
        /// </devdoc>
        string GetPathOfAssembly(AssemblyName name);
    }
}

