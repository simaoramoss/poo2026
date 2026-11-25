/**
 * @file XML.cpp
 * @brief Implementacao do leitor de XML.
 */
#include "XML.h"
#include "Util.h"

#include <cctype>
#include <fstream>
#include <sstream>

using namespace std;

// ----------------------------------------------------------------- XMLNo

XMLNo::XMLNo(const string &n) : nome(n) {}

// Destrutor: apaga todos os filhos (que por sua vez apagam os seus) -> a
// arvore inteira e libertada com um unico delete da raiz
XMLNo::~XMLNo()
{
    for (list<XMLNo *>::iterator it = filhos.begin(); it != filhos.end(); ++it)
        delete *it;
    filhos.clear();
}

const XMLNo *XMLNo::Filho(const string &n) const
{
    string alvo = Util::Maiusculas(n);
    for (list<XMLNo *>::const_iterator it = filhos.begin(); it != filhos.end(); ++it)
        if ((*it)->nome == alvo)
            return *it;
    return NULL;
}

const XMLNo *XMLNo::Procurar(const string &n) const
{
    string alvo = Util::Maiusculas(n);
    for (list<XMLNo *>::const_iterator it = filhos.begin(); it != filhos.end(); ++it) {
        if ((*it)->nome == alvo)
            return *it;
        const XMLNo *r = (*it)->Procurar(alvo);
        if (r != NULL)
            return r;
    }
    return NULL;
}

bool XMLNo::Tem(const string &chave) const
{
    string k = Util::Maiusculas(chave);
    return atribs.find(k) != atribs.end() || Filho(k) != NULL;
}

// Devolve o valor de uma chave: primeiro procura nos atributos (ID="1"),
// depois nos filhos (<ID>1</ID>); se nao existir devolve def
string XMLNo::Valor(const string &chave, const string &def) const
{
    string k = Util::Maiusculas(chave);
    map<string, string>::const_iterator it = atribs.find(k);
    if (it != atribs.end())
        return it->second;
    const XMLNo *f = Filho(k);
    if (f != NULL)
        return f->texto;
    return def;
}

// ------------------------------------------------------------- XMLParser

XMLParser::XMLParser(const string &t) : txt(t), pos(0) {}

void XMLParser::Saltar_Espacos()
{
    while (!Fim() && isspace(static_cast<unsigned char>(txt[pos])))
        pos++;
}

bool XMLParser::Comeca_Com(const char *s) const
{
    return txt.compare(pos, string(s).size(), s) == 0;
}

/** Salta declaracoes (<?...?>), comentarios (<!--...-->) e <!DOCTYPE ...>. */
void XMLParser::Saltar_Prologo()
{
    while (true) {
        Saltar_Espacos();
        size_t fim;
        if (Comeca_Com("<?")) {
            fim = txt.find("?>", pos);
            pos = (fim == string::npos) ? txt.size() : fim + 2;
        } else if (Comeca_Com("<!--")) {
            fim = txt.find("-->", pos);
            pos = (fim == string::npos) ? txt.size() : fim + 3;
        } else if (Comeca_Com("<!")) {
            fim = txt.find('>', pos);
            pos = (fim == string::npos) ? txt.size() : fim + 1;
        } else {
            return;
        }
    }
}

string XMLParser::Ler_Nome()
{
    size_t ini = pos;
    while (!Fim()) {
        unsigned char c = static_cast<unsigned char>(txt[pos]);
        if (isalnum(c) || c == '_' || c == '-' || c == '.' || c == ':' || c >= 128)
            pos++;
        else
            break;
    }
    return Util::Maiusculas(txt.substr(ini, pos - ini));
}

string XMLParser::Descodificar(const string &s)
{
    static const char *ents[] = {"&lt;", "&gt;", "&amp;", "&quot;", "&apos;"};
    static const char chars[] = {'<', '>', '&', '"', '\''};
    string r;
    size_t i = 0;
    while (i < s.size()) {
        bool trocou = false;
        if (s[i] == '&') {
            for (int e = 0; e < 5 && !trocou; e++) {
                string ent = ents[e];
                if (s.compare(i, ent.size(), ent) == 0) {
                    r += chars[e];
                    i += ent.size();
                    trocou = true;
                }
            }
        }
        if (!trocou)
            r += s[i++];
    }
    return r;
}

bool XMLParser::Ler_Atributos(XMLNo *no)
{
    while (true) {
        Saltar_Espacos();
        if (Fim()) {
            erro = "fim inesperado dentro de <" + no->nome + ">";
            return false;
        }
        if (txt[pos] == '>' || txt[pos] == '/')
            return true;
        string chave = Ler_Nome();
        if (chave.empty()) {
            erro = "atributo invalido em <" + no->nome + ">";
            return false;
        }
        Saltar_Espacos();
        if (Fim() || txt[pos] != '=') {
            erro = "falta '=' no atributo " + chave + " de <" + no->nome + ">";
            return false;
        }
        pos++;
        Saltar_Espacos();
        if (Fim() || (txt[pos] != '"' && txt[pos] != '\'')) {
            erro = "falta aspas no atributo " + chave + " de <" + no->nome + ">";
            return false;
        }
        char aspas = txt[pos++];
        size_t fim = txt.find(aspas, pos);
        if (fim == string::npos) {
            erro = "aspas por fechar no atributo " + chave + " de <" + no->nome + ">";
            return false;
        }
        no->atribs[chave] = Descodificar(txt.substr(pos, fim - pos));
        pos = fim + 1;
    }
}

// Le um elemento <NOME atrib="..."> ... </NOME> e, recursivamente, os seus filhos
XMLNo *XMLParser::Ler_Elemento()
{
    if (Fim() || txt[pos] != '<') {
        erro = "esperado '<'";
        return NULL;
    }
    pos++;
    string nome = Ler_Nome();
    if (nome.empty()) {
        erro = "nome de elemento invalido";
        return NULL;
    }
    XMLNo *no = new XMLNo(nome);
    if (!Ler_Atributos(no)) {
        delete no;
        return NULL;
    }
    if (txt[pos] == '/') { // elemento vazio: <A ... />
        pos++;
        if (Fim() || txt[pos] != '>') {
            erro = "esperado '>' apos '/' em <" + nome + ">";
            delete no;
            return NULL;
        }
        pos++;
        return no;
    }
    pos++; // '>'

    string texto;
    while (true) {
        if (Fim()) {
            erro = "elemento <" + nome + "> nao foi fechado";
            delete no;
            return NULL;
        }
        if (Comeca_Com("<!--")) {
            size_t fim = txt.find("-->", pos);
            pos = (fim == string::npos) ? txt.size() : fim + 3;
        } else if (Comeca_Com("<![CDATA[")) {
            size_t fim = txt.find("]]>", pos);
            if (fim == string::npos) {
                erro = "CDATA por fechar em <" + nome + ">";
                delete no;
                return NULL;
            }
            texto += txt.substr(pos + 9, fim - pos - 9);
            pos = fim + 3;
        } else if (Comeca_Com("</")) {
            pos += 2;
            string fecho = Ler_Nome();
            Saltar_Espacos();
            if (fecho != nome || Fim() || txt[pos] != '>') {
                erro = "fecho </" + fecho + "> nao corresponde a <" + nome + ">";
                delete no;
                return NULL;
            }
            pos++;
            break;
        } else if (txt[pos] == '<') {
            XMLNo *filho = Ler_Elemento();
            if (filho == NULL) {
                delete no;
                return NULL;
            }
            no->filhos.push_back(filho);
        } else {
            size_t fim = txt.find('<', pos);
            if (fim == string::npos)
                fim = txt.size();
            texto += Descodificar(txt.substr(pos, fim - pos));
            pos = fim;
        }
    }
    no->texto = Util::Aparar(texto);
    return no;
}

XMLNo *XMLParser::Ler_Texto(const string &texto, string &erro)
{
    XMLParser p(texto);
    // ignora a marca BOM do UTF-8, se existir
    if (texto.size() >= 3 && texto.compare(0, 3, "\xEF\xBB\xBF") == 0)
        p.pos = 3;
    p.Saltar_Prologo();
    if (p.Fim()) {
        erro = "documento vazio";
        return NULL;
    }
    XMLNo *raiz = p.Ler_Elemento();
    if (raiz == NULL)
        erro = p.erro + " (posicao " + Util::Para_Texto(static_cast<int>(p.pos)) + ")";
    return raiz;
}

// Le o ficheiro todo para uma string e analisa-a
XMLNo *XMLParser::Ler_Ficheiro(const string &ficheiro, string &erro)
{
    ifstream f(ficheiro.c_str(), ios::in | ios::binary);
    if (!f.is_open()) {
        erro = "nao foi possivel abrir o ficheiro \"" + ficheiro + "\"";
        return NULL;
    }
    ostringstream conteudo;
    conteudo << f.rdbuf();
    f.close();
    return Ler_Texto(conteudo.str(), erro);
}
