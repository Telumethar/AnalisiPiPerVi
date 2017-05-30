#include "Fisica.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

vector <string> parseRow(string riga, string delimiter) {
  size_t pos = 0;
  string token;
  vector <string> parsedString;

  int cont = 0;
  while (pos = riga.find(delimiter)) { //Cerca la prossima occorrenza del delimiter all'interno della stringa
  //Se il delimiter non viene trovato, str.find() restituisce la posizione finale della stringa (string::npos)
  //e quindi il ciclo while si interrompe
    if (pos == string::npos)
      break;
    token = riga.substr(0, pos); //Se invece find trova delimiter, restituisce la sua posizione. Estraggo tutto ciò che c'era prima
    parsedString.push_back(token); //lo inserisco nel vettore
    riga.erase(0, pos + delimiter.length()); //Lo rimuovo dalla stringa originale. Così, alla prossima iterazione
    //del ciclo, find troverà il delimiter successivo, e quindi estrarrò la porzione successiva
  }
  //L'ultimo elemento lo devo aggiungere a mano
  parsedString.push_back(riga);

  return parsedString;
}
