// Walker.cpp
#include "Walker.h"
#include <cstdlib>

Walker::Walker(int px, int py, int v, int a, bool ag)
{
    x = px;
    y = py;
    vida = v;
    ataque = a;
    agresivo = ag;
    vivo = true;
    direccion = rand() % 4;
    simbolo = 'P';
}

void Walker::mover(int maxX, int maxY) {
    if (!vivo) return;
    int dx = 0, dy = 0;
    switch (direccion) {
    case 0: dy = -1; break; // arriba
    case 1: dy = 1;  break; // abajo
    case 2: dx = -1; break; // izquierda
    case 3: dx = 1;  break; // derecha
    }
    int nx = x + dx;
    int ny = y + dy;
    if (nx >= 0 && nx < maxX && ny >= 0 && ny < maxY) {
        x = nx;
        y = ny;
    }
    else {
        direccion = rand() % 4; // cambiar dirección si hay obstáculo
    }
}

void Walker::recibirAtaque(int danio) {
    vida -= danio;
    if (vida <= 0) vivo = false;
}

bool Walker::puedeAtacar() const {
    return agresivo && vivo;
}