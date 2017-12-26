//------------------------------------------------------------------------------
// <copyright file="IPAddressCollection.cs" company="Microsoft">
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

using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace System.Net.NetworkInformation{
    public class IPAddressCollection :ICollection<IPAddress>
    {
        Collection<IPAddress> addresses = new Collection<IPAddress>();

        protected internal IPAddressCollection(){
        }

        /// <include file='doc\HttpListenerPrefixCollection.uex' path='docs/doc[@for="HttpListenerPrefixCollection.CopyTo"]/*' />
        public virtual void CopyTo(IPAddress[] array, int offset) {
            addresses.CopyTo(array,offset);
        }

        
        /// <include file='doc\HttpListenerPrefixCollection.uex' path='docs/doc[@for="HttpListenerPrefixCollection.Count"]/*' />
        public virtual int Count {
            get {
                return addresses.Count;
            }
        }
        
        public virtual bool IsReadOnly {
            get {
                return true;
            }
        }

        public virtual void Add(IPAddress address) {
                throw new NotSupportedException(SR.GetString(SR.net_collection_readonly));
        }


        internal void InternalAdd(IPAddress address) {
            addresses.Add(address);
        }


        /// <include file='doc\HttpListenerPrefixCollection.uex' path='docs/doc[@for="HttpListenerPrefixCollection.Contains"]/*' />
        public virtual bool Contains(IPAddress address) {
            return addresses.Contains(address);
        }


        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator() {
	    return null;	
        }

        public virtual IEnumerator<IPAddress> GetEnumerator() {
            return (IEnumerator<IPAddress>) addresses.GetEnumerator();
        }


        public virtual IPAddress this[int index]
        {
            get{
                return (IPAddress)addresses[index];
            }
        }


        /// <include file='doc\HttpListenerPrefixCollection.uex' path='docs/doc[@for="HttpListenerPrefixCollection.Remove"]/*' />
        public virtual bool Remove(IPAddress address) {
            throw new NotSupportedException(SR.GetString(SR.net_collection_readonly));
        }

        /// <include file='doc\HttpListenerPrefixCollection.uex' path='docs/doc[@for="HttpListenerPrefixCollection.Clear"]/*' />
        public virtual void Clear() {
            throw new NotSupportedException(SR.GetString(SR.net_collection_readonly));
        }
    }
}
