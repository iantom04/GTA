// GameManager.h
#include <vector>
#include "Config.h"
#include "Player.h"
#include "Walker.h"
#include "Car.h"
#include "BigSmoke.h"
#include "Mapa.h"

class GameManager {
private:
    Configuracion config;
    Player cj;
    Mapa mapa;
    std::vector<Walker> peatones;
    std::vector<Car> coches;
    BigSmoke* boss;
    int dinero;
    bool peaje1Pagado, peaje2Pagado;
    bool dentroCoche;
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

public:
    bool esCasillaBloqueada(int x, int y);

    GameManager(const Configuracion& cfg);
    ~GameManager();

    void iniciar();
    void procesarInput();
    void actualizar();
    void renderizar();
    bool juegoTerminado();
};
