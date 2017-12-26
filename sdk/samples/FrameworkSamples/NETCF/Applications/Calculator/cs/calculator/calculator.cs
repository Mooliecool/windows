//=====================================================================
//  File:      Calculator.cs
//
//  Summary:   Simple calculator
//
//  ---------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
//
//  Copyright (C) Microsoft Corporation.  All rights reserved.
//
//  This source code is intended only as a supplement to Microsoft
//  Development Tools and/or on-line documentation.  See these other
//  materials for detailed information regarding Microsoft code samples.
//
//  THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//  PARTICULAR PURPOSE.
//=====================================================================

using System;
using System.Drawing;
using System.Windows.Forms;
using System.Collections;
using System.Collections.Generic;

namespace Microsoft.Samples.CompactFramework
{
    // Calculator window
    public class Window : System.Windows.Forms.Form
    {
        private Button[] buttons;
        private int buttonCount;
        private Button capturedButton;

        private String[] buttonCaptions =
	    {
		    "M+",	"MR",	"MC",	"1/x",	"/",
		    "+/-",	"7",	"8",	"9",	"x",
		    "%",	"4",	"5",	"6",	"-",
		    "CE",	"1",	"2",	"3",	"+",
		    "C",	"0",	".",	"=",
	    };

        private Color[] buttonColors =
        {
	        Color.DarkRed,  Color.DarkRed,  Color.DarkRed,  Color.DarkBlue,
            Color.DarkRed,  Color.DarkBlue, Color.DarkBlue, Color.DarkBlue,
            Color.DarkBlue, Color.DarkRed,  Color.DarkBlue, Color.DarkBlue,
            Color.DarkBlue, Color.DarkBlue, Color.DarkRed,  Color.DarkRed,
            Color.DarkBlue, Color.DarkBlue, Color.DarkBlue, Color.DarkRed,
	        Color.DarkRed,  Color.DarkBlue, Color.DarkBlue, Color.DarkRed
        };

        public enum Command
        {
            MemorySet = 0, 
            MemoryRecall, 
            MemoryClear, 
            OneOver, 
            Div, 
            Minus, 
            Seven, 
            Eight, 
            Nine, 
            Multiply,
            Percent, 
            Four, 
            Five, 
            Six, 
            Sub, 
            ClearEntry, 
            One, 
            Two, 
            Three, 
            Add, 
            ClearAll, 
            Zero, 
            Dot, 
            Equal, 
            Max
        };

        private int windowWidth;
        private int windowHeight;
        private int SizeMargin;
        private int buttonWidth;
        private int buttonHeight;
        private int buttonTopRow;
        private Edit editBox;
        private Calculator calc;
        private Font windowFont;

        public Window()
        {
            int x, y;
            int editX;
            int editY;
            int editWidth;
            int editHeight;
            int row, col;

            calc = new Calculator();

            windowWidth = 240;
            windowHeight = 320 - 20;

            windowFont = new Font(
                FontFamily.GenericSansSerif, 
                12, 
                FontStyle.Bold);

            this.ClientSize = new Size(windowWidth, windowHeight);
            this.MaximizeBox = false;

            // Calculate button size based on window size (button matrix is 5x5)

            buttonWidth = windowWidth / 5;
            buttonHeight = windowHeight / 6;

            SizeMargin = Math.Min(buttonWidth / 8, buttonHeight / 8);

            buttonWidth -= SizeMargin * 2;
            buttonHeight -= SizeMargin * 2;

            // Calculate edit size

            editX = SizeMargin;
            editY = SizeMargin;
            editWidth = windowWidth - (SizeMargin * 2);
            editHeight = buttonHeight;

            // Create buttons

            buttonTopRow = SizeMargin + editHeight + SizeMargin;

            buttons = new Button[(int)Command.Max];

            y = buttonTopRow;

            for (row = 0; row < 5; row++)
            {
                x = SizeMargin;

                for (col = 0; col < 5; col++)
                {
                    if (buttonCount < (int)Command.Max)
                    {
                        buttons[buttonCount] = new Button(
                            this, 
                            windowFont, 
                            x, 
                            y, 
                            buttonWidth, 
                            buttonHeight, 
                            SizeMargin, 
                            buttonCaptions[buttonCount], 
                            buttonColors[buttonCount], 
                            (Window.Command)buttonCount);

                        buttonCount++;
                    }

                    x += SizeMargin + buttonWidth + SizeMargin;
                }

                y += SizeMargin + buttonHeight + SizeMargin;
            }

            // Adjust + button

            buttons[(int)Command.Add].IsTall = true;

            // Create edit

            editBox = new Edit(this, windowFont, new Rectangle(editX, editY, editWidth, editHeight));
            editBox.EditString = calc.Render();

            this.BackColor = Color.SlateGray;
            this.Text = "Calculator";
        }

        ~Window()
        {
            windowFont.Dispose();
        }

        private void DoCommand(Command cmd)
        {
            switch (cmd)
            {
                case Command.MemorySet:
                    calc.DoMemorySet();
                    break;

                case Command.MemoryClear:
                    calc.DoMemoryClear();
                    break;

                case Command.MemoryRecall:
                    calc.DoMemoryRecall();
                    break;

                case Command.ClearAll:
                    calc.DoClearAll();
                    break;

                case Command.ClearEntry:
                    calc.DoClearCurrentToken();
                    break;

                case Command.Percent:
                    calc.DoPercent();
                    break;

                case Command.OneOver:
                    calc.DoOneOver();
                    break;

                case Command.Sub:
                    calc.DoOperator(Token.TokenType.Subtract);
                    break;

                case Command.Add:
                    calc.DoOperator(Token.TokenType.Add);
                    break;

                case Command.Div:
                    calc.DoOperator(Token.TokenType.Divide);
                    break;

                case Command.Multiply:
                    calc.DoOperator(Token.TokenType.Multiply);
                    break;

                case Command.Minus:
                    calc.DoNegative();
                    break;

                case Command.Dot:
                    calc.DoDecimal();
                    break;

                case Command.Zero:
                    calc.DoDigit(0);
                    break;

                case Command.One:
                    calc.DoDigit(1);
                    break;

                case Command.Two:
                    calc.DoDigit(2);
                    break;

                case Command.Three:
                    calc.DoDigit(3);
                    break;

                case Command.Four:
                    calc.DoDigit(4);
                    break;

                case Command.Five:
                    calc.DoDigit(5);
                    break;

                case Command.Six:
                    calc.DoDigit(6);
                    break;

                case Command.Seven:
                    calc.DoDigit(7);
                    break;

                case Command.Eight:
                    calc.DoDigit(8);
                    break;

                case Command.Nine:
                    calc.DoDigit(9);
                    break;

                case Command.Equal:
                    calc.DoEvaluate();
                    break;
            }

            editBox.EditString = calc.Render();
        }

        protected override void OnPaint(PaintEventArgs paintArgs)
        {
            Graphics graphics;

            graphics = paintArgs.Graphics;

            // Edit line

            editBox.Render(graphics);

            // Buttons

            foreach (Button button in buttons)
            {
                button.Render(graphics);
            }
        }

        protected override void OnPaintBackground(PaintEventArgs paintArgs)
        {
            base.OnPaintBackground(paintArgs);
        }

        protected override void OnMouseDown(MouseEventArgs mouseArgs)
        {
            foreach (Button button in buttons)
            {
                if (button.IsHit(mouseArgs.X, mouseArgs.Y))
                {
                    button.IsSelected = true;
                    capturedButton = button;

                    break;
                }
            }
        }

        protected override void OnMouseMove(MouseEventArgs mouseArgs)
        {
            if (capturedButton != null)
            {
                capturedButton.IsSelected = capturedButton.IsHit(mouseArgs.X, mouseArgs.Y);
            }
        }

        protected override void OnMouseUp(MouseEventArgs mouseArgs)
        {
            if (capturedButton != null)
            {
                if (capturedButton.IsHit(mouseArgs.X, mouseArgs.Y))
                    DoCommand(capturedButton.Cmd);

                capturedButton.IsSelected = false;
                capturedButton = null;
            }
        }

        protected override void OnKeyPress(KeyPressEventArgs keyArgs)
        {
            switch (keyArgs.KeyChar)
            {
                case '1':
                    this.DoCommand(Command.One);
                    break;

                case '2':
                    this.DoCommand(Command.Two);
                    break;

                case '3':
                    this.DoCommand(Command.Three);
                    break;

                case '4':
                    this.DoCommand(Command.Four);
                    break;

                case '5':
                    this.DoCommand(Command.Five);
                    break;

                case '6':
                    this.DoCommand(Command.Six);
                    break;

                case '7':
                    this.DoCommand(Command.Seven);
                    break;

                case '8':
                    this.DoCommand(Command.Eight);
                    break;

                case '9':
                    this.DoCommand(Command.Nine);
                    break;

                case '0':
                    this.DoCommand(Command.Zero);
                    break;

                case (char)(int)Keys.Back:
                    this.DoCommand(Command.ClearEntry);
                    break;

                case '.':
                    this.DoCommand(Command.Dot);
                    break;

                case '+':
                    this.DoCommand(Command.Add);
                    break;

                case '-':
                    this.DoCommand(Command.Sub);
                    break;

                case '*':
                    this.DoCommand(Command.Multiply);
                    break;

                case '/':
                    this.DoCommand(Command.Div);
                    break;

                case '=':
                case (char)13:
                    this.DoCommand(Command.Equal);
                    break;
            }
        }

        public static int Main()
        {
            Window window;

            window = new Window();
            Application.Run(window);

            return (0);
        }
    }

    // 
    // The following structure describes a number and the operations 
    // that can be performed on it. Its value is stored internally 
    // as a double.
    //
    public struct Number
    {
        double numValue;

        public Number(double n)
        {
            numValue = n;
        }

        public static Number Add(Number a, Number b)
        {
            return (new Number(a.numValue + b.numValue));
        }

        public static Number Subtract(Number a, Number b)
        {
            return (new Number(a.numValue - b.numValue));
        }

        public static Number Multiply(Number a, Number b)
        {
            return (new Number(a.numValue * b.numValue));
        }

        public static Number Divide(Number a, Number b)
        {
            if (b.numValue == 0)
                return (new Number(0));
            else
                return (new Number(a.numValue / b.numValue));
        }

        public static bool operator ==(Number a, Number b)
        {
            return (a.numValue == b.numValue);
        }

        public static bool operator !=(Number a, Number b)
        {
            return (a.numValue != b.numValue);
        }

        public override bool Equals(Object b)
        {
            if (b is Number)
                return (this.numValue == ((Number)b).numValue);
            else
                return (false);
        }

        public override int GetHashCode()
        {
            return ((int)numValue);
        }

        public override String ToString()
        {
            return (numValue.ToString());
        }
    }

    // 
    // The following class implements a button control for the calculator
    //
    public class Button
    {
        private Form MainForm;
        private int PositionLeftX;
        private int PositionTopY;
        private int SizeWidth;
        private int SizeHeight;
        private int SizeMargin;
        private String CaptionName;
        private Color CaptionColor;
        private Font CaptionFont;
        private bool IsTallValue;
        private bool IsSelectedValue;
        private Window.Command ButtonCommand;

        public Button(Form form, Font font, int x, int y, int width, int height,
            int margin, String capString, Color capColor, Window.Command cmd)
        {
            MainForm = form;
            CaptionFont = font;
            PositionLeftX = x;
            PositionTopY = y;
            SizeWidth = width;
            SizeHeight = height;
            SizeMargin = margin;
            CaptionName = capString;
            CaptionColor = capColor;
            ButtonCommand = cmd;
        }

        public void Render(Graphics graphics)
        {
            Pen pen;
            Brush brush;
            int x, y;
            int textWidth, textHeight;

            brush = new SolidBrush(IsSelectedValue ?
                CaptionColor : Color.White);
            pen = new Pen(Color.Black);

            graphics.FillEllipse(brush, PositionLeftX, PositionTopY,
                SizeWidth, SizeHeight);
            graphics.DrawEllipse(pen, PositionLeftX, PositionTopY,
                SizeWidth, SizeHeight);

            textWidth = (int)graphics.MeasureString(CaptionName,
                            CaptionFont).Width;
            textHeight = (int)graphics.MeasureString(CaptionName,
                            CaptionFont).Height;

            x = PositionLeftX + (SizeWidth - textWidth) / 2;
            y = PositionTopY + (SizeHeight - textHeight) / 2;
            graphics.DrawString(CaptionName, CaptionFont,
                new SolidBrush(IsSelectedValue ? Color.White : CaptionColor),
                x, y);
            brush.Dispose();
        }

        public bool IsHit(int x, int y)
        {
            return (x >= PositionLeftX &&
                    x < PositionLeftX + SizeWidth &&
                    y >= PositionTopY &&
                    y < PositionTopY + SizeHeight);
        }

        public bool IsTall
        {
            get
            {
                return IsTallValue;
            }
            set
            {
                IsTallValue = value;
                if (value) SizeHeight = (SizeHeight * 2 + SizeMargin * 2);
            }
        }

        public bool IsSelected
        {
            get
            {
                return IsSelectedValue;
            }
            set
            {
                Graphics graphics;

                if (value != IsSelectedValue)
                {
                    IsSelectedValue = value;

                    // Redraw right away
                    graphics = MainForm.CreateGraphics();
                    this.Render(graphics);
                    graphics.Dispose();

                }
            }
        }

        public Window.Command Cmd
        {
            get
            {
                return (ButtonCommand);
            }
        }
    }

    //
    // The following class implements an Edit control.
    //
    public class Edit
    {
        private Form MainForm;
        private Rectangle AreaBounds;
        private String EditStringValue;
        private Font EditFont;

        public Edit(Form form, Font font, Rectangle rcBounds)
        {
            MainForm = form;
            EditFont = font;
            AreaBounds = rcBounds;
        }

        public void Render(Graphics graphics)
        {
            String str;
            int x, y;
            int textWidth, textHeight;
            SolidBrush brush = new SolidBrush(Color.Black);

            brush.Color = Color.White;
            graphics.FillRectangle(brush, AreaBounds);
            graphics.DrawRectangle(new Pen(Color.Black), AreaBounds);

            str = EditStringValue;
            textWidth = (int)graphics.MeasureString(str, EditFont).Width;
            textHeight = (int)graphics.MeasureString(str, EditFont).Height;

            x = AreaBounds.Left + AreaBounds.Width - textWidth;
            y = AreaBounds.Top + (AreaBounds.Height - textHeight) / 2;

            graphics.Clip = new Region(AreaBounds);
            brush.Color = Color.Black;
            graphics.DrawString(str, EditFont, brush,
                                x, y);
            graphics.ResetClip();
            brush.Dispose();
        }

        public String EditString
        {
            get
            {
                return (EditStringValue);
            }

            set
            {
                Graphics graphics;
                EditStringValue = value;

                // Redraw right away
                graphics = MainForm.CreateGraphics();
                this.Render(graphics);
                graphics.Dispose();
            }
        }
    }

    // The following class describes a mathematical operation token.
    // 
    //
    public class Token
    {
        private Token.TokenType TypeValue;
        private Number TokenNumberValue;
        private int DecimalFactorValue;
        private bool IsSealedValue;
        static private char[] Symbols = { '+', '-', 'x', '/' };

        public enum TokenType
        {
            Nil = -1,
            Add = 0,
            Subtract,
            Multiply,
            Divide,
            TokenNumber
        };

        public Token(Token.TokenType type)
        {
            DecimalFactor = 0;
            this.Type = type;
        }

        public Token.TokenType Type
        {
            get
            {
                return (TypeValue);
            }
            set
            {
                TypeValue = value;
            }
        }

        public Number TokenNumber
        {
            get
            {
                return (TokenNumberValue);
            }
            set
            {
                TokenNumberValue = value;
            }
        }

        public int DecimalFactor
        {
            get
            {
                return (DecimalFactorValue);
            }
            set
            {
                DecimalFactorValue = value;
            }
        }

        public bool IsSealed
        {
            get
            {
                return (IsSealedValue);
            }

            set
            {
                IsSealedValue = value;
            }
        }

        public bool IsOperator()
        {
            return (this.Type >= Token.TokenType.Add &&
                    this.Type <= Token.TokenType.Divide);
        }

        public bool IsNumber()
        {
            return (this.Type == Token.TokenType.TokenNumber);
        }

        public bool IsLessThanOrEqualTo(Token tokenCompare)
        {
            return (this.Type <= tokenCompare.Type);
        }

        public override String ToString()
        {
            String resultString;

            if (IsOperator())
                resultString = new String(Symbols[(int)this.Type], 1);
            else
                resultString = TokenNumberValue.ToString();

            return (resultString);
        }
    }

    // TODO: Add description for class Calculator
    //
    public class Calculator
    {
        private List<Token> TokenList = new List<Token>();
        private Token MemoryToken = new Token(Token.TokenType.TokenNumber);

        public Calculator()
        {
            Reset();
        }

        private void Reset()
        {
            TokenList.Clear();
            AddNumberToken(new Number(0));
        }

        private void AddNumberToken(Number number)
        {
            Token tok;
            tok = new Token(Token.TokenType.TokenNumber);
            tok.TokenNumber = number;
            TokenList.Add(tok);
        }

        private void AddOperatorToken(Token.TokenType type)
        {
            Token tok;
            tok = new Token(type);
            TokenList.Add(tok);
        }

        private void RemoveCurrentToken()
        {
            if (TokenList != null && TokenList.Count > 0)
            {
                TokenList.RemoveAt(TokenList.Count - 1);
            }
        }

        private Token FetchToken()
        {
            if (TokenList != null && TokenList.Count > 0)
            {
                Token tok;
                tok = TokenList[0];
                TokenList.RemoveAt(0);
                return (tok);
            }
            else
            {
                return (new Token(Token.TokenType.Nil));
            }
        }


        private Token CurrentToken()
        {
            if (TokenList != null && TokenList.Count > 0)
                return (TokenList[TokenList.Count - 1]);
            else
                return (new Token(Token.TokenType.Nil));
        }

        public void DoMemorySet()
        {
            if (CurrentToken().IsNumber())
            {
                MemoryToken.TokenNumber = CurrentToken().TokenNumber;
            }
        }

        public void DoMemoryClear()
        {
            MemoryToken.Type = Token.TokenType.Nil;
        }

        public void DoMemoryRecall()
        {
            if (MemoryToken.IsNumber())
            {
                if (CurrentToken().IsNumber())
                    RemoveCurrentToken();

                AddNumberToken(MemoryToken.TokenNumber);
                CurrentToken().IsSealed = true;
            }
        }

        public void DoDecimal()
        {
            if (!CurrentToken().IsNumber())
                AddNumberToken(new Number(0));

            if (CurrentToken().DecimalFactor == 0)
                CurrentToken().DecimalFactor = 10;
        }

        public void DoDigit(int n)
        {
            Token tok;

            if (CurrentToken().Type == Token.TokenType.TokenNumber &&
                CurrentToken().IsSealed)
            {
                RemoveCurrentToken();
                AddNumberToken(new Number(0));
            }

            if (CurrentToken().Type == Token.TokenType.TokenNumber)
            {
                tok = CurrentToken();

                if (tok.DecimalFactor == 0)
                {
                    tok.TokenNumber = Number.Add(
                        Number.Multiply(
                            tok.TokenNumber,
                            new Number(10)
                        ),
                        new Number(n)
                    );
                }
                else
                {
                    tok.TokenNumber = Number.Add(
                        tok.TokenNumber,
                        Number.Divide(
                            new Number(n),
                            new Number(tok.DecimalFactor)
                        )
                    );
                    tok.DecimalFactor *= 10;
                }
            }
            else
            {
                AddNumberToken(new Number(n));
            }
        }

        public void DoOperator(Token.TokenType type)
        {
            if (CurrentToken().IsOperator())
            {
                RemoveCurrentToken();
            }
            else if (CurrentToken().IsNumber())
            {
                CurrentToken().IsSealed = true;
            }

            AddOperatorToken(type);
        }

        public void DoNegative()
        {
            if (CurrentToken().IsNumber())
            {
                CurrentToken().TokenNumber = Number.Multiply(
                    CurrentToken().TokenNumber, new Number(-1));
            }
        }

        public void DoOneOver()
        {
            if (CurrentToken().IsNumber())
            {
                if (CurrentToken().TokenNumber == new Number(0))
                    CurrentToken().TokenNumber = new Number(0);
                else
                    CurrentToken().TokenNumber = Number.Divide(
                        new Number(1), CurrentToken().TokenNumber);
            }
        }

        public void DoClearAll()
        {
            Reset();
        }

        public void DoClearCurrentToken()
        {
            if (CurrentToken().IsNumber())
            {
                if ((CurrentToken().TokenNumber == new Number(0)) &&
                    (TokenList.Count > 1))
                    RemoveCurrentToken();
                else
                {
                    RemoveCurrentToken();
                    AddNumberToken(new Number(0));
                }
            }
            else if (CurrentToken().IsOperator())
            {
                RemoveCurrentToken();
            }
        }

        public void DoPercent()
        {
            if (CurrentToken().IsNumber())
            {
                CurrentToken().TokenNumber = Number.Divide(
                    CurrentToken().TokenNumber, new Number(100));
            }
        }

        private static Token TokenEvalBinOp(Token tokOp, Token aToken, Token bToken)
        {
            Token result;

            result = new Token(Token.TokenType.TokenNumber);

            switch (tokOp.Type)
            {
                case Token.TokenType.Add:
                    result.TokenNumber = Number.Add(
                        aToken.TokenNumber, bToken.TokenNumber);
                    break;

                case Token.TokenType.Subtract:
                    result.TokenNumber = Number.Subtract(
                        aToken.TokenNumber, bToken.TokenNumber);
                    break;

                case Token.TokenType.Multiply:
                    result.TokenNumber = Number.Multiply(
                        aToken.TokenNumber, bToken.TokenNumber);
                    break;

                case Token.TokenType.Divide:
                    result.TokenNumber = Number.Divide(
                        aToken.TokenNumber, bToken.TokenNumber);
                    break;
            }

            return (result);
        }

        private static void DoBinaryEval(Stack<Token> operatorStack, Stack<Token> numberStack)
        {
            Token topOperatorToken;
            Token aToken, bToken;

            topOperatorToken = operatorStack.Pop();

            bToken = numberStack.Pop();
            aToken = numberStack.Pop();

            numberStack.Push(
                TokenEvalBinOp(topOperatorToken, aToken, bToken));
        }

        public void DoEvaluate()
        {
            Stack<Token> operatorStack = new Stack<Token>();
            Stack<Token> numberStack = new Stack<Token>();
            Token currentToken, topOperatorTok;

            if (CurrentToken().IsOperator())
            {
                RemoveCurrentToken();
            }

            // Eval

            while ((currentToken = FetchToken()).Type != Token.TokenType.Nil)
            {
                if (currentToken.IsNumber())
                {
                    numberStack.Push(currentToken);
                }
                else if (currentToken.IsOperator())
                {
                    if (operatorStack.Count > 0)
                    {
                        topOperatorTok = operatorStack.Peek();

                        if (currentToken.IsLessThanOrEqualTo(topOperatorTok))
                        {
                            DoBinaryEval(operatorStack, numberStack);
                        }
                    }

                    operatorStack.Push(currentToken);
                }
            }

            // Empty the stack

            while (operatorStack.Count > 0)
            {
                DoBinaryEval(operatorStack, numberStack);
            }

            // Update token list

            currentToken = numberStack.Pop();
            Reset();
            RemoveCurrentToken();
            AddNumberToken(currentToken.TokenNumber);
            CurrentToken().IsSealed = true;

            // We're done
        }

        public String Render()
        {
            String resultString = "";

            foreach (Token tok in TokenList)
                resultString += " " + tok.ToString() + " ";

            return (resultString);
        }
    }
}