using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Reflection;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;

/**************************************************************************
 * Turing Machine 
 * (C) 2016 Jonathan Moore
 /**************************************************************************/

namespace TuringMachineSimulator
{
    public partial class frmMain : Form
    {
       
        private TuringMachine TM; 

        private static int FontWidht = 8;
        private static int TapeHeight = 0;
        private static int TapeTop = 0;

        private static int VisibleTapeLenght = 0;

        private PictureBox TapeImage;
        private PictureBox TapeHeadImage;
        private PictureBox TapeStartMarkerImage;

        private void writeConsole(string str) 
        {
            txtConsole.Text = DateTime.Now.ToString() + ": " +  str + "\r\n" + txtConsole.Text;    
        }

        private void DisableInterface() 
        {

            toolStripComboBB.Enabled = false;
            toolStripStart.Enabled = false;
            toolStripStop.Enabled = true;
            toolStripPause.Enabled = true; 
            newToolStripButton.Enabled = false;
            openToolStripButton.Enabled = false;
            saveToolStripButton.Enabled = false;
            helpToolStripButton.Enabled = false;
 
        }

        private void EnableInterface()
        {
            toolStripComboBB.Enabled = true;
            toolStripStart.Enabled = true;
            toolStripStop.Enabled = false;
            toolStripPause.Enabled = false;
            toolStripStep.Enabled = false;
            newToolStripButton.Enabled = true;
            openToolStripButton.Enabled = true;
            saveToolStripButton.Enabled = true;
            helpToolStripButton.Enabled = true; 
        }

        private void CreateEmptyMachine() 
        {           

            if (TM == null)
            {
                TM = new TuringMachine();

                TM.MachineHeadMove  += new MachineHeadMoveEventHandler(TuringMachine_HeadMove);
                TM.MachineHalt += new MachineHaltEventHandler(TuringMachine_Halt);
                TM.MachineProcess += new MachineProcessEventHandler(TuringMachine_Process);
                TM.MachineStart += new MachineStartEventHandler(TuringMachine_Start);
                TM.MachineTapeResize += new MachineTapeResizeEventHandler(TuringMachine_TapeResize);
                TM.MachineException += new MachineExceptionEventHandler(TuringMachine_Exception);

                propertyGrid1.SelectedObject = TM;
            }

            CreateGraphicTapeAndHead();

            txtConsole.BackColor = System.Drawing.Color.FromArgb(255, 224, 192); 
        }
       
        public frmMain()
        {
            InitializeComponent();                        
        }

        private void TuringMachine_Exception(Exception Exp, EventArgs e)
        {

            txtConsole.BackColor = System.Drawing.Color.FromArgb(255, 128, 128);
            writeConsole("EXCEPTION: " + Exp.Message);
            propertyGrid1.SelectedObject = TM; 
            
        }

        private void ResizeScreenTape() 
        {

            VisibleTapeLenght = TM.TapeLength * FontWidht + 10;
            TapeImage.Size = new System.Drawing.Size(VisibleTapeLenght, TapeHeight);           
        
        }

        private void MoveStartMarkerOnScreenTo(int pos)
        {

            TapeStartMarkerImage.Left = ((pos + 1) * FontWidht) - (TapeStartMarkerImage.Width / 2);            
            panelTape.Refresh();
            propertyGrid1.Refresh();
        }

        private void MoveHeadOnScreenTo(int pos) 
        {

            MoveScrollTo(pos);

            TapeHeadImage.Left = ((pos+1) * FontWidht) - (TapeHeadImage.Width/2);            
            panelTape.Refresh();
            propertyGrid1.Refresh();  
        }

        private void MoveScrollTo(int pos)
        {
            panelTape.AutoScrollPosition = new Point( ((pos+1) * FontWidht) - (panelTape.Width/2), 0);
        }        

        private void TuringMachine_TapeResize(object sender, EventArgs e)
        {
            ResizeScreenTape(); 
            panelTape.Refresh(); 
        }

        private void TuringMachine_HeadMove(object sender, EventArgs e) 
        {
            MoveHeadOnScreenTo(TM.CurrentTapePosition);
            SelectStateOnTable();
        }

        private void TuringMachine_Halt(object sender, EventArgs e)
        {

            if (TM.DisableUpdateEvent == true)
            {
                Visible = true;
                ShowInTaskbar = true;                
                notifyIcon1.Visible = false;
            }

            writeConsole("Machine Halted after " + TM.Shifts + " steps");
            propertyGrid1.SelectedObject = TM;
            EnableInterface();
        }

        private void TuringMachine_Process(object sender, EventArgs e)
        {

            SelectStateOnTable();

            propertyGrid1.SelectedObject = TM; 
            panelTape.Refresh();            
            Application.DoEvents();  
        }

        private void SelectStateOnTable() 
        {

            dataGridView1.ClearSelection();
            if (TM.CurrentState != "Z" && TM.CurrentSymbol!="")
            {

                dataGridView1[
                    TM.getStateIndex(TM.CurrentState) + 1,
                    Convert.ToInt16(TM.CurrentSymbol)
                ].Selected = true;
            }        
        
        }

        private void TuringMachine_Start(object sender, EventArgs e)
        {
            dataGridView1.ClearSelection(); 
            if (TM.DisableUpdateEvent == true)
            {
                ShowInTaskbar = false;                
                notifyIcon1.Visible = true;
                Visible = false;
            }
            else
            {

                DisableInterface();

                if (TM.Name == "")
                    TM.Name = "{noname}";

                writeConsole("Execution of " + TM.Name + " started.");
            }
        }

        private Image getImageFromRescource(string s)
        {
            Assembly a = Assembly.GetExecutingAssembly();
            Stream imgStream = a.GetManifestResourceStream(s);
            Bitmap bmp = Bitmap.FromStream(imgStream) as Bitmap;
            return bmp;
        }

        private void frmMain_Load(object sender, EventArgs e)
        {

            splitContainer1.SplitterDistance = 100;

            CreateEmptyMachine();
            toolStripComboBB.SelectedIndex = 0;
            writeConsole("*************************************************************************");
            writeConsole("Turing Machine Simulator Version 2.0 (C) 2016 Jonathan Moore");            
            writeConsole("*************************************************************************");
            writeConsole("");
            writeConsole("Ready - Try to load some sample machine on the toolbar");
            writeConsole("");            

        }

        private void CreateGraphicTapeAndHead() 
        {

            if (panelTape.Controls.Count == 1) 
            {
                panelTape.Controls.RemoveAt(0);
            }

            TapeHeight = 60;
            TapeTop = 20;

            TapeStartMarkerImage = new PictureBox();
            TapeStartMarkerImage.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            TapeStartMarkerImage.Top = 35;
            TapeStartMarkerImage.Image = getImageFromRescource("TMconcept.images.up_arr.gif");
            TapeStartMarkerImage.Size = new Size(TapeStartMarkerImage.Image.Width, TapeStartMarkerImage.Image.Height);

            TapeImage = new PictureBox();
            TapeImage.Top = 20;
            TapeImage.Size = new System.Drawing.Size(VisibleTapeLenght, TapeHeight);
            TapeImage.BackColor = System.Drawing.Color.White;
            TapeImage.BorderStyle = BorderStyle.FixedSingle;
            TapeImage.Paint += new PaintEventHandler(TapeImage_Paint);            

            TapeHeadImage = new PictureBox();
            TapeHeadImage.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            TapeHeadImage.Top = 3;
            TapeHeadImage.Image = getImageFromRescource("TMconcept.images.tape-head.bmp");
            TapeHeadImage.Size = new Size(TapeHeadImage.Image.Width, TapeHeadImage.Image.Height);

            TapeImage.Controls.Add(TapeStartMarkerImage);
            TapeImage.Controls.Add(TapeHeadImage);
            panelTape.Controls.Add(TapeImage);

            MoveHeadOnScreenTo(0);
            MoveStartMarkerOnScreenTo(0);
        
        }

        private void TapeImage_Paint(object sender, PaintEventArgs e)
        {
            if (TM.DisableUpdateEvent == false)
            {
                using (Font Font = new Font("Arial", 12))
                {

                    for (int p = 0; p <= TM.TapeLength - 1; p++)
                    {
                        e.Graphics.DrawString(TM.GetTapeValue(p), Font, Brushes.Black, new Point(FontWidht * p, TapeTop));
                    }

                    //for (int p = ((TM.TapeLength / 2) - 100); p <= ((TM.TapeLength / 2) + 100); p++)
                    //{
                    //    e.Graphics.DrawString(TM.GetTapeValue(p), Font, Brushes.Black, new Point(FontWidht * p, TapeTop));                         
                    //}

                }
            }
        }

        private void propertyGrid1_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {

            PropertyValueChangedEventArgs PropValue = (PropertyValueChangedEventArgs)e;
            System.Windows.Forms.PropertyGrid p = (System.Windows.Forms.PropertyGrid)s;
            TuringMachine tm = (TuringMachine)propertyGrid1.SelectedObject;
            switch (PropValue.ChangedItem.Label)
            {
                case("NumberOfSymbols"):

                    RedefineNumberOfSymbols(tm);
                    break;

                case("NumberOfStates"):

                    //RedefineNumberOfSymbols(tm);
                    RedefineNumberOfStates(tm);                    
                    break;

                default:
                    break;

            };
        }

        private void RedefineNumberOfSymbols(TuringMachine tm) 
        {

            ArrayList list = new ArrayList();
            StateTableRow row;
            for (int c = 0; c < (tm.NumberOfSymbols); c++)
            {
                row = new StateTableRow(
                    c,
                    "P#,R/L,go to S",
                    "P#,R/L,go to S",
                    "P#,R/L,go to S",
                    "P#,R/L,go to S",
                    "P#,R/L,go to S",
                    "",
                    "",
                    "",
                    "",
                    "",
                    "",
                    ""
                    );
                list.Add(row);
            }
            dataGridView1.DataSource = list;        

        }

        private void RedefineNumberOfStates(TuringMachine tm)
        {

            if (tm.NumberOfStates > 12)
                tm.NumberOfStates = 12;

            if (tm.NumberOfStates < 2)
                tm.NumberOfStates = 2;

            for (int c = 0; c < (dataGridView1.ColumnCount); c++)
            {
                if (c > tm.NumberOfStates)
                {
                    dataGridView1.Columns[c].Visible = false;  
                }
                else 
                {
                    dataGridView1.Columns[c].Visible = true;  
                }   
            }

        }

        private void FillTape(string value) 
        {

            
            for (int p = 0; p <= TM.TapeLength-1; p++) 
            {
                TM.SetTapeValue(value, p); 
            }
            this.Refresh();

        }

        private void toolStripStart_Click(object sender, EventArgs e)
        {

            if (TM.Pause == true)
                toolStripStep.Enabled = true;

            PrepareMachine();        
            TM.StartMachine();
        }

        private void CreateBB32()
        {

            TM.NumberOfSymbols = 2;
            TM.NumberOfStates = 3;
            TM.Name = "Busy Beaver 3 states 2 symbols";
            TM.FillSymbol = "0";
            TM.InitialState = "A";

            ArrayList stateTableBB32 = new ArrayList();

            stateTableBB32.Add(new StateTableRow(
                0,
                "P1,R,B",
                "P1,L,A",
                "P1,L,B",
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
                ));

            stateTableBB32.Add(new StateTableRow(
                1,
                "P1,L,C",
                "P1,R,B",
                "P1,R,Z",
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
                ));

            dataGridView1.DataSource = stateTableBB32;
            RedefineNumberOfStates(TM);

        }


        private void CreateBB42() 
        {            

            TM.NumberOfSymbols = 2;
            TM.NumberOfStates = 4;
            TM.Name = "Busy Beaver 4 states 2 symbols";
            TM.FillSymbol = "0";
            TM.InitialState = "A";

            ArrayList stateTableBB42 = new ArrayList();

            stateTableBB42.Add(new StateTableRow(
                0,
                "P1,R,B",
                "P1,L,A",
                "P1,R,Z",
                "P1,R,D",
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
                ));

            stateTableBB42.Add(new StateTableRow(
                1,
                "P1,L,B",
                "P0,L,C",
                "P1,L,D",
                "P0,R,A",
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
                ));

            dataGridView1.DataSource = stateTableBB42;
            RedefineNumberOfStates(TM); 

        }

        private void CreateBB52()
        {            

            TM.NumberOfSymbols = 2;
            TM.NumberOfStates = 5;
            TM.Name = "Busy Beaver 5 states 2 symbols";
            TM.FillSymbol = "0";
            TM.InitialState = "A";

            ArrayList stateTableBB52 = new ArrayList();

            stateTableBB52.Add(new StateTableRow(
                0,
                "P1,R,B",
                "P1,R,C",
                "P1,R,D",
                "P1,L,A",
                "P1,R,Z",
                    "",
                    "",
                    "",
                    "",
                    "",
                    "",
                    ""
                ));

            stateTableBB52.Add(new StateTableRow(
                1,
                "P1,L,C",
                "P1,R,B",
                "P0,L,E",
                "P1,L,D",
                "P0,L,A",
                    "",
                    "",
                    "",
                    "",
                    "",
                    "",
                    ""
                ));

            dataGridView1.DataSource = stateTableBB52;
            RedefineNumberOfStates(TM); 

        }

        private void CreateBB62()
        {

            TM.NumberOfSymbols = 2;
            TM.NumberOfStates = 6;
            TM.Name = "Busy Beaver 6 states 2 symbols";
            TM.FillSymbol = "0";
            TM.InitialState = "A";

            ArrayList stateTableBB62 = new ArrayList();

            stateTableBB62.Add(new StateTableRow(
                0,
                "P1,R,B",
                "P0,R,C",
                "P1,L,D",
                "P0,L,E",
                "P0,R,A",
                "P1,L,A",
                    "",
                    "",
                    "",
                    "",
                    "",
                    ""
                ));

            stateTableBB62.Add(new StateTableRow(
                1,
                "P0,L,F",
                "P0,R,D",
                "P1,R,E",
                "P0,L,D",
                "P1,R,C",
                "P1,R,Z",
                    "",
                    "",
                    "",
                    "",
                    "",
                    ""
                ));

            dataGridView1.DataSource = stateTableBB62;
            // This method reshape the dataGridView to show the correct numbers of columns and rows
            RedefineNumberOfStates(TM);

        }

        private void RunTroughTapeTest() 
        {
        
            FillTape("0");
            TM.Running = true;
            for (int p = (TM.TapeLength / 2); p <= TM.TapeLength - 1; p++) 
            {
                TM.CurrentTapePosition = p;
                MoveHeadOnScreenTo(p);                                
                Application.DoEvents();
                if (TM.Running == false)                    
                    break;

            }
        
        }

        private void toolStripStop_Click(object sender, EventArgs e)
        {
            TM.HaltMachine(); 
        }

        private void DeSerializeTuringMachine(string filename)
        {
            ArrayList objectToSerialize;
            Stream stream = File.Open(filename, FileMode.Open);
            BinaryFormatter bFormatter = new BinaryFormatter();
            objectToSerialize = (ArrayList)bFormatter.Deserialize(stream);
            stream.Close();
            dataGridView1.DataSource = objectToSerialize;

            stream = File.Open(Path.GetDirectoryName(filename) + @"/" + Path.GetFileNameWithoutExtension(filename) + ".cfg", FileMode.Open);
            bFormatter = new BinaryFormatter();
            TuringMachineConfig config = (TuringMachineConfig)bFormatter.Deserialize(stream);
            stream.Close();

            CreateEmptyMachine();

            TM.CurrentState = config.curr_state;
            TM.TapeLength = config.tape_lenght;
            TM.CurrentTapePosition = config.curr_tape_pos;
            TM.NumberOfStates = config.number_of_states;
            TM.NumberOfSymbols = config.number_of_symbols;            
            TM.Name = config.name;
            TM.BlankSymbol = config.blank_symbol;
            TM.FillSymbol = config.fill_symbol;
            TM.InitialState = config.init_state; 

            propertyGrid1.SelectedObject = TM;

        }

        private void SerializeTuringMachine(string filename) 
        {

            TuringMachineConfig config = new TuringMachineConfig();

            config.curr_state = TM.CurrentState;
            config.curr_tape_pos = TM.CurrentTapePosition;
            config.number_of_shifts = TM.Shifts;
            config.number_of_states = TM.NumberOfStates;
            config.number_of_symbols = TM.NumberOfSymbols;
            config.tape_lenght = TM.TapeLength;
            config.sigma = TM.Sigma;
            config.name = TM.Name;
            config.blank_symbol = TM.BlankSymbol;
            config.fill_symbol = TM.FillSymbol;
            config.init_state = TM.InitialState; 

            SerializeObject(filename, (ArrayList)dataGridView1.DataSource);
            SerializeObject(  Path.GetDirectoryName(filename) + @"/" + Path.GetFileNameWithoutExtension(filename) + ".cfg" , config);
        }

        private void SerializeObject(string filename,object obj)
        {

            Stream f;
            BinaryFormatter bf = new BinaryFormatter();
            f = File.OpenWrite(filename);
            bf.Serialize(f, obj);
            f.Close();

        }

        private void saveToolStripButton_Click(object sender, EventArgs e)
        {

            SaveFileDialog saveFileDialog1 = new SaveFileDialog();
            saveFileDialog1.Title = "Save Turing Machine";
            saveFileDialog1.Filter = "Bin Files|*.bin";
            saveFileDialog1.FileName = "";

            saveFileDialog1.ShowDialog();

            if (saveFileDialog1.FileName != "")
            {

                SerializeTuringMachine(saveFileDialog1.FileName);

            }           

        }

        private void openToolStripButton_Click(object sender, EventArgs e)
        {

            OpenFileDialog openFileDialog1 = new OpenFileDialog();
            openFileDialog1.Title = "Load Turing Machine";
            openFileDialog1.Filter = "Bin Files|*.bin";
            openFileDialog1.FileName = "";

            openFileDialog1.ShowDialog();

            if (openFileDialog1.FileName != "") 
            {

               DeSerializeTuringMachine(openFileDialog1.FileName); 

            }
            RedefineNumberOfStates(TM); 
        }

        private void newToolStripButton_Click(object sender, EventArgs e)
        {

            dataGridView1.DataSource = null;
 
            propertyGrid1.SelectedObject = null;
            TM = null;
            CreateEmptyMachine(); 
            this.Refresh();
            toolStripComboBB.SelectedIndex = 0;     

        }

        private void PrepareMachine() 
        {

            if (TM.FillSymbol!="")
                FillTape(TM.FillSymbol);

            TM.CurrentState = TM.InitialState;
            TM.CurrentTapePosition = TM.TapeLength / 2;
            MoveHeadOnScreenTo(TM.CurrentTapePosition);
            MoveStartMarkerOnScreenTo(TM.CurrentTapePosition);
            TM.StateTable = (ArrayList)dataGridView1.DataSource;

            if (TM.StepThrough)
                writeConsole("Starting simulation in Debug Mode"); 

            txtConsole.BackColor = System.Drawing.Color.FromArgb(255, 224, 192);

 
        
        }

        private void LoadSampleMachine() 
        {

            TM = null;
            CreateEmptyMachine(); 

            try
            {                
                switch (toolStripComboBB.SelectedIndex)
                {
                    case 1:
                        TM.TapeLength = 150;
                        CreateBB32();
                        writeConsole("Busy Beaver Machine 3 states 2 symbols loaded");
                        break;
                    case 2:                        
                        TM.TapeLength = 150;
                        CreateBB42();
                        writeConsole("Busy Beaver Machine 4 states 2 symbols loaded");
                        break;
                    case 3:                        
                        TM.TapeLength = 150;
                        CreateBB52();
                        writeConsole("Busy Beaver Machine 5 states 2 symbols loaded");
                        break;
                    case 4:                        
                        TM.TapeLength = 150;
                        CreateBB62();
                        TM.StepThrough = true;
                        writeConsole("Busy Beaver Machine 6 states 2 symbols loaded");
                        break;

                };                
            }
            catch (Exception ex) 
            {
                writeConsole(ex.Message); 
            }
        
        }

        private void helpToolStripButton_Click(object sender, EventArgs e)
        {
            MessageBox.Show("Turing Machine Simulator - (C) 2012 Toni Novak","About Turing Machine",MessageBoxButtons.OK,MessageBoxIcon.Information,MessageBoxDefaultButton.Button1,MessageBoxOptions.ServiceNotification);  
        }

        private void toolStripComboBB_TextChanged(object sender, EventArgs e)
        {
            LoadSampleMachine();
            propertyGrid1.SelectedObject = TM; 
        }

        private void toolStripPause_Click(object sender, EventArgs e)
        {
            TM.Pause=!TM.Pause;

            if (TM.Pause == false)
            {
                toolStripStep.Enabled = false;
                dataGridView1.ClearSelection();
                writeConsole("Resuming execution"); 
                TM.Resume();                
            }else
            {
                writeConsole("Simulation paused. You can go step by step"); 
                toolStripStep.Enabled = true; 
            }
        }

        private void toolStripStep_Click(object sender, EventArgs e)
        {
            if (TM.Pause) 
            {
                TM.RunStep(); 
            }
        }


    }

}
