/**
 * @file Util.cpp
 * @brief Implementacao das funcoes auxiliares.
 */
#include "Util.h"

#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <sstream>

using namespace std;

namespace Util {

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

} // namespace Util
