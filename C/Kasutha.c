#include <stdio.h>
#include <stdlib.h>

// int value(Card* card);

const char *types[] = {"HEARTS", "CLUBS", "DIAMONDS", "SPADES"};
const char *ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};

typedef struct Card
{
    int id;
    int index;
    const char *myType;
    const char *myRank;
    struct Card *next;
} Card;

int value(Card *card)
{
    switch (*card->myRank)
    {
    case 2:
        return 1;
        break;
    case 3:
        return 2;
        break;
    case 4:
        return 3;
        break;
    case 5:
        return 4;
        break;
    case 6:
        return 5;
        break;
    case 7:
        return 6;
        break;
    case 8:
        return 7;
        break;
    case 9:
        return 8;
        break;
    case 10:
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
} Player;


//Initialate Cards

Card *addCard(Card *hand, int rank, int suit)
{
    Card *newCard = (Card *)malloc(sizeof(Card));
    newCard->myRank = ranks[rank];
    newCard->myType = types[suit];
    newCard->next = hand;
    hand = newCard;
    return hand;
}

Card* removeCard(Card *hand, int rank, int suit)
{
    Card *curr = hand, *prev = NULL;
    //Card *temp=NULL;
    while (curr != NULL)
    {
        if (curr->myRank == ranks[rank] && curr->myType == types[suit])
        {
            if (prev)
                prev->next = curr->next;
            else
                hand = curr->next;
            //free(curr);
            return(curr);
            //return 1;
        }
        prev = curr;
        curr = curr->next;
    }
    return 0;
}

void freeHand(Card *hand)
{
    Card *curr = hand;
    while (curr)
    {
        Card *temp = curr;
        curr = curr->next;
        free(temp);
    }
}

void Display(Card *hand)
{
    Card *curr = hand;
    printf("Cards:\n");
    while (curr!= NULL)
    {
        // Card *temp = curr;
        printf("%s %s\n", curr->myRank, curr->myType);
        curr = curr->next;
        
        
    }
    return;
    printf("Done");
}


//Adding To Table

Card *tableAdd(Card *card,int rank,int type,Card *top)
{
    Card *temp = removeCard(card,rank,type);
    temp->next=top;
    top=temp;
    printf("Pushed:%s %s\n",top->myRank,top->myType);
    return top;
}

void tableDisplay(Card*top)
{
    if (top == NULL)
    {
        printf("Overflow\n");
    }
    else
    { 
        Card * ptr = top;
        printf("Table:\n");
        while (ptr!= NULL)
        {

            //printf("%d\n",ptr->data);
            printf("%s %s\n",ptr->myRank,ptr->myType);
            ptr = ptr->next;
        }
        printf("\n");
        
    }
}

Card* tablePop(Card* top)
{
    if (top == NULL)
    {
        printf("Overflow\n");
    }
    else
    {
        Card* temp = top;
        top = top->next;
        printf("Poped:%s %s\n",temp->myRank,temp->myType);
        //free(temp);
        return temp;
    }
    
}


//Adding TO playerCards
Card *playerCardAdd(Card *allcard,int rank,int type,Card *playerCard)
{
    Card* temp = removeCard(allcard,rank,type);
    if (playerCard == NULL)
    {
        playerCard = temp;
    }
    else
    {
        Card* ptr = playerCard;
        playerCard = temp;
        playerCard->next = ptr;
    }
    return playerCard;
}

void playerCardDisplay(Card* pcard)
{
    Card*ptr = pcard;
    if (ptr == NULL)
    {
        printf("EmptyPcards\n");
    }
    else
    {
        printf("Players Cards:\n");
        while (ptr!=NULL)
        {
            printf("%s %s\n",ptr->myRank,ptr->myType);
            ptr = ptr->next;
        }
        
    }
    
}

// void playerCardToTable(int pre)
// {
//     if(Head == NULL)
//     {
//         printf("Its Empty\n");
//        // exit(0);
//     }
//     else
//     {
//         Node* preptr = Head;
//         while (preptr->data != pre)
//         {
//             preptr = preptr->link;
//             if (preptr== NULL)
//             {
//                 printf("No Data Found\n");
//                 break;
//                 //exit(0);
//             }
//         }
//         Node* temp = preptr->link;
//         Node* ptr = temp->link;
//         preptr->link = ptr;
//         free(temp);
//     }
//     printf("Deleted Between\n");
// }

int main()
{

    printf("Helo\n");
    // Player hand = {NULL};

    Card *allCards = NULL;

    Card *tableCards = NULL;

    for (size_t j = 0; j < 13; j++)
    {
        for (size_t k = 0; k < 4; k++)
        {
            allCards = addCard(allCards, j, k);
            /* code */
        }
    }
    Display(allCards);

    //printf("TableCards:\n");
    //tableCards = tableAdd(allCards,0,0,tableCards);
    //tableCards = tableAdd(allCards,0,1,tableCards);
    //tableDisplay(tableCards);

    // addCard(&hand, 1, 'H');  // Ace of Hearts
    // addCard(&hand, 10, 'S'); // 10 of Spades

    // removeCard(&hand, 1, 'H'); // Remove Ace of Hearts

    Player p1;
    printf("Enter a Name:");
    scanf("%s",&p1.name);
    p1.myCards = NULL;
    p1.myCards = playerCardAdd(allCards,0,0,p1.myCards);
    playerCardDisplay(p1.myCards);
    tableCards = tableAdd(p1.myCards,0,0,tableCards);
    tableDisplay(tableCards);
    freeHand(allCards);
    printf("Done");

    return 0;
}
