using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace test2
{
    /// <summary>
    /// 普通字符串与UniCode字符串间转换
    /// </summary>
    public static class StringToUniCode
    {
        /// <summary>
        /// 字符串转为UniCode码字符串
        /// </summary>
        /// <param name="s"></param>
        /// <returns></returns>
        public static string StringToUnicode(string s)
        {
            char[] charbuffers = s.ToCharArray();
            byte[] buffer;
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < charbuffers.Length; i++)
            {
                buffer = System.Text.Encoding.Unicode.GetBytes(charbuffers[i].ToString());
                sb.Append(String.Format("//u{0:X2}{1:X2}", buffer[1], buffer[0]));
            }
            return sb.ToString();
        }
        /// <summary>
        /// Unicode字符串转为正常字符串
        /// </summary>
        /// <param name="srcText"></param>
        /// <returns></returns>
        public static string UnicodeToString(string srcText)
        {
            string dst = "";
            string src = srcText;
            int len = srcText.Length / 6;
            for (int i = 0; i <= len - 1; i++)
            {
                string str = "";
                str = src.Substring(0, 6).Substring(2);
                src = src.Substring(6);
                byte[] bytes = new byte[2];
                bytes[1] = byte.Parse(int.Parse(str.Substring(0, 2), NumberStyles.HexNumber).ToString());
                bytes[0] = byte.Parse(int.Parse(str.Substring(2, 2), NumberStyles.HexNumber).ToString());
                dst += Encoding.Unicode.GetString(bytes);
            }
            return dst;
        }

        public static string UTF8ToGb2312(string str)
        {
            string gb2312info = string.Empty;

            Encoding utf8 = Encoding.UTF8;
            Encoding gb2312 = Encoding.GetEncoding("gb2312");

            byte[] unicodeBytes = utf8.GetBytes(str);

            byte[] asciiBytes = Encoding.Convert(utf8, gb2312, unicodeBytes);

            char[] asciiChars = new char[gb2312.GetCharCount(asciiBytes, 0, asciiBytes.Length)];
            gb2312.GetChars(asciiBytes, 0, asciiBytes.Length, asciiChars, 0);
            gb2312info = new string(asciiChars);
            return gb2312info;
        }
    }
}
