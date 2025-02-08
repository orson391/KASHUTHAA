using System;
using System.Net.Sockets;
using System.Text;

class CardClient
{
    public void ClientMain()
    {
        TcpClient client = new TcpClient("127.0.0.1", 5000);
        NetworkStream stream = client.GetStream();

        Console.Write("Enter your name: ");
        string name = Console.ReadLine();
        byte[] nameData = Encoding.UTF8.GetBytes(name);
        stream.Write(nameData, 0, nameData.Length);

        byte[] buffer = new byte[1024];
        int bytesRead = stream.Read(buffer, 0, buffer.Length);
        string response = Encoding.UTF8.GetString(buffer, 0, bytesRead);
        Console.WriteLine(response);

        // Handle game updates here (e.g., receiving cards, playing turns)
    }
}
