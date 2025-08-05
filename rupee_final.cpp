#include<iostream>
#include <string>
using namespace std;

class Card {
public:
    string rank;
    string suit;
    Card* next;

    // Constructor to initialize the card
    Card(string rank, string suit) : rank(rank), suit(suit), next(nullptr) {}

    // Method to return card as string
    string toString() const {
        return rank + suit;
    }
};

class LinkedList {
public:
    Card* head;

    LinkedList() : head(nullptr) {}

    void addCard(Card* card) {
        if (!head) {
            head = card;
        }
        else {
            card->next = head;
            head = card;
        }
    }

    Card* drawCard() {
        if (!head) return nullptr;
        Card* card = head;
        head = head->next;
        card->next = nullptr;
        return card;
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    void display() const {
        Card* temp = head;
        while (temp) {
            cout << temp->toString() << " ";
            temp = temp->next;
        }
        cout << endl;
    }

    void sortByRank() {
        if (!head || !head->next) return;

        bool swapped;
        do {
            swapped = false;
            Card* current = head;
            Card* prev = nullptr;
            Card* next = nullptr;

            while (current->next) {
                next = current->next;
                if (LinkedList::rankToInt(current->rank) > LinkedList::rankToInt(next->rank)) {
                    // Swap the nodes
                    if (prev) {
                        prev->next = next;
                    }
                    else {
                        head = next;
                    }
                    current->next = next->next;
                    next->next = current;

                    swapped = true;
                    prev = next;
                }
                else {
                    prev = current;
                    current = current->next;
                }
            }
        } while (swapped);
    }


    //# Converts rank to an integer for sorting
    static int rankToInt(const string& rank) {
        if (rank == "2") return 2;
        if (rank == "3") return 3;
        if (rank == "4") return 4;
        if (rank == "5") return 5;
        if (rank == "6") return 6;
        if (rank == "7") return 7;
        if (rank == "8") return 8;
        if (rank == "9") return 9;
        if (rank == "10") return 10;
        if (rank == "J") return 11;
        if (rank == "Q") return 12;
        if (rank == "K") return 13;
        if (rank == "A") return 14;  //# Ace as highest card in sequence
        return -1;  //# safty
    }

    //# Method to display matches
    void displayMatches() {
        cout << "Matches: ";
        Card* temp = head;
        while (temp) {
            cout << temp->toString() << " ";
            temp = temp->next;
        }
        cout << endl;
    }
};

class Deck : public LinkedList {
public:
    Deck() {
        initializeDeck();
        shuffleDeck();
    }

    //# Initializes the deck with 52 standard cards
    void initializeDeck() {
        string suits[] = { "S", "C", "H", "D" };
        string ranks[] = { "2", "3", "4", "5", "6", "7", "8", "9", "10","J", "Q", "K", "A" };

        for (const string& suit : suits) {
            for (const string& rank : ranks) {
                addCard(new Card(rank, suit));
            }
        }
    }

    //# Shuffles the deck using linked list-based swapping
    void shuffleDeck() {
        int deckSize = size();
        if (deckSize <= 1) return;  //# No need to shuffle a deck with 1 or fewer cards

        srand(static_cast<unsigned>(time(0)));

        for (int i = 0; i < deckSize; ++i) {
            //# Select two random positions in the linked list
            int pos1 = rand() % deckSize;
            int pos2 = rand() % deckSize;
            if (pos1 != pos2) {
                swapNodes(pos1, pos2);
            }
        }
    }

private:
    //# Helper function to swap two nodes at specified positions
    void swapNodes(int pos1, int pos2) {
        if (pos1 == pos2) return;

        // Pointers to nodes and their previous nodes
        Card* prev1 = nullptr;
        Card* prev2 = nullptr;
        Card* node1 = head;
        Card* node2 = head;

        //# Traverse to find nodes at pos1 and pos2
        for (int i = 0; i < pos1; ++i) {
            if (node1 == nullptr) return; //# Ensure node1 is not null
            prev1 = node1;
            node1 = node1->next;
        }
        for (int i = 0; i < pos2; ++i) {
            if (node2 == nullptr) return; //# Ensure node2 is not null
            prev2 = node2;
            node2 = node2->next;
        }

        //# Ensure nodes to be swapped are not null
        if (node1 == nullptr || node2 == nullptr) return;

        if (prev1) prev1->next = node2; else head = node2;
        if (prev2) prev2->next = node1; else head = node1;

        //# Swap next pointers
        Card* temp = node1->next;
        node1->next = node2->next;
        node2->next = temp;
    }

    // Returns the size of the deck (linked list)
    int size() const {
        int count = 0;
        Card* current = head;
        while (current) {
            count++;
            current = current->next;
        }
        return count;
    }
};
class PlayerHand : public LinkedList {
public:
    LinkedList matches; //# New linked list to store matches

    //# Add the extractMatches method to return actual matches
    LinkedList extractMatches() {
        LinkedList extractedMatches;
        Card* current = matches.head;
        while (current) {
            Card* matchCard = current;
            current = current->next;
            matchCard->next = nullptr;
            extractedMatches.addCard(matchCard);
        }
        matches.head = nullptr; //# Clear the matches list
        return extractedMatches;
    }

    Card* drawSpecificCard(const string& rank, const string& suit) {
        Card* current = head;
        Card* previous = nullptr;

        while (current) {
            if (current->rank == rank && current->suit == suit) {
                if (previous) {
                    previous->next = current->next;
                }
                else {
                    head = current->next;
                }
                current->next = nullptr;
                return current;
            }
            previous = current;
            current = current->next;
        }
        return nullptr; // Card not found
    }

    void moveMatchesToPlayerMatches(LinkedList& matchesList) {
        Card* current = matchesList.head;
        while (current) {
            Card* matchCard = current;
            current = current->next;
            matchCard->next = nullptr;
            matches.addCard(matchCard);
        }
    }

    bool hasMatch() {
        // This method is public and checks both match types
        return hasNumericalMatch() || hasSequentialMatch();
    }


    bool hasSequentialMatch() {
        // Create a linked list to store cards by suit
        LinkedList suitLists[4]; // Assuming 4 suits: S, C, H, D
        Card* current = head;

        // Populate the linked lists with cards grouped by suit
        while (current) {
            int suitIndex = suitToIndex(current->suit);
            suitLists[suitIndex].addCard(new Card(current->rank, current->suit));
            current = current->next;
        }

        for (int i = 0; i < 4; ++i) {
            suitLists[i].sortByRank();
            if (checkSequence(suitLists[i])) {
                moveSequentialMatches(suitLists[i]);
                return true;
            }
        }
        return false;
    }

    string matchesToString() const {
        string result;
        Card* current = matches.head;
        while (current) {
            result += current->toString() + " ";
            current = current->next;
        }
        return result;
    }

    void displayMatches() {
        cout << "Matches: " << matchesToString() << endl;
    }

    bool hasNumericalMatch() {
        int rankCount[15] = { 0 };
        Card* current = head;

        while (current) {
            rankCount[rankToInt(current->rank)]++;
            current = current->next;
        }

        for (int i = 2; i <= 14; ++i) {
            if (rankCount[i] >= 3) {
                moveToMatches(intToRank(i));
                return true;
            }
        }
        return false;
    }
    //# Checks for matches suits
    void moveSequentialMatches(LinkedList& suitList) {
        int count = 1;
        Card* current = suitList.head;
        Card* prev = nullptr;

        while (current && current->next) {
            if (rankToInt(current->next->rank) == rankToInt(current->rank) + 1) {
                count++;
                if (count >= 3) {
                    for (int i = 0; i < count; ++i) {
                        Card* matchCard = prev ? prev->next : suitList.head;
                        if (matchCard == nullptr) {
                            // Handle the error or break the loop
                            break;
                        }
                        if (prev) {
                            prev->next = matchCard->next;
                        }
                        else {
                            suitList.head = matchCard->next;
                        }
                        delete matchCard;
                    }

                    return;
                }
            }
            else {
                count = 1;
            }
            prev = current;
            current = current->next;
        }
    }
    //# checks rank
    void moveToMatches(const string& rank) {
        Card* current = head;
        Card* previous = nullptr;

        while (current) {
            if (current->rank == rank) {
                Card* matchCard = current;
                if (previous) {
                    previous->next = current->next;
                }
                else {
                    head = current->next;
                }
                current = current->next;
                matchCard->next = nullptr;
                matches.addCard(matchCard);
            }
            else {
                previous = current;
                current = current->next;
            }
        }
    }

    static bool checkSequence(LinkedList& suitList) {
        int consecutiveCount = 1;
        Card* current = suitList.head;

        while (current && current->next) {
            if (rankToInt(current->next->rank) == rankToInt(current->rank) + 1) {
                consecutiveCount++;
                if (consecutiveCount >= 3) {
                    return true;
                }
            }
            else {
                consecutiveCount = 1;
            }
            current = current->next;
        }
        return false;
    }

    static int suitToIndex(const string& suit) {
        if (suit == "S") return 0;
        if (suit == "C") return 1;
        if (suit == "H") return 2;
        if (suit == "D") return 3;
        return -1; //# in case
    }

    static string intToRank(int rank) {
        switch (rank) {
        case 2: return "2";
        case 3: return "3";
        case 4: return "4";
        case 5: return "5";
        case 6: return "6";
        case 7: return "7";
        case 8: return "8";
        case 9: return "9";
        case 10: return "10";
        case 11: return "J";
        case 12: return "Q";
        case 13: return "K";
        case 14: return "A";
        default: return "";
        }
    }
};

class DiscardRiver : public LinkedList {
public:
    void discard(Card* card) {
        if (!card) return;
        LinkedList::addCard(card);
    }
    Card* drawTop() {
        return drawCard();
    }
    void findCardAndAddToPlayerDeck(const string& rank, const string& suit, PlayerHand& playerHand) {
        Card* current = head;
        Card* previous = nullptr;

        while (current) {
            if (current->rank == rank && current->suit == suit) {
                if (previous) {
                    previous->next = current->next;
                }
                else {
                    head = current->next;
                }
                current->next = nullptr;
                playerHand.addCard(current);
                return;
            }
            previous = current;
            current = current->next;
        }
    }
};
bool isGameEnd(const PlayerHand& player1, const PlayerHand& player2, const Deck& deck) {
    //# Game ends if either player has emptied their hand or if the deck is empty
    return player1.isEmpty() || player2.isEmpty() || deck.isEmpty();
}

int main() {
    Deck deck;
    DiscardRiver discardRiver;

    PlayerHand players[2];
    LinkedList playerMatches[2]; //# Linked lists to store matches for each player

    for (int i = 0; i < 7; i++) {
        players[0].addCard(deck.drawCard());
        players[1].addCard(deck.drawCard());
    }
    discardRiver.addCard(deck.drawCard());
    bool gameEnded = false;
    int currentPlayer = 0;

    while (!gameEnded) {
        PlayerHand& currentPlayerHand = players[currentPlayer];
        PlayerHand& opponentHand = players[1 - currentPlayer];
        LinkedList& currentPlayerMatches = playerMatches[currentPlayer];
        cout << "Player " << (currentPlayer + 1) << "'s turn:" << endl;

        bool hasDrawn = false;
        bool hasDiscarded = false;
        bool turnEnded = false;

        while (!turnEnded) {
            // Print the current state of the discard river, current player's hand, and matches
            cout << "Discard River: ";
            discardRiver.display();
            cout << "Player " << (currentPlayer + 1) << " Hand: ";
            currentPlayerHand.display();

            // Display both players' matches regardless of turn
            cout << "Player 1 Matches: ";
            playerMatches[0].display();
            cout << "Player 2 Matches: ";
            playerMatches[1].display();

            int choice;
            cout << "1) Draw from Deck\n2) Draw from Discard River\n3) Check for Matches\n4) Discard a Card\n5) End Turn" << endl;
            while (!(cin >> choice) || choice < 1 || choice > 5) {
                cout << "Invalid choice. Please select a valid option (1-5): ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

            switch (choice) {
            case 1:
                if (!hasDrawn) {
                    if (!deck.isEmpty()) {
                        Card* drawnCard = deck.drawCard();
                        currentPlayerHand.addCard(drawnCard);
                        cout << "Drawn " << drawnCard->rank << drawnCard->suit << " from Deck" << endl;
                        hasDrawn = true;
                    }
                    else {
                        cout << "Deck is empty! Cannot draw a card." << endl;
                    }
                }
                else {
                    cout << "You have already drawn a card this turn." << endl;
                }
                break;
            case 2:
                if (!discardRiver.isEmpty()) {
                    string cardStr;
                    string rank, suit; //# ran out of time to make this functional, if you see this i will talk to you about it. 
                    cout << "Select a card to draw from the discard river example (10H) Honor rules choose all cards leading up to the card you want: ";
                    cin >> cardStr;
                    if (cardStr.length() > 1) {
                        rank = cardStr.substr(0, cardStr.size() - 1);
                        suit = cardStr.substr(cardStr.size() - 1);
                        discardRiver.findCardAndAddToPlayerDeck(rank, suit, currentPlayerHand);
                        hasDrawn = true;
                    }
                    else {
                        cout << "Invalid card. Please enter a valid card in the format case sensitive! (10H)." << endl;
                    }
                }
                else {
                    cout << "Discard River is empty! Cannot draw a card." << endl;
                }

                break;
            case 3:
                if (currentPlayerHand.hasMatch()) {
                    cout << "You have a match!" << endl;
                    char layDownChoice;
                    while (true) {
                        cout << "Do you want to put your match down (y/n): ";
                        cin >> layDownChoice;
                        if (layDownChoice == 'y' || layDownChoice == 'Y' || layDownChoice == 'n' || layDownChoice == 'N') break;
                        cout << "Invalid choice. Please enter 'y' or 'n'." << endl;
                    }

                    if (layDownChoice == 'y' || layDownChoice == 'Y') {
                        cout << "putting your match down!" << endl;
                        LinkedList matches = currentPlayerHand.extractMatches();
                        while (!matches.isEmpty()) {
                            currentPlayerHand.moveMatchesToPlayerMatches(currentPlayerHand.matches);
                            currentPlayerMatches.addCard(matches.drawCard());
                        }
                        cout << "Matches moved to the match pile." << endl;
                        currentPlayerMatches.display(); // Display the matches after moving them
                    }
                }
                else {
                    cout << "No matches found." << endl;
                }
                break;
            case 4:
                if (!hasDiscarded) {
                    string cardStr;
                    string rank, suit;
                    while (true) {
                        cout << "Select a card to discard (10H): ";
                        cin >> cardStr;
                        if (cardStr.length() > 1) {
                            rank = cardStr.substr(0, cardStr.size() - 1);
                            suit = cardStr.substr(cardStr.size() - 1);
                            Card* cardToDiscard = currentPlayerHand.drawSpecificCard(rank, suit);
                            if (cardToDiscard) {
                                discardRiver.discard(cardToDiscard);
                                cout << "Discarded " << rank << suit << endl;
                                hasDiscarded = true;
                                break;
                            }
                        }
                        cout << "Invalid card. Please enter a valid card in the format case sensitive (10H)." << endl;
                    }
                }
                else {
                    cout << "You have already discarded a card this turn." << endl;
                }
                break;
            case 5:
                turnEnded = true;
                system("cls");

                break;
            default:
                cout << "Invalid choice. Please select a valid option." << endl;
                break;
            }
        }

        //# Check for zero cards left
        gameEnded = isGameEnd(players[0], players[1], deck);

        //# swaps player
        currentPlayer = 1 - currentPlayer;
    }

    //# Tally scores at the end
    cout << "Game Over!" << endl;

    auto calculateScore = [](const LinkedList& matches) -> int {
        int score = 0;
        Card* current = matches.head;
        while (current) {
            int rankValue = LinkedList::rankToInt(current->rank);
            if (rankValue >= 2 && rankValue <= 9) {
                score += 5;
            }
            else if (rankValue >= 10 && rankValue <= 13) {
                score += 10;
            }
            else if (rankValue == 14) { // Ace
                score += 15;
            }
            current = current->next;
        }
        return score;
        };
    //# Calculate points for remaining cards in hand and subtract from total score
    auto calculateHand = [](const PlayerHand& hand) -> int {
        int points = 0;
        Card* current = hand.head;
        while (current) {
            int rankValue = LinkedList::rankToInt(current->rank);
            if (rankValue >= 2 && rankValue <= 9) {
                points += 5;
            }
            else if (rankValue >= 10 && rankValue <= 13) {
                points += 10;
            }
            else if (rankValue == 14) { // Ace
                points += 15;
            }
            current = current->next;
        }
        return points;
        };
    int player1Hand = players[0].isEmpty() ? 0 : calculateHand(players[0]);
    int player2Hand = players[1].isEmpty() ? 0 : calculateHand(players[1]);

    int player1Score = calculateScore(playerMatches[0]);
    int player2Score = calculateScore(playerMatches[1]);

    if (player1Score < player2Score) {
        player1Score -= player1Hand;
    }
    else {
        player2Score -= player2Hand;
    }
    cout << "Player 1 Score: " << player1Score << endl;
    cout << "Player 2 Score: " << player2Score << endl;
    return 0;
}
