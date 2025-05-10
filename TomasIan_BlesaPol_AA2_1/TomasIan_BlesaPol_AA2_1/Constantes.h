#pragma once

enum class Direcciones
{
	UP, DOWN, LEFT, RIGHT
};

struct IslasConfig
{
	int numPeatones;
	int peaje;
	int maxDinero;
};

const int NUM_ISLAS = 3;
const int WIDTH_VISTA = 15;
const int HEIGHT_VISTA = 15;

// Nomenclatura general:
const char SIMBOLO_PARED = 'X';
const char SIMBOLO_PEATON = 'P';
const char SIMBOLO_DINERO = '$';

// Nomenclatura jugador (CJ):
const char CJ_UP = '^';
const char CJ_DOWN = 'v';
const char CJ_LEFT = '<';
const char CJ_RIGHT = '>';