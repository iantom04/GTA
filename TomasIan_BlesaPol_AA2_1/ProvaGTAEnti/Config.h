#pragma once
#include <iostream>
#include <fstream>

struct Config
{
    int mapWidth;
    int mapHeight;
    int PeatonesLos;
    int dineroSanFierro;
    int maxDineroLos;
    int PeatonesSan;
    int dineroVenturas;
    int maxDineroSan;
};

bool ReadConfig(Config& config);
