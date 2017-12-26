// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
/*============================================================
**
** Purpose: Resource annotation rules.
**
===========================================================*/

using System;
using System.Diagnostics;
using Microsoft.Win32;

namespace System.Runtime.Versioning
{
    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Property | AttributeTargets.Constructor, Inherited = false)]
    [Conditional("RESOURCE_ANNOTATION_WORK")]
    public sealed class ResourceConsumptionAttribute : Attribute
	{
        private ResourceScope _consumptionScope;
        private ResourceScope _resourceScope;

        public ResourceConsumptionAttribute(ResourceScope resourceScope)
        {
            _resourceScope = resourceScope;
            _consumptionScope = _resourceScope;
        }

        public ResourceConsumptionAttribute(ResourceScope resourceScope, ResourceScope consumptionScope)
        {
            _resourceScope = resourceScope;
            _consumptionScope = consumptionScope;
        }

        public ResourceScope ResourceScope {
            get { return _resourceScope; }
        }

        public ResourceScope ConsumptionScope {
            get { return _consumptionScope; }
        }
    }

    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Field | AttributeTargets.Property | AttributeTargets.Constructor, Inherited = false)]
    [Conditional("RESOURCE_ANNOTATION_WORK")]
    public sealed class ResourceExposureAttribute : Attribute
    {
        private ResourceScope _resourceExposureLevel;

        public ResourceExposureAttribute(ResourceScope exposureLevel)
        {
            _resourceExposureLevel = exposureLevel;
        }

        public ResourceScope ResourceExposureLevel {
            get { return _resourceExposureLevel; }
        }
    }


    // Default visibility is Public, which isn't specified in this enum.
    // Public == the lack of Private or Assembly
    // Does this actually work?  Need to investigate that.
    [Flags]
    public enum ResourceScope
    {
        None = 0,
        // Resource type
        Machine   = 0x1,
        Process   = 0x2,
        AppDomain = 0x4,
        Library   = 0x8,
        // Visibility
        Private  = 0x10,  // Private to this one class.
        Assembly = 0x20,  // Assembly-level, like C#'s "internal"
    }


    [Flags]
    internal enum SxSRequirements
    {
        None = 0,
        AppDomainID = 0x1,
        ProcessID = 0x2,
        AssemblyName = 0x4,
        TypeName = 0x8
    }

    public static class VersioningHelper
    {
        // These depend on the exact values given to members of the ResourceScope enum.
        private static ResourceScope ResTypeMask = ResourceScope.Machine | ResourceScope.Process | ResourceScope.AppDomain | ResourceScope.Library;
        private static ResourceScope VisibilityMask = ResourceScope.Private | ResourceScope.Assembly;

        public static String MakeVersionSafeName(String name, ResourceScope from, ResourceScope to)
        {
            return MakeVersionSafeName(name, from, to, null);
        }

        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Process, ResourceScope.Process)]
        public static String MakeVersionSafeName(String name, ResourceScope from, ResourceScope to, Type type)
        {
            ResourceScope fromResType = from & ResTypeMask;
            ResourceScope toResType = to & ResTypeMask;
            if (fromResType > toResType)
                throw new ArgumentException(Environment.GetResourceString("Argument_ResourceScopeWrongDirection", fromResType, toResType), "from");

            SxSRequirements requires = GetRequirements(to, from);

            if ((requires & (SxSRequirements.AssemblyName | SxSRequirements.TypeName)) != 0 && type == null)
                throw new ArgumentNullException("type", Environment.GetResourceString("ArgumentNull_TypeRequiredByResourceScope"));

            String postfix = "";
            const String separator = "_";
            if ((requires & SxSRequirements.ProcessID) != 0)
                postfix += separator + Win32Native.GetCurrentProcessId();
            if ((requires & SxSRequirements.AppDomainID) != 0)
                postfix += separator + AppDomain.CurrentDomain.GetAppDomainId();
            if ((requires & SxSRequirements.TypeName) != 0)
                postfix += separator + type.Name;
            if ((requires & SxSRequirements.AssemblyName) != 0)
                postfix += separator + type.Assembly.FullName;

            return name + postfix;
        }

        private static SxSRequirements GetRequirements(ResourceScope consumeAsScope, ResourceScope calleeScope)
        {
            SxSRequirements requires = SxSRequirements.None;
        
            switch(calleeScope & ResTypeMask) {
            case ResourceScope.Machine:
                switch(consumeAsScope & ResTypeMask) {
                case ResourceScope.Machine:
                    // No work
                    break;

                case ResourceScope.Process:
                    requires |= SxSRequirements.ProcessID;
                    break;

                case ResourceScope.AppDomain:
                    requires |= SxSRequirements.AppDomainID | SxSRequirements.ProcessID;
                    break;

                default:
                    throw new ArgumentException(Environment.GetResourceString("Argument_BadResourceScopeTypeBits", consumeAsScope), "consumeAsScope");
                }
                break;
            
            case ResourceScope.Process:
                if ((consumeAsScope & ResourceScope.AppDomain) != 0)
                    requires |= SxSRequirements.AppDomainID;
                break;

            case ResourceScope.AppDomain:
                // No work
                break;

            default:
                throw new ArgumentException(Environment.GetResourceString("Argument_BadResourceScopeTypeBits", calleeScope), "calleeScope");
            }

            switch(calleeScope & VisibilityMask) {
            case ResourceScope.None:  // Public - implied
                switch(consumeAsScope & VisibilityMask) {
                case ResourceScope.None:  // Public - implied
                    // No work
                    break;

                case ResourceScope.Assembly:
                    requires |= SxSRequirements.AssemblyName;
                    break;

                case ResourceScope.Private:
                    requires |= SxSRequirements.TypeName | SxSRequirements.AssemblyName;
                    break;

                default:
                    throw new ArgumentException(Environment.GetResourceString("Argument_BadResourceScopeVisibilityBits", consumeAsScope), "consumeAsScope");
                }
                break;

            case ResourceScope.Assembly:
                if ((consumeAsScope & ResourceScope.Private) != 0)
                    requires |= SxSRequirements.TypeName;
                break;

            case ResourceScope.Private:
                // No work
                break;

            default:
                throw new ArgumentException(Environment.GetResourceString("Argument_BadResourceScopeVisibilityBits", calleeScope), "calleeScope");
            }

            if (consumeAsScope == calleeScope) {
                BCLDebug.Assert(requires == SxSRequirements.None, "Computed a strange set of required resource scoping.  It's probably wrong.");
            }

            return requires;
        }
    }
}
