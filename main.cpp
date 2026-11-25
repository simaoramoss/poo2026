/**
 * @file main.cpp
 * @brief Programa de teste: carrega a fabrica do XML e lista o seu conteudo.
 *
 * Utilizacao: fabrica [ficheiro.xml]   (por omissao "fabrica.xml")
 */
#include <iostream>
#include <string>

#include "Fabrica.h"

using namespace std;

int main(int argc, char *argv[])
{
    // ficheiro XML: o 1o argumento, ou "fabrica.xml" se nao for dado
    string ficheiro = (argc > 1) ? argv[1] : "fabrica.xml";

    // o utilizador que arranca o programa e o administrador (gestor)
    Fabrica fab(new Admin(0, "Gestor"));
    if (!fab.Load(ficheiro)) // se o XML nao abrir, termina com erro
        return 1;

    fab.Listar(); // mostra tudo na consola (cout)
    return 0; // o destrutor de Fabrica liberta toda a memoria
}
