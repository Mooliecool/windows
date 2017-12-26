/***********************************************************
 *
 *   Copyright (c) Microsoft Corporation, 2004
 *
 *   Description:   Common mesh geometries that can be described with
 *                  parametric equations.
 *   Created:       8/19/04
 *   Author:        Bob Brown (robbrow)
 *
 ************************************************************/

using System;
using System.Windows.Media.Media3D;

namespace Tests
{
    //--------------------------------------------------------------

    public sealed class         Sphere                      : FunctionMesh
    {
        public                  Sphere( double radius )
        {
            Init( new MultExpression( new ConstantExpression( radius ), FunctionParser.Parse( "cos(u)sin(v)" ) ),
                  new MultExpression( new ConstantExpression( radius ), FunctionParser.Parse( "-cos(v)" ) ),
                  new MultExpression( new ConstantExpression( radius ), FunctionParser.Parse( "sin(-u)sin(v)" ) ),
                  -Math.PI, Math.PI, 0.0, Math.PI );
        }
    }

    //--------------------------------------------------------------

    public sealed class         Ellipsoid                   : FunctionMesh
    {
        public                  Ellipsoid( double ax, double by, double cz )
        {
            Init( new MultExpression( new ConstantExpression( ax ), FunctionParser.Parse( "cos(u)sin(v)" ) ),
                  new MultExpression( new ConstantExpression( by ), FunctionParser.Parse( "-cos(v)" ) ),
                  new MultExpression( new ConstantExpression( cz ), FunctionParser.Parse( "sin(-u)sin(v)" ) ),
                  -Math.PI, Math.PI, 0.0, Math.PI );
        }
    }

    public sealed class         PlaneXY                     : FunctionMesh
    {
        public                  PlaneXY( double lengthX, double lengthY )
        {
            Init( new VariableExpression( "u" ),
                  new VariableExpression( "v" ),
                  new ConstantExpression( 0.0 ),
                  -lengthX/2.0, lengthX/2.0,
                  -lengthY/2.0, lengthY/2.0
                  );
        }
    }

    //--------------------------------------------------------------

    public sealed class         Cone                        : FunctionMesh
    {
        public                  Cone( double radius, double height )
        {
            IExpression fx = new MultExpression(
                                    new MultExpression(
                                        new SubExpression(
                                            new ConstantExpression( height ),
                                            new VariableExpression( "v" ) ),
                                        new ConstantExpression( radius/height ) ),
                                    new CosineExpression(
                                        new VariableExpression( "u" ) ) );
            IExpression fy = new VariableExpression( "v" );
            IExpression fz = new MultExpression(
                                    new MultExpression(
                                        new SubExpression(
                                            new ConstantExpression( height ),
                                            new VariableExpression( "v" ) ),
                                        new ConstantExpression( radius/height ) ),
                                    new SineExpression(
                                        new NegateExpression(
                                            new VariableExpression( "u" ) ) ) );

            Init( fx, fy, fz, -Math.PI, Math.PI, 0.0, height );
        }
    }

    //--------------------------------------------------------------

    public sealed class         Horn                        : FunctionMesh
    {
        public                  Horn( double scale )
        {
            Init( new MultExpression( new ConstantExpression( scale ), FunctionParser.Parse( "-(1+.15u cos(v))cos(u)" ) ),
                  new MultExpression( new ConstantExpression( scale ), FunctionParser.Parse( "(1+.15u cos(v))sin(u)" ) ),
                  new MultExpression( new ConstantExpression( scale ), FunctionParser.Parse( "-.15u sin(v)" ) ),
                  -Math.PI / 2.0, 0.0, 0.0, 2 * Math.PI );

        }
    }

    //--------------------------------------------------------------

    /*
    public class                Apple : Function
    {
        // This will throw.  I don't support log(x) function yet
        public                  Apple( double radius )
        {
            Init( new MultExpression( radius * 0.125, FunctionParser.Parse( "cos(u)(4+3.8cos(v))" ) ),
                  new MultExpression( radius * 0.125, FunctionParser.Parse( "(cos(v)+sin(-v)-1)(1+sin(-v))log(1-pi*v/10)+7.5sin(-v)" ) ),
                  new MultExpression( radius * 0.125, FunctionParser.Parse( "sin(-u)(4+3.8cos(v))" ) ),
                  -Math.PI, Math.PI, 0.0, Math.PI )
        }
    }
    */
}
