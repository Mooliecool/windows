using System;

using Microsoft.SDK.Samples.VistaBridge.Interop;
using System.Windows.Markup;
using System.Windows.Media.Imaging;
using BitmapSourceDictionary = System.Collections.Generic.Dictionary<int, System.Windows.Media.Imaging.BitmapSource>;
namespace Microsoft.SDK.Samples.VistaBridge.Library.StockIcons
{
    /// <summary>
    /// Represents a standard system icon.
    /// </summary>
    /// <remarks>This class supports Xaml markup.</remarks>
    public class StockIcon : MarkupExtension
    {
        StockIconIdentifier _identifier;
        StockIconOptions _options;
        BitmapSource _bitmapSource;
        static BitmapSourceDictionary _cache = new BitmapSourceDictionary();
        /// <summary>
        /// Creates a new StockIcon instance.
        /// </summary>
        public StockIcon()
        {
        }
        /// <summary>
        /// Creates a new StockIcon instance with the specified identifer.
        /// </summary>
        /// <param name="identifier">
        /// A value that identifies the icon represented by this instance.
        /// </param>
        public StockIcon(StockIconIdentifier identifier)
            : this(identifier, 0)
        {
        }
        /// <summary>
        /// Creates a new StockIcon instance with the 
        /// specified 
        /// <see cref="Microsoft.SDK.Samples.VistaBridge.Library.StockIcons.StockIconIdentifier"/> and <see cref="Microsoft.SDK.Samples.VistaBridge.Library.StockIcons.StockIconOptions"/> values.
        /// </summary>
        /// <param name="identifier">
        /// A value that identifies the icon represented by this instance.
        /// </param>
        /// <param name="options">
        /// A set of values that controls the visual 
        /// representation of the icon.
        /// </param>
        public StockIcon(StockIconIdentifier identifier, StockIconOptions options)
        {
            Identifier = identifier;
            Selected = (options & StockIconOptions.Selected) == StockIconOptions.Selected;
            LinkOverlay = (options & StockIconOptions.LinkOverlay) == StockIconOptions.LinkOverlay;
            ShellSize = (options & StockIconOptions.ShellSize) == StockIconOptions.ShellSize;
            Small = (options & StockIconOptions.Small) == StockIconOptions.Small;
        }
        /// <summary>
        /// Checks whether the icon has already been retrieved.
        /// </summary>
        /// <exception cref="System.InvalidOperationException"> You cannot modify the properties of a StockIcon object after its bitmap has been created.</exception>
        /// <remarks> This method
        /// is used by property setters to ensure that property
        /// values are not modified after the icon has been retrieved.</remarks>
        protected void Check()
        {
            if (_bitmapSource != null)
                throw new InvalidOperationException(
                    "The BitmapSource has already been created.");
        }
        /// <summary>
        /// Gets or sets a value indicating whether the icon
        /// appears selected.
        /// </summary>
        /// <exception cref="System.InvalidOperationException">
        /// You cannot modify the properties of a <b>StockIcon</b>object 
        /// after its bitmap has been created.</exception>
        /// <value>A <see cref="System.Boolean"/> value.</value>
        public bool Selected
        {
            get { return (_options & StockIconOptions.Selected) == StockIconOptions.Selected; }
            set
            {
                Check();
                if (value)
                    _options |= StockIconOptions.Selected;
                else
                    _options &= ~StockIconOptions.Selected;
            }
        }
        /// <summary>
        /// Gets or sets a value that cotrols whether to put 
        /// a link overlay on the icon.
        /// </summary>
        /// <exception cref="System.InvalidOperationException">
        /// You cannot modify the properties of a <b>StockIcon</b> object 
        /// after its bitmap has been created.</exception>
        /// <value>A <see cref="System.Boolean"/> value.</value>
        public bool LinkOverlay
        {
            get { return (_options & StockIconOptions.LinkOverlay) == StockIconOptions.LinkOverlay; }
            set
            {
                Check();
                if (value)
                    _options |= StockIconOptions.LinkOverlay;
                else
                    _options &= ~StockIconOptions.LinkOverlay;
            }
        }
        /// <summary>
        /// Gets or sets a value that controls whether 
        /// the icon is shell sized.
        /// </summary>
        /// <exception cref="System.InvalidOperationException">
        /// You cannot modify the properties of a <b>StockIcon</b> object 
        /// after its bitmap has been created.</exception>
        /// <value>A <see cref="System.Boolean"/> value.</value>
        public bool ShellSize
        {
            get { return (_options & StockIconOptions.ShellSize) == StockIconOptions.ShellSize; }
            set
            {
                Check();
                if (value)
                    _options |= StockIconOptions.ShellSize;
                else
                    _options &= ~StockIconOptions.ShellSize;
            }
        }
        /// <summary>
        /// Gets or sets a value that controls whether the 
        /// small version of the icon is used.
        /// </summary>
        /// <exception cref="System.InvalidOperationException">
        /// You cannot modify the properties of a <b>StockIcon</b> object 
        /// after its bitmap has been created.</exception>
        /// <value>A <see cref="System.Boolean"/> value.</value>
        public bool Small
        {
            get { return (_options & StockIconOptions.Small) == StockIconOptions.Small; }
            set
            {
                Check();
                if (value)
                    _options |= StockIconOptions.Small;
                else
                    _options &= ~StockIconOptions.Small;
            }
        }
        /// <summary>
        /// Gets or sets a value that indicates which 
        /// standard icon is represented by this instance.
        /// </summary>
        /// <exception cref="System.InvalidOperationException">
        /// You cannot modify the properties of a <b>StockIcon</b> object 
        /// after its bitmap has been created.</exception>
        /// <value>A <see cref="Microsoft.SDK.Samples.VistaBridge.Library.StockIcons.StockIconIdentifier"/> value.</value>
        public StockIconIdentifier Identifier
        {
            get { return _identifier; }
            set
            {
                Check();
                _identifier = value;
            }
        }
        /// <summary>
        /// Returns the bitmap for the icon represented by this instance.
        /// </summary>
        /// <returns>An object.</returns>
        public override Object ProvideValue(IServiceProvider serviceProvider)
        {
            return Bitmap;
        }

        /// <summary>
        /// Gets the bitmap for the icon represented by this instance.
        /// </summary>
        /// <value>A <see cref="System.Windows.Media.Imaging.BitmapSource"/> object.</value>
        public BitmapSource Bitmap
        {
            get
            {
                if (_bitmapSource == null)
                {
                    _bitmapSource = GetBitmapSource(_identifier, _options);
                }
                return _bitmapSource;
            }
        }
        /// <summary>
        /// Gets the hash code for this instance.
        /// </summary>
        /// <returns>An <see cref="System.Int32"/> value.</returns>
        public override int GetHashCode()
        {
            return ComputeUniqueValue(_identifier, _options);
        }

        static int ComputeUniqueValue(StockIconIdentifier identifier, StockIconOptions flags)
        {
            int value = checked(((int)identifier) << 4);
            if ((flags & StockIconOptions.Selected) == StockIconOptions.Selected)
                value += 8;
            if ((flags & StockIconOptions.Small) == StockIconOptions.Small)
                value += 4;
            if ((flags & StockIconOptions.LinkOverlay) == StockIconOptions.LinkOverlay)
                value += 2;
            if ((flags & StockIconOptions.ShellSize) == StockIconOptions.ShellSize)
                value += 1;
            return value;
        }
        /// <summary>
        /// Creates a <see cref="System.Windows.Media.Imaging.BitmapSource"/> object for the specified icon.
        /// </summary>
        /// <param name="identifier">
        /// The identifier for the icon represented by this instance.
        /// </param>
        /// <param name="options">
        /// Settings that control the visual representation of the icon.
        /// </param>
        /// <returns>A <see cref="System.Windows.Media.Imaging.BitmapSource"/> object that represents the specified icon.</returns>
        protected internal static BitmapSource GetBitmapSource(StockIconIdentifier identifier, StockIconOptions options)
        {
            BitmapSource bitmapSource;
            int uniqueValue = ComputeUniqueValue(identifier, options);
            if (!_cache.TryGetValue(uniqueValue, out bitmapSource))
            {
                bitmapSource = (BitmapSource)StockIconHelper.MakeImage(identifier, StockIconOptions.Handle | options);
                bitmapSource.Freeze();
                _cache[uniqueValue] = bitmapSource;
            }
            else
            {
                System.Diagnostics.Debug.WriteLine("Found the BitmapSource for " + identifier.ToString() + " in the cache!");
            }
            return bitmapSource;
        }
    }
}
