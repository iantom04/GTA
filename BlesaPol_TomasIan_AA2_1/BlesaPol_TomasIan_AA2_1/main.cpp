// main.cpp
#include <windows.h>
#include <iostream>
#include "Config.h"
#include "GameManager.h"

enum GameState {
    STATE_INIT,
    STATE_MENU,
    STATE_GAME,
    STATE_GAMEOVER
};

template<typename T>
void sleepMs(T ms) { Sleep(ms); }

int main() {
    GameState estado = STATE_INIT;
    Configuracion config = cargarConfiguracion("config.txt");
    GameManager game(config);

    while (true) {
        switch (estado) {
        case STATE_INIT:
            std::cout << "\n\tGTA: ENTI CITY\n\tLoading..." << std::endl;
            sleepMs(3000);
            estado = STATE_MENU;
            break;

        case STATE_MENU:
            std::cout << "\n--- MENU ---\nPresiona ESPACIO para jugar o ESC para salir" << std::endl;
            if (GetAsyncKeyState(VK_SPACE)) {
                estado = STATE_GAME;
            }
            else if (GetAsyncKeyState(VK_ESCAPE)) {
                return 0;
            }
            sleepMs(100);
            break;

        case STATE_GAME:
            game.procesarInput();
            game.actualizar();
            game.renderizar();
            if (game.juegoTerminado()) estado = STATE_GAMEOVER;
            sleepMs(100);
            break;

        case STATE_GAMEOVER:
            std::cout << "\n\tGAME OVER" << std::endl;
            sleepMs(5000);
            return 0;
        }
    }

    return 0;
}