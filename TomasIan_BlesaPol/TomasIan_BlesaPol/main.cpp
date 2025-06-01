// Grand Theft Auto: ENTI City
// POL BLESA ALBIÑANA i IAN TOMÁS CELMA
// 06 / 06 / 2025
// ENTI-UB
// Tecnologia de la Programació

#include <iostream>
#include <fstream>
#include <Windows.h>
#include <ctime>
#include <cstdlib>
#include <vector>

const int VISTA_WIDTH = 20;
const int VISTA_HEIGHT = 15;
const int ACT_TIEMPO = 100;

enum class Direcciones
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

enum class TipoDeCelda
{
    VACIO,
    PARED,
    PLAYER,
    PEATON,
    DINERO
};

struct Posicion
{
    int x;
    int y;
};

class Config
{
public:
    int mapWidth, mapHeight;
    int PeatonesLos, dineroSanFierro, maxDineroLos;
    int PeatonesSan, dineroVenturas, maxDineroSan;

    bool cargar(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cout << "Error: No se pudo abrir el archivo de configuración\n";
            return false;
        }
        char separator;
        file >> mapWidth >> separator >> mapHeight;
        file >> PeatonesLos >> separator >> dineroSanFierro >> separator >> maxDineroLos;
        file >> PeatonesSan >> separator >> dineroVenturas >> separator >> maxDineroSan;
        return true;
    }
};

class MapaDeJuego
{
public:
    int width;
    int height;
    std::vector<std::vector<TipoDeCelda>> celdas;

    void inicializar(int w, int h)
    {
        width = w;
        height = h;
        celdas.resize(height, std::vector<TipoDeCelda>(width, TipoDeCelda::VACIO));

        for (int i = 0; i < height; ++i)
        {
            celdas[i][0] = TipoDeCelda::PARED;
            celdas[i][width - 1] = TipoDeCelda::PARED;
        }
        for (int j = 0; j < width; ++j)
        {
            celdas[0][j] = TipoDeCelda::PARED;
            celdas[height - 1][j] = TipoDeCelda::PARED;
        }

        int iw = width / 3;
        for (int i = 0; i < height; ++i)
        {
            if (i != height / 2) celdas[i][iw] = TipoDeCelda::PARED;
            if (i != height / 2) celdas[i][iw * 2] = TipoDeCelda::PARED;
        }
    }

    Posicion posicionAleatoria(int isla)
    {
        int iw = width / 3;
        int startX = isla * iw + 1;
        int endX = (isla + 1) * iw - 1;
        if (isla == 1 || isla == 2) startX++;

        Posicion p;
        do
        {
            p.x = startX + rand() % (endX - startX);
            p.y = 1 + rand() % (height - 2);
        } while (celdas[p.y][p.x] != TipoDeCelda::VACIO);
        return p;
    }
};

class VistaDeJuego
{
public:
    int vistaWidth = VISTA_WIDTH, vistaHeight = VISTA_HEIGHT;
    int offsetX = 0, offsetY = 0;

    void actualizar(Posicion jugador, int mapW, int mapH)
    {
        offsetX = jugador.x - vistaWidth / 2;
        offsetY = jugador.y - vistaHeight / 2;

        if (offsetX < 0) offsetX = 0;
        if (offsetY < 0) offsetY = 0;
        if (offsetX + vistaWidth > mapW) offsetX = mapW - vistaWidth;
        if (offsetY + vistaHeight > mapH) offsetY = mapH - vistaHeight;
    }
};

class Dinero
{
public:
    Posicion pos;
    bool activo = false;
    int cantidad = 0;
};

class Player
{
public:
    Posicion pos;
    Direcciones dir = Direcciones::RIGHT;
    int dinero = 0;

    void inicializar(MapaDeJuego& mapa)
    {
        pos = { mapa.width / 6, mapa.height / 2 };
        mapa.celdas[pos.y][pos.x] = TipoDeCelda::PLAYER;
    }

    bool mover(Direcciones d, MapaDeJuego& mapa)
    {
        dir = d;
        int nx = pos.x, ny = pos.y;

        switch (dir)
        {
        case Direcciones::UP: ny--; break;
        case Direcciones::DOWN: ny++; break;
        case Direcciones::LEFT: nx--; break;
        case Direcciones::RIGHT: nx++; break;
        }

        if (nx >= 0 && nx < mapa.width && ny >= 0 && ny < mapa.height && mapa.celdas[ny][nx] != TipoDeCelda::PARED && mapa.celdas[ny][nx] != TipoDeCelda::PEATON)
        {
            TipoDeCelda oldCell = mapa.celdas[ny][nx];
            mapa.celdas[pos.y][pos.x] = TipoDeCelda::VACIO;
            mapa.celdas[ny][nx] = TipoDeCelda::PLAYER;
            pos = { nx, ny };
            return oldCell == TipoDeCelda::DINERO;
        }

        return false;
    }

    void recogerDinero(std::vector<Dinero>& d)
    {
        for (auto& e : d)
        {
            if (e.activo && e.pos.x == pos.x && e.pos.y == pos.y)
            {
                dinero += e.cantidad;
                e.activo = false;
            }
        }
    }
};

class Peaton
{
public:
    Posicion pos;
    Direcciones dir;
    bool vivo = true;

    void inicializar(MapaDeJuego& mapa, int isla)
    {
        pos = mapa.posicionAleatoria(isla);
        dir = static_cast<Direcciones>(rand() % 4);
        mapa.celdas[pos.y][pos.x] = TipoDeCelda::PEATON;
    }

    void mover(MapaDeJuego& mapa, const Posicion& jugador)
    {
        if (!vivo) return;

        int dx = abs(pos.x - jugador.x);
        int dy = abs(pos.y - jugador.y);
        if (dx <= 1 && dy <= 1) return;

        if (rand() % 20 == 0)
        {
            dir = static_cast<Direcciones>(rand() % 4);
        }

        int nx = pos.x, ny = pos.y;
        switch (dir)
        {
        case Direcciones::UP: ny--; break;
        case Direcciones::DOWN: ny++; break;
        case Direcciones::LEFT: nx--; break;
        case Direcciones::RIGHT: nx++; break;
        }

        if (nx >= 0 && nx < mapa.width && ny >= 0 && ny < mapa.height && mapa.celdas[ny][nx] == TipoDeCelda::VACIO)
        {
            mapa.celdas[pos.y][pos.x] = TipoDeCelda::VACIO;
            pos = { nx, ny };
            mapa.celdas[pos.y][pos.x] = TipoDeCelda::PEATON;
        }
        else
        {
            dir = static_cast<Direcciones>(rand() % 4);
        }
    }
};

int main()
{
    srand(time(NULL));

    Config config;
    if (!config.cargar("config.txt")) return 1;

    MapaDeJuego mapa;
    mapa.inicializar(config.mapWidth, config.mapHeight);

    Player jugador;
    jugador.inicializar(mapa);

    int totalPeatones = config.PeatonesLos + config.PeatonesSan;
    std::vector<Peaton> peatones(totalPeatones);
    for (int i = 0; i < totalPeatones; ++i)
    {
        peatones[i].inicializar(mapa, (i < config.PeatonesLos) ? 0 : 1);
    }

    std::vector<Dinero> dinero(totalPeatones);

    VistaDeJuego vista;

    bool running = true;
    while (running) {
        vista.actualizar(jugador.pos, mapa.width, mapa.height);

        system("cls");
        for (int i = 0; i < vista.vistaHeight; ++i)
        {
            for (int j = 0; j < vista.vistaWidth; ++j)
            {
                int y = i + vista.offsetY;
                int x = j + vista.offsetX;
                if (y == jugador.pos.y && x == jugador.pos.x)
                {
                    switch (jugador.dir)
                    {
                    case Direcciones::UP: std::cout << '^'; break;
                    case Direcciones::DOWN: std::cout << 'v'; break;
                    case Direcciones::LEFT: std::cout << '<'; break;
                    case Direcciones::RIGHT: std::cout << '>'; break;
                    }
                }
                else {
                    switch (mapa.celdas[y][x])
                    {
                    case TipoDeCelda::VACIO: std::cout << ' '; break;
                    case TipoDeCelda::PARED: std::cout << 'X'; break;
                    case TipoDeCelda::PEATON: std::cout << 'P'; break;
                    case TipoDeCelda::DINERO: std::cout << '$'; break;
                    default: std::cout << ' '; break;
                    }
                }
            }
            std::cout << "\n";
        }
        std::cout << "Dinero: $" << jugador.dinero << "\n";

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) running = false;

        if (GetAsyncKeyState(VK_UP) & 0x8000 && jugador.mover(Direcciones::UP, mapa)) jugador.recogerDinero(dinero);
        if (GetAsyncKeyState(VK_DOWN) & 0x8000 && jugador.mover(Direcciones::DOWN, mapa)) jugador.recogerDinero(dinero);
        if (GetAsyncKeyState(VK_LEFT) & 0x8000 && jugador.mover(Direcciones::LEFT, mapa)) jugador.recogerDinero(dinero);
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && jugador.mover(Direcciones::RIGHT, mapa)) jugador.recogerDinero(dinero);

        if (GetAsyncKeyState(VK_SPACE) & 0x8000)
        {
            for (int i = 0; i < totalPeatones; ++i)
            {
                if (!peatones[i].vivo) continue;
                int dx = abs(peatones[i].pos.x - jugador.pos.x);
                int dy = abs(peatones[i].pos.y - jugador.pos.y);
                if (dx <= 1 && dy <= 1)
                {
                    peatones[i].vivo = false;
                    mapa.celdas[peatones[i].pos.y][peatones[i].pos.x] = TipoDeCelda::DINERO;
                    dinero[i].pos = peatones[i].pos;
                    dinero[i].activo = true;
                    dinero[i].cantidad = 1 + rand() % ((i < config.PeatonesLos) ? config.maxDineroLos : config.maxDineroSan);
                    break;
                }
            }
            for (int i = 0; i < totalPeatones; ++i)
            {
                if (!peatones[i].vivo)
                {
                    peatones[i].inicializar(mapa, (i < config.PeatonesLos) ? 0 : 1);
                }
            }
        }

        for (auto& p : peatones) p.mover(mapa, jugador.pos);

        Sleep(ACT_TIEMPO);
    }

    return 0;
}
