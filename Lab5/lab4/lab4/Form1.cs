using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Security;

namespace lab4 {
    public partial class Form1 : Form {
        public Form1() {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e) {
            List<double> yValues = new List<double>() { 55.62, 45.54, 73.45, 9.73, 88.42, 45.9, 63.6, 85.1, 67.2, 23.6 };

            MemoryStatus status = MemoryStatus.CreateInstance();
            uint MemoryLoad = status.MemoryLoad;
            ulong TotalPhys = status.TotalPhys;
            ulong AvailPhys = status.AvailPhys;
            ulong TotalPageFile = status.TotalPageFile;
            ulong AvailPageFile = status.AvailPageFile;
            ulong TotalVirtual = status.TotalVirtual;
            ulong AvailVirtual = status.AvailVirtual;
            ulong AvailExtendedVirtual = status.AvailExtendedVirtual;
            lbx_inform.Items.Add("Всего физической памяти= " + TotalPhys / 1024 / 1024 + " MB");
            lbx_inform.Items.Add("Объем физической памяти, доступный в данный момент " + AvailPhys / 1024 / 1024 + " MB");
            lbx_inform.Items.Add("Всего файл подкачки= " + TotalPageFile / 1024 / 1024 + " MB");
            lbx_inform.Items.Add("Объем файла подкачки, доступный в данный момент " + AvailPageFile / 1024 / 1024 + " MB");
            lbx_inform.Items.Add("Всего виртуальной памяти= " + TotalVirtual / 1024 / 1024 + " MB");
            lbx_inform.Items.Add("Объем виртуальной памяти, доступный в данный момент " + AvailVirtual / 1024 / 1024 + " MB");
            lbx_inform.Items.Add("Используется памяти данным процессом: " + MemoryLoad + "байт");
            List<ulong> statuses = new List<ulong>() { (TotalPhys-AvailPhys), AvailPhys };
             List<ulong> statuses1 = new List<ulong>() { (TotalPageFile-AvailPageFile), AvailPageFile };
            List<ulong> statuses2 = new List<ulong>() { (TotalVirtual-AvailVirtual), AvailVirtual };
            Chart1.Series["Point"].Points.DataBindY(statuses);
            
            chart2.Series["Point"].Points.DataBindY(statuses1);
            chart3.Series["Point"].Points.DataBindY(statuses2);
        }

        private void button2_Click(object sender, EventArgs e) {
            System.Diagnostics.Process MyProc = new System.Diagnostics.Process();
            MyProc.StartInfo.FileName = @"C:\Users\gasan\source\repos\OS_lab4\Debug\OS_lab4.exe";
            MyProc.Start();
        }
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]

    public class MemoryStatus {
        [return: MarshalAs(UnmanagedType.Bool)]
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        internal static extern bool GlobalMemoryStatusEx([In, Out] MemoryStatus lpBuffer);

        private uint dwLength;
        public uint MemoryLoad;
        public ulong TotalPhys;
        public ulong AvailPhys;
        public ulong TotalPageFile;
        public ulong AvailPageFile;
        public ulong TotalVirtual;
        public ulong AvailVirtual;
        public ulong AvailExtendedVirtual;

        private static volatile MemoryStatus singleton;
        private static readonly object syncroot = new object();
        public static MemoryStatus CreateInstance() {
            if (singleton == null)
                lock (syncroot)
                    if (singleton == null)
                        singleton = new MemoryStatus();
            return singleton;
        }

        [SecurityCritical]
        private MemoryStatus() {
            dwLength = (uint)Marshal.SizeOf(typeof(MemoryStatus));
            GlobalMemoryStatusEx(this);
        }
    }
}
