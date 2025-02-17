using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.Threading.Tasks;

namespace Play_Kazhutha.PlayLocal
{
    public class Client
    {
        private TcpClient client;
        private NetworkStream stream;
        string recived;

        public async Task ConnectToServer(string serverIP, int port = 7777)
        {
            client = new TcpClient();
            await client.ConnectAsync(serverIP, port);
            stream = client.GetStream();
            Console.WriteLine($"Connected to {serverIP}:{port}");

            _ = Task.Run(() => ListenForMessages());
        }

        private async void ListenForMessages()
        {
            byte[] buffer = new byte[1024];

            while (true)
            {
                int bytesRead = await stream.ReadAsync(buffer, 0, buffer.Length);
                if (bytesRead == 0) break;

                string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);
                Console.WriteLine($"Server says: {message}");
                recived = message;
            }
        }

        public async Task SendMessage(string message)
        {
            byte[] data = Encoding.UTF8.GetBytes(message);
            await stream.WriteAsync(data, 0, data.Length);
        }
        public string ReturnMessageTemp()
        {
            return "Received: " + recived;
        }

    }
}
