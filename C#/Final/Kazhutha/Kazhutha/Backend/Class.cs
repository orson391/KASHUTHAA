using Microsoft.AspNetCore.SignalR;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Kazhutha.Backend
{
    public class GameHub : Hub
    {
        private static List<Player> players = new List<Player>();
        private static Dictionary<string, Card> tableCards = new Dictionary<string, Card>();

        public async Task JoinGame(string playerName)
        {
            var player = new Player { Name = playerName, ConnectionId = Context.ConnectionId };
            players.Add(player);
            await Clients.All.SendAsync("UpdatePlayers", players);
        }

        public async Task PlayCard(string playerName, int rank, string suit)
        {
            var playedCard = new Card(rank, suit);
            tableCards[playerName] = playedCard;

            await Clients.All.SendAsync("UpdateTable", tableCards);
        }
    }

    public class Player
    {
        public string Name { get; set; }
        public string ConnectionId { get; set; }
    }

    public class Card
    {
        public int Rank { get; set; }
        public string Suit { get; set; }

        public Card(int rank, string suit)
        {
            Rank = rank;
            Suit = suit;
        }
    }
}
