/****************************** Module Header ******************************\
* Module Name:    UploadStatus.cs
* Project:        CSASPNETFileUploadStatus
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to display the upload status and progress without
* a third part component like ActiveX control, Flash or Silverlight.
* 
* We use this class to store upload progress status.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Web;
using System.Web.Caching;
using System.Threading;
using System.Runtime.Serialization;
using System.Web.Script.Serialization;
using System.IO;

namespace CSASPNETFileUploadStatus
{
    #region ////////////delegate for events////////////
    // we define a EventHandler delegate to hold the events int the UploadStatus.
    public delegate void UploadStatusEventHandler(
                        object sender, UploadStatusEventArgs e);

    public class UploadStatusEventArgs : EventArgs
    {
        [ScriptIgnore]
        public HttpContext context
        {
            get;
            protected set;
        }
        public UploadStatusEventArgs(HttpContext ctx)
        {
            context = ctx;
        }
    }
    #endregion

    [Serializable]
    public class UploadStatus
    {
        #region ////////////Private variables////////////

        private enum DataUnit
        {
            Byte = 1,
            KB = 1024,
            MB = 1048576,
            GB = 1073741824
        }
        private enum TimeUnit
        {
            Seconds = 1,
            Minutes = 60,
            Hours = 3600,
            Day = 86400
        }

        //return the unit of the Uploaded data.
        private DataUnit LoadedUnit
        {
            get
            {
                return GetDataUnit(LoadedLength);
            }
        }
        //return the unit of the whole content data.
        private DataUnit ContentUnit
        {
            get
            {
                return GetDataUnit(ContentLength);
            }
        }

        //the context of the request
        [NonSerialized]
        private HttpContext Context;

        #endregion

        #region ////////////Public Properties////////////

        /// <summary>
        /// If the user abort the upload, then it will return true.
        /// </summary>
        public bool Aborted
        {
            get;
            private set;
        }

        /// <summary>
        /// get the content length of the file
        /// </summary>
        public long ContentLength
        {
            get;
            private set;
        }
        /// <summary>
        /// get the content length of the file which is formatted with unit
        /// </summary>
        public string ContentLengthString
        {
            get
            {
                decimal rslWithUnit = (decimal)ContentLength / (int)ContentUnit;
                return rslWithUnit.ToString("0.00") + " " + ContentUnit.ToString();
            }
        }
        /// <summary>
        /// get the uploaded content length of the file
        /// </summary>
        public long LoadedLength
        {
            get;
            private set;
        }
        /// <summary>
        /// get the uploaded content length of the file 
        /// which is formatted with unit
        /// </summary>
        public string LoadedLengthString
        {
            get
            {
                decimal rslWithUnit = (decimal)LoadedLength / (int)LoadedUnit;
                return rslWithUnit.ToString("0.00") + " " + LoadedUnit.ToString();
            }
        }
        /// <summary>
        /// get the time when the uploading started.
        /// </summary>
        public DateTime StartTime
        {
            get;
            private set;
        }
        /// <summary>
        /// get the time when the uploading finished or aborted.
        /// </summary>
        public DateTime EndTime
        {
            get;
            private set;
        }
        /// <summary>
        /// get whether the uploading finished.
        /// </summary>
        public bool IsFinished
        {
            get;
            private set;
        }
        /// <summary>
        /// get the persentage of the uploaded content
        /// </summary>
        public int LoadedPersentage
        {
            get
            {
                int percent = Convert.ToInt32(
                        Math.Ceiling(
                            (decimal)LoadedLength / (decimal)ContentLength
                            * 100
                        )
                    );
                return percent;
            }
        }
        /// <summary>
        /// get the spent time of the uploading
        /// the unit is seconds
        /// </summary>
        public double SpendTimeSeconds
        {
            get
            {
                DateTime calcTime = DateTime.Now;
                if (IsFinished || Aborted)
                {
                    calcTime = EndTime;
                }
                double spendtime = Math.Ceiling(calcTime.Subtract(StartTime).TotalSeconds);
                if (spendtime == 0 && IsFinished)
                {
                    spendtime = 1;
                }
                return spendtime;
            }
        }
        /// <summary>
        /// get the spent time of the uploading which is formatted with unit
        /// </summary>
        public string SpendTimeString
        {
            get
            {
                double spent = SpendTimeSeconds;
                TimeUnit unit = GetTimeUnit(spent);
                double unitTime = spent / (int)GetTimeUnit(spent);
                return unitTime.ToString("0.0") + " " + unit.ToString();
            }
        }

        /// <summary>
        /// get the upload speed
        /// the unit is bytes/second
        /// </summary>
        public double UploadSpeed
        {
            get
            {
                double spendtime = SpendTimeSeconds;
                double speed = (double)LoadedLength / spendtime;
                return speed;
            }
        }
        /// <summary>
        /// get the upload speed which is formatted with unit.
        /// </summary>
        public string UploadSpeedString
        {
            get
            {
                double spendtime = SpendTimeSeconds;
                DataUnit unit = GetDataUnit((long)Math.Ceiling((double)LoadedLength / spendtime));
                double speed = UploadSpeed / (int)unit;
                return speed.ToString("0.0") + " " + unit.ToString() + "/seconds";
            }
        }
        /// <summary>
        /// get the left time
        /// the unit is seconds
        /// </summary>
        public double LeftTimeSeconds
        {
            get
            {
                double remain = Math.Floor((ContentLength - LoadedLength) / UploadSpeed);
                return remain;
            }
        }
        /// <summary>
        /// get the left time which is formatted with unit
        /// </summary>
        public string LeftTimeString
        {
            get
            {
                double remain = LeftTimeSeconds;
                TimeUnit unit = GetTimeUnit(remain);
                double newRemain = remain / (int)unit;
                return newRemain.ToString("0.0") + " " + unit.ToString();
            }
        }

        #endregion

        #region ////////////Events////////////
        public event UploadStatusEventHandler OnDataChanged;
        public event UploadStatusEventHandler OnFinish;
        #endregion

        #region ////////////Constructor////////////
        public UploadStatus()
        {
        }
        public UploadStatus(HttpContext ctx, long length)
        {
            Aborted = false;
            IsFinished = false;
            StartTime = DateTime.Now;
            Context = ctx;
            ContentLength = length;
            UpdateLoadedLength(0);
        }
        #endregion

        #region ////////////Methods////////////
        private TimeUnit GetTimeUnit(double seconds)
        {
            if (seconds > (int)TimeUnit.Day)
            {
                return TimeUnit.Day;
            }
            if (seconds > (int)TimeUnit.Hours)
            {
                return TimeUnit.Hours;
            }
            if (seconds > (int)TimeUnit.Minutes)
            {
                return TimeUnit.Minutes;
            }
            return TimeUnit.Seconds;

        }
        private DataUnit GetDataUnit(long length)
        {
            if (length > Math.Pow(2D, 30))
            {
                return DataUnit.GB;
            }
            if (length > Math.Pow(2D, 20))
            {
                return DataUnit.MB;
            }
            if (length > Math.Pow(2D, 10))
            {
                return DataUnit.KB;
            }
            return DataUnit.Byte;
        }

        private void changeFinish()
        {
            if (OnFinish != null)
            {
                OnFinish(this, new UploadStatusEventArgs(Context));
            }
        }
        private void changeData()
        {
            if (Aborted)
            {
                return;
            }
            if (OnDataChanged != null)
            {
                OnDataChanged(this, new UploadStatusEventArgs(Context));
            }
            if (LoadedLength == ContentLength)
            {
                EndTime = DateTime.Now;
                IsFinished = true;
                changeFinish();
            }
        }
        public void Abort()
        {
            Aborted = true;
            EndTime = DateTime.Now;
        }
        public void UpdateLoadedLength(long length)
        {
            if (!IsFinished && !Aborted)
            {
                LoadedLength += length;
                changeData();
            }
        }



        #endregion
    }
}
