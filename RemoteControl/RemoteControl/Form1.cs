using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace RemoteControl
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button_connect_Click(object sender, EventArgs e)
        {
            if (!isConnected())
            {
                connect();
            } else {
                disconnect();
            }
        }

        private bool isConnected()
        {
            return port != null;
        }

        private void update_ui()
        {
            if (isConnected())
            {
                button_connect.Text = "Disconnect";
            }
            else
            {
                button_connect.Text = "Connect";
            }

            trackBar_kp.Value = trackBar_kp.Maximum / 2;
            trackBar_ki.Value = trackBar_ki.Maximum / 2;
            trackBar_kd.Value = trackBar_kd.Maximum / 2;

            trackBar_at.Value = trackBar_at.Maximum / 2;
        }

        private SerialPort port = null;
        private const string comport = "COM7";
        private void connect()
        {
            try
            {
                port = new SerialPort(comport, 9600);
                port.Parity = Parity.None;
                port.StopBits = StopBits.One;
                port.DataBits = 8;
                port.Handshake = Handshake.None;
                port.DataReceived += Port_DataReceived;
                port.Open();
            } catch (Exception ex) {
                MessageBox.Show(ex.Message);
                return;
            }

            listBox_events.Items.Clear();
            update_ui();
        }

        private void disconnect()
        {
            port.DataReceived -= Port_DataReceived;
            port.Close();
            port = null;

            update_ui();
        }

        private delegate int AddCallDelegate(string text);
        private void register_event(string msg)
        {
            string entry_text = DateTime.Now.ToLongTimeString() + " --- " + msg;
            
            var add_entry = new AddCallDelegate(listBox_events.Items.Add);
            //Invoke(add_entry, new object[] { entry_text });
            add_entry(entry_text);

            listBox_events.SelectedIndex = listBox_events.Items.Count - 1;
            listBox_events.SelectedIndex = -1;
        }

        private void handle_in_data(byte ch, byte[] data, int len)
        {
            string msg;

            switch (ch)
            {
                case 0:
                    msg = ASCIIEncoding.ASCII.GetString(data, 0, len);
                    register_event("inf: " + msg);
                    break;
                case 1:
                    msg = ASCIIEncoding.ASCII.GetString(data, 0, len);
                    register_event("dbg: " + msg);
                    break;
                case 2:
                    msg = ASCIIEncoding.ASCII.GetString(data, 0, len);
                    register_event("err: " + msg);
                    break;
                case 10:
                    // a pid data package
                    UInt32 pid_sampletime = BitConverter.ToUInt32(data, 4 * 0);
                    UInt32 pid_handlertime = BitConverter.ToUInt32(data, 4 * 1);
                    UInt32 pid_counter = BitConverter.ToUInt32(data, 4 * 2);

                    float angle = BitConverter.ToSingle(data, 4 * 3);
                    float error = BitConverter.ToSingle(data, 4 * 4);
                    float errorDiff = BitConverter.ToSingle(data, 4 * 5);
                    float errorSum = BitConverter.ToSingle(data, 4 * 6);

                    byte last_noyield = data[4 * 7];

                    label_angleVal.Text = angle.ToString();

                    break;
                default:
                    register_event(String.Format("incoming data, unhandled channel: {0}", ch));
                    break;
            }
        }

        private void send_value(byte ch, Int32 val)
        {
            send_out_data(ch, BitConverter.GetBytes(val), 4);
        }
        private void send_value(byte ch, float val)
        {
            send_out_data(ch, BitConverter.GetBytes(val), 4);
        }

        private byte[] out_buffer = new byte[2 + 1 + 1 + 255 + 2];
        private void send_out_data(byte ch, byte[] data, int len)
        {
            byte lenb = (byte)len;
            int lenTotal = 2 + 1 + 1 + lenb + 2;
            out_buffer[0] = 0x55;
            out_buffer[1] = 0xAA;
            out_buffer[2] = ch;
            out_buffer[3] = lenb;
            Buffer.BlockCopy(data, 0, out_buffer, 4, lenb);
            out_buffer[lenTotal - 2] = 0x88;
            out_buffer[lenTotal - 1] = 0x11;

            port.Write(out_buffer, 0, lenTotal);
            System.Threading.Thread.Sleep(100);
        }

        private int in_idx = 0;
        private byte in_ch = 0;
        private byte in_data_len = 0;
        private byte[] in_data = new byte[255];

        private delegate void HandleSerialPortDelegate(SerialPort port);
        private void Port_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {

            BeginInvoke(new HandleSerialPortDelegate(in_handle), new object[] { port });
            /*
            try
            {
                in_handle();
            }
            catch (Exception ex)
            {
                register_event(ex.Message);
            }*/
        }

        private void in_handle(SerialPort port)
        {
            try
            {
                while (port.BytesToRead > 0)
                {
                    int i = port.ReadByte();
                    if (i < 0)
                        throw new Exception("stream ended unexpectedly");
                    byte b = (byte)i;

                    switch (in_idx)
                    {
                        case 0:
                            if (b == 0x55)
                                in_idx++;
                            continue;
                        case 1:
                            if (b == 0xAA)
                            {
                                in_idx++;
                            }
                            else
                            {
                                in_idx = 0;
                                register_event("ERR: comm init sync");
                            }
                            continue;
                        case 2: // channel
                            in_idx++;
                            in_ch = b;
                            continue;
                        case 3: // length
                            in_idx++;
                            in_data_len = b;
                            continue;
                        default:
                            break;
                    }

                    int in_data_idx = in_idx - 4;
                    int in_data_after_idx = in_data_idx - in_data_len;
                    if (in_data_after_idx < 0)
                    {
                        in_idx++;
                        in_data[in_data_idx] = b;
                    }
                    else if (in_data_after_idx == 0)
                    {
                        if (b == 0x88)
                        {
                            in_idx++;
                        }
                        else
                        {
                            in_idx = 0;
                            register_event("ERR: comm end sync");
                        }
                    }
                    else if (in_data_after_idx == 1)
                    {
                        if (b == 0x11)
                        {
                            in_idx = 0;
                            handle_in_data(in_ch, in_data, in_data_len);
                        }
                        else
                        {
                            in_idx = 0;
                            register_event("ERR: comm end sync");
                        }
                    }
                    else
                    {
                        throw new Exception("unexpected comm parsing state");
                    }
                }

            }
            catch (Exception ex)
            {
                register_event(ex.Message);
            }
        }

        private bool motor_on = false;
        private void button_motor_Click(object sender, EventArgs e)
        {
            motor_on = !motor_on;
            send_value(50, motor_on ? (int)0 : (int)1);
        }

        private void trackBar_kp_ValueChanged(object sender, EventArgs e)
        {
            if (port == null) return;
            float value = ((float)trackBar_kp.Value) / (trackBar_kp.Maximum / 2) - 1;

            float kp = 0.15f;
            kp += value * 0.1f;

            label_kp_val.Text = kp.ToString();

            send_value(60, kp);
        }

        private void trackBar_ki_ValueChanged(object sender, EventArgs e)
        {
            if (port == null) return;
            float value = ((float)trackBar_ki.Value) / (trackBar_ki.Maximum / 2) - 1;

            float ki = 0.9f;
            ki += value * 0.6f;

            label_ki_val.Text = ki.ToString();

            send_value(61, ki);
        }

        private void trackBar_kd_ValueChanged(object sender, EventArgs e)
        {
            if (port == null) return;
            float value = ((float)trackBar_kd.Value) / (trackBar_kd.Maximum / 2) - 1;

            float kd = 0.003f;
            kd += value * 0.002f;

            label_kd_val.Text = kd.ToString();

            send_value(62, kd);
        }

        private void trackBar_at_ValueChanged(object sender, EventArgs e)
        {
            if (port == null) return;
            float value = ((float)trackBar_at.Value) / (trackBar_at.Maximum / 2) - 1;

            float at = 6.2f;
            at += value * 2.0f;

            label_at_val.Text = at.ToString();

            send_value(70, at);
        }
    }
}
