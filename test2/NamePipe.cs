using System;
using System.IO;
using System.IO.Pipes;
using System.Text;

namespace test2
{
    public class NamePipe
    {
        private const string PipeName = "PipeCppCshap";
        private NamedPipeClientStream pipe;

        public NamePipe()
        {
            pipe = new NamedPipeClientStream("localhost", PipeName, PipeDirection.InOut);

            pipe.Connect(5000);
            pipe.ReadMode = PipeTransmissionMode.Message;
        }
        ~NamePipe()
        {
        }

        public void Read(out byte[] bytes, int datalen)
        {
            byte[] buffer = new byte[1024];
            using (var ms = new MemoryStream())
            {
                do
                {
                    var readBytes = pipe.Read(buffer, 0, buffer.Length);
                    ms.Write(buffer, 0, readBytes);
                }
                while (!pipe.IsMessageComplete);

                bytes = ms.ToArray();
            }
        }

        public void Write(byte[] bytes, int datalen)
        {
            if (pipe.IsConnected)
            {
                try
                {
                    pipe.Write(bytes, 0, datalen);
                    //pipe.Flush();
                    //pipe.WaitForPipeDrain();
                }
                catch { }
            }
        }
    }
}
