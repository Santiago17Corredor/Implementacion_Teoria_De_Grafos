#include <iostream>
#include <vector>
#include <cctype>

using namespace std;



// Pide la cantiad de nodos y como se nombrarán con letras del abecedario por ahora el limite maximo es 26....
int pedirCantidadNodos() {
    int cantidad;

    do {
        cout << "Ingrese la cantidad de nodos (entre 1 y 26): ";
        cin >> cantidad;

        if (cantidad < 1 || cantidad > 26) {
            cout << "La cantidad debe estar entre 1 y 26." << endl;
        }
    } while (cantidad < 1 || cantidad > 26);

    return cantidad;
}



/*---------------------verificaciones de entrada de datos de cada nodo -------------------*/

// Convierte una letra minuscula en mayuscula
char convertirAMayuscula(char letra) {
    return static_cast<char>(toupper(letra));
}

// Comprueba que el caracter sea una letra entre A y Z
bool esLetraValida(char letra) {
    return letra >= 'A' && letra <= 'Z';
}

// Comprueba que la letra no haya sido usada en otro nodo
bool letraRepetida(const vector<char>& nombres, char letra) {
    for (int i = 0; i < nombres.size(); i++) {
        if (nombres[i] == letra) {
            return true;
        }
    }

    return false;
}
/*----------------------------------------------------------------------------------------*/



// Pide una letra diferente para identificar cada nodo
void pedirNombresNodos(vector<char>& nombres, int cantidadNodos) {
    char letra;

    cout << "\nIngrese una letra del abecedario para cada nodo." << endl;

    for (int i = 0; i < cantidadNodos; i++) {
        do {
            cout << "Nombre del nodo " << i + 1 << ": ";
            cin >> letra;

            letra = convertirAMayuscula(letra);

            if (!esLetraValida(letra)) {
                cout << "Debe ingresar una letra entre A y Z." << endl;
            } else if (letraRepetida(nombres, letra)) {
                cout << "Esa letra ya pertenece a otro nodo." << endl;
            }
        } while (!esLetraValida(letra) || letraRepetida(nombres, letra));

        nombres.push_back(letra);
    }
}

//elegir si el grafo es dirigido o no dirigido
int pedirTipoGrafo() {
    int opcion;

    cout << "\nSeleccione el tipo de grafo:" << endl;
    cout << "1. Grafo no dirigido" << endl;
    cout << "2. Grafo dirigido" << endl;

    do {
        cout << "Opcion: ";
        cin >> opcion;

        if (opcion < 1 || opcion > 2) {
            cout << "Opcion invalida. Intente de nuevo." << endl;
        }
    } while (opcion < 1 || opcion > 2);

    return opcion;
}

//elegir si el grafo es etiquetado o no etiquetado.
int pedirTipoEtiquetado() {
    int opcion;

    cout << "\nSeleccione si el grafo sera etiquetado:" << endl;
    cout << "1. Grafo etiquetado" << endl;
    cout << "2. Grafo no etiquetado" << endl;

    do {
        cout << "Opcion: ";
        cin >> opcion;

        if (opcion < 1 || opcion > 2) {
            cout << "Opcion invalida. Intente de nuevo." << endl;
        }
    } while (opcion < 1 || opcion > 2);

    return opcion;
}

//muestra los datos principales del grafo.
void mostrarResumen(int cantidadNodos, int tipoGrafo, int tipoEtiquetado) {
    cout << "\nCantidad de nodos: " << cantidadNodos << endl;

    if (tipoGrafo == 1) {
        cout << "Tipo de grafo: No dirigido" << endl;
    } else {
        cout << "Tipo de grafo: Dirigido" << endl;
    }

    if (tipoEtiquetado == 1) {
        cout << "Etiquetado: Si" << endl;
    } else {
        cout << "Etiquetado: No" << endl;
    }
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
