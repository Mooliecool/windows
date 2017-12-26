using System;
using System.Windows;
using System.Collections;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Documents;

namespace SDKSample
{
	public partial class Window1 : Window
	{
        // For quick access to the underlying DynamicPageinator.
        DynamicDocumentPaginator dynPaginator;

        public void windowLoaded(Object sender, RoutedEventArgs e)
        {
            dynPaginator = ((IDocumentPaginatorSource)fd1).DocumentPaginator as DynamicDocumentPaginator;
            if (dynPaginator == null) 
                throw new NullReferenceException("Can't get a DynamicDocumentPaginator for FlowDocument fd1.");
        }

        public void makebgVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            backgroundProp.Visibility = Visibility.Visible;
            tb2.Text = "If you set the Foreground and Background colors to the same color, TextContent will no longer be visible.";
        }

        public void makecgVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            columngapProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }

        public void makecrbVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            columnrulebrushProp.Visibility = Visibility.Visible;
            tb2.Text = "This property will have no effect if ColumnRuleWidth is not set.";
        }

        public void makecrwVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            columnrulewidthProp.Visibility = Visibility.Visible;
            tb2.Text = "This property will have no effect if ColumnRuleBrush is not set.";
        }

        public void makecwVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            columnwidthProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }

        public void makeceVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            contentendProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makecsVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            contentstartProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makefdVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            flowdirectionProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makeffVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            fontfamilyProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makefsVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            fontsizeProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makefstVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            fontstretchProp.Visibility = Visibility.Visible;
            tb2.Text = "Some fonts do not support all FontStretch values.";
        }
        public void makefstyleVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            fontstyleProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makefwVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            fontweightProp.Visibility = Visibility.Visible;
            tb2.Text = "Not all FontWeight values result in unique rendering.";
        }
        public void makebpeVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            isbackgroundpaginationenabledProp.Visibility = Visibility.Visible;
            tb2.Text = "This setting will not effect visual rendering of content.";
        }
        public void makecwfVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            iscolumnwidthflexibleProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makepcfVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            ispagecountfinalProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }

        public void makefgVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            foregroundProp.Visibility = Visibility.Visible;
            tb2.Text = "If you set the Foreground and Background colors to the same color, TextContent will no longer be visible.";
        }

        public void makelhVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            lineheightProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makemaxphVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            maxpageheightProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makemaxpwVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            maxpagewidthProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makeminphVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            minpageheightProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makeminpwVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            minpagewidthProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makepcountVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            pagecountProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makephVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            pageheightProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makeppVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            pagepaddingProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makepsVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            pagesizeProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makepwVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            pagewidthProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void maketaVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            textalignmentProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        /*
        public void makettVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            texttrimmingProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        */
        public void maketeVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            texteffectsProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        /*
        public void maketwVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            textwrapProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        */
        public void maketpVisible(object sender, System.Windows.RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            typographyProp.Visibility = Visibility.Visible;
            fd1.FontFamily = new FontFamily("Palatino Linotype");
            tb2.Text = "FontFamily has been changed to Palatino as it is an OpenType font and supports Typography features.";
        }

        // Begin Background change methods
        public void setBackgroundRed(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.Background = Brushes.Red;
        }
        public void setBackgroundBlue(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.Background = Brushes.Blue;
        }
        public void setBackgroundGreen(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.Background = Brushes.Green;
        }
        public void setBackgroundPurple(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.Background = Brushes.Purple;
        }
        public void setBackgroundWhite(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.Background = Brushes.White;
        }

        // Begin ColumnGap change methods
        public void cg5(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.ColumnGap = 5;
        }
        public void cg10(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.ColumnGap = 10;
        }
        public void cg15(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.ColumnGap = 15;
        }
        public void cg20(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.ColumnGap = 20;
        }

        // Begin ColumnRuleBrush change methods
        public void setcolruleGray(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.ColumnRuleBrush = Brushes.Gray;
        }
        public void setcolruleBlack(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.ColumnRuleBrush = Brushes.Black;
        }
        public void setcolruleRed(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.ColumnRuleBrush = Brushes.Red;
        }
        public void setcolruleBlue(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.ColumnRuleBrush = Brushes.Blue;
        }

        // Begin ColumnRuleWidth change methods
        public void crw1(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.ColumnRuleWidth = 1;
        }
        public void crw3(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.ColumnRuleWidth = 3;
        }
        public void crw5(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.ColumnRuleWidth = 5;
        }
        public void crw10(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.ColumnRuleWidth = 10;
        }

        // Begin ColumnWidth change methods
        public void cw100(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.ColumnWidth = 100;
        }
        public void cw150(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.ColumnWidth = 150;
        }
        public void cw200(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.ColumnWidth = 200;
        }
        public void cw250(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.ColumnWidth = 250;
        }

        // Begin ContentEnd method
        public void getCEValue(object sender, System.Windows.RoutedEventArgs e)
        {
            Paragraph para1 = new Paragraph();
            fd1.Blocks.Add(para1);
            para1.Inlines.Add(new Run(" " + "Text added to the end of the FlowDocument."));
        }

        // Begin ContentStart method
        public void getCSValue(object sender, System.Windows.RoutedEventArgs e)
        {
            Paragraph para2 = new Paragraph();
            fd1.Blocks.Add(para2);
            para2.Inlines.Add(new Run(" " + "Text added to the beginning of the FlowDocument."));
        }

        // Begin FlowDirection change methods
        public void fdirection1(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FlowDirection = FlowDirection.LeftToRight;
        }
        public void fdirection2(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FlowDirection = FlowDirection.RightToLeft;
        }
        
        // Begin FontFamily change methods
        public void ffTimes(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontFamily = new FontFamily("Times New Roman");
        }
        public void ffVerdana(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontFamily = new FontFamily("Verdana");
        }
        public void ffPalatino(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontFamily = new FontFamily("Palatino Linotype");
        }
        public void ffCourier(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontFamily = new FontFamily("Courier New");
        }

        // Begin FontSize change methods
        public void fs10(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontSize = 10;
        }
        public void fs15(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontSize = 15;
        }
        public void fs20(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontSize = 20;
        }
        public void fs25(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontSize = 25;
        }

        // Begin FontStretch change methods
        public void fstNormal(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontStretch = FontStretches.Normal;
        }
        public void fstCondensed(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontStretch = FontStretches.Condensed;
        }
        public void fstExpanded(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontStretch = FontStretches.Expanded;
        }
        public void fstMedium(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontStretch = FontStretches.Medium;
        }

        // Begin FontWeight change methods
        public void fwNormal(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontWeight = FontWeights.Normal;
        }
        public void fwLight(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontWeight = FontWeights.Light;
        }
        public void fwMedium(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontWeight = FontWeights.Medium;
        }
        public void fwBold(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontWeight = FontWeights.Bold;
        }
        public void fwUltraBold(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontWeight = FontWeights.UltraBold;
        }

        // Begin FontStyle change methods
        public void fstyleNormal(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontStyle = FontStyles.Normal;
        }
        public void fstyleItalic(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontStyle = FontStyles.Italic;
        }
        public void fstyleOblique(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.FontStyle = FontStyles.Oblique;
        }

        // Begin Foreground change methods
        public void foregroundWhite(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.Foreground = Brushes.White;
        }
        public void foregroundBlack(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.Foreground = Brushes.Black;
        }
        public void foregroundBlue(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.Foreground = Brushes.Blue ;
        }
        public void foregroundRed(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.Foreground = Brushes.Red;
        }

        // Begin IsBackgroundPaginationEnabled change methods
        public void bpeTrue(object sender, System.Windows.RoutedEventArgs e)
        {
            dynPaginator.IsBackgroundPaginationEnabled = true;
            tb2.Text = "IsBackgroundPaginationEnabled property is set to " + dynPaginator.IsBackgroundPaginationEnabled.ToString();
        }
        public void bpeFalse(object sender, System.Windows.RoutedEventArgs e)
        {
           dynPaginator.IsBackgroundPaginationEnabled = false;
            tb2.Text = "IsBackgroundPaginationEnabled property is set to " + dynPaginator.IsBackgroundPaginationEnabled.ToString();
        }

        // Begin IsColumnWidthFlexible change methods
        public void cwfTrue(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.IsColumnWidthFlexible = true;
            tb2.Text = "IsColumnWidthFlexible property is set to " + fd1.IsColumnWidthFlexible.ToString();
        }
        public void cwfFalse(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.IsColumnWidthFlexible = false;
            tb2.Text = "IsColumnWidthFlexible property is set to " + fd1.IsColumnWidthFlexible.ToString();
        }

        // Begin IsPageCountFinal get method
        public void getpcfValue(object sender, System.Windows.RoutedEventArgs e)
        {
            tb2.Text = "IsPageCountFinal value is " + dynPaginator.IsPageCountValid.ToString();
        }

        // Begin Lineheight change methods
        public void lineheight10(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.LineHeight = 10;
        }
        public void lineheight20(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.LineHeight = 20;
        }
        public void lineheight30(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.LineHeight = 30;
        }
        public void lineheight40(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.LineHeight = 40;
        }

        // Begin MaxPageHeight change methods
        public void maxpageheight500(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.MaxPageHeight = 500;
            tb2.Text = "MaxPageHeight property is set to " + fd1.MaxPageHeight.ToString();
        }
        public void maxpageheight600(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.MaxPageHeight = 600;
            tb2.Text = "MaxPageHeight property is set to " + fd1.MaxPageHeight.ToString();
        }
        public void maxpageheight700(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.MaxPageHeight = 700;
            tb2.Text = "MaxPageHeight property is set to " + fd1.MaxPageHeight.ToString();
        }
        public void maxpageheight800(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.MaxPageHeight = 800;
            tb2.Text = "MaxPageHeight property is set to " + fd1.MaxPageHeight.ToString();
        }

        // Begin MaxPageWidth change methods
        public void maxpagewidth300(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.MaxPageWidth = 300;
            tb2.Text = "MaxPageWidth property is set to " + fd1.MaxPageWidth.ToString();
        }
        public void maxpagewidth400(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.MaxPageWidth = 400;
            tb2.Text = "MaxPageWidth property is set to " + fd1.MaxPageWidth.ToString();
        }
        public void maxpagewidth500(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.MaxPageWidth = 500;
            tb2.Text = "MaxPageWidth property is set to " + fd1.MaxPageWidth.ToString();
        }
        public void maxpagewidth600(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.MaxPageWidth = 600;
            tb2.Text = "MaxPageWidth property is set to " + fd1.MaxPageWidth.ToString();
        }

        // Begin MinPageHeight change methods
        public void minpageheight200(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.MinPageHeight = 200;
            tb2.Text = "MinPageHeight property is set to " + fd1.MinPageHeight.ToString();
        }
        public void minpageheight300(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.MinPageHeight = 300;
            tb2.Text = "MaxPageHeight property is set to " + fd1.MinPageHeight.ToString();
        }
        public void minpageheight400(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.MinPageHeight = 400;
            tb2.Text = "MinPageHeight property is set to " + fd1.MinPageHeight.ToString();
        }
        public void minpageheight500(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.MinPageHeight = 500;
            tb2.Text = "MinPageHeight property is set to " + fd1.MinPageHeight.ToString();
        }

        // Begin MinPageWidth change methods
        public void minpagewidth200(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.MinPageWidth = 200;
            tb2.Text = "MinPageWidth property is set to " + fd1.MinPageWidth.ToString();
        }
        public void minpagewidth300(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.MinPageWidth = 300;
            tb2.Text = "MaxPageWidth property is set to " + fd1.MinPageWidth.ToString();
        }
        public void minpagewidth400(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.MinPageWidth = 400;
            tb2.Text = "MinPageWidth property is set to " + fd1.MinPageWidth.ToString();
        }
        public void minpagewidth500(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.MinPageWidth = 500;
            tb2.Text = "MinPageWidth property is set to " + fd1.MinPageWidth.ToString();
        }

        // Begin PageCount get method
        public void getpcValue(object sender, System.Windows.RoutedEventArgs e)
        {
            tb2.Text = "PageCount value is " + dynPaginator.PageCount;
        }

        // Begin PageHeight change methods
        public void pageheight200(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.PageHeight = 200;
            tb2.Text = "PageHeight property is set to " + fd1.PageHeight.ToString();
        }
        public void pageheight400(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.PageHeight = 400;
            tb2.Text = "PageHeight property is set to " + fd1.PageHeight.ToString();
        }
        public void pageheight600(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.PageHeight = 600;
            tb2.Text = "PageHeight property is set to " + fd1.PageHeight.ToString();
        }
        public void pageheight800(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.PageHeight = 800;
            tb2.Text = "PageHeight property is set to " + fd1.PageHeight.ToString();
        }

        // Begin PagePadding change methods
        public void pagepadding5(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.PagePadding = new Thickness(5);
            tb2.Text = "PagePadding property is set to " + fd1.PagePadding.ToString();
        }
        public void pagepadding10(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.PagePadding = new Thickness(10);
            tb2.Text = "PagePadding property is set to " + fd1.PagePadding.ToString();
        }
        public void pagepadding15(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.PagePadding = new Thickness(15);
            tb2.Text = "PagePadding property is set to " + fd1.PagePadding.ToString();
        }
        public void pagepadding20(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.PagePadding = new Thickness(20);
            tb2.Text = "PagePadding property is set to " + fd1.PagePadding.ToString();
        }

        // Begin PageSize change methods
        public void pagesize200(object sender, System.Windows.RoutedEventArgs e)
        {
            // fd1.PageSize = new Size(200, 200);
            // tb2.Text = "PageSize property is set to " + fd1.PageSize.ToString();
            dynPaginator.PageSize = new Size(200, 200);
            tb2.Text = "PageSize is set to " + dynPaginator.PageSize.ToString();
        }
        public void pagesize400(object sender, System.Windows.RoutedEventArgs e)
        {
            dynPaginator.PageSize = new Size(400, 400);
            tb2.Text = "PageSize property is set to " + dynPaginator.PageSize.ToString();
        }
        public void pagesize600(object sender, System.Windows.RoutedEventArgs e)
        {
            dynPaginator.PageSize = new Size(600, 600);
            tb2.Text = "PageSize property is set to " + dynPaginator.PageSize.ToString();
        }
        public void pagesize800(object sender, System.Windows.RoutedEventArgs e)
        {
            dynPaginator.PageSize = new Size(800, 800);
            tb2.Text = "PageSize property is set to " + dynPaginator.PageSize.ToString();
        }

        // Begin PageWidth change methods
        public void pagewidth200(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.PageWidth = 200;
            tb2.Text = "PageWidth property is set to " + fd1.PageWidth.ToString();
        }
        public void pagewidth400(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.PageWidth = 400;
            tb2.Text = "PageWidth property is set to " + fd1.PageWidth.ToString();
        }
        public void pagewidth600(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.PageWidth = 600;
            tb2.Text = "PageWidth property is set to " + fd1.PageWidth.ToString();
        }

        // Begin TextAlignment change methods
        public void talignLeft(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.TextAlignment = TextAlignment.Left;
        }
        public void talignRight(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.TextAlignment = TextAlignment.Right;
        }
        public void talignCenter(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.TextAlignment = TextAlignment.Center;
        }
        public void talignJustify(object sender, System.Windows.RoutedEventArgs e)
        {
            fd1.TextAlignment = TextAlignment.Justify;
        }
              

        // Begin Typography change methods
        public void typo1(object sender, RoutedEventArgs e)
        {
            Typography.SetHistoricalForms(fd1, false);
            Typography.SetVariants(fd1, FontVariants.Normal);
            Typography.SetCapitals(fd1, FontCapitals.SmallCaps);
        }
        public void typo2(object sender, RoutedEventArgs e)
        {
            Typography.SetHistoricalForms(fd1, false);
            Typography.SetCapitals(fd1, FontCapitals.Normal);
            Typography.SetVariants(fd1, FontVariants.Subscript);
        }
        public void typo3(object sender, RoutedEventArgs e)
        {
            Typography.SetVariants(fd1, FontVariants.Normal);
            Typography.SetCapitals(fd1, FontCapitals.Normal);
            Typography.SetHistoricalForms(fd1, true);               
        }


        // Begin TextEffect methods
        public void teTranslate(object sender, System.Windows.RoutedEventArgs e)
        {
            DisableTextEffects();

            TextEffect myEffect = new TextEffect();
            myEffect.PositionStart = 0;
            myEffect.PositionCount = 999;
            TranslateTransform myTranslateTransform = new TranslateTransform(50, 50);
            myEffect.Transform = myTranslateTransform;

            EnableTextEffects(fd1, myEffect);
            tb2.Text = "The TranslateTransform applied moved the TextBlock to an offset position of 50,50.";
         }

        public void teScale(object sender, System.Windows.RoutedEventArgs e)
        {
            DisableTextEffects();

            TextEffect myEffect = new TextEffect();
            myEffect.PositionStart = 0;
            myEffect.PositionCount = 999;
            ScaleTransform myScaleTransform = new ScaleTransform(5, 5);
            myEffect.Transform = myScaleTransform;

            EnableTextEffects(fd1, myEffect);
            tb2.Text = "The ScaleTransform applied scaled the text by a factor of 5.";
        }

        public void teRotate(object sender, System.Windows.RoutedEventArgs e)
        {
            DisableTextEffects();

            TextEffect myEffect = new TextEffect();
            myEffect.PositionStart = 0;
            myEffect.PositionCount = 999;
            RotateTransform myRotateTransform = new RotateTransform(45);
            myEffect.Transform = myRotateTransform;

            EnableTextEffects(fd1, myEffect);
            tb2.Text = "The RotateTransform applied rotated the text by 45 degrees.";
        }

        private void DisableTextEffects()
        {
            if (_textEffectTargets != null)
            {
                foreach (TextEffectTarget target in _textEffectTargets)
                    target.Disable();
            }
        }

        private void EnableTextEffects(FlowDocument fd, TextEffect effect)
        {
            _textEffectTargets = TextEffectResolver.Resolve(fd.ContentStart, fd.ContentEnd, effect);
            foreach (TextEffectTarget target in _textEffectTargets)
                target.Enable();
        }

        private TextEffectTarget[] _textEffectTargets; 

    }
}