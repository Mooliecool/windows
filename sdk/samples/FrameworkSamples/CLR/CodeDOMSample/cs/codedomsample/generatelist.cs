//---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//  This source code is intended only as a supplement to Microsoft
//  Development Tools and/or on-line documentation.  See these other
//  materials for detailed information regarding Microsoft code samples.
// 
//  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//---------------------------------------------------------------------
using System;
using System.CodeDom;
using System.CodeDom.Compiler;
using Microsoft.CSharp;
using Microsoft.VisualBasic;
using System.IO;

namespace Microsoft.VisualStudio.Samples.CodeDOM.CodeDOMSample
{
	/// <summary>
	/// Class to generate the generic code
	/// </summary>
	public class GenerateList
	{
		//Static variables
		private CodeDomProvider provider;
		//Compile unit instance 
		private CodeCompileUnit compileUnit;
		//Generator options object to set code options
		private CodeGeneratorOptions options;
		//Target location of generated files
		private string path = string.Empty;
		//Generic type parameters
		private CodeTypeParameter keyTypeParameter,itemTypeParameter;
		//Generic type references
		private CodeTypeReference keyType,itemType,nodeType;

		//Constructor to initialize private fields
		public GenerateList(CodeDomProvider prov, string filePath)
		{
			provider = prov;
			compileUnit = new CodeCompileUnit();
			path = filePath;

			//Generator options object to set code options
			options = new CodeGeneratorOptions();

			//Verbatim ordering turned on - code generated in the order added
			options.VerbatimOrder = true;

			//String for code indentation
			options.IndentString = "     ";
			options.BlankLinesBetweenMembers = false;

			keyTypeParameter = new CodeTypeParameter("K");
			//Add constraint IComparable
			//where K:IComparable
			keyTypeParameter.Constraints.Add(typeof(IComparable));
			keyType = new CodeTypeReference(keyTypeParameter);

			itemTypeParameter = new CodeTypeParameter("T");
			//Add constructor constraint to itemType
			//where T:new()
			itemTypeParameter.HasConstructorConstraint = true;
			itemType = new CodeTypeReference(itemTypeParameter);

            // Build a reference to Node<K,T>
            nodeType = new CodeTypeReference("Node",
                                             new CodeTypeReference(keyTypeParameter), 
                                             new CodeTypeReference(itemTypeParameter));
		}

		//Generate the four files
		public void GenerateCode()
		{
			GenerateNode();
			GenerateLinkedList();
			GenerateListInterface();
			GenerateMain();
		}

		//Generate code for class Node 
		private void GenerateNode()
		{
			CodeNamespace ns = new CodeNamespace("GenericList");
			compileUnit.Namespaces.Add(ns);

			//Create class Node
			CodeTypeDeclaration nodeClass = new CodeTypeDeclaration("Node");

			//Add types to generic class 
			nodeClass.TypeParameters.Add(keyTypeParameter);
			nodeClass.TypeParameters.Add(itemTypeParameter);

			//Add class to namespace
			ns.Types.Add(nodeClass);

			//Add field 
			//public K Key;
			CodeMemberField keyField = GeneratePublicField("Key", keyType);

			//Add start region directive 
			//#region Public Fields
			keyField.StartDirectives.Add(new CodeRegionDirective(CodeRegionMode.Start, "Public Fields"));
			nodeClass.Members.Add(keyField);

			//Add field
			//public T item;
			CodeMemberField itemField = GeneratePublicField("Item", itemType);
			nodeClass.Members.Add(itemField);

			//Add field 
			//public Node<K,T> NextNode
			CodeMemberField nextNodeField = GeneratePublicField("NextNode", nodeType);

			//Add end region directive
			//#endregion
			nextNodeField.EndDirectives.Add(new CodeRegionDirective(CodeRegionMode.End, "End"));
			nodeClass.Members.Add(nextNodeField);

			//Add default constructor
			#region C# code
			//Node()
			//{
			//	Key = K.default;
			//	Item = new T();
			//	NextNode = null;
			//}
			#endregion

			CodeStatementCollection defConstructorColl = new CodeStatementCollection();
			
			//this.Key = K.default
			defConstructorColl.Add(
				new CodeAssignStatement(
					new CodeFieldReferenceExpression(
						new CodeThisReferenceExpression(),
						"Key"), 
				new CodeDefaultValueExpression(keyType)));

			//this.item = new T()
			defConstructorColl.Add(
				new CodeAssignStatement(
					new CodeFieldReferenceExpression(
						new CodeThisReferenceExpression(), 
						"Item"), 
					new CodeObjectCreateExpression(itemType, new CodeExpression[]{}))
				);

			//this.NextNode = null
			defConstructorColl.Add(
				new CodeAssignStatement(
					new CodeFieldReferenceExpression(
						new CodeThisReferenceExpression(), 
						"NextNode"), 
					new CodePrimitiveExpression(null))
				);


			CodeConstructor defConstructor = GeneratePublicConstructor("Node", new CodeTypeReference[]{}, new string[]{}, defConstructorColl);
			nodeClass.Members.Add(defConstructor);

			//Add parameterized constructor
			#region C# code
			//Node(K keyPar, T itemPar, Node<K,T> nextNodePar)
			//{
			//	Key = keyPar;
			//	Item = itemPar;
			//	NextNode = nextNodePar;
			//}
			#endregion

			CodeStatementCollection parmConstructorColl = new CodeStatementCollection();

			//this.Key = keyPar
			parmConstructorColl.Add(
				new CodeAssignStatement(
					new CodeFieldReferenceExpression(
						new CodeThisReferenceExpression(),
						"Key"),
					new CodeArgumentReferenceExpression("keyPar"))
				);

			//this.Item = itemPar
			parmConstructorColl.Add(
				new CodeAssignStatement(
					new CodeFieldReferenceExpression(
						new CodeThisReferenceExpression(),
						"Item"),
					new CodeArgumentReferenceExpression("itemPar"))
				);

			//this.NextNode = nextNodePar
			parmConstructorColl.Add(
				new CodeAssignStatement(
					new CodeFieldReferenceExpression(
						new CodeThisReferenceExpression(),
						"NextNode"),
					new CodeArgumentReferenceExpression("nextNodePar"))
				);

			CodeConstructor parmConstructor = GeneratePublicConstructor("Node", new CodeTypeReference[] { keyType, itemType, nodeType }, new string[] { "keyPar", "itemPar", "nextNodePar" }, parmConstructorColl);
			nodeClass.Members.Add(parmConstructor);

			TextWriter t = new StreamWriter(path + @"\Node." + provider.FileExtension , false);
			provider.GenerateCodeFromNamespace(ns, t, options);
			t.Close();
		}

		//Code to generate a public method
		private CodeConstructor GeneratePublicConstructor(string name, CodeTypeReference[] parameters, string[] parameterNames, CodeStatementCollection statements)
		{
			CodeConstructor constructor = new CodeConstructor ();
			constructor.Attributes = (constructor.Attributes & ~MemberAttributes.AccessMask) | MemberAttributes.Public;
			constructor.Name = name;

			for (int i = 0; i < parameters.Length; i++)
			{
				constructor.Parameters.Add(new CodeParameterDeclarationExpression(parameters[i], parameterNames[i]));
			}
			constructor.Statements.AddRange(statements);

			return constructor;
		}

		//Generate code for class LinkedList
		private void GenerateLinkedList()
		{
			CodeNamespace ns = new CodeNamespace("GenericList");
			compileUnit.Namespaces.Add(ns);

			//Create class LinkedList<K,T>
			CodeTypeDeclaration listClassPartOne = new CodeTypeDeclaration("LinkedList");

			//Add type parameters to class
			listClassPartOne.TypeParameters.Add(keyTypeParameter);
			listClassPartOne.TypeParameters.Add(itemTypeParameter);

			//Make class partial 
			listClassPartOne.IsPartial = true;

			ns.Types.Add(listClassPartOne);

			//Add field
			//Node<K,T> header;
			CodeMemberField headerField = new CodeMemberField();

			headerField.Name = "header";
			headerField.Attributes = MemberAttributes.Private;
			headerField.Type = nodeType;
			listClassPartOne.Members.Add(headerField);

			//Default constructor
			#region C# code
			//public LinkedList()
			//{
			//	header = new Node<K,T>;
			//}
			#endregion
			CodeConstructor listDefConstructor = new CodeConstructor();

			listDefConstructor.Name = "LinkedList";
			listDefConstructor.Attributes = MemberAttributes.Public;

			//this.header
			CodeFieldReferenceExpression nodeRefExp = 
				new CodeFieldReferenceExpression(
						new CodeThisReferenceExpression(), 
						"header");

			//header = new Node<K,T>
			CodeAssignStatement varInitStmt =
				new CodeAssignStatement(
					nodeRefExp,
					new CodeObjectCreateExpression(nodeType, new CodeExpression[]{}));

			listDefConstructor.Statements.Add(varInitStmt);
			listClassPartOne.Members.Add(listDefConstructor);

			//Write to file
			TextWriter t = new StreamWriter(path + @"\List." + provider.FileExtension, false);
			provider.GenerateCodeFromNamespace(ns, t, options);
			t.Close();
		}

		//Generate the list interface
		private CodeTypeDeclaration GetListInterface()
		{
			//Create generic interface
			//interface ILinkedList<K,T>
			CodeTypeDeclaration linkedListInterface = new CodeTypeDeclaration("ILinkedList");
			
			//Add region start
			linkedListInterface.StartDirectives.Add(new CodeRegionDirective(CodeRegionMode.Start, "Generic interface"));
			
			//Add end region
			linkedListInterface.EndDirectives.Add(new CodeRegionDirective(CodeRegionMode.End, "Generic interface"));
			
			//Make interface
			linkedListInterface.IsInterface = true;
			
			//Add types to generic interface
			linkedListInterface.TypeParameters.Add(keyTypeParameter);
			linkedListInterface.TypeParameters.Add(itemTypeParameter);

			//Add method
			//T Find(K key);
			CodeMemberMethod findInterfaceMethod = GeneratePublicMethod("Find", itemType, new CodeTypeReference[] { keyType }, new string[] { "key" }, new CodeStatementCollection());
			linkedListInterface.Members.Add(findInterfaceMethod);

			//Add Method AddHead
			//public void AddHead(K key,T item)
			CodeMemberMethod addHeadInterfaceMethod = GeneratePublicMethod("AddHead", null, new CodeTypeReference[] { keyType, itemType }, new string[] { "key", "item" }, new CodeStatementCollection());
			linkedListInterface.Members.Add(addHeadInterfaceMethod);

			return linkedListInterface;
		}

		//Generate code for implementation of List interface
		private void GenerateListInterface()
		{
			CodeNamespace ns = new CodeNamespace("GenericList");
			compileUnit.Namespaces.Add(ns);
		
			ns.Types.Add(GetListInterface ());

			//Partial class LinkedList
			CodeTypeDeclaration listClassPartTwo = new CodeTypeDeclaration("LinkedList");

            //Add type parameters to class & mark partial
			listClassPartTwo.TypeParameters.Add(keyTypeParameter);
			listClassPartTwo.TypeParameters.Add(itemTypeParameter);
            listClassPartTwo.BaseTypes.Add(typeof(object));
            CodeTypeReference listInterface = new CodeTypeReference("ILinkedList", 
                                                                 new CodeTypeReference(keyTypeParameter), 
                                                                 new CodeTypeReference(itemTypeParameter));
            listClassPartTwo.BaseTypes.Add(listInterface);
            listClassPartTwo.IsPartial = true;

            //Add Method AddHead
			#region C# code
			//public void AddHead(K key,T item)
			//{
			//	Node<K,T> newNode = new Node<K,T>(key, item, header.NextNode);
			//	header.NextNode = newNode;
			//}
			#endregion
			CodeStatementCollection addHeadColl = new CodeStatementCollection();

			//header.NextNode
			CodeFieldReferenceExpression headerNextNodeRefExprn =
				new CodeFieldReferenceExpression(
					new CodeFieldReferenceExpression(
						new CodeThisReferenceExpression(),
						"header"),
					"NextNode");


			//new Node<K,T>(key, item, header.NextNode)
			CodeObjectCreateExpression newExpr =
				new CodeObjectCreateExpression(
				nodeType, 
				new CodeExpression[] { 
					new CodeArgumentReferenceExpression("key"), 
					new CodeArgumentReferenceExpression("item"), 
					headerNextNodeRefExprn 
					 });

			//Node<K,T> newNode = new Node<K,T>(key, item, header.NextNode)
			CodeVariableDeclarationStatement newNodeDecStmt =
				new CodeVariableDeclarationStatement(
					nodeType,
					"newNode",
					newExpr);

			//Header.NextNode = newNode
			CodeAssignStatement assignStmt =
				new CodeAssignStatement(
					headerNextNodeRefExprn, 
					new CodeVariableReferenceExpression("newNode"));

			addHeadColl.Add(newNodeDecStmt);
			addHeadColl.Add(assignStmt);

			//Generate AddHead
			CodeMemberMethod addHeadMethod = GeneratePublicMethod("AddHead", null, new CodeTypeReference[] { keyType, itemType }, new string[] { "key", "item" }, addHeadColl);
            addHeadMethod.ImplementationTypes.Add(listInterface);
            listClassPartTwo.Members.Add(addHeadMethod);

			//Add method Find
			#region C# code
			//public T Find(K key)
			//{
			//	 for (Node<K,T> current = this.header; (current != null); current = current.NextNode) {
			//		if ((current.Key.CompareTo(key) == 0))
			//		{
			//			return current.Item;
			//		}
			//}
			#endregion
			CodeStatementCollection findColl = new CodeStatementCollection();

			//current.Key
			CodeFieldReferenceExpression fieldRefExpr = 
				new CodeFieldReferenceExpression(
					new CodeVariableReferenceExpression("current"),
					"Key");

			//current.Key.CompareTo(key) 
			CodeMethodInvokeExpression compareToMethodInvStmt = 
				new CodeMethodInvokeExpression(
					fieldRefExpr,
					"CompareTo", 
					new CodeExpression[] 
						{ new CodeArgumentReferenceExpression("key") });

			//(current.Key.CompareTo(key) == 0)
			CodeBinaryOperatorExpression ifCondExpr = 
				new CodeBinaryOperatorExpression(
					compareToMethodInvStmt ,
					CodeBinaryOperatorType.ValueEquality,
					new CodePrimitiveExpression(0));

			//return current.Item
			CodeMethodReturnStatement retStmt =
				 new CodeMethodReturnStatement(new CodeFieldReferenceExpression(
								new CodeVariableReferenceExpression("current"), "Item"));

			//if ((current.Key.CompareTo(key) == 0))
			CodeConditionStatement ifStatement = new CodeConditionStatement(
				ifCondExpr,
				new CodeStatement[] {retStmt}
				);

			//this.header
			CodeFieldReferenceExpression headerRefExpr = 
				new CodeFieldReferenceExpression(
					new CodeThisReferenceExpression(),
					"header");

			//Node<K,T> current = this.header
			CodeVariableDeclarationStatement indexDeclrnStmt = 
				new CodeVariableDeclarationStatement(
					nodeType, 
					"current", 
					headerRefExpr);
			
			//current != null
			CodeBinaryOperatorExpression condExpr = 
				new CodeBinaryOperatorExpression(
					new CodeVariableReferenceExpression("current"),
					CodeBinaryOperatorType.IdentityInequality,
					new CodePrimitiveExpression(null));

			//current = current.NextNode
			CodeAssignStatement incrementStmt = 
				new CodeAssignStatement(
					new CodeVariableReferenceExpression("current"), 
					new CodeFieldReferenceExpression(
						new CodeVariableReferenceExpression("current"),
						"NextNode")
					);

			//for (Node<K,T> current = this.header; (current != null); current = current.NextNode)
			CodeIterationStatement forStatement = 
				new CodeIterationStatement(
					indexDeclrnStmt,
					condExpr,
					incrementStmt,
					new CodeStatement[] { ifStatement });


			findColl.Add(forStatement);

			//return this.header.Item
			CodeMethodReturnStatement retHdrStmt = 
				new CodeMethodReturnStatement(
					new CodeFieldReferenceExpression(
						new CodeFieldReferenceExpression(
							new CodeThisReferenceExpression(),
							"header"),
					 "Item")
					);

			findColl.Add(retHdrStmt);
				

			//Find method
			CodeMemberMethod findMethod = GeneratePublicMethod("Find", itemType, new CodeTypeReference[] { keyType }, new string[] { "key" }, findColl);
            findMethod.ImplementationTypes.Add(listInterface);
            listClassPartTwo.Members.Add(findMethod);

			//Add property
			#region C# code
			//public T this[K key]
			//{
			//	get
			//	{
			//		return Find(key);
			//	}
			//}
			#endregion
			CodeMemberProperty itemProperty = new CodeMemberProperty();
			itemProperty.Attributes = (itemProperty.Attributes & ~MemberAttributes.AccessMask) | MemberAttributes.Public;
			itemProperty.Parameters.Add(new CodeParameterDeclarationExpression(keyType, "key"));
			itemProperty.Name = "Item";
			itemProperty.GetStatements.Add(
				new CodeMethodReturnStatement(
					new CodeMethodInvokeExpression(
						new CodeMethodReferenceExpression(
							new CodeThisReferenceExpression(), 
							"Find"), 
						new CodeArgumentReferenceExpression("key")))
				);

			itemProperty.Type = itemType;
			listClassPartTwo.Members.Add(itemProperty);

			ns.Types.Add(listClassPartTwo);
			

			TextWriter tw = new StreamWriter(path + @"\ListInterface." + provider.FileExtension, false);
			provider.GenerateCodeFromNamespace(ns, tw, options);
			tw.Close();
		}

		//Code to generate Main method
		private void GenerateMain()
		{
			//Generate namespace
			CodeNamespace ns = new CodeNamespace("GenericList");
			
			//Generate MainClass
			CodeTypeDeclaration mainClass = new CodeTypeDeclaration("GenericList");
			ns.Types.Add(mainClass);

			mainClass.Members.Add(GetGenericMethod());

			//Generate main method
			#region C# code
			//	public static void Main() {
			//               System.Double x = 12;
			//               System.Double y = 10;
			//               LinkedList<int,Object> newList = new LinkedList<int,Object>();
			//               newList.AddHead(1, x);
			//               newList.AddHead(2, y);
			//               bool entry1greater = global::GenericList.GenericList.isGreater<double>(x, y);
			//               if (entry1greater) {
			//                    System.Console.WriteLine("Item 1 has higher value: {0}", newList[1]);
			//               }
			//               else {
			//                    System.Console.WriteLine("Item 2 has higher value: {0}", newList[2]);
			//               }
			//          }
			#endregion

			//Generate the entry point for the project
			CodeEntryPointMethod mainMethod = new CodeEntryPointMethod();
			mainMethod.Name = "Main";
			mainMethod.Attributes = (mainMethod.Attributes & ~MemberAttributes.AccessMask) | MemberAttributes.Public | MemberAttributes.Static;
			mainClass.Members.Add(mainMethod);

			//double x = 12.4;
			mainMethod.Statements.Add(
				new CodeVariableDeclarationStatement(
					new CodeTypeReference (typeof(double)),
					"x",
					new CodePrimitiveExpression(12.4))
				);

			//double y = 10;
			mainMethod.Statements.Add(
				new CodeVariableDeclarationStatement(
					new CodeTypeReference(typeof(double)),
					"y",
					new CodePrimitiveExpression(10))
				);

            // Build a reference to LinkedList<int, object>
			CodeTypeReference linkedListType = new CodeTypeReference("LinkedList",
                                                                     new CodeTypeReference(typeof(int)),
                                                                     new CodeTypeReference(typeof(object)));
			
			//LinkedList <int,Object> newList = new LinkedList<int,Object>()
			mainMethod.Statements.Add(
				new CodeVariableDeclarationStatement(
					linkedListType,
					"newList",
					new CodeObjectCreateExpression(
						linkedListType,
						new CodeExpression[] {})
					)
				);

			//newList.AddHead(1,x)
			mainMethod.Statements.Add(
				new CodeMethodInvokeExpression(
					new CodeVariableReferenceExpression("newList"),
					"AddHead",
					new CodeExpression[] { 
						new CodePrimitiveExpression(1), 
						new CodeVariableReferenceExpression("x") }
					)
				);

			//newList.AddHead(1,x)
			mainMethod.Statements.Add(
				new CodeMethodInvokeExpression(
					new CodeVariableReferenceExpression("newList"),
					"AddHead",
					new CodeExpression[] {
						new CodePrimitiveExpression(2), 
						new CodeVariableReferenceExpression("y") }
					)
				);

			// Global reference 
			// Call CodeTypeReference with CodeTypeReferenceOptions.GlobalReference set
			CodeTypeReferenceExpression globalExp = new CodeTypeReferenceExpression(
				new CodeTypeReference("GenericList.GenericList", CodeTypeReferenceOptions.GlobalReference));

            //isGreater<double>(x,y)
            CodeMethodInvokeExpression methodInvokeExp = 
				new CodeMethodInvokeExpression(
						new CodeMethodReferenceExpression(globalExp, 
                                                          "isGreater", 
                                                          new CodeTypeReference(typeof(double))),
						new CodeVariableReferenceExpression ("x"),
						new CodeVariableReferenceExpression ("y"));

			// bool entry1greater = isGreater<double>(x,y)
			mainMethod.Statements.Add(
				new CodeVariableDeclarationStatement(
					typeof(bool),
					"entry1greater",
			methodInvokeExp 		
			));


			CodeConditionStatement condStatement =
				new CodeConditionStatement(
					new CodeVariableReferenceExpression("entry1greater"),
					new CodeStatement[] {}
				);

			//newList[1]
			CodeIndexerExpression newList1Expr = 
				new CodeIndexerExpression(
					new CodeVariableReferenceExpression("newList"),
					new CodeExpression[] {new CodePrimitiveExpression(1)}
				);

			//newList[2]
			CodeIndexerExpression newList2Expr = new CodeIndexerExpression(
													new CodeVariableReferenceExpression("newList"),
													new CodeExpression[] {new CodePrimitiveExpression(2)});


			//Console.WriteLine("Item 1 has higher value: {0}", newList[1])
			condStatement.TrueStatements.Add(
				new CodeMethodInvokeExpression(
					new CodeTypeReferenceExpression(typeof(Console)),
					"WriteLine",
					new CodeExpression[] {
                        new CodePrimitiveExpression("Item 1 has higher value: {0}"),
                        newList1Expr
						}
					)
				);

			//Console.WriteLine("Item 2 has higher value: {0}", newList[2])
			condStatement.FalseStatements.Add(
				new CodeMethodInvokeExpression(
					new CodeTypeReferenceExpression(typeof(Console)),
					"WriteLine",
					new CodeExpression[] {
						new CodePrimitiveExpression("Item 2 has higher value: {0}"),
                        newList2Expr
						}
					)
				);

			mainMethod.Statements.Add(condStatement);

			TextWriter t = new StreamWriter(path + @"\Main." + provider.FileExtension, false);
			provider.GenerateCodeFromNamespace(ns, t, options);
			t.Close();
		}

		//Code to generate a public method
		private CodeMemberMethod GeneratePublicMethod(string name, CodeTypeReference returnType, CodeTypeReference [] parameters, string[] parameterNames, CodeStatementCollection statements)
		{
			CodeMemberMethod method = new CodeMemberMethod();
			method.Attributes = (method.Attributes & ~MemberAttributes.AccessMask) | MemberAttributes.Public;
			method.Name = name;

			if(returnType != null)
				method.ReturnType = returnType;

			for (int i = 0; i < parameters.Length; i++)
			{
				method.Parameters.Add(new CodeParameterDeclarationExpression(parameters[i], parameterNames[i]));
			}
			method.Statements.AddRange(statements);

			return method;
		}

		//Code to generate a field
		private CodeMemberField GeneratePublicField(string name, CodeTypeReference fieldType)
		{
			CodeMemberField field = new CodeMemberField();

			field.Name = name;
			field.Attributes = MemberAttributes.Public;
			field.Type = fieldType;

			return field;
		}

		//Generate a generic method
		private CodeMemberMethod GetGenericMethod()
		{
			//Generate generic method
			#region C# code
			//public static bool isGreater<T>(T item1, T item2) where T:System.IComparable
			//{
			//	if (item1.CompareTo(item2) > 0)
			//		return true;
			//	else
			//		return false;
			//}
			#endregion

			//Parameter type T for generic method 
			CodeTypeParameter methodTypeParameter = new CodeTypeParameter("T");

			//Add constraint for generic type
			methodTypeParameter.Constraints.Add(typeof(IComparable));
			CodeTypeReference methodType = new CodeTypeReference(methodTypeParameter);
			
			//return true;
			CodeMethodReturnStatement returnTrueStmt = new CodeMethodReturnStatement(new CodePrimitiveExpression(true));

			//return false;
			CodeMethodReturnStatement returnFalseStmt = new CodeMethodReturnStatement(new CodePrimitiveExpression(false));
			
			//item1.CompareTo(item2)
			CodeMethodInvokeExpression methodInvokeExpr = new CodeMethodInvokeExpression(
						new CodeMethodReferenceExpression(new CodeArgumentReferenceExpression("item1"), "CompareTo"),
						new CodeExpression[] { new CodeArgumentReferenceExpression("item2") });

			//(item1.CompareTo(item2) > 0)
			CodeBinaryOperatorExpression condExpr = new CodeBinaryOperatorExpression(
					methodInvokeExpr ,
					CodeBinaryOperatorType.GreaterThan,
					new CodePrimitiveExpression(0));

			//Add to statement collection
			CodeStatementCollection isGreaterColl = new CodeStatementCollection();
			isGreaterColl.Add(new CodeConditionStatement(
				condExpr,
				new CodeStatement[] { returnTrueStmt },
				new CodeStatement[] { returnFalseStmt}));

			//Generate method
			CodeMemberMethod isGreaterMethod = GeneratePublicMethod("isGreater", new CodeTypeReference(typeof(bool)), new CodeTypeReference[] { methodType, methodType }, new string[] { "item1", "item2" }, isGreaterColl);
			
			//Add type parameter to method to make it generic
			isGreaterMethod.TypeParameters.Add(methodTypeParameter);
			
			//make method static
			isGreaterMethod.Attributes = isGreaterMethod.Attributes | MemberAttributes.Static;

			return isGreaterMethod;
		}

	}
}
