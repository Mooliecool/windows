using System;
using System.Windows;           // UIElement
using System.Windows.Media;     // Visual

namespace Microsoft.Samples.WPFNotepad
{
    internal class VisualContainer : FrameworkElement
    {
        #region Constructors

        internal VisualContainer()
            : base()
        {
        }

        #endregion Constructors

        #region Internal Properties

        internal Vector Offset
        {
            get
            {
                return VisualOffset;
            }
            set
            {
                VisualOffset = value;
            }
        }

        internal Transform Transform
        {
            get
            {
                return VisualTransform;
            }
            set
            {
                VisualTransform = value;
            }
        }

        internal Visual PageVisual
        {
            get
            {
                return pageVisual;
            }
            set
            {
                this.RemoveVisualChild(pageVisual);
                this.AddVisualChild(value);
                pageVisual = value;
            }
        }

        internal Size PageSize
        {
            get
            {
                return pageSize;
            }

            set
            {
                pageSize = value;
                Width = pageSize.Width;
                Height = pageSize.Height;
            }
        }

        #endregion Internal Properties

        #region VisualChildren

        protected override Visual GetVisualChild(int index)
        {
            if (index != 0 || pageVisual == null)
            {
                throw new ArgumentOutOfRangeException("index", index, "Illegal child index");
            }

            return pageVisual;
        }

        protected override int VisualChildrenCount
        {
            get { return pageVisual != null ? 1 : 0; }
        }

        #endregion VisualChildren

        #region Private Fields

        private Visual pageVisual;
        private Size pageSize;

        #endregion Private Fields

    }
}



