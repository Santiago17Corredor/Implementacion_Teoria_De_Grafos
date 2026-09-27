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

char convertirAMayuscula(char letra) {
    return static_cast<char>(toupper(static_cast<unsigned char>(letra)));
}

bool esLetraValida(char letra) {
    return letra >= 'A' && letra <= 'Z';
}

bool letraRepetida(const vector<char>& nombres, char letra) {
    for (char nombre : nombres) {
        if (nombre == letra) {
            return true;
        }
    }

    return false;
}

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

int pedirCantidadNodos() {
    return leerEnteroEnRango("Ingrese la cantidad de nodos (1 a 26): ", 1, 26);
}

void pedirNombresNodos(vector<char>& nombres, int cantidad) {
    cout << "\nAsigne una letra diferente a cada nodo.\n";

    for (int i = 0; i < cantidad; i++) {
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

void mostrarResumen(const vector<char>& nombres, TipoGrafo tipo) {
    cout << "\nResumen del grafo\n";
    cout << "Cantidad de nodos: " << nombres.size() << '\n';
    cout << "Tipo: "
         << (tipo == TipoGrafo::Dirigido ? "Dirigido" : "No dirigido")
         << '\n';
}

void imprimirMatriz(const vector<vector<int>>& matriz,
                    const vector<char>& nombres) {
    cout << "\nMatriz de adyacencia:\n    ";

    for (char nombre : nombres) {
        cout << setw(3) << nombre;
    }

    cout << '\n';

    for (int i = 0; i < static_cast<int>(matriz.size()); i++) {
        cout << setw(3) << nombres[i] << ' ';

        for (int valor : matriz[i]) {
            cout << setw(3) << valor;
        }

        cout << '\n';
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

int pedirNodoExistente(const vector<char>& nombres, const string& mensaje) {
    while (true) {
        char nombre = pedirLetra(mensaje);
        int indice = buscarIndiceNodo(nombres, nombre);

        if (indice != -1) {
            return indice;
        }

        cout << "El nodo no existe en el grafo.\n";
    }
}

void imprimirNodosEncontrados(const vector<char>& nombres,
                              const vector<int>& indices) {
    if (indices.empty()) {
        cout << "Ninguno";
        return;
    }

    for (int i = 0; i < static_cast<int>(indices.size()); i++) {
        if (i > 0) {
            cout << ", ";
        }
        cout << nombres[indices[i]];
    }
}

void mostrarAdyacentes(int indice,
                       const vector<vector<int>>& matriz,
                       const vector<char>& nombres,
                       TipoGrafo tipo) {
    vector<int> salientes;
    vector<int> entrantes;

    for (int j = 0; j < static_cast<int>(matriz.size()); j++) {
        if (matriz[indice][j] == 1) {
            salientes.push_back(j);
        }

        if (tipo == TipoGrafo::Dirigido && matriz[j][indice] == 1) {
            entrantes.push_back(j);
        }
    }

    if (tipo == TipoGrafo::NoDirigido) {
        cout << "Adyacentes de " << nombres[indice] << ": ";
        imprimirNodosEncontrados(nombres, salientes);
        cout << '\n';
        return;
    }

    cout << "Sucesores de " << nombres[indice] << ": ";
    imprimirNodosEncontrados(nombres, salientes);
    cout << "\nPredecesores de " << nombres[indice] << ": ";
    imprimirNodosEncontrados(nombres, entrantes);
    cout << '\n';
}

vector<int> pedirSecuenciaNodos(const vector<char>& nombres) {
    int longitud = leerEnteroEnRango(
        "Cantidad de nodos de la secuencia (1 a 100): ", 1, 100);
    vector<int> secuencia;

    for (int i = 0; i < longitud; i++) {
        secuencia.push_back(pedirNodoExistente(
            nombres, "Nodo " + to_string(i + 1) + ": "));
    }

    return secuencia;
}

bool esCamino(const vector<int>& secuencia,
              const vector<vector<int>>& matriz) {
    if (secuencia.empty()) {
        return false;
    }

    for (int i = 0; i + 1 < static_cast<int>(secuencia.size()); i++) {
        if (matriz[secuencia[i]][secuencia[i + 1]] == 0) {
            return false;
        }
    }

    return true;
}

bool esCaminoSimple(const vector<int>& secuencia) {
    set<int> visitados;

    for (int i = 0; i < static_cast<int>(secuencia.size()); i++) {
        bool ultimoIgualAlPrimero =
            i == static_cast<int>(secuencia.size()) - 1 &&
            secuencia.size() > 1 && secuencia[i] == secuencia[0];

        if (ultimoIgualAlPrimero) {
            continue;
        }

        if (!visitados.insert(secuencia[i]).second) {
            return false;
        }
    }

    return true;
}

bool tieneAristasDiferentes(const vector<int>& secuencia) {
    set<pair<int, int>> usadas;

    for (int i = 0; i + 1 < static_cast<int>(secuencia.size()); i++) {
        int menor = min(secuencia[i], secuencia[i + 1]);
        int mayor = max(secuencia[i], secuencia[i + 1]);

        if (!usadas.insert({menor, mayor}).second) {
            return false;
        }
    }

    return true;
}

bool esCiclo(const vector<int>& secuencia,
             const vector<vector<int>>& matriz,
             TipoGrafo tipo) {
    if (secuencia.size() < 3 || secuencia.front() != secuencia.back() ||
        !esCamino(secuencia, matriz)) {
        return false;
    }

    return tipo == TipoGrafo::Dirigido || tieneAristasDiferentes(secuencia);
}

void analizarSecuencia(const vector<int>& secuencia,
                       const vector<vector<int>>& matriz,
                       TipoGrafo tipo) {
    if (!esCamino(secuencia, matriz)) {
        cout << "La secuencia no es un camino.\n";
        return;
    }

    cout << "La secuencia es un camino";

    if (esCaminoSimple(secuencia)) {
        cout << " simple";
    }

    cout << ". Longitud: " << secuencia.size() - 1 << " aristas.\n";

    if (esCiclo(secuencia, matriz, tipo)) {
        cout << "Tambien forma un ciclo";

        if (esCaminoSimple(secuencia)) {
            cout << " simple";
        }

        cout << ".\n";
    } else {
        cout << "No forma un ciclo.\n";
    }
}

bool generarArchivoDOT(const vector<vector<int>>& matriz,
                       const vector<char>& nombres,
                       TipoGrafo tipo,
                       const string& nombreArchivo) {
    ofstream archivo(nombreArchivo);

    if (!archivo) {
        return false;
    }

    bool dirigido = tipo == TipoGrafo::Dirigido;
    archivo << (dirigido ? "digraph" : "graph") << " G {\n";
    archivo << "    rankdir=LR;\n";
    archivo << "    node [shape=circle];\n";

    for (char nombre : nombres) {
        archivo << "    " << nombre << ";\n";
    }

    string conector = dirigido ? " -> " : " -- ";

    for (int i = 0; i < static_cast<int>(matriz.size()); i++) {
        int inicio = dirigido ? 0 : i + 1;

        for (int j = inicio; j < static_cast<int>(matriz.size()); j++) {
            if (matriz[i][j] == 1) {
                archivo << "    " << nombres[i] << conector << nombres[j] << ";\n";
            }
        }
    }

    archivo << "}\n";
    return true;
}

void mostrarMenu() {
    cout << "\nMenu\n";
    cout << "1. Mostrar matriz de adyacencia\n";
    cout << "2. Mostrar representacion matematica\n";
    cout << "3. Consultar nodos adyacentes\n";
    cout << "4. Verificar camino o ciclo\n";
    cout << "5. Generar archivo grafico\n";
    cout << "0. Salir\n";
}

int main() {
    cout << "=== Laboratorio 3: Teoria de grafos ===\n";

    int cantidadNodos = pedirCantidadNodos();
    vector<char> nombresNodos;
    pedirNombresNodos(nombresNodos, cantidadNodos);

    TipoGrafo tipo = pedirTipoGrafo();
    vector<vector<int>> matrizAdyacencia(
        cantidadNodos, vector<int>(cantidadNodos, 0));

    ingresarMatriz(matrizAdyacencia, nombresNodos, tipo);

    if (!validarMatriz(matrizAdyacencia, tipo)) {
        cerr << "La matriz de adyacencia no es valida.\n";
        return 1;
    }

    mostrarResumen(nombresNodos, tipo);
    imprimirMatriz(matrizAdyacencia, nombresNodos);

    int opcion;

    do {
        mostrarMenu();
        opcion = leerEnteroEnRango("Opcion: ", 0, 5);

        switch (opcion) {
            case 1:
                imprimirMatriz(matrizAdyacencia, nombresNodos);
                break;

            case 2:
                mostrarRepresentacionMatematica(
                    matrizAdyacencia, nombresNodos, tipo);
                break;

            case 3: {
                int nodo = pedirNodoExistente(
                    nombresNodos, "Nodo que desea consultar: ");
                mostrarAdyacentes(nodo, matrizAdyacencia, nombresNodos, tipo);
                break;
            }

            case 4: {
                vector<int> secuencia = pedirSecuenciaNodos(nombresNodos);
                analizarSecuencia(secuencia, matrizAdyacencia, tipo);
                break;
            }

            case 5:
                if (generarArchivoDOT(
                        matrizAdyacencia, nombresNodos, tipo, "grafo.dot")) {
                    cout << "Archivo grafo.dot generado correctamente.\n";
                    cout << "Puede convertirlo con: "
                         << "dot -Tpng grafo.dot -o grafo.png\n";
                } else {
                    cout << "No se pudo crear el archivo grafo.dot.\n";
                }
                break;

            case 0:
                cout << "Programa finalizado.\n";
                break;
        }
    } while (opcion != 0);

    return 0;
}
