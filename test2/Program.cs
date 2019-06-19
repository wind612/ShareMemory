using System;
using System.IO;
using System.IO.MemoryMappedFiles;
using System.Runtime.InteropServices;
using System.Threading;

namespace test2
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Person
    {
        public Int32 age;

        public Int32 sex;

        //public string secret;

    }

    class Program
    {
        static void Main(string[] args)
        {
            test2();

            Console.ReadKey();
        }

        static void test1()
        {
            byte[] datas = new byte[] { 1, 2, 3 };

            ShareMemory sm = new ShareMemory();
            sm.Write(datas, datas.Length);
            byte[] bytes = sm.Read();
            foreach (var data in bytes)
            {
                Console.WriteLine($"{data.ToString()}");
            }
        }

        static void test2()
        {
            Person pp = new Person();
            pp.age = 13;
            pp.sex = 1;
            
            var bytesBuffer = Struct_Transform.StructToBytes(pp);
            foreach (var data in bytesBuffer)
            {
                Console.WriteLine($"{data.ToString()}");
            }

            ShareMemory sm = new ShareMemory();
            //sm.Write(bytesBuffer, bytesBuffer.Length);

            byte[] bytes = sm.Read();
            foreach (var data in bytes)
            {
                Console.WriteLine($"{data.ToString()}");
            }
            Person pp2 = Struct_Transform.BytesToStruct<Person>(bytes);
            Console.WriteLine($"age = {pp2.age}, sex = {pp2.sex}");
        }

        static void test()
        {
            Console.WriteLine("共享内存公用名(ShareMemory):");
            string shareName = "ShareMemory";

            using (MemoryMappedFile mmf = MemoryMappedFile.CreateOrOpen(shareName, 1024000, MemoryMappedFileAccess.ReadWrite))
            {
                bool mutexCreated;
                //进程间同步
                var mutex = new Mutex(true, "testmapmutex", out mutexCreated);
                using (MemoryMappedViewStream stream = mmf.CreateViewStream()) //创建文件内存视图流
                {
                    Console.WriteLine("waitting for write...");
                    Thread.Sleep(5000);
                    var writer = new BinaryWriter(stream);
                    for (int i = 0; i < 5; i++)
                    {
                        writer.Write((byte)i);
                        Console.WriteLine("{0}位置写入流:{0}", i);
                    }
                }
                mutex.ReleaseMutex();
                Console.WriteLine("启动状态服务，按【回车】读取共享内存数据");
                Console.ReadLine();
                mutex.WaitOne();
                using (MemoryMappedViewStream stream = mmf.CreateViewStream())
                {
                    var reader = new BinaryReader(stream);
                    for (int i = 0; i < 10; i++)
                    {
                        Console.WriteLine("{1}位置:{0}", reader.ReadByte(), i);
                    }
                }
                //using (MemoryMappedViewAccessor accessor = mmf.CreateViewAccessor(1024, 10240))
                //{
                //    int colorSize = Marshal.SizeOf(typeof(ServiceMsg));
                //    ServiceMsg color;
                //    for (int i = 0; i < 50; i += colorSize)
                //    {
                //        accessor.Read(i, out color);
                //        Console.WriteLine("{1}\tNowTime:{0}", new DateTime(color.NowTime), color.Id);
                //    }
                //}
                mutex.ReleaseMutex();
            }
            Console.WriteLine("测试： 我是 即时通讯 - 消息服务 我启动啦！！！");
            Console.ReadKey();
        }
    }
}
