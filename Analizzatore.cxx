#include "Src/Fisica.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

double findEdge(vector <double> vettore, double from) {
  for (int i = from+1; i < vettore.size(); ++i) {
    //cout << vettore[from] << ":" << vettore[i] << endl;
    //cout << vettore[from] << "\t" << vettore[i] << endl;
    if ((vettore[from]+i-from) != vettore[i]) {
      return i-1;
    }
  }
  return vettore.size()-1;
}

void strutturaVector (vector <double> vettore) {
  int i = 0;
  while (i < vettore.size()) {
    int to = findEdge(vettore, i);
    cout << "[" << vettore[i] << ";" << vettore[to] << "] ";
    i = to+1;
  }
  cout << endl;
}

void printInterp(Interpolazione interp) {
  cout << "a: " << approssima(interp.a) << " b: " << approssima(interp.b) << endl
       << interp.corr << " T: " << interp.t_stud << " g.l. " << interp.gl << endl;
}

vector <double> reciproco(vector <double> vettore) {
  vector <double> res;
  for (int i = 0; i < vettore.size(); ++i) {
    res.push_back(1.0/vettore[i]);
  }
  return res;
}

struct SerieDati {
  DataTable<double> rawdata, espansione, compressione;
  double sigma_P = 0.01, sigma_V = 0.1, sigma_T = 0.1;
  // 1/P [cm^2/kg] +- 0.01 // V [cm^3] +- 0.1 // T [°C] +- 0.1

  bool debug_divisione = true;

  void Dividi(void) {

    //Calcolo già il reciproco delle pressioni
    vector <double> pressioni = this->rawdata.getColumn(0);
    this->rawdata.removeColumn(0);
    this->rawdata.insertColumn(reciproco(pressioni), 0);


    vector <double> index_comp, index_exp, index_scarti;
    bool inizio = true;
    for (int i = 0; i < rawdata.x; ++i) {
      vector <double> riga = rawdata.getRow(i);
      double volume = riga[1]; //Estraggo il volume
      double differenziale; //Studio l'andamento
      int ref = i+10;
      if (ref < rawdata.x) {
        double volume2 = rawdata.getRow(ref)[1];
        differenziale = (volume2-volume);
      } else {
        differenziale = 0;
      }


      if ((volume <= 22.5) && (volume >= 5.3) && (inizio == true)) { //Compressione
        index_comp.push_back(i);
        this->compressione.insertRow(riga);
      } else if ((volume < 5.3) && (inizio == true)) {
        inizio = false;
      } else if ((volume >= 5.3) && (volume <= 22.5) && (inizio == false)) { //Espansione
        index_exp.push_back(i);
        this->espansione.insertRow(riga);
      } else {
        index_scarti.push_back(i);
      }

    }
    //cout << findEdge(index_comp, 10) << endl;

    if (debug_divisione) {
      cout << "Comp:\t"; strutturaVector(index_comp); cout << endl;
      cout << "Esp.:\t"; strutturaVector(index_exp); cout << endl;
      cout << "Scar:\t"; strutturaVector(index_scarti); cout << endl;
    }
  }
  void AnalisiStatistica(void) {
    //Valutare con Mathematica l'errore dato dalla temperatura
    vector <double> pressioni = this->compressione.getColumn(0);
    vector <double> volumi = this->compressione.getColumn(1);
    vector <double> temperature = this->compressione.getColumn(2);

    misura media_temp_comp = media(temperature);
    cout << "Media temp (compressione): " << approssima(media_temp_comp) << endl;
    Interpolazione comp = interpola(pressioni, volumi);
    comp.printAll();
    this->compressione.printToFile("Results/" + approssima(media_temp_comp.val) + "Comp.txt");

    pressioni = this->espansione.getColumn(0);
    volumi = this->espansione.getColumn(1);
    temperature = this->espansione.getColumn(2);
    misura media_temp_exp = media(temperature);
    cout << "Media temp (espansione): " << approssima(media_temp_exp) << endl;
    Interpolazione esp = interpola(pressioni, volumi);
    esp.printAll();
    this->espansione.printToFile("Results/" + approssima(media_temp_exp.val) + "Exp.txt");

    misura R = {8.314472,0}; //J/(K*mol)
    media_temp_exp.val += 273.15;
    media_temp_comp.val += 273.15; //Trasformo in Kelvin

    misura n_exp = dividi(dividi(esp.b, R), media_temp_exp);
    misura n_comp = dividi(dividi(comp.b, R), media_temp_comp);

    cout << "n exp: " << approssima(n_exp) << " n comp: " << approssima(n_comp) << endl;
    cout << "Compatibilità: " << compatibile(n_exp, n_comp) << "\n";

    //FIT or FEED
  }
};
int main(void) {
  /* Acquisizione dati */

  vector <SerieDati> tutteMisure;
  for (int i = 0; i < 6+1; ++i) {
    ifstream inputfile("Data/csv/" + to_string(i) + ".csv");
    if (!inputfile) {
      cout << "Errore I/O al punto: " << i << endl;
    }
    string riga;
    vector <string> celle;

    SerieDati serieMisure; //Creo una nuova struttura per contenere le misure
    while (inputfile >> riga) {
      celle = parseRow(riga);
      double Pinv = stod(celle[0]), V = stod(celle[1]), T = stod(celle[2]);
      serieMisure.rawdata.insertRow({Pinv, V, T});
    }
    tutteMisure.push_back(serieMisure); //E la inserisco nel vettore

    vector <double> temperature = serieMisure.rawdata.getColumn(2);
    misura media_T = media(temperature);
    cout << "Caricata serie di misure n. " << i << " con media temperature " << approssima(media_T) << endl;

    //serieMisure.misure.printAll();

    /* //Codice per sezionare le misure
    DataTable<double> compressione, espansione;
    //Prima c'è l'espansione
    for (int i = 0; i < serieMisure.misure.x; ++i) { //Scorro le righe

    } */
  }
  cout << "Caricamento completato!\n\n\n";

  for (int i = 0; i < tutteMisure.size(); ++i) {
    cout << "Serie " << i << endl;
    tutteMisure[i].Dividi();
  }
  tutteMisure[0].AnalisiStatistica();

  return 0;
}
