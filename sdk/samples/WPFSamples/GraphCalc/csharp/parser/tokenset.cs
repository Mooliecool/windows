/***********************************************************
 *
 *   Copyright (c) Microsoft Corporation, 2004
 *
 *   Description:   Recognized symbols are defined by the TokenType enum.
 *                  The TokenSet is a mathematical set representing
 *                  a group of TokenTypes (no repeats).  It is used by
 *                  the parser during syntactical analysis to determine
 *                  if the current Token is acceptable or not.
 *   Created:       8/19/04
 *   Author:        Bob Brown (robbrow)
 *
 ************************************************************/

using System;

namespace Tests
{
    //--------------------------------------------------------------

    // TokenTypes are bit-exclusive so that we can easily group them
    //  together in sets using a bit-vector
    public enum TokenType
    {
        None = 0x0,
        Constant = 0x1,
        Variable = 0x2,
        Plus = 0x4,
        Minus = 0x8,
        Multiply = 0x10,
        Divide = 0x20,
        Exponent = 0x40,
        Sine = 0x80,
        Cosine = 0x100,
        Tangent = 0x200,
        OpenParen = 0x400,
        CloseParen = 0x800,
        EOF = 0x1000,
    }

    //--------------------------------------------------------------

    public class TokenSet
    {
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        public TokenSet( TokenType type )
        {
            this.tokens = (uint)type;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        public TokenSet( TokenSet t )
        {
            this.tokens = t.tokens;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        private TokenSet( uint tokens )
        {
            this.tokens = tokens;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        public static TokenSet operator +( TokenSet t1, TokenSet t2 )
        {
            return new TokenSet( (uint)t1.tokens | (uint)t2.tokens );
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        public static TokenSet operator +( TokenSet t1, TokenType t2 )
        {
            return new TokenSet( t1.tokens | (uint)t2 );
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        public bool Contains( TokenType type )
        {
            return ( tokens & (uint)type ) != 0;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        private uint tokens;
    }
}
