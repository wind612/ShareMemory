﻿using Newtonsoft.Json;
using System;
using System.Collections.Generic;
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

    public class mypoint
    {
        public int x { get; set; }
        public int y { get; set; }
    };

    public class Person2
    {
        public string name { get; set; }
        public string address { get; set; }
        public int age { get; set; }
        public List<int> Tmplist1 { get; set; } = new List<int>();
        public List<mypoint> Tmplist2 { get; set; } = new List<mypoint>();
    }

    class Program
    {
        static void Main(string[] args)
        {
            test2();
            //test_write();
            //test_read();

            Console.ReadKey();
        }

        static void test_write()
        {
            //write
            Console.WriteLine("begin to write...");

            var points = new mypoint[]
            {
                new mypoint(){ x=1, y=1 },
                new mypoint(){ x=2, y=2 },
                new mypoint(){ x=3, y=3 },
            };
            List<mypoint> Tmplist2 = new List<mypoint>();
            Tmplist2.AddRange(points);
            Person2 pp2 = new Person2() { name = "李四 abc1", address = "add2", age = 10, Tmplist1 = { 11, 20, 35 }, Tmplist2 = Tmplist2 };

            // json序列化
            string jsonStr = JsonConvert.SerializeObject(pp2);
            var StrBuffer = System.Text.Encoding.UTF8.GetBytes(jsonStr);

            smm_header head = new smm_header();
            head.command = 1;
            head.length = StrBuffer.Length;

            // write head
            ShareMemory sm = new ShareMemory();

            var bytesBuffer = Struct_Transform.StructToBytes(head);
            Console.WriteLine("waitting for write smm_header...");
            sm.Write(bytesBuffer, 0, bytesBuffer.Length);

            // write data
            Console.WriteLine("waitting for write data...");
            sm.Write(StrBuffer, Marshal.SizeOf(typeof(smm_header)), StrBuffer.Length);

            Console.WriteLine("write data finish.");
        }

        static void test_read()
        {
            // read
            ShareMemory sm = new ShareMemory();

            Console.WriteLine("waitting for read...");
            Thread.Sleep(3000);

            // read head.
            var bytes = new byte[Marshal.SizeOf(typeof(smm_header))];
            sm.Read(out bytes, 0, Marshal.SizeOf(typeof(smm_header)));
            smm_header head2 = Struct_Transform.BytesToStruct<smm_header>(bytes);
            Console.WriteLine($"command = {head2.command}, length = {head2.length}");

            // read data.
            var bytes2 = new byte[head2.length];
            sm.Read(out bytes2, Marshal.SizeOf(typeof(smm_header)), head2.length);
            string str = System.Text.Encoding.UTF8.GetString(bytes2);   
            Console.WriteLine($"str = {str}");

            //反序列化
            Person2 two = JsonConvert.DeserializeObject<Person2>(str);
            Console.WriteLine(
                   string.Format("学生信息  name:{0},address:{1},age:{2}",
                   two.name, two.address, two.age));//显示结果
            two.Tmplist1.ForEach(o => Console.WriteLine($"Tmplist1:{o}"));
            two.Tmplist2.ForEach(o => Console.WriteLine($"Tmplist2: x={o.x}, y={o.y}"));
        }


        static void test1()
        {
            //write
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
            var bytes = new byte[Marshal.SizeOf(typeof(smm_header))];
            sm.Read(out bytes, 0, Marshal.SizeOf(typeof(smm_header)));
            smm_header head2 = Struct_Transform.BytesToStruct<smm_header>(bytes);
            Console.WriteLine($"command = {head2.command}, length = {head2.length}");
        }

        static void test2()
        {
            // read
            NamePipe pipe = new NamePipe();

            Console.WriteLine("waitting for read...");
            Thread.Sleep(3000);

            // read head.
            var bytes = new byte[Marshal.SizeOf(typeof(smm_header))];
            pipe.Read(out bytes, Marshal.SizeOf(typeof(smm_header)));
            smm_header head2 = Struct_Transform.BytesToStruct<smm_header>(bytes);
            Console.WriteLine($"command = {head2.command}, length = {head2.length}");

            //test write
            smm_header head = new smm_header();
            head.command = 1;
            head.length = 10;

            // write head
            var bytesBuffer = Struct_Transform.StructToBytes(head);
            Console.WriteLine("waitting for write smm_header...");
            pipe.Write(bytesBuffer, bytesBuffer.Length);
            Console.WriteLine("write finish.");
        }

    }
}
