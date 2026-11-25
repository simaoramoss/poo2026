/**
 * @file Fabrica.h
 * @brief Classe principal: gestao da fabrica.
 */
#ifndef FABRICA_H
#define FABRICA_H

#include <iostream>
#include <list>
#include <string>

#include "Motor.h"
#include "Sensor.h"
#include "User.h"

class XMLNo;

/**
 * @class Fabrica
 * @brief Gere os utilizadores, motores e sensores da fabrica.
 *
 * Gestao de memoria: a fabrica e DONA de todos os utilizadores, motores e
 * sensores que lhe sao entregues (construtor, Add() com sucesso e Load()) e
 * liberta-os no destrutor. Quando um Add() devolve false, o objeto continua a
 * pertencer a quem chamou.
 */
class Fabrica {
    std::string nome;              ///< Nome da fabrica.
    User *atual;                   ///< Utilizador que esta a operar (pertence a lista users).
    std::list<User *> users;       ///< Utilizadores registados.
    std::list<Motor *> motores;    ///< Motores instalados.
    std::list<Sensor *> sensores;  ///< Sensores instalados.

    int hora_abertura;             ///< Hora de abertura (0..23).
    int hora_fecho;                ///< Hora de encerramento (0..24).
    double verde_min;              ///< Inicio da zona VERDE.
    double amarelo_min;            ///< Inicio da zona AMARELO (temperatura de aviso por omissao).
    double vermelho_min;           ///< Inicio da zona VERMELHO (temperatura de paragem por omissao).
    double vermelho_max;           ///< Fim da zona VERMELHO.

    Fabrica(const Fabrica &);            // copia nao permitida
    Fabrica &operator=(const Fabrica &); // atribuicao nao permitida

    // ---- metodos auxiliares privados ----
    bool Pode(PERMISSAO p, const char *operacao) const;
    User *Procurar_User(int id) const;
    Motor *Procurar_Motor(int id) const;
    Sensor *Procurar_Sensor(int id) const;
    bool Inserir(User *ut);
    bool Inserir(Motor *m);
    bool Inserir(Sensor *s);
    void Libertar_Dispositivos();
    void Ler_Configuracao(const XMLNo *raiz);
    void Carregar_No(const XMLNo *no, int &n_ok, int &n_erros);

public:
    /**
     * @brief Constroi a fabrica, sendo dado o utilizador atual.
     * @param ut Utilizador que opera a fabrica; passa a pertencer a fabrica.
     */
    Fabrica(User *ut);

    /** @brief Liberta todos os utilizadores, motores e sensores. */
    ~Fabrica();

    /**
     * @brief Carrega a configuracao (horario, temperaturas, utilizadores,
     *        motores e sensores) de um ficheiro XML.
     * @return false se o ficheiro nao existir ou nao for XML valido.
     */
    bool Load(const std::string &ficheiro);

    /** @brief Adiciona um utilizador (permissao ADD; nivel <= ao do atual; ID unico). */
    bool Add(User *ut);

    /** @brief Adiciona um motor (permissao ADD; ID de motor unico). */
    bool Add(Motor *m);

    /** @brief Adiciona um sensor (permissao ADD; ID de sensor unico). */
    bool Add(Sensor *s);

    /** @brief Lista o estado atual da fabrica (utilizadores, motores e sensores). */
    void Listar(std::ostream &f = std::cout);
};

#endif
