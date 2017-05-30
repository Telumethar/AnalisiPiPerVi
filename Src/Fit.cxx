#include "Fisica.h"

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

Interpolazione interpola(vector <double> &vettor_x, vector <double> &vettor_y, double errx, double erry) {
  //Vettori passati con pass by reference (velocizza l'esecuzione)
  //Ipotesi: sigma_x << sigma_y (sigma_x trascurabile)
  //sigma_y costante

  Interpolazione result;

  bool scambia_assi = false;

  //Controlla se interpolabile: i due vettori devono avere la stessa dimensione!
  int N = vettor_x.size();
  if (N != vettor_y.size()) {
    result.riuscita = false;
    return result;
  }

  //Controllo se devo invertire gli assi
  double tot_err_x = 0, tot_err_y = 0;
  if ((errx != -1) && (erry != -1)) {
    for (int i = 0; i < N; ++i) {
      tot_err_x += errore_percentuale({vettor_x[i], errx});
      tot_err_y += errore_percentuale({vettor_y[i], erry});
    }
    //cout << tot_err_x << "\t" << tot_err_y << endl;
    if (tot_err_x > tot_err_y) {
      scambia_assi = true;
      cout << "Errore relativo totale sulle x " << tot_err_x << " > errore sulle y " << tot_err_y << endl
           << "ATTENZIONE! Scambio gli assi\n";

      vector <double> contenitore = vettor_x;
      vettor_x = vettor_y;
      vettor_y = contenitore;
    } else {
      cout << "Non serve scambiare gli assi.\n";
    }
  }


  double sum_x = 0, sum_x2 = 0, sum_y = 0, sum_xy = 0, sum_sigma = 0, sigma_y, delta;
     for(auto &x : vettor_x){
       sum_x += x;
       sum_x2 += pow(x,2);
     }
     for(auto &y: vettor_y){
       sum_y += y;
     }
     for (int i=0; i < N; ++i) {
       sum_xy += vettor_x[i]*vettor_y[i];
     }

     delta = N*sum_x2 - pow(sum_x,2);

     result.a.val = (1/delta)*((sum_x2 *sum_y) - (sum_x*sum_xy));
     result.b.val = (1/delta)*(N*sum_xy - sum_x*sum_y);

     for(int i=0; i < N; ++i){
         sum_sigma += pow((result.a.val+result.b.val*vettor_x[i])-vettor_y[i],2);
     }
     sigma_y = max(sqrt(sum_sigma/(N-2)), erry);
     //sigma_y = 1.0/sqrt(24); //Specifico per questo set di dati
     //Deve essere un altro dei parametri della funzione -> Sarà sistemato nella versione generale!

     if (scambia_assi) { //Riporto alla situazione di prima
       vector <double> contenitore = vettor_x;
       vettor_x = vettor_y;
       vettor_y = contenitore;
     }
     result.a.err = sigma_y*sqrt(sum_x2/delta);
     result.b.err = sigma_y*sqrt(N/delta);

     result.riuscita = true;
     result.corr = coeff_correlazione(vettor_x, vettor_y);
     result.t_stud = T_Student(result.corr, N);
     result.gl = N-2;
     return result;
}

Interpolazione interpola(vector <double> &vettor_x, vector <misura> &vettor_y, double errx) { //Interpolazione generalizzata: sistemare!
  //Interpolazione lineare generale, con errore sulle y variabile
  //L'errore sulle x viene considerato trascurabile.


  Interpolazione result;

  //Controlla se interpolabile: i due vettori devono avere la stessa dimensione!
  int N = vettor_x.size();
  if (N != vettor_y.size()) {
    result.riuscita = false;
    cout << "Errore interpolazione! \n";
    return result;
  }

  //Controllo se devo scambiare gli assi o meno
  bool scambia_assi = false;
  if (errx != -1) { //Se l'utente ha indicato un errore di riferimento per l'asse x
    double tot_err_x = 0, tot_err_y = 0;
    for (int i = 0; i < N; ++i) {
      tot_err_x += errore_percentuale({vettor_x[i], errx});
      tot_err_y += errore_percentuale(vettor_y[i]);
    }
    //cout << tot_err_x << "\t" << tot_err_y << endl;
    if (tot_err_x > tot_err_y) {
      scambia_assi = true;
      cout << "Scambio gli assi\n"; //Avviso l'utente
      //Scambiando gli assi, non mi è più necessario considerare il diverso errore sulle y
      //Perciò uso la formula semplificata
      vector <double> valori = values(vettor_y);
      return interpola(valori, vettor_x, errx, 0);
    }
  }

  double sum_x = 0, sum_x2 = 0, sum_y = 0, sum_xy = 0, sum_sigma = 0, sigma_y, delta;
  for (int i=0; i < N; ++i) {
    if(vettor_y[i].err == 0) continue;
    sum_x += vettor_x[i] / pow(vettor_y[i].err, 2);
    sum_x2 += pow(vettor_x[i], 2) / pow(vettor_y[i].err, 2);
    sum_sigma += 1.0 / pow(vettor_y[i].err, 2);
    sum_y += vettor_y[i].val / pow(vettor_y[i].err, 2);
    sum_xy += (vettor_x[i] * vettor_y[i].val) / pow(vettor_y[i].err, 2);
  }
  //cout << sum_x << "\t" << sum_x2 << "\t" << sum_sigma << endl;

  delta = sum_sigma * sum_x2 - pow(sum_x, 2);

  result.a.val = (1.0/delta) * ((sum_x2 * sum_y) - (sum_x * sum_xy));
  result.b.val = (1.0/delta) * ((sum_sigma * sum_xy) - (sum_x * sum_y));

  result.a.err = sqrt(sum_x2/delta);
  result.b.err = sqrt(sum_sigma/delta);

  result.riuscita = true;

  result.corr = coeff_correlazione(vettor_x, values(vettor_y));
  result.t_stud = T_Student(result.corr, N);
  result.gl = N-2;
  return result;
}

Interpolazione interpola(vector <misura> vettor_x, vector <misura> vettor_y) {
  //Caso generalissimo
  int N = vettor_x.size();
  if (N != vettor_y.size()) {
    cout << "Errore dimensione vettori!\n";
    return Interpolazione();
  }
  double tot_err_x = 0, tot_err_y = 0;
  for (int i = 0; i < N; ++i) {
    tot_err_x += errore_percentuale(vettor_x[i]);
    tot_err_y += errore_percentuale(vettor_y[i]);
  }
  if (tot_err_x <= tot_err_y) {
    cout << "Assi non scambiati\n";
    vector <double> asse_x = values(vettor_x);
    return interpola(asse_x, vettor_y);
  } else {
    cout << "Scambio gli assi!\n";
    vector <double> asse_x = values(vettor_y);
    return interpola(asse_x, vettor_x);
  }
}
