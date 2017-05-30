#include "Fisica.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm> //Per trovare il massimo/minimo di vettori
#include <functional> //Per usare le funzioni come parametri

using namespace std;

/*** Medie ***/
double calcola_media(vector <double> &misure) {
  double media = 0, N = misure.size();

  for (auto &x: misure) {
    media += x;
  }


  return media/N;
}

misura media(vector <double> &misure) {
  misura media = {0,0};
  double N = misure.size();

  media.val = calcola_media(misure);
  //cout << errore_quadratico_medio(misure) << endl;
  media.err = errore_quadratico_medio(misure)/sqrt(N);

  return media;
}

misura MediaPesata(vector <misura> &vettore) {
  misura result;
  double denominatore = 0, numeratore = 0;

  for (auto &x: vettore) {
    numeratore += x.val/pow(x.err, 2);
    denominatore += 1.0/pow(x.err, 2);
  }
  result.val = numeratore/denominatore;
  result.err = sqrt(1.0/denominatore);

  return result;
}

misura MediaPesata(misura a, misura b) {
  misura media_pesata;

  double denominatore = 1.0/pow(a.err, 2) + 1.0/pow(b.err, 2);
  media_pesata.val = (a.val/pow(a.err, 2) + b.val/pow(b.err, 2))/denominatore;
  media_pesata.err = sqrt(1.0/denominatore);

  return media_pesata;
}

/*** Errori ***/
double errore_quadratico_medio(vector <double> misure) {
  double err, sum = 0, media = calcola_media(misure);
  int N = misure.size();

  for (auto &x: misure) {
    sum += pow(x-media, 2);
  }
  //cout << sum << endl;
  //cout << "Res: " << sqrt(sum/(N-1));
  return sqrt(sum/(N-1));
}

double errore_percentuale(misura a) {
  if (a.val == 0) return 0;
  return (a.err/abs(a.val))*100.0;
}

Istogramma GeneraIstogrammaFrequenze(vector <double> &Vettore, int numero_colonne, bool left_closed, bool right_closed) {
  //Questa funzione prende i dati da un Vettore (passato per reference) e li organizza su numero_colonne classi di frequenza, per poi salvarli su outfile. I restanti parametri stabiliscono la chiusura degli intervalli delle classi di frequenza.
  Istogramma result;
  result.N = Vettore.size();

  double minimo, massimo, deltax;
  vector <double> histo_left(numero_colonne+1, 0), histo_right(numero_colonne+1, 0), both(numero_colonne, 0), *histo;

  minimo = *min_element(Vettore.begin(), Vettore.end());
  massimo = *max_element(Vettore.begin(), Vettore.end());

  if (!left_closed && !right_closed) left_closed = true; //Di default gli intervalli sono chiusi a sinistra. Non è possibile renderli aperti da entrambe le parti (non ha senso matematicamente)

  if (left_closed != right_closed) //Equivale ad un XOR: "se solo uno dei due parametri è true"
    deltax = (massimo-minimo)/(numero_colonne-1); //Normalizza il numero di colonne
    //Se gli intervalli sono chiusi a sinistra, ci sarà un intervallo [massimo, massimo+deltax) che contiene solo il massimo, e che è oltre [minimo, massimo). Ma voglio che il numero di classi di frequenza sia quello stabilito dall'utente, ed ecco perché inserisco il -1.
    //Se gli intervalli sono chiusi a destra, l'intervallo aggiuntivo sarà il primo, tra (minimo-deltax, minimo]
  else
    deltax = (massimo - minimo)/(numero_colonne);

  if (left_closed) {
    for (int i = 0; i < Vettore.size(); ++i) {
      histo_left[ floor((Vettore[i]-minimo)/deltax) ] += 1; //Arrotondando per difetto chiudo l'intervallo a sinistra
    }
    histo = &histo_left;	//Imposto quale istogramma stampare
    //massimo += deltax; //Logicamente sarebbe così (l'ultima colonna termina a massimo+deltax), ma ai fini del programma non è necessario specificarlo
  }
  if (right_closed) {
    for (int i = 0; i < Vettore.size(); ++i) {
      histo_right[ ceil((Vettore[i]-minimo)/deltax) ] += 1; //Arrotondando per eccesso chiudo l'intervallo a destra
    }
    minimo -= deltax;	//Tuttavia parto da un minimo più basso
    histo = &histo_right;
  }
  if (left_closed && right_closed) {	//Se voglio gli intervalli chiusi da entrambe le parte, mi basta fare l'unione della chiusura a sinistra e a destra
    for (int j = 0; j < numero_colonne; ++j) {
      both[j] = max(histo_left[j], histo_right[j+1]); //Correggendo ovviamente la sfasatura data dalle due classi di frequenza aggiuntive
    }
    minimo += deltax;
    histo = &both;
  }

  for (int j = 0; j < numero_colonne; ++j) {
    result.x.push_back(minimo+j*deltax+deltax/2.0);
    result.freq.push_back((*histo)[j]);
    //Salvo i risultati ottenuti
  }
  result.deltax = deltax;
  double sigma = errore_quadratico_medio(Vettore);
  result.sigma = sigma;
  result.g = GeneraGaussiana(result.N, result.deltax, result.sigma);
  return result;
}

int ReiezioneDati(vector <double> &dati, int nsigma) {
  //Bisogna aggiungere il controllo su N!

  //vector <double> result = dati;
  double media = calcola_media(dati);
  double sigma = errore_quadratico_medio(dati);
  int reietti = 0;
  for (int i = 0; i < dati.size(); ++i) {
    if ((dati[i] <= media-(nsigma*sigma)) || (dati[i] >= media+(nsigma*sigma))) {
      dati.erase(dati.begin()+i); //Rimuovo quell'elemento
      reietti++;
    }
  }
  //cout << "Cancellati " << reietti <<  " dati\n";
  return reietti;
}

vector <double> GeneraScarti(vector <double> vettore) {
  double media = calcola_media(vettore);
  vector <double> result;
  for (auto &x: vettore) {
    result.push_back(x-media);
  }

  return result;
}

Gaussiana GeneraGaussiana(int N, double deltax, double sigma) {
  double a = (N*deltax)/(sqrt(2.0*M_PI)*sigma);
  double b = -1.0/(2.0*pow(sigma,2));

  Gaussiana result = {a,b};
  return result;
}

double Gauss(double x, vector<double> parametri) {
  double N = parametri[0];
  double deltax = parametri[1];
  double sigma = parametri[2];

  return (1.0)/(sqrt(2.0*M_PI)*sigma) * exp(-pow(x,2)/(2.0*pow(sigma,2)));
}

double Integra(double a, double b, function <double (double, vector <double>)> f, vector<double> params, double samplesperunit) { //Probabilmente dovrei spostarla in un file tipo "matematica.cxx", ma ora non ho voglia
  if (b < a) {
    cout << "Errore, b deve essere maggiore di a!\n";
    return 0;
  }
  if (a == b) {
    return 0;
  }
  double segmenti_tot = (b-a)*samplesperunit;
  double deltax = (b-a)/segmenti_tot;
  double sum = 0;
  sum += f(a, params) + f(b, params);
  for (int i = 1; i < segmenti_tot; ++i) {
    sum += 2*f(a+deltax*i, params);
  }
  return sum*deltax/(2.0);
}

Istogramma GaussIsto(Istogramma isto) {
  Istogramma result;
  vector <double> params = {isto.N, isto.deltax, isto.sigma};
  result.x = isto.x;
  for(int i = 0; i < isto.x.size(); ++i) {
    double a = isto.x[i]-isto.deltax/2.0;
    double b = isto.x[i]+isto.deltax/2.0;
    double newfreq = isto.N * Integra(a, b, Gauss, params, 1000);
    //cout << isto.x[i] << "\t" << newfreq << endl;
    result.freq.push_back(newfreq);
  }
  return result;
}

double ChiPearson(Istogramma sper, Istogramma th) {
  double chi = 0;
  for (int i = 0; i < sper.x.size(); ++i) {
    chi += pow(sper.freq[i] - th.freq[i], 2) / th.freq[i];
  }
  //cout << "Gradi libertà: " << sper.N -3 <<  endl; //Dovrei mettere il numero di classi di frequenza, non N totale
  //Comunque il chi quadro non funziona granché bene, il campione è troppo piccolo!
  //cout << chi << endl;
  return chi;
}
