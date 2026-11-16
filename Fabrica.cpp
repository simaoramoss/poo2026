/**
 * @file Fabrica.cpp
 * @brief Implementacao da classe Fabrica.
 */
#include "Fabrica.h"
#include "Util.h"
#include "XML.h"

#include <fstream>
#include <map>
#include <utility>

using namespace std;

/// Numero maximo de avisos guardados no historico.
static const size_t MAX_AVISOS = 500;

// ------------------------------------------------- funcoes de ordenacao

/** @brief Ordena motores por horas de trabalho (decrescente) e depois por ID. */
static bool Mais_Trabalhador(const Motor *a, const Motor *b)
{
    if (a->Get_Horas_Trabalho() != b->Get_Horas_Trabalho())
        return a->Get_Horas_Trabalho() > b->Get_Horas_Trabalho();
    return a->Get_ID() < b->Get_ID();
}

/** @brief Ordena pares (marca, avarias) por avarias (decrescente) e depois por marca. */
static bool Mais_Fraco(const pair<string, int> &a, const pair<string, int> &b)
{
    if (a.second != b.second)
        return a.second > b.second;
    return a.first < b.first;
}

// ------------------------------------------------ construcao/destruicao

Fabrica::Fabrica(User *ut)
    : nome("Fabrica"), atual(ut), hora_abertura(8), hora_fecho(18), dia(1), minutos(7 * 60 + 30),
      vizinhanca(15.0), largura(100.0), altura(60.0), verde_min(0.0), amarelo_min(70.0),
      vermelho_min(90.0), vermelho_max(150.0), video_fumo("saidas.mp4"), video_luz("regresso.mp4"),
      video_missel("abrigos.mp4"), desligada(false)
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

bool Fabrica::Remover(int id_motor)
{
    if (!Pode(P_REMOVER, "remover motores"))
        return false;
    for (list<Motor *>::iterator it = motores.begin(); it != motores.end(); ++it) {
        if ((*it)->Get_ID() == id_motor) {
            delete *it;
            motores.erase(it);
            return true;
        }
    }
    return false;
}

// ------------------------------------------------------- leitura do XML

void Fabrica::Ler_Configuracao(const XMLNo *raiz)
{
    nome = raiz->Valor("NOME", nome);
    hora_abertura = Util::Para_Int(raiz->Valor("HORA_ABERTURA"), hora_abertura);
    hora_fecho = Util::Para_Int(raiz->Valor("HORA_FECHO"), hora_fecho);
    vizinhanca = Util::Para_Double(raiz->Valor("VIZINHANCA"), vizinhanca);
    largura = Util::Para_Double(raiz->Valor("LARGURA"), largura);
    altura = Util::Para_Double(raiz->Valor("ALTURA"), altura);
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

    const XMLNo *videos = raiz->Procurar("VIDEOS");
    if (videos != NULL) {
        video_fumo = videos->Valor("FUMO", video_fumo);
        video_luz = videos->Valor("LUZ", video_luz);
        video_missel = videos->Valor("MISSEL", video_missel);
    }
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

    // a simulacao comeca meia hora antes da abertura
    dia = 1;
    minutos = hora_abertura * 60 - 30;
    if (minutos < 0)
        minutos = 0;
    desligada = false;
    cout << "  \"" << ficheiro << "\" carregado: " << motores.size() << " motores, "
         << sensores.size() << " sensores, " << users.size() << " utilizadores";
    if (n_erros > 0)
        cout << " (" << n_erros << " elementos ignorados: tipo desconhecido ou ID repetido)";
    cout << "." << endl;
    return true;
}

// ------------------------------------------------------------- listagens

string Fabrica::Hora_Texto() const
{
    int h = minutos / 60;
    int m = minutos % 60;
    string s = "Dia " + Util::Para_Texto(dia) + " ";
    s += (h < 10 ? "0" : "") + Util::Para_Texto(h) + ":" + (m < 10 ? "0" : "") + Util::Para_Texto(m);
    return s;
}

double Fabrica::Consumo_Dia() const
{
    double total = 0.0;
    for (list<Motor *>::const_iterator it = motores.begin(); it != motores.end(); ++it)
        total += (*it)->Get_Consumo_Dia();
    return total;
}

void Fabrica::Listar(ostream &f)
{
    f << "==================== " << nome << " ====================" << endl;
    f << Hora_Texto() << " | horario " << hora_abertura << "h-" << hora_fecho << "h | "
      << (desligada ? "DESLIGADA" : (Aberta() ? "ABERTA" : "FECHADA")) << endl;
    f << "Temperaturas: VERDE [" << verde_min << "," << amarelo_min << "[  AMARELO [" << amarelo_min
      << "," << vermelho_min << "[  VERMELHO [" << vermelho_min << "," << vermelho_max << "]" << endl;
    f << "Vizinhanca (humidade): " << vizinhanca << endl;
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
        (*it)->Mostrar(f);
        f << endl;
    }
    f << "--- Sensores (" << sensores.size() << ") ---" << endl;
    for (list<Sensor *>::const_iterator it = sensores.begin(); it != sensores.end(); ++it) {
        f << "  ";
        (*it)->Mostrar(f);
        f << endl;
    }
    f << "Consumo total do dia: " << Util::Para_Texto(Consumo_Dia()) << " kWh" << endl;
}

void Fabrica::Resumo(ostream &f) const
{
    int n_run = 0, n_stop = 0, n_avariados = 0, n_quentes = 0;
    for (list<Motor *>::const_iterator it = motores.begin(); it != motores.end(); ++it) {
        if ((*it)->Esta_Avariado())
            n_avariados++;
        else if ((*it)->Esta_Ligado())
            n_run++;
        else
            n_stop++;
        if ((*it)->Estou_Quente() != TEMP_VERDE)
            n_quentes++;
    }
    f << "[" << Hora_Texto() << "] " << (Aberta() ? "ABERTA " : "FECHADA") << " | motores: " << n_run
      << " ligados, " << n_stop << " parados, " << n_avariados << " avariados, " << n_quentes
      << " quentes | consumo do dia: " << Util::Para_Texto(Consumo_Dia(), 1) << " kWh" << endl;
}

void Fabrica::Listar_Avisos(ostream &f, int ultimos) const
{
    f << "--- Avisos ao gestor (" << avisos.size() << " no total) ---" << endl;
    int saltar = static_cast<int>(avisos.size()) - ultimos;
    int i = 0;
    for (list<string>::const_iterator it = avisos.begin(); it != avisos.end(); ++it, ++i)
        if (i >= saltar)
            f << "  " << *it << endl;
}

list<Motor *> *Fabrica::Listar_Tipo(string Tipo, ostream &f)
{
    list<Motor *> *res = new list<Motor *>();
    string alvo = Util::Maiusculas(Util::Aparar(Tipo));
    for (list<Motor *>::const_iterator it = motores.begin(); it != motores.end(); ++it) {
        if ((*it)->Tipo() == alvo) {
            res->push_back(*it);
            f << "  ";
            (*it)->Mostrar(f);
            f << endl;
        }
    }
    f << "  Total de motores do tipo " << alvo << ": " << res->size() << endl;
    return res;
}

// ------------------------------------------------------ comando de motores

bool Fabrica::Ligar(int id_motor)
{
    if (!Pode(P_RUN, "ligar motores"))
        return false;
    Motor *m = Procurar_Motor(id_motor);
    if (m == NULL || desligada)
        return false;
    return m->Start();
}

void Fabrica::Desligar(int id_motor)
{
    if (!Pode(P_RUN, "desligar motores"))
        return;
    Motor *m = Procurar_Motor(id_motor);
    if (m == NULL) {
        cout << "  Nao existe nenhum motor com o ID " << id_motor << "." << endl;
        return;
    }
    if (!m->Stop())
        cout << "  O motor " << id_motor << " esta avariado; precisa de manutencao." << endl;
}

ESTADO_MOTOR Fabrica::Get_Estado(int id_motor)
{
    Motor *m = Procurar_Motor(id_motor);
    return (m != NULL) ? m->Get_Estado() : ESTADO_INVALIDO;
}

bool Fabrica::Manutencao()
{
    if (!Pode(P_MANUTENCAO, "fazer manutencao"))
        return false;
    for (list<Motor *>::iterator it = motores.begin(); it != motores.end(); ++it)
        (*it)->Manutencao(verde_min, amarelo_min);
    for (list<Sensor *>::iterator it = sensores.begin(); it != sensores.end(); ++it)
        (*it)->Manutencao();
    return true;
}

bool Fabrica::Set_Valor_Sensor(int id_sensor, double valor)
{
    if (!Pode(P_RUN, "alterar sensores"))
        return false;
    Sensor *s = Procurar_Sensor(id_sensor);
    if (s == NULL)
        return false;
    s->Forcar_Valor(valor);
    return true;
}

bool Fabrica::Set_User_Atual(int id_user)
{
    User *u = Procurar_User(id_user);
    if (u == NULL)
        return false;
    atual = u;
    return true;
}

// --------------------------------------------------------------- rankings

list<string> *Fabrica::Ranking_Dos_Fracos()
{
    // soma as avarias por marca (motores e sensores)
    map<string, int> por_marca;
    for (list<Motor *>::const_iterator it = motores.begin(); it != motores.end(); ++it)
        por_marca[(*it)->Get_Marca()] += (*it)->Get_N_Avarias();
    for (list<Sensor *>::const_iterator it = sensores.begin(); it != sensores.end(); ++it)
        por_marca[(*it)->Get_Marca()] += (*it)->Get_N_Avarias();

    list<pair<string, int> > ordenada;
    for (map<string, int>::const_iterator it = por_marca.begin(); it != por_marca.end(); ++it)
        ordenada.push_back(*it);
    ordenada.sort(Mais_Fraco);

    list<string> *res = new list<string>();
    for (list<pair<string, int> >::const_iterator it = ordenada.begin(); it != ordenada.end(); ++it)
        res->push_back(it->first);
    return res;
}

list<Motor *> *Fabrica::Ranking_Dos_Mais_Trabalhadores()
{
    list<Motor *> *res = new list<Motor *>(motores);
    res->sort(Mais_Trabalhador);
    return res;
}

// ----------------------------------------------------------------- avisos

void Fabrica::Avisar_Gestor(const string &msg)
{
    string linha = "[" + Hora_Texto() + "] " + msg;
    avisos.push_back(linha);
    if (avisos.size() > MAX_AVISOS)
        avisos.pop_front();
    cout << "  ** AVISO AO GESTOR ** " << linha << endl;
}

int Fabrica::Parar_Todos(list<Motor *> &lm)
{
    int n = 0;
    for (list<Motor *>::iterator it = motores.begin(); it != motores.end(); ++it) {
        if ((*it)->Esta_Ligado() && (*it)->Stop()) {
            lm.push_back(*it);
            n++;
        }
    }
    return n;
}

int Fabrica::Aviso_Humidade(list<Motor *> &lm)
{
    int n = 0;
    for (list<Sensor *>::const_iterator is = sensores.begin(); is != sensores.end(); ++is) {
        Sensor *s = *is;
        if (s->Tipo() != "HUMIDADE" || !s->Em_Alerta())
            continue;
        for (list<Motor *>::iterator im = motores.begin(); im != motores.end(); ++im) {
            Motor *m = *im;
            // um motor ja parado por outro sensor deixa de estar ligado: nao repete
            if (m->Esta_Ligado() && m->Distancia(*s) <= vizinhanca && m->Stop()) {
                lm.push_back(m);
                n++;
            }
        }
    }
    return n;
}

int Fabrica::Aviso_Fumo(list<Motor *> &lm, string fich_video)
{
    int n = Parar_Todos(lm);
    cout << "  FUMO! Dirija-se as saidas de emergencia." << endl;
    Util::Mostrar_Video(fich_video);
    return n;
}

int Fabrica::Aviso_Luz(string fich_video)
{
    int n = 0;
    for (list<Sensor *>::const_iterator it = sensores.begin(); it != sensores.end(); ++it)
        if ((*it)->Tipo() == "LUZ" && (*it)->Em_Alerta())
            n++;
    if (n > 0) {
        cout << "  Esta a escurecer... bom regresso a casa!" << endl;
        Util::Mostrar_Video(fich_video);
    }
    return n;
}

void Fabrica::Aviso_Missel(string fvideo, string festado)
{
    cout << "  MISSIL DETETADO! Dirija-se imediatamente aos abrigos." << endl;
    Util::Mostrar_Video(fvideo);

    // guarda o estado ANTES de desligar, para se saber como estava a fabrica
    ofstream f(festado.c_str());
    if (f.is_open()) {
        Listar(f);
        Listar_Avisos(f, static_cast<int>(MAX_AVISOS));
        f.close();
        cout << "  Estado atual guardado em \"" << festado << "\"." << endl;
    } else {
        cout << "  Nao foi possivel gravar \"" << festado << "\"." << endl;
    }

    list<Motor *> parados;
    Parar_Todos(parados);
    desligada = true;
}

// -------------------------------------------------------------- simulacao

bool Fabrica::Aberta() const
{
    int h = minutos / 60;
    return !desligada && h >= hora_abertura && h < hora_fecho;
}

void Fabrica::Abrir()
{
    int n = 0;
    for (list<Motor *>::iterator it = motores.begin(); it != motores.end(); ++it)
        if ((*it)->Start())
            n++;
    cout << "  A fabrica abriu: " << n << " motores em arranque." << endl;
}

void Fabrica::Fechar()
{
    list<Motor *> parados;
    int n = Parar_Todos(parados);
    cout << "  A fabrica fechou: " << n << " motores desligados. Consumo do dia: "
         << Util::Para_Texto(Consumo_Dia()) << " kWh." << endl;
}

void Fabrica::Simular_Motores(double horas)
{
    for (list<Motor *>::iterator it = motores.begin(); it != motores.end(); ++it) {
        Motor *m = *it;
        string quem = "Motor " + Util::Para_Texto(m->Get_ID()) + " (" + m->Get_Marca() + ")";
        if (m->Simular(horas))
            Avisar_Gestor(quem + " AVARIOU.");
        if (!m->Mudou_Classe())
            continue;
        string temp = Util::Para_Texto(m->Get_Temperatura(), 1) + "C";
        CLASSE_TEMP classe = m->Estou_Quente();
        if (classe == TEMP_AMARELO && m->Esta_Ligado()) { // a arrefecer nao avisa
            Avisar_Gestor(quem + " esta a aquecer: " + temp + " (AMARELO).");
        } else if (classe == TEMP_VERMELHO) {
            m->Stop(); // protege-se parando
            Avisar_Gestor(quem + " muito quente: " + temp + " (VERMELHO). Motor parado.");
        }
    }
}

void Fabrica::Simular_Sensores(double horas)
{
    bool humidade = false, fumo = false, luz = false, missel = false;
    double hora = minutos / 60.0;
    for (list<Sensor *>::iterator it = sensores.begin(); it != sensores.end(); ++it) {
        Sensor *s = *it;
        string quem = "Sensor " + Util::Para_Texto(s->Get_ID()) + " de " + s->Tipo() + " (" +
                      s->Get_Marca() + ")";
        if (s->Simular(hora, horas))
            Avisar_Gestor(quem + " AVARIOU.");
        if (!s->Novo_Alerta())
            continue;
        Avisar_Gestor(quem + " em alerta: valor " + Util::Para_Texto(s->Get_Valor(), 1) + ".");
        string tipo = s->Tipo();
        if (tipo == "HUMIDADE")
            humidade = true;
        else if (tipo == "FUMO")
            fumo = true;
        else if (tipo == "LUZ")
            luz = true;
        else if (tipo == "MISSEIS")
            missel = true;
    }

    // as reacoes sao feitas depois de lidos todos os sensores
    list<Motor *> parados;
    if (humidade) {
        int n = Aviso_Humidade(parados);
        Avisar_Gestor("Humidade: " + Util::Para_Texto(n) + " motores desligados na vizinhanca.");
    }
    if (fumo) {
        parados.clear();
        int n = Aviso_Fumo(parados, video_fumo);
        Avisar_Gestor("Fumo: " + Util::Para_Texto(n) + " motores desligados.");
    }
    if (luz)
        Aviso_Luz(video_luz);
    if (missel) {
        Avisar_Gestor("Missil: fabrica vai ser desligada.");
        Aviso_Missel(video_missel);
    }
}

void Fabrica::Simular_Passo(int min)
{
    if (desligada || min <= 0)
        return;
    bool aberta_antes = Aberta();
    minutos += min;
    if (minutos >= 24 * 60) { // novo dia: o consumo diario recomeca
        minutos -= 24 * 60;
        dia++;
        for (list<Motor *>::iterator it = motores.begin(); it != motores.end(); ++it)
            (*it)->Novo_Dia();
    }
    bool aberta_agora = Aberta();
    if (!aberta_antes && aberta_agora)
        Abrir();
    else if (aberta_antes && !aberta_agora)
        Fechar();

    double horas = min / 60.0;
    Simular_Motores(horas);
    Simular_Sensores(horas);
}

// ---------------------------------------------------------- XML e HTML

void Fabrica::Relatorio(string fich_xml)
{
    ofstream f(fich_xml.c_str());
    if (!f.is_open()) {
        cout << "  Nao foi possivel gravar \"" << fich_xml << "\"." << endl;
        return;
    }
    int h = minutos / 60;
    int m = minutos % 60;
    f << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    f << "<RELATORIO FABRICA=\"" << Util::Escapar(nome) << "\" DIA=\"" << dia << "\" HORA=\""
      << (h < 10 ? "0" : "") << h << ":" << (m < 10 ? "0" : "") << m << "\" ESTADO=\""
      << (desligada ? "DESLIGADA" : (Aberta() ? "ABERTA" : "FECHADA")) << "\">" << endl;
    f << "  <CONSUMO_TOTAL_DIA UNIDADE=\"kWh\">" << Util::Para_Texto(Consumo_Dia())
      << "</CONSUMO_TOTAL_DIA>" << endl;

    f << "  <MOTORES TOTAL=\"" << motores.size() << "\">" << endl;
    for (list<Motor *>::const_iterator it = motores.begin(); it != motores.end(); ++it) {
        const Motor *mt = *it;
        f << "    <MOTOR ID=\"" << mt->Get_ID() << "\" TIPO=\"" << mt->Tipo() << "\" MARCA=\""
          << Util::Escapar(mt->Get_Marca()) << "\" X=\"" << mt->Get_X() << "\" Y=\"" << mt->Get_Y()
          << "\" ESTADO=\"" << Texto_Estado(mt->Get_Estado()) << "\" TEMPERATURA=\""
          << Util::Para_Texto(mt->Get_Temperatura(), 1) << "\" CLASSE=\""
          << Texto_Classe(mt->Estou_Quente()) << "\" CONSUMO_DIA=\""
          << Util::Para_Texto(mt->Get_Consumo_Dia()) << "\" CONSUMO_TOTAL=\""
          << Util::Para_Texto(mt->Get_Consumo_Total()) << "\" HORAS_TRABALHO=\""
          << Util::Para_Texto(mt->Get_Horas_Trabalho()) << "\" AVARIAS=\"" << mt->Get_N_Avarias()
          << "\"/>" << endl;
    }
    f << "  </MOTORES>" << endl;

    f << "  <SENSORES TOTAL=\"" << sensores.size() << "\">" << endl;
    for (list<Sensor *>::const_iterator it = sensores.begin(); it != sensores.end(); ++it) {
        const Sensor *s = *it;
        f << "    <SENSOR ID=\"" << s->Get_ID() << "\" TIPO=\"" << s->Tipo() << "\" MARCA=\""
          << Util::Escapar(s->Get_Marca()) << "\" X=\"" << s->Get_X() << "\" Y=\"" << s->Get_Y()
          << "\" VALOR=\"" << Util::Para_Texto(s->Get_Valor(), 1) << "\" LIMITE=\""
          << Util::Para_Texto(s->Get_Limite(), 1) << "\" ESTADO=\""
          << (s->Esta_Avariado() ? "AVARIADO" : (s->Em_Alerta() ? "ALERTA" : "OK"))
          << "\" AVARIAS=\"" << s->Get_N_Avarias() << "\"/>" << endl;
    }
    f << "  </SENSORES>" << endl;

    f << "  <AVISOS TOTAL=\"" << avisos.size() << "\">" << endl;
    for (list<string>::const_iterator it = avisos.begin(); it != avisos.end(); ++it)
        f << "    <AVISO>" << Util::Escapar(*it) << "</AVISO>" << endl;
    f << "  </AVISOS>" << endl;
    f << "</RELATORIO>" << endl;
    f.close();
}

void Fabrica::Listagem_HTML(string fich_html)
{
    ofstream f(fich_html.c_str());
    if (!f.is_open()) {
        cout << "  Nao foi possivel gravar \"" << fich_html << "\"." << endl;
        return;
    }
    f << "<!DOCTYPE html>\n<html lang=\"pt\">\n<head>\n<meta charset=\"UTF-8\">\n<title>"
      << Util::Escapar(nome) << "</title>\n<style>\n"
      << "body{font-family:Arial,sans-serif;margin:24px;background:#f4f6f8;color:#222}\n"
      << "table{border-collapse:collapse;background:#fff;margin-bottom:24px}\n"
      << "th,td{border:1px solid #ccc;padding:6px 10px;text-align:left}\n"
      << "th{background:#005b7f;color:#fff}\n"
      << ".VERDE,.OK,.RUN{background:#c8f0c8}.AMARELO,.START{background:#fff3b0}\n"
      << ".VERMELHO,.AVARIADO,.ALERTA{background:#f5b5b5}.STOP{background:#e0e0e0}\n"
      << "svg{background:#fff;border:1px solid #999;margin-bottom:24px}\n"
      << "</style>\n</head>\n<body>\n";
    f << "<h1>" << Util::Escapar(nome) << "</h1>\n";
    f << "<p><b>" << Hora_Texto() << "</b> &mdash; horario " << hora_abertura << "h-" << hora_fecho
      << "h &mdash; " << (desligada ? "DESLIGADA" : (Aberta() ? "ABERTA" : "FECHADA"))
      << " &mdash; consumo do dia: <b>" << Util::Para_Texto(Consumo_Dia()) << " kWh</b></p>\n";

    // planta da fabrica: motores = circulos, sensores = quadrados
    double escala = 8.0;
    f << "<h2>Planta</h2>\n<svg width=\"" << largura * escala + 40 << "\" height=\""
      << altura * escala + 40 << "\">\n";
    for (list<Motor *>::const_iterator it = motores.begin(); it != motores.end(); ++it) {
        const Motor *m = *it;
        const char *cor = "#9e9e9e"; // STOP
        if (m->Esta_Avariado())
            cor = "#000000";
        else if (m->Esta_Ligado())
            cor = (m->Estou_Quente() == TEMP_VERDE) ? "#2e9e3a"
                  : (m->Estou_Quente() == TEMP_AMARELO ? "#e0b000" : "#d02020");
        double cx = 20 + m->Get_X() * escala;
        double cy = 20 + m->Get_Y() * escala;
        f << "<circle cx=\"" << cx << "\" cy=\"" << cy << "\" r=\"10\" fill=\"" << cor
          << "\"><title>Motor " << m->Get_ID() << " " << Texto_Estado(m->Get_Estado())
          << "</title></circle><text x=\"" << cx + 12 << "\" y=\"" << cy + 4
          << "\" font-size=\"11\">M" << m->Get_ID() << "</text>\n";
    }
    for (list<Sensor *>::const_iterator it = sensores.begin(); it != sensores.end(); ++it) {
        const Sensor *s = *it;
        const char *cor = s->Esta_Avariado() ? "#000000" : (s->Em_Alerta() ? "#d02020" : "#1976d2");
        double sx = 20 + s->Get_X() * escala;
        double sy = 20 + s->Get_Y() * escala;
        f << "<rect x=\"" << sx - 7 << "\" y=\"" << sy - 7 << "\" width=\"14\" height=\"14\" fill=\""
          << cor << "\"><title>Sensor " << s->Get_ID() << " " << s->Tipo()
          << "</title></rect><text x=\"" << sx + 10 << "\" y=\"" << sy + 4
          << "\" font-size=\"11\">S" << s->Get_ID() << "</text>\n";
    }
    f << "</svg>\n";

    f << "<h2>Motores</h2>\n<table>\n<tr><th>ID</th><th>Tipo</th><th>Marca</th><th>Posicao</th>"
      << "<th>Estado</th><th>Temperatura</th><th>Classe</th><th>Consumo dia (kWh)</th>"
      << "<th>Horas</th><th>Avarias</th></tr>\n";
    for (list<Motor *>::const_iterator it = motores.begin(); it != motores.end(); ++it) {
        const Motor *m = *it;
        f << "<tr><td>" << m->Get_ID() << "</td><td>" << m->Tipo() << "</td><td>"
          << Util::Escapar(m->Get_Marca()) << "</td><td>(" << m->Get_X() << ", " << m->Get_Y()
          << ")</td><td class=\"" << Texto_Estado(m->Get_Estado()) << "\">"
          << Texto_Estado(m->Get_Estado()) << "</td><td>"
          << Util::Para_Texto(m->Get_Temperatura(), 1) << " C</td><td class=\""
          << Texto_Classe(m->Estou_Quente()) << "\">" << Texto_Classe(m->Estou_Quente())
          << "</td><td>" << Util::Para_Texto(m->Get_Consumo_Dia()) << "</td><td>"
          << Util::Para_Texto(m->Get_Horas_Trabalho()) << "</td><td>" << m->Get_N_Avarias()
          << "</td></tr>\n";
    }
    f << "</table>\n";

    f << "<h2>Sensores</h2>\n<table>\n<tr><th>ID</th><th>Tipo</th><th>Marca</th><th>Posicao</th>"
      << "<th>Valor</th><th>Limite</th><th>Estado</th><th>Avarias</th></tr>\n";
    for (list<Sensor *>::const_iterator it = sensores.begin(); it != sensores.end(); ++it) {
        const Sensor *s = *it;
        const char *est = s->Esta_Avariado() ? "AVARIADO" : (s->Em_Alerta() ? "ALERTA" : "OK");
        f << "<tr><td>" << s->Get_ID() << "</td><td>" << s->Tipo() << "</td><td>"
          << Util::Escapar(s->Get_Marca()) << "</td><td>(" << s->Get_X() << ", " << s->Get_Y()
          << ")</td><td>" << Util::Para_Texto(s->Get_Valor(), 1) << "</td><td>"
          << Util::Para_Texto(s->Get_Limite(), 1) << "</td><td class=\"" << est << "\">" << est
          << "</td><td>" << s->Get_N_Avarias() << "</td></tr>\n";
    }
    f << "</table>\n";

    f << "<h2>Utilizadores</h2>\n<table>\n<tr><th>ID</th><th>Nome</th><th>Tipo</th></tr>\n";
    for (list<User *>::const_iterator it = users.begin(); it != users.end(); ++it)
        f << "<tr><td>" << (*it)->Get_ID() << "</td><td>" << Util::Escapar((*it)->Get_Nome())
          << "</td><td>" << (*it)->Tipo() << "</td></tr>\n";
    f << "</table>\n";

    f << "<h2>Avisos ao gestor</h2>\n<ul>\n";
    for (list<string>::const_iterator it = avisos.begin(); it != avisos.end(); ++it)
        f << "<li>" << Util::Escapar(*it) << "</li>\n";
    f << "</ul>\n</body>\n</html>\n";
    f.close();
}
