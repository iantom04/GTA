// Config.h
#include <string>

struct IslaConfig {
    int numPeatones;
    int dineroMax;
    int vidaPeaton;
    int ataquePeaton;
};

struct Configuracion {
    int anchoMapa, altoMapa;
    int vidaCJ, ataqueCJ;
    int peaje1, peaje2;
    IslaConfig losSantos, sanFierro, lasVenturas;
};

Configuracion cargarConfiguracion(const std::string & filename);
