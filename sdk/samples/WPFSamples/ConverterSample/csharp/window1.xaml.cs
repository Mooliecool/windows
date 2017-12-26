//This is a list of commonly used namespaces for a window.
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Media3D;

namespace ConverterSample
{
	/// <summary>
	/// Interaction logic for Window1.xaml
	/// </summary>

	public partial class Window1 : Window
	{

        public Window1()
        {
            InitializeComponent();

        }

	    // This method performs the Point operations
		public void PerformOperation(object sender, RoutedEventArgs e)
		{
			
			RadioButton li = (sender as RadioButton);
			
            // Strings used to display the results
            String syntaxString, resultType, operationString;
					
			// The local variables point1, point2, vector2, etc are defined in each
			// case block for readability reasons. Each variable is contained within
			// the scope of each case statement.  
      switch (li.Name)
			{   //begin switch

                case "rb1":
                    {
                        // Converts a String to a Point using a PointConverter
                        // Returns a Point.

                        PointConverter pConverter = new PointConverter();
                        Point pointResult = new Point();
                        string string1 = "10,20";

                        pointResult = (Point)pConverter.ConvertFromString(string1);
                        // pointResult is equal to (10, 20)

                        // Displaying Results
                        syntaxString = "pointResult = (Point)pConverter1.ConvertFromString(string1);";
                        resultType = "Point";
                        operationString = "Converting a String to a Point";
                        ShowResults(pointResult.ToString(), syntaxString, resultType, operationString);
                        break;
                    }

                case "rb2":
					{
                        // Converts a String to a Vector using a VectorConverter
                        // Returns a Vector.

                        VectorConverter vConverter = new VectorConverter();
                        Vector vectorResult = new Vector();
                        string string1 = "10,20";

                        vectorResult = (Vector)vConverter.ConvertFromString(string1);
                        // vectorResult is equal to (10, 20)

                        // Displaying Results
                        syntaxString = "vectorResult = (Vector)vConverter.ConvertFromString(string1);";
                        resultType = "Vector";
						operationString = "Converting a String into a Vector";
						ShowResults(vectorResult.ToString(), syntaxString, resultType, operationString);
						break;
					}

				case "rb3":
					{
                        // Converts a String to a Matrix using a MatrixConverter
                        // Returns a Matrix.

                        MatrixConverter mConverter = new MatrixConverter();
                        Matrix matrixResult = new Matrix();
                        string string2 = "10,20,30,40,50,60";

                        matrixResult = (Matrix)mConverter.ConvertFromString(string2);
                        // matrixResult is equal to (10, 20, 30, 40, 50, 60)

                        // Displaying Results
                        syntaxString = "matrixResult = (Vector)mConverter.ConvertFromString(string2);";
                        resultType = "Matrix";
						operationString = "Converting a String into a Matrix";
                        ShowResults(matrixResult.ToString(), syntaxString, resultType, operationString);
                        break;
					}

				case "rb4":
					{
                        // Converts a String to a Point3D using a Point3DConverter
                        // Returns a Point3D.

                        Point3DConverter p3DConverter = new Point3DConverter();
                        Point3D point3DResult = new Point3D();
                        string string3 = "10,20,30";

                        point3DResult = (Point3D)p3DConverter.ConvertFromString(string3);
                        // point3DResult is equal to (10, 20, 30)

                        // Displaying Results
                        syntaxString = "point3DResult = (Point3D)p3DConverter.ConvertFromString(string3);";
                        resultType = "Point3D";
                        operationString = "Converting a String into a Point3D";
                        ShowResults(point3DResult.ToString(), syntaxString, resultType, operationString);
                        break;
					}

				case "rb5":
					{
                        // Converts a String to a Vector3D using a Vector3DConverter
                        // Returns a Vector3D.

                        Vector3DConverter v3DConverter = new Vector3DConverter();
                        Vector3D vector3DResult = new Vector3D();
                        string string3 = "10,20,30";    

                        vector3DResult = (Vector3D)v3DConverter.ConvertFromString(string3);
                        // vector3DResult is equal to (10, 20, 30)

                        // Displaying Results
                        syntaxString = "vector3DResult = (Vector3D)v3DConverter.ConvertFromString(string3);";
                        resultType = "Vector3D";
                        operationString = "Converting a String into a Vector3D";
                        ShowResults(vector3DResult.ToString(), syntaxString, resultType, operationString);
                        break;
					}

                case "rb6":
					{
                        // Converts a String to a Size3D using a Size3DConverter
                        // Returns a Size3D.

                        Size3DConverter s3DConverter = new Size3DConverter();
                        Size3D size3DResult = new Size3D();
                        string string3 = "10,20,30";

                        size3DResult = (Size3D)s3DConverter.ConvertFromString(string3);
                        // size3DResult is equal to (10, 20, 30)

                        // Displaying Results
                        syntaxString = "size3DResult = (Size3D)v3DConverter.ConvertFromString(string3);";
                        resultType = "Size3D";
                        operationString = "Converting a String into a Size3D";
                        ShowResults(size3DResult.ToString(), syntaxString, resultType, operationString);
                        break;
					}

				case "rb7":
					{
                        // Converts a String to a Point4D using a Point4DConverter
                        // Returns a Point4D.

                        Point4DConverter p4DConverter = new Point4DConverter();
                        Point4D point4DResult = new Point4D();
                        string string4 = "10,20,30,40";

                        point4DResult = (Point4D)p4DConverter.ConvertFromString(string4);
                        // point4DResult is equal to (10, 20, 30)

                        // Displaying Results
                        syntaxString = "point4DResult = (Point4D)v3DConverter.ConvertFromString(string3);";
                        resultType = "Point4D";
                        operationString = "Converting a String into a Point4D";
                        ShowResults(point4DResult.ToString(), syntaxString, resultType, operationString);
                        break;
					}

                    default:
                    break;

            } //end switch
		}


        // Displays the results of the operation
        private void ShowResults(String resultValue, String syntax, String resultType, String opString)	
		{

			txtResultValue.Text = resultValue;
			txtSyntax.Text = syntax;
			txtResultType.Text = resultType;
			txtOperation.Text = opString;
		}

        // Displays the values of the variables
        public void ShowVars()
        {

            string s1 = "10, 20";
            string s2 = "10, 20, 30, 40, 50, 60";
            string s3 = "10, 20, 30";
            string s4 = "10, 20, 30, 40";

            // Displaying values in Text objects
            
            txtString1.Text = s1.ToString();
            txtString2.Text = s2.ToString();
            txtString3.Text = s3.ToString();
            txtString4.Text = s4.ToString();
		}
	}
}