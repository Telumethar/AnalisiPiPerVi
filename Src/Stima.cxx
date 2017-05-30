#include "Fisica.h"

#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

double compatibile(misura a, misura b) {
  double compatib = (abs(a.val - b.val))/(sqrt( pow(a.err, 2) + pow(b.err, 2) ));

  return compatib;
}

double coeff_correlazione(vector <double> x, vector <double> y) { //Ritorna già il valore in T-Student
  double sum_x=0, sum_y=0, sum_xy=0, media_x, media_y, sigma_x = 0, sigma_y = 0;
  for (int i = 0; i < x.size(); i++) {
      sum_x += x[i];
      sum_y += y[i];
      sum_xy += x[i]*y[i];
  }
  int N = x.size();
  media_x=sum_x/N;
  media_y=sum_y/N;
  double covarianza = (1.0/N)*((sum_xy) - (1.0/N)*(sum_x*sum_y));
  double sqm_x = 0, sqm_y=0;
  for(int i=0; i< N; i++){
    sqm_x += pow(x[i]-media_x,2);
    sqm_y += pow(y[i]-media_y,2);
  }
  sigma_x = sqrt(sqm_x/N);
  sigma_y = sqrt(sqm_y/N);

  double risultato = covarianza/(sigma_y*sigma_x);
  //cout << "Correlazione: " << risultato << "\t T-Student: " << t_student << "\n";
  return risultato;
}

double T_Student(double x, int N) {
  return x*sqrt((N-2) / (1 - pow(x, 2)));
}

double chi_quadro(vector <double> x, vector <double> y, misura a, misura b) {
  //Modificare in vector <misura> quando ci saranno tutte le misure giuste
  misura chi;
  double chi_quadro = 0;
  double xerr = (0.1*9.807*4)/(20.0*sqrt(24)); //0.04
  double yerr = 1.0/sqrt(24);
  int N = x.size();
  for (int i = 0; i < N; ++i) {
    chi.val = y[i] - (a.val + b.val*x[i]);
    //chi.err = pow(b.val*xerr, 2) + pow(yerr, 2) + pow(a.err, 2) + pow(x[i]*b.err, 2);
    //chi.err = sqrt(chi.err);
    chi.err = yerr;

    chi_quadro += pow(chi.val/chi.err, 2);
  }
  int gradi_lib = N-2;
  misura speranza;
  speranza.val = gradi_lib;
  speranza.err = sqrt(2*gradi_lib);
  misura chiq;
  chiq.val = chi_quadro;
  chiq.err = 0; //Approssimo!
  cout << "CHI QUADRO: " << chi_quadro << " a " << gradi_lib << " gradi \n"
       << "Compatibilità: " << compatibile(chiq, speranza) << "\n";
}
