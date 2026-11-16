/**
 * @file Fabrica.h
 * @brief Classe principal: gestao e simulacao da fabrica.
 */
#ifndef FABRICA_H
#define FABRICA_H

#include <iostream>
#include <list>
#include <string>

#include "Motor.h"
#include "Sensor.h"
#include "User.h"

class XMLNo;

/**
 * @class Fabrica
 * @brief Gere utilizadores, motores e sensores e simula o funcionamento da fabrica.
 *
 * Gestao de memoria: a fabrica e DONA de todos os utilizadores, motores e
 * sensores que lhe sao entregues (construtor, Add() com sucesso e Load()) e
 * liberta-os no destrutor. Quando um Add() devolve false, o objeto continua a
 * pertencer a quem chamou. As listas devolvidas por Listar_Tipo() e pelos
 * rankings sao criadas com new e devem ser libertadas por quem chama (apenas a
 * lista; os motores continuam a pertencer a fabrica).
 */
class Fabrica {
    std::string nome;                 ///< Nome da fabrica.
    User *atual;                      ///< Utilizador que esta a operar (pertence a lista users).
    std::list<User *> users;          ///< Utilizadores registados.
    std::list<Motor *> motores;       ///< Motores instalados.
    std::list<Sensor *> sensores;     ///< Sensores instalados.
    std::list<std::string> avisos;    ///< Historico de avisos enviados ao gestor.

    int hora_abertura;                ///< Hora de abertura (0..23).
    int hora_fecho;                   ///< Hora de encerramento (0..24).
    int dia;                          ///< Dia de simulacao (comeca em 1).
    int minutos;                      ///< Minutos desde as 00:00 do dia corrente.
    double vizinhanca;                ///< Raio de vizinhanca dos sensores de humidade.
    double largura;                   ///< Largura da planta (para o mapa HTML).
    double altura;                    ///< Altura da planta (para o mapa HTML).
    double verde_min;                 ///< Inicio da zona VERDE.
    double amarelo_min;               ///< Inicio da zona AMARELO (temperatura de aviso por omissao).
    double vermelho_min;              ///< Inicio da zona VERMELHO (temperatura de paragem por omissao).
    double vermelho_max;              ///< Fim da zona VERMELHO.
    std::string video_fumo;           ///< Video das saidas de emergencia.
    std::string video_luz;            ///< Video de "bom regresso a casa".
    std::string video_missel;         ///< Video dos abrigos.
    bool desligada;                   ///< true depois de um aviso de missil.

    Fabrica(const Fabrica &);            // copia nao permitida
    Fabrica &operator=(const Fabrica &); // atribuicao nao permitida

    // ---- metodos auxiliares privados ----
    bool Pode(PERMISSAO p, const char *operacao) const;
    User *Procurar_User(int id) const;
    Motor *Procurar_Motor(int id) const;
    Sensor *Procurar_Sensor(int id) const;
    bool Inserir(User *ut);
    bool Inserir(Motor *m);
    bool Inserir(Sensor *s);
    void Libertar_Dispositivos();
    void Ler_Configuracao(const XMLNo *raiz);
    void Carregar_No(const XMLNo *no, int &n_ok, int &n_erros);
    void Avisar_Gestor(const std::string &msg);
    bool Aberta() const;
    void Abrir();
    void Fechar();
    int Parar_Todos(std::list<Motor *> &lm);
    void Simular_Motores(double horas);
    void Simular_Sensores(double horas);
    double Consumo_Dia() const;

public:
    /**
     * @brief Constroi a fabrica, sendo dado o utilizador atual.
     * @param ut Utilizador que opera a fabrica; passa a pertencer a fabrica.
     */
    Fabrica(User *ut);

    /** @brief Liberta todos os utilizadores, motores e sensores. */
    ~Fabrica();

    /**
     * @brief Carrega a configuracao (horarios, temperaturas, utilizadores,
     *        motores e sensores) de um ficheiro XML. Substitui os dispositivos
     *        existentes.
     * @return false se o ficheiro nao existir ou nao for XML valido.
     */
    bool Load(const std::string &ficheiro);

    /**
     * @brief Adiciona um utilizador. Regras: o utilizador atual precisa da
     *        permissao ADD, so pode adicionar utilizadores de nivel igual ou
     *        inferior ao seu e o ID nao pode estar repetido.
     */
    bool Add(User *ut);

    /** @brief Adiciona um motor (permissao ADD; ID de motor unico). */
    bool Add(Motor *m);

    /** @brief Adiciona um sensor (permissao ADD; ID de sensor unico). */
    bool Add(Sensor *s);

    /** @brief Remove (e destroi) um motor, dado o seu ID (permissao REMOVER). */
    bool Remover(int id_motor);

    /** @brief Lista o estado atual da fabrica. */
    void Listar(std::ostream &f = std::cout);

    /** @brief Liga (START) um motor especifico, dado o seu ID (permissao RUN). */
    bool Ligar(int id_motor);

    /** @brief Desliga um motor especifico, dado o seu ID (permissao RUN). */
    void Desligar(int id_motor);

    /** @brief Estado de um motor, dado o seu ID (ESTADO_INVALIDO se nao existir). */
    ESTADO_MOTOR Get_Estado(int id_motor);

    /**
     * @brief Lista e devolve todos os motores de um dado tipo.
     * @return Lista nova (a libertar com delete por quem chama).
     */
    std::list<Motor *> *Listar_Tipo(std::string Tipo, std::ostream &f = std::cout);

    /**
     * @brief Manutencao de todos os motores e sensores (permissao MANUTENCAO):
     *        repara avarias e coloca a temperatura dos motores na zona VERDE.
     */
    bool Manutencao();

    /**
     * @brief Marcas de motores/sensores ordenadas da que mais avaria para a mais fiavel.
     * @return Lista nova (a libertar com delete por quem chama).
     */
    std::list<std::string> *Ranking_Dos_Fracos();

    /**
     * @brief Motores ordenados do que mais trabalhou para o que menos trabalhou.
     * @return Lista nova (a libertar com delete por quem chama).
     */
    std::list<Motor *> *Ranking_Dos_Mais_Trabalhadores();

    /** @brief Grava um relatorio XML do estado da fabrica, com o consumo total do dia. */
    void Relatorio(std::string fich_xml);

    /**
     * @brief Aviso de humidade: desliga os motores na vizinhanca dos sensores
     *        de humidade em alerta.
     * @param lm Recebe os motores que foram desligados.
     * @return Numero de motores desligados.
     */
    int Aviso_Humidade(std::list<Motor *> &lm);

    /**
     * @brief Aviso de fumo: desliga todos os motores e mostra o video das saidas.
     * @param lm Recebe os motores que foram desligados.
     * @return Numero de motores desligados.
     */
    int Aviso_Fumo(std::list<Motor *> &lm, std::string fich_video);

    /**
     * @brief Aviso de luz: mostra o video de bom regresso a casa.
     * @return Numero de sensores de luz em alerta.
     */
    int Aviso_Luz(std::string fich_video);

    /**
     * @brief Aviso de missil: mostra o video dos abrigos, guarda o estado atual
     *        em @p festado e desliga completamente a fabrica.
     */
    void Aviso_Missel(std::string fvideo, std::string festado = "Estado.txt");

    /** @brief Grava uma pagina HTML com o estado atual da fabrica. */
    void Listagem_HTML(std::string fich_html);

    // ---- simulacao e apoio ao menu ----

    /** @brief Avanca a simulacao @p min minutos (horario, motores, sensores e avisos). */
    void Simular_Passo(int min);

    /** @brief Escreve uma linha de resumo (hora, estado e contagem de motores). */
    void Resumo(std::ostream &f = std::cout) const;

    /** @brief Mostra os ultimos avisos enviados ao gestor. */
    void Listar_Avisos(std::ostream &f = std::cout, int ultimos = 20) const;

    /** @brief Impoe o valor de um sensor para testar os avisos (permissao RUN). */
    bool Set_Valor_Sensor(int id_sensor, double valor);

    /** @brief Muda o utilizador atual para o utilizador registado com o ID dado. */
    bool Set_User_Atual(int id_user);

    const User *Get_User_Atual() const { return atual; }
    bool Esta_Desligada() const { return desligada; }
    /** @brief Hora simulada atual no formato "Dia N HH:MM". */
    std::string Hora_Texto() const;
    double Get_Temp_Aviso_Omissao() const { return amarelo_min; }
    double Get_Temp_Paragem_Omissao() const { return vermelho_min; }
};

#endif
