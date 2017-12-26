//This is a list of commonly used namespaces for a window.
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace PointSample
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
			
			RadioButton li = sender as RadioButton;
			
            // Strings used to display the results
            String syntaxString, resultType, operationString;
					
			// The local variables point1, point2, vector2, etc are defined in each
			// case block for readability reasons. Each variable is contained within
			// the scope of each case statement.  
			switch (li.Name)
			{   //begin switch

                case "rb1":
                    {
                        // Translates a Point by a Vector using the overloaded + operator. 
                        // Returns a Point.
                        Point point1 = new Point(10, 5);
                        Vector vector1 = new Vector(20, 30);
                        Point pointResult = new Point();

                        pointResult = point1 + vector1;
                        // pointResult is equal to (30, 35)

                        // Note: Adding a Point to a Point is not a legal operation 

                        // Displaying Results
                        syntaxString = "pointResult = point1 + vector1;";
                        resultType = "Point";
                        operationString = "Adding a Point and Vector";
                        ShowResults(pointResult.ToString(), syntaxString, resultType, operationString);
                        break;
                    }

                case "rb2":
					{
                        // Translates a Point by a Vector using the static Add method.
                        // Returns a Point.  
                        Point point1 = new Point(10, 5);
                        Vector vector1 = new Vector(20, 30);
                        Point pointResult = new Point();

                        pointResult = Point.Add(point1, vector1);
                        // pointResult is equal to (30, 35)

                        // Displaying Results
						syntaxString = "pointResult = Point.Add(point1, vector1);";
						resultType = "Point";
						operationString = "Adding a Point and Vector";
						ShowResults(pointResult.ToString(), syntaxString, resultType, operationString);
						break;
					}

				case "rb3":
					{
                        // Subtracts a Vector from a Point using the overloaded - operator.
                        // Returns a Point.
                        Point point1 = new Point(10, 5);
                        Vector vector1 = new Vector(20, 30);
                        Point pointResult = new Point();

                        pointResult = point1 - vector1;
                        // pointResult is equal to (-10, -25) 

                        // Displaying Results
						syntaxString = "pointResult = point1 - vector1;";
						resultType = "Point";
						operationString = "Subtracting a Vector from a Point";
						ShowResults(pointResult.ToString(), syntaxString, resultType, operationString);
						break;
					}

				case "rb4":
					{
                        // Subtracts a Vector from a Point using the static Subtract method. 
                        // Returns a Point.
                        Point point1 = new Point(10, 5);
                        Vector vector1 = new Vector(20, 30);
                        Point pointResult = new Point();

                        pointResult = Point.Subtract(point1, vector1);
                        // pointResult is equal to (-10, -25)

                        // Displaying Results
						syntaxString = "pointResult = Point.Subtract(point1, vector1);";
						resultType = "Point";
						operationString = "Subtracting a Vector from a Point";
						ShowResults(pointResult.ToString(), syntaxString, resultType, operationString);
						break;
					}

				case "rb5":
					{
                        // Subtracts a Point from a Point using the overloaded - operator.
                        // Returns a Vector.
                        Point point1 = new Point(10, 5);
                        Point point2 = new Point(15, 40);
                        Vector vectorResult = new Vector();

                        vectorResult = point1 - point2;
                        // vectorResult is equal to (-5, -35)

                        // Displaying Results
						syntaxString = "vectorResult = point1 - point2;";
						resultType = "Vector";
						operationString = "Subtracting a Point from a Point";
						ShowResults(vectorResult.ToString(), syntaxString, resultType, operationString);
						break;
					}

                case "rb6":
					{
                        // Subtracts a Point from a Point using the static Subtract method.  
                        // Returns a Vector.
                        Point point1 = new Point(10, 5);
                        Point point2 = new Point(15, 40);
                        Vector vectorResult = new Vector();

                        vectorResult = Point.Subtract(point1, point2);
                        // vectorResult is equal to (-5, -35)

                        // Displaying Results
						syntaxString = "vectorResult = Point.Subtract(point1, point2);";
						resultType = "Vector";
						operationString = "Subtracting a Point from a Point";
						ShowResults(vectorResult.ToString(), syntaxString, resultType, operationString);
						break;
					}

				case "rb7":
					{
                        // Offsets the X and Y values of a Point.
                        Point point1 = new Point(10, 5);

                        point1.Offset(20, 30);
                        // point1 is equal to (30, 35)

                        // Note: This operation is equivalent to adding a point 
                        // to vector with the corresponding X,Y values.

                        // Displaying Results
						syntaxString = "point1.Offset(20,30);";
						resultType = "Point";
						operationString = "Offsetting a Point";
						ShowResults(point1.ToString(), syntaxString, resultType, operationString);
						break;
					}

				case "rb8":
					{
                        // Multiplies a Point by a Matrix.  
                        // Returns a Point.
						Point point1 = new Point(10, 5);
                        Point pointResult = new Point();
                        Matrix matrix1 = new Matrix(40, 50, 60, 70, 80, 90);

						pointResult = point1 * matrix1;
                        // pointResult is equal to (780, 940)

						// Displaying Results
						resultType = "Point";
						syntaxString = "pointResult = point1 * matrix1;";
						operationString = "Multiplying a Point by a Matrix";
                        ShowResults(pointResult.ToString(), syntaxString, resultType, operationString);
                        break;
					}

				case "rb9":
					{
                        // Multiplies a Point by a Matrix.  
                        // Returns a Point.
						Point point1 = new Point(10, 5);
                        Point pointResult = new Point();
                        Matrix matrix1 = new Matrix(40, 50, 60, 70, 80, 90);

                        pointResult = Point.Multiply(point1, matrix1);
                        // pointResult is equal to (780, 940)

                        // Displaying Results
						resultType = "Point";
						syntaxString = "pointResult = Point.Multiply(point1, matrix1);";
						operationString = "Multiplying a Point by a Matrix";
                        ShowResults(pointResult.ToString(), syntaxString, resultType, operationString);
                        break;
					}

                case "rb10":
                    {
                        // Checks if two Points are equal using the overloaded equality operator.
                        Point point1 = new Point(10, 5);
                        Point point2 = new Point(15, 40);
                        Boolean areEqual;

                        areEqual = (point1 == point2);
                        // areEqual is False

                        // Displaying Results
                        syntaxString = "areEqual = (point1 == point2);";
                        resultType = "Boolean";
                        operationString = "Checking if two points are equal";
                        ShowResults(areEqual.ToString(), syntaxString, resultType, operationString);
                        break;
                    }
    

                case "rb11":
                    {
                        // Checks if two Points are equal using the static Equals method.
                        Point point1 = new Point(10, 5);
                        Point point2 = new Point(15, 40);
                        Boolean areEqual;

                        areEqual = Point.Equals(point1, point2);
                        // areEqual is False	

                        // Displaying Results
                        syntaxString = "areEqual = Point.Equals(point1, point2);";
                        resultType = "Boolean";
                        operationString = "Checking if two points are equal";
                        ShowResults(areEqual.ToString(), syntaxString, resultType, operationString);
                        break;
                    }
                case "rb12":
                    {
                        // Compares an Object and a Point for equality using the non-static Equals method.
                        Point point1 = new Point(10, 5);
                        Point point2 = new Point(15, 40);
                        Boolean areEqual;

                        areEqual = point1.Equals(point2);
                        // areEqual is False	


                        // Displaying Results
                        syntaxString = "areEqual = point1.Equals(point2);";
                        resultType = "Boolean";
                        operationString = "Checking if two points are equal";
                        ShowResults(areEqual.ToString(), syntaxString, resultType, operationString);
                        break;
                    }
                
                case "rb13":
                    {
                        // Compares an Object and a Vector for equality using the non-static Equals method.
                        Vector vector1 = new Vector(20, 30);
                        Vector vector2 = new Vector(45, 70);
                        Boolean areEqual;

                        areEqual = vector1.Equals(vector2);
                        // areEqual is False	


                        // Displaying Results
                        syntaxString = "areEqual = vector1.Equals(vector2);";
                        resultType = "Boolean";
                        operationString = "Checking if two vectors are equal";
                        ShowResults(areEqual.ToString(), syntaxString, resultType, operationString);
                        break;
                    }

                case "rb14":
                    {
                        // Converts a string representation of a point into a Point structure
                        Point pointResult = new Point();

                        pointResult = Point.Parse("1,3");
                        // pointResult is equal to (1, 3)

                        // Displaying Results
                        syntaxString = "pointResult = Point.Parse(\"1,3\");";
                        resultType = "Matrix";
                        operationString = "Converts a string into a Point structure.";
                        ShowResults(pointResult.ToString(), syntaxString, resultType, operationString);
                        break;
                    }
                case "rb15":
                    {
                        // Gets a string representation of a Point structure
                        Point point1 = new Point(10, 5);
                        String pointString;

                        pointString = point1.ToString();
                        // pointString is equal to 10,5

                        // Displaying Results
                        syntaxString = "pointString = point1.ToString();";
                        resultType = "String";
                        operationString = "Getting the string representation of a Point";
                        ShowResults(pointString.ToString(), syntaxString, resultType, operationString);
                        break;
                    }
                case "rb16":
                    {
                        // Gets the hashcode of a Point structure

                        Point point1 = new Point(10, 5);
                        int pointHashCode;

                        pointHashCode = point1.GetHashCode();

                        // Displaying Results
                        syntaxString = "pointHashCode = point1.GetHashCode();";
                        resultType = "int";
                        operationString = "Getting the hashcode of Point";
                        ShowResults(pointHashCode.ToString(), syntaxString, resultType, operationString);
                        break;
                    }
                case "rb17":
                    {
                        // Explicitly converts a Point structure into a Size structure
                        // Returns a Size.

                        Point point1 = new Point(10, 5);
                        Size size1 = new Size();

                        size1 = (Size)point1;
                        // size1 has a width of 10 and a height of 5

                        // Displaying Results
                        syntaxString = "size1 = (Size)point1;";
                        resultType = "Size";
                        operationString = "Expliciting casting a Point into a Size";
                        ShowResults(size1.ToString(), syntaxString, resultType, operationString);
                        break;
                    }

                case "rb18":
                    {
                        // Explicitly converts a Point structure into a Vector structure
                        // Returns a Vector.

                        Point point1 = new Point(10, 5);
                        Vector vector1 = new Vector();

                        vector1 = (Vector)point1;
                        // vector1 is equal to (10,5)

                        // Displaying Results
                        syntaxString = "vector1 = (Vector)point1;";
                        resultType = "Vector";
                        operationString = "Expliciting casting a Point into a Vector";
                        ShowResults(vector1.ToString(), syntaxString, resultType, operationString);
                        break;
                    }

                // task example.  Not accessed through radio buttons
                case "rb20":
                    {
                        // Checks if two Points are not equal using the overloaded inequality operator.

                        // Declaring point1 and initializing x,y values
                        Point point1 = new Point(10, 5);

                        // Declaring point2 without initializing x,y values
                        Point point2 = new Point();

                        // Boolean to hold the result of the comparison
                        Boolean areNotEqual;

                        // assigning values to point2
                        point2.X = 15;
                        point2.Y = 40;

                        // checking for inequality
                        areNotEqual = (point1 != point2);

                        // areNotEqual is True

                        // Displaying Results
                        syntaxString = "areNotEqual = (point1 != point2);";
                        resultType = "Boolean";
                        operationString = "Checking if two points are not equal";
                        ShowResults(areNotEqual.ToString(), syntaxString, resultType, operationString);
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

			Point p1 = new Point(10, 5);
			Point p2 = new Point(15, 40);

			Vector v1 = new Vector(20, 30);
			Vector v2 = new Vector(45, 70);

			Matrix m1 = new Matrix(40, 50, 60, 70, 80, 90);

            // Displaying values in Text objects
			txtPoint1.Text = p1.ToString();
			txtPoint2.Text = p2.ToString();
			txtVector1.Text = v1.ToString();
			txtVector2.Text = v2.ToString();	
			txtMatrix1.Text = m1.ToString();
		}
	}
}