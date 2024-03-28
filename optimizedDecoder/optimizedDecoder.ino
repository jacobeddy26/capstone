#include <Wire.h>
#include <Servo.h>

// Define the home position for the arm
const int homePosition[2] = {0, 0}; // { x-coordinate, y-coordinate }

// Define stepper motor control pins
const uint8_t stepPin1 = 4; // Stepper motor step pin
const uint8_t dirPin1 = 5;  // Stepper motor direction pin
const uint8_t stepPin2 = 6; // Stepper motor step pin
const uint8_t dirPin2 = 7;  // Stepper motor direction pin

// Define steps per revolution for the stepper motors
const uint16_t stepsPerRevolution = 360 / 0.35;

// Define the electromagnet and servo pins
const uint8_t electromagnetPin = 40;
const uint8_t servoPin = 38;

// Define the resting and maximum piece height
const uint8_t RESTING_HEIGHT = 100;
const uint8_t MAX_PIECE_HEIGHT = 56;

// Define the heights of each type of chess piece
const uint8_t PIECE_HEIGHTS[] = {41, 34, 20, 28, 24, 19};

// Define move received from I2C
char receivedMove[6];

// Define individual move components (row, column) i.e. "a2b3" = "a2" & "b3"
char srcX, srcY, destX, destY;

// Define the Gripper class
class Gripper {
private:
    Servo servo;
    int previousZ;

public:
    Gripper() : previousZ(-1) {
        pinMode(servoPin, OUTPUT);
        pinMode(electromagnetPin, OUTPUT);
    }

    void calibrate() {
        move(RESTING_HEIGHT);
    }

    void move(uint8_t z) {
        z = constrain(z, 0, 100);
        uint8_t dc = (z * 0.067) + 4;
        servo.writeMicroseconds(dc * 100);
        int t = (previousZ == -1) ? 1000 : abs(previousZ - z) / 10 + 500;
        delay(t);
        servo.detach();
        previousZ = z;
    }

    void electromagnet(bool on) {
        digitalWrite(electromagnetPin, on ? HIGH : LOW);
    }

    void pickup(uint8_t pieceType) {
        uint8_t pieceHeight = PIECE_HEIGHTS[pieceType];
        move(pieceHeight);
        delay(400);
        electromagnet(true);
        delay(200);
        move(RESTING_HEIGHT + pieceHeight);
    }

    void dropoff(uint8_t pieceType) {
        uint8_t pieceHeight = PIECE_HEIGHTS[pieceType];
        move(pieceHeight);
        delay(200);
        electromagnet(false);
        delay(400);
        move(RESTING_HEIGHT);
    }

    void cleanup() {
        digitalWrite(electromagnetPin, LOW);
        digitalWrite(servoPin, LOW);
    }
};

Gripper gripper;

// Define struct scaraAngle
struct scaraAngle {
    const char *square;
    double theta1;
    double theta2;
};

// Class to represent a chessboard square
class ChessboardSquare {
private:
    scaraAngle angles;

public:
    // Constructors
    ChessboardSquare() : angles({nullptr, 0.0, 0.0}) {}
    ChessboardSquare(const char *square, double theta1, double theta2) : angles({square, theta1, theta2}) {}

    // Getter for theta1
    double getTheta1() const {
        return angles.theta1;
    }

    // Getter for theta2
    double getTheta2() const {
        return angles.theta2;
    }

    // Getter for square name
    const char *getName() const {
        return angles.square;
    }
};

// Class to represent a chessboard with squares
class Chessboard {
private:
    ChessboardSquare squares[8][8];

public:
    // Constructor
    Chessboard(const scaraAngle hardcodedAngles[8][8][3]) {
        // Initialize angles for each square on the chessboard
        for (uint8_t row = 0; row < 8; ++row) {
            for (uint8_t col = 0; col < 8; ++col) {
                squares[row][col] = ChessboardSquare(hardcodedAngles[row][col]->square, hardcodedAngles[row][col]->theta1, hardcodedAngles[row][col]->theta2);
            }
        }
    }

    // Getter for accessing a square by its position
    ChessboardSquare &getSquare(char file, uint8_t rank) {
        uint8_t col = file - 'a';
        uint8_t row = 8 - rank;
        return squares[row][col];
    }
};

// Hardcoded names and angle values for each chessboard position

const scaraAngle hardcodedAngles[8][8][3] = {
    {{"h1", 1.0, 1.0}, {"h2", 1.0, 2.0}, {"h3", 1.0, 3.0}, {"h4", 1.0, 4.0}, {"h5", 1.0, 5.0}, {"h6", 1.0, 6.0}, {"h7", 1.0, 7.0}, {"h8", 1.0, 1.0}},
    {{"g1", 2.0, 1.0}, {"g2", 2.0, 2.0}, {"g3", 2.0, 3.0}, {"g4", 2.0, 4.0}, {"g5", 2.0, 5.0}, {"g6", 2.0, 6.0}, {"g7", 2.0, 7.0}, {"g8", 2.0, 8.0}},
    {{"f1", 3.0, 1.0}, {"f2", 3.0, 2.0}, {"f3", 3.0, 3.0}, {"f4", 3.0, 4.0}, {"f5", 3.0, 5.0}, {"f6", 3.0, 6.0}, {"f7", 3.0, 7.0}, {"f8", 3.0, 8.0}},
    {{"e1", 4.0, 1.0}, {"e2", 4.0, 2.0}, {"e3", 4.0, 3.0}, {"e4", 4.0, 4.0}, {"e5", 4.0, 5.0}, {"e6", 4.0, 6.0}, {"e7", 4.0, 7.0}, {"e8", 4.0, 8.0}},
    {{"d1", 5.0, 1.0}, {"d2", 5.0, 2.0}, {"d3", 5.0, 3.0}, {"d4", 5.0, 4.0}, {"d5", 5.0, 5.0}, {"d6", 5.0, 6.0}, {"d7", 5.0, 7.0}, {"d8", 5.0, 8.0}},
    {{"c1", 6.0, 1.0}, {"c2", 6.0, 2.0}, {"c3", 6.0, 3.0}, {"c4", 6.0, 4.0}, {"c5", 6.0, 5.0}, {"c6", 6.0, 6.0}, {"c7", 6.0, 7.0}, {"c8", 6.0, 8.0}},
    {{"b1", 7.0, 1.0}, {"b2", 7.0, 2.0}, {"b3", 7.0, 3.0}, {"b4", 7.0, 4.0}, {"b5", 7.0, 5.0}, {"b6", 7.0, 6.0}, {"b7", 7.0, 7.0}, {"b8", 7.0, 8.0}},
    {{"a1", 1.0, 1.0}, {"a2", 8.0, 2.0}, {"a3", 8.0, 3.0}, {"a4", 8.0, 4.0}, {"a5", 8.0, 5.0}, {"a6", 8.0, 6.0}, {"a7", 8.0, 7.0}, {"a8", 8.0, 8.0}}
};

Chessboard board(hardcodedAngles);

ChessboardSquare &a1 = board.getSquare('a', 1);ChessboardSquare &a2 = board.getSquare('a', 2); ChessboardSquare &a3 = board.getSquare('a', 3);ChessboardSquare &a4 = board.getSquare('a', 4);
ChessboardSquare &a5 = board.getSquare('a', 5);ChessboardSquare &a6 = board.getSquare('a', 6);ChessboardSquare &a7 = board.getSquare('a', 7);ChessboardSquare &a8 = board.getSquare('a', 8);
ChessboardSquare &b1 = board.getSquare('b', 1);ChessboardSquare &b2 = board.getSquare('b', 2);ChessboardSquare &b3 = board.getSquare('b', 3);ChessboardSquare &b4 = board.getSquare('b', 4);
ChessboardSquare &b5 = board.getSquare('b', 5);ChessboardSquare &b6 = board.getSquare('b', 6);ChessboardSquare &b7 = board.getSquare('b', 7);ChessboardSquare &b8 = board.getSquare('b', 8);
ChessboardSquare &c1 = board.getSquare('c', 1);ChessboardSquare &c2 = board.getSquare('c', 2);ChessboardSquare &c3 = board.getSquare('c', 3);ChessboardSquare &c4 = board.getSquare('c', 4);
ChessboardSquare &c5 = board.getSquare('c', 5);ChessboardSquare &c6 = board.getSquare('c', 6);ChessboardSquare &c7 = board.getSquare('c', 7);ChessboardSquare &c8 = board.getSquare('c', 8);
ChessboardSquare &d1 = board.getSquare('d', 1);ChessboardSquare &d2 = board.getSquare('d', 2);ChessboardSquare &d3 = board.getSquare('d', 3);ChessboardSquare &d4 = board.getSquare('d', 4);
ChessboardSquare &d5 = board.getSquare('d', 5);ChessboardSquare &d6 = board.getSquare('d', 6);ChessboardSquare &d7 = board.getSquare('d', 7);ChessboardSquare &d8 = board.getSquare('d', 8);
ChessboardSquare &e1 = board.getSquare('e', 1);ChessboardSquare &e2 = board.getSquare('e', 2);ChessboardSquare &e3 = board.getSquare('e', 3);ChessboardSquare &e4 = board.getSquare('e', 4);
ChessboardSquare &e5 = board.getSquare('e', 5);ChessboardSquare &e6 = board.getSquare('e', 6);ChessboardSquare &e7 = board.getSquare('e', 7);ChessboardSquare &e8 = board.getSquare('e', 8);
ChessboardSquare &f1 = board.getSquare('f', 1);ChessboardSquare &f2 = board.getSquare('f', 2);ChessboardSquare &f3 = board.getSquare('f', 3);ChessboardSquare &f4 = board.getSquare('f', 4);
ChessboardSquare &f5 = board.getSquare('f', 5);ChessboardSquare &f6 = board.getSquare('f', 6);ChessboardSquare &f7 = board.getSquare('f', 7);ChessboardSquare &f8 = board.getSquare('f', 8);
ChessboardSquare &g1 = board.getSquare('g', 1);ChessboardSquare &g2 = board.getSquare('g', 2);ChessboardSquare &g3 = board.getSquare('g', 3);ChessboardSquare &g4 = board.getSquare('g', 4);
ChessboardSquare &g5 = board.getSquare('g', 5);ChessboardSquare &g6 = board.getSquare('g', 6);ChessboardSquare &g7 = board.getSquare('g', 7);ChessboardSquare &g8 = board.getSquare('g', 8);
ChessboardSquare &h1 = board.getSquare('h', 1);ChessboardSquare &h2 = board.getSquare('h', 2);ChessboardSquare &h3 = board.getSquare('h', 3);ChessboardSquare &h4 = board.getSquare('h', 4);
ChessboardSquare &h5 = board.getSquare('h', 5);ChessboardSquare &h6 = board.getSquare('h', 6);ChessboardSquare &h7 = board.getSquare('h', 7);ChessboardSquare &h8 = board.getSquare('h', 8);

void setup() {
    Serial.begin(115200);
}

void loop() {
    Serial.print("Square ");
    Serial.print(a1.getName());
    Serial.print(": theta1 = ");
    Serial.print(a1.getTheta1());
    Serial.print(", theta2 = ");
    Serial.println(a1.getTheta2());

    delay(2000);
}

// Function to convert chess notation to source and destination squares
void parseChessMove(char move[6], char &srcX, char &srcY, char &destX, char &destY) {
    srcX = move[1];  // Convert column character to index
    srcY = move[2];  // Convert row character to index
    destX = move[3]; // Convert column character to index
    destY = move[4]; // Convert row character to index
}

// Function to receive data over I2C
void receiveEvent() {
    int i = 0;
    while (Wire.available() && i < 6) {
        receivedMove[i] = Wire.read(); // Read char data
        i++;
    }
    receivedMove[5] = '\0'; // Null-terminate the received char array
    Serial.println(receivedMove); // Print received data to serial monitor

    if (receivedMove[0] == 'o') {
        // Castle
    } else if (receivedMove[0] == 'x') {
        // Capture
        //removeCapture()
    } else {
        // Normal
        //parseChessMove(receivedMove);
        //makeMove();
    }
}

