using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace EnvironmentSimulation
{
    class Enviroment
    {
        double[,] RadiationValues;
        bool[,] ValidPosations;
        int Xmax;
        int Ymax;
        Point Source;

        /// <summary>
        /// Initalize The Enviroment , and the 
        /// </summary>
        /// <param name="_Xmax">max x coordinate</param>
        /// <param name="_Ymax">max y coordinate</param>
        /// <param name="Ps">Radation Source Posation Location</param>
        /// 
        public Enviroment(int _Xmax, int _Ymax,Point Ps)
        {
            Xmax = _Xmax;
            Ymax = _Ymax;
            Source = Ps;
            RadiationValues = new double[Xmax, Ymax];
            ValidPosations = new bool[Xmax, Ymax];
            //Calculate the intensity
            calculateIntensity();
        }

        private void calculateIntensity()
        {
            for (int x = 0; x < Xmax; x++)
            {
                for (int y = 0; y < Ymax; y++)
                {
                    RadiationValues[x, y] = Math.Exp(-1 * (Math.Pow((x - Source.X), 2) + Math.Pow((y - Source.Y), 2)));
                }
            }
        }

        /// <summary>
        /// Set the Area as invalid Location,where agent cann't locate at this point
        /// </summary>
        /// <param name="p">Invalid Point</param>
        /// 
        public void setinValidLocation(Point p)
        {
            ValidPosations[p.X,p.Y]=false;
        }

        /// <summary>
        /// Radation Model the intensity of radation R(x,y) = a * exp(-((x-xs)^2+(y-ys)^2));
        /// we see the Noise is an Addative Noise ~N(0,1) -> R(x,y) ~ N(a * exp(-((x-xs)^2+(y-ys)^2)),1)
        /// </summary>
        public void simulateNoiseEffect()
        {
            //generate a random variabe 
            calculateIntensity();
            
            for (int x = 0; x < Xmax; x++)
            {
                for (int y = 0; y < Ymax; y++)
                {
                    RadiationValues[x, y] +=  new GaussianRandom().NextGaussian(RadiationValues[x, y],1);
                }
            }
        }

        /// <summary>
        /// Get Radiation Intensity at p
        /// </summary>
        /// <param name="p"></param>
        /// <returns></returns>
        public double getIntensity(Point p)
        {
            return RadiationValues[p.X, p.Y];
        }
        /// <summary>
        /// Get The state of Posation p valid or not ?
        /// </summary>
        /// <param name="p"></param>
        /// <returns></returns>
        public bool isValid(Point p)
        {
            return ValidPosations[p.X, p.Y];
        }


    }

    public class GaussianRandom : Random
    {
        /// <summary>
        /// Choose the next random number. The distribution of randomness is "normal"
        /// or "Gaussian" with a mean value dMu and a standard deviation of dSigma
        /// </summary>
        /// <param name="dMu">The center point (mean) of the normal distribution</param>
        /// <param name="dSigma">The standard deviation of the distribution</param>
        /// <returns>Random value</returns>
        public double NextGaussian(double dMu, double dSigma)
        {
            return dMu + CumulativeGaussian(base.NextDouble()) * dSigma;
        }

        private static double CumulativeGaussian(double p)
        {
            // p is a rectangular probability between 0 and 1
            // convert that into a gaussian.
            // Apply the inverse cumulative gaussian distribution function
            // This is an approximation by Abramowitz and Stegun; Press, et al.
            // See http://www.pitt.edu/~wpilib/statfaq/gaussfaq.html
            // Because of the symmetry of the normal
            // distribution, we need only consider 0 < p < 0.5. If you have p > 0.5,
            // then apply the algorithm below to q = 1-p, and then negate the value
            // for X obtained.
            bool fNegate = false;

            if (p > 0.5)
            {
                p = 1.0 - p;
                fNegate = true;
            }

            double t = Math.Sqrt(Math.Log(1.0 / (p * p)));
            double tt = t * t;
            double ttt = tt * t;
            double X = t - ((c_0 + c_1 * t + c_2 * tt) / (1 + d_1 * t + d_2 * tt + d_3 * ttt));

            return fNegate ? -X : X;
        }

        private const double c_0 = 2.515517;
        private const double c_1 = 0.802853;
        private const double c_2 = 0.010328;
        private const double d_1 = 1.432788;
        private const double d_2 = 0.189269;
        private const double d_3 = 0.001308;
    }
}
