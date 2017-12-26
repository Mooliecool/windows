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

/*************************** Red-Black Tree ************************************************

 Rule 0: Binary Tree useful for data storage
 Rule 1: Every node has value (key) greater than left-child & less than the right-child value
 Rule 2: Every node is "red" or "black" (color)
 Rule 3: Root is black
 Rule 4: Leaf is red
 Rule 5. Red node which is not a leaf has black children(consecutive red nodes not allowed)
 Rule 6: Every path from root to leaf contains same no. of black nodes(black-height of the tree)
 Rule 7: Every node has a rank(0 to max) where root has max. rank..called height of the tree
 Rule 8: If a black node has rank "r", it's parent will have rank "r+1"
 Rule 9: If a red node had rank "r", it's parent will have rank "r"
 Rule 10: Readjustment of the tree is by rotation
	
**********************************************************************************************/


namespace Default {
using System;

public class Node {
	public int key;
	public string color;
	public int rank;
	public Node left;
	public Node right;
	public Node parent;
	public int[] array;

}

public class Tree {
		
	// initialize random number generator

	public static DateTime time = DateTime.Now;
	public static Int32 seed = (Int32)time.Ticks;
	public static Random rand = new Random(seed);
	public int nodes;
	public Node root;

	//Node[] array;  //temp..will become tree..
	//public int count;

	public Tree(int n) { 
		nodes = n;
		//array = new Node[nodes];  // temp
	}

	public void BuildTree() {
		for(int i=0;i<nodes;i++) {
			try {
				bool result = InsertNode();
				if(result==false) return;
			} catch(Exception e) {
				Console.WriteLine("Caught: {0}",e);
				return;
			}
			//PrintTree(root);
			Console.WriteLine("RedBlack tree now has {0} nodes",i);
			GC.Collect();
		}
		Console.WriteLine("Final Red Black Tree Constructed");
		//PrintTree(root);
	}	
	
	public bool InsertNode() {
			Node n=BuildNode();
			
			if(n==null) return false;

			if(root==null) {
				root=n;
				root.color="black";
				return true;
			}

			//Rule 1: Every node has value (key) greater than left-child & less than the right-child value
			// so traverse tree to find it's place

			Node temp=root;
			while((temp.left!=null) || (temp.right!=null)) {
				if(n.key < temp.key) {
					if(temp.left != null) {
					     temp=temp.left;
					     continue;
					}
					else {
					    temp.left=n;
					    n.parent=temp;
					    return true;
					}
				}
				else if(n.key > temp.key){
					if(temp.right != null) {
					    temp=temp.right;
					    continue;
					}
					else {
					    temp.right=n;
					    n.parent=temp;
					    return true;
					}
				}
			}
			if(n.key < temp.key) 
				temp.left=n;
			else if(n.key > temp.key)
				temp.right=n;
			n.parent=temp;

			// Adjust tree after insertion
			AdjustTree(n);
			return true;
	}

	public Node BuildNode() {
		Node temp;
		try {
			temp = new Node();
		}catch(Exception e) {
			Console.WriteLine("Caught: {0}",e);
			return null;
		}
		int k = rand.Next(0,nodes);
		bool result = UniqueKey(root,k);

		while(result==false) {
			k = rand.Next(0,nodes);
			result=UniqueKey(root,k);
		}
		
		temp.key = k;
		temp.color = "red";   //Rule 4: Leaf is red
			
		try {
	 		temp.array = new int[128];
		}catch(Exception e) {
			Console.WriteLine("Caught: {0}",e);
			temp.array = new int[5];
		}

		//Console.WriteLine("inserted: {0}",temp.key);
		return temp;
	}

	public bool UniqueKey(Node r,int k) {
		if(root==null) return true;
		
		//Console.WriteLine("r.key={0} , k={1}",r.key, k);
		
		if(k==r.key) return false;
		
		else if(k<r.key){
			if(r.left!=null) return(UniqueKey(r.left,k));
			else { return true;}
		}
		else {
		 	if(r.right!=null) return(UniqueKey(r.right,k));
			else { return true;}
		}
	}

	public void AdjustTree(Node x) {
		
		//Rule 10: Readjustment of the tree is by rotation
		RotateTree(x);

		//Rule 3: Root is black
		root.color="black";	

		//Rule 4: Leaf is red...automatically as we have all nodes as red to begin with
	
		//Rule 5. Red node which is not a leaf has black children(consecutive red nodes not allowed)
		//SetNodeColor(root);
		
	}

		public void RotateTree(Node x) {
		Node uncle=null;
		Node sibling=null;

		if(x.parent.color=="red") {
			
			if(WhichChild(x.parent)=="left") 	// uncle is the right child
				//if((x.parent).parent.right != null) 
					uncle=(x.parent).parent.right;
			else if(WhichChild(x.parent)=="right") 	// uncle is the left child
				//if((x.parent).parent.left != null)
					 uncle=(x.parent).parent.left;

			if(WhichChild(x)=="left") // x is left child
				//if(x.parent.right != null) 
				sibling=x.parent.right;	
			else  //x is right child
				//if(x.parent.left != null)
				sibling=x.parent.left;

		}
		else return;

		//Rotation Type 1 : x is red, p[x] is red and uncle=null, sibling=null
		if((uncle==null)&&(sibling==null)) {

			//Different orientation...Works!

			if(WhichChild(x)!= WhichChild(x.parent)) {
				int temp = x.key;
				x.key=x.parent.key;
				x.parent.key=temp;

				// reverse orientations
				if(WhichChild(x)=="left") {
					x.parent.right=x;
					x.parent.left=null;
				}
				else {
					x.parent.left=x;
					x.parent.right=null;
				}
				RotateTree(x);
			}

			// Same orientation..Works!

			else {
			if(x.parent.parent != root) {

				if(WhichChild(x.parent.parent) == "left") 
					x.parent.parent.parent.left=x.parent;
				else x.parent.parent.parent.right=x.parent;
			}
	
			else root=x.parent;

			if(WhichChild(x)=="left") {
				(x.parent).right=(x.parent).parent;
				
				((x.parent).right).parent=x.parent;
				
				/*if(WhichChild(x.parent) == "left") { 
					Console.WriteLine("ok");
					x.parent.parent.left=null;
				}
				else { ((x.parent).right).right=null;}*/
				
				x.parent.parent.left=null;
				
				x.parent.right.color="red";
				//Console.WriteLine("root {0},left {1}, right {2}, last row {3},{4},{5},{6}",root.key,root.left.key,root.right.key,root.left.left,root.left.right,root.right.left,root.right.right);
				
			}
			else {
				x.parent.left=x.parent.parent;
				x.parent.left.parent=x.parent;
				if(WhichChild(x.parent) == "left") x.parent.left.left=null;
				else x.parent.left.right=null;
				x.parent.left.color="red";
			}
			
			x.parent.color="black";
			}
		}  // end of Rotation Type 1

		//Rotation Type 2: depending on uncle's color
		else {
			switch(uncle.color) {
			case "red":
				if(WhichChild(uncle)=="left") x.parent.parent.left.color="black";   //u[x] = black
				else x.parent.parent.right.color="black";
				x.parent.color="black";		//p[x] = black
				x.parent.parent.color="red";    //p(p[x]) = red
				break;
			case "black":
				if(WhichChild(x)=="right") {
					x.parent.color="black";
					x.parent.left.color="red";
				}
				else {
				}
				break;
			default: break;
			}
		}
	
	}

	public string WhichChild(Node n) {
		if(n==root) return "none";
		if(n==n.parent.left) return "left";
		else return "right";
	}

	public void SetNodeColor(Node n) {
		
		//Rule 5. Red node which is not a leaf has black children(consecutive red nodes not allowed)		
		
		if(n.color=="red") {   //set child color as black
			if(n.left!=null) n.left.color="black";
			if(n.right!=null) n.right.color="black";
		}

		if(n.left!=null) SetNodeColor(n.left);
		if(n.right!=null) SetNodeColor(n.right);
	}
	
	public Node FindNode(Node r,int k) {
		//Console.WriteLine("In FindNode()");
		try {
			if(k==r.key) return r;
			
			else if(k<r.key) {
			    if(r.left!=null) return(FindNode(r.left,k));
			    else return null; // skip this node
			}
			else {
		    	if(r.right!=null) return(FindNode(r.right,k));
		    	else return null;
			}
		}catch(Exception e) {
			Console.WriteLine("Exception caught in FindNode(): "+e);
			return null;
		}
	}
	
	public void PrintTree(Node r) {
		if(r==null) return;
					
		Console.WriteLine("{0}, {1}",r.key,r.color);
		if(r.left!=null) PrintTree(r.left);
		if(r.right!=null) PrintTree(r.right);
		}

	
}

public class Test {
	
	public static void Main() {
		Environment.ExitCode=1;

		Console.WriteLine("Forcing JIT of overflow path....");
		try
		{ throw new System.OverflowException();
		}catch(Exception e) {
			Console.WriteLine("Exception caught in FindLargestInLeftSubTree(): "+e);
		}
		Console.WriteLine("Now, on with the test!");

		Console.WriteLine("Constructing Red-Black Tree with 10000 nodes");
		Tree rbtree;
		GC.Collect();
		try {
		  rbtree = new Tree(10000);
		rbtree.BuildTree();
		} catch(Exception e) {
			Console.WriteLine("Caught: {0}",e);
			Environment.ExitCode=1;
			return;
		}
		
		rbtree = null;
		GC.Collect();
		
		Console.WriteLine("Done");
		Console.WriteLine("Test Passed");
		Environment.ExitCode=0;
	}
	
}
}
