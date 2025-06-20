// Player.h
#pragma once
class Player {
public:
    int x, y;
    int vida;
    int ataque;
    char simbolo;

    Player(int vida_, int ataque_);
    void mover(int dx, int dy, int maxX, int maxY);
};
