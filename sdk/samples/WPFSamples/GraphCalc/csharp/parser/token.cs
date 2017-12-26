/***********************************************************
 *
 *   Copyright (c) Microsoft Corporation, 2004
 *
 *   Description:   Storage for a portion of a mathematical
 *                  function.  The symbol's type is stored for
 *                  convenience during parsing.
 *   Created:       8/19/04
 *   Author:        Bob Brown (robbrow)
 *
 ************************************************************/

using System;

namespace Tests
{
    //--------------------------------------------------------------

    public struct Token
    {
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        public Token( string value, TokenType type )
        {
            this.value = value;
            this.type = type;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        public readonly string value;
        public readonly TokenType type;
    }
}
