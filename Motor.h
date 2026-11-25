/**
 * @file Motor.h
 * @brief Motores da fabrica (classe base e tipos especificos).
 */
#ifndef MOTOR_H
#define MOTOR_H

#include "Dispositivo.h"

/** @brief Estados possiveis de um motor (ESTADO_INVALIDO = motor inexistente). */
enum ESTADO_MOTOR { ESTADO_STOP, ESTADO_START, ESTADO_RUN, ESTADO_AVARIADO, ESTADO_INVALIDO };

/** @brief Classes de temperatura de um motor. */
enum CLASSE_TEMP { TEMP_VERDE, TEMP_AMARELO, TEMP_VERMELHO };

/** @brief Converte um estado para texto. */
std::string Texto_Estado(ESTADO_MOTOR e);
/** @brief Converte uma classe de temperatura para texto. */
std::string Texto_Classe(CLASSE_TEMP c);

/**
 * @class Motor
 * @brief MOTOR (ID, MARCA, CONSUMO_HORA, TEMPERATURA_AVISO, TEMPERATURA_PARAGEM,
 *        PROBABILIDADE_AVARIA).
 *
 * Os metodos Start/Run/Restart/Stop/Estou_Quente sao virtuais e podem ser
 * reimplementados pelos motores especificos.
 */
class Motor : public Dispositivo {
    double consumo_hora;   ///< Consumo por hora de funcionamento (kWh).
    double temp_aviso;     ///< Temperatura a partir da qual a classe e AMARELO.
    double temp_paragem;   ///< Temperatura a partir da qual a classe e VERMELHO.
    ESTADO_MOTOR estado;   ///< Estado atual.
    double temperatura;    ///< Temperatura atual (graus C).

protected:
    /** @brief Temperatura ambiente (temperatura inicial dos motores). */
    static const double TEMP_AMBIENTE;

    void Set_Temperatura(double t) { temperatura = t; }

public:
    Motor(int _id, const std::string &_marca, double _consumo_hora, double _temp_aviso,
          double _temp_paragem, double _prob_avaria, double _x, double _y);
    virtual ~Motor();

    double Get_Consumo_Hora() const { return consumo_hora; }
    double Get_Temp_Aviso() const { return temp_aviso; }
    double Get_Temp_Paragem() const { return temp_paragem; }
    ESTADO_MOTOR Get_Estado() const { return estado; }
    double Get_Temperatura() const { return temperatura; }

    virtual bool Esta_Avariado() const { return estado == ESTADO_AVARIADO; }

    /** @brief Arranque: STOP -> START. @return false se nao estiver em STOP. */
    virtual bool Start();
    /** @brief Entrada em regime: START -> RUN. @return false se nao estiver em START. */
    virtual bool Run();
    /** @brief Para e volta a arrancar o motor. @return false se estiver avariado. */
    virtual bool Restart();
    /** @brief Paragem: qualquer estado (exceto AVARIADO) -> STOP. */
    virtual bool Stop();
    /** @brief Classe de temperatura atual (VERDE, AMARELO ou VERMELHO). */
    virtual CLASSE_TEMP Estou_Quente() const;

    virtual void Mostrar(std::ostream &f) const;

    /**
     * @brief Cria um motor a partir do nome do tipo ("ELETRICO", "COMBUSTAO", "INDUCAO").
     * @return Novo motor (a libertar por quem chama) ou NULL se o tipo e desconhecido.
     */
    static Motor *Criar(const std::string &tipo, int id, const std::string &marca,
                        double consumo_hora, double temp_aviso, double temp_paragem,
                        double prob_avaria, double x, double y);
};

/** @brief Motor eletrico. */
class MotorEletrico : public Motor {
public:
    MotorEletrico(int _id, const std::string &_marca, double _consumo_hora, double _temp_aviso,
                  double _temp_paragem, double _prob_avaria, double _x, double _y);
    virtual ~MotorEletrico();
    virtual std::string Tipo() const { return "ELETRICO"; }
};

/** @brief Motor de combustao: o arranque aquece o bloco (reimplementa Start()). */
class MotorCombustao : public Motor {
public:
    MotorCombustao(int _id, const std::string &_marca, double _consumo_hora, double _temp_aviso,
                   double _temp_paragem, double _prob_avaria, double _x, double _y);
    virtual ~MotorCombustao();
    virtual std::string Tipo() const { return "COMBUSTAO"; }
    virtual bool Start();
};

/** @brief Motor de inducao: o Restart() inclui ventilacao forcada (reimplementa Restart()). */
class MotorInducao : public Motor {
public:
    MotorInducao(int _id, const std::string &_marca, double _consumo_hora, double _temp_aviso,
                 double _temp_paragem, double _prob_avaria, double _x, double _y);
    virtual ~MotorInducao();
    virtual std::string Tipo() const { return "INDUCAO"; }
    virtual bool Restart();
};

#endif
