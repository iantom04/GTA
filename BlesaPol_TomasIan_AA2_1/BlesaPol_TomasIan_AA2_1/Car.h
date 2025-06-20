// Car.h
#pragma once
class Car {
public:
    int x, y;
    bool activo; 
    bool disponible; 

    Car(int px, int py);
    void mover(int dx, int dy, int maxX, int maxY);
};
