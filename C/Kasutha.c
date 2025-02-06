#include <stdio.h>


int value(Card* card);

const char* types[] = { "HEARTS", "CLUBS", "DIAMONDS", "SPADES" };
const char* ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};

typedef struct Card
{
    int id;
    int index;
    const char* myType;
    const char* myRank;
    struct Card* next;
}Card;

int value(Card* card)
{
    switch (*card->myRank)
    {
    case '2':
        return 1;
        break;
    case '3':
        return 2;
        break;
    case '4':
        return 3;
        break;
    case '5':
        return 4;
        break;
    case '6':
        return 5;
        break;
    case '7':
        return 6;
        break;
    case '8':
        return 7;
        break;
    case '9':
        return 8;
        break;
    case '10':
        return 9;
        break;
    case 'J':
        return 10;
        break;
    case 'Q':
        return 11;
        break;
    case 'K':
        return 12;
        break;
    case 'A':
        return 13;
        break;
    default:
        break;
    }
}

typedef struct Player
{
    char name[20];
    Card *myCards;
}Player;

void addCard(Card *hand, char rank, char suit) {
    Card *newCard = (Card *)malloc(sizeof(Card));
    newCard->myRank = rank;
    newCard->myType = suit;
    newCard->next = hand;
    hand = newCard;
}

// Find and remove a specific card
int removeCard(Player *hand, char rank, char suit) {
    Card *curr = hand->myCards, *prev = NULL;

    while (curr != NULL) {
        if (curr->myRank == rank && curr->myType == suit) {
            if (prev) prev->next = curr->next;
            else hand->myCards = curr->next;
            free(curr);
            return 1;
        }
        prev = curr;
        curr = curr->next;
    }
    return 0;
}

// Free memory
void freeHand(Card *hand) {
    Card *curr = hand;
    while (curr) {
        Card *temp = curr;
        curr = curr->next;
        free(temp);
    }
}

void Display(Card *hand) {
    Card *curr = hand;
    while (curr) {
        Card *temp = curr;
        curr = curr->next;
        printf("%s %s\n",temp->myRank,temp->myType);
    }
}



int main() {


    //Player hand = {NULL};

    Card* allCards;
    for (size_t i = 0; i < 4; i++)
    {
       for (size_t j = 0; j < 13; j++)
       {
            addCard(*allCards, *types[i], *ranks[j]);
       }
       
    }
    Display(&allCards);

    //addCard(&hand, 1, 'H');  // Ace of Hearts
    //addCard(&hand, 10, 'S'); // 10 of Spades

    //removeCard(&hand, 1, 'H'); // Remove Ace of Hearts

    freeHand(&allCards);
    return 0;
}
