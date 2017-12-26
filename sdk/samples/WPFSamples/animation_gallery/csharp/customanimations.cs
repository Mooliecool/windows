using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Data;

namespace Microsoft.Samples.Animation.AnimationGallery.CustomAnimations
{


    /// <summary>
    /// BounceDoubleAnimation
    /// </summary>
    public class BounceDoubleAnimation : DoubleAnimationBase
    {
        public enum EdgeBehaviorEnum
        {
            EaseIn, EaseOut, EaseInOut
        }

        public static readonly DependencyProperty EdgeBehaviorProperty =
            DependencyProperty.Register("EdgeBehavior",
                typeof(EdgeBehaviorEnum),
                typeof(BounceDoubleAnimation),
                new PropertyMetadata(EdgeBehaviorEnum.EaseInOut));

        public static readonly DependencyProperty BouncesProperty =
            DependencyProperty.Register("Bounces",
                typeof(int),
                typeof(BounceDoubleAnimation),
                new PropertyMetadata(5));

        public static readonly DependencyProperty BouncinessProperty =
            DependencyProperty.Register("Bounciness",
                typeof(double),
                typeof(BounceDoubleAnimation),
                new PropertyMetadata(3.0));

        public static readonly DependencyProperty FromProperty =
            DependencyProperty.Register("From",
                typeof(double?),
                typeof(BounceDoubleAnimation),
                new PropertyMetadata(null));

        public static readonly DependencyProperty ToProperty =
            DependencyProperty.Register("To",
                typeof(double?),
                typeof(BounceDoubleAnimation),
                new PropertyMetadata(null));




        /// <summary>
        /// Specifies which side of the transition gets the "bounce" effect.
        /// </summary>
        public EdgeBehaviorEnum EdgeBehavior
        {
            get
            {
                return (EdgeBehaviorEnum)GetValue(EdgeBehaviorProperty);
            }
            set
            {
                SetValue(EdgeBehaviorProperty, value);
            }
        }
        /// <summary>
        ///  Number of bounces in the effect
        /// </summary>
        public int Bounces
        {
            get
            {
                return (int)GetValue(BouncesProperty);
            }
            set
            {
                if (value > 0)
                {
                    SetValue(BouncesProperty, value);
                }
                else
                {
                    throw new ArgumentException("can't set the bounces to " + value);
                }
            }
        }

        /// <summary>
        /// Specifies the amount by which the element springs back.
        /// </summary>
        public double Bounciness
        {
            get
            {
                return (double)GetValue(BouncinessProperty);
            }
            set
            {
                if (value > 0)
                {
                    SetValue(BouncinessProperty, value);
                }
                else
                {
                    throw new ArgumentException("can't set the bounciness to " + value);
                }
            }
        }

        /// <summary>
        /// Specifies the starting value of the animation.
        /// </summary>
        public double? From
        {
            get
            {
                return (double?)GetValue(FromProperty);
            }
            set
            {

                SetValue(FromProperty, value);

            }
        }

        /// <summary>
        /// Specifies the ending value of the animation.
        /// </summary>
        public double? To
        {
            get
            {
                return (double?)GetValue(ToProperty);
            }
            set
            {

                SetValue(ToProperty, value);

            }
        }





        protected override double GetCurrentValueCore(
            double defaultOriginValue,
            double defaultDestinationValue,
            AnimationClock clock)
        {
            double returnValue;
            double start = From != null ? (double)From : defaultOriginValue;
            double delta = To != null ? (double)To - start : defaultOriginValue - start;

            switch (this.EdgeBehavior)
            {
                case EdgeBehaviorEnum.EaseIn:
                    returnValue = easeIn(clock.CurrentProgress.Value, start, delta, Bounciness, Bounces);
                    break;
                case EdgeBehaviorEnum.EaseOut:
                    returnValue = easeOut(clock.CurrentProgress.Value, start, delta, Bounciness, Bounces);
                    break;
                case EdgeBehaviorEnum.EaseInOut:
                default:
                    returnValue = easeInOut(clock.CurrentProgress.Value, start, delta, Bounciness, Bounces);
                    break;
            }
            return returnValue;
        }


        protected override Freezable CreateInstanceCore()
        {

            return new BounceDoubleAnimation();
        }


        private static double easeOut(double timeFraction, double start, double delta, double bounciness, int bounces)
        {
            double returnValue = 0.0;

            // math magic: The cosine gives us the right wave, the timeFraction is the frequency of the wave, 
            // the absolute value keeps every value positive (so it "bounces" off the midpoint of the cosine 
            // wave, and the amplitude (the exponent) makes the sine wave get smaller and smaller at the end.
            returnValue = Math.Abs(Math.Pow((1 - timeFraction), bounciness)
                          * Math.Cos(2 * Math.PI * timeFraction * bounces));
            returnValue = delta - (returnValue * delta);
            returnValue += start;
            return returnValue;

        }
        private static double easeIn(double timeFraction, double start, double delta, double bounciness, int bounces)
        {
            double returnValue = 0.0;
            // math magic: The cosine gives us the right wave, the timeFraction is the amplitude of the wave, 
            // the absolute value keeps every value positive (so it "bounces" off the midpoint of the cosine 
            // wave, and the amplitude (the exponent) makes the sine wave get bigger and bigger towards the end.
            returnValue = Math.Abs(Math.Pow((timeFraction), bounciness)
                          * Math.Cos(2 * Math.PI * timeFraction * bounces));
            returnValue = returnValue * delta;
            returnValue += start;
            return returnValue;
        }
        private static double easeInOut(double timeFraction, double start, double delta, double bounciness, int bounces)
        {
            double returnValue = 0.0;

            // we cut each effect in half by multiplying the time fraction by two and halving the distance.
            if (timeFraction <= 0.5)
            {
                returnValue = easeIn(timeFraction * 2, start, delta / 2, bounciness, bounces);
            }
            else
            {
                returnValue = easeOut((timeFraction - 0.5) * 2, start, delta / 2, bounciness, bounces);
                returnValue += delta / 2;
            }
            return returnValue;
        }
    }



    /// <summary>
    /// ElasticDoubleAnimation - like something attached to a rubber band
    /// </summary>
    public class ElasticDoubleAnimation : DoubleAnimationBase
    {
        public enum EdgeBehaviorEnum
        {
            EaseIn, EaseOut, EaseInOut
        }

        public static readonly DependencyProperty EdgeBehaviorProperty =
            DependencyProperty.Register("EdgeBehavior", typeof(EdgeBehaviorEnum), typeof(ElasticDoubleAnimation), new PropertyMetadata(EdgeBehaviorEnum.EaseIn));

        public static readonly DependencyProperty SpringinessProperty =
            DependencyProperty.Register("Springiness", typeof(double), typeof(ElasticDoubleAnimation), new PropertyMetadata(3.0));

        public static readonly DependencyProperty OscillationsProperty =
            DependencyProperty.Register("Oscillations", typeof(double), typeof(ElasticDoubleAnimation), new PropertyMetadata(10.0));

        public static readonly DependencyProperty FromProperty =
            DependencyProperty.Register("From",
                typeof(double?),
                typeof(ElasticDoubleAnimation),
                new PropertyMetadata(null));

        public static readonly DependencyProperty ToProperty =
            DependencyProperty.Register("To",
                typeof(double?),
                typeof(ElasticDoubleAnimation),
                new PropertyMetadata(null));

        /// <summary>
        /// which side gets the effect
        /// </summary>
        public EdgeBehaviorEnum EdgeBehavior
        {
            get
            {
                return (EdgeBehaviorEnum)GetValue(EdgeBehaviorProperty);
            }
            set
            {
                SetValue(EdgeBehaviorProperty, value);
            }
        }

        /// <summary>
        /// how much springiness is there in the effect
        /// </summary>
        public double Springiness
        {
            get
            {
                return (double)GetValue(SpringinessProperty);
            }
            set
            {
                SetValue(SpringinessProperty, value);
            }
        }

        /// <summary>
        /// number of oscillations in the effect
        /// </summary>
        public double Oscillations
        {
            get
            {
                return (double)GetValue(OscillationsProperty);
            }
            set
            {
                SetValue(OscillationsProperty, value);
            }
        }


        /// <summary>
        /// Specifies the starting value of the animation.
        /// </summary>
        public double? From
        {
            get
            {
                return (double?)GetValue(FromProperty);
            }
            set
            {

                SetValue(FromProperty, value);

            }
        }

        /// <summary>
        /// Specifies the ending value of the animation.
        /// </summary>
        public double? To
        {
            get
            {
                return (double?)GetValue(ToProperty);
            }
            set
            {

                SetValue(ToProperty, value);

            }
        }


        protected override double GetCurrentValueCore(double defaultOriginValue, double defaultDestinationValue, AnimationClock clock)
        {
            double returnValue;
            double start = From != null ? (double)From : defaultOriginValue;
            double delta = To != null ? (double)To - start : defaultOriginValue - start;
            switch (this.EdgeBehavior)
            {
                case EdgeBehaviorEnum.EaseIn:
                    returnValue = easeIn(clock.CurrentProgress.Value, start, delta, Springiness, Oscillations);
                    break;
                case EdgeBehaviorEnum.EaseOut:
                    returnValue = easeOut(clock.CurrentProgress.Value, start, delta, Springiness, Oscillations);
                    break;
                case EdgeBehaviorEnum.EaseInOut:
                default:
                    returnValue = easeInOut(clock.CurrentProgress.Value, start, delta, Springiness, Oscillations);
                    break;
            }
            return returnValue;
        }

        protected override Freezable CreateInstanceCore()
        {
            return new ElasticDoubleAnimation();
        }


        private static double easeOut(double timeFraction, double start, double delta, double springiness, double oscillations)
        {
            double returnValue = 0.0;

            // math magic: The cosine gives us the right wave, the timeFraction * the # of oscillations is the 
            // frequency of the wave, and the amplitude (the exponent) makes the wave get smaller at the end
            // by the "springiness" factor. This is extremely similar to the bounce equation.
            returnValue = Math.Pow((1 - timeFraction), springiness)
                          * Math.Cos(2 * Math.PI * timeFraction * oscillations);
            returnValue = delta - (returnValue * delta);
            returnValue += start;
            return returnValue;
        }
        private static double easeIn(double timeFraction, double start, double delta, double springiness, double oscillations)
        {
            double returnValue = 0.0;
            // math magic: The cosine gives us the right wave, the timeFraction * the # of oscillations is the 
            // frequency of the wave, and the amplitude (the exponent) makes the wave get smaller at the beginning
            // by the "springiness" factor. This is extremely similar to the bounce equation. 
            returnValue = Math.Pow((timeFraction), springiness)
                          * Math.Cos(2 * Math.PI * timeFraction * oscillations);
            returnValue = returnValue * delta;
            returnValue += start;
            return returnValue;
        }
        private static double easeInOut(double timeFraction, double start, double delta, double springiness, double oscillations)
        {
            double returnValue = 0.0;

            // we cut each effect in half by multiplying the time fraction by two and halving the distance.
            if (timeFraction <= 0.5)
            {
                return easeIn(timeFraction * 2, start, delta / 2, springiness, oscillations);
            }
            else
            {
                returnValue = easeOut((timeFraction - 0.5) * 2, start, delta / 2, springiness, oscillations);
                returnValue += (delta / 2);
            }
            return returnValue;
        }
    }





    /// <summary>
    /// BackDoubleAnimation: goes in the opposite direction first
    /// </summary>
    public class BackDoubleAnimation : DoubleAnimationBase
    {
        public enum EdgeBehaviorEnum
        {
            EaseIn, EaseOut, EaseInOut
        }

        public static readonly DependencyProperty EdgeBehaviorProperty =
            DependencyProperty.Register("EdgeBehavior", typeof(EdgeBehaviorEnum), typeof(BackDoubleAnimation), new PropertyMetadata(EdgeBehaviorEnum.EaseIn));

        public static readonly DependencyProperty AmplitudeProperty =
            DependencyProperty.Register("Amplitude", typeof(double), typeof(BackDoubleAnimation), new PropertyMetadata(4.0));

        public static readonly DependencyProperty SuppressionProperty =
            DependencyProperty.Register("Suppression", typeof(double), typeof(BackDoubleAnimation), new PropertyMetadata(2.0));

        public static readonly DependencyProperty FromProperty =
            DependencyProperty.Register("From",
                typeof(double?),
                typeof(BackDoubleAnimation),
                new PropertyMetadata(null));

        public static readonly DependencyProperty ToProperty =
            DependencyProperty.Register("To",
                typeof(double?),
                typeof(BackDoubleAnimation),
                new PropertyMetadata(null));



        /// <summary>
        /// which side gets the effect
        /// </summary>
        public EdgeBehaviorEnum EdgeBehavior
        {
            get
            {
                return (EdgeBehaviorEnum)GetValue(EdgeBehaviorProperty);
            }
            set
            {
                SetValue(EdgeBehaviorProperty, value);
            }
        }

        /// <summary>
        /// how much backwards motion is there in the effect
        /// </summary>
        public double Amplitude
        {
            get
            {
                return (double)GetValue(AmplitudeProperty);
            }
            set
            {
                SetValue(AmplitudeProperty, value);
            }
        }

        /// <summary>
        /// how quickly the effect drops off vs. the entire timeline
        /// </summary>
        public double Suppression
        {
            get
            {
                return (double)GetValue(SuppressionProperty);
            }
            set
            {
                SetValue(SuppressionProperty, value);
            }
        }

        /// <summary>
        /// Specifies the starting value of the animation.
        /// </summary>
        public double? From
        {
            get
            {
                return (double?)GetValue(FromProperty);
            }
            set
            {

                SetValue(FromProperty, value);

            }
        }

        /// <summary>
        /// Specifies the ending value of the animation.
        /// </summary>
        public double? To
        {
            get
            {
                return (double?)GetValue(ToProperty);
            }
            set
            {

                SetValue(ToProperty, value);

            }
        }


        protected override double GetCurrentValueCore(double defaultOriginValue, double defaultDestinationValue, AnimationClock clock)
        {
            double returnValue;
            double start = From != null ? (double)From : defaultOriginValue;
            double delta = To != null ? (double)To - start : defaultOriginValue - start;
            switch (this.EdgeBehavior)
            {
                case EdgeBehaviorEnum.EaseIn:
                    returnValue = easeIn(clock.CurrentProgress.Value, start, delta, Amplitude, Suppression);
                    break;
                case EdgeBehaviorEnum.EaseOut:
                    returnValue = easeOut(clock.CurrentProgress.Value, start, delta, Amplitude, Suppression);
                    break;
                case EdgeBehaviorEnum.EaseInOut:
                default:
                    returnValue = easeInOut(clock.CurrentProgress.Value, start, delta, Amplitude, Suppression);
                    break;
            }
            return returnValue;
        }

        protected override Freezable CreateInstanceCore()
        {

            return new BackDoubleAnimation();
        }


        private static double easeOut(double timeFraction, double start, double delta, double amplitude, double suppression)
        {
            double returnValue = 0.0;
            double frequency = 0.5;

            // math magic: The sine gives us the right wave, the timeFraction * 0.5 (frequency) gives us only half 
            // of the full wave, the amplitude gives us the relative height of the peak, and the exponent makes the 
            // effect drop off more quickly by the "suppression" factor. 
            returnValue = Math.Pow((timeFraction), suppression)
                          * amplitude * Math.Sin(2 * Math.PI * timeFraction * frequency) + timeFraction;
            returnValue = (returnValue * delta);
            returnValue += start;
            return returnValue;
        }

        private static double easeIn(double timeFraction, double start, double delta, double amplitude, double suppression)
        {
            double returnValue = 0.0;
            double frequency = 0.5;

            // math magic: The sine gives us the right wave, the timeFraction * 0.5 (frequency) gives us only half 
            // of the full wave (flipped by multiplying by -1 so that we go "backwards" first), the amplitude gives 
            // us the relative height of the peak, and the exponent makes the effect start later by the "suppression" 
            // factor. 
            returnValue = Math.Pow((1 - timeFraction), suppression)
                          * amplitude * Math.Sin(2 * Math.PI * timeFraction * frequency) * -1 + timeFraction;
            returnValue = (returnValue * delta);
            returnValue += start;
            return returnValue;
        }

        private static double easeInOut(double timeFraction, double start, double delta, double amplitude, double suppression)
        {
            double returnValue = 0.0;

            // we cut each effect in half by multiplying the time fraction by two and halving the distance.
            if (timeFraction <= 0.5)
            {
                return easeIn(timeFraction * 2, start, delta / 2, amplitude, suppression);
            }
            else
            {
                returnValue = easeOut((timeFraction - 0.5) * 2, start, delta / 2, amplitude, suppression);
                returnValue += (delta / 2);
            }
            return returnValue;
        }
    }







    /// <summary>
    /// ExponentialDoubleAnimation - gets exponentially faster / slower
    /// </summary>
    public class ExponentialDoubleAnimation : DoubleAnimationBase
    {
        public enum EdgeBehaviorEnum
        {
            EaseIn, EaseOut, EaseInOut
        }

        public static readonly DependencyProperty EdgeBehaviorProperty =
            DependencyProperty.Register("EdgeBehavior", typeof(EdgeBehaviorEnum), typeof(ExponentialDoubleAnimation), new PropertyMetadata(EdgeBehaviorEnum.EaseIn));

        public static readonly DependencyProperty PowerProperty =
            DependencyProperty.Register("Power", typeof(double), typeof(ExponentialDoubleAnimation), new PropertyMetadata(2.0));


        public static readonly DependencyProperty FromProperty =
            DependencyProperty.Register("From",
                typeof(double?),
                typeof(ExponentialDoubleAnimation),
                new PropertyMetadata(null));

        public static readonly DependencyProperty ToProperty =
            DependencyProperty.Register("To",
                typeof(double?),
                typeof(ExponentialDoubleAnimation),
                new PropertyMetadata(null));


        /// <summary>
        /// which side gets the effect
        /// </summary>
        public EdgeBehaviorEnum EdgeBehavior
        {
            get
            {
                return (EdgeBehaviorEnum)GetValue(EdgeBehaviorProperty);
            }
            set
            {
                SetValue(EdgeBehaviorProperty, value);
            }
        }

        /// <summary>
        /// exponential rate of growth
        /// </summary>
        public double Power
        {
            get
            {
                return (double)GetValue(PowerProperty);
            }
            set
            {
                if (value > 0.0)
                {
                    SetValue(PowerProperty, value);
                }
                else
                {
                    throw new ArgumentException("cannot set power to less than 0.0. Value: " + value);
                }
            }
        }

        /// <summary>
        /// Specifies the starting value of the animation.
        /// </summary>
        public double? From
        {
            get
            {
                return (double?)GetValue(FromProperty);
            }
            set
            {

                SetValue(FromProperty, value);

            }
        }

        /// <summary>
        /// Specifies the ending value of the animation.
        /// </summary>
        public double? To
        {
            get
            {
                return (double?)GetValue(ToProperty);
            }
            set
            {

                SetValue(ToProperty, value);

            }
        }



        protected override double GetCurrentValueCore(double defaultOriginValue, double defaultDestinationValue, AnimationClock clock)
        {
            double returnValue;
            double start = From != null ? (double)From : defaultOriginValue;
            double delta = To != null ? (double)To - start : defaultOriginValue - start;

            switch (this.EdgeBehavior)
            {
                case EdgeBehaviorEnum.EaseIn:
                    returnValue = easeIn(clock.CurrentProgress.Value, start, delta, Power);
                    break;
                case EdgeBehaviorEnum.EaseOut:
                    returnValue = easeOut(clock.CurrentProgress.Value, start, delta, Power);
                    break;
                case EdgeBehaviorEnum.EaseInOut:
                default:
                    returnValue = easeInOut(clock.CurrentProgress.Value, start, delta, Power);
                    break;
            }
            return returnValue;
        }

        protected override Freezable CreateInstanceCore()
        {
            return new ExponentialDoubleAnimation();
        }

        private static double easeIn(double timeFraction, double start, double delta, double power)
        {
            double returnValue = 0.0;

            // math magic: simple exponential growth
            returnValue = Math.Pow(timeFraction, power);
            returnValue *= delta;
            returnValue = returnValue + start;
            return returnValue;
        }
        private static double easeOut(double timeFraction, double start, double delta, double power)
        {
            double returnValue = 0.0;

            // math magic: simple exponential decay
            returnValue = Math.Pow(timeFraction, 1 / power);
            returnValue *= delta;
            returnValue = returnValue + start;
            return returnValue;
        }
        private static double easeInOut(double timeFraction, double start, double delta, double power)
        {
            double returnValue = 0.0;

            // we cut each effect in half by multiplying the time fraction by two and halving the distance.
            if (timeFraction <= 0.5)
            {
                returnValue = easeOut(timeFraction * 2, start, delta / 2, power);
            }
            else
            {
                returnValue = easeIn((timeFraction - 0.5) * 2, start, delta / 2, power);
                returnValue += (delta / 2);
            }
            return returnValue;
        }
    }



    /// <summary>
    /// CircleAnimation: calculates polar coordinates as a function of time. 
    /// Use two of these (XDirection and YDirection) to move an element in an elliptical manner
    /// </summary>
    public class CircleAnimation : DoubleAnimationBase
    {
        public enum DirectionEnum
        {
            XDirection, YDirection
        }

        public static readonly DependencyProperty DirectionProperty =
            DependencyProperty.Register("Direction", typeof(DirectionEnum), typeof(CircleAnimation), new PropertyMetadata(DirectionEnum.XDirection));

        public static readonly DependencyProperty RadiusProperty =
            DependencyProperty.Register("Radius", typeof(double), typeof(CircleAnimation), new PropertyMetadata((double)10));


        /// <summary>
        /// distance from origin to polar coordinate
        /// </summary>
        public double Radius
        {
            get
            {
                return (double)GetValue(RadiusProperty);
            }
            set
            {
                if (value > 0.0)
                {
                    SetValue(RadiusProperty, value);
                }
                else
                {
                    throw new ArgumentException("a radius of " + value + " is not allowed!");
                }
            }
        }

        /// <summary>
        /// are we measuring in the X or Y direction?
        /// </summary>
        public DirectionEnum Direction
        {
            get
            {
                return (DirectionEnum)GetValue(DirectionProperty);
            }
            set
            {
                SetValue(DirectionProperty, value);
            }
        }




        protected override double GetCurrentValueCore(double defaultOriginValue, double defaultDestinationValue, AnimationClock clock)
        {
            double returnValue;
            double time = clock.CurrentProgress.Value;

            // math magic: calculate new coordinates using polar coordinate equations. This requires two 
            // animations to be wired up in order to move in a circle, since we don't make any assumptions
            // about what we're animating (e.g. a TranslateTransform). 
            if (Direction == DirectionEnum.XDirection)
            {
                returnValue = Math.Cos(2 * Math.PI * time);
            }
            else
            {
                returnValue = Math.Sin(2 * Math.PI * time);
            }

            // Need to add the defaultOriginValue so that composition works.
            return returnValue * Radius + defaultOriginValue;
        }

        protected override Freezable CreateInstanceCore()
        {
            return new CircleAnimation();
        }

    }
}