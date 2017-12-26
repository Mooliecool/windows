//------------------------------------------------------------------------------
// <copyright file="SecurityUtils.cs" company="Microsoft">
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


#if SYSTEM_NAMESPACE
    namespace System
#else
   namespace System.Windows.Forms 
#endif
{
    using System;
    using System.Reflection;
    using System.Diagnostics.CodeAnalysis;
    using System.Security;
    using System.Security.Permissions;

    /// <devdoc>
    ///     Useful methods to securely call 'dangerous' managed APIs (especially reflection).
    ///     See http://wiki/default.aspx/Microsoft.Projects.DotNetClient.SecurityConcernsAroundReflection
    ///     for more information specifically about why we need to be careful about reflection invocations.
    /// </devdoc>
    internal static class SecurityUtils {

        private static bool HasReflectionPermission {
            get {
                try {
                    (new ReflectionPermission(PermissionState.Unrestricted)).Demand();
                    return true;
                }
                catch (SecurityException) {
                }

                return false;
            }
        }

        /// <devdoc>
        ///     This helper method provides safe access to Activator.CreateInstance.
        ///     NOTE: This overload will work only with public .ctors. 
        /// </devdoc>
        internal static object SecureCreateInstance(Type type) {
            return SecureCreateInstance(type, null);
        }


        /// <devdoc>
        ///     This helper method provides safe access to Activator.CreateInstance.
        ///     NOTE: This overload will work only with public .ctors. 
        /// </devdoc>
        internal static object SecureCreateInstance(Type type, object[] args) {
            if (type == null) {
                throw new ArgumentNullException("type");
            }

            // The only case we need to worry about is when the type is in the same assembly
            // as us. In all other cases, reflection will take care of security.
            if (type.Assembly == typeof(SecurityUtils).Assembly && !(type.IsPublic || type.IsNestedPublic)) {
                (new ReflectionPermission(PermissionState.Unrestricted)).Demand();
            }

            return Activator.CreateInstance(type, args);
        }

        /// <devdoc>
        ///     This helper method provides safe access to Activator.CreateInstance.
        ///     Set allowNonPublic to true if you want non public ctors to be used. 
        /// </devdoc>
        internal static object SecureCreateInstance(Type type, object[] args, bool allowNonPublic) {
            if (type == null) {
                throw new ArgumentNullException("type");
            }

            BindingFlags flags = BindingFlags.Instance | BindingFlags.Public | BindingFlags.CreateInstance;

            // The only case we need to worry about is when the type is in the same assembly
            // as us. In all other cases, reflection will take care of security.
            if (type.Assembly == typeof(SecurityUtils).Assembly) {
                // if it's an internal type, we demand reflection permission.
                if (!(type.IsPublic || type.IsNestedPublic)) {
                    (new ReflectionPermission(PermissionState.Unrestricted)).Demand();
                }
                else if (allowNonPublic && !HasReflectionPermission) {
                    // Someone is trying to instantiate a public type in *our* assembly, but does not
                    // have full reflection permission. We shouldn't pass BindingFlags.NonPublic in this case.
                    // The reason we don't directly demand the permission here is because we don't know whether
                    // a public nr non-public .ctor will be invoked. We want to allow the public .ctor case to
                    // succeed.
                    allowNonPublic = false;
                }                
            }

            if (allowNonPublic) {
                flags |= BindingFlags.NonPublic;
            }

            return Activator.CreateInstance(type, flags, null, args, null);
        }

        /// <devdoc>
        ///     Helper method to safely invoke a .ctor. You should prefer SecureCreateInstance to this.
        ///     Set allowNonPublic to true if you want non public ctors to be used. 
        /// </devdoc>
        internal static object SecureConstructorInvoke(Type type, Type[] argTypes, object[] args, bool allowNonPublic) {
            return SecureConstructorInvoke(type, argTypes, args, allowNonPublic, BindingFlags.Default);
        }

        /// <devdoc>
        ///     Helper method to safely invoke a .ctor. You should prefer SecureCreateInstance to this.
        ///     Set allowNonPublic to true if you want non public ctors to be used. 
        ///     The 'extraFlags' parameter is used to pass in any other flags you need, 
        ///     besides Public, NonPublic and Instance.
        /// </devdoc>
        internal static object SecureConstructorInvoke(Type type, Type[] argTypes, object[] args, 
                                                       bool allowNonPublic, BindingFlags extraFlags) {
            if (type == null) {
                throw new ArgumentNullException("type");
            }

            BindingFlags flags = BindingFlags.Instance | BindingFlags.Public | extraFlags;
            
            // The only case we need to worry about is when the type is in the same assembly
            // as us. In all other cases, reflection will take care of security.
            if (type.Assembly == typeof(SecurityUtils).Assembly) {
                // if it's an internal type, we demand reflection permission.
                if (!(type.IsPublic || type.IsNestedPublic)) {
                    (new ReflectionPermission(PermissionState.Unrestricted)).Demand();
                }
                else if (allowNonPublic && !HasReflectionPermission) {
                    // Someone is trying to invoke a ctor on a public type in *our* assembly, but does not
                    // have full reflection permission. We shouldn't pass BindingFlags.NonPublic in this case.
                    allowNonPublic = false;
                }                
            }

            if (allowNonPublic) {
                flags |= BindingFlags.NonPublic;
            }

            ConstructorInfo ctor = type.GetConstructor(flags, null, argTypes, null);
            if (ctor != null) {
                return ctor.Invoke(args);
            }

            return null;
        }
     }
}
