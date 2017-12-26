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
// TokenBasedSet.cs
//

namespace System.Security.Util
{
    using System;
    using System.Collections;
    using System.Security.Permissions;
    using System.Runtime.Serialization;

    [Serializable()] internal class TokenBasedSet
    {


        // Following 3 fields are used only for serialization compat purposes: DO NOT USE THESE EVER!
#pragma warning disable 414        
        private int      m_initSize = 24;
        private int      m_increment = 8;
#pragma warning restore 414        
        private Object[] m_objSet;
        //  END -> Serialization only fields

        [OptionalField(VersionAdded = 2)]        
        private Object m_Obj;
        [OptionalField(VersionAdded = 2)]        
        private Object[] m_Set;
        
        private int m_cElt;
        private int m_maxIndex;


        [OnDeserialized]
        private void OnDeserialized(StreamingContext ctx)
        {
            OnDeserializedInternal();
        }
        private void OnDeserializedInternal()
        {
            if (m_objSet != null) //v1.x case 
            {
                if (m_cElt == 1)
                    m_Obj = m_objSet[m_maxIndex];
                else
                    m_Set = m_objSet;
                m_objSet = null;
            }
            // Nothing to do for the v2.0 and beyond case
        }

        [OnSerializing]
        private void OnSerializing(StreamingContext ctx)
        {

            if ((ctx.State & ~(StreamingContextStates.Clone|StreamingContextStates.CrossAppDomain)) != 0)
            {
                //Nothing special for the v2 and beyond case
                
                // for the v1.x case, we need to create m_objSet if necessary
                if (m_cElt == 1)
                {
                    m_objSet = new Object[m_maxIndex+1];
                    m_objSet[m_maxIndex] = m_Obj;
                }
                else if (m_cElt > 0)
                {
                    // Array case:
                    m_objSet = m_Set;
                }
                
            }
        }   
        [OnSerialized]
        private void OnSerialized(StreamingContext ctx)
        {
            if ((ctx.State & ~(StreamingContextStates.Clone|StreamingContextStates.CrossAppDomain)) != 0)
            {
                m_objSet = null;
                
            }
        }


        internal bool MoveNext(ref TokenBasedSetEnumerator e)
        {
            switch (m_cElt)
            {
            case 0:
                return false;

            case 1:
                if (e.Index == -1)
                {
                    e.Index = m_maxIndex;
                    e.Current = m_Obj;
                    return true;
                }
                else
                {
                    e.Index = (short)(m_maxIndex+1);
                    e.Current = null;
                    return false;
                }

            default:
                while (++e.Index <= m_maxIndex)
                {
                    e.Current = m_Set[e.Index];
                    
                    if (e.Current != null)
                        return true;
                }

                e.Current = null;
                return false;
            }
        }

        internal TokenBasedSet()
        {
            Reset();
        }

        internal TokenBasedSet(TokenBasedSet tbSet)
        {
            if (tbSet == null)
            {
                Reset();
                return;
            }

            if (tbSet.m_cElt > 1)
            {
                Object[] aObj = tbSet.m_Set;
                int aLen = aObj.Length;
                
                Object[] aNew = new Object[aLen];
                System.Array.Copy(aObj, 0, aNew, 0, aLen);
                
                m_Set = aNew;
            }
            else
            {
                m_Obj = tbSet.m_Obj;
            }

            m_cElt      = tbSet.m_cElt;
            m_maxIndex  = tbSet.m_maxIndex;
        }

        internal void Reset()
        {
            m_Obj = null;
            m_Set = null;
            m_cElt = 0;
            m_maxIndex = -1;
        }

        internal void SetItem(int index, Object item)
        {
            Object[] aObj = null;

            if (item == null)
            {
                RemoveItem(index);
                return;
            }

            switch (m_cElt)
            {
            case 0:
                // on the first item, we don't create an array, we merely remember it's index and value
                // this this the 99% case
                m_cElt = 1;
                m_maxIndex = (short)index;
                m_Obj = item;
                break;

            case 1:
                // we have to decide if a 2nd item has indeed been added and create the array
                // if it has
                if (index == m_maxIndex)
                {
                    // replacing the one existing item
                    m_Obj = item;
                 }
                else
                {
                    // adding a second distinct permission
                    Object objSaved = m_Obj;
                    int iMax = Math.Max(m_maxIndex, index);
                    
                    aObj = new Object[iMax+1];
                    aObj[m_maxIndex] = objSaved;
                    aObj[index] = item;
                    m_maxIndex = (short)iMax;
                    m_cElt = 2;
                    m_Set = aObj;
                    m_Obj = null;
                }
                break;

            default:
                // this is the general case code for when there is really an array

                aObj = m_Set;

                // we are now adding an item, check if we need to grow

                if (index >= aObj.Length)
                {
                    Object[] newset = new Object[index+1];
                    System.Array.Copy(aObj, 0, newset, 0, m_maxIndex+1);
                    m_maxIndex = (short)index;
                    newset[index] = item;
                    m_Set = newset;
                    m_cElt++;
                }
                else
                {
                    if (aObj[index] == null)
                        m_cElt++;

                    aObj[index] = item;

                    if (index > m_maxIndex)
                        m_maxIndex = (short)index;
                }
                break;
            }
        }

        internal Object GetItem(int index)
        {
            switch (m_cElt)
            {
            case 0:
                return null;

            case 1:
                if (index == m_maxIndex)
                    return m_Obj;
                else
                    return null;
            default:
                if (index < m_Set.Length)
                    return m_Set[index];
                else
                    return null;
            }
        }

        internal Object RemoveItem(int index)
        {
            Object ret = null;

            switch (m_cElt)
            {
            case 0:
                ret = null;
                break;

            case 1:
                if (index != m_maxIndex)
                {
                    // removing a permission we don't have ignore it
                    ret = null;
                }
                else 
                {
                    // removing the permission we have at the moment
                    ret = m_Obj;
                    Reset();
                }
                break;

            default:
                // this is the general case code for when there is really an array

                // we are removing an item                
                if (index < m_Set.Length && m_Set[index] != null)
                {
                    // ok we really deleted something at this point

                    ret = m_Set[index];

                    m_Set[index] = null;
                    m_cElt--;

                    if (index == m_maxIndex)
                        ResetMaxIndex(m_Set);

                    // collapse the array
                    if (m_cElt == 1)
                    {
                        m_Obj = m_Set[m_maxIndex];
                        m_Set = null;
                    }
                }
                break;
            }

            return ret;
        }

        private void ResetMaxIndex(Object[] aObj)
        {
            int i;

            // Start at the end of the array, and
            // scan backwards for the first non-null
            // slot. That is the new maxIndex.
            for (i = aObj.Length - 1; i >= 0; i--)
            {
                if (aObj[i] != null)
                {
                    m_maxIndex = (short)i;
                    return;
                }
            }

            m_maxIndex = -1;
        }
        internal int GetStartingIndex()
        {
            if (m_cElt <= 1)
                return m_maxIndex;
            return 0;
        }
        internal int GetCount()
        {
            return m_cElt;
        }

        internal int GetMaxUsedIndex()
        {
            return m_maxIndex;
        }

        internal bool FastIsEmpty()
        {
            return m_cElt == 0;
        }

        // Used to merge two distinct TokenBasedSets (used currently only in PermissionSet Deserialization)
        internal TokenBasedSet SpecialUnion(TokenBasedSet other, ref bool canUnrestrictedOverride)
        {
            // This gets called from PermissionSet.OnDeserialized and it's possible that the TokenBasedSets have 
            // not been subjected to VTS callbacks yet
            OnDeserializedInternal();
            TokenBasedSet unionSet = new TokenBasedSet();
            int maxMax;
            if (other != null)
            {
                other.OnDeserializedInternal();
                maxMax = this.GetMaxUsedIndex() > other.GetMaxUsedIndex() ? this.GetMaxUsedIndex() : other.GetMaxUsedIndex();
            }
            else
                maxMax = this.GetMaxUsedIndex();
        
            for (int i = 0; i <= maxMax; ++i)
            {
                Object thisObj = this.GetItem( i );
                IPermission thisPerm = thisObj as IPermission;
                ISecurityElementFactory thisElem = thisObj as ISecurityElementFactory;
                
                Object otherObj = (other != null)?other.GetItem( i ):null;
                IPermission otherPerm = otherObj as IPermission;
                ISecurityElementFactory otherElem = otherObj as ISecurityElementFactory;
        
                if (thisObj == null && otherObj == null)
                    continue;
        
             
                if (thisObj == null)
                {
                    if (otherElem != null)
                    {
                        otherPerm = PermissionSet.CreatePerm(otherElem, false);
                    }
                    
                    
                    

                    PermissionToken token = PermissionToken.GetToken(otherPerm);
                    
                    if (token == null)
                    {
                        throw new SerializationException(Environment.GetResourceString("Serialization_InsufficientState"));
                    }
                    
                    unionSet.SetItem(token.m_index, otherPerm);
                    if (!CodeAccessPermission.CanUnrestrictedOverride(otherPerm))
                        canUnrestrictedOverride = false;
                }
                else if (otherObj == null)
                {
                    if (thisElem != null)
                    {
                        thisPerm = PermissionSet.CreatePerm(thisElem, false);
                    }
                    PermissionToken token = PermissionToken.GetToken(thisPerm);
                    if (token == null)
                    {
                        throw new SerializationException(Environment.GetResourceString("Serialization_InsufficientState"));
                    }
                    unionSet.SetItem( token.m_index, thisPerm);
                    if (!CodeAccessPermission.CanUnrestrictedOverride(thisPerm))
                        canUnrestrictedOverride = false;

                }
                else
                {
                    BCLDebug.Assert( (thisObj == null || otherObj == null), "Permission cannot be in both TokenBasedSets" );
                }
            }
            return unionSet;
        }
        internal void SpecialSplit(ref TokenBasedSet unrestrictedPermSet, ref TokenBasedSet normalPermSet, bool ignoreTypeLoadFailures)
        {
           int maxIndex = GetMaxUsedIndex();

            for (int i = GetStartingIndex(); i <= maxIndex; ++i)
            {
                Object obj = GetItem( i );
                if (obj != null)
                {
                    IPermission perm = obj as IPermission;
                    if (perm == null)
                        perm = PermissionSet.CreatePerm(obj, ignoreTypeLoadFailures);
                    PermissionToken token = PermissionToken.GetToken(perm);

                    if (perm == null || token == null)
                        continue;

                    if (perm is IUnrestrictedPermission)
                    {
                        // Add to unrestrictedPermSet
                        if (unrestrictedPermSet == null)
                            unrestrictedPermSet = new TokenBasedSet();
                        unrestrictedPermSet.SetItem(token.m_index, perm);
                    }
                    else
                    {
                        // Add to normalPermSet
                        if (normalPermSet == null)
                            normalPermSet = new TokenBasedSet();
                        normalPermSet.SetItem(token.m_index, perm);
                    }

                }

            }
            
        }
    }
}
