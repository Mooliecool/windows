//------------------------------------------------------------------------------
// <copyright file="listbuilder.cs" company="Microsoft">
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


//  This sample demonstrates how to use the CodeDom to generate a strongly typed collection

using System;
using System.Collections;
using System.CodeDom;
using System.CodeDom.Compiler;
using System.IO;
using System.Reflection;
using Microsoft.CSharp;

public class ListBuilder 
{
  private static string usage = "listbuilder <TypeName> -N:<Namespace>";

  public static void Main (string [] args) 
  {
    if (args.Length < 1) 
    {
      Console.WriteLine (usage);
      Console.WriteLine ();
      Console.WriteLine ("Press enter to continue...");
      Console.ReadLine();
      return;
    } //if
	
    string typeName = args[0];
    string ns = null;
    string suffix = "cs";
    string compileLine = "csc /t:library {0}";
    ICodeGenerator cg =new CSharpCodeProvider().CreateGenerator();

    foreach (string arg in args) 
    {
      if (arg.ToUpper().StartsWith ("-?") || arg.ToUpper().StartsWith ("/?")) 
      {
        Console.WriteLine (usage);
        Console.WriteLine ();
        Console.WriteLine ("Press enter to continue...");
        Console.ReadLine();
        return;
      } //if

      if (arg.ToUpper().StartsWith ("-N:") || arg.ToUpper().StartsWith ("/N:")) 
      {
        ns = arg.Remove(0,3);
      } //if
    } //foreach

    string fileName = typeName + "Collection." + suffix;
    Console.WriteLine ("Creating source file {0}.", fileName);
    if (compileLine != null)
    {
      Console.WriteLine ("compile with: '{0}'.", string.Format (compileLine, fileName));
    } //if

    TextWriter t = new StreamWriter (new FileStream (fileName, FileMode.Create));
    SpitList (t, typeName, cg, ns);
    t.Close();
  } //Main()

  public  static void SpitList (TextWriter w, string typeName, ICodeGenerator  baseCompiler, string ns) 
  {

    CodeCommentStatement c = new CodeCommentStatement  (string.Format ("List of {0}", typeName));
    baseCompiler.GenerateCodeFromStatement (c, w, null);

    CodeNamespace cnamespace = new CodeNamespace("Microsoft.Samples");
    cnamespace.Imports.Add (new CodeNamespaceImport ("System") );
    cnamespace.Imports.Add (new CodeNamespaceImport ("System.Collections") );
    if (ns != null &&  ns != "") cnamespace.Imports.Add (new CodeNamespaceImport (ns) );
    CodeTypeDeclaration co = new CodeTypeDeclaration (typeName +"List");
    co.IsClass = true;
    cnamespace.Types.Add (co);
    co.BaseTypes.Add (typeof (CollectionBase) );
    co.TypeAttributes  = TypeAttributes.Public;


//Generates: public <TYPE> this[int index] {
    //      get {
    //         return ((<TYPE>)List[index]);
    //      }
    //      set {
    //          List[index] = value;
    //      }
    //  }
    CodeMemberProperty  cp = new CodeMemberProperty ();
    cp.Name = "Item";
    cp.Attributes = MemberAttributes.Public | MemberAttributes.Final ;
    cp.Type = new CodeTypeReference(typeName);
    cp.Parameters.Add (new CodeParameterDeclarationExpression (new CodeTypeReference(typeof(int)), "index"));
    cp.GetStatements.Add (new CodeMethodReturnStatement (new CodeCastExpression (typeName, new CodeIndexerExpression (new CodeFieldReferenceExpression  (new CodeThisReferenceExpression(), "List"), new CodeArgumentReferenceExpression   ("index")))));
    cp.SetStatements.Add (new CodeAssignStatement (new CodeIndexerExpression (new CodeFieldReferenceExpression  (new CodeThisReferenceExpression(),"List"), new CodeArgumentReferenceExpression   ("index")), new CodeArgumentReferenceExpression  ("value")));
    co.Members.Add (cp);


    //Gen: public int Add(<TYPE> value) {
    //        return List.Add(value);
    //      }
    CodeMemberMethod cm = new CodeMemberMethod ();
    cm.Name = "Add";
    cm.ReturnType = new CodeTypeReference(typeof(int));
    cm.Parameters.Add (new CodeParameterDeclarationExpression (typeName, "value"));
    cm.Attributes = MemberAttributes.Public | MemberAttributes.Final ;
    cm.Statements.Add (new CodeMethodReturnStatement (new CodeMethodInvokeExpression (new CodeFieldReferenceExpression  (new CodeThisReferenceExpression(),"List"), "Add", 
      new CodeArgumentReferenceExpression  ("value"))));
    co.Members.Add (cm);

    //Gen: public void Insert(int index, <TYPE> value)
    //    {
    //         List.Insert(index, info);
    //     }
    cm = new CodeMemberMethod ();
    cm.Name = "Insert";
    cm.ReturnType = new CodeTypeReference(typeof(void));
    cm.Parameters.Add (new CodeParameterDeclarationExpression (new CodeTypeReference(typeof(int)), "index"));
    cm.Parameters.Add (new CodeParameterDeclarationExpression (typeName, "value"));
    cm.Attributes = MemberAttributes.Public | MemberAttributes.Final ;
    cm.Statements.Add (new CodeMethodInvokeExpression (new CodeFieldReferenceExpression  (new CodeThisReferenceExpression(),"List"), "Insert", 
      new CodeArgumentReferenceExpression  ("index"), new CodeArgumentReferenceExpression  ("value")));
    co.Members.Add (cm);


    //Gen: public int IndexOf(<TYPE> value) 
    //      {
    //        return List.IndexOf(value);
    //      }
    cm = new CodeMemberMethod ();
    cm.Name = "IndexOf";
    cm.ReturnType = new CodeTypeReference(typeof(int));
    cm.Parameters.Add (new CodeParameterDeclarationExpression (typeName, "value"));
    cm.Attributes = MemberAttributes.Public | MemberAttributes.Final ;
    cm.Statements.Add (new CodeMethodReturnStatement (new CodeMethodInvokeExpression (new CodeFieldReferenceExpression  (new CodeThisReferenceExpression(),"List"), "IndexOf", new CodeExpression []
                {new CodeArgumentReferenceExpression  ("value")})));
    co.Members.Add (cm);

    //Gen: public bool Contains(<TYPE> value) 
    //      {
    //        return List.Contains(value);
    //      }
    cm = new CodeMemberMethod ();
    cm.Name = "Contains";
    cm.ReturnType = new CodeTypeReference(typeof(bool));
    cm.Parameters.Add (new CodeParameterDeclarationExpression (typeName, "value"));
    cm.Attributes = MemberAttributes.Public | MemberAttributes.Final ;
    cm.Statements.Add (new CodeMethodReturnStatement (new CodeMethodInvokeExpression (new CodeFieldReferenceExpression  (new CodeThisReferenceExpression(),"List"), "Contains", new CodeExpression []
                {new CodeArgumentReferenceExpression  ("value")})));
    co.Members.Add (cm);

    //Gen: public void Remove(<TYPE> value) 
    //      {
    //       List.Remove(value);
    //      }
    cm = new CodeMemberMethod ();
    cm.Name = "Remove";
    cm.Parameters.Add (new CodeParameterDeclarationExpression (typeName, "value"));
    cm.Attributes = MemberAttributes.Public | MemberAttributes.Final ;
    cm.Statements.Add (new CodeMethodInvokeExpression (new CodeFieldReferenceExpression  (new CodeThisReferenceExpression(),"List"), "Remove", new CodeExpression []
                {new CodeArgumentReferenceExpression  ("value")}));
    co.Members.Add (cm);

    //Gen: public void CopyTo(<Type>[] array, int index) 
    //     {
    //         List.CopyTo(array, index);
    //     }
    cm = new CodeMemberMethod ();
    cm.Name = "CopyTo";
    cm.Parameters.Add (new CodeParameterDeclarationExpression (new CodeTypeReference(typeName, 1), "array"));
    cm.Parameters.Add (new CodeParameterDeclarationExpression (new CodeTypeReference(typeof(int)), "index"));
    cm.Attributes = MemberAttributes.Public | MemberAttributes.Final ;
    cm.Statements.Add (new CodeMethodInvokeExpression (new CodeFieldReferenceExpression  (new CodeThisReferenceExpression(),"List"), "CopyTo", new CodeExpression []
                {new CodeArgumentReferenceExpression  ("array"), new CodeArgumentReferenceExpression ("index")}));
    co.Members.Add (cm);

    baseCompiler.GenerateCodeFromNamespace (cnamespace, w, null);
  } //SpitList()
} //class ListBuilder


