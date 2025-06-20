// Mapa.cpp
#include "Mapa.h"
#include <windows.h>

const int MAX_VIEW_WIDTH = 15;
const int MAX_VIEW_HEIGHT = 10;

Mapa::Mapa(int f, int c) : filas(f), columnas(c) {
    grid = new char* [filas];
    for (int i = 0; i < filas; ++i) {
        grid[i] = new char[columnas];
        for (int j = 0; j < columnas; ++j) {
            grid[i][j] = ' ';
        }
    }
}

void Mapa::colocarElemento(int x, int y, char ch) {
    if (y >= 0 && y < filas && x >= 0 && x < columnas) {
        grid[y][x] = ch;
    }
}

void Mapa::mostrar(int cx, int cy) {
    system("cls");
    for (int i = 0; i < MAX_VIEW_HEIGHT; ++i) {
        for (int j = 0; j < MAX_VIEW_WIDTH; ++j) {
            int mi = cy - MAX_VIEW_HEIGHT / 2 + i;
            int mj = cx - MAX_VIEW_WIDTH / 2 + j;
            if (mi >= 0 && mi < filas && mj >= 0 && mj < columnas)
                std::cout << grid[mi][mj];
            else
                std::cout << ' ';
        }
        std::cout << std::endl;
    }
}

void Mapa::limpiar() {
    for (int i = 0; i < filas; ++i)
        for (int j = 0; j < columnas; ++j)
            grid[i][j] = ' ';
}

int Mapa::getFilas() const { return filas; }
int Mapa::getColumnas() const { return columnas; }

Mapa::~Mapa() {
    for (int i = 0; i < filas; ++i) delete[] grid[i];
    delete[] grid;
}