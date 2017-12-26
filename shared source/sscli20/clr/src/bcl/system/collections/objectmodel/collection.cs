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

namespace System.Collections.ObjectModel
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.Diagnostics;
    
    [Serializable()]
    [System.Runtime.InteropServices.ComVisible(false)]
    [DebuggerTypeProxy(typeof(Mscorlib_CollectionDebugView<>))]
    [DebuggerDisplay("Count = {Count}")]    
    public class Collection<T>: IList<T>, IList
    {
        IList<T> items;
        [NonSerialized]
        private Object _syncRoot;

        public Collection() {
            items = new List<T>();
        }

        public Collection(IList<T> list) {
            if (list == null) {
                ThrowHelper.ThrowArgumentNullException(ExceptionArgument.list);
            }
            items = list;
        }

        public int Count {
            get { return items.Count; }
        }

        protected IList<T> Items {
            get { return items; }
        }

        public T this[int index] {
            get { return items[index]; }
            set {
                if( items.IsReadOnly) {
                    ThrowHelper.ThrowNotSupportedException(ExceptionResource.NotSupported_ReadOnlyCollection);
                }
                
                if (index < 0 || index >= items.Count) {
                    ThrowHelper.ThrowArgumentOutOfRangeException();
                }

                SetItem(index, value);
            }
        }

        public void Add(T item) {
            if( items.IsReadOnly) {
                ThrowHelper.ThrowNotSupportedException(ExceptionResource.NotSupported_ReadOnlyCollection);
            }
            
            int index = items.Count;
            InsertItem(index, item);
        }

        public void Clear() {
            if( items.IsReadOnly) {
                ThrowHelper.ThrowNotSupportedException(ExceptionResource.NotSupported_ReadOnlyCollection);
            }
            
            ClearItems();
        }

        public void CopyTo(T[] array, int index) {
            items.CopyTo(array, index);
        }

        public bool Contains(T item) {
            return items.Contains(item);
        }

        public IEnumerator<T> GetEnumerator() {
            return items.GetEnumerator();
        }

        public int IndexOf(T item) {
            return items.IndexOf(item);
        }

        public void Insert(int index, T item) {
            if (index < 0 || index > items.Count) {
                ThrowHelper.ThrowArgumentOutOfRangeException(ExceptionArgument.index, ExceptionResource.ArgumentOutOfRange_ListInsert);
            }

            InsertItem(index, item);
        }

        public bool Remove(T item) {
            if( items.IsReadOnly) {
                ThrowHelper.ThrowNotSupportedException(ExceptionResource.NotSupported_ReadOnlyCollection);
            }
            
            int index = items.IndexOf(item);
            if (index < 0) return false;
            RemoveItem(index);
            return true;
        }

        public void RemoveAt(int index) {
            if( items.IsReadOnly) {
                ThrowHelper.ThrowNotSupportedException(ExceptionResource.NotSupported_ReadOnlyCollection);
            }

            if (index < 0 || index >= items.Count) {
                ThrowHelper.ThrowArgumentOutOfRangeException();
            }

            RemoveItem(index);
        }

        protected virtual void ClearItems() {
            items.Clear();
        }

        protected virtual void InsertItem(int index, T item) {
            items.Insert(index, item);
        }
        
        protected virtual void RemoveItem(int index) {
            items.RemoveAt(index);
        }

        protected virtual void SetItem(int index, T item) {
            items[index] = item;
        }
        
        bool ICollection<T>.IsReadOnly {
            get { 
                return items.IsReadOnly; 
            }
        }
		
        IEnumerator IEnumerable.GetEnumerator() {
            return ((IEnumerable)items).GetEnumerator();
        }

        bool ICollection.IsSynchronized {
            get { return false; }
        }

        object ICollection.SyncRoot { 
            get {                 
                if( _syncRoot == null) {
                    ICollection c = items as ICollection;
                    if( c != null) {
                        _syncRoot = c.SyncRoot;
                    }
                    else {
                        System.Threading.Interlocked.CompareExchange(ref _syncRoot, new Object(), null);    
                    }
                }
                return _syncRoot;                 
            }
        }

        void ICollection.CopyTo(Array array, int index) {
            if (array == null) {
                ThrowHelper.ThrowArgumentNullException(ExceptionArgument.array);
            }
            
            if (array.Rank != 1) {
                ThrowHelper.ThrowArgumentException(ExceptionResource.Arg_RankMultiDimNotSupported);
            }

            if( array.GetLowerBound(0) != 0 ) {
                ThrowHelper.ThrowArgumentException(ExceptionResource.Arg_NonZeroLowerBound);
            }
            
            if (index < 0 ) {
                ThrowHelper.ThrowArgumentOutOfRangeException(ExceptionArgument.index, ExceptionResource.ArgumentOutOfRange_NeedNonNegNum);
            }

            if (array.Length - index < Count) {
                ThrowHelper.ThrowArgumentException(ExceptionResource.Arg_ArrayPlusOffTooSmall);
            }

            T[] tArray = array as T[];
            if (tArray  != null) {
                items.CopyTo(tArray , index);
            }
            else {
                //
                // Catch the obvious case assignment will fail.
                // We can found all possible problems by doing the check though.
                // For example, if the element type of the Array is derived from T,
                // we can't figure out if we can successfully copy the element beforehand.
                //
                Type targetType = array.GetType().GetElementType(); 
                Type sourceType = typeof(T);
                if(!(targetType.IsAssignableFrom(sourceType) || sourceType.IsAssignableFrom(targetType))) {
                    ThrowHelper.ThrowArgumentException(ExceptionResource.Argument_InvalidArrayType);
                }

                //
                // We can't cast array of value type to object[], so we don't support 
                // widening of primitive types here.
                //
                object[] objects = array as object[];
                if( objects == null) {
                    ThrowHelper.ThrowArgumentException(ExceptionResource.Argument_InvalidArrayType);
                }

                int count = items.Count;
                try {
                    for (int i = 0; i < count; i++) {
                        objects[index++] = items[i];
                    }
                }
                catch(ArrayTypeMismatchException) {
                    ThrowHelper.ThrowArgumentException(ExceptionResource.Argument_InvalidArrayType);
                }
            }            
        }

        object IList.this[int index] {
            get { return items[index]; }
            set { 
                VerifyValueType(value); 
                this[index] = (T)value;               
            }
        }

        bool IList.IsReadOnly {
            get { 
                return items.IsReadOnly;
            }
        }

        bool IList.IsFixedSize {
            get { 
                // There is no IList<T>.IsFixedSize, so we must assume false if our
                // internal item collection does not implement IList
                IList list = items as IList;
                if(list != null)
                {
                    return list.IsFixedSize; 
                }
                return false;
            }
        }

        int IList.Add(object value) {
            if( items.IsReadOnly) {
                ThrowHelper.ThrowNotSupportedException(ExceptionResource.NotSupported_ReadOnlyCollection);
            }
            
            VerifyValueType(value);            
            Add((T)value);
            return this.Count - 1;
        }

        bool IList.Contains(object value) {
            if(IsCompatibleObject(value)) {
                return Contains((T)value);
            }
            return false;
        }

        int IList.IndexOf(object value) {            
            if(IsCompatibleObject(value)) {
                return IndexOf((T)value);
            }
            return -1;
        }

        void IList.Insert(int index, object value) {
            if( items.IsReadOnly) {
                ThrowHelper.ThrowNotSupportedException(ExceptionResource.NotSupported_ReadOnlyCollection);
            }
            
            VerifyValueType(value);            
            Insert(index, (T)value);
        }

        void IList.Remove(object value) {
            if( items.IsReadOnly) {
                ThrowHelper.ThrowNotSupportedException(ExceptionResource.NotSupported_ReadOnlyCollection);
            }
            
            if(IsCompatibleObject(value)) {
                Remove((T)value);                
            }
        }

        private static bool IsCompatibleObject(object value) {
            if( (value is T) || ( value == null && !typeof(T).IsValueType) ) {
                return true;
            }            
            return false;
        }

        private static void VerifyValueType(object value) {
            if( !IsCompatibleObject(value)) {
                ThrowHelper.ThrowWrongValueTypeArgumentException(value, typeof(T));
            }            
        }
    }
}
