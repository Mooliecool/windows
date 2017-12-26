//------------------------------------------------------------------------------
// <copyright file="lispruntime.cs" company="Microsoft">
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

using System;
using System.Collections;

public class CList
{
    internal ArrayList la;
    
    public CList()
    {
	la = new ArrayList();
    }

    public CList(CList l)
    {
	la = new ArrayList(l.la);
    }
    

    public int Count 
    {
	get 
	    {
		return la.Count;
	    }
    }
    
    public Object Item(int index)
    {
	return la[index];
    }
    
    public void Add(Object o)
    {
	la.Add(o);
    }
    
    
}


public class LispRuntime
{
    public static CList Car(CList l)
    {
	if (l.la.Count == 0)
	    return new CList();
	if (l.la[0] is CList)
	    return (CList)l.la[0];
	CList newlist = new CList();
	newlist.Add(l.la[0]);
	return newlist;
    }

    public static CList Cdr(CList l)
    {
	if (l.la.Count == 0)
	    return new CList();
	
	CList newlist = new CList(l);
	newlist.la.RemoveRange(0, 1);
	return newlist;
    }
    
    public static CList Cons(CList l, CList r)
    {
	CList newlist = new CList(l);
	for(int i = 0; i < r.Count; i++)
	    newlist.la.Add(r.la[i]);
	return newlist;
    }

    public static CList Subst(CList r, CList f, CList exp)
    {
	String rstr = (String)r.la[0];
	String fstr = (String)f.la[0];
	
	for (int i = 0; i < exp.la.Count; i++){
	    if (exp.la[i] is CList){
		exp.la[i] = Subst(r, f, (CList)exp.la[i]);
	    }
	    else if (exp.la[i] is String)
		if(((String)exp.la[i]).CompareTo(fstr) == 0){
		    exp.la[i] = rstr;
		}
	}
	return exp;
    }
    
    private static CList InitMultiple(String str, ref int index)
    {
	CList newList = new CList();
	//String tempstr = str.SubString(1);
	//tempstr = str.SubString(0, str.Length - 1);
	String temp = null;
	bool InState = false;
	index++;
	
	while (index < str.Length){
	    if(Char.IsWhiteSpace(str[index])){
		if (InState){
		    newList.Add(temp);
		    temp = null;
		}
		index++;
		InState = false;
	    }
	    else if(str[index] == '('){
		newList.Add(InitMultiple(str, ref index));
	    }
	    else if(str[index] == ')'){
		if(InState)
		    newList.Add(temp);
		index++;
		return newList;
	    }
	    else{
		InState = true;
		temp = temp + str[index];
		index++;
	    }
	}
	Console.WriteLine("IlFormed Parens");
	return null;
    }

    public static CList Init(String item)
    {
	CList newlist = new CList();
	int index = 0;

	if (item.CompareTo(String.Empty) == 0)
	    return newlist;                 //Empty CList is NIL
	if(item[0] == '('){
	    return InitMultiple(item, ref index);
	}
	else
	    newlist.Add(item);

	return newlist;
    }
    

    public static void Print(CList item)
    {
	if (item.Count == 0){
	    Console.Write("NIL");
	    return;
	}
	
	if(item.Count > 1){
	    Console.Write("(");
	    for (int i = 0; i < item.Count; i++){
		if (item.la[i] is CList)
		    Print((CList)item.la[i]);
		else
		    Console.Write((String)item.la[i]);
		if (i < item.Count - 1)
		    Console.Write(" ");
	    }
	    Console.Write(")");
	}
	else{
	    if(item.la[0] is CList)
		Print((CList)item.la[0]);
	    else
		Console.Write((String)item.la[0]);
	}
     
    }

    public static void PrintBool(bool val)
    {
	if (val)
	    Console.WriteLine("T");
	else 
	    Console.WriteLine("NIL");
    }
    

    public static bool IsNull(CList value)
    {
	return (value.Count == 0);
    }

    public static bool IsAtom(CList value)
    {
	return (value.Count == 1);
    }
    
    public static int ToInt(CList l)
    {
	if (l.la.Count == 0)
	    Console.WriteLine("Somethings wrong");
	
	if (l.la[0] is String){
	    try {
		return Convert.ToInt32((String)l.la[0]);
	    }
	    catch(FormatException ){
		return 0;
	    }
	}
	else{
	    Console.WriteLine("Runtime Error 1: Cannot Convert to Int");
	    return 0;
	}
    }
    
    public static CList ToList(int Val)
    {
	CList c = new CList();
	c.la.Add(Convert.ToString(Val));
	return c;
    }
    
    
}

