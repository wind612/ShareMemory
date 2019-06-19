﻿using System;
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

    [StructLayout(LayoutKind.Sequential)]
    public struct smm_header
    {
        public Int32 length;
        public Int32 command;
    }

    class Program
    {
        static void Main(string[] args)
        {
            test1();

            Console.ReadKey();
        }
    
        static void test1()
        {
            smm_header head = new smm_header();
            head.command = 1;
            head.length = Marshal.SizeOf(typeof(Person));

            ShareMemory sm = new ShareMemory();

            var bytesBuffer = Struct_Transform.StructToBytes(head);
            Console.WriteLine("waitting for write smm_header...");
            sm.Write(bytesBuffer, 0, bytesBuffer.Length);

            Person pp = new Person();
            pp.age = 13;
            pp.sex = 1;
            bytesBuffer = Struct_Transform.StructToBytes(pp);
            Console.WriteLine("waitting for write Person...");
            sm.Write(bytesBuffer, Marshal.SizeOf(typeof(smm_header)), bytesBuffer.Length);

            // read
            Console.WriteLine("waitting for read...");
            Thread.Sleep(3000);
            byte[] bytes = new byte[Marshal.SizeOf(typeof(smm_header))];
            bytes = sm.Read(0, bytes.Length);
            smm_header head2 = Struct_Transform.BytesToStruct<smm_header>(bytes);
            Console.WriteLine($"command = {head2.command}, length = {head2.length}");
        }

    }
}
