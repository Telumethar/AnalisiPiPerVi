#include "Fisica.h"

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <sstream>

using namespace std;

string approssima(misura a, int precision, int order, bool exp) { //Dovrei anche inserire "l'ordine" dell'esponente da tenere come possibile funzione...
  //Nota per il futuro: inserisci condizione a.val != 0!
  int moltiplicatore = floor(log10(abs(a.err))); //ordine di grandezza dell'errore, approssimato per difetto
  //Se ora moltiplicassi a.err per 10^(-moltiplicatore), lo ridurrei in notazione esponenziale:
  //una cifra intera, il resto decimali
  //Se arrotondassi adesso otterrei un risultato con UNA sola cifra significativa
  //Ma magari ne voglio di più! Il numero di cifre significative è dato da precision
  //Una c'è già, quindi per "scoprire" ulteriori cifre significative, mettendole nella parte intera, dove sono al sicuro da round,
  //moltiplico ancora per 10^(precision-1)
  //Unendo le operazioni:
  int scalar = -moltiplicatore +(precision-1);

  double rounded = round(a.err*pow(10, scalar)); //Moltiplico ed eseguo l'arrotondamento

  int ordine = floor(log10(abs(a.val))); //Intanto prelevo anche l'ordine di grandezza di a
  double rounded_a = round(a.val*pow(10, scalar)); //E lo arrotondo allo stesso modo
  //In questo modo i due numeri avranno lo stesso numero di decimali

  //Ora devo stampare i numeri arrotondati in notazione esponenziale
  //Ogni numero avrà lo stesso numero di decimali, che è dato da: scalar+ordine
  //Se moltiplico per 10^(-scalar-ordine) porto il valore di a in notazione esponenziale
  //(Essenzialmente inverto le operazioni fatte finora, però l'arrotondamento rimane)
  stringstream ss;

  if(exp) {
    if(ordine > 4 || ordine < -4) {
      exp = true;
    } else {
      exp = false;
    }
  }
  if(exp) {
    ss << fixed << setprecision(scalar+ordine) << "(" << rounded_a*pow(10, -scalar-ordine) <<
            " $\\pm$ " << rounded*pow(10, -scalar-ordine) <<
            ") $\\cdot 10^{" << defaultfloat <<  (+ordine) << "}$";
  } else {
    ss << fixed << setprecision(max(-moltiplicatore+precision-1+order, 0)) << "" << rounded_a*pow(10, -scalar-order) <<
          " $\\pm$ " << rounded*pow(10, -scalar-order) << "";
  }
  //cout << ss.str();

  return ss.str();
}
string approssima(double a, int precision) {
  if (a != 0) {
    int ordine = floor(log10(abs(a)));
    stringstream ss;
    double rounded = round(a*pow(10, -ordine+precision-1));
    ss << fixed << setprecision(max(-ordine+precision-1, 0)) << rounded*pow(10, ordine-precision+1);
    //cout << "COMPAT : " << a << " = " << ss.str() << endl;
    return ss.str();
  } else {
    return "0";
  }
}
