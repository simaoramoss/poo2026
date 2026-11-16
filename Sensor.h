/**
 * @file Sensor.h
 * @brief Sensores da fabrica (classe base e tipos especificos).
 */
#ifndef SENSOR_H
#define SENSOR_H

#include "Dispositivo.h"

/**
 * @class Sensor
 * @brief SENSOR (ID, MARCA, VALOR, PROBABILIDADE_AVARIA).
 *
 * Cada tipo concreto define como o valor evolui (Ler()) e quando esta em
 * alerta (Em_Alerta()), comparando o valor com um limite configuravel.
 */
class Sensor : public Dispositivo {
    double valor;       ///< Ultima leitura.
    double limite;      ///< Limiar que dispara o aviso.
    bool avariado;      ///< Sensor avariado (nao le nem avisa).
    bool forcado;       ///< Valor imposto manualmente (a simulacao nao o altera).
    bool alerta_ant;    ///< Estado de alerta no passo anterior (deteta transicoes).

protected:
    void Set_Valor_Interno(double v) { valor = v; }

    /**
     * @brief Produz uma nova leitura.
     * @param hora  Hora do dia (0..24, com fracao).
     * @param horas Duracao do passo de simulacao, em horas.
     */
    virtual void Ler(double hora, double horas) = 0;

    /** @brief Valor "normal" para onde o sensor volta apos manutencao. */
    virtual double Valor_Normal() const = 0;

public:
    Sensor(int _id, const std::string &_marca, double _valor, double _limite,
           double _prob_avaria, double _x, double _y);
    virtual ~Sensor();

    double Get_Valor() const { return valor; }
    double Get_Limite() const { return limite; }
    virtual bool Esta_Avariado() const { return avariado; }

    /** @brief Impoe um valor ao sensor (para testes/demonstracao dos avisos). */
    void Forcar_Valor(double v);

    /** @brief Indica se a leitura atual justifica um aviso (sensores avariados nunca avisam). */
    bool Em_Alerta() const;

    /** @brief true apenas no passo em que o sensor ENTRA em alerta. */
    bool Novo_Alerta();

    /**
     * @brief Avanca a simulacao do sensor: pode avariar e, se estiver bom, faz nova leitura.
     * @return true se o sensor avariou neste passo.
     */
    bool Simular(double hora, double horas);

    /** @brief Manutencao: repara o sensor e repoe uma leitura normal. */
    virtual void Manutencao();

    virtual void Mostrar(std::ostream &f) const;

    /**
     * @brief Fabrica de sensores a partir do nome do tipo
     *        ("LUZ", "HUMIDADE", "FUMO", "MISSEIS").
     * @param limite Limiar de aviso; valores negativos usam o limite por omissao do tipo.
     * @return Novo sensor (a libertar por quem chama) ou NULL se o tipo e desconhecido.
     */
    static Sensor *Criar(const std::string &tipo, int id, const std::string &marca, double valor,
                         double limite, double prob_avaria, double x, double y);

protected:
    /** @brief Condicao de alerta especifica do tipo de sensor. */
    virtual bool Condicao_Alerta() const = 0;
};

/** @brief Sensor de luminosidade (0..100): avisa quando o valor desce abaixo do limite. */
class SensorLuz : public Sensor {
protected:
    virtual void Ler(double hora, double horas);
    virtual double Valor_Normal() const { return 70.0; }
    virtual bool Condicao_Alerta() const { return Get_Valor() < Get_Limite(); }

public:
    SensorLuz(int _id, const std::string &_marca, double _valor, double _limite,
              double _prob_avaria, double _x, double _y);
    virtual ~SensorLuz();
    virtual std::string Tipo() const { return "LUZ"; }
};

/** @brief Sensor de humidade (0..100 %): avisa quando o valor ultrapassa o limite. */
class SensorHumidade : public Sensor {
protected:
    virtual void Ler(double hora, double horas);
    virtual double Valor_Normal() const { return 45.0; }
    virtual bool Condicao_Alerta() const { return Get_Valor() > Get_Limite(); }

public:
    SensorHumidade(int _id, const std::string &_marca, double _valor, double _limite,
                   double _prob_avaria, double _x, double _y);
    virtual ~SensorHumidade();
    virtual std::string Tipo() const { return "HUMIDADE"; }
};

/** @brief Sensor de fumo (0..100): avisa quando o valor ultrapassa o limite. */
class SensorFumo : public Sensor {
protected:
    virtual void Ler(double hora, double horas);
    virtual double Valor_Normal() const { return 0.0; }
    virtual bool Condicao_Alerta() const { return Get_Valor() > Get_Limite(); }

public:
    SensorFumo(int _id, const std::string &_marca, double _valor, double _limite,
               double _prob_avaria, double _x, double _y);
    virtual ~SensorFumo();
    virtual std::string Tipo() const { return "FUMO"; }
};

/** @brief Sensor de misseis (0 ou 1): avisa quando o valor e 1 (>= limite). */
class SensorMisseis : public Sensor {
protected:
    virtual void Ler(double hora, double horas);
    virtual double Valor_Normal() const { return 0.0; }
    virtual bool Condicao_Alerta() const { return Get_Valor() >= Get_Limite(); }

public:
    SensorMisseis(int _id, const std::string &_marca, double _valor, double _limite,
                  double _prob_avaria, double _x, double _y);
    virtual ~SensorMisseis();
    virtual std::string Tipo() const { return "MISSEIS"; }
};

#endif
