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
** Class:  ObjectHandle
**
**
** ObjectHandle wraps object references. A Handle allows a 
** marshal by value object to be returned through an 
** indirection allowing the caller to control when the
** object is loaded into their domain.
**
** 
===========================================================*/

namespace System.Runtime.Remoting{

    using System;
    using System.Security.Permissions;
    using System.Runtime.InteropServices;
    using System.Runtime.Remoting;
    using System.Runtime.Remoting.Activation;
    using System.Runtime.Remoting.Lifetime;

    [ClassInterface(ClassInterfaceType.AutoDual)]
[System.Runtime.InteropServices.ComVisible(true)]
    public class ObjectHandle: MarshalByRefObject, IObjectHandle 
    {
        private Object WrappedObject;
        
        private ObjectHandle()
        {
        }

        public ObjectHandle(Object o)
        {
            WrappedObject = o;
        }

        public Object Unwrap()
        {
            return WrappedObject;
        }

        // ObjectHandle has a finite lifetime. For now the default
        // lifetime is being used, this can be changed in this method to
        // specify a custom lifetime.
        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
        public override Object InitializeLifetimeService()
        {
            BCLDebug.Trace("REMOTE", "ObjectHandle.InitializeLifetimeService");

            //
            // If the wrapped object has implemented InitializeLifetimeService to return null,
            // we don't want to go to the base class (which will result in a lease being
            // requested from the MarshalByRefObject, which starts up the LeaseManager,
            // which starts up the ThreadPool, adding three threads to the process.
            // We check if the wrapped object is a MarshalByRef object, and call InitializeLifetimeServices on it
            // and if it returns null, we return null. Otherwise we fall back to the old behavior.
            //

            MarshalByRefObject mbr = WrappedObject as MarshalByRefObject;
            if (mbr != null) {
                Object o = mbr.InitializeLifetimeService();
                if (o == null)
                    return null;
            }
            ILease lease = (ILease)base.InitializeLifetimeService();
            return lease;
        }
    }
}
