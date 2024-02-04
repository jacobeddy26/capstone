#include <Servo.h>

const int electromagnetPin = 40;
const int servoPin = 38;

const int RESTING_HEIGHT = 100;
const int MAX_PIECE_HEIGHT = 56;

const int PIECE_HEIGHTS[] = {41, 34, 20, 28, 24, 19};

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

    void move(int z) {
        z = constrain(z, 0, 100);
        int dc = (z * 0.067) + 4;
        servo.writeMicroseconds(dc * 100);
        int t = (previousZ == -1) ? 1000 : abs(previousZ - z) / 10 + 500;
        delay(t);
        servo.detach();
        previousZ = z;
    }

    void electromagnet(bool on) {
        digitalWrite(electromagnetPin, on ? HIGH : LOW);
    }

    void pickup(int pieceType) {
        int pieceHeight = PIECE_HEIGHTS[pieceType];
        move(pieceHeight);
        delay(400);
        electromagnet(true);
        delay(200);
        move(RESTING_HEIGHT + pieceHeight);
    }

    void dropoff(int pieceType) {
        int pieceHeight = PIECE_HEIGHTS[pieceType];
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

void setup() {
    gripper.calibrate();
}

void loop() {
    // Your chess robot logic here
}
