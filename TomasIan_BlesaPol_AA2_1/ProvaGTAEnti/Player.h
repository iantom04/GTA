#pragma once

struct Player
{
    Posicion pos;
    Direcciones dir;
    int dinero;
};

struct VistaDeJuego
{
    int vistaWidth;
    int vistaHeight;
    int offsetX;
    int offsetY;
};
