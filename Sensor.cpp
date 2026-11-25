/**
 * @file Sensor.cpp
 * @brief Implementacao dos sensores.
 */
#include "Sensor.h"
#include "Util.h"

using namespace std;

// Construtor: constroi a parte Dispositivo e guarda valor e limite
Sensor::Sensor(int _id, const string &_marca, double _valor, double _limite,
               double _prob_avaria, double _x, double _y)
    : Dispositivo(_id, _marca, _prob_avaria, _x, _y), valor(_valor), limite(_limite), avariado(false)
{
}

Sensor::~Sensor() {}

// Igual para todos os sensores; a condicao concreta e decidida pela
// classe filha em Condicao_Alerta() (virtual pura)
bool Sensor::Em_Alerta() const
{
    return !avariado && Condicao_Alerta();
}

// Escreve uma linha com os dados do sensor
void Sensor::Mostrar(ostream &f) const
{
    f << "SENSOR " << Get_ID() << " [" << Tipo() << "] marca=" << Get_Marca()
      << " pos=(" << Util::Para_Texto(Get_X(), 0) << "," << Util::Para_Texto(Get_Y(), 0) << ")"
      << " valor=" << Util::Para_Texto(valor, 1)
      << " limite=" << Util::Para_Texto(limite, 1)
      << " estado=" << (avariado ? "AVARIADO" : (Em_Alerta() ? "ALERTA" : "OK"))
      << " prob_avaria=" << Get_Prob_Avaria();
}

// Factory: cria o sensor certo a partir do tipo em texto.
// Se o XML nao trouxer LIMITE (limite < 0), usa o limite por omissao do tipo.
Sensor *Sensor::Criar(const string &tipo, int id, const string &marca, double valor, double limite,
                      double prob_avaria, double x, double y)
{
    string t = Util::Maiusculas(Util::Aparar(tipo));
    if (t == "LUZ")
        return new SensorLuz(id, marca, valor, limite < 0 ? 20.0 : limite, prob_avaria, x, y);
    if (t == "HUMIDADE")
        return new SensorHumidade(id, marca, valor, limite < 0 ? 80.0 : limite, prob_avaria, x, y);
    if (t == "FUMO")
        return new SensorFumo(id, marca, valor, limite < 0 ? 50.0 : limite, prob_avaria, x, y);
    if (t == "MISSEIS" || t == "MISSEL" || t == "MISSIL")
        return new SensorMisseis(id, marca, valor, limite < 0 ? 1.0 : limite, prob_avaria, x, y);
    return NULL;
}

// Construtores das filhas: so passam os dados ao construtor de Sensor
SensorLuz::SensorLuz(int _id, const string &_marca, double _valor, double _limite,
                     double _prob_avaria, double _x, double _y)
    : Sensor(_id, _marca, _valor, _limite, _prob_avaria, _x, _y)
{
}
SensorLuz::~SensorLuz() {}

SensorHumidade::SensorHumidade(int _id, const string &_marca, double _valor, double _limite,
                               double _prob_avaria, double _x, double _y)
    : Sensor(_id, _marca, _valor, _limite, _prob_avaria, _x, _y)
{
}
SensorHumidade::~SensorHumidade() {}

SensorFumo::SensorFumo(int _id, const string &_marca, double _valor, double _limite,
                       double _prob_avaria, double _x, double _y)
    : Sensor(_id, _marca, _valor, _limite, _prob_avaria, _x, _y)
{
}
SensorFumo::~SensorFumo() {}

SensorMisseis::SensorMisseis(int _id, const string &_marca, double _valor, double _limite,
                             double _prob_avaria, double _x, double _y)
    : Sensor(_id, _marca, _valor, _limite, _prob_avaria, _x, _y)
{
}
SensorMisseis::~SensorMisseis() {}
