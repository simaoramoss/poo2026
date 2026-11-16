/**
 * @file main.cpp
 * @brief Simulador da fabrica: ciclo de simulacao e menu (tecla 'M').
 *
 * Utilizacao: fabrica [ficheiro.xml] [n_passos] [ms_por_passo]
 *  - ficheiro.xml : configuracao da fabrica (por omissao "fabrica.xml");
 *  - n_passos     : numero de passos a simular (0 = ate o utilizador sair);
 *  - ms_por_passo : pausa real entre passos, em milissegundos (por omissao 500).
 */
#include <iostream>
#include <list>
#include <string>

#include "Fabrica.h"
#include "Util.h"

using namespace std;

/// Minutos simulados em cada passo do simulador.
static const int MINUTOS_POR_PASSO = 10;

/** @brief Le uma linha de texto do teclado, depois de mostrar @p pergunta. */
static string Ler_Linha(const string &pergunta)
{
    cout << pergunta;
    cout.flush();
    string linha;
    if (!getline(cin, linha))
        return "";
    return Util::Aparar(linha);
}

/** @brief Le um inteiro (devolve @p def se a resposta nao for um numero). */
static int Ler_Int(const string &pergunta, int def = -1)
{
    return Util::Para_Int(Ler_Linha(pergunta), def);
}

/** @brief Le um real (devolve @p def se a resposta nao for um numero). */
static double Ler_Double(const string &pergunta, double def = 0.0)
{
    return Util::Para_Double(Ler_Linha(pergunta), def);
}

/** @brief Opcao do menu: pede os dados e adiciona um utilizador. */
static void Menu_Add_User(Fabrica &fab)
{
    string tipo = Ler_Linha("  Tipo (ADMIN/NORMAL/VISITANTE): ");
    int id = Ler_Int("  ID: ");
    string nome = Ler_Linha("  Nome: ");
    User *u = User::Criar(tipo, id, nome);
    if (u == NULL) {
        cout << "  Tipo de utilizador desconhecido." << endl;
        return;
    }
    if (fab.Add(u))
        cout << "  Utilizador adicionado." << endl;
    else
        delete u; // nao foi aceite: continua a ser nosso
}

/** @brief Opcao do menu: pede os dados e adiciona um motor. */
static void Menu_Add_Motor(Fabrica &fab)
{
    string tipo = Ler_Linha("  Tipo (ELETRICO/COMBUSTAO/INDUCAO): ");
    int id = Ler_Int("  ID: ");
    string marca = Ler_Linha("  Marca: ");
    double consumo = Ler_Double("  Consumo por hora (kWh): ", 1.0);
    double aviso = Ler_Double("  Temperatura de aviso (ENTER = omissao): ", fab.Get_Temp_Aviso_Omissao());
    double paragem = Ler_Double("  Temperatura de paragem (ENTER = omissao): ", fab.Get_Temp_Paragem_Omissao());
    double prob = Ler_Double("  Probabilidade de avaria por hora (0..1): ", 0.0);
    double x = Ler_Double("  Posicao X: ", 0.0);
    double y = Ler_Double("  Posicao Y: ", 0.0);
    Motor *m = Motor::Criar(tipo, id, marca, consumo, aviso, paragem, prob, x, y);
    if (m == NULL) {
        cout << "  Tipo de motor desconhecido." << endl;
        return;
    }
    if (fab.Add(m))
        cout << "  Motor adicionado." << endl;
    else
        delete m;
}

/** @brief Opcao do menu: pede os dados e adiciona um sensor. */
static void Menu_Add_Sensor(Fabrica &fab)
{
    string tipo = Ler_Linha("  Tipo (LUZ/HUMIDADE/FUMO/MISSEIS): ");
    int id = Ler_Int("  ID: ");
    string marca = Ler_Linha("  Marca: ");
    double limite = Ler_Double("  Limite de aviso (ENTER = omissao): ", -1.0);
    double prob = Ler_Double("  Probabilidade de avaria por hora (0..1): ", 0.0);
    double x = Ler_Double("  Posicao X: ", 0.0);
    double y = Ler_Double("  Posicao Y: ", 0.0);
    Sensor *s = Sensor::Criar(tipo, id, marca, 0.0, limite, prob, x, y);
    if (s == NULL) {
        cout << "  Tipo de sensor desconhecido." << endl;
        return;
    }
    s->Manutencao(); // comeca com uma leitura normal para o seu tipo
    if (fab.Add(s))
        cout << "  Sensor adicionado." << endl;
    else
        delete s;
}

/**
 * @brief Mostra o menu, le UMA opcao e executa-a (o simulador fica parado).
 * @return false se o utilizador pediu para terminar o programa.
 */
static bool Menu(Fabrica &fab)
{
    cout << endl
         << "================ MENU (simulador em pausa) ================" << endl
         << "  1. Listar estado da fabrica      10. Manutencao geral" << endl
         << "  2. Adicionar utilizador          11. Ranking das marcas mais fracas" << endl
         << "  3. Adicionar motor               12. Ranking dos motores mais trabalhadores" << endl
         << "  4. Adicionar sensor              13. Relatorio XML" << endl
         << "  5. Remover motor                 14. Listagem HTML" << endl
         << "  6. Ligar motor                   15. Forcar valor de um sensor (testar avisos)" << endl
         << "  7. Desligar motor                16. Mudar de utilizador" << endl
         << "  8. Estado de um motor            17. Ver avisos ao gestor" << endl
         << "  9. Listar motores de um tipo     18. Terminar o programa" << endl
         << "  0. Continuar a simulacao" << endl;
    cout << "Utilizador: " << fab.Get_User_Atual()->Get_Nome() << " ["
         << fab.Get_User_Atual()->Tipo() << "]" << endl;

    int op = Ler_Int("Opcao: ", 0);
    switch (op) {
    case 1:
        fab.Listar();
        break;
    case 2:
        Menu_Add_User(fab);
        break;
    case 3:
        Menu_Add_Motor(fab);
        break;
    case 4:
        Menu_Add_Sensor(fab);
        break;
    case 5:
        if (fab.Remover(Ler_Int("  ID do motor a remover: ")))
            cout << "  Motor removido." << endl;
        else
            cout << "  Motor nao removido." << endl;
        break;
    case 6:
        if (fab.Ligar(Ler_Int("  ID do motor a ligar: ")))
            cout << "  Motor em arranque." << endl;
        else
            cout << "  Nao foi possivel ligar o motor." << endl;
        break;
    case 7: {
        int id = Ler_Int("  ID do motor a desligar: ");
        fab.Desligar(id);
        cout << "  Estado do motor " << id << ": " << Texto_Estado(fab.Get_Estado(id)) << endl;
        break;
    }
    case 8: {
        int id = Ler_Int("  ID do motor: ");
        cout << "  Estado do motor " << id << ": " << Texto_Estado(fab.Get_Estado(id)) << endl;
        break;
    }
    case 9: {
        list<Motor *> *lm = fab.Listar_Tipo(Ler_Linha("  Tipo (ELETRICO/COMBUSTAO/INDUCAO): "));
        delete lm; // so a lista; os motores sao da fabrica
        break;
    }
    case 10:
        if (fab.Manutencao())
            cout << "  Manutencao concluida em todos os motores e sensores." << endl;
        break;
    case 11: {
        list<string> *marcas = fab.Ranking_Dos_Fracos();
        int pos = 1;
        cout << "  Marcas, da que mais avaria para a mais fiavel:" << endl;
        for (list<string>::const_iterator it = marcas->begin(); it != marcas->end(); ++it)
            cout << "  " << pos++ << ". " << *it << endl;
        delete marcas;
        break;
    }
    case 12: {
        list<Motor *> *lm = fab.Ranking_Dos_Mais_Trabalhadores();
        int pos = 1;
        for (list<Motor *>::const_iterator it = lm->begin(); it != lm->end(); ++it)
            cout << "  " << pos++ << ". Motor " << (*it)->Get_ID() << " [" << (*it)->Tipo() << "] "
                 << (*it)->Get_Marca() << " - " << Util::Para_Texto((*it)->Get_Horas_Trabalho())
                 << " horas" << endl;
        delete lm;
        break;
    }
    case 13: {
        string fich = Ler_Linha("  Ficheiro (ENTER = Relatorio.xml): ");
        if (fich.empty())
            fich = "Relatorio.xml";
        fab.Relatorio(fich);
        cout << "  Relatorio gravado em \"" << fich << "\"." << endl;
        break;
    }
    case 14: {
        string fich = Ler_Linha("  Ficheiro (ENTER = Fabrica.html): ");
        if (fich.empty())
            fich = "Fabrica.html";
        fab.Listagem_HTML(fich);
        cout << "  Listagem gravada em \"" << fich << "\"." << endl;
        break;
    }
    case 15: {
        int id = Ler_Int("  ID do sensor: ");
        double v = Ler_Double("  Novo valor: ");
        if (fab.Set_Valor_Sensor(id, v))
            cout << "  Valor alterado; o aviso (se existir) surge no proximo passo." << endl;
        else
            cout << "  Valor nao alterado." << endl;
        break;
    }
    case 16:
        if (fab.Set_User_Atual(Ler_Int("  ID do utilizador: ")))
            cout << "  Utilizador atual: " << fab.Get_User_Atual()->Get_Nome() << endl;
        else
            cout << "  Utilizador inexistente." << endl;
        break;
    case 17:
        fab.Listar_Avisos();
        break;
    case 18:
        return false;
    default:
        break;
    }
    cout << "===========================================================" << endl << endl;
    return true;
}

/** @brief Ponto de entrada: carrega a fabrica e corre o simulador. */
int main(int argc, char *argv[])
{
    string ficheiro = (argc > 1) ? argv[1] : "fabrica.xml";
    int max_passos = (argc > 2) ? Util::Para_Int(argv[2], 0) : 0;
    int ms_passo = (argc > 3) ? Util::Para_Int(argv[3], 500) : 500;

    Util::Iniciar_Aleatorio();

    // o utilizador que arranca o simulador e o administrador (gestor)
    Fabrica fab(new Admin(0, "Gestor"));
    if (!fab.Load(ficheiro))
        return 1;

    cout << "Simulador a correr. Tecla 'M' = menu, 'Q' = sair." << endl;
    Util::Modo_Tecla(true);
    bool sair = false;
    bool ha_teclado = true;
    int passo = 0;
    while (!sair && !fab.Esta_Desligada() && (max_passos == 0 || passo < max_passos)) {
        fab.Simular_Passo(MINUTOS_POR_PASSO);
        fab.Resumo();
        passo++;

        // espera pelo proximo passo, atenta ao teclado
        int espera = 0;
        do {
            int tecla = ha_teclado ? Util::Ler_Tecla() : 0;
            if (tecla == 'm' || tecla == 'M') {
                Util::Modo_Tecla(false);
                if (!Menu(fab))
                    sair = true;
                Util::Modo_Tecla(true);
            } else if (tecla == 'q' || tecla == 'Q') {
                sair = true;
            } else if (tecla < 0) { // entrada fechada (EOF)
                ha_teclado = false;
                if (max_passos == 0)
                    sair = true;
            }
            if (!sair && espera < ms_passo)
                Util::Esperar_ms(ms_passo - espera < 50 ? ms_passo - espera : 50);
            espera += 50;
        } while (!sair && espera < ms_passo);
    }
    Util::Modo_Tecla(false);

    if (fab.Esta_Desligada())
        cout << "A fabrica foi desligada. Simulacao terminada." << endl;
    fab.Relatorio("Relatorio.xml");
    cout << "Relatorio final gravado em \"Relatorio.xml\"." << endl;
    return 0; // o destrutor de Fabrica liberta toda a memoria
}
