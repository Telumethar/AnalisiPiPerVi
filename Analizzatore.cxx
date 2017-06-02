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

bool printInterpToFile(Interpolazione interp, string filename, string extra = "") {
  ofstream file(filename);
  if (!file) {
    return false;
  }
  file << extra << endl;
  file << "a: " << interp.a << "\t b: " << interp.b << endl
       << "Corr: " << interp.corr << " T: " << interp.t_stud << " g.l. " << interp.gl << endl
       << "a: " << approssima(interp.a) << "\t b: " << approssima(interp.b) << endl;
  return true;
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

    //Inserisco indicazione temporale
    vector <double> tempi;
    for (int i = 0; i < this->rawdata.x; ++i) {
      tempi.push_back(i/10.0); //10 misure al secondo
    }
    this->rawdata.insertColumn(tempi);


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

  void FittaIsocora(string folder = "") {
    //TO DO
    vector <double> pressioni = this->rawdata.getColumn(0);
    vector <double> temperature = this->rawdata.getColumn(2);
    vector <double> tempi;
    for (int i = 0; i < this->rawdata.x; ++i) {
      tempi.push_back(i/10.0); //10 misure al secondo
    }
    this->rawdata.insertColumn(tempi);

    Interpolazione temp = interpola(tempi, temperature, 0, this->sigma_T);
    Interpolazione pT = interpola(pressioni, temperature, 0, this->sigma_T);

    printInterpToFile(temp, "Results/"+folder+"_interp_temperature.txt");
    printInterpToFile(pT, "Results/" + folder + "_interp_P_temp.txt");

    this->rawdata.printToFile("Results/"+folder+".txt");

  }

  void AnalisiStatistica(string folder = "") {
    //Compressione
    //Estraggo i dati
    vector <double> pressioni = this->compressione.getColumn(0);
    vector <double> volumi = this->compressione.getColumn(1);
    vector <double> temperature = this->compressione.getColumn(2);
    vector <double> tempi = this->compressione.getColumn(3);

    misura media_temp_comp = media(temperature); //Calcolo la temperatura media durante la trasformazione
    cout << "Media temp (compressione): " << approssima(media_temp_comp) << endl;
    Interpolazione comp = interpola(pressioni, volumi, 0, this->sigma_V); //Interpolo (1/P, V)
    Interpolazione temp_comp = interpola(tempi, temperature, 0, this->sigma_T); //Temperatura in funzione del tempo
    comp.printAll();
    cout << "Temperatura: \n"; temp_comp.printAll();
    this->compressione.printToFile("Results/Graphs/" + folder + approssima(media_temp_comp.val, 3) + "Comp.txt"); //Stampo sul file


    //Espansione
    pressioni = this->espansione.getColumn(0);
    volumi = this->espansione.getColumn(1);
    temperature = this->espansione.getColumn(2);
    tempi = this->espansione.getColumn(3);

    misura media_temp_exp = media(temperature);
    cout << "Media temp (espansione): " << approssima(media_temp_exp) << endl;
    Interpolazione esp = interpola(pressioni, volumi, 0, this->sigma_V);
    Interpolazione temp_esp = interpola(tempi, temperature, 0, this->sigma_T);

    esp.printAll();
    cout << "Temperatura: \n"; temp_esp.printAll();
    this->espansione.printToFile("Results/Graphs/" + folder + approssima(media_temp_exp.val, 3) + "Exp.txt");

    misura R = {8.314472,0}; //J/(K*mol)

    misura n_exp = dividi(dividi(esp.b, R), {media_temp_exp.val +273.15, media_temp_exp.err});
    misura n_comp = dividi(dividi(comp.b, R), {media_temp_comp.val + 273.15, media_temp_comp.err});

    double compatib_n = compatibile(n_exp, n_comp);
    double compatib_a = compatibile(comp.a, esp.a);
    double compatib_b = compatibile(comp.b, esp.b);
    double compatib_T = compatibile(media_temp_comp, media_temp_exp);
    string filename = "Results/" + folder + "Interp_" + approssima(media_temp_comp.val, 4) + ".txt";
    ofstream dati_interpolazione(filename);
    dati_interpolazione << ",Compressione,Espansione,Compatibilità\n" //Stampo tutti i dati
                        << "$\\overline{T}$," << approssima(media_temp_comp) << "," << approssima(media_temp_exp) << "," << compatib_T << "\n"
                        << "a $[cm^3]$," << approssima(comp.a) << "," << approssima(esp.a) << "," << compatib_a << "\n"
                        << "b $[kg\\,cm]$," << approssima(comp.b) << "," << approssima(esp.b) << "," << compatib_b << "\n"
                        << "n $[mol]$," << approssima(n_comp) << "," << approssima(n_exp) << "," << compatib_n << "\n"
                        << "$r_c$," << comp.corr << "," << esp.corr << ",\n"
                        << "$T_S$," << comp.t_stud << " (" << comp.gl << " g.l.)," << esp.t_stud << " (" << esp.gl << " g.l.),\n";
    //Stampa info temperatura
    dati_interpolazione << "Andamento Temperatura, Compressione, Espansione, Compatibilità\n"
                        << "a $[°C]$," << approssima(temp_comp.a) << "," << approssima(temp_esp.a) << "," << compatibile(temp_comp.a, temp_esp.a) << "\n"
                        << "b $[°C\\cdot s^{-1}]$," << approssima(temp_comp.b) << "," << approssima(temp_esp.b) << "," << compatibile(temp_comp.b, temp_esp.b) << "\n"
                        << "$r_c$," << temp_comp.corr << "," << temp_esp.corr << ",\n"
                        << "$T_S$," << temp_comp.t_stud << " (" << temp_comp.gl << " g.l.)," << temp_esp.t_stud << " (" << temp_esp.gl << " g.l.),\n";
    //FIT or FEED
  }
};

SerieDati loadMisure(string nomefile);

int main(void) {
  /* Acquisizione dati */

  vector <SerieDati> tutteMisure;
  for (int i = 0; i < 6+1; ++i) {
    string filename = "Data/csv/" + to_string(i) + ".csv";

    SerieDati serieMisure = loadMisure(filename);
    tutteMisure.push_back(serieMisure); //E la inserisco nel vettore

    vector <double> temperature = serieMisure.rawdata.getColumn(2);
    misura media_T = media(temperature);
    cout << "Caricata serie di misure n. " << i << " con media temperature " << approssima(media_T) << endl;
  }
  cout << "Caricamento completato!\n\n\n";

  for (int i = 0; i < tutteMisure.size(); ++i) {
    cout << "Serie " << i << endl;
    tutteMisure[i].Dividi();
    tutteMisure[i].AnalisiStatistica();
  }
  //tutteMisure[0].AnalisiStatistica();

  //Misura bella
  SerieDati misurabella = loadMisure("Data/csv2/Bella2.csv");
  misurabella.Dividi();
  misurabella.AnalisiStatistica("ErroriSist/");

  //Lente/Veloci
  SerieDati lenta0 = loadMisure("Data/csv2/0lenta.csv"), veloce0 = loadMisure("Data/csv2/0vel.csv"), veloce24 = loadMisure("Data/csv2/24vel.csv");
  lenta0.Dividi(); veloce0.Dividi(); veloce24.Dividi();
  lenta0.AnalisiStatistica("ErroriSist/Lenta_");
  veloce0.AnalisiStatistica("ErroriSist/Veloce_");
  veloce24.AnalisiStatistica("ErroriSist/Veloce_");

  //N diversa
  SerieDati ndiversa = loadMisure("Data/csv2/Ndiversa.csv");
  ndiversa.Dividi();
  ndiversa.AnalisiStatistica("ErroriSist/Ndiversa_");

  //Isocora e tenute
  SerieDati isocora = loadMisure("Data/csv2/Isocora.csv");
  isocora.FittaIsocora("ErroriSist/Isocora");

  SerieDati tenutaN = loadMisure("Data/csv2/TenutaN.csv");
  tenutaN.FittaIsocora("ErroriSist/TenutaN");

  SerieDati tenutaT = loadMisure("Data/csv2/TenutaCalore.csv");
  tenutaT.FittaIsocora("ErroriSist/TenutaT");

  return 0;
}

SerieDati loadMisure(string nomefile) {
  ifstream inputfile(nomefile);
  if (!inputfile) {
    cout << "Errore I/O, file " << nomefile << " non trovato. " << endl;
  }
  string riga;
  vector <string> celle;

  SerieDati serieMisure; //Creo una nuova struttura per contenere le misure
  while (inputfile >> riga) {
    celle = parseRow(riga);
    double Pinv = stod(celle[0]), V = stod(celle[1]), T = stod(celle[2]);
    serieMisure.rawdata.insertRow({Pinv, V, T});
  }

  return serieMisure;
}
