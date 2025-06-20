// BigSmoke.cpp
#include "BigSmoke.h"

BigSmoke::BigSmoke(int px, int py, int vida, int ataque)
    : Walker(px, py, vida, ataque, true) {
    simbolo = 'B';
}