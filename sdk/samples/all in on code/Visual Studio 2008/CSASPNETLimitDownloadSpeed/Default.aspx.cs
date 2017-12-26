/**************************** Module Header ********************************\
* Module Name:    Default.aspx.cs
* Project:        CSASPNETLimitDownloadSpeed
* Copyright (c) Microsoft Corporation
*
* This project illustrates how to limit the download speed via coding. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\***************************************************************************/

using System;
using System.Web;
using System.IO;
using System.Threading;

namespace CSASPNETLimitDownloadSpeed
{

    public partial class Default : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            // You can enlarge the size of the file to have a longer download period later.
            // 1024 * 1024 * 1 = 1 Mb
            int length = 1024 * 1024 * 1;
            byte[] buffer = new byte[length];

            string filepath = Server.MapPath("~/bigFileSample.dat");
            using (FileStream fs = new FileStream(filepath, FileMode.Create, FileAccess.Write))
            {
                fs.Write(buffer, 0, length);
            }
        }

        protected void btnDownload_Click(object sender, EventArgs e)
        {
            string outputFileName = "bigFileSample.dat";
            string filePath = Server.MapPath("~/bigFileSample.dat");

            string value = ddlDonwloadSpeed.SelectedValue;

            // 1024 * 20 = 20 Kb/s.
            int downloadSpeed = 1024 * int.Parse(value);

            Response.Clear();

            // Call DownloadFileWithLimitedSpeed method to download the file.
            try
            {
                DownloadFileWithLimitedSpeed(outputFileName, filePath, downloadSpeed);
            }
            catch (Exception ex)
            {
                Response.Write(@"<p><font color=""red"">");
                Response.Write(ex.Message);
                Response.Write(@"</font></p>");
            }
            Response.End();
        }

        public void DownloadFileWithLimitedSpeed(string fileName, string filePath, long downloadSpeed)
        {
            if (!File.Exists(filePath))
            {
                throw new Exception("Err: There is no such a file to download.");
            }

            // Get the BinaryReader instance to the file to download.
            using (FileStream fs = new FileStream(filePath, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
            {
                using (BinaryReader br = new BinaryReader(fs))
                {

                    Response.Buffer = false;

                    // The file length.
                    long fileLength = fs.Length;

                    // The minimum size of a package 1024 = 1 Kb.
                    int pack = 1024;

                    // The original formula is: sleep = 1000 / (downloadspeed / pack)
                    // which equals to 1000.0 * pack / downloadSpeed.
                    // And here 1000.0 stands for 1000 millisecond = 1 second
                    int sleep = (int)Math.Ceiling(1000.0 * pack / downloadSpeed);


                    // Set the Header of the current Response.
                    Response.AddHeader("Content-Length", fileLength.ToString());
                    Response.ContentType = "application/octet-stream";

                    string utf8EncodingFileName = HttpUtility.UrlEncode(fileName, System.Text.Encoding.UTF8);
                    Response.AddHeader("Content-Disposition", "attachment;filename=" + utf8EncodingFileName);

                    // The maxCount stands for a total count that the thread sends the file pack.
                    int maxCount = (int)Math.Ceiling(Convert.ToDouble(fileLength) / pack);

                    for (int i = 0; i < maxCount; i++)
                    {
                        if (Response.IsClientConnected)
                        {
                            Response.BinaryWrite(br.ReadBytes(pack));

                            // Sleep the response thread after it sends a file pack.
                            Thread.Sleep(sleep);
                        }
                        else
                        {
                            break;
                        }
                    }

                }
            }
        }
    }
}