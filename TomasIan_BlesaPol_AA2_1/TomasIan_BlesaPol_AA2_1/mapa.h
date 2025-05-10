#pragma once
#include "Constantes.h"

struct Mapa
{
	int width;
	int height;
	char** grid;

	int maxDinero[NUM_ISLAS];
};
