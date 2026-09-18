#include <iostream>

using namespace std;

int main() {
    int cantidadNodos;
    int opcion;

    cout << "=== Creacion de un grafo ===" << endl;

    do {
        cout << "Ingrese la cantidad de nodos: ";
        cin >> cantidadNodos;

        if (cantidadNodos <= 0) {
            cout << "La cantidad de nodos debe ser mayor que cero." << endl;
        }
    } while (cantidadNodos <= 0);

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

    cout << "\nCantidad de nodos: " << cantidadNodos << endl;

    if (opcion == 1) {
        cout << "Tipo de grafo: No dirigido" << endl;
    } else {
        cout << "Tipo de grafo: Dirigido" << endl;
    }



    return 0;
}
