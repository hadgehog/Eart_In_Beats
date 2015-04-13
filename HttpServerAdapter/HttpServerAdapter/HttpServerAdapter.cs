using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using MusicMaper.Models;

namespace HttpServerAdapter
{
    class HttpServerAdapter
    {
        public string Url { get; set; }

        public HttpServerAdapter(string ServerUrl)
        {
            this.Url = ServerUrl;
        }

        public bool Connect(PlayerNode Node)
        {
            string url = this.Url + "?Data=Connect?" + Node.ToStringUrl();

            return SendTo(url);
        }

        public bool Play(PlayerNode Node)
        {
            string url = this.Url + "?Data=Play?" + Node.ToStringUrl();

            return SendTo(url);
        }

        public bool Stop(PlayerNode Node)
        {
            string url = this.Url + "?Data=Stop?" + Node.ToStringUrl();

            return SendTo(url);
        }

        public bool Disconnect(PlayerNode Node)
        {
            string url = this.Url + "?Data=Disconnect?" + Node.ToStringUrl();
            
            return SendTo(url);
        }


        bool SendTo(string Url)
        {
            var request = WebRequest.Create(Url);
            var requestStream = new StreamReader(request.GetResponse().GetResponseStream());
            string answer = requestStream.ReadToEnd();
            requestStream.Close();
            if (answer.Contains("Ok"))
                return true;
            else
                return false;
        }
    }
}
