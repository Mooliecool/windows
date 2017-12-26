//------------------------------------------------------------------------------
// <copyright file="clisp.cs" company="Microsoft">
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
using System.IO;
using System.Collections;

class CLisp {
    String file;

    Lexer l;
    Parser p;
    CodeGen cg;

    public bool Lex()
    {
	l = new Lexer(file);
	return l.Lex();
    }    

    public bool Parse()
    {
	p = new Parser(l.tokens);
	return p.Parse();
    }    

    public void CodeGen(String filename)
    {
	cg = new CodeGen(p.e, p.Functions, p.GlobalVars);
	cg.Generate(filename);
    }
     
    public void ReadFile(String FileName, int FileSize)
    {
	char[] buf = new char[FileSize];
	StreamReader sr = new StreamReader(new FileStream(FileName, FileMode.Open, FileAccess.Read));
	int retval = sr.ReadBlock(buf, 0, FileSize);
	file = new String(buf);
	//	Console.WriteLine(file);
    }

    public static void Main(String[] args){
	String[] cmdline = args;
	Environment.ExitCode = (100);

	Console.WriteLine("Microsoft (R) CLisp - Demo Lisp Compiler \n" +
    			  "Copyright (c) 2000-2001,  Microsoft Corporation  All rights reserved.\n\n");
	
	if (cmdline.Length < 1 || args[0].Equals("/?") == true) {
		Console.WriteLine("Usage: CLisp <input file> ");
		return;
	}

	CLisp mine = new CLisp();

	try{
	    FileInfo[] fe = (new DirectoryInfo(".")).GetFiles(cmdline[0]);
	    if (fe.Length == 0){
		Console.WriteLine(cmdline[0] + ": file not found");
		return;	    
	    }
	    mine.ReadFile(cmdline[0], (int)fe[0].Length);
	}
	catch(IOException e){
	    Console.WriteLine("I/O error occured" + e);
	    Environment.ExitCode = (-1);
            return;
        }
	catch(Exception e){
	    Console.WriteLine("Either the FileName was incorrect or the feature is unsupported\n" + e);
	    Environment.ExitCode = (-1);
            return;
        }

	try{
	    if (!mine.Lex()){
		Queue tokens = new Queue(mine.l.tokens);
		while(tokens.Count > 0)
		    Console.WriteLine(tokens.Dequeue());
		Console.WriteLine("Lex Error. Stopping");
		Environment.ExitCode = (-1);
		return;
		
	    }
	}
	catch(Exception e){
	    Console.WriteLine("Unexpected LEX error: " + e);
	    Environment.ExitCode = (-1);
	    return;
	}

	try {
	    if (!mine.Parse()){
	    	    Console.WriteLine(mine.p.e);
		    Console.WriteLine("Parse Error. Stopping");
		    Environment.ExitCode = (-1);
		    return;
	    }	    
	}
	catch(Exception e){
	    Console.WriteLine("Unexpected Parse error:" + e);
	    Environment.ExitCode = (-1);
	    return;
	}
	
	try {
	    String name = cmdline[0].Substring(0, cmdline[0].LastIndexOf('.'));
	    mine.CodeGen(name);
	}
	catch(Exception e){
	    Console.WriteLine("Unexpected CodeGen error:" + e);
	    Environment.ExitCode = (-1);
	    return;
	}

    }
}
    
