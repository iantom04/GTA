// GameManager.cpp
#include "GameManager.h"
#include <windows.h>
#include <iostream>

bool GameManager::esCasillaBloqueada(int x, int y) {
    if (x < 0 || y < 0 || x >= config.anchoMapa || y >= config.altoMapa) return true;
    int tercio = config.anchoMapa / 3;
    bool borde = x == 0 || y == 0 || x == config.anchoMapa - 1 || y == config.altoMapa - 1;
    bool peaje1 = x == tercio && y != config.altoMapa / 2 && !peaje1Pagado;
    bool peaje2 = x == tercio * 2 && y != config.altoMapa / 2 && !peaje2Pagado;
    return borde || peaje1 || peaje2;
}

GameManager::GameManager(const Configuracion& cfg)
    : config(cfg), cj(cfg.vidaCJ, cfg.ataqueCJ), mapa(cfg.altoMapa, cfg.anchoMapa), dinero(0),
    peaje1Pagado(false), peaje2Pagado(false), dentroCoche(false), boss(nullptr) {
    
    for (int i = 0; i < config.losSantos.numPeatones; ++i)
        peatones.emplace_back(rand() % cfg.anchoMapa / 3, rand() % cfg.altoMapa, cfg.losSantos.vidaPeaton, cfg.losSantos.ataquePeaton, rand() % 2);

    
    boss = new BigSmoke(cfg.anchoMapa - 2, cfg.altoMapa / 2, cfg.lasVenturas.vidaPeaton * 2, cfg.lasVenturas.ataquePeaton * 2);

    
    int tercio = config.anchoMapa / 3;

    auto generarCoche = [&](int minX, int maxX) {
        int cx, cy;
        bool casillaValida = false;
        int intentos = 0;

        while (!casillaValida && intentos < 100) {
            cx = minX + rand() % (maxX - minX);
            cy = rand() % config.altoMapa;

            bool ocupada = esCasillaBloqueada(cx, cy);

            for (const auto& w : peatones)
                if (w.x == cx && w.y == cy) ocupada = true;

            for (const auto& car : coches)
                if (car.x == cx && car.y == cy) ocupada = true;

            if (!ocupada) casillaValida = true;
            ++intentos;
        }

        if (casillaValida) coches.emplace_back(cx, cy);
        };

   
    generarCoche(0, tercio);

    
    generarCoche(tercio, tercio * 2);

    
    generarCoche(tercio * 2, config.anchoMapa);

}

GameManager::~GameManager() {
    delete boss;
}

void GameManager::procesarInput() {
    int dx = 0, dy = 0;
    if (GetAsyncKeyState(VK_LEFT)) dx = -1;
    if (GetAsyncKeyState(VK_RIGHT)) dx = 1;
    if (GetAsyncKeyState(VK_UP)) dy = -1;
    if (GetAsyncKeyState(VK_DOWN)) dy = 1;

    if (dx != 0 || dy != 0) {
        int nx, ny;
        if (dentroCoche) {
            for (auto& car : coches) {
                if (car.activo) {
                    nx = car.x + dx;
                    ny = car.y + dy;
                    if (!esCasillaBloqueada(nx, ny)) {
                        car.mover(dx, dy, config.anchoMapa, config.altoMapa);
                    }
                }
            }
        }
        else {
            nx = cj.x + dx;
            ny = cj.y + dy;
            if (!esCasillaBloqueada(nx, ny)) {
                cj.mover(dx, dy, config.anchoMapa, config.altoMapa);
            }
        }
    }


    
    if (GetAsyncKeyState(0x45) & 0x8000) { 
        if (!dentroCoche) {
  
            for (auto& car : coches) {
                if (car.x == cj.x && car.y == cj.y && car.disponible) {
                    dentroCoche = true;
                    car.activo = true;
                    break;
                }
            }
        }
        else {
            
            dentroCoche = false;
            for (auto& car : coches)
                if (car.activo) car.activo = false;
        }
        Sleep(200); 
    }

    if (GetAsyncKeyState(VK_SPACE)) {
        for (auto& w : peatones) {
            if (abs(w.x - cj.x) <= 1 && abs(w.y - cj.y) <= 1 && w.vivo) {
                w.recibirAtaque(cj.ataque);
                if (!w.vivo) dinero += rand() % config.losSantos.dineroMax + 1;
            }
        }
        if (abs(boss->x - cj.x) <= 1 && abs(boss->y - cj.y) <= 1 && boss->vivo)
            boss->recibirAtaque(cj.ataque);
    }
    int tercio = config.anchoMapa / 3;
    if (cj.x == tercio && cj.y == config.altoMapa / 2 && !peaje1Pagado) {
        if (dinero >= config.peaje1) {
            dinero -= config.peaje1;
            peaje1Pagado = true;
        }
        else {
            cj.vida = 0;
        }
    }
    else if (cj.x == tercio * 2 && cj.y == config.altoMapa / 2 && !peaje2Pagado) {
        if (dinero >= config.peaje2) {
            dinero -= config.peaje2;
            peaje2Pagado = true;
        }
        else {
            cj.vida = 0; 
        }
    }
}

void GameManager::actualizar() {
    for (auto& w : peatones) {
        if (!w.vivo) {
           
            int tercio = config.anchoMapa / 3;
            int minX = 0, maxX = tercio;
            IslaConfig isla = config.losSantos;

            if (cj.x >= tercio && cj.x < tercio * 2) {
                minX = tercio;
                maxX = tercio * 2;
                isla = config.sanFierro;
            }
            else if (cj.x >= tercio * 2) {
                minX = tercio * 2;
                maxX = config.anchoMapa;
                isla = config.lasVenturas;
            }

           
            int wx, wy;
            do {
                wx = minX + rand() % (maxX - minX);
                wy = rand() % config.altoMapa;
            } while (esCasillaBloqueada(wx, wy));

            
            w.x = wx;
            w.y = wy;
            w.vivo = true;
            w.vida = isla.vidaPeaton;
            w.ataque = isla.ataquePeaton;
            w.agresivo = rand() % 2;
            w.direccion = rand() % 4;
            continue; 
        }

        if (abs(w.x - cj.x) <= 1 && abs(w.y - cj.y) <= 1) continue;

        int dx = 0, dy = 0;
        switch (w.direccion) {
        case 0: dy = -1; break;
        case 1: dy = 1; break;
        case 2: dx = -1; break;
        case 3: dx = 1; break;
        }
        int nx = w.x + dx;
        int ny = w.y + dy;
        if (!esCasillaBloqueada(nx, ny)) {
            w.mover(config.anchoMapa, config.altoMapa);
        }
        else {
            w.direccion = rand() % 4;
        }
    }

}

void GameManager::renderizar() {
    mapa.limpiar();

    
    for (int i = 0; i < config.anchoMapa; ++i) {
        mapa.colocarElemento(i, 0, 'X');
        mapa.colocarElemento(i, config.altoMapa - 1, 'X');
    }
    for (int j = 0; j < config.altoMapa; ++j) {
        mapa.colocarElemento(0, j, 'X');
        mapa.colocarElemento(config.anchoMapa - 1, j, 'X');
    }

    
    int tercio = config.anchoMapa / 3;
    for (int y = 0; y < config.altoMapa; ++y) {
        mapa.colocarElemento(tercio, y, 'T');
        mapa.colocarElemento(tercio * 2, y, 'T');
    }
    
    mapa.colocarElemento(tercio, config.altoMapa / 2, ' ');
    mapa.colocarElemento(tercio * 2, config.altoMapa / 2, ' ');


    if (!dentroCoche) mapa.colocarElemento(cj.x, cj.y, cj.simbolo);
    for (auto& w : peatones)
        if (w.vivo)
            mapa.colocarElemento(w.x, w.y, w.simbolo);

    for (auto& car : coches)
        if (car.disponible || car.activo)
            mapa.colocarElemento(car.x, car.y, 'C');

    if (boss->vivo) mapa.colocarElemento(boss->x, boss->y, boss->simbolo);

    int focoX = cj.x;
    int focoY = cj.y;

    for (const auto& car : coches) {
        if (car.activo) {
            focoX = car.x;
            focoY = car.y;
            break;
        }
    }

    mapa.mostrar(focoX, focoY);    std::cout << "Vida CJ: " << cj.vida << "  Dinero: " << dinero << std::endl;
}

bool GameManager::juegoTerminado() {
    return (cj.vida <= 0 || !boss->vivo);
}