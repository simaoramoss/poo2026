/**
 * @file Motor.cpp
 * @brief Implementacao dos motores.
 */
#include "Motor.h"
#include "Util.h"

#include <cmath>

using namespace std;

const double Motor::TEMP_AMBIENTE = 20.0;

string Texto_Estado(ESTADO_MOTOR e)
{
    switch (e) {
    case ESTADO_STOP: return "STOP";
    case ESTADO_START: return "START";
    case ESTADO_RUN: return "RUN";
    case ESTADO_AVARIADO: return "AVARIADO";
    default: return "INVALIDO";
    }
}

string Texto_Classe(CLASSE_TEMP c)
{
    switch (c) {
    case TEMP_VERDE: return "VERDE";
    case TEMP_AMARELO: return "AMARELO";
    default: return "VERMELHO";
    }
}

Motor::Motor(int _id, const string &_marca, double _consumo_hora, double _temp_aviso,
             double _temp_paragem, double _prob_avaria, double _x, double _y)
    : Dispositivo(_id, _marca, _prob_avaria, _x, _y),
      consumo_hora(_consumo_hora), temp_aviso(_temp_aviso), temp_paragem(_temp_paragem),
      estado(ESTADO_STOP), temperatura(TEMP_AMBIENTE), horas_trabalho(0.0),
      consumo_dia(0.0), consumo_total(0.0), ultima_classe(TEMP_VERDE)
{
    if (consumo_hora < 0.0)
        consumo_hora = 0.0;
    if (temp_paragem < temp_aviso) // garante aviso <= paragem
        temp_paragem = temp_aviso;
}

Motor::~Motor() {}

void Motor::Set_Temperatura(double t)
{
    temperatura = t;
}

void Motor::Somar_Consumo(double kwh)
{
    consumo_dia += kwh;
    consumo_total += kwh;
}

bool Motor::Start()
{
    if (estado != ESTADO_STOP)
        return false;
    estado = ESTADO_START;
    return true;
}

bool Motor::Run()
{
    if (estado != ESTADO_START)
        return false;
    estado = ESTADO_RUN;
    return true;
}

bool Motor::Restart()
{
    if (estado == ESTADO_AVARIADO)
        return false;
    Stop();
    return Start();
}

bool Motor::Stop()
{
    if (estado == ESTADO_AVARIADO)
        return false;
    estado = ESTADO_STOP;
    return true;
}

CLASSE_TEMP Motor::Estou_Quente() const
{
    if (temperatura >= temp_paragem)
        return TEMP_VERMELHO;
    if (temperatura >= temp_aviso)
        return TEMP_AMARELO;
    return TEMP_VERDE;
}

bool Motor::Simular(double horas)
{
    if (estado == ESTADO_START) {
        Run(); // o arranque demora um passo de simulacao
        return false;
    }
    if (estado == ESTADO_RUN) {
        // aquece a um ritmo irregular e perde calor para o ambiente
        double ganho = Taxa_Aquecimento() * Util::Aleatorio(0.5, 1.5) * horas;
        double perda = 0.15 * (temperatura - TEMP_AMBIENTE) * horas;
        temperatura += ganho - perda;
        horas_trabalho += horas;
        Somar_Consumo(consumo_hora * horas);
        if (Util::Acontece(Get_Prob_Avaria() * horas)) {
            estado = ESTADO_AVARIADO;
            Contar_Avaria();
            return true;
        }
        return false;
    }
    // parado ou avariado: arrefece exponencialmente ate a temperatura ambiente
    temperatura = TEMP_AMBIENTE + (temperatura - TEMP_AMBIENTE) * exp(-0.8 * horas);
    return false;
}

bool Motor::Mudou_Classe()
{
    CLASSE_TEMP atual = Estou_Quente();
    bool mudou = (atual != ultima_classe);
    ultima_classe = atual;
    return mudou;
}

void Motor::Manutencao(double verde_min, double verde_max)
{
    // a zona verde do motor termina na sua propria temperatura de aviso
    if (verde_max > temp_aviso)
        verde_max = temp_aviso;
    if (verde_min > verde_max)
        verde_min = verde_max;
    temperatura = Util::Aleatorio(verde_min, verde_max);
    if (estado == ESTADO_AVARIADO)
        estado = ESTADO_STOP;
    ultima_classe = Estou_Quente();
}

void Motor::Mostrar(ostream &f) const
{
    f << "MOTOR " << Get_ID() << " [" << Tipo() << "] marca=" << Get_Marca()
      << " pos=(" << Util::Para_Texto(Get_X(), 0) << "," << Util::Para_Texto(Get_Y(), 0) << ")"
      << " estado=" << Texto_Estado(estado)
      << " temp=" << Util::Para_Texto(temperatura, 1) << "C (" << Texto_Classe(Estou_Quente())
      << "; aviso=" << Util::Para_Texto(temp_aviso, 0)
      << ", paragem=" << Util::Para_Texto(temp_paragem, 0) << ")"
      << " consumo_dia=" << Util::Para_Texto(consumo_dia) << "kWh"
      << " horas=" << Util::Para_Texto(horas_trabalho)
      << " avarias=" << Get_N_Avarias();
}

Motor *Motor::Criar(const string &tipo, int id, const string &marca, double consumo_hora,
                    double temp_aviso, double temp_paragem, double prob_avaria, double x, double y)
{
    string t = Util::Maiusculas(Util::Aparar(tipo));
    if (t == "ELETRICO" || t == "ELECTRICO" || t == "MELETRICO" || t == "MELECTRICO")
        return new MotorEletrico(id, marca, consumo_hora, temp_aviso, temp_paragem, prob_avaria, x, y);
    if (t == "COMBUSTAO" || t == "MCOMBUSTAO")
        return new MotorCombustao(id, marca, consumo_hora, temp_aviso, temp_paragem, prob_avaria, x, y);
    if (t == "INDUCAO" || t == "MINDUCAO")
        return new MotorInducao(id, marca, consumo_hora, temp_aviso, temp_paragem, prob_avaria, x, y);
    return NULL;
}

// ---------------------------------------------------------- MotorEletrico

MotorEletrico::MotorEletrico(int _id, const string &_marca, double _consumo_hora, double _temp_aviso,
                             double _temp_paragem, double _prob_avaria, double _x, double _y)
    : Motor(_id, _marca, _consumo_hora, _temp_aviso, _temp_paragem, _prob_avaria, _x, _y)
{
}

MotorEletrico::~MotorEletrico() {}

// --------------------------------------------------------- MotorCombustao

MotorCombustao::MotorCombustao(int _id, const string &_marca, double _consumo_hora, double _temp_aviso,
                               double _temp_paragem, double _prob_avaria, double _x, double _y)
    : Motor(_id, _marca, _consumo_hora, _temp_aviso, _temp_paragem, _prob_avaria, _x, _y)
{
}

MotorCombustao::~MotorCombustao() {}

bool MotorCombustao::Start()
{
    if (!Motor::Start())
        return false;
    // a ignicao gasta o equivalente a 3 minutos de consumo e aquece o bloco
    Somar_Consumo(Get_Consumo_Hora() * 0.05);
    Set_Temperatura(Get_Temperatura() + 5.0);
    return true;
}

// ----------------------------------------------------------- MotorInducao

MotorInducao::MotorInducao(int _id, const string &_marca, double _consumo_hora, double _temp_aviso,
                           double _temp_paragem, double _prob_avaria, double _x, double _y)
    : Motor(_id, _marca, _consumo_hora, _temp_aviso, _temp_paragem, _prob_avaria, _x, _y)
{
}

MotorInducao::~MotorInducao() {}

bool MotorInducao::Restart()
{
    if (!Motor::Restart())
        return false;
    // ventilacao forcada durante o rearranque
    double t = Get_Temperatura() - 10.0;
    if (t < TEMP_AMBIENTE)
        t = TEMP_AMBIENTE;
    Set_Temperatura(t);
    return true;
}
