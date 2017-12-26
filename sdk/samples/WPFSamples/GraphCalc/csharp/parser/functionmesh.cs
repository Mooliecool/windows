/***********************************************************
 *
 *   Copyright (c) Microsoft Corporation, 2004
 *
 *   Description:   Generate a mesh given parametric equations that
 *                  define the x,y,and z coordinates over a specified
 *                  range of values
 *   Created:       8/19/04
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
    public class FunctionMesh
    {
        protected FunctionMesh()
        {
            // We open this class up for extension only by those who promise to call Init
            //  at some time during their constructors.  If they do not, CreateMesh will fail.
        }

        public FunctionMesh( 
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
        public MeshGeometry3D CreateMesh( int precisionU, int precisionV )
        {
            lengthX = precisionU;
            lengthY = precisionV;
            du = ( uMax - uMin ) / (double)(precisionU-1);
            dv = ( vMax - vMin ) / (double)(precisionV-1);

            positions = new Point3D[ lengthX, lengthY ];
            normals = new Vector3D[ lengthX, lengthY ];
            textureCoords = new System.Windows.Point[ lengthX, lengthY ];
            indices = new ArrayList();

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
                    normals[ x,y ] = GetNormal( u,v );
                    u += du;
                }
                v += dv;
            }
            VariableExpression.Undefine( "u" );
            VariableExpression.Undefine( "v" );

            SetTextureCoordinates();
            SetIndices();

            MeshGeometry3D mesh = new MeshGeometry3D();
            for ( int y = 0; y < lengthY; y++ )
            {
                for ( int x = 0; x < lengthX; x++ )
                {
                    mesh.Positions.Add( positions[ x,y ] );
                    mesh.Normals.Add( normals[ x,y ] );
                    mesh.TextureCoordinates.Add( textureCoords[ x,y ] );
                }
            }
            mesh.TriangleIndices = new Int32Collection();
            foreach (int index in indices)
                mesh.TriangleIndices.Add(index);

            return mesh;
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

        /// <summary>
        /// Returns the normal of the mesh at the indicated parameters.
        /// </summary>
        private Vector3D GetNormal( double u, double v )
        {
            Vector3D a = ToVector3D( fxDu, fyDu, fzDu );
            Vector3D b = ToVector3D( fxDv, fyDv, fzDv );

            Vector3D normal = Vector3D.CrossProduct( a, b );
            normal.Normalize();

            if ( double.IsNaN( normal.X ) || double.IsNaN( normal.Y ) || double.IsNaN( normal.Z ) )
            {
                // This is sort-of what Michael Kallay said to do in his paper, I think...
                // It seems to work okay.

                // We take a second derivative of the equation, this time using the other variable,
                //  then we re-evaluate it.  The direction of the normal is dependent on where we
                //  are in the mesh grid
                if ( a.Length < b.Length )
                {
                    a = TryToVector3DAgain( fxDu, fyDu, fzDu, "v" );
                    if ( v < (vMax+vMin)/2 )
                    {
                        normal = Vector3D.CrossProduct( a,b );
                    }
                    else
                    {
                        normal = Vector3D.CrossProduct( b,a );
                    }
                }
                else
                {
                    b = TryToVector3DAgain( fxDv, fyDv, fzDv, "u" );
                    if ( u < (uMax+uMin)/2 )
                    {
                        normal = Vector3D.CrossProduct( a,b );
                    }
                    else
                    {
                        normal = Vector3D.CrossProduct( b,a );
                    }
                }

                normal.Normalize();
                if ( double.IsNaN( normal.X ) || double.IsNaN( normal.Y ) || double.IsNaN( normal.Z ) )
                {
                    Debug.Assert( false, "Persistent degenerate normal.  Bailing out!" );
                    normal = new Vector3D( 0,1,0 );
                }
            }
            return normal;
        }

        private Vector3D ToVector3D( 
            IExpression fx, 
            IExpression fy, 
            IExpression fz
            )
        {
            return new Vector3D( fx.Evaluate(), fy.Evaluate(), fz.Evaluate() );
        }

        private Vector3D TryToVector3DAgain(
            IExpression fx,
            IExpression fy,
            IExpression fz,
            string otherVar
            )
        {
            IExpression fxDn = fx.Differentiate( otherVar ).Simplify();
            IExpression fyDn = fy.Differentiate( otherVar ).Simplify();
            IExpression fzDn = fz.Differentiate( otherVar ).Simplify();

            return ToVector3D( fxDn, fyDn, fzDn );
        }

        private void SetTextureCoordinates()
        {
            double scaleU = 1.0/(double)(lengthX-1);
            double scaleV = 1.0/(double)(lengthY-1);
            double v = 0.0;

            for ( int y = 0; y < lengthY; y++ )
            {
                double u = 0.0;
                if ( y == lengthY-1 )
                {
                    v = 1.0;
                }
                for ( int x = 0; x < lengthX; x++ )
                {
                    if ( x == lengthX-1 )
                    {
                        u = 1.0;
                    }
                    // WPF textures things upside down
                    textureCoords[ x,y ] = new System.Windows.Point( u, 1.0 - v );
                    u += scaleU;
                }
                v += scaleV;
            }
        }

        private void SetIndices()
        {
            // Connect the dots (vertices)...

            for ( int y = 0; y < lengthY-1; y++ )
            {
                int rowX = y*lengthX;
                int nextRowX = rowX+lengthX;
                for ( int x = 0; x < lengthX-1; x++ )
                {
                    indices.Add( rowX + x );
                    indices.Add( rowX + x+1 );
                    indices.Add( nextRowX + x );
                    indices.Add( nextRowX + x+1 );
                    indices.Add( nextRowX + x );
                    indices.Add( rowX + x+1 );
                }
            }
            Debug.Assert( indices.Count == (lengthX-1)*(lengthY-1)*6, "indices = "+indices.Count );
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
        private Vector3D[,] normals;
        private System.Windows.Point[,] textureCoords;
        private ArrayList indices;
    }
}
