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
        public void makebgVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            backgroundProp.Visibility = Visibility.Visible;
            tb2.Text = "If you set the Foreground and Background colors to the same color, TextContent will no longer be visible.";
        }
        public void makeboVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            baselineOffsetProp.Visibility = Visibility.Visible;
            tb2.Text = "";
            tb3.Visibility = Visibility.Visible;
            tb3.FontWeight = FontWeights.Bold;
            tb3.Text = "Another TextBlock Element.";
        }
        public void makebaVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            breakafterProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makebbVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            breakbeforeProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }

        public void makeffVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            fontfamilyProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makefsVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            fontsizeProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makefstVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            fontstretchProp.Visibility = Visibility.Visible;
            tb2.Text = "Some fonts do not support all FontStretch values.";
        }
        public void makefstyleVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp1.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            fontstyleProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makefwVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            fontweightProp.Visibility = Visibility.Visible;
            tb2.Text = "Not all FontWeight values result in unique rendering.";
        }
        public void makefgVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            foregroundProp.Visibility = Visibility.Visible;
            tb2.Text = "If you set the Foreground and Background colors to the same color, TextContent will no longer be visible.";
        }

        public void makelhVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            lineheightProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void maketaVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            textalignmentProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void maketdVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            textdecorationsProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void maketcVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            textcontentProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void makettVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            texttrimmingProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }
        public void maketeVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            texteffectsProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }

        public void maketwVisible(object sender, RoutedEventArgs e)
        {
            foreach (UIElement child in sp2.Children)
            {
                child.Visibility = Visibility.Collapsed;
            }
            textwrapProp.Visibility = Visibility.Visible;
            tb2.Text = "";
        }

        // Begin Background change methods
        public void setBackgroundRed(object sender, RoutedEventArgs e)
        {
            tb1.Background = Brushes.Red;
        }
        public void setBackgroundBlue(object sender, RoutedEventArgs e)
        {
            tb1.Background = Brushes.Blue;
        }
        public void setBackgroundGreen(object sender, RoutedEventArgs e)
        {
            tb1.Background = Brushes.Green;
        }
        public void setBackgroundPurple(object sender, RoutedEventArgs e)
        {
            tb1.Background = Brushes.Purple;
        }
        public void setBackgroundWhite(object sender, RoutedEventArgs e)
        {
            tb1.Background = Brushes.White;
        }

        // Begin BaselineOffset change methods
        public void offsetChange10(object sender, RoutedEventArgs e)
        {
            tb3.BaselineOffset = 10;
        }
        public void offsetChange20(object sender, RoutedEventArgs e)
        {
            tb3.BaselineOffset = 20;
        }
        public void offsetChange30(object sender, RoutedEventArgs e)
        {
            tb3.BaselineOffset = 30;
        }
        public void offsetChange40(object sender, RoutedEventArgs e)
        {
            tb3.BaselineOffset = 40;
        }

        // Begin BreakAfter change methods
        public void getbaValue(object sender, RoutedEventArgs e)
        {
            tb2.Text = "BreakAfter value is set to " + tb1.BreakAfter.ToString();
        }

        // Begin BreakBefore change methods
        public void getbbValue(object sender, RoutedEventArgs e)
        {
            tb2.Text = "BreakBefore value is set to " + tb1.BreakBefore.ToString();
        }

        // Begin FontFamily change methods
        public void ffTimes(object sender, RoutedEventArgs e)
        {
            tb1.FontFamily = new FontFamily("Times New Roman");
        }
        public void ffVerdana(object sender, RoutedEventArgs e)
        {
            tb1.FontFamily = new FontFamily("Verdana");
        }
        public void ffPalatino(object sender, RoutedEventArgs e)
        {
            tb1.FontFamily = new FontFamily("Palatino Linotype");
        }
        public void ffCourier(object sender, RoutedEventArgs e)
        {
            tb1.FontFamily = new FontFamily("Courier New");
        }

        // Begin FontSize change methods
        public void fs10(object sender, RoutedEventArgs e)
        {
            tb1.FontSize = 10;
        }
        public void fs15(object sender, RoutedEventArgs e)
        {
            tb1.FontSize = 15;
        }
        public void fs20(object sender, RoutedEventArgs e)
        {
            tb1.FontSize = 20;
        }
        public void fs25(object sender, RoutedEventArgs e)
        {
            tb1.FontSize = 25;
        }

        // Begin FontStretch change methods
        public void fstNormal(object sender, RoutedEventArgs e)
        {
            tb1.FontStretch = FontStretches.Normal;
        }
        public void fstCondensed(object sender, RoutedEventArgs e)
        {
            tb1.FontStretch = FontStretches.Condensed;
        }
        public void fstExpanded(object sender, RoutedEventArgs e)
        {
            tb1.FontStretch = FontStretches.Expanded;
        }
        public void fstMedium(object sender, RoutedEventArgs e)
        {
            tb1.FontStretch = FontStretches.Medium;
        }

        // Begin FontWeight change methods
        public void fwNormal(object sender, RoutedEventArgs e)
        {
            tb1.FontWeight = FontWeights.Normal;
        }
        public void fwLight(object sender, RoutedEventArgs e)
        {
            tb1.FontWeight = FontWeights.Light;
        }
        public void fwMedium(object sender, RoutedEventArgs e)
        {
            tb1.FontWeight = FontWeights.Medium;
        }
        public void fwBold(object sender, RoutedEventArgs e)
        {
            tb1.FontWeight = FontWeights.Bold;
        }
        public void fwUltraBold(object sender, RoutedEventArgs e)
        {
            tb1.FontWeight = FontWeights.UltraBold;
        }

        // Begin FontStyle change methods
        public void fstyleNormal(object sender, RoutedEventArgs e)
        {
            tb1.FontStyle = FontStyles.Normal;
        }
        public void fstyleItalic(object sender, RoutedEventArgs e)
        {
            tb1.FontStyle = FontStyles.Italic;
        }
        public void fstyleOblique(object sender, RoutedEventArgs e)
        {
            tb1.FontStyle = FontStyles.Oblique;
        }

        // Begin Foreground change methods
        public void foregroundWhite(object sender, RoutedEventArgs e)
        {
            tb1.Foreground = Brushes.White;
        }
        public void foregroundBlack(object sender, RoutedEventArgs e)
        {
            tb1.Foreground = Brushes.Black;
        }
        public void foregroundBlue(object sender, RoutedEventArgs e)
        {
            tb1.Foreground = Brushes.Blue ;
        }
        public void foregroundRed(object sender, RoutedEventArgs e)
        {
            tb1.Foreground = Brushes.Red;
        }

        // Begin Lineheight change methods
        public void lineheight10(object sender, RoutedEventArgs e)
        {
            tb1.LineHeight = 10;
        }
        public void lineheight20(object sender, RoutedEventArgs e)
        {
            tb1.LineHeight = 20;
        }
        public void lineheight30(object sender, RoutedEventArgs e)
        {
            tb1.LineHeight = 30;
        }
        public void lineheight40(object sender, RoutedEventArgs e)
        {
            tb1.LineHeight = 40;
        }

        // Begin TextAlignment change methods
        public void talignLeft(object sender, RoutedEventArgs e)
        {
            tb1.TextAlignment = TextAlignment.Left;
        }
        public void talignRight(object sender, RoutedEventArgs e)
        {
            tb1.TextAlignment = TextAlignment.Right;
        }
        public void talignCenter(object sender, RoutedEventArgs e)
        {
            tb1.TextAlignment = TextAlignment.Center;
        }
        public void talignJustify(object sender, RoutedEventArgs e)
        {
            tb1.TextAlignment = TextAlignment.Justify;
        }

        // Begin TextContent change methods

        public void tcUpdate(object sender, RoutedEventArgs e)
        {
            tb1.Text = tbox1.SelectedText;
        }

        // Begin TextWrap change methods
        public void wrapWrap(object sender, RoutedEventArgs e)
        {
            tb1.TextWrapping = TextWrapping.Wrap;
        }
        public void wrapNoWrap(object sender, RoutedEventArgs e)
        {
            tb1.TextWrapping = TextWrapping.NoWrap;
        }
        public void wrapWrapWithOverflow(object sender, RoutedEventArgs e)
        {
            tb1.TextWrapping = TextWrapping.WrapWithOverflow;
        }

        // Begin TextTrimming change methods
        public void trimNone(object sender, RoutedEventArgs e)
        {
            tb1.TextTrimming = TextTrimming.None;
        }
        public void trimWord(object sender, RoutedEventArgs e)
        {
            tb1.TextTrimming = TextTrimming.WordEllipsis;
        }
        public void trimCharacter(object sender, RoutedEventArgs e)
        {
            tb1.TextTrimming = TextTrimming.CharacterEllipsis;
        }

        // Begin TextDecorations change methods
        public void tdBase(object sender, RoutedEventArgs e)
        {
            TextDecorationCollection myCollection = new TextDecorationCollection();
            TextDecoration myBaseline = new TextDecoration();
            myBaseline.Location = TextDecorationLocation.Baseline;
            myBaseline.Pen = new Pen(Brushes.Red, 1);
            myBaseline.PenThicknessUnit = TextDecorationUnit.FontRecommended;
            myCollection.Add(myBaseline);
            tb1.TextDecorations = myCollection;
        }

        public void tdUnder(object sender, RoutedEventArgs e)
        {
            TextDecorationCollection myCollection = new TextDecorationCollection();
            TextDecoration myUnderline = new TextDecoration();
            myUnderline.Location = TextDecorationLocation.Underline;
            myUnderline.Pen = new Pen(Brushes.Red, 1);
            myUnderline.PenThicknessUnit = TextDecorationUnit.FontRecommended;
            myCollection.Add(myUnderline);
            tb1.TextDecorations = myCollection;
        }

        public void tdStrike(object sender, RoutedEventArgs e)
        {
            TextDecorationCollection myCollection = new TextDecorationCollection();
            TextDecoration myStrikethrough = new TextDecoration();
            myStrikethrough.Location = TextDecorationLocation.Strikethrough;
            myStrikethrough.Pen = new Pen(Brushes.Red, 1);
            myStrikethrough.PenThicknessUnit = TextDecorationUnit.FontRecommended;
            myCollection.Add(myStrikethrough);
            tb1.TextDecorations = myCollection;
        }

        public void tdOver(object sender, RoutedEventArgs e)
        {
            TextDecorationCollection myCollection = new TextDecorationCollection();
            TextDecoration myOverline = new TextDecoration();
            myOverline.Location = TextDecorationLocation.OverLine;
            myOverline.Pen = new Pen(Brushes.Red, 1);
            myOverline.PenThicknessUnit = TextDecorationUnit.FontRecommended;
            myCollection.Add(myOverline);
            tb1.TextDecorations = myCollection;
        }

        // Begin TextEffect methods
        public void teTranslate(object sender, RoutedEventArgs e)
        {
            DisableTextEffects();

            TextEffect myEffect = new TextEffect();
            myEffect.PositionStart = 0;
            myEffect.PositionCount = 999;
            TranslateTransform myTranslateTransform = new TranslateTransform(50,50);
            myEffect.Transform = myTranslateTransform;

            EnableTextEffects(tb1, myEffect);
            tb2.Text = "The TranslateTransform applied moved the TextBlock to an offset position of 50,50.";

         }

        public void teScale(object sender, RoutedEventArgs e)
        {
            DisableTextEffects();

            TextEffect myEffect = new TextEffect();
            myEffect.PositionStart = 0;
            myEffect.PositionCount = 999;
            ScaleTransform myScaleTransform = new ScaleTransform(5,5);
            myEffect.Transform = myScaleTransform;

            EnableTextEffects(tb1, myEffect);
            tb2.Text = "The ScaleTransform applied scaled the text by a factor of 5.";
        }

        public void teRotate(object sender, RoutedEventArgs e)
        {
            DisableTextEffects();

            TextEffect myEffect = new TextEffect();
            myEffect.PositionStart = 0;
            myEffect.PositionCount = 999;
            RotateTransform myRotateTransform = new RotateTransform(45);
            myEffect.Transform = myRotateTransform;

            EnableTextEffects(tb1, myEffect);
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

        private void EnableTextEffects(TextBlock tb, TextEffect effect)
        {
            _textEffectTargets = TextEffectResolver.Resolve(tb.ContentStart, tb.ContentEnd, effect);
            foreach (TextEffectTarget target in _textEffectTargets)
                target.Enable();           
        }

        private TextEffectTarget[] _textEffectTargets; 
    }
}