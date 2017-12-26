/***********************************************************
 *
 *   Copyright (c) Microsoft Corporation, 2004
 *
 *   Description:   Specialized exceptions that can be thrown while
 *                  parsing or differentiating a parametric equation.
 *   Created:       8/19/04
 *   Author:        Bob Brown (robbrow)
 *
 ************************************************************/

using System;

namespace Tests
{
    //--------------------------------------------------------------

    public class FunctionParserException : Exception
    {
        public FunctionParserException( string msg ) : base( msg )
        {
        }
        public FunctionParserException( string msg, Exception innerException ) : base( msg, innerException )
        {
        }
    }

    //--------------------------------------------------------------

    public class InvalidExpressionException : FunctionParserException
    {
        public InvalidExpressionException( string msg ) : base( msg )
        {
        }
        public InvalidExpressionException( string msg, Exception innerException ) : base( msg, innerException )
        {
        }
    }

    //--------------------------------------------------------------

    public class OutOfTokensException : FunctionParserException
    {
        public OutOfTokensException( string msg ) : base( msg )
        {
        }
    }

    //--------------------------------------------------------------

    public class InvalidSyntaxException : FunctionParserException
    {
        public InvalidSyntaxException( string msg ) : base( msg )
        {
        }
    }

    //--------------------------------------------------------------

    public class UnexpectedBehaviorException : FunctionParserException
    {
        public UnexpectedBehaviorException( string msg ) : base( msg )
        {
        }
    }

    //--------------------------------------------------------------

    public class TrailingTokensException : FunctionParserException
    {
        public TrailingTokensException( string msg ) : base( msg )
        {
        }
    }

    //--------------------------------------------------------------

    public class UndefinedVariableException : FunctionParserException
    {
        public UndefinedVariableException( string msg ) : base( msg )
        {
        }
    }

    //--------------------------------------------------------------

    public class CannotDifferentiateException : FunctionParserException
    {
        public CannotDifferentiateException( string msg ) : base( msg )
        {
        }
    }
}
