#include <Arduino.h>

// Function to check if a piece is lifted from the board
bool isPieceLifted(int row, int col) {
    // Implement your code to check if a piece is lifted from the sensor at the specified row and col
    // Return true if a piece is lifted, false otherwise
    return digitalRead(row * 8 + col) == LOW;
}

// Function to encode chess move from lifted piece and new touch inputs
void encodeChessMove(char* chessMove) {
    // Check for lifted piece (source square)
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (isPieceLifted(row, col)) {
                chessMove[0] = 'a' + col;
                chessMove[1] = '1' + row;
            }
        }
    }

    // Check for a new touch input (destination square)
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (digitalRead(row * 8 + col) == HIGH) {
                chessMove[2] = 'a' + col;
                chessMove[3] = '1' + row;
            }
        }
    }
}

void setup() {
    Serial.begin(9600);
}

void loop() {
    // Initialize the chess move string
    char chessMove[5] = {'\0'};

    // Call the function to encode chess move from lifted piece and new touch inputs
    encodeChessMove(chessMove);

    // Check if a valid chess move is detected
    if (chessMove[0] != '\0' && chessMove[2] != '\0') {
        // Print the detected chess move
        Serial.print("Chess move: ");
        Serial.println(chessMove);
    }

    delay(100);  // Adjust delay for performance
}
