import random

ranks = [2, 3, 4, 5, 6, 7, 8, 9, 10, 'J', 'Q', 'K', 'A']
types = ['Spades', 'Claver', 'Diamond', 'Heart']

class Card:
    def __init__(self, rank, suit):  
        self.rank = rank
        self.suit = suit
    
    def __str__(self):
        return f'{ranks[self.rank]} of {types[self.suit]}'

    def value(self):
        """Returns the value of the card for comparisons"""
        return self.rank  

class Player: 
    def __init__(self, name):  
        self.name = name
        self.mycards = []  

    def insertcard(self, card):
        """Adds a card to the player's hand"""
        self.mycards.append(card)

    def choose_card(self):
        """Allows the player to choose a card to play"""
        if not self.mycards:
            return None  

        print(f"\n{self.name}, choose a card to play:")
        for i, card in enumerate(self.mycards):
            print(f"{i + 1}. {card}")

        while True:
            try:
                choice = int(input("Enter the number of the card you want to play: ")) - 1
                if 0 <= choice < len(self.mycards):
                    return self.mycards.pop(choice)
                else:
                    print("Invalid choice. Try again.")
            except ValueError:
                print("Invalid input. Enter a number.")

    def has_card(self, rank, suit):
        """Checks if the player has a specific card"""
        return any(card.rank == rank and card.suit == suit for card in self.mycards)

    def showcards(self):
        """Displays all cards the player has"""
        print(f"\n{self.name}'s Cards ({len(self.mycards)} cards):")
        for card in self.mycards:
            print(f" {card}")

# Create deck (52 cards) and shuffle
deck = [Card(rank, suit) for rank in range(len(ranks)) for suit in range(len(types))]
random.shuffle(deck)

# Create players
players = [Player("Alice"), Player("Bob"), Player("Charlie"), Player("Dave")]

# Distribute all 52 cards in a round-robin manner
while deck:
    for player in players:
        if deck:  
            player.insertcard(deck.pop())

# Show each player's cards
for player in players:
    player.showcards()

# Find the player who has "A of Spades"
starting_player = None
for player in players:
    if player.has_card(12, 0):  # 12 is the index of 'A', 0 is 'Spades'
        starting_player = player
        break

if starting_player:
    print(f"\n{starting_player.name} has the 'A of Spades' and will start the game!")

# Game loop (rounds continue until all players are out of cards)
while any(player.mycards for player in players):
    tablecards = []
    highest_card_player = None
    highest_card = None
    round_ended_early = None
    # Play a round, starting with the first player
    current_order = players[:]  # Copy player order
    if starting_player in current_order:
        current_order.remove(starting_player)
        current_order.insert(0, starting_player)  # Ensure starting player goes first

    for player in current_order:
        if player.mycards:
            played_card = player.choose_card()
            if played_card:
                tablecards.append((player, played_card))
                print(f"\n{player.name} played: {played_card}")

                if highest_card is not None and highest_card.suit != played_card.suit:
                    print(f"\n{player.name} played a different suit! The round ends immediately.")
                    round_ended_early = True
                    break 

                # **Track highest card dynamically**
                if highest_card is None or played_card.value() > highest_card.value():
                    highest_card = played_card
                    highest_card_player = player

    print("\nTable Cards This Round:")
    for name, card in tablecards:
        print(f"{name.name}: {card}")

    if round_ended_early:
        print(f"\n{highest_card_player.name} Losts the round (due to different suit played) and takes all table cards!")
        highest_card_player.mycards.extend([card for _, card in tablecards])
    
    # **Declare winner of this round immediately**
    if highest_card_player:
        print(f"\n{highest_card_player.name} is currently with Largestcard:  {highest_card}!")

    # Set the winner of this round as the next starting player
    starting_player = highest_card_player

    input("\nPress Enter for the next round...")  

print("\nGame Over! All cards have been played.")
