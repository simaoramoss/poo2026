/**
 * @file Util.h
 * @brief Funcoes auxiliares de texto e conversao.
 */
#ifndef UTIL_H
#define UTIL_H

#include <string>

/**
 * @namespace Util
 * @brief Conjunto de funcoes utilitarias usadas pelo projeto.
 */
namespace Util {

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

} // namespace Util

#endif
