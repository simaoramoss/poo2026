/**
 * @file Fabrica.cpp
 * @brief Implementacao da classe Fabrica.
 */
#include "Fabrica.h"
#include "Util.h"
#include "XML.h"

using namespace std;

// ------------------------------------------------ construcao/destruicao

Fabrica::Fabrica(User *ut)
    : nome("Fabrica"), atual(ut), hora_abertura(8), hora_fecho(18), verde_min(0.0),
      amarelo_min(70.0), vermelho_min(90.0), vermelho_max(150.0)
{
    if (ut != NULL)
        users.push_back(ut);
}

Fabrica::~Fabrica()
{
    Libertar_Dispositivos();
    for (list<User *>::iterator it = users.begin(); it != users.end(); ++it)
        delete *it;
    users.clear();
    atual = NULL;
}

void Fabrica::Libertar_Dispositivos()
{
    for (list<Motor *>::iterator it = motores.begin(); it != motores.end(); ++it)
        delete *it;
    motores.clear();
    for (list<Sensor *>::iterator it = sensores.begin(); it != sensores.end(); ++it)
        delete *it;
    sensores.clear();
}

// ------------------------------------------------------------ pesquisas

bool Fabrica::Pode(PERMISSAO p, const char *operacao) const
{
    if (atual != NULL && atual->Pode(p))
        return true;
    cout << "  Acesso negado: o utilizador "
         << (atual != NULL ? atual->Get_Nome() + " [" + atual->Tipo() + "]" : string("(nenhum)"))
         << " nao pode " << operacao << "." << endl;
    return false;
}

User *Fabrica::Procurar_User(int id) const
{
    for (list<User *>::const_iterator it = users.begin(); it != users.end(); ++it)
        if ((*it)->Get_ID() == id)
            return *it;
    return NULL;
}

Motor *Fabrica::Procurar_Motor(int id) const
{
    for (list<Motor *>::const_iterator it = motores.begin(); it != motores.end(); ++it)
        if ((*it)->Get_ID() == id)
            return *it;
    return NULL;
}

Sensor *Fabrica::Procurar_Sensor(int id) const
{
    for (list<Sensor *>::const_iterator it = sensores.begin(); it != sensores.end(); ++it)
        if ((*it)->Get_ID() == id)
            return *it;
    return NULL;
}

// ------------------------------------------------------------- insercao

bool Fabrica::Inserir(User *ut)
{
    if (ut == NULL || Procurar_User(ut->Get_ID()) != NULL)
        return false;
    users.push_back(ut);
    return true;
}

bool Fabrica::Inserir(Motor *m)
{
    if (m == NULL || Procurar_Motor(m->Get_ID()) != NULL)
        return false;
    motores.push_back(m);
    return true;
}

bool Fabrica::Inserir(Sensor *s)
{
    if (s == NULL || Procurar_Sensor(s->Get_ID()) != NULL)
        return false;
    sensores.push_back(s);
    return true;
}

bool Fabrica::Add(User *ut)
{
    if (ut == NULL || !Pode(P_ADD, "adicionar utilizadores"))
        return false;
    if (ut->Nivel() > atual->Nivel()) {
        cout << "  Um utilizador " << atual->Tipo() << " nao pode criar um utilizador "
             << ut->Tipo() << "." << endl;
        return false;
    }
    if (!Inserir(ut)) {
        cout << "  Ja existe um utilizador com o ID " << ut->Get_ID() << "." << endl;
        return false;
    }
    return true;
}

bool Fabrica::Add(Motor *m)
{
    if (m == NULL || !Pode(P_ADD, "adicionar motores"))
        return false;
    if (!Inserir(m)) {
        cout << "  Ja existe um motor com o ID " << m->Get_ID() << "." << endl;
        return false;
    }
    return true;
}

bool Fabrica::Add(Sensor *s)
{
    if (s == NULL || !Pode(P_ADD, "adicionar sensores"))
        return false;
    if (!Inserir(s)) {
        cout << "  Ja existe um sensor com o ID " << s->Get_ID() << "." << endl;
        return false;
    }
    return true;
}

// ------------------------------------------------------- leitura do XML

void Fabrica::Ler_Configuracao(const XMLNo *raiz)
{
    nome = raiz->Valor("NOME", nome);
    hora_abertura = Util::Para_Int(raiz->Valor("HORA_ABERTURA"), hora_abertura);
    hora_fecho = Util::Para_Int(raiz->Valor("HORA_FECHO"), hora_fecho);
    if (hora_abertura < 0 || hora_abertura > 23)
        hora_abertura = 8;
    if (hora_fecho <= hora_abertura || hora_fecho > 24)
        hora_fecho = (hora_abertura + 10 > 24) ? 24 : hora_abertura + 10;

    // intervalos das classes de temperatura
    const XMLNo *verde = raiz->Procurar("VERDE");
    const XMLNo *amarelo = raiz->Procurar("AMARELO");
    const XMLNo *vermelho = raiz->Procurar("VERMELHO");
    if (verde != NULL) {
        verde_min = Util::Para_Double(verde->Valor("MIN"), verde_min);
        amarelo_min = Util::Para_Double(verde->Valor("MAX"), amarelo_min);
    }
    if (amarelo != NULL) {
        amarelo_min = Util::Para_Double(amarelo->Valor("MIN"), amarelo_min);
        vermelho_min = Util::Para_Double(amarelo->Valor("MAX"), vermelho_min);
    }
    if (vermelho != NULL) {
        vermelho_min = Util::Para_Double(vermelho->Valor("MIN"), vermelho_min);
        vermelho_max = Util::Para_Double(vermelho->Valor("MAX"), vermelho_max);
    }
    if (amarelo_min < verde_min)
        amarelo_min = verde_min;
    if (vermelho_min < amarelo_min)
        vermelho_min = amarelo_min;
    if (vermelho_max < vermelho_min)
        vermelho_max = vermelho_min;
}

void Fabrica::Carregar_No(const XMLNo *no, int &n_ok, int &n_erros)
{
    const string &tag = no->Nome();
    if (tag == "MOTOR") {
        Motor *m = Motor::Criar(no->Valor("TIPO"),
                                Util::Para_Int(no->Valor("ID"), -1),
                                no->Valor("MARCA", "?"),
                                Util::Para_Double(no->Valor("CONSUMO_HORA"), 1.0),
                                Util::Para_Double(no->Valor("TEMPERATURA_AVISO"), amarelo_min),
                                Util::Para_Double(no->Valor("TEMPERATURA_PARAGEM"), vermelho_min),
                                Util::Para_Double(no->Valor("PROBABILIDADE_AVARIA"), 0.0),
                                Util::Para_Double(no->Valor("X"), 0.0),
                                Util::Para_Double(no->Valor("Y"), 0.0));
        if (m != NULL && Inserir(m)) {
            n_ok++;
        } else {
            delete m; // tipo desconhecido (NULL) ou ID repetido
            n_erros++;
        }
        return;
    }
    if (tag == "SENSOR") {
        Sensor *s = Sensor::Criar(no->Valor("TIPO"),
                                  Util::Para_Int(no->Valor("ID"), -1),
                                  no->Valor("MARCA", "?"),
                                  Util::Para_Double(no->Valor("VALOR"), 0.0),
                                  Util::Para_Double(no->Valor("LIMITE"), -1.0),
                                  Util::Para_Double(no->Valor("PROBABILIDADE_AVARIA"), 0.0),
                                  Util::Para_Double(no->Valor("X"), 0.0),
                                  Util::Para_Double(no->Valor("Y"), 0.0));
        if (s != NULL && Inserir(s)) {
            n_ok++;
        } else {
            delete s;
            n_erros++;
        }
        return;
    }
    if (tag == "USER" || tag == "UTILIZADOR") {
        User *u = User::Criar(no->Valor("TIPO"), Util::Para_Int(no->Valor("ID"), -1),
                              no->Valor("NOME", "?"));
        if (u != NULL && Inserir(u)) {
            n_ok++;
        } else {
            delete u;
            n_erros++;
        }
        return;
    }
    // outro elemento (ex.: <MOTORES>): procura nos filhos
    const list<XMLNo *> &filhos = no->Filhos();
    for (list<XMLNo *>::const_iterator it = filhos.begin(); it != filhos.end(); ++it)
        Carregar_No(*it, n_ok, n_erros);
}

bool Fabrica::Load(const string &ficheiro)
{
    string erro;
    XMLNo *raiz = XMLParser::Ler_Ficheiro(ficheiro, erro);
    if (raiz == NULL) {
        cout << "  Erro ao ler \"" << ficheiro << "\": " << erro << endl;
        return false;
    }
    Libertar_Dispositivos();
    Ler_Configuracao(raiz);
    int n_ok = 0;
    int n_erros = 0;
    Carregar_No(raiz, n_ok, n_erros);
    delete raiz; // a arvore XML ja nao e necessaria

    cout << "  \"" << ficheiro << "\" carregado: " << motores.size() << " motores, "
         << sensores.size() << " sensores, " << users.size() << " utilizadores";
    if (n_erros > 0)
        cout << " (" << n_erros << " elementos ignorados: tipo desconhecido ou ID repetido)";
    cout << "." << endl;
    return true;
}

// -------------------------------------------------------------- listagem

void Fabrica::Listar(ostream &f)
{
    f << "==================== " << nome << " ====================" << endl;
    f << "Horario: " << hora_abertura << "h-" << hora_fecho << "h" << endl;
    f << "Temperaturas: VERDE [" << verde_min << "," << amarelo_min << "[  AMARELO [" << amarelo_min
      << "," << vermelho_min << "[  VERMELHO [" << vermelho_min << "," << vermelho_max << "]" << endl;
    f << "Utilizador atual: ";
    if (atual != NULL)
        atual->Mostrar(f);
    else
        f << "(nenhum)";
    f << endl;

    f << "--- Utilizadores (" << users.size() << ") ---" << endl;
    for (list<User *>::const_iterator it = users.begin(); it != users.end(); ++it) {
        f << "  ";
        (*it)->Mostrar(f);
        f << endl;
    }
    f << "--- Motores (" << motores.size() << ") ---" << endl;
    for (list<Motor *>::const_iterator it = motores.begin(); it != motores.end(); ++it) {
        f << "  ";
        (*it)->Mostrar(f); // chamada polimorfica
        f << endl;
    }
    f << "--- Sensores (" << sensores.size() << ") ---" << endl;
    for (list<Sensor *>::const_iterator it = sensores.begin(); it != sensores.end(); ++it) {
        f << "  ";
        (*it)->Mostrar(f);
        f << endl;
    }
}
