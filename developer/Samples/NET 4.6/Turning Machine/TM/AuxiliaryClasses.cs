using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

/**************************************************************************
 * Turing Machine Simulator
 * (C) 2012 Toni Novak
 * toninovka@gmail.com 
 * License: The Code Project Open License (CPOL) 1.02
 * http://www.codeproject.com/info/cpol10.aspx
/**************************************************************************/

namespace TuringMachineSimulator
{

    [Serializable]
    class StateTableRow
    {

        public StateTableRow()
        {
        }

        public StateTableRow(
            int StateSymbol,
            string A,
            string B,
            string C,
            string D,
            string E,
            string F,
            string G,
            string H,
            string I,
            string J,
            string K,
            string L
            )
        {
            _TapeSymbol = StateSymbol;
            _A = A;
            _B = B;
            _C = C;
            _D = D;
            _E = E;
            _F = F;
            _G = G;
            _H = H;
            _I = I;
            _J = J;
            _K = K;
            _L = L;

        }


        private int _TapeSymbol;
        private string _A;
        private string _B;
        private string _C;
        private string _D;
        private string _E;
        private string _F;
        private string _G;
        private string _H;
        private string _I;
        private string _J;
        private string _K;
        private string _L;

        public int TapeSymbol { get { return _TapeSymbol; } set { _TapeSymbol = value; } }
        public string A { get { return _A; } set { _A = value; } }
        public string B { get { return _B; } set { _B = value; } }
        public string C { get { return _C; } set { _C = value; } }
        public string D { get { return _D; } set { _D = value; } }
        public string E { get { return _E; } set { _E = value; } }
        public string F { get { return _F; } set { _F = value; } }
        public string G { get { return _G; } set { _G = value; } }
        public string H { get { return _H; } set { _H = value; } }
        public string I { get { return _I; } set { _I = value; } }
        public string J { get { return _J; } set { _J = value; } }
        public string K { get { return _K; } set { _K = value; } }
        public string L { get { return _L; } set { _L = value; } }


    }


    [Serializable]
    class TuringMachineConfig
    {

        public int number_of_symbols = 0;
        public int number_of_shifts = 0;
        public int sigma = 0;
        public int number_of_states = 0;
        public int curr_tape_pos = 0;
        public int tape_lenght = 0;
        public string curr_state = "";
        public string name = "";
        public string fill_symbol = "";
        public string blank_symbol = "";
        public string init_state = "";


    }

    class StateCount 
    {
        public int A = 0;
        public int B = 0;
        public int C = 0;
        public int D = 0;
        public int E = 0;
        public int F = 0;
        public int G = 0;
        public int H = 0;
        public int I = 0;
        public int J = 0;
        public int K = 0;
        public int L = 0;
        public int Z = 0;            
    
    }

}
