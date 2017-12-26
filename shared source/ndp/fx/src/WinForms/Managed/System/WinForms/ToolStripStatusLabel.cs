//------------------------------------------------------------------------------
// <copyright file="ToolStripStatusLabel.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------


namespace System.Windows.Forms {
    using System;
    using System.ComponentModel;
    using System.Drawing;
    using System.Drawing.Design;
    using System.Diagnostics;
    using System.Windows.Forms.ButtonInternal;
    using System.Security.Permissions;
    using System.Security;
    using System.Windows.Forms.Layout; 
    using System.Windows.Forms.Design; 
   

    /// <include file='doc\ToolStripStatusLabel.uex' path='docs/doc[@for="ToolStripStatusLabel"]/*' />
    /// <devdoc>
    /// A non selectable winbar item
    /// </devdoc>
    [ToolStripItemDesignerAvailability(ToolStripItemDesignerAvailability.StatusStrip)]
    public class ToolStripStatusLabel : ToolStripLabel {


        private Border3DStyle borderStyle = Border3DStyle.Flat;
        private ToolStripStatusLabelBorderSides borderSides = ToolStripStatusLabelBorderSides.None;
        private bool spring = false;
    
        
        /// <include file='doc\ToolStripStatusLabel.uex' path='docs/doc[@for="ToolStripStatusLabel.ToolStripStatusLabel"]/*' />
        /// <devdoc>
        /// A non selectable winbar item
        /// </devdoc>
        public ToolStripStatusLabel() {
        }
        public ToolStripStatusLabel(string text):base(text,null,false,null) {
        }
        public ToolStripStatusLabel(Image image):base(null,image,false,null) {
        }
        public ToolStripStatusLabel(string text, Image image):base(text,image,false,null) {
        }
        public ToolStripStatusLabel(string text, Image image, EventHandler onClick):base(text,image,/*isLink=*/false,onClick,null) {
        }
        public ToolStripStatusLabel(string text, Image image, EventHandler onClick, string  name) :base(text,image,/*isLink=*/false,onClick, name) {
        }

        /// <devdoc>
        /// Creates an instance of the object that defines how image and text
        /// gets laid out in the ToolStripItem
        /// </devdoc>
        internal override ToolStripItemInternalLayout CreateInternalLayout() {
            return new ToolStripStatusLabelLayout(this);
        }

        [Browsable(false)]
        [EditorBrowsable(EditorBrowsableState.Advanced)]
        public new ToolStripItemAlignment Alignment {
           get {
               return base.Alignment;
           }
           set {
               base.Alignment = value;
           }
       }


        /// <include file='doc\ToolStripStatusLabel.uex' path='docs/doc[@for="ToolStripStatusLabel.BorderStyle"]/*' />
        [
        DefaultValue(Border3DStyle.Flat),
        SRDescription(SR.ToolStripStatusLabelBorderStyleDescr),
        SRCategory(SR.CatAppearance)
        ]
        public Border3DStyle BorderStyle {
            get {
                return borderStyle;
            }
            set {
                if (!ClientUtils.IsEnumValid_NotSequential(value, 
                                             (int)value,
                                             (int)Border3DStyle.Adjust,
                                             (int)Border3DStyle.Bump,
                                             (int)Border3DStyle.Etched,
                                             (int)Border3DStyle.Flat,
                                             (int)Border3DStyle.Raised,
                                             (int)Border3DStyle.RaisedInner,
                                             (int)Border3DStyle.RaisedOuter,
                                             (int)Border3DStyle.Sunken,
                                             (int)Border3DStyle.SunkenInner,
                                             (int)Border3DStyle.SunkenOuter
                                                )) {
                    throw new InvalidEnumArgumentException("value", (int)value, typeof(Border3DStyle));
                }
               
                if (borderStyle != value) {
                    borderStyle = value;
                    Invalidate();
                }
            }
        }

        /// <include file='doc\ToolStripStatusLabel.uex' path='docs/doc[@for="ToolStripStatusLabel.BorderSides"]/*' />
        [
        DefaultValue(ToolStripStatusLabelBorderSides.None),
        SRDescription(SR.ToolStripStatusLabelBorderSidesDescr),
        SRCategory(SR.CatAppearance)
        ]
        public ToolStripStatusLabelBorderSides BorderSides {
            get {
                return borderSides;
            }
            set {
                // no Enum.IsDefined as this is a flags enum.
                if (borderSides != value) {
                    borderSides = value;
                    LayoutTransaction.DoLayout(Owner,this, PropertyNames.BorderStyle);
                    Invalidate();
                }
            }
        }
    
        protected internal override Padding DefaultMargin {
            get {
                return new Padding(0, 3, 0, 2);
            }
        }

        [
        DefaultValue(false),
        SRDescription(SR.ToolStripStatusLabelSpringDescr),
        SRCategory(SR.CatAppearance)
        ]
        public bool Spring {
            get { return spring; }
            set {
                if (spring != value) {
                    spring = value;
                    if (ParentInternal != null) {
                       LayoutTransaction.DoLayout(ParentInternal, this, PropertyNames.Spring);
                    }
                   
                }
            }
        }

        public override System.Drawing.Size GetPreferredSize(System.Drawing.Size constrainingSize) {
             if (BorderSides != ToolStripStatusLabelBorderSides.None) {
                return base.GetPreferredSize(constrainingSize) + new Size(4, 4);
             }
             else {
                 return base.GetPreferredSize(constrainingSize);
             }
        }


        
        /// <include file='doc\ToolStripStatusLabel.uex' path='docs/doc[@for="ToolStripStatusLabel.OnPaint"]/*' />
        /// <devdoc>
        /// Inheriting classes should override this method to handle this event.
        /// </devdoc>
        protected override void OnPaint(System.Windows.Forms.PaintEventArgs e) {

            if (this.Owner != null) {
                ToolStripRenderer renderer = this.Renderer;
                  
                renderer.DrawToolStripStatusLabelBackground(new ToolStripItemRenderEventArgs(e.Graphics, this));

                if ((DisplayStyle & ToolStripItemDisplayStyle.Image) == ToolStripItemDisplayStyle.Image) { 
                    renderer.DrawItemImage(new ToolStripItemImageRenderEventArgs(e.Graphics, this, InternalLayout.ImageRectangle));
                }

                PaintText(e.Graphics);
              }
        }

        /// <devdoc>
        ///  This class performs internal layout for the "split button button" portion of a split button.
        ///  Its main job is to make sure the inner button has the same parent as the split button, so
        ///  that layout can be performed using the correct graphics context.
        /// </devdoc>
        private class ToolStripStatusLabelLayout : ToolStripItemInternalLayout {

             ToolStripStatusLabel owner;

            public ToolStripStatusLabelLayout(ToolStripStatusLabel owner) : base(owner) {
                this.owner = owner;
            }

            protected override ToolStripItemLayoutOptions CommonLayoutOptions() {
               ToolStripItemLayoutOptions layoutOptions = base.CommonLayoutOptions();
               layoutOptions.borderSize = 0;
               return layoutOptions;
            }
        }
        
    }

}
    



