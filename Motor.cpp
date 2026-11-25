/**
 * @file Motor.cpp
 * @brief Implementacao dos motores.
 */
#include "Motor.h"
#include "Util.h"

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
      estado(ESTADO_STOP), temperatura(TEMP_AMBIENTE)
{
    if (consumo_hora < 0.0)
        consumo_hora = 0.0;
    if (temp_paragem < temp_aviso) // garante aviso <= paragem
        temp_paragem = temp_aviso;
}

Motor::~Motor() {}

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

void Motor::Mostrar(ostream &f) const
{
    f << "MOTOR " << Get_ID() << " [" << Tipo() << "] marca=" << Get_Marca()
      << " pos=(" << Util::Para_Texto(Get_X(), 0) << "," << Util::Para_Texto(Get_Y(), 0) << ")"
      << " estado=" << Texto_Estado(estado)
      << " temp=" << Util::Para_Texto(temperatura, 1) << "C (" << Texto_Classe(Estou_Quente())
      << "; aviso=" << Util::Para_Texto(temp_aviso, 0)
      << ", paragem=" << Util::Para_Texto(temp_paragem, 0) << ")"
      << " consumo=" << Util::Para_Texto(consumo_hora) << "kWh/h"
      << " prob_avaria=" << Get_Prob_Avaria();
}

Motor *Motor::Criar(const string &tipo, int id, const string &marca, double consumo_hora,
                    double temp_aviso, double temp_paragem, double prob_avaria, double x, double y)
{
    string t = Util::Maiusculas(Util::Aparar(tipo));
    if (t == "ELETRICO" || t == "ELECTRICO")
        return new MotorEletrico(id, marca, consumo_hora, temp_aviso, temp_paragem, prob_avaria, x, y);
    if (t == "COMBUSTAO")
        return new MotorCombustao(id, marca, consumo_hora, temp_aviso, temp_paragem, prob_avaria, x, y);
    if (t == "INDUCAO")
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
    Set_Temperatura(Get_Temperatura() + 5.0); // a ignicao aquece o bloco
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
    double t = Get_Temperatura() - 10.0; // ventilacao forcada
    Set_Temperatura(t < TEMP_AMBIENTE ? TEMP_AMBIENTE : t);
    return true;
}
