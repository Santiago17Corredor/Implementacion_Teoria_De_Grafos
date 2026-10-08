#include <algorithm>
#include <cctype>
#include <cstdlib>
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

string leerLinea(const string& mensaje) {
    cout << mensaje;
    string linea;

    if (!getline(cin, linea)) {
        cerr << "\nEntrada finalizada. Programa cerrado.\n";
        exit(EXIT_FAILURE);
    }

    return linea;
}

int leerEnteroEnRango(const string& mensaje, int minimo, int maximo,
                     const string& mensajeMinimo = "") {
    int valor;
    char sobrante;

    while (true) {
        string linea = leerLinea(mensaje);
        stringstream entrada(linea);

        if (entrada >> valor && !(entrada >> sobrante)) {
            if (valor >= minimo && valor <= maximo) {
                return valor;
            }

            if (valor < minimo && !mensajeMinimo.empty()) {
                cout << mensajeMinimo << '\n';
                continue;
            }
        }

        cout << "Ingrese un numero entre " << minimo << " y " << maximo << ".\n";
    }
}

bool nombreRepetido(const vector<string>& nombres, const string& nombre) {
    for (const string& registrado : nombres) {
        if (registrado == nombre) {
            return true;
        }
    }

    return false;
}

string pedirNombreNodo(const string& mensaje) {
    while (true) {
        string nombre = leerLinea(mensaje);
        size_t inicio = nombre.find_first_not_of(" \t\r");

        if (inicio == string::npos) {
            cout << "El nombre del nodo no puede estar vacio.\n";
            continue;
        }

        size_t fin = nombre.find_last_not_of(" \t\r");
        nombre = nombre.substr(inicio, fin - inicio + 1);
        bool tieneControl = false;

        for (unsigned char caracter : nombre) {
            if (iscntrl(caracter)) {
                tieneControl = true;
                break;
            }
        }

        if (!tieneControl) {
            return nombre;
        }

        cout << "El nombre no puede contener caracteres de control.\n";
    }
}

int pedirCantidadNodos() {
    return leerEnteroEnRango("Ingrese la cantidad de nodos (2 a 26): ", 2, 26,
        "El grafo requiere al menos 2 nodos para estructurar la matriz de adyacencia");
}

void pedirNombresNodos(vector<string>& nombres, int cantidad) {
    cout << "\nAsigne un nombre diferente a cada nodo (un nombre por linea).\n";
    cout << "Se distinguen mayusculas y minusculas.\n";

    for (int i = 0; i < cantidad; i++) {
        while (true) {
            string nombre = pedirNombreNodo("Nombre del nodo " + to_string(i + 1) + ": ");

            if (!nombreRepetido(nombres, nombre)) {
                nombres.push_back(nombre);
                break;
            }

            cout << "El nombre ya pertenece a otro nodo. Ingrese uno diferente.\n";
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

int pedirValorAdyacencia(const string& origen, const string& destino, TipoGrafo tipo) {
    string conector = tipo == TipoGrafo::Dirigido ? " -> " : " -- ";
    string mensaje = "Existe la arista '" + origen + "'" + conector +
                     "'" + destino + "'? (0/1): ";

    while (true) {
        int valor = leerEnteroEnRango(mensaje, 0, 1);

        if (origen != destino || valor == 0) {
            return valor;
        }

        cout << "Se detecto un lazo en el nodo " << quoted(origen)
             << ". Esta version usa grafos simples; ingrese 0.\n";
    }
}

void ingresarMatriz(vector<vector<int>>& matriz,
                    const vector<string>& nombres,
                    TipoGrafo tipo) {
    int cantidad = static_cast<int>(matriz.size());
    cout << "\nIngrese las conexiones del grafo, incluida la diagonal.\n";
    cout << "La diagonal debe ser 0: esta version no admite lazos.\n";

    for (int i = 0; i < cantidad; i++) {
        int inicio = tipo == TipoGrafo::Dirigido ? 0 : i;

        for (int j = inicio; j < cantidad; j++) {
            matriz[i][j] = pedirValorAdyacencia(nombres[i], nombres[j], tipo);

            if (tipo == TipoGrafo::NoDirigido) {
                matriz[j][i] = matriz[i][j];
            }
        }
    }
}

bool validarMatriz(const vector<vector<int>>& matriz,
                   const vector<string>& nombres,
                   TipoGrafo tipo,
                   string& error) {
    error.clear();
    int cantidad = static_cast<int>(matriz.size());

    if (cantidad < 2 || cantidad > 26) {
        error = "La matriz debe tener entre 2 y 26 filas.";
        return false;
    }

    if (matriz.size() != nombres.size()) {
        error = "La cantidad de filas no coincide con la cantidad de nodos.";
        return false;
    }

    if (tipo != TipoGrafo::Dirigido && tipo != TipoGrafo::NoDirigido) {
        error = "El tipo de grafo no es valido.";
        return false;
    }

    // Todas las filas deben estar completas antes de consultar la diagonal o la simetria.
    for (int i = 0; i < cantidad; i++) {
        if (static_cast<int>(matriz[i].size()) != cantidad) {
            error = "La fila de '" + nombres[i] + "' tiene " +
                    to_string(matriz[i].size()) + " valores; se esperaban " +
                    to_string(cantidad) + ".";
            return false;
        }
    }

    for (int i = 0; i < cantidad; i++) {
        for (int j = 0; j < cantidad; j++) {
            if (matriz[i][j] != 0 && matriz[i][j] != 1) {
                error = "Valor invalido en ['" + nombres[i] + "']['" + nombres[j] +
                        "']: " + to_string(matriz[i][j]) + ". Solo se permite 0 o 1.";
                return false;
            }
        }
    }

    for (int i = 0; i < cantidad; i++) {
        if (matriz[i][i] != 0) {
            error = "Se detecto un lazo en el nodo '" + nombres[i] +
                    "'. La diagonal debe ser 0 en un grafo simple.";
            return false;
        }
    }

    if (tipo == TipoGrafo::NoDirigido) {
        for (int i = 0; i < cantidad; i++) {
            for (int j = i + 1; j < cantidad; j++) {
                if (matriz[i][j] == matriz[j][i]) {
                    continue;
                }

                error = "Inconsistencia: de '" + nombres[i] + "' a '" + nombres[j] +
                        "' hay " + to_string(matriz[i][j]) + ", pero de '" +
                        nombres[j] + "' a '" + nombres[i] + "' hay " +
                        to_string(matriz[j][i]) + ". El grafo no dirigido debe ser simetrico.";
                return false;
            }
        }
    }

    return true;
}

void mostrarResumen(const vector<string>& nombres, TipoGrafo tipo) {
    cout << "\nResumen del grafo\n";
    cout << "Cantidad de nodos: " << nombres.size() << '\n';
    cout << "Tipo: "
         << (tipo == TipoGrafo::Dirigido ? "Dirigido" : "No dirigido")
         << '\n';
}

void imprimirMatriz(const vector<vector<int>>& matriz,
                    const vector<string>& nombres) {
    int ancho = 3;

    for (const string& nombre : nombres) {
        ancho = max(ancho, static_cast<int>(nombre.size()) + 2);
    }

    cout << "\nMatriz de adyacencia:\n" << setw(ancho) << "";

    for (const string& nombre : nombres) {
        cout << setw(ancho) << nombre;
    }

    cout << '\n';

    for (int i = 0; i < static_cast<int>(matriz.size()); i++) {
        cout << setw(ancho) << nombres[i];

        for (int valor : matriz[i]) {
            cout << setw(ancho) << valor;
        }

        cout << '\n';
    }
}

void mostrarRepresentacionMatematica(const vector<vector<int>>& matriz,
                                     const vector<string>& nombres,
                                     TipoGrafo tipo) {
    cout << "\nV = {";

    for (int i = 0; i < static_cast<int>(nombres.size()); i++) {
        if (i > 0) {
            cout << ", ";
        }
        cout << quoted(nombres[i]);
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
                cout << '<' << quoted(nombres[i]) << ',' << quoted(nombres[j]) << '>';
            } else {
                cout << '{' << quoted(nombres[i]) << ',' << quoted(nombres[j]) << '}';
            }

            primera = false;
        }
    }

    cout << "}\n";
}

int buscarIndiceNodo(const vector<string>& nombres, const string& nombre) {
    for (int i = 0; i < static_cast<int>(nombres.size()); i++) {
        if (nombres[i] == nombre) {
            return i;
        }
    }

    return -1;
}

int pedirNodoExistente(const vector<string>& nombres, const string& mensaje) {
    while (true) {
        string nombre = pedirNombreNodo(mensaje);
        int indice = buscarIndiceNodo(nombres, nombre);

        if (indice != -1) {
            return indice;
        }

        cout << "El nodo no existe en el grafo.\n";
    }
}

void imprimirNodosEncontrados(const vector<string>& nombres,
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
                       const vector<string>& nombres,
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

vector<int> pedirSecuenciaNodos(const vector<string>& nombres) {
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
                       const vector<string>& nombres,
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

    // El indice identifica al nodo; el nombre se escribe como una etiqueta escapada.
    for (int i = 0; i < static_cast<int>(nombres.size()); i++) {
        archivo << "    n" << i << " [label=" << quoted(nombres[i]) << "];\n";
    }

    string conector = dirigido ? " -> " : " -- ";

    for (int i = 0; i < static_cast<int>(matriz.size()); i++) {
        int inicio = dirigido ? 0 : i + 1;

        for (int j = inicio; j < static_cast<int>(matriz.size()); j++) {
            if (matriz[i][j] == 1) {
                archivo << "    n" << i << conector << "n" << j << ";\n";
            }
        }
    }

    archivo << "}\n";
    archivo.close();
    return !archivo.fail();
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
    vector<string> nombresNodos;
    pedirNombresNodos(nombresNodos, cantidadNodos);

    TipoGrafo tipo = pedirTipoGrafo();
    vector<vector<int>> matrizAdyacencia(
        cantidadNodos, vector<int>(cantidadNodos, 0));

    string errorMatriz;
    bool matrizValida;

    do {
        ingresarMatriz(matrizAdyacencia, nombresNodos, tipo);
        matrizValida = validarMatriz(matrizAdyacencia, nombresNodos, tipo, errorMatriz);

        if (!matrizValida) {
            cout << errorMatriz << "\nVuelva a ingresar la matriz.\n";
        }
    } while (!matrizValida);

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
