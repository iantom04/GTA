// Car.h

class Car {
public:
    int x, y;
    bool activo;  // true si CJ est� dentro
    bool disponible; // false si ya se us� y se sali�

    Car(int px, int py);
    void mover(int dx, int dy, int maxX, int maxY);
};
