// Car.h

class Car {
public:
    int x, y;
    bool activo;  // true si CJ está dentro
    bool disponible; // false si ya se usó y se salió

    Car(int px, int py);
    void mover(int dx, int dy, int maxX, int maxY);
};
