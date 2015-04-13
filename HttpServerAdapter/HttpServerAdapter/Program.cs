using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MusicMaper.Models;

namespace HttpServerAdapter
{
    class Program
    {
        static void Main(string[] args)
        {
            var adapter = new HttpServerAdapter("http://localhost:1701/Home/Update/");

            Console.WriteLine(adapter.Connect(new PlayerNode { PhoneID = "byme", Latitude = 10, Longitude = 10 }));
            Console.WriteLine(adapter.Play(new PlayerNode { PhoneID = "byme", Latitude = 10, Longitude = 10, Artist = "doda", Title = "mota" }));
            Console.WriteLine(adapter.Stop(new PlayerNode { PhoneID = "byme", Latitude = 10, Longitude = 10}));
            Console.WriteLine(adapter.Disconnect(new PlayerNode { PhoneID = "byme", Latitude = 10, Longitude = 10 }));
            Console.ReadKey();
        }
    }
}
