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
 * Cada tipo concreto define quando esta em alerta (Condicao_Alerta()),
 * comparando o valor lido com um limite configuravel.
 */
class Sensor : public Dispositivo {
    double valor;   ///< Ultima leitura.
    double limite;  ///< Limiar que dispara o aviso.
    bool avariado;  ///< Sensor avariado (nao avisa).

protected:
    /** @brief Condicao de alerta especifica do tipo de sensor. */
    virtual bool Condicao_Alerta() const = 0;

public:
    Sensor(int _id, const std::string &_marca, double _valor, double _limite,
           double _prob_avaria, double _x, double _y);
    virtual ~Sensor();

    double Get_Valor() const { return valor; }
    double Get_Limite() const { return limite; }
    void Set_Valor(double v) { valor = v; }
    virtual bool Esta_Avariado() const { return avariado; }

    /** @brief Indica se a leitura atual justifica um aviso (sensores avariados nunca avisam). */
    bool Em_Alerta() const;

    virtual void Mostrar(std::ostream &f) const;

    /**
     * @brief Cria um sensor a partir do nome do tipo ("LUZ", "HUMIDADE", "FUMO", "MISSEIS").
     * @param limite Limiar de aviso; valores negativos usam o limite por omissao do tipo.
     * @return Novo sensor (a libertar por quem chama) ou NULL se o tipo e desconhecido.
     */
    static Sensor *Criar(const std::string &tipo, int id, const std::string &marca, double valor,
                         double limite, double prob_avaria, double x, double y);
};

/** @brief Sensor de luminosidade (0..100): avisa quando o valor desce abaixo do limite. */
class SensorLuz : public Sensor {
protected:
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
    virtual bool Condicao_Alerta() const { return Get_Valor() >= Get_Limite(); }

public:
    SensorMisseis(int _id, const std::string &_marca, double _valor, double _limite,
                  double _prob_avaria, double _x, double _y);
    virtual ~SensorMisseis();
    virtual std::string Tipo() const { return "MISSEIS"; }
};

#endif
