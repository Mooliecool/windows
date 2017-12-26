/****************************** Module Header ******************************\
* Module Name:    UploadFileCollection.cs
* Project:        CSASPNETFileUploadStatus
* Copyright (c) Microsoft Corporation
*
* The project illustrates how to display the upload status and progress without
* a third part component like ActiveX control, Flash or Silverlight.
* 
* This is a class which used to filter the file data in the request entity and 
* store them into the UploadFileCollection. 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
\*****************************************************************************/

using System.Text;
using System.Text.RegularExpressions;

namespace CSASPNETFileUploadStatus
{
    internal class FileUploadDataManager
    {
        private bool isFinished = true;
        private byte[] draft = null;
        private bool isFile = false;

        private readonly byte[] _backSpace = null;
        private readonly byte[] _doubleBackSpace = null;
        private readonly byte[] _boundary = null;
        private readonly byte[] _endTag = null;

        public FileUploadDataManager(string boundary)
        {
            _boundary = ASCIIEncoding.ASCII.GetBytes("--" + boundary);
            _backSpace = ASCIIEncoding.ASCII.GetBytes("\r\n");
            _doubleBackSpace = ASCIIEncoding.ASCII.GetBytes("\r\n\r\n");
            _endTag = ASCIIEncoding.ASCII.GetBytes("--" + boundary + "--\r\n");
            FilterResult = new UploadFileCollection();
            draft = null;
            isFile = false;
            isFinished = true;
        }

        public UploadFileCollection FilterResult
        {
            private set;
            get;
        }

        public void AppendData(byte[] data)
        {
            FilterFileDataFromBodyEntity(data);
            if (data == null)
                return;
        }

        private void AppendToLastEntity(byte[] data)
        {
            if (!isFinished && isFile)
            {
                UploadFile lastFile =
                    FilterResult[FilterResult.Count - 1];
                lastFile.AppendData(data);
            }
        }

        // Filter the request data to get the file binary data.
        // Here is an algorithm logic to filter the data. There
        // will be a lot of words to say before we talk about 
        // the algorithm(Maybe a small book). So I just open the
        // source here but omit the explanation in details.
        private void FilterFileDataFromBodyEntity(byte[] data)
        {
            if (data == null)
            {
                return;
            }

            if (draft != null)
            {
                byte[] temp = BinaryHelper.Combine(draft,
                                BinaryHelper.Copy(data, 0, _boundary.Length));
                int entity_st = BinaryHelper.SequenceIndexOf(temp, _boundary, 0);
                int entity_ed = BinaryHelper.SequenceIndexOf(temp, _boundary,
                    entity_st + _boundary.Length + 2);

                if (isFile && !isFinished)
                {
                    if (entity_st == 0)
                    {
                        int header_st = entity_st + _boundary.Length + 2;
                        int header_ed = BinaryHelper.SequenceIndexOf(temp,
                            _doubleBackSpace, header_st);
                        int body_st = header_ed + 4;
                        if (entity_ed == -1)
                        {
                            AppendToLastEntity(BinaryHelper.SubData(draft, body_st));
                            draft = null;
                        }
                        else
                        {
                            AppendToLastEntity(BinaryHelper.SubData(draft,
                                body_st, entity_ed - body_st - 2));
                            isFinished = true;
                            isFile = false;
                            draft = BinaryHelper.SubData(draft, entity_ed);
                        }
                    }
                    else
                    {
                        AppendToLastEntity(draft);
                        draft = null;
                    }
                }

                // When need append new data, combine the two
                // binary array into one.
                data = BinaryHelper.Combine(draft, data);
                draft = null;
            }
            while (true)
            {
                // find the boundary
                int entity_st = BinaryHelper.SequenceIndexOf(data, _boundary, 0);

                // if the current loaded data contain the boundary
                if (entity_st > -1)
                {
                    if (isFile && !isFinished)
                    {
                        AppendToLastEntity(BinaryHelper.SubData(data, 0,
                            entity_st - 2));
                        data = BinaryHelper.SubData(data, entity_st);
                        isFile = false;
                        isFinished = true;
                        continue;
                    }

                    int entity_ed = BinaryHelper.SequenceIndexOf(data, _boundary,
                        entity_st + _boundary.Length + 2);
                    int header_st = entity_st + _boundary.Length + 2;
                    int header_ed = BinaryHelper.SequenceIndexOf(data,
                        _doubleBackSpace, header_st);
                    int body_st = header_ed + 4;

                    if (body_st < 4)
                    {
                        // If the header in the entity is not complete, then
                        // set the draft as the data, and dump out the function
                        // to ask for more data.
                        draft = data;
                        return;
                    }
                    else
                    {
                        // If the header in the entity is complete 
                        if (!isFile && isFinished)
                        {
                            // Encoding the data in the header of the entity
                            string headerInEntity = ASCIIEncoding.UTF8.GetString(
                                BinaryHelper.SubData(data, header_st, header_ed - header_st));
                            // If it is a file entity, the header contain the keyword:"filename".
                            if (headerInEntity.IndexOf("filename") > -1)
                            {
                                // Use Regular Expression to get the meta key values from 
                                // the header of the entity.
                                Regex detailsReg =
                                    new Regex("Content-Disposition: form-data; name=\"([^\"]*)\";" +
                                        " filename=\"([^\"]*)\"Content-Type: ([^\"]*)");
                                Match regMatch =
                                    detailsReg.Match(headerInEntity.Replace("\r\n", ""));
                                string controlName = regMatch.Groups[1].Value;
                                string clientPath = regMatch.Groups[2].Value;
                                string contentType = regMatch.Groups[3].Value;
                                if (string.IsNullOrEmpty(clientPath))
                                {
                                    isFile = false;
                                }
                                else
                                {
                                    isFile = true;
                                    // Create a new instance for the file entity
                                    UploadFile up = new UploadFile(clientPath, contentType);
                                    FilterResult.Add(up);
                                    isFinished = false;
                                }
                            }
                            else
                            {
                                isFile = false;
                            }
                        }

                    }
                    if (entity_ed > -1)
                    {
                        // If we can find another boundary after the first boundary,
                        // that means the entity block is ended there.
                        // Only if it is a file entity we need to get the data
                        // in the body of the entity
                        if (isFile)
                        {
                            AppendToLastEntity(BinaryHelper.SubData(data,
                                body_st, entity_ed - body_st - 2));
                            isFinished = true;
                            isFile = false;
                        }
                        // Remove the current processed entity data
                        // and loop for the next one.
                        data = BinaryHelper.SubData(data, entity_ed);
                        if (BinaryHelper.Equals(data, _endTag))
                        {
                            data = null;
                            draft = null;
                            return;
                        }
                        continue;
                    }
                    else
                    {
                        // If we can't find the end mark, we have to 
                        // move the data to the draft and 
                        // ask for new data to append.
                        draft = data;
                        return;
                    }
                }
                else
                {
                    // If we can't find any mark of the boundary,
                    // we have to move the data to the draft and 
                    // ask for new data to append.
                    draft = data;
                    return;
                }

            }


        }
    }
}
