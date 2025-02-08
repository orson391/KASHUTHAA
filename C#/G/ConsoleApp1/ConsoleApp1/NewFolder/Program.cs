using System;
using System.Collections.Generic;
using System.Linq;

class Card
{
    public int Rank { get; }
    public int Suit { get; }

    public static readonly string[] Ranks = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
    public static readonly string[] Suits = { "Spades", "Claver", "Diamond", "Heart" };

    public Card(int rank, int suit)
    {
        Rank = rank;
        Suit = suit;
    }

    public override string ToString()
    {
        return $"{Ranks[Rank]} of {Suits[Suit]}";
    }

    public int Value()
    {
        return Rank;
    }
}

class Player
{
    public string Name { get; }
    public List<Card> MyCards { get; }

    public Player(string name)
    {
        Name = name;
        MyCards = new List<Card>();
    }

    public void InsertCard(Card card)
    {
        MyCards.Add(card);
    }

    public Card? ChooseCard()
    {
        if (MyCards.Count == 0)
            return null;

        Console.WriteLine($"\n{Name}, choose a card to play:");
        for (int i = 0; i < MyCards.Count; i++)
        {
            Console.WriteLine($"{i + 1}. {MyCards[i]}");
        }

        while (true)
        {
            Console.Write("Enter the number of the card you want to play: ");
            if (int.TryParse(Console.ReadLine(), out int choice) && choice > 0 && choice <= MyCards.Count)
            {
                Card chosenCard = MyCards[choice - 1];
                MyCards.RemoveAt(choice - 1);
                return chosenCard;
            }
            Console.WriteLine("Invalid choice. Try again.");
        }
    }

    public bool HasCard(int rank, int suit)
    {
        return MyCards.Any(card => card.Rank == rank && card.Suit == suit);
    }

    public void ShowCards()
    {
        Console.WriteLine($"\n{Name}'s Cards ({MyCards.Count} cards):");
        foreach (var card in MyCards)
        {
            Console.WriteLine($" {card}");
        }
    }
}

class Game
{
    private List<Card> deck;
    private List<Player> players;
    private Player? startingPlayer;

    public Game()
    {
        deck = new List<Card>();
        players = new List<Player>
        {
            new Player("Alice"),
            new Player("Bob"),
            new Player("Charlie"),
            new Player("Dave")
        };

        InitializeDeck();
        ShuffleDeck();
        DistributeCards();
        FindStartingPlayer();
        PlayGame();
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
        for (int i = deck.Count - 1; i > 0; i--)
        {
            int j = rng.Next(i + 1);
            (deck[i], deck[j]) = (deck[j], deck[i]); // Swap
        }
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

        foreach (var player in players)
        {
            player.ShowCards();
        }
    }

    private void FindStartingPlayer()
    {
        foreach (var player in players)
        {
            if (player.HasCard(12, 0)) // 'A' of 'Spades'
            {
                startingPlayer = player;
                Console.WriteLine($"\n{startingPlayer.Name} has the 'A of Spades' and will start the game!");
                break;
            }
        }
    }

    private void PlayGame()
    {
        while (players.Any(p => p.MyCards.Count > 0))
        {
            List<(Player, Card)> tableCards = new();
            Player? highestCardPlayer = null;
            Card? highestCard = null;
            bool roundEndedEarly = false;

            List<Player> turnOrder = new(players);
            if (startingPlayer != null)
            {
                turnOrder.Remove(startingPlayer);
                turnOrder.Insert(0, startingPlayer);
            }

            foreach (var player in turnOrder)
            {
                if (player.MyCards.Count > 0)
                {
                    Card? playedCard = player.ChooseCard();
                    if (playedCard != null)
                    {
                        tableCards.Add((player, playedCard));
                        Console.WriteLine($"\n{player.Name} played: {playedCard}");

                        if (highestCard != null && highestCard.Suit != playedCard.Suit)
                        {
                            Console.WriteLine($"\n{player.Name} played a different suit! The round ends immediately.");
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
            }

            Console.WriteLine("\nTable Cards This Round:");
            foreach (var (player, card) in tableCards)
            {
                Console.WriteLine($"{player.Name}: {card}");
            }

            if (roundEndedEarly && highestCardPlayer != null)
            {
                Console.WriteLine($"\n{highestCardPlayer.Name} lost this round (due to a different suit played) and takes all table cards!");
                highestCardPlayer.MyCards.AddRange(tableCards.Select(tc => tc.Item2));
            }

            if (highestCardPlayer != null)
            {
                Console.WriteLine($"\n{highestCardPlayer.Name} is currently holding the highest card: {highestCard}!");
            }

            startingPlayer = highestCardPlayer;

            Console.WriteLine("\nPress Enter for the next round...");
            Console.ReadLine();
        }

        Console.WriteLine("\nGame Over! All cards have been played.");
    }
}

class Program
{
    static void notMain()
    {
        //new Game();
        Console.WriteLine("\nPress 1 for the Client...");
        Console.WriteLine("Press 2 for the Server...");
        string input = Console.ReadLine();

        if (input == "1")
        {
            CardClient cc = new CardClient();
            cc.ClientMain();


        }
        else if (input == "2")
        {
            CardServer cs = new CardServer();
            cs.ServerMain();
        }
        else
        {
            Console.WriteLine("Invalid input. Please restart and enter 1 or 2.");
        }
    }
}
