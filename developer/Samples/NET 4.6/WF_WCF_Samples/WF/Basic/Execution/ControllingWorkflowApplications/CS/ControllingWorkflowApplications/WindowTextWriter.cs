//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.IO;
using System.Text;
using System.Windows.Controls;

namespace Microsoft.Samples.WorkflowApplicationReadLineHost
{

    class WindowTextWriter : TextWriter
    {
        const string textWriterClosed = "This TextWriter is Closed";

        Encoding encoding;
        bool isOpen;
        TextBox textBox;
        StringBuilder stringBuilder;

        public WindowTextWriter(TextBox textBox)
        {
            this.textBox = textBox;
            this.stringBuilder = new StringBuilder();
            this.isOpen = true;
        }

        public override Encoding Encoding
        {
            get
            {
                if (encoding == null)
                {
                    encoding = new UnicodeEncoding(false, false);
                }
                return encoding;
            }
        }

        public override void Close()
        {
            this.Dispose(true);
        }

        protected override void Dispose(bool disposing)
        {
            this.isOpen = false;
            base.Dispose(disposing);
        }

        public override string ToString()
        {
            return this.stringBuilder.ToString();
        }

        public override void Write(char value)
        {
            if (!this.isOpen)
            {
                throw new ApplicationException(textWriterClosed); ;
            }

            this.stringBuilder.Append(value);
            this.textBox.Dispatcher.BeginInvoke(new Action(() => this.textBox.Text = this.stringBuilder.ToString()));
        }

        public override void Write(string value)
        {
            if (!this.isOpen)
            {
                throw new ApplicationException(textWriterClosed); ;
            }
            if (value != null)
            {
                this.stringBuilder.Append(value);
                this.textBox.Dispatcher.BeginInvoke(new Action(() => this.textBox.Text = this.stringBuilder.ToString()));
            }
        }

        public override void Write(char[] buffer, int index, int count)
        {
            if (!this.isOpen)
            {
                throw new ApplicationException(textWriterClosed); ;
            }
            if (buffer == null)
            {
                throw new ArgumentNullException("buffer");
            }
            if (index < 0)
            {
                throw new ArgumentOutOfRangeException("index");
            }
            if (count < 0)
            {
                throw new ArgumentOutOfRangeException("count");
            }
            if ((buffer.Length - index) < count)
            {
                throw new ArgumentException("The size of the buffer cannot accomodate the given index and count.");
            }
            this.stringBuilder.Append(buffer, index, count);
            this.textBox.Dispatcher.BeginInvoke(new Action(() => this.textBox.Text = this.stringBuilder.ToString()));
        }
    }
}
