//------------------------------------------------------------------------------
// <copyright file="TreeSet.cs" company="Microsoft">
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

namespace System.Collections.Generic {
    using System;
    using System.Diagnostics;
    using System.Runtime.Serialization;

    //
    // A binary search tree is a red-black tree if it satifies the following red-black properties:
    // 1. Every node is either red or black
    // 2. Every leaf (nil node) is black
    // 3. If a node is red, the both its children are black
    // 4. Every simple path from a node to a descendant leaf contains the same number of black nodes
    // 
    // The basic idea of red-black tree is to represent 2-3-4 trees as standard BSTs but to add one extra bit of information  
    // per node to encode 3-nodes and 4-nodes. 
    // 4-nodes will be represented as:          B
    //                                                              R            R
    // 3 -node will be represented as:           B             or         B     
    //                                                              R          B               B       R
    // 
    // For a detailed description of the algorithm, take a look at "Algorithm" by Rebert Sedgewick.
    //

    internal delegate bool TreeWalkAction<T>(TreeSet<T>.Node node);

    internal enum TreeRotation {
        LeftRotation = 1,    
        RightRotation = 2,   
        RightLeftRotation = 3,   
        LeftRightRotation = 4,   
    }

    [Serializable]
    internal class TreeSet<T> : ICollection<T>, ICollection, ISerializable, IDeserializationCallback {
        Node root;
        IComparer<T> comparer;
        int count;
        int version;
        private Object _syncRoot;

        private const String ComparerName = "Comparer";
        private const String CountName = "Count";
        private const String ItemsName = "Items";
        private const String VersionName = "Version";

        private SerializationInfo siInfo; //A temporary variable which we need during deserialization.        
        
        public TreeSet(IComparer<T> comparer) {
            if ( comparer == null) {
                this.comparer = Comparer<T>.Default;
            } 
            else {
                this.comparer = comparer;
            }
        }
        
        protected TreeSet(SerializationInfo info, StreamingContext context) {
            siInfo = info; 
        }


        public int Count {
            get {
                return count;
            }
        }

        public IComparer<T> Comparer {
            get {
                return comparer;
            }
        }

        bool ICollection<T>.IsReadOnly {
            get {
                return false;
            }
        }

        bool ICollection.IsSynchronized {
            get {
                return false;
            }
        }

        object ICollection.SyncRoot {
            get {
                if( _syncRoot == null) {
                    System.Threading.Interlocked.CompareExchange(ref _syncRoot, new Object(), null);    
                }
                return _syncRoot; 
            }
        }

        public void Add(T item) {            
            if ( root  == null) {   // empty tree
                root = new Node(item, false);
                count = 1;
                return;  
            }

            //
            // Search for a node at bottom to insert the new node. 
            // If we can guanratee the node we found is not a 4-node, it would be easy to do insertion.
            // We split 4-nodes along the search path.
            // 
            Node current = root; 
            Node parent = null;
            Node grandParent = null;
            Node greatGrandParent = null;            

            int order = 0;
            while ( current != null) {
                order = comparer.Compare( item, current.Item);
                if ( order == 0) {
                    // We could have changed root node to red during the search process.
                    // We need to set it to black before we return.
                    root.IsRed = false;                  
                    ThrowHelper.ThrowArgumentException(ExceptionResource.Argument_AddingDuplicate);
                }

                // split a 4-node into two 2-nodes                
                if ( Is4Node(current)) {
                    Split4Node(current);    
                    // We could have introduced two consecutive red nodes after split. Fix that by rotation.
                    if ( IsRed(parent)) {
                        InsertionBalance(current, ref parent, grandParent,  greatGrandParent);
                    }
                }
                greatGrandParent =  grandParent;
                grandParent = parent;
                parent = current;
                current = (order < 0) ? current.Left : current.Right;
            }

            Debug.Assert( parent != null, "Parent node cannot be null here!");            
            // ready to insert the new node
            Node node = new Node(item);            
            if ( order > 0) {
                parent.Right = node;
            } else {
                parent.Left = node;
            }

            // the new node will be red, so we will need to adjust the colors if parent node is also red
            if ( parent.IsRed) {
                InsertionBalance(node, ref parent, grandParent,  greatGrandParent);
            }

            // Root node is always black
            root.IsRed = false;  
            ++count;
            ++version;
        }

        public void Clear() {
            root  =  null;
            count = 0;
            ++version;
        }

        public bool Contains(T item) {
            return FindNode(item) != null;
        }

        //
        // Do a in order walk on tree and calls the delegate for each node.
        // If the action delegate returns false, stop the walk.
        // 
        // Return true if the entire tree has been walked. 
        // Otherwise returns false.
        //
        internal bool InOrderTreeWalk(TreeWalkAction<T> action) {
            if ( root == null) {
                return true;
            }
            
            // The maximum height of a red-black tree is 2*lg(n+1).
            // See page 264 of "Introduction to algorithms" by Thomas H. Cormen
            Stack<Node> stack = new Stack<Node>(2* (int)Math.Log(Count + 1));
            Node current = root;
            while (current != null) {
                stack.Push(current);
                current = current.Left;
            }

            while (stack.Count != 0) {
                current = stack.Pop();
                if (!action(current)) {
                    return false;
                }

                Node node = current.Right; 
                while ( node != null) {
                    stack.Push(node);
                    node = node.Left;
                }                    
            }
            return true;
        }

        public void CopyTo(T[] array, int index) {
            if (array == null) {
                ThrowHelper.ThrowArgumentNullException(ExceptionArgument.array);
            }

            if (index < 0) {
                ThrowHelper.ThrowArgumentOutOfRangeException(ExceptionArgument.index);
            }

            if (array.Length - index < Count) {
                ThrowHelper.ThrowArgumentException(ExceptionResource.Arg_ArrayPlusOffTooSmall);
            }

            InOrderTreeWalk(delegate(Node node){ array[index++] = node.Item; return true;});
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
                ThrowHelper.ThrowArgumentOutOfRangeException(ExceptionArgument.arrayIndex, ExceptionResource.ArgumentOutOfRange_NeedNonNegNum);
            }

            if (array.Length - index < Count) {
                ThrowHelper.ThrowArgumentException(ExceptionResource.Arg_ArrayPlusOffTooSmall);
            }
            
            T[] tarray = array as T[];
            if (tarray != null) {
                CopyTo(tarray, index);
            }
            else {
                object[] objects = array as object[];
                if (objects == null) {
                    ThrowHelper.ThrowArgumentException(ExceptionResource.Argument_InvalidArrayType);
                }

                try {
                    InOrderTreeWalk(delegate(Node node){ objects[index++] = node.Item; return true;});
                }
                catch(ArrayTypeMismatchException) {
                    ThrowHelper.ThrowArgumentException(ExceptionResource.Argument_InvalidArrayType);
                }
            }
        }

        public Enumerator GetEnumerator() {
            return new Enumerator(this);
        }

        IEnumerator<T> IEnumerable<T>.GetEnumerator() {
            return new Enumerator(this);
        }

        IEnumerator IEnumerable.GetEnumerator() {
            return new Enumerator(this);
        }

        internal Node FindNode(T item) {
            Node current = root;
            while ( current != null) {
                int order = comparer.Compare( item, current.Item);
                if ( order ==0) {
                    return current;
                } else {
                    current = (order < 0) ? current.Left : current.Right;
                }
            }   

            return null;            
        }

        public bool Remove(T item) {
            if ( root == null) {
                return false;
            }

            // Search for a node and then find its succesor. 
            // Then copy the item from the succesor to the matching node and delete the successor. 
            // If a node doesn't have a successor, we can replace it with its left child (if not empty.) 
            // or delete the matching node.
            // 
            // In top-down implementation, it is important to make sure the node to be deleted is not a 2-node.
            // Following code will make sure the node on the path is not a 2 Node. 
            // 
            Node current = root; 
            Node parent = null;
            Node grandParent = null;
            Node match = null;
            Node parentOfMatch = null;    
            bool  foundMatch = false;
            while ( current != null) {
                if ( Is2Node(current)) { // fix up 2-Node
                    if ( parent == null) {   // current is root. Mark it as red
                        current.IsRed = true;
                    } else {
                        Node sibling = GetSibling( current, parent);
                        if ( sibling.IsRed) { 
                            // If parent is a 3-node, flip the orientation of the red link. 
                            // We can acheive this by a single rotation        
                            // This case is converted to one of other cased below.
                            Debug.Assert(!parent.IsRed, "parent must be a black node!");
                            if ( parent.Right == sibling) {
                                RotateLeft( parent);
                            } else {
                                RotateRight( parent);
                            }

                            parent.IsRed = true;
                            sibling.IsRed = false;    // parent's color
                            // sibling becomes child of grandParent or root after rotation. Update link from grandParent or root
                            ReplaceChildOfNodeOrRoot(grandParent, parent, sibling);                                    
                            // sibling will become grandParent of current node 
                            grandParent = sibling;
                            if ( parent == match) {
                                parentOfMatch = sibling;
                            }

                            // update sibling, this is necessary for following processing
                            sibling = (parent.Left == current) ? parent.Right : parent.Left;                                                        
                        }
                        Debug.Assert(sibling != null || sibling.IsRed == false, "sibling must not be null and it must be black!");

                        if ( Is2Node( sibling)) {
                            Merge2Nodes(parent, current, sibling);
                        } else {
                            // current is a 2-node and sibling is either a 3-node or a 4-node.
                            // We can change the color of current to red by some rotation.
                            TreeRotation rotation = RotationNeeded(parent, current, sibling);
                            Node newGrandParent = null;
                            switch (rotation) {
                            case TreeRotation.RightRotation:
                                Debug.Assert(parent.Left == sibling, "sibling must be left child of parent!");
                                Debug.Assert(sibling.Left.IsRed, "Left child of sibling must be red!");
                                sibling.Left.IsRed = false;
                                newGrandParent = RotateRight( parent);
                                break;                            
                            case TreeRotation.LeftRotation:
                                Debug.Assert(parent.Right == sibling, "sibling must be left child of parent!");
                                Debug.Assert(sibling.Right.IsRed, "Right child of sibling must be red!");
                                sibling.Right.IsRed = false;
                                newGrandParent = RotateLeft( parent);
                                break;                       

                            case TreeRotation.RightLeftRotation:
                                Debug.Assert(parent.Right == sibling, "sibling must be left child of parent!");
                                Debug.Assert(sibling.Left.IsRed, "Left child of sibling must be red!");
                                newGrandParent =  RotateRightLeft(parent);
                                break;

                            case TreeRotation.LeftRightRotation:
                                Debug.Assert(parent.Left == sibling, "sibling must be left child of parent!");
                                Debug.Assert(sibling.Right.IsRed, "Right child of sibling must be red!");
                                newGrandParent = RotateLeftRight(parent);
                                break;
                            }

                            newGrandParent.IsRed = parent.IsRed;
                            parent.IsRed = false;
                            current.IsRed = true;
                            ReplaceChildOfNodeOrRoot( grandParent, parent, newGrandParent);                            
                            if ( parent  == match) {
                                parentOfMatch = newGrandParent;
                            }
                            grandParent = newGrandParent; 
                        }                    
                    }
                }

                // we don't need to compare any more once we found the match
                int order = foundMatch? -1 : comparer.Compare( item, current.Item);                
                if ( order == 0) {
                    // save the matching node
                    foundMatch = true;
                    match = current;
                    parentOfMatch = parent;                    
                }

                grandParent = parent;
                parent  = current;

                if ( order < 0) {
                    current = current.Left;
                } else {
                    current = current.Right;       // continue the search in  right sub tree after we find a match
                }                                
            }

            // move successor to the matching node position and replace links
            if ( match != null) {
                ReplaceNode(match, parentOfMatch, parent, grandParent);
                --count;
            }

            if ( root != null) {
                root.IsRed = false;
            }
            ++version;
            return foundMatch;
        }

        // LinkDemand here is unnecessary as this is a methodimpl and linkdemand from the interface should suffice
        void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context) {
            GetObjectData(info, context);    
        }

        protected void GetObjectData(SerializationInfo info, StreamingContext context) {
            if (info==null) {
                ThrowHelper.ThrowArgumentNullException(ExceptionArgument.info);
            }

            info.AddValue(CountName, count); //This is the length of the bucket array.
            info.AddValue(ComparerName, comparer, typeof(IComparer<T>));
            info.AddValue(VersionName, version);

            if( root != null) {
                T[] items = new T[Count];
                CopyTo(items, 0);
                info.AddValue(ItemsName, items, typeof(T[]));
            }
        }

        void IDeserializationCallback.OnDeserialization(Object sender) {
            OnDeserialization(sender);
        }

        protected void OnDeserialization(Object sender) {
            if (comparer != null) {
                return; //Somebody had a dependency on this class and fixed us up before the ObjectManager got to it.
            }
            
            if (siInfo==null) {
                ThrowHelper.ThrowSerializationException(ExceptionResource.Serialization_InvalidOnDeser);
            }            
            
            comparer   = (IComparer<T>)siInfo.GetValue(ComparerName, typeof(IComparer<T>));
            int savedCount = siInfo.GetInt32(CountName);            

            if( savedCount != 0) {
                T[] items = (T[]) siInfo.GetValue(ItemsName, typeof(T[]));

                if (items == null) {
                    ThrowHelper.ThrowSerializationException(ExceptionResource.Serialization_MissingValues);
                }

                for (int i=  0; i < items.Length; i++) {
                    Add(items[i]);
                }
            }

            version = siInfo.GetInt32(VersionName);
            if( count != savedCount) {
                ThrowHelper.ThrowSerializationException(ExceptionResource.Serialization_MismatchedCount);
            }
            siInfo = null;
        }

        private static Node GetSibling( Node node, Node parent) {
            if ( parent.Left == node) {
                return parent.Right;
            }
            return parent.Left;
        }               

        // After calling InsertionBalance, we need to make sure current and parent up-to-date.
        // It doesn't matter if we keep grandParent and greatGrantParent up-to-date 
        // because we won't need to split again in the next node.
        // By the time we need to split again, everything will be correctly set.
        //  
        private void InsertionBalance(Node current, ref Node parent, Node grandParent, Node greatGrandParent ) {
            Debug.Assert( grandParent != null, "Grand parent cannot be null here!");
            bool parentIsOnRight = (grandParent.Right == parent);
            bool currentIsOnRight = (parent.Right == current);

            Node newChildOfGreatGrandParent;
            if ( parentIsOnRight == currentIsOnRight) { // same orientation, single rotation
                newChildOfGreatGrandParent = currentIsOnRight? RotateLeft(grandParent) : RotateRight(grandParent);   
            } else {  // different orientaton, double rotation
                newChildOfGreatGrandParent = currentIsOnRight? RotateLeftRight(grandParent) : RotateRightLeft(grandParent);
                // current node now becomes the child of greatgrandparent 
                parent = greatGrandParent;
            }
            // grand parent will become a child of either parent of current.
            grandParent.IsRed = true;
            newChildOfGreatGrandParent.IsRed = false;

            ReplaceChildOfNodeOrRoot( greatGrandParent, grandParent, newChildOfGreatGrandParent);
        }

        private static bool Is2Node(Node node)  {
            Debug.Assert(node != null, "node cannot be null!");
            return IsBlack(node) && IsNullOrBlack(node.Left) && IsNullOrBlack(node.Right);
        }

        private static bool Is4Node(Node node) {
            return IsRed(node.Left) && IsRed(node.Right);
        }

        private static bool IsBlack(Node node)  {
            return(node != null && !node.IsRed);
        }

        private static bool IsNullOrBlack(Node node)  {
            return(node == null ||  !node.IsRed);
        }

        private static bool IsRed(Node node)  {
            return(node != null && node.IsRed);
        }

        private static void Merge2Nodes(Node parent, Node child1, Node child2) {
            Debug.Assert(IsRed(parent), "parent must be be red");
            // combing two 2-nodes into a 4-node
            parent.IsRed = false;
            child1.IsRed = true; 
            child2.IsRed = true; 
        }

        // Replace the child of a parent node. 
        // If the parent node is null, replace the root.
        private void ReplaceChildOfNodeOrRoot(Node parent, Node child, Node newChild) {        
            if ( parent != null) {
                if (parent.Left == child) {
                    parent.Left = newChild;
                } else {
                    parent.Right = newChild;
                }
            } else {
                root = newChild;
            }
        }

        // Replace the matching node with its succesor.
        private void ReplaceNode(Node match, Node parentOfMatch, Node succesor, Node parentOfSuccesor) {
            if ( succesor == match) {  // this node has no successor, should only happen if right child of matching node is null.
                Debug.Assert( match.Right == null, "Right child must be null!");
                succesor = match.Left;
            } else {
                Debug.Assert(  parentOfSuccesor  != null, "parent of successor cannot be null!");
                Debug.Assert(  succesor.Left == null, "Left child of succesor must be null!");
                Debug.Assert(  (succesor.Right == null  && succesor.IsRed)  || ( succesor.Right.IsRed && !succesor.IsRed ), "Succesor must be in valid state");
                if ( succesor.Right != null) {
                    succesor.Right.IsRed = false;
                }

                if ( parentOfSuccesor != match) {   // detach succesor from its parent and set its right child
                    parentOfSuccesor.Left = succesor.Right;
                    succesor.Right = match.Right;
                }

                succesor.Left = match.Left;
            }

            if ( succesor != null) {
                succesor.IsRed = match.IsRed;
            }

            ReplaceChildOfNodeOrRoot(parentOfMatch, match, succesor);                
        }

        internal void UpdateVersion() {
            ++version;
        }

        private static Node RotateLeft(Node node) {
            Node x = node.Right;
            node.Right = x.Left;
            x.Left = node;
            return x;
        }

        private static Node RotateLeftRight(Node node) {
            Node child = node.Left;
            Node grandChild = child.Right;

            node.Left = grandChild.Right;
            grandChild.Right = node;
            child.Right = grandChild.Left;
            grandChild.Left = child;
            return grandChild;         
        }

        private static Node RotateRight(Node node) {
            Node x = node.Left;
            node.Left = x.Right;
            x.Right = node;
            return x;
        }

        private static Node RotateRightLeft(Node node) {
            Node child = node.Right;
            Node grandChild = child.Left;

            node.Right = grandChild.Left;
            grandChild.Left = node;
            child.Left = grandChild.Right;
            grandChild.Right = child;
            return grandChild;         
        }

        private static TreeRotation RotationNeeded(Node parent, Node current, Node sibling) {
            Debug.Assert(IsRed(sibling.Left) || IsRed(sibling.Right), "sibling must have at least one red child");
            if ( IsRed(sibling.Left)) {
                if ( parent.Left == current) {
                    return TreeRotation.RightLeftRotation;
                }
                return TreeRotation.RightRotation;
            } else {
                if ( parent.Left == current) {
                    return TreeRotation.LeftRotation;
                }
                return TreeRotation.LeftRightRotation;                
            }
        }

        private static void Split4Node(Node node) {
            node.IsRed = true;
            node.Left.IsRed = false;
            node.Right.IsRed = false;
        }

        internal class Node {
            bool isRed;
            T item;
            Node left;
            Node right;

            public Node(T item) {
                // The default color will be red, we never need to create a black node directly.                
                this.item = item;
                isRed = true;
            }

            public Node(T item, bool isRed) {
                // The default color will be red, we never need to create a black node directly.                
                this.item = item;
                this.isRed = isRed;
            }

            public T Item {
                get {
                    return item;
                }
                set {
                    item = value;
                }
            }

            public Node Left {
                get {
                    return left;
                }
                set {
                    left = value;
                }
            }

            public Node Right {
                get {
                    return right;
                }
                set  {
                    right = value;
                }
            }

            public bool IsRed {
                get {
                    return isRed;
                }
                set {
                    isRed = value;
                }   
            }
        }

        public struct Enumerator : IEnumerator<T>, IEnumerator {
            private TreeSet<T> tree; 
            private int version;    
            private Stack< TreeSet<T>.Node> stack;
            private TreeSet<T>.Node current;
            static TreeSet<T>.Node dummyNode = new TreeSet<T>.Node(default(T));

            private const string TreeName = "Tree";
            private const string NodeValueName = "Item";
            private const string EnumStartName = "EnumStarted";
            private const string VersionName = "Version";
            

            internal Enumerator(TreeSet<T> set) {
                tree = set;
                version = tree.version;

                // 2lg(n + 1) is the maximum height
                stack = new Stack< TreeSet<T>.Node>(2* (int)Math.Log(set.Count + 1));
                current = null;
                Intialize();
            }

            private void Intialize() {
                current = null;
                TreeSet<T>.Node node = tree.root;
                while (node != null) {
                    stack.Push(node);
                    node = node.Left;
                }
            }

            public bool MoveNext() {
                if (version != tree.version) {
                    ThrowHelper.ThrowInvalidOperationException(ExceptionResource.InvalidOperation_EnumFailedVersion);
                }

                if ( stack.Count == 0) {
                    current = null;
                    return false;
                }

                current = stack.Pop();
                TreeSet<T>.Node node = current.Right; 
                while ( node != null) {
                    stack.Push(node);
                    node = node.Left;
                }
                return true;
            }

            public void Dispose() {
            }

            public T Current {
                get {
                    if( current != null) {
                        return current.Item;
                    }
                    return default(T);
                }
            }

            object IEnumerator.Current {
                get {
                    if( current == null) {
                         ThrowHelper.ThrowInvalidOperationException(ExceptionResource.InvalidOperation_EnumOpCantHappen);                        
                    }
                    
                    return current.Item;
                }
            }

            internal bool NotStartedOrEnded {
                get {
                    return current == null;
                }                    
            }
            
            internal void Reset() {
                if (version != tree.version) {
                    ThrowHelper.ThrowInvalidOperationException(ExceptionResource.InvalidOperation_EnumFailedVersion);
                }
                
                stack.Clear();
                Intialize();
            }

            void IEnumerator.Reset() {
                Reset();
            }
        }
    }
}
