// Grand Theft Auto: ENTI City
// POL BLESA ALBIÑANA i IAN TOMÁS CELMA
// 11 / 05 / 2025
// ENTI-UB
// Tecnologia de la Programació

#include <iostream>
#include <fstream>
#include <Windows.h>
#include <ctime>
#include <cstdlib>

#include "Constantes.h"
#include "Mapa.h"
#include "Player.h"
#include "Config.h"

 
// Inicializar el jugador
void InitializePlayer(Player& player, MapaDeJuego& map) 
{
    player.pos.x = map.width / 6; 
    player.pos.y = map.height / 2;
    player.dir = Direcciones::RIGHT;
    player.dinero = 0;
    map.celdas[player.pos.y][player.pos.x] = TipoDeCelda::PLAYER;
}

// Ggenerar una posición aleatoria en una isla específica
Posicion GetRandomPosicionInIsland(MapaDeJuego& map, int islandIndex) 
{
    Posicion pos;
    int islandWidth = map.width / 3;
    int startX = islandIndex * islandWidth + 1;
    int endX = (islandIndex + 1) * islandWidth - 1;

    if (islandIndex == 1 || islandIndex == 2) 
    {
        startX++;  
    }

    do 
    {
        pos.x = startX + rand() % (endX - startX);
        pos.y = 1 + rand() % (map.height - 2);
    } while (map.celdas[pos.y][pos.x] != TipoDeCelda::VACIO);

    return pos;
}

// Inicializar los peatones
void InitializePeatones(Peaton* Peatones, int PeatonesLos, int PeatonesSan, MapaDeJuego& map) 
{
    for (int i = 0; i < PeatonesLos; i++) 
    {
        Posicion pos = GetRandomPosicionInIsland(map, 0);
        Peatones[i].pos = pos;
        Peatones[i].vivo = true;
        Peatones[i].dir = (Direcciones)(rand() % 4);
        map.celdas[pos.y][pos.x] = TipoDeCelda::PEATON;
    }

    for (int i = PeatonesLos; i < PeatonesLos + PeatonesSan; i++)
    {
        Posicion pos = GetRandomPosicionInIsland(map, 1);
        Peatones[i].pos = pos;
        Peatones[i].vivo = true;
        Peatones[i].dir = (Direcciones)(rand() % 4);
        map.celdas[pos.y][pos.x] = TipoDeCelda::PEATON;
    }
}

// Inicializar el dinero
void InitializeDinero(Dinero* dinero, int totalPEATONs)
{
    for (int i = 0; i < totalPEATONs; i++) 
    {
        dinero[i].activo = false;
    }
}

// Actualizar la vista del juego
void UpdateView(VistaDeJuego& view, Player& player, MapaDeJuego& map) 
{
    view.offsetX = player.pos.x - view.vistaWidth / 2;
    view.offsetY = player.pos.y - view.vistaHeight / 2;

    if (view.offsetX < 0) view.offsetX = 0;
    if (view.offsetY < 0) view.offsetY = 0;
    if (view.offsetX + view.vistaWidth > map.width) view.offsetX = map.width - view.vistaWidth;
    if (view.offsetY + view.vistaHeight > map.height) view.offsetY = map.height - view.vistaHeight;
}

// Dibujar el juego
void DrawGame(VistaDeJuego& view, Player& player, MapaDeJuego& map) 
{
    system("cls"); 

    for (int i = 0; i < view.vistaHeight; i++) 
    {
        for (int j = 0; j < view.vistaWidth; j++) 
        {
            int mapY = i + view.offsetY;
            int mapX = j + view.offsetX;

            if (mapY < 0 || mapY >= map.height || mapX < 0 || mapX >= map.width) 
            {
                std::cout << ' ';
                continue;
            }

            if (mapY == player.pos.y && mapX == player.pos.x) 
            {
                switch (player.dir) 
                {
                case Direcciones::UP:
                    std::cout << '^';
                    break;
                case Direcciones::DOWN:
                    std::cout << 'v';
                    break;
                case Direcciones::LEFT:
                    std::cout << '<';
                    break;
                case Direcciones::RIGHT:
                    std::cout << '>';
                    break;
                }
            }
            else 
            {
                switch (map.celdas[mapY][mapX]) 
                {
                case TipoDeCelda::VACIO:
                    std::cout << ' ';
                    break;
                case TipoDeCelda::PARED:
                    std::cout << 'X';
                    break;
                case TipoDeCelda::PEATON:
                    std::cout << 'P';
                    break;
                case TipoDeCelda::DINERO:
                    std::cout << '$';
                    break;
                default:
                    std::cout << ' ';
                    break;
                }
            }
        }
        std::cout << std::endl;
    }

    std::cout << "Dinero: $" << player.dinero << std::endl;
}

// Mover al jugador
bool MovePlayer(Player& player, MapaDeJuego& map, Direcciones dir) 
{
    player.dir = dir;

    int newX = player.pos.x;
    int newY = player.pos.y;

    switch (dir) {
    case Direcciones::UP:
        newY--;
        break;
    case Direcciones::DOWN:
        newY++;
        break;
    case Direcciones::LEFT:
        newX--;
        break;
    case Direcciones::RIGHT:
        newX++;
        break;
    }

    if (newX >= 0 && newX < map.width && newY >= 0 && newY < map.height) 
    {
        if (map.celdas[newY][newX] != TipoDeCelda::PARED && map.celdas[newY][newX] != TipoDeCelda::PEATON) 
        {
            TipoDeCelda oldCell = map.celdas[newY][newX];
            map.celdas[player.pos.y][player.pos.x] = TipoDeCelda::VACIO;
            map.celdas[newY][newX] = TipoDeCelda::PLAYER;

            player.pos.x = newX;
            player.pos.y = newY;

            if (oldCell == TipoDeCelda::DINERO)
            {
                return true;
            }
        }
    }

    return false;
}

// Mover a los peatones
void MovePeatones(Peaton* Peatones, int totalPeatones, MapaDeJuego& map, Player& player) 
{
    for (int i = 0; i < totalPeatones; i++)
    {
        if (!Peatones[i].vivo)
        {
            continue;
        }

        int dx = abs(Peatones[i].pos.x - player.pos.x);
        int dy = abs(Peatones[i].pos.y - player.pos.y);
        if (dx <= 1 && dy <= 1) 
        {
            continue; 
        }

        int newX = Peatones[i].pos.x;
        int newY = Peatones[i].pos.y;

        if (rand() % 20 == 0) 
        {
            if (rand() % 2 == 0) 
            {
                Peatones[i].dir = (rand() % 2 == 0) ? Direcciones::LEFT : Direcciones::RIGHT;
            }
            else 
            {
                Peatones[i].dir = (rand() % 2 == 0) ? Direcciones::UP : Direcciones::DOWN;
            }
        }

        switch (Peatones[i].dir)
        {
        case Direcciones::UP:
            newY--;
            break;
        case Direcciones::DOWN:
            newY++;
            break;
        case Direcciones::LEFT:
            newX--;
            break;
        case Direcciones::RIGHT:
            newX++;
            break;
        }

        if (newX >= 0 && newX < map.width && newY >= 0 && newY < map.height) 
        {
            if (map.celdas[newY][newX] == TipoDeCelda::VACIO) 
            {
                map.celdas[Peatones[i].pos.y][Peatones[i].pos.x] = TipoDeCelda::VACIO;
                map.celdas[newY][newX] = TipoDeCelda::PEATON;

                Peatones[i].pos.x = newX;
                Peatones[i].pos.y = newY;
            }
            else 
            {
                Peatones[i].dir = (Direcciones)(rand() % 4);
            }
        }
        else 
        {
            Peatones[i].dir = (Direcciones)(rand() % 4);
        }
    }
}

// Jugador ataque a los peatones
bool AttackPeaton(Player& player, Peaton* Peatones, int totalPEATONs, Dinero* dinero, MapaDeJuego& map, Config& config)
{
    for (int i = 0; i < totalPEATONs; i++) 
    {
        if (!Peatones[i].vivo) continue;

        int dx = abs(Peatones[i].pos.x - player.pos.x);
        int dy = abs(Peatones[i].pos.y - player.pos.y);

        if (dx <= 1 && dy <= 1) 
        {
            Peatones[i].vivo = false;

            map.celdas[Peatones[i].pos.y][Peatones[i].pos.x] = TipoDeCelda::DINERO;
            dinero[i].pos = Peatones[i].pos;
            dinero[i].activo = true;

            int islandIndex = Peatones[i].pos.x / (map.width / 3);
            int maxDinero;
            if (islandIndex == 0) 
            {
                maxDinero = config.maxDineroLos;
            }
            else 
            {
                maxDinero = config.maxDineroSan;
            }

            dinero[i].cantidad = 1 + rand() % maxDinero;

            return true;
        }
    }

    return false;
}

// Rrecoger dinero
void CollectDinero(Player& player, Dinero* dinero, int totalPEATONs, MapaDeJuego& map)
{
    for (int i = 0; i < totalPEATONs; i++) 
    {
        if (dinero[i].activo && player.pos.x == dinero[i].pos.x && player.pos.y == dinero[i].pos.y)
        {
            player.dinero += dinero[i].cantidad;
            dinero[i].activo = false;
        }
    }
}

void RegeneratePeatones(Peaton* Peatones, int PeatonesLos, int PeatonesSan, MapaDeJuego& map) 
{
    for (int i = 0; i < PeatonesLos + PeatonesSan; i++) 
    {
        if (!Peatones[i].vivo)
        {
            int islandIndex;
            if (i < PeatonesLos) 
            {
                islandIndex = 0; 
            }
            else 
            {
                islandIndex = 1; 
            }

            Posicion pos = GetRandomPosicionInIsland(map, islandIndex);
            Peatones[i].pos = pos;
            Peatones[i].vivo = true;
            Peatones[i].dir = (Direcciones)(rand() % 4);
            map.celdas[pos.y][pos.x] = TipoDeCelda::PEATON;
        }
    }
}

// Liberar la memoria del mapa
void FreeMap(MapaDeJuego& map) 
{
    for (int i = 0; i < map.height; i++) 
    {
        delete[] map.celdas[i];
    }
    delete[] map.celdas;
}

int main() 
{
    srand(time(NULL));

    Config config;
    if (!ReadConfig(config)) 
    {
        return 1;
    }

    MapaDeJuego map;
    InitializeMap(map, config.mapWidth, config.mapHeight);

    Player player;
    InitializePlayer(player, map);

    VistaDeJuego view;
    view.vistaWidth = VISTA_WIDTH;
    view.vistaHeight = VISTA_HEIGHT;

    int totalPeatones = config.PeatonesLos + config.PeatonesSan;

    Peaton* Peatones = new Peaton[totalPeatones];
    InitializePeatones(Peatones, config.PeatonesLos, config.PeatonesSan, map);

    Dinero* dinero = new Dinero[totalPeatones];
    InitializeDinero(dinero, totalPeatones);

    bool running = true;

    while (running) 
    {
        UpdateView(view, player, map);

        DrawGame(view, player, map);

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) 
        {
            running = false;
        }

        if (GetAsyncKeyState(VK_UP) & 0x8000) 
        {
            bool collectedDinero = MovePlayer(player, map, Direcciones::UP);
            if (collectedDinero)
            {
                CollectDinero(player, dinero, totalPeatones, map);
            }
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x8000) 
        {
            bool collectedDinero = MovePlayer(player, map, Direcciones::DOWN);
            if (collectedDinero)
            {
                CollectDinero(player, dinero, totalPeatones, map);
            }
        }
        else if (GetAsyncKeyState(VK_LEFT) & 0x8000) 
        {
            bool collectedDinero = MovePlayer(player, map, Direcciones::LEFT);
            if (collectedDinero)
            {
                CollectDinero(player, dinero, totalPeatones, map);
            }
        }
        else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) 
        {
            bool collectedDinero = MovePlayer(player, map, Direcciones::RIGHT);
            if (collectedDinero)
            {
                CollectDinero(player, dinero, totalPeatones, map);
            }
        }

        if (GetAsyncKeyState(VK_SPACE) & 0x8000) 
        {
            if (AttackPeaton(player, Peatones, totalPeatones, dinero, map, config))
            {
                RegeneratePeatones(Peatones, config.PeatonesLos, config.PeatonesSan, map);
            }
        }

        MovePeatones(Peatones, totalPeatones, map, player);

        Sleep(ACT_TIEMPO);
    }

    FreeMap(map);
    delete[] Peatones;
    delete[] dinero;

    return 0;
}