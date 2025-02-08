using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

class CardServer
{
    private static List<TcpClient> clients = new List<TcpClient>();
    private static Dictionary<TcpClient, string> playerNames = new Dictionary<TcpClient, string>();

    public void ServerMain()
    {
        TcpListener server = new TcpListener(IPAddress.Any, 5000);
        server.Start();
        Console.WriteLine("Server started... Waiting for players...");

        while (clients.Count < 4)  // Accept up to 4 players
        {
            TcpClient client = server.AcceptTcpClient();
            clients.Add(client);
            Console.WriteLine("A player connected!");
            Thread thread = new Thread(() => HandleClient(client));
            thread.Start();
        }

        Console.WriteLine("All players connected! Starting the game...");
        StartGame();
    }

    private static void HandleClient(TcpClient client)
    {
        NetworkStream stream = client.GetStream();
        byte[] buffer = new byte[1024];
        int bytesRead = stream.Read(buffer, 0, buffer.Length);
        string playerName = Encoding.UTF8.GetString(buffer, 0, bytesRead);
        playerNames[client] = playerName;

        Console.WriteLine($"{playerName} joined the game.");
    }

    private static void StartGame()
    {
        foreach (var client in clients)
        {
            SendMessage(client, "Game is starting! Your cards will be sent soon.");
        }
        // Implement game logic (shuffling, distributing cards, handling turns, etc.)
    }

    private static void SendMessage(TcpClient client, string message)
    {
        NetworkStream stream = client.GetStream();
        byte[] data = Encoding.UTF8.GetBytes(message);
        stream.Write(data, 0, data.Length);
    }
}
