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
// IPermission.cs
//
// Defines the interface that all Permission objects must support.
// 

namespace System.Security
{

[System.Runtime.InteropServices.ComVisible(true)]
    public interface IPermission : ISecurityEncodable
    {
        // NOTE: The constants that used to be defined here were moved to 
        // PermissionsEnum.cs due to CLS restrictions.

        // The integrity of the security system depends on a means to
        // copy objects so that references to sensitive objects are not
        // exposed outside of the runtime. Thus, all permissions must
        // implement Copy.
        // 
        // Makes an exact copy of the Permission.
        IPermission Copy();

        /*
         * Methods to support the Installation, Registration, others... PolicyEngine
         */
    
        // 
        // 
        IPermission Intersect(IPermission target);

        // The runtime policy manager also requires a means of combining the
        // state contained within two permissions of the same type in a logical OR
        // construct.  (The Union of two permission of different type is not defined, 
        // except when one of the two is a CompoundPermission of internal type equal
        // to the type of the other permission.)
        //

        IPermission Union(IPermission target);

        // IsSubsetOf defines a standard mechanism for determining
        // relative safety between two permission demands of the same type.
        // If one demand x demands no more than some other demand y, then
        // x.IsSubsetOf(y) should return true. In this case, if the
        // demand for y is satisfied, then it is possible to assume that
        // the demand for x would also be satisfied under the same
        // circumstances. On the other hand, if x demands something that y
        // does not, then x.IsSubsetOf(y) should return false; the fact
        // that x is satisfied by the current security context does not
        // also imply that the demand for y will also be satisfied.
        // 
        // Returns true if 'this' Permission allows no more access than the
        // argument.
        // 
        bool IsSubsetOf(IPermission target);

        // The Demand method is the fundamental part of the IPermission
        // interface from a component developer's perspective. The
        // permission represents the demand that the developer wants
        // satisfied, and Demand is the means to invoke the demand.
        // For each type of permission, the mechanism to verify the
        // demand will be different. However, to the developer, all
        // permissions invoke that mechanism through the Demand interface.
        // Mark this method as requiring a security object on the caller's frame
        // so the caller won't be inlined (which would mess up stack crawling).
        [DynamicSecurityMethodAttribute()]
        void Demand();

    }
}
