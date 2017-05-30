CC=g++#		Compilatore utilizzato
PARAM=-std=c++11#	Standard del C++11
DOVE=#	Specifica la directory di destinazione

Analizzatore.out : Analizzatore.cxx Src/Dati.cxx Src/ManipolaDati.cxx Src/Statistica.cxx Src/Fit.cxx Src/Stima.cxx Src/Output.cxx Src/Fisica.h
	$(CC) -o $(DOVE)Analizzatore.out Analizzatore.cxx Src/Dati.cxx Src/ManipolaDati.cxx Src/Statistica.cxx Src/Fit.cxx Src/Stima.cxx Src/Output.cxx $(PARAM)
