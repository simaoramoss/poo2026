# Gestão de Fábrica — Projeto POO 2026/2027

Gestão de uma fábrica com utilizadores, motores e sensores, em C++ standard
(C++11, sem bibliotecas externas), configurada por um ficheiro XML.

**Estado (1ª etapa):** hierarquia de classes, `Fabrica::Load` (XML),
`Fabrica::Add` e `Fabrica::Listar`. A simulação e os restantes métodos do
enunciado ficam para a 2ª etapa.

## Compilar e correr

```bash
make
./fabrica fabrica.xml
```

Sem `make` (por exemplo no Windows com MinGW):

```bash
g++ -std=c++11 -Wall -o fabrica main.cpp Fabrica.cpp Motor.cpp Sensor.cpp Dispositivo.cpp User.cpp XML.cpp Util.cpp
```

O programa carrega o XML (por omissão `fabrica.xml`) e lista utilizadores,
motores e sensores.

## Organização

| Ficheiro | Conteúdo |
|---|---|
| `User.*` | `User` (abstrata) → `Admin`, `Normal`, `Visitante` e permissões |
| `Dispositivo.*` | Base comum de motores e sensores (ID, marca, posição, avarias) |
| `Motor.*` | `Motor` → `MotorEletrico`, `MotorCombustao`, `MotorInducao` |
| `Sensor.*` | `Sensor` → `SensorLuz`, `SensorHumidade`, `SensorFumo`, `SensorMisseis` |
| `Fabrica.*` | Classe principal: construtor, `Load`, `Add`, `Listar` |
| `XML.*` | Leitor de XML próprio (`XMLNo`, `XMLParser`) |
| `Util.*` | Conversões de texto |
| `main.cpp` | Carrega o XML e lista a fábrica |
| `fabrica.xml` | Configuração de exemplo |

## Decisões de implementação

- **Memória**: a `Fabrica` é dona dos utilizadores, motores e sensores e
  liberta-os no destrutor. Se um `Add()` devolver `false`, o objeto continua a
  ser de quem chamou.
- **Permissões**: verificadas dentro da `Fabrica` contra o utilizador atual.
  Regra extra para utilizadores: só se pode adicionar um utilizador de nível
  igual ou inferior ao próprio (um `Normal` não cria um `Admin`).
- **Temperatura**: cada motor usa a sua `TEMPERATURA_AVISO` (AMARELO) e
  `TEMPERATURA_PARAGEM` (VERMELHO); se faltarem no XML, usam-se os intervalos
  globais de `<TEMPERATURAS>`.
- **Herança**: `Dispositivo` é a base abstrata de `Motor` e `Sensor`;
  `Tipo()`, `Mostrar()` e `Esta_Avariado()` são virtuais e a `Fabrica::Listar`
  usa-os de forma polimórfica.
- A instrução `auto` não é usada.

## Documentação

```bash
doxygen Doxyfile
```

Gera a documentação em `docs/html/index.html`.
