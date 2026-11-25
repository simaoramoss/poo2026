/**
 * @file User.cpp
 * @brief Implementacao dos utilizadores.
 */
#include "User.h"
#include "Util.h"

using namespace std;

// Construtor e destrutor da classe base
User::User(int _id, const string &_nome) : id(_id), nome(_nome) {}
User::~User() {}

// Escreve "USER id [TIPO] nome"
void User::Mostrar(ostream &f) const
{
    f << "USER " << id << " [" << Tipo() << "] " << nome;
}

// Factory: cria o utilizador certo a partir do tipo em texto (vindo do XML)
User *User::Criar(const string &tipo, int id, const string &nome)
{
    string t = Util::Maiusculas(Util::Aparar(tipo));
    if (t == "ADMIN" || t == "ADMINISTRADOR")
        return new Admin(id, nome);
    if (t == "NORMAL")
        return new Normal(id, nome);
    if (t == "VISITANTE")
        return new Visitante(id, nome);
    return NULL;
}

Admin::Admin(int _id, const string &_nome) : User(_id, _nome) {}
Admin::~Admin() {}
bool Admin::Pode(PERMISSAO) const
{
    return true; // o administrador pode tudo
}

Normal::Normal(int _id, const string &_nome) : User(_id, _nome) {}
Normal::~Normal() {}
// Normal pode tudo menos remover
bool Normal::Pode(PERMISSAO p) const
{
    return p != P_REMOVER;
}

Visitante::Visitante(int _id, const string &_nome) : User(_id, _nome) {}
Visitante::~Visitante() {}
// Visitante so pode listar
bool Visitante::Pode(PERMISSAO p) const
{
    return p == P_LISTAR;
}
