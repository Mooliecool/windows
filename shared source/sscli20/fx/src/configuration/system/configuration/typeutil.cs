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
using System.Configuration.Internal;
using System.Reflection;
using System.Security;
using System.Security.Permissions;

namespace System.Configuration {

    internal static class TypeUtil {

        static private Type GetLegacyType(string typeString) {
            Type type = null;

            //
            // Ignore all exceptions, otherwise callers will get unexpected
            // exceptions not related to the original failure to load the
            // desired type.
            //
            try {
                Assembly systemAssembly = typeof(ConfigurationException).Assembly;
                type = systemAssembly.GetType(typeString, false);
            }
            catch {
            }

            return type;
        }

        //
        // Get the type specified by typeString. If it fails, try to retrieve it 
        // as a type from System.dll. If that fails,  return null or throw the original
        // exception as indicated by throwOnError.
        //
        static private Type GetTypeImpl(string typeString, bool throwOnError) {
            Type type = null;
            Exception originalException = null;

            try {
                type = Type.GetType(typeString, throwOnError);
            }
            catch (Exception e) {
                originalException = e;
            }

            if (type == null) {
                type = GetLegacyType(typeString);
                if (type == null && originalException != null) {
                    throw originalException;
                }
            }

            return type;
        }

        //
        // Ask the host to get the type specified by typeString. If it fails, try to retrieve it 
        // as a type from System.dll. If that fails, return null or throw the original
        // exception as indicated by throwOnError.
        //
        [ReflectionPermission(SecurityAction.Assert, Flags=ReflectionPermissionFlag.TypeInformation)]
        static internal Type GetTypeWithReflectionPermission(IInternalConfigHost host, string typeString, bool throwOnError) {
            Type type = null;
            Exception originalException = null;

            try {
                type = host.GetConfigType(typeString, throwOnError);
            }
            catch (Exception e) {
                originalException = e;
            }

            if (type == null) {
                type = GetLegacyType(typeString);
                if (type == null && originalException != null) {
                    throw originalException;
                }
            }

            return type;
        }

        [ReflectionPermission(SecurityAction.Assert, Flags=ReflectionPermissionFlag.TypeInformation)]
        static internal Type GetTypeWithReflectionPermission(string typeString, bool throwOnError) {
            return GetTypeImpl(typeString, throwOnError);
        }

        [ReflectionPermission(SecurityAction.Assert, Flags=ReflectionPermissionFlag.TypeInformation | ReflectionPermissionFlag.MemberAccess)]
        static internal object CreateInstanceWithReflectionPermission(string typeString) {
            Type type = GetTypeImpl(typeString, true); // catch the errors and report them
            object result = Activator.CreateInstance(type, true); // create non-public types
            return result;
        }

        [ReflectionPermission(SecurityAction.Assert, Flags=ReflectionPermissionFlag.TypeInformation | ReflectionPermissionFlag.MemberAccess)]
        static internal object CreateInstanceWithReflectionPermission(Type type) {
            object result = Activator.CreateInstance(type, true); // create non-public types
            return result;
        }

        [ReflectionPermission(SecurityAction.Assert, Flags=ReflectionPermissionFlag.TypeInformation)]
        static internal ConstructorInfo GetConstructorWithReflectionPermission(Type type, Type baseType, bool throwOnError) {
            type = VerifyAssignableType(baseType, type, throwOnError);
            if (type == null) {
                return null;
            }

            BindingFlags bindingFlags = BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic;

            ConstructorInfo ctor = type.GetConstructor(bindingFlags, null, CallingConventions.HasThis, Type.EmptyTypes, null);
            if (ctor == null && throwOnError) {
                throw new TypeLoadException(SR.GetString(SR.TypeNotPublic, type.AssemblyQualifiedName));
            }

            return ctor;
        }

        [ReflectionPermission(SecurityAction.Assert, Flags=ReflectionPermissionFlag.TypeInformation | ReflectionPermissionFlag.MemberAccess)]
        static internal object InvokeCtorWithReflectionPermission(ConstructorInfo ctor) {
            return ctor.Invoke(null);
        }

        static internal bool IsTypeFromTrustedAssemblyWithoutAptca(Type type) {
            Assembly assembly = type.Assembly;
            return assembly.GlobalAssemblyCache && !HasAptcaBit(assembly);
        }

        static internal Type VerifyAssignableType(Type baseType, Type type, bool throwOnError) {
            if (baseType.IsAssignableFrom(type)) {
                return type;
            }

            if (throwOnError) {
                throw new TypeLoadException(
                    SR.GetString(SR.Config_type_doesnt_inherit_from_type, type.FullName, baseType.FullName));
            }

            return null;
        }

        [ReflectionPermission(SecurityAction.Assert, Flags=ReflectionPermissionFlag.TypeInformation | ReflectionPermissionFlag.MemberAccess)]
        private static bool HasAptcaBit(Assembly assembly) {
            Object[] attrs = assembly.GetCustomAttributes(
                typeof(System.Security.AllowPartiallyTrustedCallersAttribute), /*inherit*/ false);

            return (attrs != null && attrs.Length > 0);
        }

        static private PermissionSet s_fullTrustPermissionSet;

        // Check if the caller is fully trusted
        static internal bool IsCallerFullTrust {
            get {
                bool isFullTrust = false;
                try {
                    if (s_fullTrustPermissionSet == null) {
                        s_fullTrustPermissionSet = new PermissionSet(PermissionState.Unrestricted);
                    }

                    s_fullTrustPermissionSet.Demand();
                    isFullTrust = true;
                }
                catch {
                }

                return isFullTrust;
            }
        }

        // Check if the type is allowed to be used in config by checking the APTCA bit
        internal static bool IsTypeAllowedInConfig(Type t) {
            // Note:
            // This code is copied from HttpRuntime.IsTypeAllowedInConfig, but modified in
            // how it checks for fulltrust this can be called from non-ASP.NET apps.
            
            // Allow everything in full trust
            if (IsCallerFullTrust) {
                return true;
            }
            
            Assembly assembly = t.Assembly;
            if (!assembly.GlobalAssemblyCache)
                return true;

            // If it has the APTCA bit, allow it
            if (HasAptcaBit(assembly))
                return true;
        
            // It's a GAC type without APTCA in partial trust scenario: block it
            return false;
        }
    }
}
