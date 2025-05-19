#include <iostream>
#include <vector>
#include <algorithm> // per sort
using namespace std;

class Studente{
    private:
        string nome, cognome;
        vector <float> voti; 
    public: 
        Studente(string nome, string cognome, vector <float> voti):
        nome(nome),cognome(cognome),voti(voti){
        }     
        string getNome() const{     
            return nome;
        }
        string getCognome ()const{
            return cognome;
        }
        void print() {
            cout << "nome: " << getNome() << endl;
            cout << "cognome: " << getCognome() << endl;
            for (int i = 0; i < voti.size(); i++) {
                cout << voti[i] << " ";
            }
            cout << endl;
        }
};

// Funzione di confronto per ordinare per cognome (e nome come secondario)
bool confrontaStudenti(const Studente& a, const Studente& b) {
    if (a.getCognome() == b.getCognome())
        return a.getNome() < b.getNome();
    return a.getCognome() < b.getCognome();
}

int main(){
    int nStudenti;
    string nome, cognome;
    
    cout << "Quanti studenti vuoi inserire? ";
    cin >> nStudenti;

    vector<Studente> classe;

    for (int i=0; i<nStudenti; i++) {
        
        vector<float> voti;
        cout << "\nStudente " << i + 1 << endl;
        cout << "Nome: ";
        cin >> nome;
        cout << "Cognome: ";
        cin >> cognome;

        
        for (int j=0; j<=3; j++) {
            float voto;
            cout << "Voto " << j+1 << ": ";
            cin >> voto;
            voti.push_back(voto);
        }

        Studente s(nome, cognome, voti);
        classe.push_back(s);
    }

    // Ordinamento della classe in ordine alfabetico per cognome
    sort(classe.begin(), classe.end(), confrontaStudenti);

    cout << "\n--- Elenco studenti inseriti ---\n";
    for (Studente s : classe) {
        s.print();
    }

    return 0;
}
