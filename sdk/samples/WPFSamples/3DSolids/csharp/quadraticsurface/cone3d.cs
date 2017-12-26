using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Media3D;

namespace Primitive3DSurfaces
{
    public sealed class Cone3D : Primitive3D
    {
        internal Point3D GetPosition(double t, double y)
        {
            double r = (1 - y) / 2;
            double x = r * Math.Cos(t);
            double z = r * Math.Sin(t);

            return new Point3D(x, y, z);
        }

        private Vector3D GetNormal(double t, double y)
        {
            double x = 2 * Math.Cos(t);
            double z = 2 * Math.Sin(t);

            return new Vector3D(x, 1, z);
        }

        private Point GetTextureCoordinate(double t, double y)
        {
            Matrix m = new Matrix();
            m.Scale(1 / (2 * Math.PI), -0.5);

            Point p = new Point(t, y);
            p = p * m;

            return p;
        }

        internal override Geometry3D Tessellate()
        {
            int tDiv = 32;
            int yDiv = 32;
            double maxTheta = DegToRad(360.0);
            double minY = -1.0;
            double maxY = 1.0;

            double dt = maxTheta / tDiv;
            double dy = (maxY - minY) / yDiv;

            MeshGeometry3D mesh = new MeshGeometry3D();

            for (int yi = 0; yi <= yDiv; yi++)
            {
                double y = minY + yi * dy;

                for (int ti = 0; ti <= tDiv; ti++)
                {
                    double t = ti * dt;

                    mesh.Positions.Add(GetPosition(t, y));
                    mesh.Normals.Add(GetNormal(t, y));
                    mesh.TextureCoordinates.Add(GetTextureCoordinate(t, y));
                }
            }

            for (int yi = 0; yi < yDiv; yi++)
            {
                for (int ti = 0; ti < tDiv; ti++)
                {
                    int x0 = ti;
                    int x1 = (ti + 1);
                    int y0 = yi * (tDiv + 1);
                    int y1 = (yi + 1) * (tDiv + 1);

                    mesh.TriangleIndices.Add(x0 + y0);
                    mesh.TriangleIndices.Add(x0 + y1);
                    mesh.TriangleIndices.Add(x1 + y0);

                    mesh.TriangleIndices.Add(x1 + y0);
                    mesh.TriangleIndices.Add(x0 + y1);
                    mesh.TriangleIndices.Add(x1 + y1);
                }
            }

            mesh.Freeze();
            return mesh;
        }
    }
}
