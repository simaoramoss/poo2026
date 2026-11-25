# Projeto POO 2026/2027 - Gestao de Fabrica
CXX      ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -pedantic -O2
OBJS      = main.o Fabrica.o Motor.o Sensor.o Dispositivo.o User.o XML.o Util.o
LDLIBS    =

fabrica: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDLIBS)

%.o: %.cpp *.h
	$(CXX) $(CXXFLAGS) -c $<

run: fabrica
	./fabrica fabrica.xml

docs:
	doxygen Doxyfile

clean:
	rm -f $(OBJS) fabrica

.PHONY: run docs clean
