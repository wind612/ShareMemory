using System;
using System.IO;
using System.IO.MemoryMappedFiles;
using System.Runtime.InteropServices;
using System.Threading;

namespace test2
{
    public class ShareMemory
    {
        private const string ShareMemoryName_read = "ShareMemory_to_cshap";
        private const string ShareMemoryName_write = "ShareMemory_to_cpp";
        private const string mutexName = "testmapmutex";
        private const int MaxShareMemorySize = 1024000;
        public Mutex mutex { get; set; }
        public MemoryMappedFile mmf_read { get; set; }
        public MemoryMappedFile mmf_write { get; set; }

        public ShareMemory()
        {
            bool mutexCreated;
            mutex = new Mutex(false, mutexName, out mutexCreated);
            mmf_read = MemoryMappedFile.CreateOrOpen(ShareMemoryName_read, MaxShareMemorySize, MemoryMappedFileAccess.ReadWrite);
            mmf_write = MemoryMappedFile.CreateOrOpen(ShareMemoryName_write, MaxShareMemorySize, MemoryMappedFileAccess.ReadWrite);
        }
        ~ShareMemory()
        {                        
        }

        public byte[] Read()
        {
            byte[] bytes = new byte[10];
            Console.WriteLine("waitting for read...");

            mutex.WaitOne();
            using (MemoryMappedViewStream stream = mmf_read.CreateViewStream()) //创建文件内存视图流
            {                
                BinaryReader reader = new BinaryReader(stream);
                int datalen = reader.ReadInt32();
                Console.WriteLine($"datalen = {datalen}");
                if (datalen > 0 )
                {                    
                    bytes = reader.ReadBytes(datalen);
                }
                else
                {
                    Thread.Sleep(500);
                }
                
            }
            mutex.ReleaseMutex();

            return bytes;
        }

        public void Write(byte[] bytes, Int32 datalen)
        {
            Console.WriteLine("waitting for write...");

            if(datalen < 0)
            {
                return;
            }

            mutex.WaitOne();
            using (MemoryMappedViewStream stream = mmf_read.CreateViewStream()) //创建文件内存视图流
            {
                BinaryWriter writer = new BinaryWriter(stream);
                // write datalen first
                byte[] len = BitConverter.GetBytes(datalen);
                for (int i = 0; i < Marshal.SizeOf(datalen); i++)
                {
                    writer.Write(len[i]);
                }

                // then write data.
                for (int i = 0; i < datalen; i++)
                {
                    writer.Write(bytes[i]);
                }
            }
            mutex.ReleaseMutex();
        }

    }
}
