/**
 * @file Util.h
 * @brief Funcoes auxiliares genericas (aleatorios, texto, teclado, video).
 */
#ifndef UTIL_H
#define UTIL_H

#include <string>

/**
 * @namespace Util
 * @brief Conjunto de funcoes utilitarias usadas por todo o projeto.
 */
namespace Util {

/** @brief Inicializa o gerador de numeros aleatorios (uma vez, no arranque). */
void Iniciar_Aleatorio();

/** @brief Devolve um real aleatorio no intervalo [min, max]. */
double Aleatorio(double min, double max);

/** @brief Devolve true com probabilidade @p prob (0..1). */
bool Acontece(double prob);

/** @brief Devolve uma copia do texto em maiusculas. */
std::string Maiusculas(const std::string &s);

/** @brief Remove espacos no inicio e no fim do texto. */
std::string Aparar(const std::string &s);

/** @brief Converte texto para inteiro; devolve @p def se nao for um numero. */
int Para_Int(const std::string &s, int def);

/** @brief Converte texto para real; devolve @p def se nao for um numero. */
double Para_Double(const std::string &s, double def);

/** @brief Converte um real para texto com @p casas casas decimais. */
std::string Para_Texto(double v, int casas = 2);

/** @brief Converte um inteiro para texto. */
std::string Para_Texto(int v);

/** @brief Substitui os caracteres especiais (<, >, &, ", ') por entidades XML/HTML. */
std::string Escapar(const std::string &s);

/** @brief Suspende a execucao durante @p ms milissegundos. */
void Esperar_ms(int ms);

/**
 * @brief Liga/desliga a leitura de teclas sem ENTER e sem eco.
 * @param ligado true durante a simulacao, false quando se usa o menu.
 */
void Modo_Tecla(bool ligado);

/**
 * @brief Le uma tecla sem bloquear o programa.
 * @return O caracter premido, 0 se nao ha tecla, -1 se a entrada terminou (EOF).
 */
int Ler_Tecla();

/**
 * @brief "Passa" um video: anuncia-o na consola e, se o ficheiro existir,
 *        abre-o com o leitor por omissao do sistema operativo.
 * @return true se o ficheiro existe e foi pedido ao sistema para o abrir.
 */
bool Mostrar_Video(const std::string &ficheiro);

} // namespace Util

#endif
