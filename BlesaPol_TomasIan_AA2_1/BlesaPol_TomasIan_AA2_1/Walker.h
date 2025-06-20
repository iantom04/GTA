// Walker.h
#pragma once
#include <string>

class Walker {
public:
    int x, y;
    int vida;
    int ataque;
    bool agresivo;
    bool vivo;
    char simbolo;
    int direccion;

    Walker(int px, int py, int v, int a, bool ag);
    void mover(int maxX, int maxY);
    void recibirAtaque(int danio);
    bool puedeAtacar() const;
};
