// Car.cpp
#include "Car.h"

Car::Car(int px, int py) : x(px), y(py), activo(false), disponible(true) {}

void Car::mover(int dx, int dy, int maxX, int maxY) {
    if (!activo) return;
    int nx = x + dx;
    int ny = y + dy;
    if (nx >= 0 && nx < maxX && ny >= 0 && ny < maxY) {
        x = nx;
        y = ny;
    }
}