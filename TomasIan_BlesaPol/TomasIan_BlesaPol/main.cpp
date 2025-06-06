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
#include <string>

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
    DINERO,
    COCHE,
    PEAJE,
    BIG_SMOKE
};

enum class GameState
{
    INIT,
    MENU,
    GAME,
    GAME_OVER
};

enum class TipoPeaton
{
    NEUTRAL,
    AGRESIVO
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
    int playerVida, playerAtaque;
    int peajeSanFierro, peajeVenturas;

    // Los Santos
    int peatonesLos, maxDineroLos, vidaPeatonLos, ataquePeatonLos;
    // San Fierro
    int peatonesSan, maxDineroSan, vidaPeatonSan, ataquePeatonSan;
    // Las Venturas
    int peatonesVenturas, maxDineroVenturas, vidaPeatonVenturas, ataquePeatonVenturas;

    bool cargar(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cout << "Error: No se pudo abrir el archivo de configuración\n";
            return false;
        }

        char separator;
        file >> mapWidth >> separator >> mapHeight >> separator;
        file >> playerVida >> separator >> playerAtaque >> separator;
        file >> peajeSanFierro >> separator >> peajeVenturas >> separator;
        file >> peatonesLos >> separator >> maxDineroLos >> separator >> vidaPeatonLos >> separator >> ataquePeatonLos >> separator;
        file >> peatonesSan >> separator >> maxDineroSan >> separator >> vidaPeatonSan >> separator >> ataquePeatonSan >> separator;
        file >> peatonesVenturas >> separator >> maxDineroVenturas >> separator >> vidaPeatonVenturas >> separator >> ataquePeatonVenturas >> separator;

        file.close();
        return true;
    }
};

class MapaDeJuego
{
public:
    int width = 0;
    int height = 0;
    TipoDeCelda** celdas;
    bool peajeSanFierroPagado = false;
    bool peajeVenturasPagado = false;

    MapaDeJuego() : celdas(nullptr) {}

    ~MapaDeJuego()
    {
        if (celdas != nullptr)
        {
            for (int i = 0; i < height; ++i)
            {
                delete[] celdas[i];
            }
            delete[] celdas;
        }
    }

    void inicializar(int w, int h)
    {
        width = w;
        height = h;

        celdas = new TipoDeCelda * [height];
        for (int i = 0; i < height; ++i)
        {
            celdas[i] = new TipoDeCelda[width];
            for (int j = 0; j < width; ++j)
            {
                celdas[i][j] = TipoDeCelda::VACIO;
            }
        }

        // Paredes perimetrales
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

        // Paredes divisorias entre islas
        int iw = width / 3;
        for (int i = 0; i < height; ++i)
        {
            if (i != height / 2) // Puente en el medio
            {
                celdas[i][iw] = TipoDeCelda::PARED;
                celdas[i][iw * 2] = TipoDeCelda::PARED;
            }
        }

        // Colocar peajes
        celdas[height / 2][iw] = TipoDeCelda::PEAJE;
        celdas[height / 2][iw * 2] = TipoDeCelda::PEAJE;
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

    int obtenerIsla(int x)
    {
        int iw = width / 3;
        if (x < iw) return 0;      // Los Santos
        else if (x < iw * 2) return 1; // San Fierro
        else return 2;                 // Las Venturas
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

class Coche
{
public:
    Posicion pos;
    bool activo = true;
    int isla;

    void inicializar(MapaDeJuego& mapa, int islaNum)
    {
        isla = islaNum;
        pos = mapa.posicionAleatoria(isla);
        mapa.celdas[pos.y][pos.x] = TipoDeCelda::COCHE;
    }
};

class Player
{
public:
    Posicion pos;
    Direcciones dir = Direcciones::RIGHT;
    int dinero = 0;
    int vida;
    int ataque;
    bool enCoche = false;
    int cocheActual = -1;
    clock_t ultimoAtaque = 0;

    void inicializar(MapaDeJuego& mapa, const Config& config)
    {
        pos = { mapa.width / 6, mapa.height / 2 };
        vida = config.playerVida;
        ataque = config.playerAtaque;
        mapa.celdas[pos.y][pos.x] = TipoDeCelda::PLAYER;
    }

    bool mover(Direcciones d, MapaDeJuego& mapa, Coche* coches, int totalCoches)
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

        if (nx >= 0 && nx < mapa.width && ny >= 0 && ny < mapa.height)
        {
            TipoDeCelda celdaDestino = mapa.celdas[ny][nx];

            // Verificar peajes
            if (celdaDestino == TipoDeCelda::PEAJE)
            {
                int iw = mapa.width / 3;
                if (nx == iw && !mapa.peajeSanFierroPagado) // Peaje a San Fierro
                {
                    if (dinero >= 1000) // Usar valor hardcoded por ahora
                    {
                        dinero -= 1000;
                        mapa.peajeSanFierroPagado = true;
                        mapa.celdas[ny][nx] = TipoDeCelda::VACIO;
                    }
                    else
                    {
                        return false; // Game Over - sin dinero
                    }
                }
                else if (nx == iw * 2 && !mapa.peajeVenturasPagado) // Peaje a Las Venturas
                {
                    if (dinero >= 2000) // Usar valor hardcoded por ahora
                    {
                        dinero -= 2000;
                        mapa.peajeVenturasPagado = true;
                        mapa.celdas[ny][nx] = TipoDeCelda::VACIO;
                    }
                    else
                    {
                        return false; // Game Over - sin dinero
                    }
                }
            }

            if (celdaDestino != TipoDeCelda::PARED && celdaDestino != TipoDeCelda::PEATON && celdaDestino != TipoDeCelda::BIG_SMOKE)
            {
                TipoDeCelda oldCell = mapa.celdas[ny][nx];

                if (enCoche && cocheActual >= 0)
                {
                    // Mover coche
                    mapa.celdas[coches[cocheActual].pos.y][coches[cocheActual].pos.x] = TipoDeCelda::VACIO;
                    coches[cocheActual].pos = { nx, ny };
                    mapa.celdas[ny][nx] = TipoDeCelda::COCHE;
                }
                else
                {
                    // Mover jugador
                    mapa.celdas[pos.y][pos.x] = TipoDeCelda::VACIO;
                    mapa.celdas[ny][nx] = TipoDeCelda::PLAYER;
                }

                pos = { nx, ny };
                return oldCell == TipoDeCelda::DINERO;
            }
        }

        return false;
    }

    void subirBajarCoche(MapaDeJuego& mapa, Coche* coches, int totalCoches)
    {
        if (enCoche)
        {
            // Bajar del coche
            enCoche = false;

            // Buscar posición libre alrededor del coche
            int dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
            int dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

            for (int i = 0; i < 8; ++i)
            {
                int nx = pos.x + dx[i];
                int ny = pos.y + dy[i];

                if (nx >= 0 && nx < mapa.width && ny >= 0 && ny < mapa.height &&
                    mapa.celdas[ny][nx] == TipoDeCelda::VACIO)
                {
                    pos = { nx, ny };
                    mapa.celdas[ny][nx] = TipoDeCelda::PLAYER;
                    break;
                }
            }
            cocheActual = -1;
        }
        else
        {
            // Subir al coche
            for (int i = 0; i < totalCoches; ++i)
            {
                if (!coches[i].activo) continue;

                int dx = abs(coches[i].pos.x - pos.x);
                int dy = abs(coches[i].pos.y - pos.y);

                if (dx <= 1 && dy <= 1)
                {
                    enCoche = true;
                    cocheActual = i;
                    mapa.celdas[pos.y][pos.x] = TipoDeCelda::VACIO;
                    pos = coches[i].pos;
                    break;
                }
            }
        }
    }

    void recogerDinero(Dinero* dinero, int totalDinero)
    {
        if (enCoche) return; // No se puede recoger dinero en coche

        for (int i = 0; i < totalDinero; ++i)
        {
            if (dinero[i].activo && dinero[i].pos.x == pos.x && dinero[i].pos.y == pos.y)
            {
                this->dinero += dinero[i].cantidad;
                dinero[i].activo = false;
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
    TipoPeaton tipo;
    int vida;
    int ataque;
    int isla;
    clock_t ultimoAtaque = 0;

    void inicializar(MapaDeJuego& mapa, int islaNum, const Config& config)
    {
        isla = islaNum;
        pos = mapa.posicionAleatoria(isla);
        dir = static_cast<Direcciones>(rand() % 4);
        tipo = (rand() % 2 == 0) ? TipoPeaton::NEUTRAL : TipoPeaton::AGRESIVO;

        // Configurar vida y ataque según la isla
        switch (isla)
        {
        case 0: // Los Santos
            vida = config.vidaPeatonLos;
            ataque = config.ataquePeatonLos;
            break;
        case 1: // San Fierro
            vida = config.vidaPeatonSan;
            ataque = config.ataquePeatonSan;
            break;
        case 2: // Las Venturas
            vida = config.vidaPeatonVenturas;
            ataque = config.ataquePeatonVenturas;
            break;
        }

        mapa.celdas[pos.y][pos.x] = TipoDeCelda::PEATON;
    }

    void mover(MapaDeJuego& mapa, const Posicion& jugador)
    {
        if (!vivo) return;

        // Parar si el jugador está a 1 casilla de distancia
        int dx = abs(pos.x - jugador.x);
        int dy = abs(pos.y - jugador.y);
        if (dx <= 1 && dy <= 1) return;

        // Cambiar dirección aleatoriamente
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

        if (nx >= 0 && nx < mapa.width && ny >= 0 && ny < mapa.height &&
            mapa.celdas[ny][nx] == TipoDeCelda::VACIO)
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

    bool atacarJugador(Player& jugador)
    {
        if (!vivo || tipo != TipoPeaton::AGRESIVO) return false;

        clock_t ahora = clock();
        if (ahora - ultimoAtaque < CLOCKS_PER_SEC) return false; // 1 segundo de cooldown

        int dx = abs(pos.x - jugador.pos.x);
        int dy = abs(pos.y - jugador.pos.y);

        if (dx <= 1 && dy <= 1)
        {
            jugador.vida -= ataque;
            ultimoAtaque = ahora;
            return true;
        }

        return false;
    }
};

class BigSmoke
{
public:
    Posicion pos;
    bool vivo = true;
    int vida = 500; // Vida alta
    int ataque = 50; // Ataque alto
    clock_t ultimoAtaque = 0;

    void inicializar(MapaDeJuego& mapa)
    {
        pos = mapa.posicionAleatoria(2); // Las Venturas
        mapa.celdas[pos.y][pos.x] = TipoDeCelda::BIG_SMOKE;
    }

    bool atacarJugador(Player& jugador)
    {
        if (!vivo) return false;

        clock_t ahora = clock();
        if (ahora - ultimoAtaque < CLOCKS_PER_SEC) return false;

        int dx = abs(pos.x - jugador.pos.x);
        int dy = abs(pos.y - jugador.pos.y);

        if (dx <= 1 && dy <= 1)
        {
            jugador.vida -= ataque;
            ultimoAtaque = ahora;
            return true;
        }

        return false;
    }
};

class GameManager
{
public:
    GameState estado = GameState::INIT;
    int menuOpcion = 0;
    clock_t tiempoInicio = 0;
    std::string mensajeGameOver = "";

    void mostrarInit()
    {
        system("cls");
        std::cout << "\n\n\n\n\n\n";
        std::cout << "        ====================================\n";
        std::cout << "        |                                  |\n";
        std::cout << "        |       GRAND THEFT AUTO:         |\n";
        std::cout << "        |          ENTI CITY              |\n";
        std::cout << "        |                                  |\n";
        std::cout << "        ====================================\n";
    }

    void mostrarMenu()
    {
        system("cls");
        std::cout << "\n\n\n\n\n";
        std::cout << "        ====================================\n";
        std::cout << "        |          MENU PRINCIPAL          |\n";
        std::cout << "        ====================================\n\n";

        if (menuOpcion == 0)
            std::cout << "        >>    PLAY                     <<\n";
        else
            std::cout << "              PLAY                      \n";

        if (menuOpcion == 1)
            std::cout << "        >>    EXIT                     <<\n";
        else
            std::cout << "              EXIT                      \n";

        std::cout << "\n        Use UP/DOWN para navegar\n";
        std::cout << "        Presiona SPACE para seleccionar\n";
    }

    void mostrarGameOver()
    {
        system("cls");
        std::cout << "\n\n\n\n\n\n";
        std::cout << "        ====================================\n";
        std::cout << "        |            GAME OVER             |\n";
        std::cout << "        ====================================\n\n";
        std::cout << "        " << mensajeGameOver << "\n\n";
        std::cout << "        El juego se cerrará en 5 segundos...\n";
    }
};

int main()
{
    srand(static_cast<unsigned int>(time(NULL)));

    Config config;
    if (!config.cargar("config.txt")) return 1;

    GameManager gameManager;
    MapaDeJuego mapa;
    Player jugador;
    VistaDeJuego vista;

    int totalPeatones = config.peatonesLos + config.peatonesSan + config.peatonesVenturas;
    Peaton* peatones = new Peaton[totalPeatones];
    Dinero* dinero = new Dinero[totalPeatones * 2]; // Más dinero por si acaso

    // Coches (5 por isla)
    int totalCoches = 15;
    Coche* coches = new Coche[totalCoches];

    BigSmoke bigSmoke;

    bool juegoInicializado = false;
    bool running = true;

    gameManager.tiempoInicio = clock();

    while (running)
    {
        switch (gameManager.estado)
        {
        case GameState::INIT:
            gameManager.mostrarInit();
            if (clock() - gameManager.tiempoInicio > 3 * CLOCKS_PER_SEC)
            {
                gameManager.estado = GameState::MENU;
            }
            break;

        case GameState::MENU:
            gameManager.mostrarMenu();

            if (GetAsyncKeyState(VK_UP) & 0x8000)
            {
                gameManager.menuOpcion = (gameManager.menuOpcion - 1 + 2) % 2;
                Sleep(200);
            }
            else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
            {
                gameManager.menuOpcion = (gameManager.menuOpcion + 1) % 2;
                Sleep(200);
            }
            else if (GetAsyncKeyState(VK_SPACE) & 0x8000)
            {
                if (gameManager.menuOpcion == 0)
                {
                    gameManager.estado = GameState::GAME;
                    juegoInicializado = false;
                }
                else
                {
                    running = false;
                }
                Sleep(200);
            }
            break;

        case GameState::GAME:
            if (!juegoInicializado)
            {
                // Inicializar juego
                mapa.inicializar(config.mapWidth, config.mapHeight);
                jugador.inicializar(mapa, config);

                // Inicializar peatones
                int peatonIndex = 0;
                for (int isla = 0; isla < 3; ++isla)
                {
                    int numPeatones;
                    switch (isla)
                    {
                    case 0: numPeatones = config.peatonesLos; break;
                    case 1: numPeatones = config.peatonesSan; break;
                    case 2: numPeatones = config.peatonesVenturas; break;
                    }

                    for (int i = 0; i < numPeatones; ++i)
                    {
                        peatones[peatonIndex].inicializar(mapa, isla, config);
                        peatonIndex++;
                    }
                }

                // Inicializar coches
                for (int i = 0; i < totalCoches; ++i)
                {
                    coches[i].inicializar(mapa, i / 5); // 5 coches por isla
                }

                // Inicializar Big Smoke
                bigSmoke.inicializar(mapa);

                juegoInicializado = true;
            }

            // Lógica del juego
            vista.actualizar(jugador.pos, mapa.width, mapa.height);

            // Renderizar
            system("cls");
            for (int i = 0; i < vista.vistaHeight; ++i)
            {
                for (int j = 0; j < vista.vistaWidth; ++j)
                {
                    int y = i + vista.offsetY;
                    int x = j + vista.offsetX;

                    if (y >= 0 && y < mapa.height && x >= 0 && x < mapa.width)
                    {
                        if (y == jugador.pos.y && x == jugador.pos.x && !jugador.enCoche)
                        {
                            switch (jugador.dir)
                            {
                            case Direcciones::UP: std::cout << '^'; break;
                            case Direcciones::DOWN: std::cout << 'v'; break;
                            case Direcciones::LEFT: std::cout << '<'; break;
                            case Direcciones::RIGHT: std::cout << '>'; break;
                            }
                        }
                        else
                        {
                            switch (mapa.celdas[y][x])
                            {
                            case TipoDeCelda::VACIO: std::cout << ' '; break;
                            case TipoDeCelda::PARED: std::cout << 'X'; break;
                            case TipoDeCelda::PEATON: std::cout << 'P'; break;
                            case TipoDeCelda::DINERO: std::cout << '$'; break;
                            case TipoDeCelda::COCHE: std::cout << 'C'; break;
                            case TipoDeCelda::PEAJE: std::cout << 'T'; break;
                            case TipoDeCelda::BIG_SMOKE: std::cout << 'B'; break;
                            default: std::cout << ' '; break;
                            }
                        }
                    }
                    else
                    {
                        std::cout << ' ';
                    }
                }
                std::cout << "\n";
            }

            std::cout << "Dinero: $" << jugador.dinero << " | Vida: " << jugador.vida;
            if (jugador.enCoche) std::cout << " | EN COCHE";
            std::cout << "\n";

            // Controles
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            {
                running = false;
            }

            bool dineroRecogido = false;
            if (GetAsyncKeyState(VK_UP) & 0x8000)
            {
                dineroRecogido = jugador.mover(Direcciones::UP, mapa, coches, totalCoches);
            }
            else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
            {
                dineroRecogido = jugador.mover(Direcciones::DOWN, mapa, coches, totalCoches);
            }
            else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
            {
                dineroRecogido = jugador.mover(Direcciones::LEFT, mapa, coches, totalCoches);
            }
            else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
            {
                dineroRecogido = jugador.mover(Direcciones::RIGHT, mapa, coches, totalCoches);
            }

            if (dineroRecogido)
            {
                jugador.recogerDinero(dinero, totalPeatones * 2);
            }

            // Subir/Bajar coche
            if (GetAsyncKeyState('E') & 0x8000)
            {
                jugador.subirBajarCoche(mapa, coches, totalCoches);
                Sleep(200);
            }

            // Atacar
            if (GetAsyncKeyState(VK_SPACE) & 0x8000)
            {
                clock_t ahora = clock();
                if (ahora - jugador.ultimoAtaque > CLOCKS_PER_SEC / 2) // Cooldown de 0.5 segundos
                {
                    // Atacar peatones
                    for (int i = 0; i < totalPeatones; ++i)
                    {
                        if (!peatones[i].vivo) continue;

                        int dx = abs(peatones[i].pos.x - jugador.pos.x);
                        int dy = abs(peatones[i].pos.y - jugador.pos.y);

                        if (dx <= 1 && dy <= 1)
                        {
                            peatones[i].vida -= jugador.ataque;

                            if (peatones[i].vida <= 0)
                            {
                                peatones[i].vivo = false;
                                mapa.celdas[peatones[i].pos.y][peatones[i].pos.x] = TipoDeCelda::DINERO;

                                // Crear dinero
                                for (int j = 0; j < totalPeatones * 2; ++j)
                                {
                                    if (!dinero[j].activo)
                                    {
                                        dinero[j].pos = peatones[i].pos;
                                        dinero[j].activo = true;

                                        // Cantidad según isla
                                        switch (peatones[i].isla)
                                        {
                                        case 0: dinero[j].cantidad = 1 + rand() % config.maxDineroLos; break;
                                        case 1: dinero[j].cantidad = 1 + rand() % config.maxDineroSan; break;
                                        case 2: dinero[j].cantidad = 1 + rand() % config.maxDineroVenturas; break;
                                        }
                                        break;
                                    }
                                }
                            }
                            jugador.ultimoAtaque = ahora;
                            break;
                        }
                    }

                    // Atacar Big Smoke
                    if (bigSmoke.vivo)
                    {
                        int dx = abs(bigSmoke.pos.x - jugador.pos.x);
                        int dy = abs(bigSmoke.pos.y - jugador.pos.y);

                        if (dx <= 1 && dy <= 1)
                        {
                            bigSmoke.vida -= jugador.ataque;

                            if (bigSmoke.vida <= 0)
                            {
                                bigSmoke.vivo = false;
                                mapa.celdas[bigSmoke.pos.y][bigSmoke.pos.x] = TipoDeCelda::VACIO;
                                gameManager.mensajeGameOver = "¡Has derrotado a Big Smoke! ¡Victoria!";
                                gameManager.estado = GameState::GAME_OVER;
                            }
                            jugador.ultimoAtaque = ahora;
                        }
                    }
                }
                Sleep(100);
            }

            // Actualizar peatones
            for (int i = 0; i < totalPeatones; ++i)
            {
                if (peatones[i].vivo)
                {
                    peatones[i].mover(mapa, jugador.pos);

                    // Peatón ataca jugador
                    if (peatones[i].atacarJugador(jugador))
                    {
                        if (jugador.vida <= 0)
                        {
                            gameManager.mensajeGameOver = "CJ ha muerto. Game Over.";
                            gameManager.estado = GameState::GAME_OVER;
                        }
                    }
                }
            }

            // Big Smoke ataca jugador
            if (bigSmoke.vivo && bigSmoke.atacarJugador(jugador))
            {
                if (jugador.vida <= 0)
                {
                    gameManager.mensajeGameOver = "Big Smoke te ha derrotado. Game Over.";
                    gameManager.estado = GameState::GAME_OVER;
                }
            }

            // Atropellar peatones con coche
            if (jugador.enCoche)
            {
                for (int i = 0; i < totalPeatones; ++i)
                {
                    if (!peatones[i].vivo) continue;

                    if (peatones[i].pos.x == jugador.pos.x && peatones[i].pos.y == jugador.pos.y)
                    {
                        peatones[i].vivo = false;
                        mapa.celdas[peatones[i].pos.y][peatones[i].pos.x] = TipoDeCelda::VACIO;

                        // Buscar posición libre alrededor para el dinero
                        int dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
                        int dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

                        for (int j = 0; j < 8; ++j)
                        {
                            int nx = peatones[i].pos.x + dx[j];
                            int ny = peatones[i].pos.y + dy[j];

                            if (nx >= 0 && nx < mapa.width && ny >= 0 && ny < mapa.height &&
                                mapa.celdas[ny][nx] == TipoDeCelda::VACIO)
                            {
                                mapa.celdas[ny][nx] = TipoDeCelda::DINERO;

                                // Crear dinero
                                for (int k = 0; k < totalPeatones * 2; ++k)
                                {
                                    if (!dinero[k].activo)
                                    {
                                        dinero[k].pos = { nx, ny };
                                        dinero[k].activo = true;

                                        // Cantidad según isla
                                        switch (peatones[i].isla)
                                        {
                                        case 0: dinero[k].cantidad = 1 + rand() % config.maxDineroLos; break;
                                        case 1: dinero[k].cantidad = 1 + rand() % config.maxDineroSan; break;
                                        case 2: dinero[k].cantidad = 1 + rand() % config.maxDineroVenturas; break;
                                        }
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }

            // Verificar condición de muerte
            if (jugador.vida <= 0 && gameManager.estado != GameState::GAME_OVER)
            {
                gameManager.mensajeGameOver = "CJ se ha quedado sin vida. Game Over.";
                gameManager.estado = GameState::GAME_OVER;
            }

            Sleep(ACT_TIEMPO);
            break;

        case GameState::GAME_OVER:
            gameManager.mostrarGameOver();
            Sleep(5000); // Esperar 5 segundos
            running = false;
            break;
        }
    }

    // Limpiar memoria
    delete[] peatones;
    delete[] dinero;
    delete[] coches;

    return 0;
}
