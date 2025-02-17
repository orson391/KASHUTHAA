//using Play_Kazhutha.Platforms.Android;

//using Play_Kazhutha.Platforms.Android.NetworkHelper;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace Play_Kazhutha.PlayLocal.Host
{
    public class Server
    {
        private TcpListener listener;
        string mess;
        public async Task StartServer(int port = 7777)
        {
            listener = new TcpListener(IPAddress.Any, port);
            listener.Start();
            //Console.WriteLine($"Server started on {GetLocalIPAddress()}:{port}");

            while (true)
            {
                TcpClient client = await listener.AcceptTcpClientAsync();
                _ = Task.Run(() => HandleClient(client));
            }
        }

        private async void HandleClient(TcpClient client)
        {
            NetworkStream stream = client.GetStream();
            byte[] buffer = new byte[1024];

            while (true)
            {
                int bytesRead = await stream.ReadAsync(buffer, 0, buffer.Length);
                if (bytesRead == 0) break;

                string message = Encoding.UTF8.GetString(buffer, 0, bytesRead);
                Console.WriteLine($"Received: {message}");
                mess = message;
                // Echo back to client
                byte[] response = Encoding.UTF8.GetBytes($"Server: {message}");
                await stream.WriteAsync(response, 0, response.Length);
            }
        }

        public void StopServer()
        {
            listener.Stop();
            Console.WriteLine("Server stopped.");
        }

        public string ReturnMessageTemp()
        {
            return "Received: " + mess;
        }
        //private string GetLocalIPAddress()
        //{
        //    return Platforms.Android.NetworkHelper.GetLocalIPAddress();
        //}
    }
}
