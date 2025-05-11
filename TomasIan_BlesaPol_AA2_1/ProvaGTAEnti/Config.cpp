#include "Config.h"

bool ReadConfig(Config& config)
{
    std::ifstream file("config.txt");
    if (!file.is_open()) {
        std::cout << "Error: No se pudo abrir el archivo de configuracion" << std::endl;
        return false;
    }

    char separator;
    file >> config.mapWidth >> separator >> config.mapHeight;
    file >> config.PeatonesLos >> separator >> config.dineroSanFierro >> separator >> config.maxDineroLos;
    file >> config.PeatonesSan >> separator >> config.dineroVenturas >> separator >> config.maxDineroSan;

    file.close();
    return true;
}
