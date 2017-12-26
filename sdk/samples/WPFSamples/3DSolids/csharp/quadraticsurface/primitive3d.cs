using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Media3D;

namespace Primitive3DSurfaces
{
    public abstract class Primitive3D : ModelVisual3D
    {
        public Primitive3D()
        {
            Content = _content;
            _content.Geometry = Tessellate();
        }

        public static DependencyProperty MaterialProperty =
            DependencyProperty.Register(
                "Material",
                typeof(Material),
                typeof(Primitive3D), new PropertyMetadata(
                    null, new PropertyChangedCallback(OnMaterialChanged)));

        public Material Material
        {
            get { return (Material)GetValue(MaterialProperty); }
            set { SetValue(MaterialProperty, value); }
        }

        internal static void OnMaterialChanged(Object sender, DependencyPropertyChangedEventArgs e)
        {
            Primitive3D p = ((Primitive3D)sender);

            p._content.Material = p.Material;
        }

        internal static void OnGeometryChanged(DependencyObject d)
        {
            Primitive3D p = ((Primitive3D)d);

            p._content.Geometry = p.Tessellate();
        }

        internal double DegToRad(double degrees)
        {
            return (degrees / 180.0) * Math.PI;
        }

        internal abstract Geometry3D Tessellate();

        internal readonly GeometryModel3D _content = new GeometryModel3D();
    }
}
