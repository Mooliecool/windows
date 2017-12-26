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

//
//  Evidence.cs
//
//  Use this class to keep a list of evidence associated with an Assembly
//

namespace System.Security.Policy {
 
    using System;
    using System.Collections;
    using System.IO;
    using System.Configuration.Assemblies;
    using System.Reflection;
    using System.Runtime.InteropServices;
    using System.Runtime.Remoting;
    using System.Runtime.Serialization;
    using System.Security.Util;
    using System.Security.Permissions;
    using System.Runtime.Serialization.Formatters.Binary;
    
    [Serializable]
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class Evidence : ICollection
    {
        private IList m_hostList;
        private IList m_assemblyList;
        private bool m_locked;

        public Evidence()
        {
            m_hostList = null;
            m_assemblyList = null;
            m_locked = false;
        }

        public Evidence(Evidence evidence)
        {
            if (evidence == null)
                return;

            m_locked = false;
            Merge( evidence );
        }

        public Evidence( Object[] hostEvidence, Object[] assemblyEvidence )
        {
            m_locked = false;

            if (hostEvidence != null)
            {
                this.m_hostList = ArrayList.Synchronized( new ArrayList( hostEvidence ) );
            }

            if (assemblyEvidence != null)
            {
                this.m_assemblyList = ArrayList.Synchronized( new ArrayList( assemblyEvidence ) );
            }
        }

        internal Evidence(char[] buffer)
        {
            int position = 0;
            while (position < buffer.Length)
            {
                switch (buffer[position++])
                {
                    case BuiltInEvidenceHelper.idApplicationDirectory:
                    {
                        IBuiltInEvidence ad = new ApplicationDirectory();
                        position = ad.InitFromBuffer(buffer, position);
                        AddAssembly(ad);
                        break;
                    }
                    case BuiltInEvidenceHelper.idStrongName:
                    {
                        IBuiltInEvidence sn = new StrongName();
                        position = sn.InitFromBuffer(buffer, position);
                        AddHost(sn);
                        break;
                    }
                    case BuiltInEvidenceHelper.idZone:
                    {
                        IBuiltInEvidence z = new Zone();
                        position = z.InitFromBuffer(buffer, position);
                        AddHost(z);
                        break;
                    }
                    case BuiltInEvidenceHelper.idUrl:
                    {
                        IBuiltInEvidence u = new Url();
                        position = u.InitFromBuffer(buffer, position);
                        AddHost(u);
                        break;
                    }
                    case BuiltInEvidenceHelper.idSite:
                    {
                        IBuiltInEvidence s = new Site();
                        position = s.InitFromBuffer(buffer, position);
                        AddHost(s);
                        break;
                    }
                    case BuiltInEvidenceHelper.idPermissionRequestEvidence:
                    {
                        IBuiltInEvidence pre = new PermissionRequestEvidence();
                        position = pre.InitFromBuffer(buffer, position);
                        AddHost(pre);
                        break;
                    }
                    case BuiltInEvidenceHelper.idGac:
                    {
                        IBuiltInEvidence g = new GacInstalled();
                        position = g.InitFromBuffer(buffer, position);
                        AddHost(g);
                        break;
                    }
                    default:
                        throw new SerializationException(Environment.GetResourceString("Serialization_UnableToFixup"));
                } // switch
            } // while
        }

        public void AddHost(Object id)
        {
            if (m_hostList == null)
                m_hostList = ArrayList.Synchronized( new ArrayList() );

            if (m_locked)
                new SecurityPermission( SecurityPermissionFlag.ControlEvidence ).Demand();

            m_hostList.Add( id );
        }

        public void AddAssembly(Object id)
        {
            if (m_assemblyList == null)
                m_assemblyList = ArrayList.Synchronized( new ArrayList() );
                
            m_assemblyList.Add( id );
        }

        public bool Locked
        {
            get
            {
                return m_locked;
            }

            set
            {
                if (!value)
                {
                    new SecurityPermission( SecurityPermissionFlag.ControlEvidence ).Demand();

                    m_locked = false;
                }
                else
                {
                    m_locked = true;
                }
            }
        }
        
        public void Merge( Evidence evidence )
        {
            IEnumerator enumerator;

            if (evidence == null)
                return;

            if (evidence.m_hostList != null)
            {
                if (m_hostList == null)
                    m_hostList = ArrayList.Synchronized( new ArrayList() );

                if (evidence.m_hostList.Count != 0 && this.m_locked)
                    new SecurityPermission( SecurityPermissionFlag.ControlEvidence ).Demand();

                enumerator = evidence.m_hostList.GetEnumerator();
                while (enumerator.MoveNext())
                {
                    m_hostList.Add( enumerator.Current );
                }
            }

            if (evidence.m_assemblyList != null)
            {
                if (m_assemblyList == null)
                    m_assemblyList = ArrayList.Synchronized( new ArrayList() );

                enumerator = evidence.m_assemblyList.GetEnumerator();
                while (enumerator.MoveNext())
                {
                    m_assemblyList.Add( enumerator.Current );
                }
            }
        }

        // Same as merge, except only one instance of any one evidence type is
        // allowed. When duplicates are found, the evidence in the input
        // argument will have priority.
        internal void MergeWithNoDuplicates( Evidence evidence )
        {
            IEnumerator oldEnumerator, newEnumerator;

            if (evidence == null)
                return;

            if (evidence.m_hostList != null)
            {
                if (m_hostList == null)
                    m_hostList = ArrayList.Synchronized( new ArrayList() );

                newEnumerator = evidence.m_hostList.GetEnumerator();
                while (newEnumerator.MoveNext())
                {
                    Type newItemType = newEnumerator.Current.GetType();
                    oldEnumerator = m_hostList.GetEnumerator();
                    while (oldEnumerator.MoveNext())
                    {
                        if (oldEnumerator.Current.GetType() == newItemType)
                        {
                            m_hostList.Remove(oldEnumerator.Current);
                            break;
                        }
                    }
                    m_hostList.Add( newEnumerator.Current );
                }
            }

            if (evidence.m_assemblyList != null)
            {
                if (m_assemblyList == null)
                    m_assemblyList = ArrayList.Synchronized( new ArrayList() );

                newEnumerator = evidence.m_assemblyList.GetEnumerator();
                while (newEnumerator.MoveNext())
                {
                    Type newItemType = newEnumerator.Current.GetType();
                    oldEnumerator = m_assemblyList.GetEnumerator();
                    while (oldEnumerator.MoveNext())
                    {
                        if (oldEnumerator.Current.GetType() == newItemType)
                        {
                            m_assemblyList.Remove(oldEnumerator.Current);
                            break;
                        }
                    }
                    m_assemblyList.Add( newEnumerator.Current );
                }
            }
        }

        // ICollection implementation
        public void CopyTo(Array array, int index)
        {
            int currentIndex = index;

            if (m_hostList != null)
            {
                m_hostList.CopyTo( array, currentIndex );
                currentIndex += m_hostList.Count;
            }

            if (m_assemblyList != null)
            {
                m_assemblyList.CopyTo( array, currentIndex );
            }
        }

        public IEnumerator GetHostEnumerator()
        {
            if  (m_hostList == null)
                m_hostList = ArrayList.Synchronized( new ArrayList() );

            return m_hostList.GetEnumerator();
        }

        public IEnumerator GetAssemblyEnumerator()
        {
            if (m_assemblyList == null)
                m_assemblyList = ArrayList.Synchronized( new ArrayList() );
                
            return m_assemblyList.GetEnumerator();
        }

        public IEnumerator GetEnumerator()
        {
            return new EvidenceEnumerator( this );
        }

        public int Count
        {
            get { return (m_hostList != null ? m_hostList.Count : 0) + (m_assemblyList != null ? m_assemblyList.Count : 0); }
        }

        public Object SyncRoot
        {
            get { return this; }
        }

        public bool IsSynchronized
        {
            get { return false; }
        }

        public bool IsReadOnly
        {
            get { return false; }
        }

        internal Evidence Copy()
        {
        
            // Try the fast way. Will only work if all evidence components implement IBuiltInEvidence
            char[] serEvidence = PolicyManager.MakeEvidenceArray(this, true);
            if (serEvidence != null)
            {
                return new Evidence(serEvidence);
            }

            // Use the general purpose formatter
            new PermissionSet( true ).Assert();

            MemoryStream stream = new MemoryStream();

            BinaryFormatter formatter = new BinaryFormatter();

            formatter.Serialize( stream, this );

            stream.Position = 0;

            return (Evidence)formatter.Deserialize( stream );
        }

        internal Evidence ShallowCopy()
        {
            Evidence evidence = new Evidence();

            IEnumerator enumerator;

            enumerator = this.GetHostEnumerator();

            while (enumerator.MoveNext())
            {
                evidence.AddHost( enumerator.Current );
            }

            enumerator = this.GetAssemblyEnumerator();

            while (enumerator.MoveNext())
            {
                evidence.AddAssembly( enumerator.Current );
            }

            return evidence;
        }

        [ComVisible(false)]
        public void Clear()
        {
            m_hostList = null;
            m_assemblyList = null;
        }

        [ComVisible(false)]
        public void RemoveType(Type t)
        {
            int i;
            for(i = 0; i < (m_hostList == null ? 0 : m_hostList.Count); i++)
            {
                if (m_hostList[i].GetType() == t)
                    m_hostList.RemoveAt(i--);
            }
            for(i = 0; i < (m_assemblyList == null ? 0 : m_assemblyList.Count); i++)
            {
                if (m_assemblyList[i].GetType() == t)
                    m_assemblyList.RemoveAt(i--);
            }
        }

        [ComVisible(false)]
        public override bool Equals( Object obj )
        {
            Evidence other = obj as Evidence;

            if (other == null)
                return false;

            if (this.m_hostList != null && other.m_hostList != null)
            {
                if (this.m_hostList.Count != other.m_hostList.Count)
                    return false;

                int count = this.m_hostList.Count;
                for (int i = 0; i < count; ++i)
                {
                    bool found = false;

                    for (int j = 0; j < count; ++j)
                    {
                        if (Object.Equals( this.m_hostList[i], other.m_hostList[j] ))
                        {
                            found = true;
                            break;
                        }
                    }

                    if (!found)
                        return false;
                }
            }
            else if (this.m_hostList != null || other.m_hostList != null)
            {
                return false;
            }

            if (this.m_assemblyList != null && other.m_assemblyList != null)
            {
                if (this.m_assemblyList.Count != other.m_assemblyList.Count)
                    return false;

                int count = this.m_assemblyList.Count;
                for (int i = 0; i < count; ++i)
                {
                    bool found = false;

                    for (int j = 0; j < count; ++j)
                    {
                        if (Object.Equals( m_assemblyList[i], other.m_assemblyList[j] ))
                        {
                            found = true;
                            break;
                        }
                    }

                    if (!found)
                        return false;
                }
            }
            else if (this.m_assemblyList != null || other.m_assemblyList != null)
            {
                return false;
            }

            return true;
        }

        [ComVisible(false)]
        public override int GetHashCode()
        {
            int accumulator = 0;

            if (this.m_hostList != null)
            {
                int count = this.m_hostList.Count;
                for (int i = 0; i < count; ++i)
                {
                    accumulator = accumulator ^ this.m_hostList[i].GetHashCode();
                }
            }

            if (this.m_assemblyList != null)
            {
                int count = this.m_assemblyList.Count;
                for (int i = 0; i < count; ++i)
                {
                    accumulator = accumulator ^ this.m_assemblyList[i].GetHashCode();
                }
            }

            return accumulator;
        }

        internal Object FindType(Type t)
        {
            int i;
            for(i = 0; i < (m_hostList == null ? 0 : m_hostList.Count); i++)
            {
                if (m_hostList[i].GetType() == t)
                    return m_hostList[i];
            }
            for(i = 0; i < (m_assemblyList == null ? 0 : m_assemblyList.Count); i++)
            {
                if (m_assemblyList[i].GetType() == t)
                    return m_hostList[i];
            }
            return null;
        }
    }
    
    sealed class EvidenceEnumerator : IEnumerator
    {
        private bool m_first;
        private Evidence m_evidence;
        private IEnumerator m_enumerator;

        public EvidenceEnumerator( Evidence evidence )
        {
            this.m_evidence = evidence;
            Reset();
        }

        public bool MoveNext()
        {
            if (m_enumerator == null)
            {
                return false;
            }

            if (!m_enumerator.MoveNext())
            {
                if (m_first)
                {
                    m_enumerator = m_evidence.GetAssemblyEnumerator();
                    m_first = false;
                    if (m_enumerator != null)
                        return m_enumerator.MoveNext();
                    else
                        return false;
                }
                else
                {
                    return false;
                }
            }
            return true;
        }

        public Object Current 
        {
            get {
                if (m_enumerator == null)
                    return null;
                else
                    return m_enumerator.Current;
            }
        }

        public void Reset() {
            this.m_first = true;

            if (m_evidence != null)
            {
                m_enumerator = m_evidence.GetHostEnumerator();
            }
            else
            {
                m_enumerator = null;
            }
        }
    }
}
