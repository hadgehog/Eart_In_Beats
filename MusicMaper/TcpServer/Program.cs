using MusicMaper.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Newtonsoft.Json;
using Business.DAL;

namespace TcpServer
{
    class Program
    {
        static DefaultRepository repository = new DefaultRepository();

        static void Main(string[] args)
        {
            TcpListener listener = new TcpListener(12345);
            listener.Start();
            Thread thread = new Thread(() =>
            {
                while (true)
                {
                    var client = listener.AcceptTcpClient();
                    BinaryReader reader = new BinaryReader(client.GetStream());
                    BinaryWriter writer = new BinaryWriter(client.GetStream());

                    var data = reader.ReadString();
                    if (data == "0.1")
                    {
                        writer.Write("0.1");
                        client.Close();
                        continue;
                    }

                    DeviceInput input = JsonConvert.DeserializeObject<DeviceInput>(data);
                    repository.Processing(input);
                    writer.Write("Ok!");
                    client.Close();
                }

            });
            thread.Start();

            Console.ReadKey();
        }
    }
}
