using System;
using System.ComponentModel;
using Microsoft.Win32;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Xps;

namespace Microsoft.Samples.WPFNotepad
{

    public class HeaderFooterDocumentPaginator : DocumentPaginator
    {
        #region Private Fields

        private DocumentPaginator flowDocPaginator;
        private string headerText;
        private string footerText;
        private double printableHeight;
        private double printableWidth;
        private Size originalPageSize;

        #endregion Private Fields

        #region Constructors

        public HeaderFooterDocumentPaginator(DocumentPaginator flowDocPaginator, string header, string footer, double height, double width)
        {
            this.flowDocPaginator = flowDocPaginator;
            this.headerText = header;
            this.footerText = footer;
            this.printableHeight = height;
            this.printableWidth = width;
            this.originalPageSize = new Size(width, height); // Size.Empty;

            // Register for events
            flowDocPaginator.GetPageCompleted += new GetPageCompletedEventHandler(HandleGetPageCompleted);
            flowDocPaginator.ComputePageCountCompleted += new AsyncCompletedEventHandler(HandleComputePageCountCompleted);
            flowDocPaginator.PagesChanged += new PagesChangedEventHandler(HandlePagesChanged);
        }

        #endregion Constructors

        #region Public Methods

        public override DocumentPage GetPage(int pageNumber)
        {

            DocumentPage documentPage = ConstructPageWithHeaderAndFooter(pageNumber);
            return documentPage;

        }

        public override void GetPageAsync(int pageNumber, object userState)
        {
            flowDocPaginator.GetPageAsync(pageNumber, userState);
        }

        public override void ComputePageCount()
        {
            flowDocPaginator.ComputePageCount();
        }

        public override void ComputePageCountAsync(object userState)
        {
            flowDocPaginator.ComputePageCountAsync(userState);
        }

        public override void CancelAsync(object userState)
        {
            flowDocPaginator.CancelAsync(userState);
        }

        public override string ToString()
        {
            return ((FlowDocument)flowDocPaginator.Source).Name;
        }

        #endregion Public Methods

        #region Public Properties

        public override bool IsPageCountValid
        {
            get
            {
                return flowDocPaginator.IsPageCountValid;
            }
        }

        public override int PageCount
        {
            get
            {
                return flowDocPaginator.PageCount;
            }
        }

        public override Size PageSize
        {
            get
            {
                return flowDocPaginator.PageSize;
            }

            set
            {
                flowDocPaginator.PageSize = value;
            }
        }

        public override IDocumentPaginatorSource Source
        {
            get
            {
                return flowDocPaginator.Source;
            }
        }

        #endregion Public Properties

        #region Private Methods

        private DocumentPage ConstructPageWithHeaderAndFooter(int pageNumber)
        {
            DocumentPage page0 = flowDocPaginator.GetPage(pageNumber);

            // Coming from WPFNotepad, the source document should always be a FlowDocument
            FlowDocument originalDocument = (FlowDocument)flowDocPaginator.Source;
            TextBlock headerBlock = null;
            TextBlock footerBlock = null;
            DocumentPage newPage = null;
            if (originalPageSize == Size.Empty)
            {
                originalPageSize = ((IDocumentPaginatorSource)originalDocument).DocumentPaginator.PageSize;
            }

            Size newPageSize = originalPageSize;

            // Decrease the top and/or bottom margins to account for headers/footers
            if ((headerText != null) && (headerText.Length > 0))
            {
                string expandedHeaderText = GetExpandedText(headerText, originalDocument, pageNumber + 1);
                headerBlock = new TextBlock();
                headerBlock.Text = expandedHeaderText;
                headerBlock.FontFamily = SystemFonts.MenuFontFamily;
                headerBlock.FontSize = 10;
                headerBlock.HorizontalAlignment = HorizontalAlignment.Center;

                headerBlock.Measure(new Size(Double.PositiveInfinity, Double.PositiveInfinity));
                headerBlock.Arrange(new Rect(headerBlock.DesiredSize));
                headerBlock.UpdateLayout();
                if (headerBlock.DesiredSize.Width > 0 && headerBlock.DesiredSize.Height > 0)
                {
                    newPageSize.Height -= headerBlock.DesiredSize.Height;
                }
            }

            if ((footerText != null) && (footerText.Length > 0))
            {
                string expandedFooterText = GetExpandedText(footerText, originalDocument, pageNumber + 1);
                footerBlock = new TextBlock();
                footerBlock.Text = expandedFooterText;
                footerBlock.FontFamily = SystemFonts.MenuFontFamily;
                footerBlock.FontSize = 10;
                footerBlock.HorizontalAlignment = HorizontalAlignment.Center;

                footerBlock.Measure(new Size(Double.PositiveInfinity, Double.PositiveInfinity));
                footerBlock.Arrange(new Rect(footerBlock.DesiredSize));
                footerBlock.UpdateLayout();
                if (footerBlock.DesiredSize.Width > 0 && footerBlock.DesiredSize.Height > 0)
                {
                    newPageSize.Height -= footerBlock.DesiredSize.Height;
                }

            }

            // Get the original page with its reduced size
            flowDocPaginator.PageSize = newPageSize;
            DocumentPage page = flowDocPaginator.GetPage(pageNumber);
            if (page != DocumentPage.Missing)
            {
                // Create a Grid that will hold the header, the original page, and the footer
                Grid grid = new Grid();
                RowDefinition rowDef = new RowDefinition();
                rowDef.Height = new GridLength(0, GridUnitType.Auto);
                grid.RowDefinitions.Add(rowDef);

                rowDef = new RowDefinition();
                rowDef.Height = new GridLength(0, GridUnitType.Star);
                grid.RowDefinitions.Add(rowDef);

                rowDef = new RowDefinition();
                rowDef.Height = new GridLength(0, GridUnitType.Auto);
                grid.RowDefinitions.Add(rowDef);

                ColumnDefinition columnDef = new ColumnDefinition();
                grid.ColumnDefinitions.Add(columnDef);

                // The header and footer TextBlocks can be added to the grid
                // directly.  The Visual from the original DocumentPage needs
                // to be hosted in a container that derives from UIElement.
                if (headerBlock != null)
                {
                    headerBlock.Margin = new Thickness(0, originalDocument.PagePadding.Top, 0, 0);
                    Grid.SetRow(headerBlock, 0);
                    grid.Children.Add(headerBlock);
                }

                VisualContainer container = new VisualContainer();
                container.PageVisual = page.Visual;
                container.PageSize = newPageSize;

                Grid.SetRow(container, 1);
                grid.Children.Add(container);
            
                if (footerBlock != null)
                {
                    footerBlock.Margin = new Thickness(0, 0, 0, originalDocument.PagePadding.Bottom);
                    Grid.SetRow(footerBlock, 2);
                    grid.Children.Add(footerBlock);
                }


           
                // Recalculate the children inside the Grid
                grid.Measure(new Size(Double.PositiveInfinity, Double.PositiveInfinity));
                grid.Arrange(new Rect(grid.DesiredSize));
                grid.UpdateLayout();


                // Return the new DocumentPage constructed from the Grid's Visual
                newPage = new DocumentPage(grid);

            }

            return newPage;
        }

        private string GetExpandedText(string text, FlowDocument doc, int pageNumber)
        {
            // Replace instances of &f with the document's file name
            string expandedText = text.Replace("&f", GetDocumentFileName(doc));
            expandedText = expandedText.Replace("&F", GetDocumentFileName(doc));

            // Replace instances of &t with the current short time
            expandedText = expandedText.Replace("&t", DateTime.Now.ToShortTimeString());
            expandedText = expandedText.Replace("&T", DateTime.Now.ToShortTimeString());

            // Replace instances of &d with the current short date
            expandedText = expandedText.Replace("&d", DateTime.Now.ToShortDateString());
            expandedText = expandedText.Replace("&D", DateTime.Now.ToShortDateString());

            // Replace instances of &p with the current page number
            expandedText = expandedText.Replace("&p", pageNumber.ToString());
            expandedText = expandedText.Replace("&P", pageNumber.ToString());

            return expandedText;            
        }

        private string GetDocumentFileName(FlowDocument doc)
        {
            // Return the display name of the FlowDocument with the
            // " - WPFNotepad" stripped off.
            return doc.Name.Replace(" - WPFNotepad", "");
        }

        /// <summary>
        ///    We are being notified by the wrapped paginator.  If getting the page
        ///    was successful, we use the resulting page to produce a new page that
        ///    includes annotatons.  In either case, we fire an event from this instance.
        /// </summary>
        /// <param name="sender">source of the event</param>
        /// <param name="e">the args for this event</param>
        private void HandleGetPageCompleted(object sender, GetPageCompletedEventArgs e)
        {
            // If no errors, not cancelled, and page isn't missing, create a new page
            // with annotations and create a new event args for that page.
            if (!e.Cancelled && e.Error == null && e.DocumentPage != DocumentPage.Missing)
            {
                // Since we can't change the page the args is holding we create a new
                // args object with the page we produce.
                DocumentPage documentPage = ConstructPageWithHeaderAndFooter(e.PageNumber);

                e = new GetPageCompletedEventArgs(documentPage, e.PageNumber, e.Error, e.Cancelled, e.UserState);
            }

            // Fire the event
            OnGetPageCompleted(e);
        }

        /// <summary>
        ///     We are notified by the wrapped paginator.  In response we fire
        ///     an event from this instance.
        /// </summary>
        /// <param name="sender">source of the event</param>
        /// <param name="e">args for the event</param>
        private void HandleComputePageCountCompleted(object sender, AsyncCompletedEventArgs e)
        {
            // Fire the event
            OnComputePageCountCompleted(e);
        }

        /// <summary>
        ///     We are notified by the wrapped paginator.  In response we fire
        ///     an event from this instance.
        /// </summary>
        /// <param name="sender">source of the event</param>
        /// <param name="e">args for the event</param>
        private void HandlePagesChanged(object sender, PagesChangedEventArgs e)
        {
            // Fire the event
            OnPagesChanged(e);
        }

        #endregion Private Methods

    }


}
