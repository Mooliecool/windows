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

namespace Microsoft.JScript {
    
    using System;
    using System.Collections;
    using System.Globalization;
    using System.Reflection;
    
    internal sealed class HashtableEntry{
      internal Object key;
      internal Object value;
      internal uint hashCode;
      internal HashtableEntry next;
      
      internal HashtableEntry(Object key, Object value, uint hashCode, HashtableEntry next){
        this.key = key;
        this.value = value;
        this.hashCode = hashCode;
        this.next = next;
      }
    }
    
    public sealed class SimpleHashtable{
      private HashtableEntry[] table;
      internal int count;
      private uint threshold;

      public SimpleHashtable(uint threshold){
        if (threshold < 8)
          threshold = 8;
        this.table = new HashtableEntry[(int)(threshold * 2 - 1)];
        this.count = 0;
        this.threshold = threshold;
      }

      public IDictionaryEnumerator GetEnumerator(){
        return new SimpleHashtableEnumerator(this.table);
      }

      private HashtableEntry GetHashtableEntry(Object key, uint hashCode){      
        int index = (int)(hashCode % (uint)this.table.Length);
        HashtableEntry e = this.table[index];
        if (e == null) return null;
        if (e.key == key) return e;
        HashtableEntry prev = e;
        HashtableEntry curr = e.next;
        while (curr != null){
          if (curr.key == key)
            return curr;
          prev = curr;
          curr = curr.next;
        }
        if (e.hashCode == hashCode && e.key.Equals(key)){
          e.key = key; return e;
        }
        prev = e;
        curr = e.next;
        while (curr != null){
          if (curr.hashCode == hashCode && curr.key.Equals(key)){
            curr.key = key; return curr;
          }
          prev = curr;
          curr = curr.next;
        }
        return null;
      }

      internal Object IgnoreCaseGet(String name){
        for (uint i = 0, n = (uint)this.table.Length; i < n; i++){
          HashtableEntry e = this.table[i];
          while (e != null){
            if (String.Compare((String)e.key, name, StringComparison.OrdinalIgnoreCase) == 0) return e.value;
            e = e.next;
          }
        }
        return null;
      }

      private void Rehash(){
        HashtableEntry[] oldTable = this.table;
        uint threshold = this.threshold = (uint)(oldTable.Length+1);
        uint newCapacity = threshold * 2 - 1;
        HashtableEntry[] newTable = this.table = new HashtableEntry[newCapacity];
        for (uint i = threshold-1; i-- > 0;){
          for (HashtableEntry old = oldTable[(int)i]; old != null; ){
            HashtableEntry e = old; old = old.next;
            int index = (int)(e.hashCode % newCapacity);
            e.next = newTable[index];
            newTable[index] = e;
          }
        }
      }
      
      public void Remove(Object key){
        uint hashCode = (uint)key.GetHashCode();
        int index = (int)(hashCode % (uint)this.table.Length);
        HashtableEntry e = this.table[index];
        Debug.Assert(e != null);
        this.count--;
        while (e != null && e.hashCode == hashCode && (e.key == key || e.key.Equals(key)))
          e = e.next;
        this.table[index] = e;
        do{
          if (e == null) return;
          HashtableEntry f = e.next;
          while (f != null && f.hashCode == hashCode && (f.key == key || f.key.Equals(key)))
            f = f.next;
          e.next = f;
          e = f;
        }while(true);
      }
        
      public Object this[Object key]{
        get{
          HashtableEntry e = this.GetHashtableEntry(key, (uint)key.GetHashCode());
          if (e == null) return null;
          return e.value;
        }
        set{
          uint hashCode = (uint)key.GetHashCode();
          HashtableEntry e = this.GetHashtableEntry(key, hashCode);
          if (e != null){
            e.value = value; return;
          }
          if (++this.count >= this.threshold) this.Rehash();
          int index = (int)(hashCode % (uint)this.table.Length);
          this.table[index] = new HashtableEntry(key, value, hashCode, this.table[index]);
        }
      }

    }
    
    internal sealed class SimpleHashtableEnumerator : IDictionaryEnumerator{
      private HashtableEntry[] table;
      private int count;
      private int index;
      private HashtableEntry currentEntry;
      
      internal SimpleHashtableEnumerator(HashtableEntry[] table){
        this.table = table;
        this.count = table.Length;
        this.index = -1;
        this.currentEntry = null;
      }
      
      public Object Current{ //Used by expando classes to enumerate all the keys in the hashtable
        get{
          return this.Key;
        }
      }
      
      public DictionaryEntry Entry{
        get{
          return new DictionaryEntry(this.Key, this.Value);
        }
      }
      
      public Object Key{
        get{
          return this.currentEntry.key;
        }
      }
      
      public bool MoveNext(){
        HashtableEntry[] table = this.table;
        if (this.currentEntry != null){
          this.currentEntry = this.currentEntry.next;
          if (this.currentEntry != null)
            return true;
        }
        for (int i = ++this.index, n = this.count; i < n; i++)
          if (table[i] != null){
            this.index = i;
            this.currentEntry = table[i];
            return true;
          }
        return false;
      }
      
      public void Reset(){
        this.index = -1;
        this.currentEntry = null;
      }
      
      public Object Value{
        get{
          return this.currentEntry.value;
        }
      }
      
    } 
    
}
    
