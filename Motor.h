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
 * Maquina de estados: STOP -> START -> RUN, podendo passar a AVARIADO enquanto
 * trabalha. Os metodos Start/Run/Restart/Stop/Estou_Quente sao virtuais e podem
 * ser reimplementados pelos motores especificos.
 */
class Motor : public Dispositivo {
    double consumo_hora;     ///< Consumo por hora de funcionamento (kWh).
    double temp_aviso;       ///< Temperatura a partir da qual a classe e AMARELO.
    double temp_paragem;     ///< Temperatura a partir da qual a classe e VERMELHO.
    ESTADO_MOTOR estado;     ///< Estado atual.
    double temperatura;      ///< Temperatura atual (graus C).
    double horas_trabalho;   ///< Total de horas em RUN.
    double consumo_dia;      ///< Consumo acumulado no dia corrente.
    double consumo_total;    ///< Consumo acumulado desde o inicio.
    CLASSE_TEMP ultima_classe; ///< Ultima classe comunicada (evita avisos repetidos).

protected:
    /** @brief Temperatura ambiente para a qual o motor arrefece quando parado. */
    static const double TEMP_AMBIENTE;

    void Set_Estado(ESTADO_MOTOR e) { estado = e; }
    void Set_Temperatura(double t);
    /** @brief Soma consumo aos acumulados (dia e total). */
    void Somar_Consumo(double kwh);

    /** @brief Graus por hora que o motor aquece em RUN (depende do tipo). */
    virtual double Taxa_Aquecimento() const = 0;

public:
    Motor(int _id, const std::string &_marca, double _consumo_hora, double _temp_aviso,
          double _temp_paragem, double _prob_avaria, double _x, double _y);
    virtual ~Motor();

    double Get_Consumo_Hora() const { return consumo_hora; }
    double Get_Temp_Aviso() const { return temp_aviso; }
    double Get_Temp_Paragem() const { return temp_paragem; }
    ESTADO_MOTOR Get_Estado() const { return estado; }
    double Get_Temperatura() const { return temperatura; }
    double Get_Horas_Trabalho() const { return horas_trabalho; }
    double Get_Consumo_Dia() const { return consumo_dia; }
    double Get_Consumo_Total() const { return consumo_total; }

    virtual bool Esta_Avariado() const { return estado == ESTADO_AVARIADO; }
    /** @brief true se o motor esta em START ou RUN. */
    bool Esta_Ligado() const { return estado == ESTADO_START || estado == ESTADO_RUN; }

    /** @brief Arranque: STOP -> START. @return false se estiver avariado ou ja ligado. */
    virtual bool Start();
    /** @brief Entrada em regime: START -> RUN. @return false se nao estiver em START. */
    virtual bool Run();
    /** @brief Para e volta a arrancar o motor. @return false se estiver avariado. */
    virtual bool Restart();
    /** @brief Paragem: qualquer estado (exceto AVARIADO) -> STOP. */
    virtual bool Stop();
    /** @brief Classe de temperatura atual (VERDE, AMARELO ou VERMELHO). */
    virtual CLASSE_TEMP Estou_Quente() const;

    /**
     * @brief Avanca a simulacao do motor em @p horas.
     *
     * Em START passa a RUN; em RUN aquece, consome, acumula horas de trabalho e
     * pode avariar; parado arrefece ate a temperatura ambiente.
     * @return true se o motor avariou neste passo.
     */
    virtual bool Simular(double horas);

    /**
     * @brief Indica se a classe de temperatura mudou desde a ultima consulta
     *        (para o aviso ao gestor ser enviado uma unica vez).
     */
    bool Mudou_Classe();

    /**
     * @brief Manutencao: repara o motor (AVARIADO -> STOP) e coloca a temperatura
     *        num valor aleatorio dentro da zona VERDE [@p verde_min, @p verde_max].
     */
    virtual void Manutencao(double verde_min, double verde_max);

    /** @brief Coloca a zero o consumo do dia (mudanca de dia). */
    void Novo_Dia() { consumo_dia = 0.0; }

    virtual void Mostrar(std::ostream &f) const;

    /**
     * @brief Fabrica de motores a partir do nome do tipo
     *        ("ELETRICO", "COMBUSTAO", "INDUCAO").
     * @return Novo motor (a libertar por quem chama) ou NULL se o tipo e desconhecido.
     */
    static Motor *Criar(const std::string &tipo, int id, const std::string &marca,
                        double consumo_hora, double temp_aviso, double temp_paragem,
                        double prob_avaria, double x, double y);
};

/** @brief Motor eletrico: aquecimento moderado. */
class MotorEletrico : public Motor {
protected:
    virtual double Taxa_Aquecimento() const { return 12.0; }

public:
    MotorEletrico(int _id, const std::string &_marca, double _consumo_hora, double _temp_aviso,
                  double _temp_paragem, double _prob_avaria, double _x, double _y);
    virtual ~MotorEletrico();
    virtual std::string Tipo() const { return "ELETRICO"; }
};

/**
 * @brief Motor de combustao: aquece depressa e o arranque tem um custo extra
 *        de combustivel e de temperatura (reimplementa Start()).
 */
class MotorCombustao : public Motor {
protected:
    virtual double Taxa_Aquecimento() const { return 25.0; }

public:
    MotorCombustao(int _id, const std::string &_marca, double _consumo_hora, double _temp_aviso,
                   double _temp_paragem, double _prob_avaria, double _x, double _y);
    virtual ~MotorCombustao();
    virtual std::string Tipo() const { return "COMBUSTAO"; }
    virtual bool Start();
};

/**
 * @brief Motor de inducao: aquece pouco e o Restart() inclui ventilacao forcada,
 *        baixando a temperatura (reimplementa Restart()).
 */
class MotorInducao : public Motor {
protected:
    virtual double Taxa_Aquecimento() const { return 8.0; }

public:
    MotorInducao(int _id, const std::string &_marca, double _consumo_hora, double _temp_aviso,
                 double _temp_paragem, double _prob_avaria, double _x, double _y);
    virtual ~MotorInducao();
    virtual std::string Tipo() const { return "INDUCAO"; }
    virtual bool Restart();
};

#endif
