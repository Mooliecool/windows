//------------------------------------------------------------------------------
// <copyright file="TypeUtil.cs" company="Microsoft">
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
using System.Reflection;
using System.Security;
using System.Security.Permissions;

namespace System.Configuration {

    internal static class TypeUtil {
        [ReflectionPermission(SecurityAction.Assert, Flags=ReflectionPermissionFlag.TypeInformation | ReflectionPermissionFlag.MemberAccess)]
        static internal object CreateInstanceWithReflectionPermission(string typeString) {
            Type type = Type.GetType(typeString, true);           // catch the errors and report them
            object result = Activator.CreateInstance(type, true); // create non-public types
            return result;
        }
    }
}
