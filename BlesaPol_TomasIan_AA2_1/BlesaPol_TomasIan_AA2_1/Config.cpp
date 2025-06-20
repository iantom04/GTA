// Config.cpp
#include "Config.h"
#include <fstream>
#include <iostream>
#include <sstream>

Configuracion cargarConfiguracion(const std::string& filename) {
    Configuracion config;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo de configuración: " << filename << std::endl;
        exit(1);
    }

    std::string linea;

   
    std::getline(file, linea);
    sscanf_s(linea.c_str(), "%d;%d;", &config.anchoMapa, &config.altoMapa);

  
    std::getline(file, linea);
    sscanf_s(linea.c_str(), "%d;%d;", &config.vidaCJ, &config.ataqueCJ);

    
    std::getline(file, linea);
    sscanf_s(linea.c_str(), "%d;%d;", &config.peaje1, &config.peaje2);

   
    std::getline(file, linea);
    sscanf_s(linea.c_str(), "%d;%d;%d;%d;",
        &config.losSantos.numPeatones,
        &config.losSantos.dineroMax,
        &config.losSantos.vidaPeaton,
        &config.losSantos.ataquePeaton);

    
    std::getline(file, linea);
    sscanf_s(linea.c_str(), "%d;%d;%d;%d;",
        &config.sanFierro.numPeatones,
        &config.sanFierro.dineroMax,
        &config.sanFierro.vidaPeaton,
        &config.sanFierro.ataquePeaton);

    
    std::getline(file, linea);
    sscanf_s(linea.c_str(), "%d;%d;%d;%d;",
        &config.lasVenturas.numPeatones,
        &config.lasVenturas.dineroMax,
        &config.lasVenturas.vidaPeaton,
        &config.lasVenturas.ataquePeaton);

    file.close();
    return config;
}