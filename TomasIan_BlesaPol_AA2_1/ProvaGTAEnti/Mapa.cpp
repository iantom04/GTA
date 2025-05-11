#include "Mapa.h"

void InitializeMap(MapaDeJuego& map, int width, int height)
{
    map.width = width;
    map.height = height;

    map.celdas = new TipoDeCelda * [height];
    for (int i = 0; i < height; i++)
    {
        map.celdas[i] = new TipoDeCelda[width];
        for (int j = 0; j < width; j++)
        {
            map.celdas[i][j] = TipoDeCelda::VACIO;
        }
    }

    for (int i = 0; i < height; i++)
    {
        map.celdas[i][0] = TipoDeCelda::PARED;
        map.celdas[i][width - 1] = TipoDeCelda::PARED;
    }
    for (int j = 0; j < width; j++)
    {
        map.celdas[0][j] = TipoDeCelda::PARED;
        map.celdas[height - 1][j] = TipoDeCelda::PARED;
    }

    int islandWidth = width / 3;

    for (int i = 0; i < height; i++)
    {
        if (i != height / 2) {
            map.celdas[i][islandWidth] = TipoDeCelda::PARED;
        }
    }

    for (int i = 0; i < height; i++)
    {
        if (i != height / 2) {
            map.celdas[i][islandWidth * 2] = TipoDeCelda::PARED;
        }
    }
}
