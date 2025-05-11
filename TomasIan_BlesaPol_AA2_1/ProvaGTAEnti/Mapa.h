#pragma once
#include "Constantes.h"


enum class TipoDeCelda
{
    VACIO, PARED, PLAYER, PEATON, DINERO
};

struct Posicion
{
    int x;
    int y;
};

struct Peaton
{
    Posicion pos;
    Direcciones dir;
    bool vivo;
};

struct Dinero
{
    Posicion pos;
    bool activo;
    int cantidad;
};

struct MapaDeJuego
{
    TipoDeCelda** celdas;
    int width;
    int height;
};

void InitializeMap(MapaDeJuego& map, int width, int height);
