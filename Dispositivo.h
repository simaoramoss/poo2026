/**
 * @file Dispositivo.h
 * @brief Classe base comum a motores e sensores.
 */
#ifndef DISPOSITIVO_H
#define DISPOSITIVO_H

#include <ostream>
#include <string>

/**
 * @class Dispositivo
 * @brief Equipamento instalado numa posicao (X, Y) da fabrica.
 *
 * Reune o que e comum a SENSOR e MOTOR: ID, MARCA, PROBABILIDADE_AVARIA
 * e posicao.
 */
class Dispositivo {
    int id;              ///< Identificador do dispositivo.
    std::string marca;   ///< Marca / fabricante.
    double prob_avaria;  ///< Probabilidade de avaria por hora de funcionamento (0..1).
    double x;            ///< Posicao X na fabrica.
    double y;            ///< Posicao Y na fabrica.

public:
    Dispositivo(int _id, const std::string &_marca, double _prob_avaria, double _x, double _y);
    virtual ~Dispositivo();

    int Get_ID() const { return id; }
    const std::string &Get_Marca() const { return marca; }
    double Get_Prob_Avaria() const { return prob_avaria; }
    double Get_X() const { return x; }
    double Get_Y() const { return y; }

    /** @brief Distancia euclidiana a outro dispositivo. */
    double Distancia(const Dispositivo &outro) const;

    /** @brief Nome do tipo concreto do dispositivo. */
    virtual std::string Tipo() const = 0;

    /** @brief Indica se o dispositivo esta avariado. */
    virtual bool Esta_Avariado() const = 0;

    /** @brief Escreve uma linha com o estado do dispositivo. */
    virtual void Mostrar(std::ostream &f) const = 0;
};

#endif
