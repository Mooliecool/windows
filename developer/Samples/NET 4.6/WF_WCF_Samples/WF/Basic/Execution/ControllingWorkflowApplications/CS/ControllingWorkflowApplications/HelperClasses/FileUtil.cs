//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.IO;

namespace Microsoft.Samples.WorkflowApplicationReadLineHost.HelperClasses
{

    static class FileUtil
    {
        const int fileBufferSize = 1024;

        public static void LoadFile(string path, Action<string, Stream> onSuccessfulLoad, TextWriter errorOut)
        {
            const string ioExceptionPreamble = "Could not read program file due to an IO Exception.";
            FileStream fileStream = null;

            try
            {
                fileStream = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.Read, fileBufferSize, true);
            }
            catch (FileNotFoundException fileNotFoundException)
            {
                errorOut.WriteLine("Could not read program file." + fileNotFoundException.Message);
                return;
            }
            catch (IOException ioException)
            {
                errorOut.Write(ioExceptionPreamble + ioException.Message);
                return;
            }

            LoadState loadState = new LoadState(fileBufferSize);
            int bytesRead = -1;
            Func<bool> iterate = null;
            AsyncCallback onEndRead = null;
            Action onLoadComplete = null;

            iterate = () =>
            {
                while (bytesRead != 0)
                {
                    IAsyncResult result = fileStream.BeginRead(loadState.ReadBuffer, 0, fileBufferSize, onEndRead, null);
                    if (!result.CompletedSynchronously)
                    {
                        return false;
                    }
                    bytesRead = fileStream.EndRead(result);
                    loadState.FlushReadBuffer(bytesRead);
                }
                return true;
            };

            onEndRead = (ar) =>
            {
                if (!ar.CompletedSynchronously)
                {
                    try
                    {
                        bytesRead = fileStream.EndRead(ar);
                        loadState.FlushReadBuffer(bytesRead);
                        if (iterate())
                        {
                            onLoadComplete();
                        }
                    }
                    catch (IOException ioException)
                    {
                        errorOut.Write(ioExceptionPreamble + ioException.Message);
                        fileStream.Close();
                    }
                    catch
                    {
                        fileStream.Close();
                        throw;
                    }
                }
            };

            onLoadComplete = () =>
            {
                fileStream.Close();
                loadState.BufferedFile.Seek(0, SeekOrigin.Begin);
                onSuccessfulLoad(path, loadState.BufferedFile);
            };

            try
            {
                if (iterate())
                {
                    onLoadComplete();
                }
            }
            catch (IOException ioException)
            {
                errorOut.Write(ioExceptionPreamble + ioException.Message);
                fileStream.Close();
            }
            catch
            {
                fileStream.Close();
                throw;
            }
        }


        class LoadState
        {
            MemoryStream bufferedFile;
            byte[] readBuffer;

            public LoadState(int bufferSize)
            {
                this.bufferedFile = new MemoryStream();
                this.readBuffer = new byte[bufferSize];
            }

            public MemoryStream BufferedFile
            {
                get { return this.bufferedFile; }
            }

            public byte[] ReadBuffer
            {
                get { return this.readBuffer; }
            }

            public void FlushReadBuffer(int bytesRead)
            {
                this.bufferedFile.Write(this.readBuffer, 0, bytesRead);
                this.bufferedFile.Flush();
            }
        }

    }
}
