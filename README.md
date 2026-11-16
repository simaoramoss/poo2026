# Gestão de Fábrica — Projeto POO 2026/2027

Simulador, em C++ standard (C++11, sem bibliotecas externas), da gestão de uma
fábrica com utilizadores, motores e sensores, configurada por um ficheiro XML.

## Compilar e correr

```bash
make
./fabrica fabrica.xml
```

Sem `make` (por exemplo no Windows com MinGW):

```bash
g++ -std=c++11 -Wall -o fabrica main.cpp Fabrica.cpp Motor.cpp Sensor.cpp Dispositivo.cpp User.cpp XML.cpp Util.cpp
```

Argumentos opcionais: `fabrica [ficheiro.xml] [n_passos] [ms_por_passo]`
(`n_passos` = 0 corre até sair; cada passo simula 10 minutos).

Durante a simulação: **M** abre o menu (o simulador pára até a opção ser
executada) e **Q** termina.

## Organização

| Ficheiro | Conteúdo |
|---|---|
| `User.*` | `User` (abstrata) → `Admin`, `Normal`, `Visitante` e permissões |
| `Dispositivo.*` | Base comum de motores e sensores (ID, marca, posição, avarias) |
| `Motor.*` | `Motor` → `MotorEletrico`, `MotorCombustao`, `MotorInducao` |
| `Sensor.*` | `Sensor` → `SensorLuz`, `SensorHumidade`, `SensorFumo`, `SensorMisseis` |
| `Fabrica.*` | Classe principal com todos os métodos do enunciado e a simulação |
| `XML.*` | Leitor de XML próprio (`XMLNo`, `XMLParser`) |
| `Util.*` | Aleatórios, conversões, teclado sem bloqueio, vídeo |
| `main.cpp` | Ciclo do simulador e menu |
| `fabrica.xml` | Configuração de exemplo |

## Decisões de implementação

- **Memória**: a `Fabrica` é dona dos utilizadores, motores e sensores e
  liberta-os no destrutor. Se um `Add()` devolver `false`, o objeto continua a
  ser de quem chamou. As listas devolvidas por `Listar_Tipo()` e pelos rankings
  devem ser libertadas com `delete` (só a lista, não os motores).
- **Permissões**: verificadas dentro da `Fabrica` contra o utilizador atual.
  Regra extra para utilizadores: só se pode adicionar um utilizador de nível
  igual ou inferior ao próprio (um `Normal` não cria um `Admin`).
- **Temperatura**: cada motor usa a sua `TEMPERATURA_AVISO` (AMARELO) e
  `TEMPERATURA_PARAGEM` (VERMELHO); se faltarem no XML, usam-se os intervalos
  globais de `<TEMPERATURAS>`.
- **Avisos**: disparam uma única vez, na transição para o estado de alerta, e
  ficam registados no histórico de avisos ao gestor.
- **Vídeos**: são anunciados na consola e, se o ficheiro existir, abertos com o
  leitor por omissão do sistema.
- A instrução `auto` não é usada.

## Documentação

```bash
doxygen Doxyfile
```

Gera a documentação em `docs/html/index.html`.
