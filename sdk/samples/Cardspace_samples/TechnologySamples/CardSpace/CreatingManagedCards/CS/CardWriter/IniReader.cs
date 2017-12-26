using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace ManagedCardWriter
{
    public class Section
    {
        internal Key[] keys;
        internal string name;
        public Section( string sectionName,  Key[] keys)
        {
            this.keys = keys;
            name = sectionName;
        }

        public Key this[string index]
        {
            get
            {
                foreach (Key k in keys)
                    if (k.name.Equals(index, StringComparison.CurrentCultureIgnoreCase))
                        return k;
                return new Key("","");
            }
        }
    }

    public class Key
    {
        internal string name;
        internal string value;
        public Key(string keyName, string value)
        {
            name = keyName;
            this.value = value;
        }
    }

    public class IniFile
    {
        [DllImport("kernel32", SetLastError = true)]
        private static extern int GetPrivateProfileString(string pSection, string pKey, string pDefault, byte[] prReturn, int pBufferLen, string pFile);

        internal Section[] sections;

        public IniFile(string fileName)
        {
            if (!System.IO.File.Exists(fileName))
                throw new System.IO.FileNotFoundException("File " + fileName + " Not found");

            byte[] buffer = new byte[32767];
            int i = 0;
            string[] sTmp, kTmp;

            i = GetPrivateProfileString(null, null, null, buffer, 32767, fileName);
            if (i == 0)
            {
                sections = new Section[0];
                return;
            }

            sTmp = System.Text.Encoding.GetEncoding(1252).GetString(buffer, 0, i).TrimEnd((char)0).Split((char)0);
            sections = new Section[sTmp.Length];
            for( int j = 0; j< sTmp.Length ; j++)
            {
                i = GetPrivateProfileString(sTmp[j], null, null, buffer, 32767, fileName);
                kTmp = System.Text.Encoding.GetEncoding(1252).GetString(buffer, 0, i).TrimEnd((char)0).Split((char)0);
                Key[] keys = new Key[kTmp.Length];
                for( int k =0; k< kTmp.Length ; k++)
                {
                    i = GetPrivateProfileString(sTmp[j], kTmp[k], null, buffer, 32767, fileName);
                    keys[k] = new Key( kTmp[k] , System.Text.Encoding.GetEncoding(1252).GetString(buffer, 0, i).TrimEnd((char)0));
                }
                sections[j] = new Section(sTmp[j], keys);
            }
        }


        public Section this[string index]
        {
            get
            {
                foreach( Section s in sections )
                    if( s.name.Equals( index, StringComparison.CurrentCultureIgnoreCase ) )
                        return s;
                return new Section("", new Key[0]);
            }
        }
    }

}
