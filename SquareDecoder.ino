#include <Arduino.h>

typedef struct {
    char file;
    int rank;
} ChessSquare;

typedef struct {
    char label;
    int measurement;
} SquareMeasurement;

// Function to convert chess square to coordinates
void chessSquareToCoordinates(ChessSquare chess_square, int* x, int* y) {
    // Define the predefined measurements for each square
    SquareMeasurement square_measurements[] = {
        {'a', 0}, {'b', 2}, {'c', 4}, {'d', 6}, {'e', 8}, {'f', 10}, {'g', 12}, {'h', 14},
        {'1', 0}, {'2', 2}, {'3', 4}, {'4', 6}, {'5', 8}, {'6', 10}, {'7', 12}, {'8', 14}
    };

    // Find measurements corresponding to the file and rank
    int file_measurement = 0;
    int rank_measurement = 0;

    for (int i = 0; i < sizeof(square_measurements) / sizeof(SquareMeasurement); i++) {
        if (chess_square.file == square_measurements[i].label) {
            file_measurement = square_measurements[i].measurement;
        }
        if (chess_square.rank == square_measurements[i].label) {
            rank_measurement = square_measurements[i].measurement;
        }
    }

    // Map file and rank to x, y coordinates using predefined measurements
    *x = file_measurement;
    *y = rank_measurement;
}

void setup() {
    Serial.begin(9600);
}

void loop() {
    // Example Usage:
    ChessSquare chess_square = {'b',  '2'};
    int x, y;

    // Call the function to get coordinates
    chessSquareToCoordinates(chess_square, &x, &y);

    // Print the coordinates
    Serial.print("The coordinates for square ");
    Serial.print(chess_square.file);
    Serial.print(chess_square.rank);
    Serial.print(" are: (");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.println(")");

    delay(1000);  // Delay for readability, adjust as needed
}
