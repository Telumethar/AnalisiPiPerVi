#include "Fisica.h"

#include <vector>
#include <cmath>
#include <iostream> //Debug

using namespace std;

//Cout misura
ostream& operator<<(ostream& os, const misura& mis) {
  return os << "{" << mis.val << ";" << mis.err << "}";
}

template <>
vector <misura> DataTable<misura>::nullo(int length) { //Restituisce un vettore nullo di lunghezza length
  vector <misura> res(length, {0,0});
  return res;
}

vector <misura> to_misura(vector <double> vettore, double errore) {
  vector <misura> result;
  for (auto &x: vettore) {
    result.push_back({x, errore});
  }
  return result;
}
vector <misura> to_misura(vector <double> vettore, vector<double> errori) {
  vector <misura> result;
  if (vettore.size() != errori.size()) {
    cout << "Errore dimensione vettori!";
    return {{0,0}};
  }
  for (int i = 0; i < vettore.size(); ++i) {
    result.push_back({vettore[i], errori[i]});
  }
  return result;
}

vector <double> values(vector <misura> vettore, bool err) {
  vector <double> result;
  for (auto &x: vettore) {
    if(!err) {
      result.push_back(x.val);
    } else {
      result.push_back(x.err);
    }
  }
  return result;
}

misura scala(misura a, int order, double moltiplicatore) {
  double scalar = moltiplicatore * pow(10, order);

  return {a.val*scalar, a.err*scalar};
}

misura differenza(misura a, misura b) {
  misura result;
  result.val = a.val - b.val;
  result.err = sqrt(pow(a.err, 2) + pow(b.err, 2));

  return result;
}

misura dividi(misura a, misura b) {
  misura result;
  result.val = a.val / b.val;
  result.err = sqrt(pow(a.err / b.val, 2) + pow(a.val * b.err / pow(b.val, 2), 2));

  return result;
}

misura pow(misura a, int exp) {
  misura result;

  result.val = pow(a.val, exp);
  result.err = sqrt(pow(exp * pow(a.val, exp-1), 2) * pow(a.err, 2));

  return result;
}
misura inverso(misura a) {
  misura inverso;
  inverso.val = 1.0/a.val;
  inverso.err = sqrt(pow(1.0/a.val, 4) * pow(a.err, 2));

  return inverso;
}
string print(vector <double> vec) {
  string res = "{";
  int N = vec.size();
  for (int i=0; i < N; ++i) {
    res += to_string(vec[i]);
    if (i < N-1)
      res += "; ";
  }
  return res + "}\n";
}

vector <double> VecToDouble (vector <string> riga) {
  vector <double> res;
  for (auto &x: riga) {
    if (x != "/")
      res.push_back(stod(x));
    else
      res.push_back(-1);
  }
  return res;
}

vector <double> differenza(vector <double> a, vector <double> b, bool solopos) {
  vector <double> res;
  int N = a.size();
  if (N == b.size()) {
    for (int i = 0; i < N; ++i) {
      if ((solopos) && ((a[i] == -1) || (b[i] == -1))) {
        res.push_back(-1);
        continue;
      }
      res.push_back(a[i] - b[i]);
    }
  } else {
    cout << "Vettori di lunghezza diversa!\n";
    return {0};
  }
  return res;
}

vector <double> togliMenoUno(vector <double> a) {
//Rimuove tutti i -1 (indicatori di misura assente) da un vettore
  vector <double> res;
  for (auto &x: a) {
    if (x != -1) {
      res.push_back(x);
    }
  }
  return res;
}

vector <misura> togliMenoUno(vector <misura> a) {
  vector <misura> res;
  for (auto &x: a) {
    if (x.val != -1) {
      res.push_back(x);
    }
  }
  return res;
}
