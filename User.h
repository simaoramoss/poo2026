/**
 * @file User.h
 * @brief Utilizadores da fabrica e respetivos niveis de acesso.
 */
#ifndef USER_H
#define USER_H

#include <ostream>
#include <string>

/** @brief Operacoes sujeitas a controlo de acesso. */
enum PERMISSAO { P_ADD, P_REMOVER, P_LISTAR, P_RUN, P_MANUTENCAO };

/**
 * @class User
 * @brief Classe base abstrata de um utilizador (ID, NOME).
 *
 * Cada tipo concreto (Admin, Normal, Visitante) define o que pode fazer
 * reimplementando Pode().
 */
class User {
    int id;           ///< Identificador unico.
    std::string nome; ///< Nome do utilizador.

public:
    User(int _id, const std::string &_nome);
    virtual ~User();

    int Get_ID() const { return id; }
    const std::string &Get_Nome() const { return nome; }

    /** @brief Nome do tipo de utilizador ("ADMIN", "NORMAL", "VISITANTE"). */
    virtual std::string Tipo() const = 0;

    /** @brief Indica se o utilizador tem a permissao @p p. */
    virtual bool Pode(PERMISSAO p) const = 0;

    /**
     * @brief Nivel hierarquico (maior = mais privilegios). Usado na regra
     *        "um utilizador so pode adicionar utilizadores de nivel igual ou inferior".
     */
    virtual int Nivel() const = 0;

    /** @brief Escreve uma linha descritiva do utilizador. */
    virtual void Mostrar(std::ostream &f) const;

    /**
     * @brief Fabrica de utilizadores a partir do nome do tipo.
     * @return Novo objeto (a libertar por quem chama) ou NULL se o tipo e desconhecido.
     */
    static User *Criar(const std::string &tipo, int id, const std::string &nome);
};

/** @brief Administrador: ADD, REMOVER, LISTAR, RUN, MANUTENCAO. */
class Admin : public User {
public:
    Admin(int _id, const std::string &_nome);
    virtual ~Admin();
    virtual std::string Tipo() const { return "ADMIN"; }
    virtual bool Pode(PERMISSAO p) const;
    virtual int Nivel() const { return 3; }
};

/** @brief Utilizador normal: ADD, LISTAR, RUN, MANUTENCAO. */
class Normal : public User {
public:
    Normal(int _id, const std::string &_nome);
    virtual ~Normal();
    virtual std::string Tipo() const { return "NORMAL"; }
    virtual bool Pode(PERMISSAO p) const;
    virtual int Nivel() const { return 2; }
};

/** @brief Visitante: apenas LISTAR. */
class Visitante : public User {
public:
    Visitante(int _id, const std::string &_nome);
    virtual ~Visitante();
    virtual std::string Tipo() const { return "VISITANTE"; }
    virtual bool Pode(PERMISSAO p) const;
    virtual int Nivel() const { return 1; }
};

#endif
