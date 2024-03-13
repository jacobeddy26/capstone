#include <Arduino.h>

// Define chessboard dimensions
const int BOARD_SIZE = 8;

// Define piece constants
#define EMPTY 0
#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6

// Define piece colors
#define WHITE 0
#define BLACK 1

// Define piece structures
struct Piece {
    int type;
    int color;
};

// Define chessboard
Piece chessboard[BOARD_SIZE][BOARD_SIZE];

// Function to initialize the chessboard
void initializeChessboard() {
    // Place white pieces
    chessboard[0][0] = {ROOK, WHITE};
    chessboard[0][1] = {KNIGHT, WHITE};
    chessboard[0][2] = {BISHOP, WHITE};
    chessboard[0][3] = {QUEEN, WHITE};
    chessboard[0][4] = {KING, WHITE};
    chessboard[0][5] = {BISHOP, WHITE};
    chessboard[0][6] = {KNIGHT, WHITE};
    chessboard[0][7] = {ROOK, WHITE};
    for (int i = 0; i < BOARD_SIZE; i++) {
        chessboard[1][i] = {PAWN, WHITE};
    }

    // Place black pieces
    chessboard[7][0] = {ROOK, BLACK};
    chessboard[7][1] = {KNIGHT, BLACK};
    chessboard[7][2] = {BISHOP, BLACK};
    chessboard[7][3] = {QUEEN, BLACK};
    chessboard[7][4] = {KING, BLACK};
    chessboard[7][5] = {BISHOP, BLACK};
    chessboard[7][6] = {KNIGHT, BLACK};
    chessboard[7][7] = {ROOK, BLACK};
    for (int i = 0; i < BOARD_SIZE; i++) {
        chessboard[6][i] = {PAWN, BLACK};
    }

    // Initialize empty squares
    for (int i = 2; i < 6; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            chessboard[i][j] = {EMPTY, -1};
        }
    }
}

// Function to check if a square is within the board
bool withinBoard(int x, int y) {
    return (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE);
}

// Function to check if there is a piece at the given square
bool isPieceAtSquare(int fileIndex, int rankIndex) {
    return chessboard[rankIndex][fileIndex].type != EMPTY;
}

// Function to add a move to the list of possible moves
void addMove(String* possibleMoves, int* numMoves, int x, int y) {
    possibleMoves[*numMoves] = String((char)('a' + x)) + String(y + 1);
    (*numMoves)++;
}

// Function to get possible moves for a piece at a given square
void get_possible_moves(int fileIndex, int rankIndex, int pieceType, int pieceColor, bool canCastleKingSide, bool canCastleQueenSide, String* possibleMoves, int* numMoves) {
    // Pawn moves
    if (pieceType == PAWN) {
        // Pawns can move one square forward
        int direction = (pieceColor == WHITE) ? 1 : -1; // Define direction based on piece color
        if (withinBoard(fileIndex, rankIndex + direction) && !isPieceAtSquare(fileIndex, rankIndex + direction)) {
            addMove(possibleMoves, numMoves, fileIndex, rankIndex + direction);
        }
        // Pawns can optionally move two squares forward from the starting position
        if ((pieceColor == WHITE && rankIndex == 1) || (pieceColor == BLACK && rankIndex == 6)) {
            if (withinBoard(fileIndex, rankIndex + 2 * direction) && !isPieceAtSquare(fileIndex, rankIndex + direction) && !isPieceAtSquare(fileIndex, rankIndex + 2 * direction)) {
                addMove(possibleMoves, numMoves, fileIndex, rankIndex + 2 * direction);
            }
        }
        // Pawns can capture diagonally
        if (withinBoard(fileIndex - 1, rankIndex + direction) && isPieceAtSquare(fileIndex - 1, rankIndex + direction) && chessboard[rankIndex + direction][fileIndex - 1].color != pieceColor) {
            addMove(possibleMoves, numMoves, fileIndex - 1, rankIndex + direction);
        }
        if (withinBoard(fileIndex + 1, rankIndex + direction) && isPieceAtSquare(fileIndex + 1, rankIndex + direction) && chessboard[rankIndex + direction][fileIndex + 1].color != pieceColor) {
            addMove(possibleMoves, numMoves, fileIndex + 1, rankIndex + direction);
        }
    }
    // Knight moves
    else if (pieceType == KNIGHT) {
        int knightMoves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
        for (int i = 0; i < 8; i++) {
            int newX = fileIndex + knightMoves[i][0];
            int newY = rankIndex + knightMoves[i][1];
            if (withinBoard(newX, newY) && (!isPieceAtSquare(newX, newY) || chessboard[newY][newX].color != pieceColor)) {
                addMove(possibleMoves, numMoves, newX, newY);
            }
        }
    }
    // Bishop moves
    else if (pieceType == BISHOP || pieceType == QUEEN) {
        int bishopDirections[4][2] = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}};
        for (int i = 0; i < 4; i++) {
            for (int j = 1; j < BOARD_SIZE; j++) {
                int newX = fileIndex + bishopDirections[i][0] * j;
                int newY = rankIndex + bishopDirections[i][1] * j;
                if (withinBoard(newX, newY)) {
                    if (!isPieceAtSquare(newX, newY) || chessboard[newY][newX].color != pieceColor) {
                        addMove(possibleMoves, numMoves, newX, newY);
                    } else {
                        if (pieceType != QUEEN) break; // Queen can continue beyond obstacles
                        else break; // If it is QUEEN we allow it to move to the captured position
                    }
                } else {
                    break;
                }
            }
        }
    }
    // Rook moves
    else if (pieceType == ROOK || pieceType == QUEEN) {
        int rookDirections[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
        for (int i = 0; i < 4; i++) {
            for (int j = 1; j < BOARD_SIZE; j++) {
                int newX = fileIndex + rookDirections[i][0] * j;
                int newY = rankIndex + rookDirections[i][1] * j;
                if (withinBoard(newX, newY)) {
                    if (!isPieceAtSquare(newX, newY) || chessboard[newY][newX].color != pieceColor) {
                        addMove(possibleMoves, numMoves, newX, newY);
                    } else {
                        if (pieceType != QUEEN) break; // Queen can continue beyond obstacles
                        else break; // If it is QUEEN we allow it to move to the captured position
                    }
                } else {
                    break;
                }
            }
        }
    }
    // King moves
    else if (pieceType == KING) {
        int kingMoves[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
        for (int i = 0; i < 8; i++) {
            int newX = fileIndex + kingMoves[i][0];
            int newY = rankIndex + kingMoves[i][1];
            if (withinBoard(newX, newY) && (!isPieceAtSquare(newX, newY) || chessboard[newY][newX].color != pieceColor)) {
                addMove(possibleMoves, numMoves, newX, newY);
            }
        }
        // Castling moves...
    }
}

// Function to print possible moves for a piece at a given square
void print_possible_moves(int fileIndex, int rankIndex, int pieceType, int pieceColor, bool canCastleKingSide, bool canCastleQueenSide) {
    String possibleMoves[BOARD_SIZE * BOARD_SIZE]; // Maximum possible moves
    int numMoves = 0;
    get_possible_moves(fileIndex, rankIndex, pieceType, pieceColor, canCastleKingSide, canCastleQueenSide, possibleMoves, &numMoves);

    Serial.print("Possible Moves for ");
    switch (pieceType) {
        case PAWN:
            Serial.print("Pawn");
            break;
        case KNIGHT:
            Serial.print("Knight");
            break;
        case BISHOP:
            Serial.print("Bishop");
            break;
        case ROOK:
            Serial.print("Rook");
            break;
        case QUEEN:
            Serial.print("Queen");
            break;
        case KING:
            Serial.print("King");
            break;
        default:
            Serial.print("Unknown Piece");
    }
    Serial.print(" at square ");
    Serial.print(char('a' + fileIndex));
    Serial.println(char('1' + rankIndex));

    for (int i = 0; i < numMoves; i++) {
        Serial.println("Move: " + possibleMoves[i]);
    }
}

void setup() {
    Serial.begin(115200);
    initializeChessboard();
    // Test the function
    print_possible_moves(1, 0, KNIGHT, WHITE, true, true);  //Knight at b1
    print_possible_moves(0, 0, ROOK, WHITE, true, true);  //Rook at a1
    print_possible_moves(3, 0, QUEEN, WHITE, true, true);  //Queen at d1
    print_possible_moves(1, 1, PAWN, WHITE, true, true);  //Pawn at b2
    print_possible_moves(2, 0, BISHOP, WHITE, true, true);  //Bishop at c1
   // a=0,b=1,c=2,d=3,e=4,f=5,g=6,h=7
   // 0=1,1=2,2=3,3=4,4=5,5=6,6=7,7=8
}

void loop() {
    // Nothing to do here
}
