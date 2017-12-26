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
//+----------------------------------------------------------------------------
//
// File:        ClientSponsor.cs
//
// Contents:    Agent for keeping Server Object's lifetime in sync with a client's lifetime
//
// History:     8/9/00                                 Created
//
//+----------------------------------------------------------------------------

namespace System.Runtime.Remoting.Lifetime
{
    using System;
    using System.Collections;
    using System.Security.Permissions;

    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
    [SecurityPermissionAttribute(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.Infrastructure)]	
[System.Runtime.InteropServices.ComVisible(true)]
    public class ClientSponsor : MarshalByRefObject, ISponsor
    {
		private Hashtable sponsorTable = new Hashtable(10);
		private TimeSpan m_renewalTime = TimeSpan.FromMinutes(2);

		public ClientSponsor()
		{
		}

		public ClientSponsor(TimeSpan renewalTime)
		{
			this.m_renewalTime = renewalTime;
		}

		public TimeSpan RenewalTime
		{
			get{ return m_renewalTime;}
			set{ m_renewalTime = value;}
		}
			
		public bool Register(MarshalByRefObject obj)
		{
			BCLDebug.Trace("REMOTE", "ClientSponsor Register "+obj);
			ILease lease = (ILease)obj.GetLifetimeService();
			if (lease == null)
				return false;

			lease.Register(this);
			lock(sponsorTable)
			{
				sponsorTable[obj] = lease;
			}
			return true;
		}

		public void Unregister(MarshalByRefObject obj)
		{
			BCLDebug.Trace("REMOTE", "ClientSponsor Unregister "+obj);

			ILease lease = null;
			lock(sponsorTable)
			{
				lease = (ILease)sponsorTable[obj];
			}
			if (lease != null)
				lease.Unregister(this);
		}

		// ISponsor method
		public TimeSpan Renewal(ILease lease)
		{
			BCLDebug.Trace("REMOTE", "ClientSponsor Renewal "+m_renewalTime);
			return m_renewalTime;
		}

		public void Close()
		{
			BCLDebug.Trace("REMOTE","ClientSponsor Close");
			lock(sponsorTable)
			{
				IDictionaryEnumerator e = sponsorTable.GetEnumerator();
				while(e.MoveNext())
					((ILease)e.Value).Unregister(this);
				sponsorTable.Clear();
			}
		}

		// Don't create a lease on the sponsor
		public override Object InitializeLifetimeService()
		{
			return null;
		}

        ~ClientSponsor()
		{
			BCLDebug.Trace("REMOTE","ClientSponsor Finalize");
		}
    }
}
