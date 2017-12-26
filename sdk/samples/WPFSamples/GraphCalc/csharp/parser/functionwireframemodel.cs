/***********************************************************
 *
 *   Copyright (c) Microsoft Corporation, 2004
 *
 *   Description:   Generate a mesh given parametric equations that
 *                  define the x,y,and z coordinates over a specified
 *                  range of values
 *   Created:       8/19/04
 *   Author:        Bob Brown (robbrow)
 *
 ************************************************************/

using System;
using System.Collections;
using System.Diagnostics;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Media3D;

namespace Tests
{
    public class FunctionWireframeModel
    {
        protected FunctionWireframeModel()
        {
            // We open this class up for extension only by those who promise to call Init
            //  at some time during their constructors.  If they do not, CreateMesh will fail.
        }

        public FunctionWireframeModel( 
            string fx, 
            string fy, 
            string fz, 
            double uMin, 
            double uMax, 
            double vMin, 
            double vMax 
            )
        {
            Init( 
                FunctionParser.Parse( fx ), 
                FunctionParser.Parse( fy ), 
                FunctionParser.Parse( fz ), 
                uMin, 
                uMax, 
                vMin, 
                vMax 
                );
        }

        protected void Init( 
            IExpression fx, 
            IExpression fy, 
            IExpression fz, 
            double uMin, 
            double uMax, 
            double vMin, 
            double vMax 
            )
        {
            this.fx = fx.Simplify();
            this.fy = fy.Simplify();
            this.fz = fz.Simplify();

            fxDu = this.fx.Differentiate( "u" ).Simplify();
            fxDv = this.fx.Differentiate( "v" ).Simplify();
            fyDu = this.fy.Differentiate( "u" ).Simplify();
            fyDv = this.fy.Differentiate( "v" ).Simplify();
            fzDu = this.fz.Differentiate( "u" ).Simplify();
            fzDv = this.fz.Differentiate( "v" ).Simplify();

            this.uMin = uMin;
            this.uMax = uMax;
            this.vMin = vMin;
            this.vMax = vMax;
        }

        /// <summary>
        /// Creates a mesh with a horizontal and vertical resolution indicated
        /// by the input parameters.  It will generate 
        /// (precisionU-1)*(precisionV-1) quads.
        /// </summary>
        public Model3DGroup CreateWireframeModel( int precisionU, int precisionV )
        {
            lengthX = precisionU;
            lengthY = precisionV;
            du = ( uMax - uMin ) / (double)(precisionU-1);
            dv = ( vMax - vMin ) / (double)(precisionV-1);

            positions = new Point3D[ lengthX, lengthY ];

            double v = vMin;
            for ( int y = 0; y < lengthY; y++ )
            {
                double u = uMin;
                if ( y == lengthY-1 )
                {
                    v = vMax;
                }
                for ( int x = 0; x < lengthX; x++ )
                {
                    if ( x == lengthX-1 )
                    {
                        u = uMax;
                    }
                    VariableExpression.Define( "u", u );
                    VariableExpression.Define( "v", v );
                    positions[ x,y ] = Evaluate();
                    u += du;
                }
                v += dv;
            }
            VariableExpression.Undefine( "u" );
            VariableExpression.Undefine( "v" );

            Model3DGroup group = new Model3DGroup();
            
            // TODO: Remove
            //ScreenSpaceLines3D lines;
            /*
            // Create Horizontal lines
            for ( int y = 0; y < lengthY; y++ )
            {
                lines = new ScreenSpaceLines3D();
                lines.Color = Colors.Black;
                lines.Thickness = 1;
                for ( int x = 0; x < lengthX; x++ )
                {
                    lines.Points.Add( positions[ x,y ] );
                }
                group.Children.Add( lines );
            }

            // Create Vertical lines
            for ( int x = 0; x < lengthX; x++ )
            {
                lines = new ScreenSpaceLines3D();
                lines.Color = Colors.Black;
                lines.Thickness = 1;
                for ( int y = 0; y < lengthY; y++ )
                {
                    lines.Points.Add( positions[ x,y ] );
                }
                group.Children.Add( lines );
            }
            */

            return group;
        }

        /// <summary>
        /// Evaluates the mesh position at the indicated parameters.
        /// </summary>
        private Point3D Evaluate()
        {
            Point3D p = new Point3D();

            p.X = fx.Evaluate();
            p.Y = fy.Evaluate();
            p.Z = fz.Evaluate();

            return p;
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        protected double uMin;
        protected double uMax;
        protected double vMin;
        protected double vMax;
        protected Point3D this[ int x, int y ]
        {
            get
            {
                return positions[ x,y ];
            }
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - -

        private IExpression fx;
        private IExpression fy;
        private IExpression fz;

        private IExpression fxDu;
        private IExpression fxDv;
        private IExpression fyDu;
        private IExpression fyDv;
        private IExpression fzDu;
        private IExpression fzDv;

        private double du;
        private double dv;
        private int lengthX;
        private int lengthY;

        private Point3D[,] positions;
    }
}
