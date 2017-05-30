#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <fstream>
//#include <cmath>

#define STANDARDPRECISION 1 //Numero di cifre significative standard
#define SPACE cout << "\n\n\n";

using namespace std;

/*** Gestione dati ***/
struct misura {
  double val, err;
};

struct Interpolazione {
  misura a, b;
  bool riuscita;
  double corr, t_stud, gl;

  void printAll() {
    cout << a.val << " +- " << a.err << endl
         << b.val << " +- " << b.err << endl
         << "Corr: " << corr << "; T-Student: " << t_stud << " a Gradi Lib: " << gl << endl;
  }
};

struct Gaussiana {
  double a, b; //Parametri Gaussiana (y = a*e^(bx^2))
};

struct Istogramma {
  vector <double> x, freq; //x sono i punti medi, freq le frequenze assolute
  double N; //Numero di elementi (se si vuole calcolare la frequenza relativa)
  double deltax;
  double sigma;
  Gaussiana g;
};

template <typename T> //Così funziona per tutti i tipi di variabili! Yeah!
struct DataTable { //Work in progress
  vector <vector <T>> data;
  int x = 0, y = 0; //x = numero righe, y = numero colonne

  vector <T> nullo(int length) { //Restituisce un vettore nullo di lunghezza length
    vector <T> res(length, 0);

    return res;
  }
  bool expand(int to_x, int to_y) {
    //cout << "Hi!\n";
    if (to_x > this->x) { //Se devo incrementare le righe
      for (int i = this->x; i < to_x; ++i) {
        data.push_back(this->nullo(y));
      }
      this->x = to_x;
      //cout << "Riciao\n";
    }
    if (to_y > this->y) {
      for (int i = 0; i < this->x; ++i) {
        data[i] = allarga(data[i], to_y);
      }
    }
    return true;
  }
  vector <T> allarga(vector <T> vettore, int to_x) { //Aggiunge 0 alla fine di un vettore, fino a che non raggiunge la lunghezza desiderata
    int N = vettore.size();
    for (int i = N; i < to_x; ++i) {
      vettore.push_back(this->nullo(1)[0]);
    }
    return vettore;
  }
  int insertRow(vector <T> riga, int pos = -1) { //Possibilità di scegliere dove posizionarla (e se sovrascrivere o meno)
    if ((pos >= this->x) || (pos == -1)) {
      pos = this->x;
    }

    int l = riga.size();
    if (l > this->y) { //Se la riga che sto inserendo è più larga della tabella
      this->expand(0, l); //Allargo la tabella
      this->y = l;
    } else if (l < this->y) { //Altrimenti allargo la riga
      riga = allarga(riga, this->y);
    }
    this->data.insert(this->data.begin()+pos, riga);
    ++this->x;
    return this->x;
  }
  vector <T> getRow(int i) {
    if (i < x) {
      return data[i];
    } else {
      cout << "La riga richiesta non è presente!\n";
      return this->nullo(1);
    }
  }
  vector <T> getColumn(int j) {
    vector <T> res;
    if (j < this->y) {
      for (int i = 0; i < this->x; ++i) {
        res.push_back(data[i][j]);
      }
    } else {
      cout << "Colonna non esistente! \n";
      res = this->nullo(1);
    }
    return res;
  }

  int insertColumn(vector <T> colonna, int pos = -1) {
    //Inserisce una colonna ad una determinata posizione (di default alla fine)
    if ((pos >= this->y) || (pos == -1))
      pos = this->y; //Se la posizione è dopo la fine la imposto alla fine

    int length = colonna.size(); //Questo pezzo di codice potrei metterlo in una funzione a parte!
    if (length > this->x) { //Se devo aggiungere delle righe lo faccio
      this->expand(length, 0);
      this->x = length;
    } else if (length < this->x) {
      colonna = allarga(colonna, this->x);
    }

    for (int i = 0; i < this->x; ++i) {
      data[i].insert(data[i].begin()+pos, colonna[i]);
    }
    ++this->y; //Ho una colonna in più

    return this->y;
  }

  T getCell(int posx, int posy) {
    if ((posx < this->x) && (posy < this->y)) {
      return this->data[posx][posy];
    } else {
      cout << "Cella non esistente!\n";
    }
  }

  int removeColumn(int pos) {
    if (pos < this->y) {
      for (int i = 0; i < this->x; ++i) {
        this->data[i].erase(this->data[i].begin()+pos);
      }
      --this->y;
    }
    return this->y;
  }

  int removeRow(int pos) {
    if (pos < this->x) {
      this->data.erase(this->data.begin()+pos);
      --this->x;
    }
    return this->x;
  }

  vector <T> to_vector() { //Si può rendere più efficiente
    vector <T> res;
    for (int i = 0; i < this->x; ++i) {
      for (auto &x: this->getRow(i)) {
        res.push_back(x);
      }
    }
    return res;
  }
  void printDim() {
    cout << "Righe: " << this->x << "\t Colonne: " << this->y << endl;
  }
  void printAll() {
    for (auto &riga: this->data) {
      for (auto &cella: riga) {
        cout << cella << "\t";
      }
      cout << "\n";
    }
  }

  bool printToFile(string filename) {
    ofstream file(filename);
    if (!file)
      return false;
    for (auto &riga: this->data) {
      for (auto &cella: riga) {
        file << cella << "\t";
      }
      file << "\n";
    }
    return true;
  }
};

template <typename T>
DataTable<T> trasponi(DataTable<T> tabella) {
  DataTable<T> result;
  for(int i = 0; i < tabella.y; ++i) {
    result.insertRow(tabella.getColumn(i));
  }
  return result;
} //Nota per il me stesso del futuro. Le funzioni template vanno nel file di header,
//Ma le funzioni di overload (o le specializzazioni di template) vanno solo prototipate nell'header,
//e definite in altro file

template <typename T>
bool uguaglianza(vector<T> a, vector<T> b, vector <int> ref) {
  for (auto &x: ref) {
    if (a[x] != b[x]) {
      return false;
    }
  }
  return true;
}

template <typename T>
vector <DataTable<T>> collapse(DataTable<T> tabella, vector <int> ref) {
  //ref contiene gli id delle colonne in cui cercare i termini in comune
  //la funzione deve ritornare un vettore di tabelle in cui ciascuna tabella ha i campi indicati da ref
  //completamente uguali (in pratica spezza una tabella più grande)
  int N = ref.size();
  //bool uguaglianza = false;
  vector<T> actual, saved = tabella.nullo(tabella.y);
  //cout << "Shish\n";

  vector <DataTable<T>> result;
  int num = -1;

  for (int i = 0; i < tabella.x; ++i) {
    //cout << i << endl;
    actual = tabella.getRow(i);
    if (!uguaglianza(actual, saved, ref)) {
      //cout << "STOP\n\n";
      ++num;
      result.push_back(DataTable<T>());
      saved = actual;
    }
    result[num].insertRow(actual);
    //result[num].printAll();
  }

  return result;
}

//Nota2: forse dovrei fare un altro file di header, solo con le strutture/funzioni da prototipare (si può?)

//Overload di cout (per poter stampare le variabili di tipo misura con cout)
ostream& operator<<(ostream& os, const misura& mis);

//Specializzazione di DataTable per poter gestire variabili di tipo misura
template <>
vector <misura> DataTable<misura>::nullo(int length);

/*** Acquisizione dati - Dati.cxx - ***/
vector <string> parseRow(string riga, string delimiter = ",");
//Data una riga di un file csv, la divide in sottostringhe basandosi su una sequenza di caratteri di limite (nel caso dei csv, la virgola)
//Restituisce un vettore contenente le sottostringhe così trovate
//es "ciao,a,tutti" diventa ["ciao", "a", "tutti"]

/*** Manipolazione dati - ManipolaDati.cxx - ***/
vector <misura> to_misura(vector <double> vettore, double errore);
//A partire da un vettore di double crea un vettore di misure che hanno tutte lo stesso errore
vector <misura> to_misura(vector <double> vettore, vector<double> errori);
vector <double> values(vector <misura> vettore, bool err = false);
//Ritorna solo i valori, o se il flag err è true, solo gli errori di un vettore di misure
misura scala(misura a, int order, double moltiplicatore = 1.0); //Scala una misura e il suo errore
misura differenza(misura a, misura b);
misura dividi(misura a, misura b);
misura pow(misura a, int exp);
misura inverso(misura a);
string print(vector <double> vec); //Stampa un vettore (debug)
vector <double> VecToDouble (vector <string> riga);
vector <double> differenza(vector <double> a, vector <double> b, bool solopos = false);
vector <double> togliMenoUno(vector <double> a);
vector <misura> togliMenoUno(vector <misura> a);

/*** Analisi statistica - Statistica.cxx -***/
/*** Medie ***/
double calcola_media(vector <double> &misure); //Media semplice (se tutte le misure hanno lo stesso errore)
misura media(vector <double> &misure);
misura MediaPesata(vector <misura> &vettore); //Media pesata (misure con diverso errore)
misura MediaPesata(misura a, misura b);

/*** Errori ***/
double errore_quadratico_medio(vector <double> misure);
double errore_percentuale(misura a);
vector <double> GeneraScarti(vector <double> vettore);

/*** ISTOGRAMMI ***/
Istogramma GeneraIstogrammaFrequenze(vector <double> &Vettore, int numero_colonne = 10, bool left_closed = true, bool right_closed = true);

/*** GAUSSIANA ***/
int ReiezioneDati(vector <double> &dati, int nsigma = 3); //Reiezione dati
Gaussiana GeneraGaussiana(int N, double deltax, double sigma);
double Gauss(double x, vector<double> parametri);
double Integra(double a, double b, function <double (double, vector <double>)> f, vector<double> params, double samplesperunit = 1000);
Istogramma GaussIsto(Istogramma isto);
double ChiPearson(Istogramma sper, Istogramma th);

/*** Fit - Fit.cxx - ***/
Interpolazione interpola(vector <double> &vettor_x, vector <double> &vettor_y, double errx = -1, double erry = -1); //Interpolazione lineare di base
Interpolazione interpola(vector <double> &vettor_x, vector <misura> &vettor_y, double errx = -1); //Interpolazione lineare generalizzata
Interpolazione interpola(vector <misura> vettor_x, vector <misura> vettor_y); //Caso generalissimo

/*** Stima bontà - Stima.cxx - ***/
double compatibile(misura a, misura b); //Calcola la compatibilità tra due misure
double coeff_correlazione(vector <double> x, vector <double> y);
double T_Student(double x, int N);
double chi_quadro(vector <double> x, vector <double> y, misura a, misura b);
//Inserire massima verosimiglianza

/*** Output dati - Output.cxx - ***/
string approssima(misura a, int precision = STANDARDPRECISION, int order = 0, bool exp = false);
string approssima(double a, int precision = STANDARDPRECISION);
//Restituisce una stringa con la misura approssimata al giusto numero di cifre significative
//precision regola il numero di cifre significative, order indica l'ordine di grandezza di default, exp attiva la modalità notazione scientifica
