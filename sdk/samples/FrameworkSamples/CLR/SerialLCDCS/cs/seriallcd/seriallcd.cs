//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//-----------------------------------------------------------------------
using System;
using System.IO;
using System.IO.Ports;
using System.Diagnostics;
using System.Threading;
using System.Collections.Generic;

namespace Microsoft.Samples.SerialTest
{
	public enum LCDKey
	{
		None,
		Up,
		Down,
		Left,
		Right,
		Enter,
		Exit
	}

	public class SerialLCD : IDisposable
	{
		private const int READ_BUFFER_SIZE = 128;
		private const int MAX_DATA_LENGTH = 16;
		private const int MAX_RESPONSE_TIME = 250;
		private const int LINE_LENGTH = 16;
		private const int MAX_PACKET_LENGTH = MAX_DATA_LENGTH + 4;
		private const byte NORMAL_RESPONSE = 0x40;
		private const byte NORMAL_REPORT = 0x80;
		private const byte ERROR_RESPONSE = 0xC0;
		private const byte KEY_ACTIVITY_REPORT = 0x80;
		private const byte FAN_SPEED_REPORT = 0x81;
		private const byte TEMPATURE_SENSOR_REPORT = 0x82;
		ushort CRC_SEED = 0xFFFF;
		private byte[] packetRcvBuffer;
		private byte[] packetXMitBuffer;
		private bool _timedOut;
		private Thread receiveThread;
		private Thread eventThread;
		private LCDPacket responsePacket;
		private Queue<LCDPacket> reportQueue;
		private Object responseSignal;
		private Object reportSignal;
		private LCDKey _key;
		public enum CursorStyle { NoCursor = 0, BlinkingBlockCursor = 1, UnderscoreCursor = 2, BlinkingBlockUnderscoreCursor = 3 };
		[FlagsAttribute]
		public enum Fans { Fan1 = 1, Fan2 = 2, Fan3 = 4, Fan4 = 8 };
		private byte fan1Power, fan2Power, fan3Power, fan4Power;
        	private Boolean disposed = false;
		
		// comPort
		private SerialPort com;

		public SerialLCD()
		{
			// comPort
			com = new SerialPort("COM1", 19200);
			com.ReadTimeout = 3000;
			com.WriteTimeout = 3000;
			com.Open();

			packetRcvBuffer = new byte[MAX_PACKET_LENGTH];
			packetXMitBuffer = new byte[MAX_PACKET_LENGTH];
			fan1Power = fan2Power = fan3Power = fan4Power = (byte)0;
			reportQueue = new Queue<LCDPacket>();
			responseSignal = new Object();
			reportSignal = new Object();
			eventThread = new Thread(new ThreadStart(ReportEventHandler));
			eventThread.Start();
			receiveThread = new Thread(new ThreadStart(Receive));
			receiveThread.Start();
		}

		public bool TimedOut { get { return _timedOut; } set { _timedOut = value; } }
		public byte[] Ping(byte[] data)
		{
			byte type = 0;

			if (16 < data.Length)
			{
				throw new ArgumentException("data", "must have 16 items or less");
			}

			return SendReturnData(type, (byte)data.Length, data);
		}
		public byte[] GetHardwareFirmwareVersion()
		{
			byte type = 1;

			return SendReturnData(type, 0, null);
		}
		public bool WriteUserFlashArea(byte[] data)
		{
			byte type = 2;

			data = CreateByteArrayOfLength(data, 16);
			return SendReturnBool(type, 16, data);
		}
		public byte[] ReadUserFlashArea()
		{
			byte type = 3;

			return SendReturnData(type, 0, null);
		}
		public bool SaveState()
		{
			byte type = 4;

			return SendReturnBool(type, 0, null);
		}
		public bool Clear()
		{
			byte type = 6;

			return SendReturnBool(type, 0, null);
		}
		public bool WriteLine1(string line)
		{
			byte type = 7;

			return WriteLine(type, line);
		}
		public bool WriteLine1(byte[] data)
		{
			byte type = 7;

			data = CreateByteArrayOfLength(data, 16);
			return SendReturnBool(type, 16, data);
		}
		public bool WriteLine2(string line)
		{
			byte type = 8;
			return WriteLine(type, line);
		}
		public bool WriteLine2(byte[] data)
		{
			byte type = 8;

			data = CreateByteArrayOfLength(data, 16);
			return SendReturnBool(type, 16, data);
		}
		private bool WriteLine(byte type, string line)
		{
			line = CreateStringOfLength(line, LINE_LENGTH);
			return SendReturnBool(type, 16, System.Text.Encoding.ASCII.GetBytes(line.ToCharArray()));
		}
		public bool SetCursorPosition(int column, int row)
		{
			byte type = 11;
			byte[] data = new byte[2];

			if (15 < column || 0 > column)
			{
				throw new ArgumentOutOfRangeException("column", "must be 0-15");
			}

			if (1 < column || 0 > column)
			{
				throw new ArgumentOutOfRangeException("row", "must be 0-1");
			}

			data[0] = (byte)column;
			data[1] = (byte)row;
			return SendReturnBool(type, 2, data);
		}
		private bool SetCursorStyle(CursorStyle cursorStyle)
		{
			byte type = 12;
			byte[] data = new byte[1];

			if (0 > (int)cursorStyle || 3 < (int)cursorStyle)
			{
				throw new ArgumentOutOfRangeException("cursorStyle", "must be a valid value in the enum");
			}

			data[0] = (byte)cursorStyle;
			return SendReturnBool(type, 1, data);
		}
		//0-50(0=light 50= very dark)
		public bool SetLCDContrast(int contrastValue)
		{
			byte type = 13;
			byte[] data = new byte[1];

			if (0 > contrastValue || 50 < contrastValue)
			{
				throw new ArgumentOutOfRangeException("contrastValue", "must be 0-50");
			}

			data[0] = (byte)contrastValue;
			return SendReturnBool(type, 1, data);
		}
		//0-100(0= 0ff 100=on)
		public bool SetLCDBacklight(int backlightValue)
		{
			byte type = 14;
			byte[] data = new byte[1];

			if (0 > backlightValue || 100 < backlightValue)
			{
				throw new ArgumentOutOfRangeException("backlightValue", "must be 0-100");
			}

			data[0] = (byte)backlightValue;
			return SendReturnBool(type, 1, data);
		}
		public byte[] GetLastFanPulse()
		{
			byte type = 15;

			return SendReturnData(type, 0, null);
		}
		public bool SetupFanReporting(Fans fans)
		{
			byte type = 16;
			byte[] data = new byte[1];

			if ((int)fans < (int)Fans.Fan1 || (int)(Fans.Fan1 | Fans.Fan2 | Fans.Fan3 | Fans.Fan4) < (int)fans)
			{
				throw new ArgumentOutOfRangeException("fans", "fan value out of range");
			}

			data[0] = (byte)fans;
			return SendReturnBool(type, 1, data);
		}
		public bool SetFan1Power(int fan1)
		{
			return SetFanPower(fan1, fan2Power, fan3Power, fan4Power);
		}
		public bool SetFan2Power(int fan2)
		{
			return SetFanPower(fan1Power, fan2, fan3Power, fan4Power);
		}
		public bool SetFan3Power(int fan3)
		{
			return SetFanPower(fan1Power, fan2Power, fan3, fan4Power);
		}
		public bool SetFan4Power(int fan4)
		{
			return SetFanPower(fan1Power, fan2Power, fan3Power, fan4);
		}
		public bool SetFanPower(int fan1, int fan2, int fan3, int fan4)
		{
			byte type = 17;
			byte[] data = new byte[4];

			if (fan1 < 0 || 100 < fan1)
				throw new ArgumentOutOfRangeException("fan1", "must be 0-100");

			if (fan2 < 0 || 100 < fan2)
				throw new ArgumentOutOfRangeException("fan2", "must be 0-100");

			if (fan3 < 0 || 100 < fan3)
				throw new ArgumentOutOfRangeException("fan3", "must be 0-100");

			if (fan4 < 0 || 100 < fan4)
				throw new ArgumentOutOfRangeException("fan4", "must be 0-100");

			data[0] = fan1Power = (byte)fan1;
			data[1] = fan2Power = (byte)fan2;
			data[2] = fan3Power = (byte)fan3;
			data[3] = fan4Power = (byte)fan4;
			return SendReturnBool(type, 4, data);
		}
		private string CreateStringOfLength(string s, int length)
		{
			if (length < s.Length)
				s = s.Substring(length);
			else if (length > s.Length)
			{
				s = s + (new String(' ', length - s.Length));
			}

			return s;
		}
		private byte[] CreateByteArrayOfLength(byte[] bArray, int length)
		{
			if (length != bArray.Length)
			{
				int dataLength = bArray.Length > length ? length : bArray.Length;
				byte[] newbArray = new byte[length];

				Array.Copy(bArray, newbArray, dataLength);
				bArray = newbArray;
			}

			return bArray;
		}
		private LCDPacket Send(byte type, byte dataLength, byte[] data)
		{
			ushort crc;

			if ((null == data && dataLength != 0) || dataLength > data.Length)
				throw new ArgumentException("bad data sent to Send");

			packetXMitBuffer[0] = type;
			packetXMitBuffer[1] = dataLength;
			if (0 != dataLength)
				Array.Copy(data, 0, packetXMitBuffer, 2, dataLength);

			crc = CRCGenerator.GenerateCRC(packetXMitBuffer, dataLength + 2, CRC_SEED);
			packetXMitBuffer[2 + dataLength + 1] = (byte)(crc >> 8);
			packetXMitBuffer[2 + dataLength] = (byte)crc;
			lock (responseSignal)
			{
				responsePacket = null;
				// comPort
				com.Write(packetXMitBuffer, 0, dataLength + 4);
				if (Monitor.Wait(responseSignal, MAX_RESPONSE_TIME))
				{
					return responsePacket;
				}
			}

			return null;
		}
		private byte[] SendReturnData(byte type, byte dataLength, byte[] data)
		{
			LCDPacket packet = Send(type, dataLength, data);

			if (null != packet)
			{
				return packet.Data;
			}
			else
			{
				return null;
			}
		}
		private bool SendReturnBool(byte type, byte dataLength, byte[] data)
		{
			LCDPacket packet = Send(type, dataLength, data);

			if (null != packet)
			{
				return (type == (packet.Type & 0x0F) && LCDPacket.LCDPacketType.NORMAL_RESPONSE == responsePacket.PacketType);
			}
			else
			{
				return false;
			}
		}
		private void Receive()
		{
			try
			{
				byte[] receiveBuffer = new byte[128];
				int bytesRead = 0;
				int bufferIndex = 0;
				int startPacketIndex = 0;
				int expectedPacketLength = -1;
				bool expectedPacketLengthIsSet = false;
				int numBytesToRead = receiveBuffer.Length;

				while (true)
				{
					if (expectedPacketLengthIsSet || 1 >= bytesRead)
					{
						//If the expectedPacketLength has been or no bytes have been read
						//This covers the case that more then 1 entire packet has been read in at a time
						// comPort
						try
						{
							bytesRead += com.Read(receiveBuffer, bufferIndex, numBytesToRead);
							bufferIndex = startPacketIndex + bytesRead;
						}
						catch (TimeoutException)
						{
							_timedOut = true;
						}
					}

					if (1 < bytesRead)
					{
						//The buffer has the dataLength for the packet
						if (!expectedPacketLengthIsSet)
						{
							//If the expectedPacketLength has not been set for this packet
							expectedPacketLength = receiveBuffer[(1 + startPacketIndex) % receiveBuffer.Length] + 4;
							expectedPacketLengthIsSet = true;
						}

						if (bytesRead >= expectedPacketLength)
						{
							//The buffer has atleast as many bytes for this packet
							AddPacket(receiveBuffer, startPacketIndex);
							expectedPacketLengthIsSet = false;
							if (bytesRead == expectedPacketLength)
							{
								//The buffer contains only the bytes for this packet
								bytesRead = 0;
								bufferIndex = startPacketIndex;
							}
							else
							{
								//The buffer also has bytes for the next packet
								startPacketIndex += expectedPacketLength;
								startPacketIndex %= receiveBuffer.Length;
								bytesRead -= expectedPacketLength;
								bufferIndex = startPacketIndex + bytesRead;
							}
						}
					}

					bufferIndex %= receiveBuffer.Length;
					numBytesToRead = bufferIndex < startPacketIndex ? startPacketIndex - bufferIndex : receiveBuffer.Length - bufferIndex;
				}
			}
			catch (IOException)
			{
				// abort the thread
				System.Threading.Thread.CurrentThread.Abort();
			}
			catch (ObjectDisposedException)
			{
				if (receiveThread != null)
				{
					receiveThread = null;
				}
			}
		}
		private void ReportEventHandler()
		{
			try
			{
				LCDPacket packet = null;

				while (true)
				{
					while (null == packet)
					{
						lock (reportSignal)
						{
							if (0 != reportQueue.Count)
								packet = reportQueue.Dequeue();
							else
								Monitor.Wait(reportSignal);
						}
					}

					switch (packet.Type)
					{
						case KEY_ACTIVITY_REPORT:
							ReportKeyActivityEventHandler(packet);
							break;

						case TEMPATURE_SENSOR_REPORT:
							break;
					}
					packet = null;
				}
			}
			catch (IOException)
			{
				// abort the thread
				System.Threading.Thread.CurrentThread.Abort();
			}
			catch (ObjectDisposedException)
			{
				if (eventThread != null)
				{
					eventThread = null;
				}
			}
		}

		public LCDKey Key
		{
			get
			{
				return _key;
			}
			set
			{
				_key = value;
			}
		}

		private void ReportKeyActivityEventHandler(LCDPacket packet)
		{
			if (1 != packet.DataLength)
			{
				return;
			}

			switch (packet.Data[0])
			{
				case 1 :
					Key = LCDKey.Up;
					break;

				case 2 :
					Key = LCDKey.Down;
					break;

				case 3 :
					Key = LCDKey.Left;
					break;

				case 4 :
					Key = LCDKey.Right;
					break;

				case 5 :
					Key = LCDKey.Enter;
					break;

				case 6 :
					Key = LCDKey.Exit;
					break;

				case 7 :
				case 8 :
				case 9 :
				case 10 :
				case 11 :
				case 12 :
					break;
				default:
					throw new ArgumentException("Invalid packet data");
			}
		}

		private LCDPacket CreatePacket(byte[] buffer, int startIndex)
		{
			byte type = buffer[startIndex];
			byte dataLength = buffer[(startIndex + 1) % buffer.Length];
			byte[] data = new byte[dataLength];
			ushort crc = 0;

			for (int i = 0; i < dataLength; i++)
			{
				data[i] = buffer[(startIndex + 2 + i) % buffer.Length];
			}

			crc |= (ushort)buffer[(startIndex + 2 + dataLength) % buffer.Length];
			crc |= (ushort)(buffer[(startIndex + 2 + dataLength + 1) % buffer.Length] << 8);
			return new LCDPacket(type, dataLength, data, crc);
		}
		private bool AddPacket(byte[] buffer, int startIndex)
		{
			LCDPacket packet = CreatePacket(buffer, startIndex);
			ushort calculatedCRC = CRCGenerator.GenerateCRC(buffer, startIndex, packet.DataLength + 2, CRC_SEED);

			switch (packet.PacketType)
			{
				case LCDPacket.LCDPacketType.NORMAL_RESPONSE :
					AddResponsePacket(packet);
					break;

				case LCDPacket.LCDPacketType.NORMAL_REPORT :
					AddReportPacket(packet);
					break;

				case LCDPacket.LCDPacketType.ERROR_RESPONSE :
					AddResponsePacket(packet);
					break;
			}
			if (calculatedCRC != packet.CRC)
			{
				Console.WriteLine("CRC ERROR!!!: Calculated CRC={0} Actual CRC={1}", Convert.ToString(calculatedCRC, 16), packet.CRC);
				return false;
			}

			return true;
		}

		private void AddResponsePacket(LCDPacket packet)
		{
			lock (responseSignal)
			{
				responsePacket = packet;
				Monitor.Pulse(responseSignal);
			}
		}
		private void AddReportPacket(LCDPacket packet)
		{
			lock (reportSignal)
			{
				reportQueue.Enqueue(packet);
				Monitor.Pulse(reportSignal);
			}
		}

		public bool IsOpen
		{
			get
			{
				if (com == null)
				{
					return false;
				}
				else
				{
					return com.IsOpen;
				}
			}
		}

		public void Reset()
		{
			SetLCDBacklight(100);
			SetLCDContrast(15);
			SetFan1Power(0);
			WriteLine1("Crystalfontz 633");
			WriteLine2("HW v1.2  FW v1.0");
		}

		private void CloseThreads()
		{
			if (receiveThread != null)
			{
				receiveThread.Abort();
			}
			if (eventThread != null)
			{
				eventThread.Abort();
			}
        }
        
        // Public IDisposable.Dispose implementation - calls the internal helper,
        public void Dispose()
        {
            Dispose(true);
        }

        // This is "friendly" wrapper method for IDisposable.Dispose
        public void Close()
        {
            // Call the internal diposal helper
            Dispose(true);
        }

        /*
         * This class wraps a SerialPort instance that is not exposed 
         * directly to the consuming application.  The SerialPort implements 
         * IDisposable, and calling Dispose there causes the SafeHandle
         * given by the OS to the serial port to be released. 
         * 
         * 'disposing' is true if Close or Dispose have been called explicitly,
         * at a point when all managed references are still valid. 
         * Since this class does not directly refer to any system resources, and
         * managed resources can only be cleaned up when the refs are valid, we
         * only clean-up if explicitly called.
         * 
         * Proper usage of this class is to call Close() as soon as the port isn't 
         * needed anymore (as in MainForm.cs). However, if Dispose isn't called here, 
         * then the automatic Dispose call from finalization of the SerialPort 
         * itself will free the OS handle to the serial port.
         * 
         * Note that we would also want to call GCSuppressFinalize (regardless of the
         * value of disposing) if this class were finalizable (i.e. if it had a finalizer).
         * In this case, there are no direct refs to an unmanaged resource, so custom finalization
         * code isn't necessary.
         */
        private void Dispose(bool disposing)
        {
            if (!disposed && disposing && com != null && com.IsOpen)
            {
                Reset();
                com.Close();
                CloseThreads();

                // Keep us from calling resetting or closing multiple times
                disposed = true;
            }
        }
    }

    public class LCDPacket
    {
		private byte type;
		private byte dataLength;
		private byte[] data;
		private ushort crc;
		public enum LCDPacketType { NORMAL_RESPONSE, NORMAL_REPORT, ERROR_RESPONSE };
		private const byte NORMAL_RESPONSE = 0x40;
		private const byte NORMAL_REPORT = 0x80;
		private const byte ERROR_RESPONSE = 0xC0;
		public LCDPacket(byte type, byte dataLength, byte[] data)
		{
			this.type = type;
			this.dataLength = dataLength;
			this.data = data;
		}
		public LCDPacket(byte type, byte dataLength, byte[] data, ushort crc)
		{
			this.type = type;
			this.dataLength = dataLength;
			this.data = data;
			this.crc = crc;
		}
		public override string ToString()
		{
			System.Text.StringBuilder sb = new System.Text.StringBuilder();

			sb.Append("Type: " + Convert.ToString(type, 16) + "\n");
			sb.Append("DataLength: " + Convert.ToString(dataLength, 16) + "\n");
			sb.Append("Data: ");
			for (int i = 0; i < dataLength; i++)
			{
				sb.Append(Convert.ToString(data[i], 16) + ", ");
			}

			sb.Append("\n");
			sb.Append("CRC: " + Convert.ToString(crc, 16) + "\n");
			return sb.ToString();
		}
		public byte Type
		{
			get
			{
				return type;
			}
		}

		public LCDPacketType PacketType
		{
			get
			{
				switch (type & 0xC0)
				{
					case NORMAL_RESPONSE :
						return LCDPacketType.NORMAL_RESPONSE;

					case NORMAL_REPORT :
						return LCDPacketType.NORMAL_REPORT;

					case ERROR_RESPONSE :
						return LCDPacketType.ERROR_RESPONSE;

					default :
						throw new InvalidOperationException("Unexpected Packet Type: " + System.Convert.ToString(type, 16));
				}
			}
		}

		public byte DataLength
		{
			get
			{
				return dataLength;
			}
		}
		public byte[] Data
		{
			get
			{
				return data;
			}
		}
		public ushort CRC
		{
			get
			{
				return crc;
			}
		}
	}

	public static class CRCGenerator
	{
		//CRC lookup table to avoid bit-shifting loops.
		static ushort[] crcLookupTable = {
			0x00000, 0x01189, 0x02312, 0x0329B, 0x04624, 0x057AD, 0x06536, 0x074BF, 0x08C48, 0x09DC1, 0x0AF5A, 0x0BED3, 0x0CA6C, 0x0DBE5, 0x0E97E, 0x0F8F7, 0x01081, 0x00108, 0x03393, 0x0221A, 0x056A5, 0x0472C, 0x075B7, 0x0643E, 0x09CC9, 0x08D40, 0x0BFDB, 0x0AE52, 0x0DAED, 0x0CB64, 0x0F9FF, 0x0E876, 0x02102, 0x0308B, 0x00210, 0x01399, 0x06726, 0x076AF, 0x04434, 0x055BD, 0x0AD4A, 0x0BCC3, 0x08E58, 0x09FD1, 0x0EB6E, 0x0FAE7, 0x0C87C, 0x0D9F5, 0x03183, 0x0200A, 0x01291, 0x00318, 0x077A7, 0x0662E, 0x054B5, 0x0453C, 0x0BDCB, 0x0AC42, 0x09ED9, 0x08F50, 0x0FBEF, 0x0EA66, 0x0D8FD, 0x0C974, 0x04204, 0x0538D, 0x06116, 0x0709F, 0x00420, 0x015A9, 0x02732, 0x036BB, 0x0CE4C, 0x0DFC5, 0x0ED5E, 0x0FCD7, 0x08868, 0x099E1, 0x0AB7A, 0x0BAF3, 0x05285, 0x0430C, 0x07197, 0x0601E, 0x014A1, 0x00528, 0x037B3, 0x0263A, 0x0DECD, 0x0CF44, 0x0FDDF, 0x0EC56, 0x098E9, 0x08960, 0x0BBFB, 0x0AA72, 0x06306, 0x0728F, 0x04014, 0x0519D, 0x02522, 0x034AB, 0x00630, 0x017B9, 0x0EF4E, 0x0FEC7, 0x0CC5C, 0x0DDD5, 0x0A96A, 0x0B8E3, 0x08A78, 0x09BF1, 0x07387, 0x0620E, 0x05095, 0x0411C, 0x035A3, 0x0242A, 0x016B1, 0x00738, 0x0FFCF, 0x0EE46, 0x0DCDD, 0x0CD54, 0x0B9EB, 0x0A862, 0x09AF9, 0x08B70, 0x08408, 0x09581, 0x0A71A, 0x0B693, 0x0C22C, 0x0D3A5, 0x0E13E, 0x0F0B7, 0x00840, 0x019C9, 0x02B52, 0x03ADB, 0x04E64, 0x05FED, 0x06D76, 0x07CFF, 0x09489, 0x08500, 0x0B79B, 0x0A612, 0x0D2AD, 0x0C324, 0x0F1BF, 0x0E036, 0x018C1, 0x00948, 0x03BD3, 0x02A5A, 0x05EE5, 0x04F6C, 0x07DF7, 0x06C7E, 0x0A50A, 0x0B483, 0x08618, 0x09791, 0x0E32E, 0x0F2A7, 0x0C03C, 0x0D1B5, 0x02942, 0x038CB, 0x00A50, 0x01BD9, 0x06F66, 0x07EEF, 0x04C74, 0x05DFD, 0x0B58B, 0x0A402, 0x09699, 0x08710, 0x0F3AF, 0x0E226, 0x0D0BD, 0x0C134, 0x039C3, 0x0284A, 0x01AD1, 0x00B58, 0x07FE7, 0x06E6E, 0x05CF5, 0x04D7C, 0x0C60C, 0x0D785, 0x0E51E, 0x0F497, 0x08028, 0x091A1, 0x0A33A, 0x0B2B3, 0x04A44, 0x05BCD, 0x06956, 0x078DF, 0x00C60, 0x01DE9, 0x02F72, 0x03EFB, 0x0D68D, 0x0C704, 0x0F59F, 0x0E416, 0x090A9, 0x08120, 0x0B3BB, 0x0A232, 0x05AC5, 0x04B4C, 0x079D7, 0x0685E, 0x01CE1, 0x00D68, 0x03FF3, 0x02E7A, 0x0E70E, 0x0F687, 0x0C41C, 0x0D595, 0x0A12A, 0x0B0A3, 0x08238, 0x093B1, 0x06B46, 0x07ACF, 0x04854, 0x059DD, 0x02D62, 0x03CEB, 0x00E70, 0x01FF9, 0x0F78F, 0x0E606, 0x0D49D, 0x0C514, 0x0B1AB, 0x0A022, 0x092B9, 0x08330, 0x07BC7, 0x06A4E, 0x058D5, 0x0495C, 0x03DE3, 0x02C6A, 0x01EF1, 0x00F78
		};

		public static ushort GenerateCRC(byte[] data, int dataLength, ushort seed)
		{
			ushort newCrc;

			newCrc = seed;
			for (int i = 0; i < dataLength; i++)
			{
				newCrc = (ushort)((newCrc >> 8) ^ crcLookupTable[(newCrc ^ data[i]) & 0xff]);
			}

			return ((ushort)~newCrc);
		}

		public static ushort GenerateCRC(byte[] data, int startIndex, int length, ushort seed)
		{
			ushort newCrc;

			newCrc = seed;
			for (int i = 0; i < length; i++)
			{
				newCrc = (ushort)((newCrc >> 8) ^ crcLookupTable[(newCrc ^ data[(startIndex + i) % data.Length]) & 0xff]);
			}

			return ((ushort)~newCrc);
		}
	}
}