#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TOKEN_SIZE 10
#define PAWN   "pawn"
#define KNIGHT "knight"
#define ROOK   "rook"
#define QUEEN  "queen"
#define KING   "king"
#define MOVE_SIZE 6
const char CHESS_TOKEN[] = "CHESS";

char* createBlackToken(const char* name)
{
    if (name == NULL) return NULL;

    char* tokenHolder = (char*)malloc(TOKEN_SIZE);
    if (!tokenHolder) return NULL;

    for (int i = 0; i < TOKEN_SIZE; ++i)
    {
        srand(time(0));
        tokenHolder[i] = rand() % 255;
    }
    tokenHolder[TOKEN_SIZE - 1] = '\0';
    return tokenHolder;
}


char* createWhiteToken(const char* previousToken)
{
    if (previousToken == NULL) return NULL;

    char* currentToken = (char*)malloc(TOKEN_SIZE);
    if (!currentToken) return NULL;

    for (int i = 0; i < TOKEN_SIZE; ++i)
    {
        if (i < (int)strlen(CHESS_TOKEN))
            currentToken[i] = CHESS_TOKEN[i];
        else
            currentToken[i] = previousToken[i] + 1;
    }
    currentToken[TOKEN_SIZE - 1] = '\0';
    return currentToken;
}

char* initFirstMove(char* whiteToken)
{
    if (strncmp(whiteToken, CHESS_TOKEN, strlen(CHESS_TOKEN)) != 0)
        return whiteToken;

    int choice;
    printf(
        "0: A King move\n"
        "1: A Queen move\n"
        "2: A Rook move\n"
        "3: A Knight move\n"
        "4: A Pawn move\n"
        "White's turn, enter the first move: ");
    scanf("%d", &choice);

    char* newToken = (char*)malloc(MOVE_SIZE);
    if (!newToken) return NULL;

    switch (choice)
    {
    case(0):
        strncpy(newToken, KING, MOVE_SIZE - 1);
        break;
    case(1):
        strncpy(newToken, QUEEN, MOVE_SIZE - 1);
        break;
    case(2):
        strncpy(newToken, ROOK, MOVE_SIZE - 1);
        break;
    case(3):
        strncpy(newToken, KNIGHT, MOVE_SIZE - 1);
        break;
    case(4):
        strncpy(newToken, PAWN, MOVE_SIZE - 1);
        break;
    default:
        strncpy(newToken, "Invalid", MOVE_SIZE - 1);
    }
    newToken[MOVE_SIZE - 1] = '\0';
    return newToken;
}


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* blackToken = createBlackToken(argv[1]);
    if (blackToken)
    {
        printf("Black's token: %s\n", blackToken);
        char* whiteToken = createWhiteToken(blackToken);
        if (whiteToken)
        {
            char* newToken = initFirstMove(whiteToken);
            free(whiteToken);
            whiteToken = newToken;
            printf("White's move: %s\n", whiteToken);
            free(whiteToken);
        }
        free(blackToken);
    }

    return EXIT_SUCCESS;
}

