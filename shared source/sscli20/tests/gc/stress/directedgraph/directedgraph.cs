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

/************************************************************************************************************
A directed weighted graph is constructed with 'n' nodes and random edges.
All the nodes are made reachable.
A random node is deleted and the entire graph is restructured by deleting the related edges
and entries in the adjacent-node list.
n is 800 here
*************************************************************************************************************/

namespace DefaultNamespace {
using System;

public class Graph
{
	private Vertex Vfirst = null;
	private Vertex Vlast = null;
	private Edge Efirst = null;
	private Edge Elast = null;
	private int WeightSum = 0;
	
	public static int Nodes;
	

	public Graph(int n) { Nodes = n;}

	public void SetWeightSum() {
		Edge temp = Efirst;
		WeightSum = 0;
		while(temp != null) {
			WeightSum += temp.Weight;
			temp = temp.Next;
		}
	}
	
	public int GetWeightSum() {
		return WeightSum;
	}
		
	public void BuildEdge(int v1,int v2) {
		Vertex n1 = null,n2 = null;
		Vertex temp = Vfirst;
		
		while(temp != null) {
			int i = Decimal.Compare(v1,temp.Name);
			if(i==0) {
				//found 1st node..
				n1 = temp;
				break;
			}
			else temp = temp.Next;
		}
		
		//check if edge already exists
		for(int i=0;i<n1.Num_Edges;i++) {
			int j = Decimal.Compare(v2,n1.Adjacent[i].Name);
			if(j==0) return;
		}

		temp = Vfirst;
		while(temp != null) {
			int i = Decimal.Compare(v2,temp.Name);
			if(i==0) {
				//found 2nd node..
				n2 = temp;
				break;
			}
			else temp = temp.Next;
		}
		
		n1.Adjacent[n1.Num_Edges++]=n2;
		
		Edge temp2 = new Edge(n1,n2);
		        if(Efirst==null) {
		                Efirst = temp2;
		                Elast = temp2;
		        }
		        else {
		                temp2.AddEdge(Elast,temp2);
		                Elast = temp2;
                       }
	}
	
	public void BuildGraph() {
		
		// Build Nodes	
		Console.WriteLine("Building Vertices...");
		for(int i=0;i< Nodes; i++) {
			Vertex temp = new Vertex(i);
			if(Vfirst==null) {
			     Vfirst = temp;
			     Vlast = temp;
		        }
		        else {
			     temp.AddVertex(Vlast,temp);
			     Vlast = temp;
                        }
			Console.WriteLine("Vertex {0} built...",i);
		}
		
		// Build Edges
		Console.WriteLine("Building Edges...");
	
		DateTime time = DateTime.Now;
		Int32 seed = (Int32)time.Ticks;
		Random rand = new Random(seed);
		
		for(int i=0;i< Nodes;i++) {
		    
		    int j = rand.Next(0,Nodes);
		    for(int k=0;k<j;k++) {
		       int v2;
		       while((v2 = rand.Next(0,Nodes))==i);     //select a random node, also avoid self-loops
		       BuildEdge(i,v2);                //build edge betn node i and v2
		       //Console.WriteLine("Edge built between {0} and {1}...",i,v2);
	              
		       
		    }		
		}
	}


	public void CheckIfReachable() {
		int[] temp = new int[Nodes];
		Vertex t1 = Vfirst;
		
		Console.WriteLine("Making all vertices reachable...");
		while(t1 != null) {
			for(int i=0;i<t1.Num_Edges;i++) {
				if(temp[t1.Adjacent[i].Name] == 0)
					temp[t1.Adjacent[i].Name]=1;
			}
			t1 = t1.Next;
		}

		for(int v2=0;v2<Nodes;v2++) {
			if(temp[v2]==0) {  //this vertex is not connected
				DateTime time = DateTime.Now;
				Int32 seed = (Int32)time.Ticks;
				Random rand = new Random(seed);
				int v1;
				while((v1 = rand.Next(0,Nodes))==v2);     //select a random node, also avoid self-loops
				BuildEdge(v1,v2);
				temp[v2]=1;
			}
		}
		
	}
	
	/*public void TraverseGraph() {
		Vertex root = Vfirst;
		int i=0,j=0;
		Vertex next = root.Adjacent[i];
		
		while(j<Nodes) {
			
				Console.WriteLine("root: " + root.Name);
				while(next != null) {
					Console.WriteLine(next.Name);
					if(next.Name == j) {break;}
					next = next.Adjacent[0]; 
				}
				i++;
				if((next = root.Adjacent[i]) == null) {
					i=0;
					j++;
					if(root == Vlast) break;
					else root = root.Next;
					next = root.Adjacent[i];
					
		                }
	        }	
		
	}*/
	
	public void DeleteVertex() {
		Vertex temp1 = null;
		Vertex temp2 = Vfirst;

		DateTime time = DateTime.Now;
		Int32 seed = (Int32)time.Ticks;
		Random rand = new Random(seed);
		
		int j = rand.Next(0,Nodes);
		//Console.WriteLine("Deleting vertex: " + j);

		while(temp2 != null) {
			int i = Decimal.Compare(j,temp2.Name);
			if(i==0) {
				if(temp2 == Vfirst) {
					temp2 = null;
					Vfirst = Vfirst.Next;
					break;
				}
				temp1.Next = temp2.Next;
				temp2 = null;
				break;
				
			}
			else {
				temp1 = temp2;
				temp2 = temp2.Next;
			}
		}
		
		// Restructuring the Graph
		Console.WriteLine("Restructuring the Graph...");
		temp2=Vfirst;
		while(temp2 != null) {
			temp2.DeleteAdjacentEntry(j);
			temp2 = temp2.Next;
		}

		Edge e1 = null;
		Edge e2 = Efirst;
		Edge temp = null;

		while(e2 != null) {
			int v1 = Decimal.Compare(j,e2.v1.Name);
			int v2 = Decimal.Compare(j,e2.v2.Name);
			if((v1==0) || (v2==0)) {
				if(e2 == Efirst) {
					temp = e2;
					e2 = e2.Next;
					Efirst = Efirst.Next;
					temp = null;
					
				}
				else {
					temp = e1;
					e1.Next = e2.Next;
					e2 = e2.Next;
				}
			}
			else {
				e1 = e2;
				e2 = e2.Next;
			}	
		}
	}
	
	public void PrintGraph() {

		// Print Vertices
		Vertex temp = Vfirst;
		while(temp != null) {
			Console.WriteLine("Vertex: {0}",temp.Name);
			Console.WriteLine("Adjacent Vertices:");
			for(int i=0;i< temp.Num_Edges;i++) {
		   		Console.WriteLine(temp.Adjacent[i].Name);
			}
			temp = temp.Next;
		}

		//Print Edges
		Edge temp2 = Efirst;
		int edge=0;
		while(temp2 != null) {
			Console.WriteLine("Edge " + edge++);
			Console.WriteLine("Weight: {0}, v1: {1}, v2: {2}",temp2.Weight,temp2.v1.Name,temp2.v2.Name);
			temp2 = temp2.Next;
		}
		SetWeightSum();
		Console.WriteLine("Sum of Weights is: {0}",GetWeightSum());
		
	}
	
}

public class Vertex
	{
		public int Name;
		//public bool Visited = false;
		
		public Vertex Next;
		public Vertex[] Adjacent;
		public Edge[] Edges;
		public int Num_Edges = 0;

		public Vertex(int val) {
			Name = val;
			Next = null;
			Adjacent = new Vertex[Graph.Nodes];	
		}
		
		public void AddVertex(Vertex x, Vertex y) {
			x.Next = y;				
		}
		
		public void DeleteAdjacentEntry(int n) {
			int temp=Num_Edges;
			for(int i=0;i< temp;i++) {
				if(n == Adjacent[i].Name) {
					for(int j=i;j<Num_Edges;j++) 
						Adjacent[j] = Adjacent[j+1];
					Num_Edges--;
					return;
				}
			}
		}
	}


public class Edge 
	{
		public int Weight;
		public Vertex v1,v2;
		public Edge Next;
	
		public Edge(Vertex n1, Vertex n2) {
			v1=n1;
			v2=n2;
			
			int seed = n1.Name+n2.Name;
			Random rand = new Random(seed);
			Weight = rand.Next(0,50);
		}
		
		public void AddEdge(Edge x, Edge y) {
			x.Next = y;				
		}

	}


public class Test
{
  public static void Main()
  {
	Console.WriteLine("Building Graph with 800 vertices...");
	Graph MyGraph = new Graph(800);  // graph with 800 nodes
	MyGraph.BuildGraph();  

	Console.WriteLine("Checking if all vertices are reachable...");  
	MyGraph.CheckIfReachable();

	//Console.WriteLine("Printing the Graph...");
	//MyGraph.PrintGraph();

	Console.WriteLine("Deleting a random vertex...");
	MyGraph.DeleteVertex();

	//MyGraph.PrintGraph();
		
	Console.WriteLine("Done");
	Environment.ExitCode = (0);
	Console.WriteLine("Test Passed");
  }
}
}
