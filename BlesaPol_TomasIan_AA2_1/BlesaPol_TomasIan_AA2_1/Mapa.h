// Mapa.h
#pragma once
#include <iostream>

class Mapa {
private:
    char** grid;
    int filas;
    int columnas;

public:
    Mapa(int f, int c);
    void colocarElemento(int x, int y, char ch);
    void mostrar(int cx, int cy);
    void limpiar();
    int getFilas() const;
    int getColumnas() const;
    ~Mapa();
};
