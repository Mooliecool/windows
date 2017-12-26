/***********************************************************
 *
 *   Copyright (c) Microsoft Corporation, 2004
 *
 *   Description:   The Tokenizer converts a string representing
 *                  a mathematical function into symbolic tokens
 *                  used by the parser to create an expression tree.
 *   Created:       8/19/04
 *   Author:        Bob Brown (robbrow)
 *
 ************************************************************/

using System;
using System.Globalization;

namespace Tests
{
    //--------------------------------------------------------------

    public class Tokenizer
    {
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        public Tokenizer( string function )
        {
            if ( function == null )
            {
                function = string.Empty;
            }
            this.function = function;
            this.index = 0;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        public Token Next()
        {
            while ( index < function.Length )
            {
                if ( IsNumber( function[ index ] ) )
                {
                    string val = string.Empty;
                    val += function[ index++ ];
                    while ( index < function.Length && IsNumber( function[ index ] ) )
                    {
                        val += function[ index++ ];
                    }
                    return new Token( val, TokenType.Constant );
                }
                if ( IsAlpha( function[ index ] ) )
                {
                    string var = string.Empty;
                    var += function[ index++ ];
                    while ( index < function.Length && IsAlpha( function[ index ] ) )
                    {
                        var += function[ index++ ];
                    }
                    switch ( var.ToLower( CultureInfo.InvariantCulture ) )
                    {
                    case "sin": return new Token( "sin", TokenType.Sine );
                    case "cos": return new Token( "cos", TokenType.Cosine );
                    case "tan": return new Token( "tan", TokenType.Tangent );
                    default:    return new Token( var, TokenType.Variable );
                    }
                }
                switch ( function[ index++ ] )
                {
                case ' ':
                case '\t':
                case '\r':
                case '\n':  continue;

                case '+':   return new Token( "+", TokenType.Plus );
                case '-':   return new Token( "-", TokenType.Minus );
                case '*':   return new Token( "*", TokenType.Multiply );
                case '/':   return new Token( "/", TokenType.Divide );
                case '^':   return new Token( "^", TokenType.Exponent );
                case '(':   return new Token( "(", TokenType.OpenParen );
                case ')':   return new Token( ")", TokenType.CloseParen );
                default:    throw new InvalidSyntaxException( "Invalid token '" + function[ index-1 ] + "' in function: " + function );
                }
            }
            return new Token( "", TokenType.EOF );
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        private bool IsAlpha( char c )
        {
            return ( ( 'a' <= c && c <= 'z' ) || ( 'A' <= c && c <= 'Z' ) );
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        private bool IsNumber( char c )
        {
            return ( '.' == c || ( '0' <= c && c <= '9' ) );
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        private string function;
        private int index;
    }
}
