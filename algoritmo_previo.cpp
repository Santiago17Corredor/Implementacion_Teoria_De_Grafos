#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

enum class TipoGrafo {
    NoDirigido = 1,
    Dirigido = 2
};

int leerEnteroEnRango(const string& mensaje, int minimo, int maximo) {
    string linea;
    int valor;
    char sobrante;

    while (true) {
        cout << mensaje;
        getline(cin, linea);
        stringstream entrada(linea);

        if (entrada >> valor && !(entrada >> sobrante) &&
            valor >= minimo && valor <= maximo) {
            return valor;
        }

        cout << "Ingrese un numero entre " << minimo << " y " << maximo << ".\n";
    }
}



// Pide la cantiad de nodos y como se nombrarán con letras del abecedario por ahora el limite maximo es 26....
int pedirCantidadNodos() {
    return leerEnteroEnRango("Ingrese la cantidad de nodos (1 a 26): ", 1, 26);
}



/*---------------------verificaciones de entrada de datos de cada nodo -------------------*/

// Convierte una letra minuscula en mayuscula
char convertirAMayuscula(char letra) {
    return static_cast<char>(toupper(static_cast<unsigned char>(letra)));
}

// Comprueba que el caracter sea una letra entre A y Z
bool esLetraValida(char letra) {
    return letra >= 'A' && letra <= 'Z';
}

// Comprueba que la letra no haya sido usada en otro nodo
bool letraRepetida(const vector<char>& nombres, char letra) {
    for (char nombre : nombres) {
        if (nombre == letra) {
            return true;
        }
    }

    return false;
}
/*----------------------------------------------------------------------------------------*/

char pedirLetra(const string& mensaje) {
    string linea;
    string valor;
    string sobrante;

    while (true) {
        cout << mensaje;
        getline(cin, linea);
        stringstream entrada(linea);

        if (entrada >> valor && !(entrada >> sobrante) && valor.size() == 1) {
            char letra = convertirAMayuscula(valor[0]);

            if (esLetraValida(letra)) {
                return letra;
            }
        }

        cout << "Ingrese una sola letra entre A y Z.\n";
    }
}



// Pide una letra diferente para identificar cada nodo
void pedirNombresNodos(vector<char>& nombres, int cantidadNodos) {
    cout << "\nAsigne una letra diferente a cada nodo.\n";

    for (int i = 0; i < cantidadNodos; i++) {
        while (true) {
            char letra = pedirLetra("Nombre del nodo " + to_string(i + 1) + ": ");

            if (!letraRepetida(nombres, letra)) {
                nombres.push_back(letra);
                break;
            }

            cout << "La letra ya pertenece a otro nodo.\n";
        }
    }
}

//elegir si el grafo es dirigido o no dirigido
TipoGrafo pedirTipoGrafo() {
    cout << "\nTipo de grafo:\n";
    cout << "1. No dirigido\n";
    cout << "2. Dirigido\n";

    int opcion = leerEnteroEnRango("Opcion: ", 1, 2);
    return static_cast<TipoGrafo>(opcion);
}

int pedirValorAdyacencia(char origen, char destino, TipoGrafo tipo) {
    string conector = tipo == TipoGrafo::Dirigido ? " -> " : " -- ";
    string mensaje = "Existe la arista " + string(1, origen) + conector +
                     string(1, destino) + "? (0/1): ";

    return leerEnteroEnRango(mensaje, 0, 1);
}

void ingresarMatriz(vector<vector<int>>& matriz,
                    const vector<char>& nombres,
                    TipoGrafo tipo) {
    int cantidad = static_cast<int>(matriz.size());
    cout << "\nIngrese las conexiones del grafo.\n";

    if (tipo == TipoGrafo::Dirigido) {
        for (int i = 0; i < cantidad; i++) {
            for (int j = 0; j < cantidad; j++) {
                if (i != j) {
                    matriz[i][j] = pedirValorAdyacencia(nombres[i], nombres[j], tipo);
                }
            }
        }
        return;
    }

    for (int i = 0; i < cantidad; i++) {
        for (int j = i + 1; j < cantidad; j++) {
            int valor = pedirValorAdyacencia(nombres[i], nombres[j], tipo);
            matriz[i][j] = valor;
            matriz[j][i] = valor;
        }
    }
}

bool validarMatriz(const vector<vector<int>>& matriz, TipoGrafo tipo) {
    int cantidad = static_cast<int>(matriz.size());

    if (cantidad == 0) {
        return false;
    }

    for (int i = 0; i < cantidad; i++) {
        if (static_cast<int>(matriz[i].size()) != cantidad || matriz[i][i] != 0) {
            return false;
        }

        for (int j = 0; j < cantidad; j++) {
            if (matriz[i][j] != 0 && matriz[i][j] != 1) {
                return false;
            }

            if (tipo == TipoGrafo::NoDirigido && matriz[i][j] != matriz[j][i]) {
                return false;
            }
        }
    }

    return true;
}

// Muestra los datos principales del grafo.
void mostrarResumen(const vector<char>& nombres, TipoGrafo tipo) {
    cout << "\nResumen del grafo\n";
    cout << "Cantidad de nodos: " << nombres.size() << '\n';

    cout << "Tipo: "
         << (tipo == TipoGrafo::Dirigido ? "Dirigido" : "No dirigido")
         << '\n';

}

//i6mprime la matriz junto con los nombres de los nodos.
void imprimirMatriz(const vector<vector<int>>& matriz, const vector<char>& nombres) {
    cout << "\nMatriz de adyacencia:" << endl;
    cout << "    ";

    for (int i = 0; i < nombres.size(); i++) {
        cout << nombres[i] << " ";
    }

    
    cout << endl;

    for (int i = 0; i < matriz.size(); i++) {
        cout << nombres[i] << " | ";

        for (int j = 0; j < matriz[i].size(); j++) {
            cout << matriz[i][j] << " ";
        }

        cout << endl;
    }
}

void mostrarRepresentacionMatematica(const vector<vector<int>>& matriz,
                                     const vector<char>& nombres,
                                     TipoGrafo tipo) {
    cout << "\nV = {";

    for (int i = 0; i < static_cast<int>(nombres.size()); i++) {
        if (i > 0) {
            cout << ", ";
        }
        cout << nombres[i];
    }

    cout << "}\n";
    cout << (tipo == TipoGrafo::Dirigido ? "A = {" : "E = {");
    bool primera = true;

    for (int i = 0; i < static_cast<int>(matriz.size()); i++) {
        int inicio = tipo == TipoGrafo::Dirigido ? 0 : i + 1;

        for (int j = inicio; j < static_cast<int>(matriz.size()); j++) {
            if (matriz[i][j] == 0) {
                continue;
            }

            if (!primera) {
                cout << ", ";
            }

            if (tipo == TipoGrafo::Dirigido) {
                cout << '<' << nombres[i] << ',' << nombres[j] << '>';
            } else {
                cout << '{' << nombres[i] << ',' << nombres[j] << '}';
            }

            primera = false;
        }
    }

    cout << "}\n";
}

int buscarIndiceNodo(const vector<char>& nombres, char nombre) {
    for (int i = 0; i < static_cast<int>(nombres.size()); i++) {
        if (nombres[i] == nombre) {
            return i;
        }
    }

    return -1;
}

int main() {
    cout << "=== Creacion de un grafo ===" << endl;

    // Paso 1: pedir los datos del grafo.
    int cantidadNodos = pedirCantidadNodos();

    vector<char> nombresNodos;
    pedirNombresNodos(nombresNodos, cantidadNodos);

    int tipoGrafo = pedirTipoGrafo();
    int tipoEtiquetado = pedirTipoEtiquetado();

    // Paso 2: crear la matriz llena de ceros.
    vector<vector<int>> matrizAdyacencia(
        cantidadNodos,
        vector<int>(cantidadNodos, 0)
    );

    // Paso 3: mostrar el resultado.
    mostrarResumen(cantidadNodos, tipoGrafo, tipoEtiquetado);
    imprimirMatriz(matrizAdyacencia, nombresNodos);

    return 0;
}
