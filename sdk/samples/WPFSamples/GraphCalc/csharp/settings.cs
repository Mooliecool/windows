using System;

namespace GraphCalc
{
    public class Settings
    {
        public const string uMin = "u Min";
        public const string uMax = "u Max";
        public const string uGrid = "u GridSections";
        public const string vMin = "v Min";
        public const string vMax = "v Max";
        public const string vGrid = "v GridSections";
        public const string xMin = "x Min";
        public const string xMax = "x Max";
        public const string yMin = "y Min";
        public const string yMax = "y Max";
        public const string xMin2D = "x Min";
        public const string xMax2D = "x Max";
        public const string yMin2D = "y Min";
        public const string yMax2D = "y Max";
        public const string tMin2D = "t Min";
        public const string tMax2D = "t Max";
        public const string tStep2D = "t Step";

        public const string uMinReg = "uMin";
        public const string uMaxReg = "uMax";
        public const string uGridReg = "uGrid";
        public const string vMinReg = "vMin";
        public const string vMaxReg = "vMax";
        public const string vGridReg = "vGrid";
        public const string xMinReg = "xMin";
        public const string xMaxReg = "xMax";
        public const string yMinReg = "yMin";
        public const string yMaxReg = "yMax";
        public const string xMin2DReg = "xMin2D";
        public const string xMax2DReg = "xMax2D";
        public const string yMin2DReg = "yMin2D";
        public const string yMax2DReg = "yMax2D";
        public const string tMin2DReg = "tMin2D";
        public const string tMax2DReg = "tMax2D";
        public const string tStep2DReg = "tStep2D";

        public const string uMinDefault = "-pi";
        public const string uMaxDefault = "pi";
        public const string uGridDefault = "24";
        public const string vMinDefault = "0";
        public const string vMaxDefault = "pi";
        public const string vGridDefault = "48";
        public const string xMinDefault = "-10";
        public const string xMaxDefault = "10";
        public const string yMinDefault = "-10";
        public const string yMaxDefault = "10";
        public const string xMin2DDefault = "-10";
        public const string xMax2DDefault = "10";
        public const string yMin2DDefault = "-10";
        public const string yMax2DDefault = "10";
        public const string tMin2DDefault = "0";
        public const string tMax2DDefault = "10pi";
        public const string tStep2DDefault = "pi/16";

        public const string fx = "x( u,v )=";
        public const string fy = "y( u,v )=";
        public const string fz = "z( u,v )=";
        public const string y = "y=";
        public const string xt = "x( t )=";
        public const string yt = "y( t )=";

        public const string fxReg = "fx";
        public const string fyReg = "fy";
        public const string fzReg = "fz";
        public const string yReg = "y";
        public const string xtReg = "xt";
        public const string ytReg = "yt";

        public const string fxDefault = "cos(u)sin(v)";
        public const string fyDefault = "-cos(v)";
        public const string fzDefault = "sin(-u)sin(v)";
        public const string yDefault = "x*sin(x^2)+x";
        public const string xtDefault = "sin(t)*t/pi";
        public const string ytDefault = "cos(t)*t/pi";

        public const string function = "2D";
        public const string function2D = "2D Parametric";
        public const string function3D = "3D Parametric";
        public const string functionNone = "None";
    }
}