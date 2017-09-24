using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace Interface
{
    public static class Logger
    {
        private static StreamWriter m_LogStream = null;

        public static void InitLogger(string logname)
        {
            m_LogStream = new StreamWriter(logname, true);
            m_LogStream.WriteLine();
            m_LogStream.WriteLine();
            m_LogStream.WriteLine(DateTime.Now.ToString());
        }

        public static void Info(string msg)
        {
            Log("INFO", msg);
        }

        public static void Error(string msg)
        {
            Log("ERROR", msg);
            m_LogStream.Flush();
        }

        public static void Warning(string msg)
        {
            Log("WARNING", msg);
        }

        private static void Log(string type, string msg)
        {
            m_LogStream?.WriteLine(type + ":\t" + msg);
        }

        public static void StopLogger()
        {
            m_LogStream.WriteLine("************************************");
            m_LogStream.Close();
        }
    }
}
