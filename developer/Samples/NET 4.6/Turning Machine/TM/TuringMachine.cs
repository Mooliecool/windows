using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.ComponentModel;


/**************************************************************************
 * Turing Machine 
 * (C) 2016 Jonathan Moore
 /**************************************************************************/
namespace TuringMachineSimulator
{

    #region Delegates

    public delegate void MachineHeadMoveEventHandler(object sender, EventArgs e);
    public delegate void MachineHaltEventHandler(object sender, EventArgs e);
    public delegate void MachineProcessEventHandler(object sender, EventArgs e);
    public delegate void MachineStartEventHandler(object sender, EventArgs e);
    public delegate void MachineTapeResizeEventHandler(object sender, EventArgs e);
    public delegate void MachineExceptionEventHandler(Exception sender, EventArgs e);

    #endregion

    [Serializable]
    class TuringMachine
    {

        #region Events

        public event MachineHeadMoveEventHandler MachineHeadMove;
        public event MachineHaltEventHandler MachineHalt;
        public event MachineProcessEventHandler MachineProcess;
        public event MachineStartEventHandler MachineStart;
        public event MachineTapeResizeEventHandler MachineTapeResize;
        public event MachineExceptionEventHandler MachineException;

        #endregion 

        #region Variables

        private  ArrayList _state_table;

        private  string[] _tape;      
        
        private  int _number_of_symbols = 0;
        private  int _number_of_shifts = 0;
        private  int _sigma = 0;
        private  int _number_of_states = 0;                
        private  int _curr_tape_pos = 0;
        private  int _tape_lenght = 0;
        private string _fill_symbol = "";
        private string _blank_symbol = "#";
        private  string _curr_state = "";
        private string _curr_symbol = "";
        private string _init_state = "";
        private  string _name = "";
        private string _total_processing_time = "";
  
        private  bool _processing = false;
        private bool _pause = false;
        private bool _step_flag = false;

        private string _next_direction = "";
        private string _symbol_to_print = "";
        private string _next_state = "";

        private bool _disable_process_event = false;

        private DateTime _startTime;
        private DateTime _endTime;

        #endregion

        #region TuringMachine

        public TuringMachine() 
        { 
        
        }

        private void MakeException(string message) 
        {
            Exception exp = new Exception(message); 
            MachineException(exp,null);
            HaltMachine(); 
        }

        [BrowsableAttribute(false)]        
        public ArrayList StateTable 
        {

            get 
            {
                return _state_table;
            }

            set 
            {
                _state_table = value;
            }

        }

        public int getStateIndex(string state) 
        {

            switch (state)
            {
                case "A":
                    return 0;                    
                case "B":
                    return 1;                    
                case "C":
                    return 2;                    
                case "D":
                    return 3;                    
                case "E":
                    return 4;                    
                case "F":
                    return 5;                    
                case "G":
                    return 6;                    
                case "H":
                    return 7;                    
                case "I":
                    return 8;                    
                case "J":
                    return 9;                    
                case "K":
                    return 10;                    
                case "L":
                    return 11;
                case "Z": 
                    return 12;
                default:
                    return -1;
            };

        }

        private void MoveHeadToRight() 
        {

            _curr_tape_pos = _curr_tape_pos + 1;

            if (_curr_tape_pos > _tape_lenght) 
            {
                _curr_tape_pos = _tape_lenght;
                HaltMachine(); 
                MakeException("Execution reached the end of the Tape"); 
            }                        
        }

        private void MoveHeadToLeft()
        {

            _curr_tape_pos = _curr_tape_pos - 1;

            if (_curr_tape_pos < 0)
            {
                _curr_tape_pos = _tape_lenght;
                HaltMachine();
                MakeException("Execution reached the end of the Tape");
            }
            
        }

        public void HaltMachine()
        {
            _endTime = DateTime.Now;
            _total_processing_time = Convert.ToString(_endTime.Subtract(_startTime));   
            _processing = false;       
            CalculateSigma();
            MachineHalt(this, null);
        }        

        public void StartMachine()
        {
            _number_of_shifts = 0;
            _sigma = 0;
            _processing = true;

            MachineStart(this, null);
            _startTime = DateTime.Now;

            RunTuringMachine(); 

        }

        private void RunTuringMachine() 
        {

            while (_processing && !_pause )
            {
                    Process();
                    _number_of_shifts++;
            }           

        }

        public void RunStep() 
        {

            if (_processing == false)
                return;

            if (!_step_flag)
            {
                _step_flag = !_step_flag;
                ReadTable(ReadFromTape());
            }
            else 
            {
                _step_flag = !_step_flag;
                ExecuteTable();
                _number_of_shifts++;
                RunStep(); 
            }
            

        }

        [Browsable(false)]
        public bool Pause
        {
            get
            {
                return _pause;
            }
            set
            {
                _pause = value;
            }
        }

        public void Resume()
        {
            RunTuringMachine();
        }

        private void Process() 
        {
            if (_processing == false)
                return;
            
            ReadTable(ReadFromTape());
            ExecuteTable(); 
        }

        private void ReadTable(string symbol) 
        {

            if (symbol == "" || symbol == null) 
            {
                MakeException("Machine Error: null symbol to evaluate");
                return;
            }

            int s = 0;

            try
            {
                s = Convert.ToInt16(symbol);
            }catch(Exception ex) 
            {
                MakeException(ex.Message + "This may be caused because this version of the Simulator only accepts numbers as tape symbols or fill symbols"); 
            }

            string function = "";

            StateTableRow tableRow;
            try
            {
                tableRow = (StateTableRow)_state_table[s];
            }catch(Exception ex) 
            {
                MakeException(ex.Message + "\r\n" + "The current symbol to access the state table is not defined or it's ilegal. There is no entry in the state table matrix to retrieve a function");
                HaltMachine();
                return;
            }

            switch(_curr_state) 
            {            
                case "A":
                    function = tableRow.A;
                    break;
                case "B":
                    function = tableRow.B;
                    break;
                case "C":
                    function = tableRow.C;
                    break;
                case "D":
                    function = tableRow.D;
                    break;
                case "E":
                    function = tableRow.E;
                    break;
                case "F":
                    function = tableRow.F;
                    break;
                case "G":
                    function = tableRow.G;
                    break;
                case "H":
                    function = tableRow.H;
                    break;
                case "I":
                    function = tableRow.I;
                    break;
                case "J":
                    function = tableRow.J;
                    break;
                case "K":
                    function = tableRow.K;
                    break;
                case "L":
                    function = tableRow.L;
                    break;
                case "Z":
                    HaltMachine();
                    return;                                
            };

            if (function == "") 
            {
                MakeException("Machine Error: no function to perform - check you state table. This can be caused by a non existent 'to go STATE (A,B,C...)' on the state table. Or a initial state was not set");
                HaltMachine();
                return;
            }

            string[] steps = function.Split(',');

            _symbol_to_print = steps[0].Substring(1, 1);
            _next_direction = steps[1];
            _next_state = steps[2];

            if (_disable_process_event == false)
                MachineProcess(this, null);

        }

        private void ExecuteTable() 
        {           

            PrintOnTape(_symbol_to_print);

            switch (_next_direction)
            {
                case "R":
                    MoveHeadToRight();
                    break;
                case "L":
                    MoveHeadToLeft();
                    break;
            };

            _curr_state = _next_state;

            if (_disable_process_event == false)
                MachineHeadMove(this, null);


        }

        private void CalculateSigma() 
        {
            for (int p = 0; p <= _tape_lenght - 1; p++) 
            {
                if (_tape[p] == "1") 
                {
                    _sigma++;
                }
            }
        }

        #endregion

        #region Tape

        private void PrintOnTape(string value) 
        {
            try{
                _tape[_curr_tape_pos]=value;
}
            catch (Exception ex) 
            {
                MakeException(ex.Message);                
            }
        }

        private string ReadFromTape() 
        {
            try
            {
                _curr_symbol = _tape[_curr_tape_pos];
                return _tape[_curr_tape_pos];
            }
            catch (Exception ex) 
            {
                MakeException(ex.Message);
                return "";
            }
        }

        public void SetTapeValue(string value, int pos)
        {
            _tape[pos] = value;
        }

        public string GetTapeValue(int pos)
        {
            if (_tape == null)
                return "null";

            if (_tape[pos] == null)
                _tape[pos] = _blank_symbol;

            return _tape[pos];
        }

        #endregion

        #region ExecutionProperties

        [CategoryAttribute("Execution")]
        [DescriptionAttribute("True if the Machine is running or False if not. Exceptions, user interruptions or normal halt can make the machine stop")]
        public bool Running
        {
            get
            {
                return _processing;
            }

            set
            {
                _processing = value;
            }


        }

        [CategoryAttribute("Execution")]
        [DescriptionAttribute("Set to True if you want to start in Debug Mode")]
        public bool StepThrough
        {
            get
            {
                return _pause;
            }
            set
            {
                _pause = value;
            }
        }

        #endregion

        #region TotalsProperties

        [CategoryAttribute("Totals")]
        [DescriptionAttribute("Numner of Shifts")]
        public int Shifts
        {
            get
            {
                return _number_of_shifts;
            }

        }

        [CategoryAttribute("Totals")]
        [DescriptionAttribute("Total processing time HH:MM:SS")]
        public string TotalProcessingTime
        {
            get
            {
                return _total_processing_time;
            }

        }


        [CategoryAttribute("Totals")]
        [DescriptionAttribute("The Sigma Function result: total number of 1(s) printed in the tape after running the Busy Beaver Game")]
        public int Sigma
        {
            get
            {
                return _sigma;
            }

        }

        #endregion

        #region TapeConfigurarionProperties

        [CategoryAttribute("Tape Configuration")]
        [DescriptionAttribute("Symbol to fill the entire tape before execution")]
        public string FillSymbol
        {
            get
            {
                return _fill_symbol;
            }
            set
            {
                _fill_symbol = value;
            }
        }

        [CategoryAttribute("Tape Configuration")]
        [DescriptionAttribute("Tape length")]
        public int TapeLength
        {
            get
            {
                return _tape_lenght;
            }
            set
            {
                _tape_lenght = value;
                _tape = null;
                _tape = new string[_tape_lenght];
                MachineTapeResize(this, null);
            }
        }

        #endregion

        #region GeneralProperties

        [CategoryAttribute("General")]
        [DescriptionAttribute("Disable update events for fast processing")]
        public bool DisableUpdateEvent
        {
            get
            {
                return _disable_process_event;
            }
            set
            {
                _disable_process_event = value;
            }
        }

        [CategoryAttribute("General")]
        [DescriptionAttribute("Name")]
        public string Name
        {
            get
            {
                return _name;
            }
            set
            {
                _name = value;
            }
        }

        [CategoryAttribute("General")]
        [DescriptionAttribute("Initial state to start the machine")]
        public string InitialState
        {
            get
            {
                return _init_state;
            }
            set
            {
                _init_state = value;
            }
        }

        #endregion

        #region ActualStateProperties

        [CategoryAttribute("Actual State")]
        [DescriptionAttribute("Current State of the Head")]
        public string CurrentState
        {
            get
            {
                return _curr_state;
            }
            set
            {
                _curr_state = value;
            }
        }

        [CategoryAttribute("Actual State")]
        [DescriptionAttribute("Current Symbol read")]
        public string CurrentSymbol
        {
            get
            {
                return _curr_symbol;
            }
            set
            {
                _curr_symbol = value;
            }
        }

        [CategoryAttribute("Actual State")]
        [DescriptionAttribute("Symbol to Print")]
        public string SymbolToPrint
        {
            get
            {
                return _symbol_to_print;
            }
        }

        [CategoryAttribute("Actual State")]
        [DescriptionAttribute("Next direction")]
        public string NextDirection
        {
            get
            {
                return _next_direction;
            }
        }

        [CategoryAttribute("Actual State")]
        [DescriptionAttribute("Next state to go")]
        public string NextStateToGo
        {
            get
            {
                return _next_state;
            }
        }

        [CategoryAttribute("Actual State")]
        [DescriptionAttribute("Current Head position on the Tape")]
        public int CurrentTapePosition
        {
            get
            {
                return _curr_tape_pos;
            }
            set
            {

                _curr_tape_pos = value;

                if (_curr_tape_pos < 0)
                {
                    _curr_tape_pos = _tape_lenght;
                    _processing = false;
                    MakeException("Head tried to reached a position before the start of the Tape");
                }

                if (_curr_tape_pos > _tape_lenght)
                {
                    _curr_tape_pos = _tape_lenght;
                    _processing = false;
                    MakeException("Head has passed the end of the Tape");
                }

                MachineHeadMove(this, null);

            }
        }

        #endregion

        #region TableProperties

        [CategoryAttribute("Table")]
        [DescriptionAttribute("Number of Symbols")]
        public int NumberOfSymbols
        {
            get
            {
                return _number_of_symbols;
            }
            set
            {
                _number_of_symbols = value;
            }
        }

        [CategoryAttribute("Table")]
        [DescriptionAttribute("Number of States")]
        public int NumberOfStates
        {
            get
            {
                return _number_of_states;
            }
            set
            {
                _number_of_states = value;
            }
        }

        [CategoryAttribute("Table")]
        [DescriptionAttribute("Blank Symbol")] 
        public string BlankSymbol
        {
            get
            {
                return _blank_symbol;
            }
            set
            {
                _blank_symbol = value;                
            }
        }

        #endregion

    }

}
