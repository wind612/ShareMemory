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
        private const string mutexName_read = "testmapmutex_read";
        private const string mutexName_write = "testmapmutex_write";
        private const int MaxShareMemorySize = 1024000;
        public Mutex mutex_read { get; set; }
        public Mutex mutex_write { get; set; }
        public MemoryMappedFile mmf_read { get; set; }
        public MemoryMappedFile mmf_write { get; set; }

        public ShareMemory()
        {
            bool mutexCreated;
            mutex_read = new Mutex(false, mutexName_read, out mutexCreated);
            mutex_write = new Mutex(false, mutexName_write, out mutexCreated);
            mmf_read = MemoryMappedFile.CreateOrOpen(ShareMemoryName_read, MaxShareMemorySize, MemoryMappedFileAccess.ReadWrite);
            mmf_write = MemoryMappedFile.CreateOrOpen(ShareMemoryName_write, MaxShareMemorySize, MemoryMappedFileAccess.ReadWrite);
        }
        ~ShareMemory()
        {                        
        }

        public void Read(out byte[] bytes, int index, int datalen)
        {
            mutex_read.WaitOne();
            using (MemoryMappedViewStream stream = mmf_read.CreateViewStream(index, datalen)) //创建文件内存视图流
            {                
                BinaryReader reader = new BinaryReader(stream);
                bytes = reader.ReadBytes(datalen);
                //reader.Read(bytes, 0, datalen); //为何不需要out/ref也能传出参数？
            }
            mutex_read.ReleaseMutex();
        }

        public void Write(byte[] bytes, int index, int datalen)
        {
            mutex_write.WaitOne();
            using (MemoryMappedViewStream stream = mmf_write.CreateViewStream(index, datalen)) //创建文件内存视图流
            {
                BinaryWriter writer = new BinaryWriter(stream);
                writer.Write(bytes);
            }
            mutex_write.ReleaseMutex();
        }
    }
}
