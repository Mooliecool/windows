//------------------------------------------------------------------------------
// <copyright file="ToolboxBitmapAttribute.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

/*
 */
namespace System.Drawing {
    using System.ComponentModel;
    using System.Diagnostics;
    using System.Diagnostics.CodeAnalysis;    
    using System;
    using System.IO;    
    using Microsoft.Win32;
    using System.Runtime.InteropServices;
    using System.Globalization;
    using System.Runtime.Versioning;

    /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute"]/*' />
    /// <devdoc>
    ///     ToolboxBitmapAttribute defines the images associated with
    ///     a specified component. The component can offer a small
    ///     and large image (large is optional).
    ///
    /// </devdoc>
    [AttributeUsage(AttributeTargets.Class)]
    public class ToolboxBitmapAttribute : Attribute {

        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.smallImage"]/*' />
        /// <devdoc>
        ///     The small image for this component
        /// </devdoc>
        private Image smallImage;

        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.largeImage"]/*' />
        /// <devdoc>
        ///     The large image for this component.
        /// </devdoc>
        private Image largeImage;

        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.largeDim"]/*' />
        /// <devdoc>
        ///     The default size of the large image.
        /// </devdoc>
        private static readonly Point largeDim = new Point(32, 32);

        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.largeDim"]/*' />
        /// <devdoc>
        ///     The default size of the large image.
        /// </devdoc>
        private static readonly Point smallDim = new Point(16, 16);

        // Used to help cache the last result of BitmapSelector.GetFileName
        private static string lastOriginalFileName;
        private static string lastUpdatedFileName;

        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.ToolboxBitmapAttribute"]/*' />
        /// <devdoc>
        ///     Constructs a new ToolboxBitmapAttribute.
        /// </devdoc>
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public ToolboxBitmapAttribute(string imageFile)
            : this(GetImageFromFile(imageFile, false), GetImageFromFile(imageFile, true)) {
        }

        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.ToolboxBitmapAttribute1"]/*' />
        /// <devdoc>
        ///     Constructs a new ToolboxBitmapAttribute.
        /// </devdoc>
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public ToolboxBitmapAttribute(Type t)
            : this(GetImageFromResource(t, null, false), GetImageFromResource(t, null, true)) {
        }

        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.ToolboxBitmapAttribute2"]/*' />
        /// <devdoc>
        ///     Constructs a new ToolboxBitmapAttribute.
        /// </devdoc>
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public ToolboxBitmapAttribute(Type t, string name)
            : this(GetImageFromResource(t, name, false), GetImageFromResource(t, name, true)) {
        }


        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.ToolboxBitmapAttribute3"]/*' />
        /// <devdoc>
        ///     Constructs a new ToolboxBitmapAttribute.
        /// </devdoc>
        private ToolboxBitmapAttribute(Image smallImage, Image largeImage) {
            this.smallImage = smallImage;
            this.largeImage = largeImage;
        }

        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.Equals"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public override bool Equals(object value) {
            if (value == this) {
                return true;
            }

            ToolboxBitmapAttribute attr = value as ToolboxBitmapAttribute;
            if (attr != null) {
                return attr.smallImage == smallImage && attr.largeImage == largeImage;
            }

            return false;
        }

        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.GetHashCode"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public override int GetHashCode() {
            return base.GetHashCode();
        }
        
        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.GetImage"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public Image GetImage(object component) {
            return GetImage(component, true);
        }
        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.GetImage1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public Image GetImage(object component, bool large) {
            if (component != null) {
                return GetImage(component.GetType(), large);
            }
            return null;
        }
        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.GetImage2"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public Image GetImage(Type type) {
            return GetImage(type, false);
        }

        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.GetImage3"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public Image GetImage(Type type, bool large) {
            return GetImage(type, null, large);
        }

        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.GetImage4"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public Image GetImage(Type type, string imgName, bool large) {

            if ((large && largeImage == null) ||
                (!large && smallImage == null)) {

                Point largeDim = new Point(32, 32);
                Image img = null;
                if (large) {
                    img = largeImage;
                }
                else {
                    img = smallImage;
                }

                if (img == null) {
                    img = GetImageFromResource(type, imgName, large);
                }

                //last resort for large images.
                if (large && largeImage == null && smallImage != null) {
                    img = new Bitmap((Bitmap)smallImage, largeDim.X, largeDim.Y);
                }

                Bitmap b = img as Bitmap;

                if (b != null) {
                    MakeBackgroundAlphaZero(b);
                }

                if (img == null) {
                    img = DefaultComponent.GetImage(type, large);
                }

                if (large) {
                    largeImage = img;
                }
                else {
                    smallImage = img;
                }
            }

            Image toReturn = (large) ? largeImage : smallImage;

            if (this.Equals(Default)) {
                largeImage = null;
                smallImage = null;
            }

            return toReturn;

        }

        //helper to get the right icon from the given stream that represents an icon
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        private static Image GetIconFromStream(Stream stream, bool large) {
            if (stream == null) {
                return null;
            }
            Icon ico = new Icon(stream);
            Icon sizedico = new Icon(ico, large ? new Size(largeDim.X, largeDim.Y):new Size(smallDim.X, smallDim.Y));
            return sizedico.ToBitmap();
        }

        // Cache the last result of BitmapSelector.GetFileName because we commonly load images twice
        // in succession from the same file and we don't need to compute the name twice.
        private static string GetFileNameFromBitmapSelector(string originalName)
        {
            if (originalName != lastOriginalFileName) {
                lastOriginalFileName = originalName;
                lastUpdatedFileName = BitmapSelector.GetFileName(originalName);
            }

            return lastUpdatedFileName;
        }

        // Just forwards to Image.FromFile eating any non-critical exceptions that may result.
        [SuppressMessage("Microsoft.Design", "CA1031:DoNotCatchGeneralExceptionTypes")]
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        private static Image GetImageFromFile(string imageFile, bool large) {
            Image image = null;
            try {
                if (imageFile != null) {
                    imageFile = GetFileNameFromBitmapSelector(imageFile);

                    string ext = Path.GetExtension(imageFile);
                    if (ext != null && string.Equals(ext, ".ico", StringComparison.OrdinalIgnoreCase))
                    {
                        //ico files support both large and small, so we respect the large flag here.

                        FileStream reader = System.IO.File.Open(imageFile, FileMode.Open);
                        if (reader != null) {
                            try {
                                image = GetIconFromStream(reader, large);
                            }
                            finally {
                                reader.Close();
                            }
                        }
                    }
                    else if (!large) {
                        //we only read small from non-ico files.
                        image = Image.FromFile(imageFile);
                    }
                }
            } catch (Exception e) {            
                if (ClientUtils.IsCriticalException(e)) {
                    throw;
                }
                Debug.Fail("Failed to load toolbox image '" + imageFile + "':\r\n" + e.ToString());
            }

            return image;
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        static private Image GetBitmapFromResource(Type t, string bitmapname, bool large) {
            if (bitmapname == null) {
                return null;
            }

            Image img = null;

            // load the image from the manifest resources. 
            //
            Stream stream = BitmapSelector.GetResourceStream(t, bitmapname);
            if (stream != null) {
                Bitmap b = new Bitmap(stream);
                img = b;
                MakeBackgroundAlphaZero(b);
                if (large) {
                    img = new Bitmap(b, largeDim.X, largeDim.Y);
                }
            }
            return img;
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        static private Image GetIconFromResource(Type t, string bitmapname, bool large) {
            if (bitmapname == null) {
                return null;
            }

            return GetIconFromStream(BitmapSelector.GetResourceStream(t, bitmapname), large);
        }

        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.GetImageFromResource"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [SuppressMessage("Microsoft.Design", "CA1031:DoNotCatchGeneralExceptionTypes")]
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public static Image GetImageFromResource(Type t, string imageName, bool large) {
            Image img = null;
            try {

                string name = imageName;
                string iconname = null;
                string bmpname = null;
                string rawbmpname = null;

                // if we didn't get a name, use the class name
                //
                if (name == null) {
                    name = t.FullName;
                    int indexDot = name.LastIndexOf('.');
                    if (indexDot != -1) {
                        name = name.Substring(indexDot + 1);
                    }
                    iconname = name + ".ico";
                    bmpname = name + ".bmp";
                }
                else {
                    if (String.Compare(Path.GetExtension(imageName), ".ico", true, CultureInfo.CurrentCulture) == 0) {
                        iconname = name;
                    }
                    else if (String.Compare(Path.GetExtension(imageName), ".bmp", true, CultureInfo.CurrentCulture) == 0) {
                        bmpname = name;
                    }
                    else {
                        //we dont recognize the name as either bmp or ico. we need to try three things.
                        //1.  the name as a bitmap (back compat)
                        //2.  name+.bmp
                        //3.  name+.ico
                        rawbmpname = name;
                        bmpname = name + ".bmp";
                        iconname = name + ".ico";
                    }

                }
                if (rawbmpname != null) {
                    img = GetBitmapFromResource(t, rawbmpname, large);
                }
                if (img == null && bmpname != null) {
                    img = GetBitmapFromResource(t, bmpname, large);
                }
                if (img == null && iconname != null) {
                    img = GetIconFromResource(t, iconname, large);
                }
            }
            catch (Exception e) {
                if (t == null) {
                    Debug.Fail("Failed to load toolbox image for null type:\r\n" + e.ToString());
                } 
                else {
                    Debug.Fail("Failed to load toolbox image for '" + t.FullName + "':\r\n" + e.ToString());
                }
            }
            return img;
        }

        private static void MakeBackgroundAlphaZero(Bitmap img) {
            Color bottomLeft = img.GetPixel(0, img.Height - 1);
            img.MakeTransparent();

            Color newBottomLeft = Color.FromArgb(0, bottomLeft);
            img.SetPixel(0, img.Height - 1, newBottomLeft);
        }

        /// <include file='doc\ToolboxBitmapAttribute.uex' path='docs/doc[@for="ToolboxBitmapAttribute.Default"]/*' />
        /// <devdoc>
        ///     Default name is null
        /// </devdoc>        
        [SuppressMessage("Microsoft.Security", "CA2104:DoNotDeclareReadOnlyMutableReferenceTypes")]
        public static readonly ToolboxBitmapAttribute Default = new ToolboxBitmapAttribute((Image)null, (Image)null);

        private static readonly ToolboxBitmapAttribute DefaultComponent;
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        static ToolboxBitmapAttribute() {

            //Fix for Dev10 560430. When we call Gdip.DummyFunction, JIT will make sure Gdip..cctor will be called before
            SafeNativeMethods.Gdip.DummyFunction();

            Bitmap bitmap = null;
            Stream stream = BitmapSelector.GetResourceStream(typeof(ToolboxBitmapAttribute), "DefaultComponent.bmp");
            if (stream != null) {
                bitmap = new Bitmap(stream);
                MakeBackgroundAlphaZero(bitmap);
            }
            DefaultComponent = new ToolboxBitmapAttribute(bitmap, null);
        }
    }
}
