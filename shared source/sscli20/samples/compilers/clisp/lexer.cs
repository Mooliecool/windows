//------------------------------------------------------------------------------
// <copyright file="lexer.cs" company="Microsoft">
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
using LexToken;

enum States {
    Number = 0,
    Operator = 2
}


class Lexer{
    public Queue tokens;
    String file;
    int FileSize;    
    int index;
    bool success;

    public Lexer(String f)
    {
	tokens = new Queue();
	file = f;
	FileSize = file.Length;
	success = true;
    }

    public bool Lex()
    {
	index = 0;
	//Avoiding a overflow of buffer during lookahead
	//Mind that the FileSize is only initialized in the beginning(constructor).
	file = file + "\n";  
	
	while(index < FileSize){	    
	    Token t = GetNextToken();
	    if (t !=null)
		tokens.Enqueue(t);	    
	}
	//This adds the EOFSYM for the Parser
	tokens.Enqueue(new Token(TokenType.EOFSYM));
	
	return success;
	
    }

    Token GetNextToken()
    {
	String value;
	
	    switch (file[index]){
	    case '\n':
	    case '\r':
	    case '\t':
	    case ' ':
		index++;
		return null;
	    case ';':
		index++;
		while (file[index] != '\n'){
		    index++;
		}
		return null;
		
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
		value = Convert.ToString(file[index]);
		index++;
		while (Char.IsDigit(file[index])){
		    value = value + file[index];
		    index++;
		}
		return new NumberToken(value);
	    case '<':
		index++;
		if (file[index] == '='){
		    index++;
		    return new Token(TokenType.LE);
		}
		else
		return new Token(TokenType.LT);
	    case '>':
		index++;
		if (file[index] == '='){
		    index++;
		    return new Token(TokenType.GE);
		}
		else
		    return new Token(TokenType.GT);
	    case '=':
		index++;
		return new Token(TokenType.EQ);
	    case '+':
		index++;
		if (Char.IsDigit(file[index])){
		    value = Convert.ToString('+');
		    while(Char.IsDigit(file[index])){
			value = value + file[index];
			index++;
		    }
		    return new NumberToken(value);
		}
		else    
		    return new Token(TokenType.PLUS);

       	    case '-':
		index++;
		if (Char.IsDigit(file[index])){
		    value = Convert.ToString('-');
		    while(Char.IsDigit(file[index])){
			value = value + file[index];
			index++;
		    }
		    return new NumberToken(value);
		}
		else    
		    return new Token(TokenType.MINUS);

	    case '*':
		index++;
		return new Token(TokenType.MUL);
	    case '/':
		index++;
		return new Token(TokenType.DIVIDE);
	    case '(':
		index++;
		return new Token(TokenType.LPAREN);
	    case ')':
		index++;
		return new Token(TokenType.RPAREN); 
	    case '\'':
		index++;
		value = null;
		if (Char.IsLetterOrDigit(file[index])){
		    value = file[index].ToString();
		    index++;
		    while(Char.IsLetterOrDigit(file[index])){
			value = value + file[index];
			index++;
		    }
		    if (value.CompareTo("NIL") == 0)
			return new Token(TokenType.NIL);
		    if (value.ToUpper().CompareTo(value.ToLower()) == 0)
			return new NumberToken(value);
		}
		else if(file[index] == '('){
		    value = "(";
		    index++;
		    int count = 1;
		    while(file[index] != ')' || --count > 0){
			if(file[index] == '(')
			    count++;
			value = value + file[index];
			index++;
		    }
		    value = value + ")";
		    index++;
		}
		return new StringToken(value);
		
	    default:
		if(Char.IsLetter(file[index])){
		       value = Convert.ToString(file[index]);
		       index++;		       
		       while(Char.IsLetterOrDigit(file[index])){
			   value = value + file[index];
			   index++;
		       }
		       if(value.CompareTo("cdr") == 0)
			   return new Token(TokenType.CDR);
		       if(value.CompareTo("car") == 0)
			   return new Token(TokenType.CAR);
		       if (value.CompareTo("cons") == 0)
			   return new Token(TokenType.CONS);
		       if (value.CompareTo("subst") == 0)
			   return new Token(TokenType.SUBST);
		       if (value.CompareTo("defun") == 0)
			   return new Token(TokenType.DEFUN);
		       if (value.CompareTo("setq") == 0)
			   return new Token(TokenType.SETQ);
		       if (value.CompareTo("null") == 0)
			   return new Token(TokenType.NULL);
		       if (value.CompareTo("if") == 0)
			   return new Token(TokenType.IF);
		       if (value.CompareTo("atom") == 0)
			   return new Token(TokenType.ATOM);
		       if (value.CompareTo("do") == 0)
			   return new Token(TokenType.DO);
		       //else
			   return new AlphaToken(value);			   
			   
		}
		else{
		    Console.WriteLine("Error in Input");
		    index++;
		    success = false;
		    return null;
		   }
		
	    }
    }
}

