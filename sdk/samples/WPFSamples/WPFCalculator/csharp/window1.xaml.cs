using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Windows.Media.Animation;

namespace WPFCalculator
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : Window
    {
        static MyTextBox DisplayBox;
        static MyTextBox PaperBox;
        static PaperTrail Paper;

        public Window1()
            : base()
        {
            InitializeComponent();
            //sub-class our textBox
            DisplayBox = new MyTextBox();
            Grid.SetRow(DisplayBox, 0);
            Grid.SetColumn(DisplayBox, 0);
            Grid.SetColumnSpan(DisplayBox, 9);
            DisplayBox.Height = 30;
            MyGrid.Children.Add(DisplayBox);

            //sub-class our paper trail textBox
            PaperBox = new MyTextBox();
            Grid.SetRow(PaperBox, 1);
            Grid.SetColumn(PaperBox, 0);
            Grid.SetColumnSpan(PaperBox, 3);
            Grid.SetRowSpan(PaperBox, 5);
            PaperBox.IsReadOnly = true;
            PaperBox.VerticalScrollBarVisibility = ScrollBarVisibility.Visible;
            PaperBox.Margin = new Thickness(3.0,1.0,1.0,1.0);
            PaperBox.HorizontalScrollBarVisibility = ScrollBarVisibility.Auto;

            Paper = new PaperTrail();

            MyGrid.Children.Add(PaperBox);
            ProcessKey('0');
            EraseDisplay = true;

        }

        private enum Operation
        {
            None,
            Devide,
            Multiply,
            Subtract,
            Add,
            Percent,
            Sqrt,
            OneX,
            Negate
        }
        private Operation LastOper;
        private string _display;
        private string _last_val;
        private string _mem_val;
        private bool _erasediplay;

        //flag to erase or just add to current display flag
        private bool EraseDisplay
        {
            get
            {
                return _erasediplay;

            }
            set
            {
                _erasediplay = value;
            }
        }
        //Get/Set Memory cell value
        private Double Memory
        {
            get
            {
                if (_mem_val == string.Empty)
                    return 0.0;
                else
                    return Convert.ToDouble(_mem_val);
            }
            set
            {
                _mem_val = value.ToString();
            }
        }
        //Lats value entered
        private string LastValue
        {
            get
            {
                if (_last_val == string.Empty)
                    return "0";
                return _last_val;

            }
            set
            {
                _last_val = value;
            }
        }
        //The current Calculator display
        private string Display
        {
            get
            {
                return _display;
            }
            set
            {
                _display = value;
            }
        }

        // Sample event handler:  
        private void OnWindowKeyDown(object sender, System.Windows.Input.TextCompositionEventArgs /*System.Windows.Input.KeyEventArgs*/ e)
        {
            string s = e.Text;

            if(s.Length == 0)
                return ;

            char c = (s.ToCharArray())[0];
            e.Handled = true;

            if ((c >= '0' && c <= '9') || c == '.' || c == '\b')  // '\b' is backspace
            {
                ProcessKey(c);
                return;
            }
            switch (c)
            {
                case '+':
                    ProcessOperation("BPlus");
                    break;
                case '-':
                    ProcessOperation("BMinus");
                    break;
                case '*':
                    ProcessOperation("BMultiply");
                    break;
                case '/':
                    ProcessOperation("BDevide");
                    break;
                case '%':
                    ProcessOperation("BPercent");
                    break;
                case '=':
                    ProcessOperation("BEqual");
                    break;
            }

        }
        private void DigitBtn_Click(object sender, RoutedEventArgs e)
        {
            string s = ((Button)sender).Content.ToString();

            //char[] ids = ((Button)sender).ID.ToCharArray();
            char[] ids = s.ToCharArray();
            ProcessKey(ids[0]);

        }
        private void ProcessKey(char c)
        {
            if (EraseDisplay)
            {
                Display = string.Empty;
                EraseDisplay = false;
            }
            AddToDisplay(c);
        }
        private void ProcessOperation(string s)
        {
            Double d = 0.0;
            switch (s)
            {
                case "BPM":
                    LastOper = Operation.Negate;
                    LastValue = Display;
                    CalcResults();
                    LastValue = Display;
                    EraseDisplay = true;
                    LastOper = Operation.None;
                    break;
                case "BDevide":

                    if (EraseDisplay)    //stil wait for a digit...
                    {  //stil wait for a digit...
                        LastOper = Operation.Devide;
                        break;
                    }
                    CalcResults();
                    LastOper = Operation.Devide;
                    LastValue = Display;
                    EraseDisplay = true;
                    break;
                case "BMultiply":
                    if (EraseDisplay)    //stil wait for a digit...
                    {  //stil wait for a digit...
                        LastOper = Operation.Multiply;
                        break;
                    }
                    CalcResults();
                    LastOper = Operation.Multiply;
                    LastValue = Display;
                    EraseDisplay = true;
                    break;
                case "BMinus":
                    if (EraseDisplay)    //stil wait for a digit...
                    {  //stil wait for a digit...
                        LastOper = Operation.Subtract;
                        break;
                    }
                    CalcResults();
                    LastOper = Operation.Subtract;
                    LastValue = Display;
                    EraseDisplay = true;
                    break;
                case "BPlus":
                    if (EraseDisplay)
                    {  //stil wait for a digit...
                        LastOper = Operation.Add;
                        break;
                    }
                    CalcResults();
                    LastOper = Operation.Add;
                    LastValue = Display;
                    EraseDisplay = true;
                    break;
                case "BEqual":
                    if (EraseDisplay)    //stil wait for a digit...
                        break;
                    CalcResults();
                    EraseDisplay = true;
                    LastOper = Operation.None;
                    LastValue = Display;
                    //val = Display;
                    break;
                case "BSqrt":
                    LastOper = Operation.Sqrt;
                    LastValue = Display;
                    CalcResults();
                    LastValue = Display;
                    EraseDisplay = true;
                    LastOper = Operation.None;
                    break;
                case "BPercent":
                    if (EraseDisplay)    //stil wait for a digit...
                    {  //stil wait for a digit...
                        LastOper = Operation.Percent;
                        break;
                    }
                    CalcResults();
                    LastOper = Operation.Percent;
                    LastValue = Display;
                    EraseDisplay = true;
                    //LastOper = Operation.None;
                    break;
                case "BOneOver":
                    LastOper = Operation.OneX;
                    LastValue = Display;
                    CalcResults();
                    LastValue = Display;
                    EraseDisplay = true;
                    LastOper = Operation.None;
                    break;
                case "BC":  //clear All
                    LastOper = Operation.None;
                    Display = LastValue = string.Empty;
                    Paper.Clear();
                    UpdateDisplay();
                    break;
                case "BCE":  //clear entry
                    LastOper = Operation.None;
                    Display = LastValue;
                    UpdateDisplay();
                    break;
                case "BMemClear":
                    Memory = 0.0F;
                    DisplayMemory();
                    break;
                case "BMemSave":
                    Memory = Convert.ToDouble(Display);
                    DisplayMemory();
                    EraseDisplay = true;
                    break;
                case "BMemRecall":
                    Display = /*val =*/ Memory.ToString();
                    UpdateDisplay();
                    //if (LastOper != Operation.None)   //using MR is like entring a digit
                    EraseDisplay = false;
                    break;
                case "BMemPlus":
                    d = Memory + Convert.ToDouble(Display);
                    Memory = d;
                    DisplayMemory();
                    EraseDisplay = true;
                    break;
            }

        }

        private void OperBtn_Click(object sender, RoutedEventArgs e)
        {
            ProcessOperation(((Button)sender).Name.ToString());
        }


        private double Calc(Operation LastOper)
        {
            double d = 0.0;


            try {
            switch (LastOper)
            {
                case Operation.Devide:
                    Paper.AddArguments(LastValue + " / " + Display);
                    d = (Convert.ToDouble(LastValue) / Convert.ToDouble(Display));
                    CheckResult(d);
                    Paper.AddResult(d.ToString());
                    break;
                case Operation.Add:
                    Paper.AddArguments(LastValue + " + " + Display);
                    d = Convert.ToDouble(LastValue) + Convert.ToDouble(Display);
                    CheckResult(d);
                    Paper.AddResult(d.ToString());
                    break;
                case Operation.Multiply:
                    Paper.AddArguments(LastValue + " * " + Display);
                    d = Convert.ToDouble(LastValue) * Convert.ToDouble(Display);
                    CheckResult(d);
                    Paper.AddResult(d.ToString());
                    break;
                case Operation.Percent:
                    //Note: this is different (but make more sense) then Windows calculator
                    Paper.AddArguments(LastValue + " % " + Display);
                    d = (Convert.ToDouble(LastValue) * Convert.ToDouble(Display)) / 100.0F;
                    CheckResult(d);
                    Paper.AddResult(d.ToString());
                    break;
                case Operation.Subtract:
                    Paper.AddArguments(LastValue + " - " + Display);
                    d = Convert.ToDouble(LastValue) - Convert.ToDouble(Display);
                    CheckResult(d);
                    Paper.AddResult(d.ToString());
                    break;
                case Operation.Sqrt:
                    Paper.AddArguments("Sqrt( " + LastValue + " )");
                    d = Math.Sqrt(Convert.ToDouble(LastValue));
                    CheckResult(d);
                    Paper.AddResult(d.ToString());
                    break;
                case Operation.OneX:
                    Paper.AddArguments("1 / " + LastValue);
                    d = 1.0F / Convert.ToDouble(LastValue);
                    CheckResult(d);
                    Paper.AddResult(d.ToString());
                    break;
                case Operation.Negate:
                    d = Convert.ToDouble(LastValue) * (-1.0F);
                    break;
                }
            }
            catch {
                d = 0;
                Window parent = (Window)MyPanel.Parent;
                Paper.AddResult("Error");
                MessageBox.Show(parent, "Operation cannot be perfomed", parent.Title);
            }

            return d;
        }
        private void CheckResult(double d)
        {
            if (Double.IsNegativeInfinity(d) || Double.IsPositiveInfinity(d) || Double.IsNaN(d))
                throw new Exception("Illegal value");
        }

        private void DisplayMemory()
        {
            if (_mem_val != String.Empty)
                BMemBox.Text = "Memory: " + _mem_val;
            else
                BMemBox.Text = "Memory: [empty]";
        }
        private void CalcResults()
        {
            double d;
            if (LastOper == Operation.None)
                return;

            d = Calc(LastOper);
            Display = d.ToString();

            UpdateDisplay();
        }

        private void UpdateDisplay()
        {
            if (Display == String.Empty)
                DisplayBox.Text = "0";
            else
                DisplayBox.Text = Display;
        }
        private void AddToDisplay(char c)
        {
            if (c == '.')
            {
                if (Display.IndexOf('.', 0) >= 0)  //already exists
                    return;
                Display = Display + c;
            }
            else
            {
                if (c >= '0' && c <= '9') {
                    Display = Display + c;
                }
                else
                if (c == '\b')  //backspace ?
                {
                    if (Display.Length <= 1)
                        Display = String.Empty;
                    else
                    {
                        int i = Display.Length;
                        Display = Display.Remove(i - 1, 1);  //remove last char 
                    }
                }

            }

            UpdateDisplay();
        }
   
        void OnMenuAbout(object sender, RoutedEventArgs e)
        {
            Window parent = (Window)MyPanel.Parent;
            MessageBox.Show(parent, parent.Title + " - By Jossef Goldberg ", parent.Title,MessageBoxButton.OK, MessageBoxImage.Information);
        }
        void OnMenuExit(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
        void OnMenuStandard(object sender, RoutedEventArgs e)
        {
            //((MenuItem)ScientificMenu).IsChecked = false;
            ((MenuItem)StandardMenu).IsChecked = true; //for now always Standard
        }
        //Not implemenetd 
        void OnMenuScientific(object sender, RoutedEventArgs e)
        {
           //((MenuItem)StandardMenu).IsChecked = false; 
        }   
        private class PaperTrail
        {
            string args;

            public PaperTrail()
            {
            }
            public void AddArguments(string a)
            {
                args = a;
            }
            public void AddResult(string r)
            {
                PaperBox.Text += args + " = " + r + "\n";
            }
            public void Clear()
            {
                PaperBox.Text = string.Empty;
                args = string.Empty;
            }
        }


    }


}