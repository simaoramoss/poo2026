/**
 * @file Sensor.cpp
 * @brief Implementacao dos sensores.
 */
#include "Sensor.h"
#include "Util.h"

#include <cmath>

using namespace std;

/** @brief Limita @p v ao intervalo [min, max]. */
static double Limitar(double v, double min, double max)
{
    if (v < min)
        return min;
    if (v > max)
        return max;
    return v;
}

Sensor::Sensor(int _id, const string &_marca, double _valor, double _limite,
               double _prob_avaria, double _x, double _y)
    : Dispositivo(_id, _marca, _prob_avaria, _x, _y),
      valor(_valor), limite(_limite), avariado(false), forcado(false), alerta_ant(false)
{
}

Sensor::~Sensor() {}

void Sensor::Forcar_Valor(double v)
{
    valor = v;
    forcado = true;
}

bool Sensor::Em_Alerta() const
{
    return !avariado && Condicao_Alerta();
}

bool Sensor::Novo_Alerta()
{
    bool agora = Em_Alerta();
    bool novo = agora && !alerta_ant;
    alerta_ant = agora;
    return novo;
}

bool Sensor::Simular(double hora, double horas)
{
    if (avariado)
        return false;
    if (Util::Acontece(Get_Prob_Avaria() * horas)) {
        avariado = true;
        Contar_Avaria();
        return true;
    }
    if (!forcado)
        Ler(hora, horas);
    return false;
}

void Sensor::Manutencao()
{
    avariado = false;
    forcado = false;
    valor = Valor_Normal();
    alerta_ant = Em_Alerta();
}

void Sensor::Mostrar(ostream &f) const
{
    f << "SENSOR " << Get_ID() << " [" << Tipo() << "] marca=" << Get_Marca()
      << " pos=(" << Util::Para_Texto(Get_X(), 0) << "," << Util::Para_Texto(Get_Y(), 0) << ")"
      << " valor=" << Util::Para_Texto(valor, 1)
      << " limite=" << Util::Para_Texto(limite, 1)
      << " estado=" << (avariado ? "AVARIADO" : (Em_Alerta() ? "ALERTA" : "OK"))
      << " avarias=" << Get_N_Avarias();
}

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
    if (t == "MISSEIS" || t == "MISSEL" || t == "MISSIL" || t == "MISSEIL")
        return new SensorMisseis(id, marca, valor, limite < 0 ? 1.0 : limite, prob_avaria, x, y);
    return NULL;
}

// -------------------------------------------------------------- SensorLuz

SensorLuz::SensorLuz(int _id, const string &_marca, double _valor, double _limite,
                     double _prob_avaria, double _x, double _y)
    : Sensor(_id, _marca, _valor, _limite, _prob_avaria, _x, _y)
{
}

SensorLuz::~SensorLuz() {}

void SensorLuz::Ler(double hora, double)
{
    // luz natural entre as 6h e as 20h, com o maximo a meio do dia
    double luz = 0.0;
    if (hora > 6.0 && hora < 20.0)
        luz = 100.0 * sin(3.14159265358979 * (hora - 6.0) / 14.0);
    Set_Valor_Interno(Limitar(luz + Util::Aleatorio(-2.0, 2.0), 0.0, 100.0));
}

// --------------------------------------------------------- SensorHumidade

SensorHumidade::SensorHumidade(int _id, const string &_marca, double _valor, double _limite,
                               double _prob_avaria, double _x, double _y)
    : Sensor(_id, _marca, _valor, _limite, _prob_avaria, _x, _y)
{
}

SensorHumidade::~SensorHumidade() {}

void SensorHumidade::Ler(double, double horas)
{
    // passeio aleatorio com tendencia para o valor normal; fugas de agua raras
    double v = Get_Valor();
    v += Util::Aleatorio(-2.0, 2.0) + (Valor_Normal() - v) * 0.3 * horas;
    if (Util::Acontece(0.01 * horas))
        v += 45.0;
    Set_Valor_Interno(Limitar(v, 0.0, 100.0));
}

// ------------------------------------------------------------- SensorFumo

SensorFumo::SensorFumo(int _id, const string &_marca, double _valor, double _limite,
                       double _prob_avaria, double _x, double _y)
    : Sensor(_id, _marca, _valor, _limite, _prob_avaria, _x, _y)
{
}

SensorFumo::~SensorFumo() {}

void SensorFumo::Ler(double, double horas)
{
    // o fumo dissipa-se; raramente surge um foco de fumo
    double v = Get_Valor() * exp(-2.0 * horas) + Util::Aleatorio(0.0, 1.0);
    if (Util::Acontece(0.005 * horas))
        v += 80.0;
    Set_Valor_Interno(Limitar(v, 0.0, 100.0));
}

// ---------------------------------------------------------- SensorMisseis

SensorMisseis::SensorMisseis(int _id, const string &_marca, double _valor, double _limite,
                             double _prob_avaria, double _x, double _y)
    : Sensor(_id, _marca, _valor, _limite, _prob_avaria, _x, _y)
{
}

SensorMisseis::~SensorMisseis() {}

void SensorMisseis::Ler(double, double horas)
{
    // 0 = ceu limpo; 1 = missil detetado (acontecimento muito raro)
    Set_Valor_Interno(Util::Acontece(0.001 * horas) ? 1.0 : 0.0);
}
