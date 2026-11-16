/**
 * @file Util.cpp
 * @brief Implementacao das funcoes auxiliares.
 */
#include "Util.h"

#include <cctype>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>

#ifdef _WIN32
#include <conio.h>
#else
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

namespace Util {

void Iniciar_Aleatorio()
{
    srand(static_cast<unsigned int>(time(NULL)));
}

double Aleatorio(double min, double max)
{
    double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    return min + r * (max - min);
}

bool Acontece(double prob)
{
    if (prob <= 0.0)
        return false;
    return Aleatorio(0.0, 1.0) < prob;
}

string Maiusculas(const string &s)
{
    string r = s;
    for (size_t i = 0; i < r.size(); i++)
        r[i] = static_cast<char>(toupper(static_cast<unsigned char>(r[i])));
    return r;
}

string Aparar(const string &s)
{
    size_t ini = 0;
    size_t fim = s.size();
    while (ini < fim && isspace(static_cast<unsigned char>(s[ini])))
        ini++;
    while (fim > ini && isspace(static_cast<unsigned char>(s[fim - 1])))
        fim--;
    return s.substr(ini, fim - ini);
}

int Para_Int(const string &s, int def)
{
    string t = Aparar(s);
    if (t.empty())
        return def;
    char *fim = NULL;
    long v = strtol(t.c_str(), &fim, 10);
    if (fim == t.c_str())
        return def;
    return static_cast<int>(v);
}

double Para_Double(const string &s, double def)
{
    string t = Aparar(s);
    // aceita a virgula como separador decimal
    for (size_t i = 0; i < t.size(); i++)
        if (t[i] == ',')
            t[i] = '.';
    if (t.empty())
        return def;
    char *fim = NULL;
    double v = strtod(t.c_str(), &fim);
    if (fim == t.c_str())
        return def;
    return v;
}

string Para_Texto(double v, int casas)
{
    ostringstream os;
    os << fixed << setprecision(casas) << v;
    return os.str();
}

string Para_Texto(int v)
{
    ostringstream os;
    os << v;
    return os.str();
}

string Escapar(const string &s)
{
    string r;
    for (size_t i = 0; i < s.size(); i++) {
        switch (s[i]) {
        case '<': r += "&lt;"; break;
        case '>': r += "&gt;"; break;
        case '&': r += "&amp;"; break;
        case '"': r += "&quot;"; break;
        case '\'': r += "&apos;"; break;
        default: r += s[i]; break;
        }
    }
    return r;
}

void Esperar_ms(int ms)
{
    if (ms > 0)
        this_thread::sleep_for(chrono::milliseconds(ms));
}

#ifdef _WIN32

void Modo_Tecla(bool)
{
    // No Windows o _kbhit()/_getch() ja funcionam sem ENTER e sem eco.
}

int Ler_Tecla()
{
    if (cin.eof())
        return -1;
    if (_kbhit())
        return _getch();
    return 0;
}

#else

/// Configuracao original do terminal (para repor ao sair do modo tecla).
static struct termios g_term_original;
/// Indica se o terminal esta atualmente em modo tecla.
static bool g_modo_tecla = false;

void Modo_Tecla(bool ligado)
{
    if (ligado == g_modo_tecla || !isatty(STDIN_FILENO))
        return;
    if (ligado) {
        if (tcgetattr(STDIN_FILENO, &g_term_original) != 0)
            return;
        struct termios novo = g_term_original;
        novo.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
        novo.c_cc[VMIN] = 0;
        novo.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &novo);
        g_modo_tecla = true;
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &g_term_original);
        g_modo_tecla = false;
    }
}

int Ler_Tecla()
{
    // entrada redirecionada (ficheiro/pipe): le pelo cin para nao baralhar
    // com as leituras do menu, que tambem usam o cin
    if (!isatty(STDIN_FILENO)) {
        int c = cin.get();
        return (c == EOF) ? -1 : c;
    }
    fd_set leitura;
    FD_ZERO(&leitura);
    FD_SET(STDIN_FILENO, &leitura);
    struct timeval espera;
    espera.tv_sec = 0;
    espera.tv_usec = 0;
    if (select(STDIN_FILENO + 1, &leitura, NULL, NULL, &espera) <= 0)
        return 0;
    char c = 0;
    ssize_t n = read(STDIN_FILENO, &c, 1);
    if (n == 1)
        return static_cast<unsigned char>(c);
    return (n == 0 && !g_modo_tecla) ? -1 : 0;
}

#endif

/**
 * @brief Verifica se o nome do ficheiro so tem caracteres seguros para ser
 *        passado a linha de comandos do sistema.
 */
static bool Nome_Seguro(const string &f)
{
    if (f.empty() || f[0] == '-')
        return false;
    for (size_t i = 0; i < f.size(); i++) {
        unsigned char c = static_cast<unsigned char>(f[i]);
        if (!(isalnum(c) || c == '.' || c == '_' || c == '-' || c == '/' || c == '\\' || c == ':' || c == ' '))
            return false;
    }
    return true;
}

bool Mostrar_Video(const string &ficheiro)
{
    cout << "  >>> VIDEO: \"" << ficheiro << "\"" << endl;
    ifstream teste(ficheiro.c_str());
    if (!teste.is_open()) {
        cout << "      (ficheiro de video nao encontrado - apenas anunciado)" << endl;
        return false;
    }
    teste.close();
    if (!Nome_Seguro(ficheiro)) {
        cout << "      (nome de ficheiro nao permitido - apenas anunciado)" << endl;
        return false;
    }
#ifdef _WIN32
    string cmd = "start \"\" \"" + ficheiro + "\"";
#elif defined(__APPLE__)
    string cmd = "open \"" + ficheiro + "\" >/dev/null 2>&1 &";
#else
    string cmd = "xdg-open \"" + ficheiro + "\" >/dev/null 2>&1 &";
#endif
    int r = system(cmd.c_str());
    return r == 0;
}

} // namespace Util
