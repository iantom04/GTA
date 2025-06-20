// Player.cpp
#include "Player.h"

Player::Player(int vida_, int ataque_) : vida(vida_), ataque(ataque_), simbolo('v') {
    x = 1;
    y = 1;
}

void Player::mover(int dx, int dy, int maxX, int maxY) {
    int nx = x + dx;
    int ny = y + dy;
    if (nx >= 0 && nx < maxX && ny >= 0 && ny < maxY) {
        x = nx;
        y = ny;
        if (dx == 1) simbolo = '>';
        else if (dx == -1) simbolo = '<';
        else if (dy == -1) simbolo = '^';
        else if (dy == 1) simbolo = 'v';
    }
}