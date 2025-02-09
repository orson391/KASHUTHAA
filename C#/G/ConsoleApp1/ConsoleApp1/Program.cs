using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Numerics;
namespace ConsoleApp1
{

    class CardServer
    {
        private TcpListener listener;
        private List<TcpClient> clients = new();
        private List<Player> players = new();
        private Queue<Player> turnOrder = new();
        private Dictionary<TcpClient, Player> clientPlayerMap = new();
        private List<Card> deck = new();
        private static Dictionary<TcpClient, string> playerNames = new Dictionary<TcpClient, string>();

        public CardServer()
        {
            Console.WriteLine("Enter number of players");
            int pl = int.Parse(Console.ReadLine());
            listener = new TcpListener(IPAddress.Any, 5555);
            //listener = new TcpListener(IPAddress.Parse("192.168."), 5555);
            listener.Start();
            Console.WriteLine("Server started, waiting for players...");

            AcceptClients(pl);
            InitializeGame();
        }

        private void AcceptClients(int pl)
        {
            while (clients.Count < pl)  // Accept 4 players
            {
                TcpClient client = listener.AcceptTcpClient();
                clients.Add(client);
                Console.WriteLine($"Player {clients.Count} connected.");
                //SendToClient(client, "Enter a Name :");
                string name = ReceiveFromClient(client);
                //Thread thread = new Thread(() => HandleClient(client));
                Player player = new($"Player {name}");
                Console.WriteLine($"Players:{name}");
                //Player player = new($"Player {clients.Count}");
            
                players.Add(player);
                clientPlayerMap[client] = player;
            }
            foreach (var (client, player) in clientPlayerMap)
            {


                foreach (var playerName in players)
                {
                    SendToClient(client, playerName.Name);
                }
                    
                
            }

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

        private void InitializeGame()
        {
            InitializeDeck();
            ShuffleDeck();
            DistributeCards();
            StartGame();
        }

        private void InitializeDeck()
        {
            for (int suit = 0; suit < Card.Suits.Length; suit++)
            {
                for (int rank = 0; rank < Card.Ranks.Length; rank++)
                {
                    deck.Add(new Card(rank, suit));
                }
            }
        }

        private void ShuffleDeck()
        {
            Random rng = new Random();
            deck = deck.OrderBy(_ => rng.Next()).ToList();
        }

        private void DistributeCards()
        {
            int index = 0;
            while (index < deck.Count)
            {
                foreach (var player in players)
                {
                    if (index < deck.Count)
                    {
                        player.InsertCard(deck[index]);
                        index++;
                    }
                }
            }

            foreach (var (client, player) in clientPlayerMap)
            {
                int k = player.MyCards.Count;
                string kk = "Your Cards:\n";

                for (int i = 0; i < k; i++)
                {
                    kk += (i + 1) + ") " + player.MyCards[i] + "\n";
                }

                SendToClient(client, kk);
            }

        }

        public bool HasCard(List<Card> MyCards,int rank, int suit)
        {
            return MyCards.Any(card => card.Rank == rank && card.Suit == suit);
        }

        private Player FindStartingPlayer(List<Player> players)
        {
            foreach (var player in players)
            {
                if (player.HasCard(12, 0)) // 'A' of 'Spades'
                {
                    return player;
                }
            }
            return null;
        }
        private void StartGame()
        {
            Player startingPlayer = FindStartingPlayer(players);
            while (players.Any(p => p.MyCards.Count > 0))
            {
                List<(Player, Card)> tableCards = new();
                Player? highestCardPlayer = null;
                Card? highestCard = null;
                bool roundEndedEarly = false;

                // Determine turn order
                List<Player> turnOrder = new(players);
                if (startingPlayer != null)
                {
                    turnOrder.Remove(startingPlayer);
                    turnOrder.Insert(0, startingPlayer);
                }

                foreach (var player in turnOrder)
                {
                    if (player.MyCards.Count == 0) continue; // Skip players with no cards

                    TcpClient client = clientPlayerMap.FirstOrDefault(x => x.Value == player).Key;
                    SendToClient(client, "Your turn! Choose a card (Enter card index):");

                    string response = ReceiveFromClient(client);
                    if (int.TryParse(response, out int choice) && choice > 0 && choice <= player.MyCards.Count)
                    {
                        Card playedCard = player.MyCards[choice - 1];
                        tableCards.Add((player, playedCard));
                        player.MyCards.RemoveAt(choice - 1);
                        Broadcast($"{player.Name} played: {playedCard}");

                        if (highestCard != null && highestCard.Suit != playedCard.Suit)
                        {
                            Broadcast($"\n{player.Name} played a different suit! The round ends immediately.");
                            roundEndedEarly = true;
                            break;
                        }

                        if (highestCard == null || playedCard.Value() > highestCard.Value())
                        {
                            highestCard = playedCard;
                            highestCardPlayer = player;
                        }
                    }
                }

                // Show table cards
                Broadcast("\nTable Cards This Round:");
                foreach (var (player, card) in tableCards)
                {
                    Broadcast($"{player.Name}: {card}");
                }

                // If round ended early, assign all table cards to the losing player
                if (roundEndedEarly && highestCardPlayer != null)
                {
                    Broadcast($"\n{highestCardPlayer.Name} lost this round and takes all table cards!");
                    highestCardPlayer.MyCards.AddRange(tableCards.Select(tc => tc.Item2));
                }

                if (highestCardPlayer != null)
                {
                    Broadcast($"\n{highestCardPlayer.Name} is currently holding the highest card: {highestCard}!");
                }

                startingPlayer = highestCardPlayer;

                // Send updated card list to all players
                foreach (var (clientk, player) in clientPlayerMap)
                {
                    string cardList = "Your Cards:\n";
                    for (int i = 0; i < player.MyCards.Count; i++)
                    {
                        cardList += (i + 1) + ") " + player.MyCards[i] + "\n";
                    }
                    SendToClient(clientk, cardList);
                }

                Broadcast("\nThe next round...");
                //ReceiveFromClient(clientPlayerMap.First().Key); // Wait for confirmation from any client
            }

            Broadcast("\nGame Over! All cards have been played.");

        }

        private void SendToClient(TcpClient client, string message)
        {
            byte[] data = Encoding.UTF8.GetBytes(message + "\n");
            client.GetStream().Write(data, 0, data.Length);
        }

        private string ReceiveFromClient(TcpClient client)
        {
            byte[] buffer = new byte[1024];
            int bytesRead = client.GetStream().Read(buffer, 0, buffer.Length);
            return Encoding.UTF8.GetString(buffer, 0, bytesRead).Trim();
        }

        private void Broadcast(string message)
        {
            foreach (var client in clients)
            {
                SendToClient(client, message);
            }
        }
    }


    class CardClient
    {
        private TcpClient client;
        private NetworkStream stream;

        public CardClient()
        {
            Console.Write("Enter your name: ");
            string name = Console.ReadLine();

            try
            {
                client = new TcpClient("141.95.55.255", 5555);
                stream = client.GetStream();
                Console.WriteLine("Connected to server.");
                System.Threading.Thread.Sleep(5000);

                SendToServer(name);
                ListenForServerMessages();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error connecting to server: {ex.Message}");
            }
        }


        private void ListenForServerMessages()
        {
            byte[] buffer = new byte[1024];
            


            while (true)
            {
                int bytesRead = stream.Read(buffer, 0, buffer.Length);
                string message = Encoding.UTF8.GetString(buffer, 0, bytesRead).Trim();
                Console.WriteLine(message);
                //if (message.Contains("Enter a Name"))
                //{
                    
                  //  string choice = Console.ReadLine();
                    //SendToServer(choice);
                //}
                if (message.Contains("Choose a card"))
                {
                    Console.Write("Enter your choice: ");
                    string choice = Console.ReadLine();
                    SendToServer(choice);
                }
                

            }
        }

        private void SendToServer(string message)
        {
            byte[] data = Encoding.UTF8.GetBytes(message);
            stream.Write(data, 0, data.Length);
        }
    }


    internal class Program
    {
        static void Main()
        {
            //new Game();
            Console.WriteLine("\nPress 1 for the Client...");
            Console.WriteLine("Press 2 for the Server...");
            string input = Console.ReadLine();

            if (input == "1")
            {
                CardClient cc = new CardClient();
                //cc.();


            }
            else if (input == "2")
            {
                CardServer cs = new CardServer();
                //cs.ServerMain();
            }
            else
            {
                Console.WriteLine("Invalid input. Please restart and enter 1 or 2.");
            }
        }
    }
}
