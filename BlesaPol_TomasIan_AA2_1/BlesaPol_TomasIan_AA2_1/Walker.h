// Walker.h
#include <string>

class Walker {
public:
    int x, y;
    int vida;
    int ataque;
    bool agresivo;
    bool vivo;
    char simbolo;
    int direccion; // 0: arriba, 1: abajo, 2: izquierda, 3: derecha

    Walker(int px, int py, int v, int a, bool ag);
    void mover(int maxX, int maxY);
    void recibirAtaque(int danio);
    bool puedeAtacar() const;
};
