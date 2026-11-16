/**
 * @file XML.h
 * @brief Leitor de XML minimalista (sem bibliotecas externas).
 *
 * Suporta elementos, atributos, texto, comentarios, declaracao <?xml ?> e
 * elementos vazios (<A/>). Os nomes de elementos e atributos sao guardados em
 * maiusculas, pelo que a pesquisa nao distingue maiusculas de minusculas.
 */
#ifndef XML_H
#define XML_H

#include <list>
#include <map>
#include <string>

/**
 * @class XMLNo
 * @brief Um elemento de um documento XML (no da arvore).
 *
 * Cada no e dono dos seus filhos: ao ser destruido destroi toda a sub-arvore.
 */
class XMLNo {
    std::string nome;                           ///< Nome do elemento (maiusculas).
    std::string texto;                          ///< Texto contido no elemento.
    std::map<std::string, std::string> atribs;  ///< Atributos (chave em maiusculas).
    std::list<XMLNo *> filhos;                  ///< Elementos filhos (pertencem ao no).

    XMLNo(const XMLNo &);            // copia nao permitida
    XMLNo &operator=(const XMLNo &); // atribuicao nao permitida

    friend class XMLParser;

public:
    /** @brief Cria um no com o nome dado. */
    explicit XMLNo(const std::string &n);
    /** @brief Destroi o no e todos os seus filhos. */
    ~XMLNo();

    const std::string &Nome() const { return nome; }
    const std::string &Texto() const { return texto; }
    const std::list<XMLNo *> &Filhos() const { return filhos; }

    /** @brief Devolve o primeiro filho direto com o nome dado (ou NULL). */
    const XMLNo *Filho(const std::string &n) const;

    /** @brief Procura em profundidade o primeiro descendente com o nome dado (ou NULL). */
    const XMLNo *Procurar(const std::string &n) const;

    /** @brief Indica se existe um atributo ou um filho com o nome @p chave. */
    bool Tem(const std::string &chave) const;

    /**
     * @brief Devolve o valor de @p chave, quer esteja como atributo
     *        (<M ID="1"/>) quer como elemento filho (<M><ID>1</ID></M>).
     * @param def Valor devolvido quando a chave nao existe.
     */
    std::string Valor(const std::string &chave, const std::string &def = "") const;
};

/**
 * @class XMLParser
 * @brief Analisador sintatico que constroi a arvore de XMLNo a partir de texto.
 */
class XMLParser {
    const std::string &txt; ///< Texto a analisar.
    size_t pos;             ///< Posicao atual no texto.
    std::string erro;       ///< Descricao do primeiro erro encontrado.

    explicit XMLParser(const std::string &t);

    bool Fim() const { return pos >= txt.size(); }
    void Saltar_Espacos();
    bool Comeca_Com(const char *s) const;
    void Saltar_Prologo();
    std::string Ler_Nome();
    bool Ler_Atributos(XMLNo *no);
    XMLNo *Ler_Elemento();
    static std::string Descodificar(const std::string &s);

public:
    /**
     * @brief Le e analisa um ficheiro XML.
     * @param ficheiro Caminho do ficheiro.
     * @param erro     Recebe a descricao do erro em caso de falha.
     * @return Raiz da arvore (a libertar com delete por quem chama) ou NULL.
     */
    static XMLNo *Ler_Ficheiro(const std::string &ficheiro, std::string &erro);

    /** @brief Analisa XML ja em memoria. Ver Ler_Ficheiro(). */
    static XMLNo *Ler_Texto(const std::string &texto, std::string &erro);
};

#endif
