/**
 * @file Dispositivo.cpp
 * @brief Implementacao da classe base Dispositivo.
 */
#include "Dispositivo.h"

#include <cmath>

using namespace std;

// Construtor: guarda os dados comuns e garante que a probabilidade fica entre 0 e 1
Dispositivo::Dispositivo(int _id, const string &_marca, double _prob_avaria, double _x, double _y)
    : id(_id), marca(_marca), prob_avaria(_prob_avaria), x(_x), y(_y)
{
    if (prob_avaria < 0.0)
        prob_avaria = 0.0;
    if (prob_avaria > 1.0)
        prob_avaria = 1.0;
}

// Destrutor virtual (declarado no .h): garante que o delete de um Motor*/Sensor*
// chama tambem o destrutor da classe filha
Dispositivo::~Dispositivo() {}

// Distancia euclidiana entre dois dispositivos (sera usada nos avisos de humidade)
double Dispositivo::Distancia(const Dispositivo &outro) const
{
    double dx = x - outro.x;
    double dy = y - outro.y;
    return sqrt(dx * dx + dy * dy);
}
