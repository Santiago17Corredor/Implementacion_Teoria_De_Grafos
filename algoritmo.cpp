#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace std;

enum class TipoGrafo {
    NoDirigido = 1,
    Dirigido = 2
};

struct Conexion {
    int existe = 0;
    double peso = 0;
};

bool pesoValido(double peso) {
    return isfinite(peso) && abs(peso) <= 1e9;
}

string formatearPeso(double peso) {
    ostringstream salida;
    salida << setprecision(15) << (peso == 0 ? 0 : peso);
    return salida.str();
}

string textoConexion(const Conexion& conexion, bool ponderado) {
    if (!ponderado) {
        return to_string(conexion.existe);
    }

    return conexion.existe ? formatearPeso(conexion.peso) : "x";
}

string escaparJSON(const string& texto) {
    ostringstream salida;

    for (unsigned char caracter : texto) {
        switch (caracter) {
            case '"': salida << "\\\""; break;
            case '\\': salida << "\\\\"; break;
            case '\b': salida << "\\b"; break;
            case '\f': salida << "\\f"; break;
            case '\n': salida << "\\n"; break;
            case '\r': salida << "\\r"; break;
            case '\t': salida << "\\t"; break;
            default:
                if (caracter < 0x20) {
                    salida << "\\u" << hex << setw(4) << setfill('0')
                           << static_cast<int>(caracter) << dec << setfill(' ');
                } else {
                    salida << caracter;
                }
        }
    }

    return salida.str();
}

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

bool pedirPonderacion() {
    cout << "\nPesos de las conexiones:\n";
    cout << "1. No ponderado (0/1)\n";
    cout << "2. Ponderado (pesos numericos)\n";
    return leerEnteroEnRango("Opcion: ", 1, 2) == 2;
}

bool interpretarPeso(const string& texto, double& peso) {
    try {
        size_t consumidos;
        double valor = stod(texto, &consumidos);

        if (consumidos != texto.size() || !pesoValido(valor)) {
            return false;
        }

        peso = valor;
        return true;
    } catch (const invalid_argument&) {
        return false;
    } catch (const out_of_range&) {
        return false;
    }
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

Conexion pedirConexion(const string& origen, const string& destino,
                        TipoGrafo tipo, bool ponderado) {
    if (!ponderado) {
        int existe = pedirValorAdyacencia(origen, destino, tipo);
        return {existe, static_cast<double>(existe)};
    }

    string conector = tipo == TipoGrafo::Dirigido ? " -> " : " -- ";
    string mensaje = "Peso de '" + origen + "'" + conector + "'" + destino +
                     "' (x = sin conexion): ";

    while (true) {
        stringstream entrada(leerLinea(mensaje));
        string texto;
        string sobrante;
        double peso;

        if (!(entrada >> texto) || (entrada >> sobrante)) {
            cout << "Ingrese un solo peso o x.\n";
            continue;
        }

        if (texto == "x" || texto == "X") {
            return {};
        }

        if (!interpretarPeso(texto, peso)) {
            cout << "Peso invalido. Use un numero finito entre -1000000000 y "
                 << "1000000000, con punto decimal, o x.\n";
            continue;
        }

        if (origen == destino) {
            cout << "Se detecto un lazo en el nodo " << quoted(origen)
                 << ". Esta version usa grafos simples; ingrese x.\n";
            continue;
        }

        return {1, peso};
    }
}

void ingresarMatriz(vector<vector<Conexion>>& matriz,
                    const vector<string>& nombres,
                    TipoGrafo tipo, bool ponderado) {
    int cantidad = static_cast<int>(matriz.size());
    cout << "\nIngrese las conexiones del grafo, incluida la diagonal.\n";
    if (ponderado) {
        cout << "Use x para ausencia, incluso en la diagonal; 0 es una arista de costo cero.\n";
        cout << "Pesos entre -1000000000 y 1000000000; decimales con punto.\n";
    } else {
        cout << "La diagonal debe ser 0: esta version no admite lazos.\n";
    }

    for (int i = 0; i < cantidad; i++) {
        int inicio = tipo == TipoGrafo::Dirigido ? 0 : i;

        for (int j = inicio; j < cantidad; j++) {
            matriz[i][j] = pedirConexion(nombres[i], nombres[j], tipo, ponderado);

            if (tipo == TipoGrafo::NoDirigido) {
                matriz[j][i] = matriz[i][j];
            }
        }
    }
}

bool validarMatriz(const vector<vector<Conexion>>& matriz,
                   const vector<string>& nombres,
                   TipoGrafo tipo, bool ponderado,
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
            const Conexion& conexion = matriz[i][j];
            string celda = "['" + nombres[i] + "']['" + nombres[j] + "']";

            if (conexion.existe != 0 && conexion.existe != 1) {
                error = "Valor invalido en " + celda + ": " +
                        to_string(conexion.existe) + ". Solo se permite 0 o 1.";
                return false;
            }

            if (!pesoValido(conexion.peso)) {
                error = "Peso invalido en " + celda +
                        ". Debe ser finito y estar entre -1000000000 y 1000000000.";
                return false;
            }

            if ((!conexion.existe && conexion.peso != 0) ||
                (!ponderado && conexion.existe && conexion.peso != 1)) {
                error = "Peso inconsistente en " + celda +
                        ". Una ausencia guarda 0; una arista no ponderada guarda 1.";
                return false;
            }
        }
    }

    for (int i = 0; i < cantidad; i++) {
        if (matriz[i][i].existe) {
            error = "Se detecto un lazo en el nodo '" + nombres[i] +
                    "'. La diagonal debe ser " + (ponderado ? "x" : "0") +
                    " en un grafo simple.";
            return false;
        }
    }

    if (tipo == TipoGrafo::NoDirigido) {
        for (int i = 0; i < cantidad; i++) {
            for (int j = i + 1; j < cantidad; j++) {
                if (matriz[i][j].existe == matriz[j][i].existe &&
                    matriz[i][j].peso == matriz[j][i].peso) {
                    continue;
                }

                error = "Inconsistencia: de '" + nombres[i] + "' a '" + nombres[j] +
                        "' hay " + textoConexion(matriz[i][j], ponderado) + ", pero de '" +
                        nombres[j] + "' a '" + nombres[i] + "' hay " +
                        textoConexion(matriz[j][i], ponderado) + ". El grafo no dirigido debe ser simetrico.";
                return false;
            }
        }
    }

    return true;
}

void mostrarResumen(const vector<string>& nombres, TipoGrafo tipo, bool ponderado) {
    cout << "\nResumen del grafo\n";
    cout << "Cantidad de nodos: " << nombres.size() << '\n';
    cout << "Tipo: "
         << (tipo == TipoGrafo::Dirigido ? "Dirigido" : "No dirigido")
         << '\n';
    cout << "Ponderacion: " << (ponderado ? "Ponderado" : "No ponderado") << '\n';
}

void imprimirMatriz(const vector<vector<Conexion>>& matriz,
                    const vector<string>& nombres, bool ponderado) {
    int ancho = 3;

    for (const string& nombre : nombres) {
        ancho = max(ancho, static_cast<int>(nombre.size()) + 2);
    }

    for (const auto& fila : matriz) {
        for (const Conexion& conexion : fila) {
            ancho = max(ancho, static_cast<int>(textoConexion(conexion, ponderado).size()) + 2);
        }
    }

    if (ponderado) {
        cout << "\nPesos: x = sin conexion; 0 = arista de costo cero.\n";
    }

    cout << "\nMatriz de adyacencia:\n" << setw(ancho) << "";

    for (const string& nombre : nombres) {
        cout << setw(ancho) << nombre;
    }

    cout << '\n';

    for (int i = 0; i < static_cast<int>(matriz.size()); i++) {
        cout << setw(ancho) << nombres[i];

        for (const Conexion& conexion : matriz[i]) {
            cout << setw(ancho) << textoConexion(conexion, ponderado);
        }

        cout << '\n';
    }
}

void mostrarRepresentacionMatematica(const vector<vector<Conexion>>& matriz,
                                     const vector<string>& nombres,
                                     TipoGrafo tipo, bool ponderado) {
    cout << (tipo == TipoGrafo::Dirigido ? "\nG = (V, A)\n" : "\nG = (V, E)\n");
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
            if (!matriz[i][j].existe) {
                continue;
            }

            if (!primera) {
                cout << ", ";
            }

            if (tipo == TipoGrafo::Dirigido) {
                cout << '<' << quoted(nombres[i]) << ',' << quoted(nombres[j]);
                if (ponderado) {
                    cout << ',' << formatearPeso(matriz[i][j].peso);
                }
                cout << '>';
            } else if (ponderado) {
                cout << '(' << quoted(nombres[i]) << ',' << quoted(nombres[j])
                     << ',' << formatearPeso(matriz[i][j].peso) << ')';
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
                       const vector<vector<Conexion>>& matriz,
                       const vector<string>& nombres,
                       TipoGrafo tipo) {
    vector<int> salientes;
    vector<int> entrantes;

    for (int j = 0; j < static_cast<int>(matriz.size()); j++) {
        if (matriz[indice][j].existe) {
            salientes.push_back(j);
        }

        if (tipo == TipoGrafo::Dirigido && matriz[j][indice].existe) {
            entrantes.push_back(j);
        }
    }

    if (tipo == TipoGrafo::NoDirigido) {
        cout << "Adyacentes de " << nombres[indice] << ": ";
        imprimirNodosEncontrados(nombres, salientes);
        cout << "\nGrado: " << salientes.size() << '\n';
        if (salientes.empty()) {
            cout << "El nodo " << quoted(nombres[indice]) << " es un nodo aislado.\n";
        }
        return;
    }

    cout << "Sucesores de " << nombres[indice] << ": ";
    imprimirNodosEncontrados(nombres, salientes);
    cout << "\nPredecesores de " << nombres[indice] << ": ";
    imprimirNodosEncontrados(nombres, entrantes);
    cout << "\nGrado de salida: " << salientes.size();
    cout << "\nGrado de entrada: " << entrantes.size() << '\n';
    if (salientes.empty() && entrantes.empty()) {
        cout << "El nodo " << quoted(nombres[indice]) << " es un nodo aislado.\n";
    }
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
              const vector<vector<Conexion>>& matriz) {
    if (secuencia.empty()) {
        return false;
    }

    for (int i = 0; i + 1 < static_cast<int>(secuencia.size()); i++) {
        if (!matriz[secuencia[i]][secuencia[i + 1]].existe) {
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
             const vector<vector<Conexion>>& matriz,
             TipoGrafo tipo) {
    if (secuencia.size() < 3 || secuencia.front() != secuencia.back() ||
        !esCamino(secuencia, matriz)) {
        return false;
    }

    return tipo == TipoGrafo::Dirigido || tieneAristasDiferentes(secuencia);
}

void analizarSecuencia(const vector<int>& secuencia,
                       const vector<vector<Conexion>>& matriz,
                       TipoGrafo tipo, bool ponderado) {
    if (!esCamino(secuencia, matriz)) {
        cout << "La secuencia no es un camino.\n";
        return;
    }

    cout << "La secuencia es un camino";

    if (esCaminoSimple(secuencia)) {
        cout << " simple";
    }

    cout << ". Longitud: " << secuencia.size() - 1 << " aristas.\n";

    if (ponderado) {
        double costo = 0;
        for (int i = 0; i + 1 < static_cast<int>(secuencia.size()); i++) {
            costo += matriz[secuencia[i]][secuencia[i + 1]].peso;
        }
        cout << "Costo total: " << formatearPeso(costo) << '\n';
    }

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

double calcularCosto(const vector<int>& recorrido,
                     const vector<vector<Conexion>>& matriz) {
    double costo = 0;

    for (int i = 0; i + 1 < static_cast<int>(recorrido.size()); i++) {
        costo += matriz[recorrido[i]][recorrido[i + 1]].peso;
    }

    return costo;
}

void explorarCaminosSimples(int actual, int destino,
                            const vector<vector<Conexion>>& matriz,
                            vector<bool>& visitados, vector<int>& recorrido,
                            vector<vector<int>>& caminos) {
    if (actual == destino) {
        caminos.push_back(recorrido);
        return;
    }

    for (int vecino = 0; vecino < static_cast<int>(matriz.size()); vecino++) {
        if (!matriz[actual][vecino].existe || visitados[vecino]) {
            continue;
        }

        visitados[vecino] = true;
        recorrido.push_back(vecino);
        explorarCaminosSimples(vecino, destino, matriz, visitados, recorrido, caminos);
        recorrido.pop_back();
        visitados[vecino] = false;
    }
}

void explorarCiclosDesdeOrigen(int actual, int origen,
                               const vector<vector<Conexion>>& matriz,
                               TipoGrafo tipo, vector<bool>& visitados,
                               vector<int>& recorrido,
                               vector<vector<int>>& ciclos) {
    for (int vecino = 0; vecino < static_cast<int>(matriz.size()); vecino++) {
        if (!matriz[actual][vecino].existe) {
            continue;
        }

        if (vecino == origen) {
            vector<int> ciclo = recorrido;
            ciclo.push_back(origen);
            bool orientacionNueva = tipo == TipoGrafo::Dirigido ||
                (recorrido.size() > 2 && recorrido[1] < recorrido.back());

            if (orientacionNueva && esCiclo(ciclo, matriz, tipo)) {
                ciclos.push_back(ciclo);
            }
            continue;
        }

        if (!visitados[vecino]) {
            visitados[vecino] = true;
            recorrido.push_back(vecino);
            explorarCiclosDesdeOrigen(
                vecino, origen, matriz, tipo, visitados, recorrido, ciclos);
            recorrido.pop_back();
            visitados[vecino] = false;
        }
    }
}

vector<vector<int>> buscarCaminosSimples(
        int origen, int destino, const vector<vector<Conexion>>& matriz,
        TipoGrafo tipo) {
    vector<vector<int>> caminos;
    vector<bool> visitados(matriz.size(), false);
    vector<int> recorrido = {origen};
    visitados[origen] = true;

    if (origen == destino) {
        explorarCiclosDesdeOrigen(
            origen, origen, matriz, tipo, visitados, recorrido, caminos);
    } else {
        explorarCaminosSimples(
            origen, destino, matriz, visitados, recorrido, caminos);
    }

    return caminos;
}

void imprimirRecorrido(const vector<int>& recorrido,
                       const vector<string>& nombres) {
    for (int i = 0; i < static_cast<int>(recorrido.size()); i++) {
        if (i > 0) {
            cout << " -> ";
        }
        cout << nombres[recorrido[i]];
    }
}

void mostrarCaminosEntre(int origen, int destino,
                         const vector<vector<Conexion>>& matriz,
                         const vector<string>& nombres, TipoGrafo tipo,
                         bool ponderado) {
    vector<vector<int>> caminos = buscarCaminosSimples(origen, destino, matriz, tipo);
    bool buscaCiclos = origen == destino;

    if (buscaCiclos) {
        cout << "\nEl origen y el destino son el mismo nodo. "
             << "Se buscaran ciclos simples.\n";
    }

    if (caminos.empty()) {
        if (buscaCiclos) {
            cout << "No existen ciclos que inicien y terminen en "
                 << quoted(nombres[origen]) << ".\n";
        } else {
            cout << "No existe camino posible entre " << quoted(nombres[origen])
                 << " y " << quoted(nombres[destino]) << ".\n";
        }
        return;
    }

    cout << (buscaCiclos ? "Ciclos encontrados: " : "Caminos encontrados: ")
         << caminos.size() << '\n';

    for (int i = 0; i < static_cast<int>(caminos.size()); i++) {
        cout << (buscaCiclos ? "Ciclo " : "Camino ") << i + 1 << ": ";
        imprimirRecorrido(caminos[i], nombres);
        cout << "\n  Longitud: " << caminos[i].size() - 1 << " aristas.";
        if (ponderado) {
            cout << " Costo: " << formatearPeso(calcularCosto(caminos[i], matriz)) << '.';
        }
        cout << (buscaCiclos ? " Ciclo simple: si.\n" : " Camino simple: si.\n");
    }
}

bool buscarCicloDFS(int actual, int padre,
                    const vector<vector<Conexion>>& matriz, TipoGrafo tipo,
                    vector<int>& estado, vector<int>& pila,
                    vector<int>& ciclo) {
    estado[actual] = 1;
    pila.push_back(actual);

    for (int vecino = 0; vecino < static_cast<int>(matriz.size()); vecino++) {
        if (!matriz[actual][vecino].existe ||
            (tipo == TipoGrafo::NoDirigido && vecino == padre)) {
            continue;
        }

        if (estado[vecino] == 0) {
            if (buscarCicloDFS(vecino, actual, matriz, tipo, estado, pila, ciclo)) {
                return true;
            }
        } else if (estado[vecino] == 1) {
            auto inicio = find(pila.begin(), pila.end(), vecino);
            ciclo.assign(inicio, pila.end());
            ciclo.push_back(vecino);
            return true;
        }
    }

    pila.pop_back();
    estado[actual] = 2;
    return false;
}

vector<int> buscarUnCiclo(const vector<vector<Conexion>>& matriz,
                          TipoGrafo tipo) {
    vector<int> estado(matriz.size(), 0);
    vector<int> pila;
    vector<int> ciclo;

    for (int nodo = 0; nodo < static_cast<int>(matriz.size()); nodo++) {
        if (estado[nodo] == 0 &&
            buscarCicloDFS(nodo, -1, matriz, tipo, estado, pila, ciclo)) {
            return ciclo;
        }
    }

    return {};
}

void mostrarDeteccionCiclos(const vector<vector<Conexion>>& matriz,
                            const vector<string>& nombres, TipoGrafo tipo,
                            bool ponderado) {
    vector<int> ciclo = buscarUnCiclo(matriz, tipo);

    if (ciclo.empty()) {
        cout << "\nEl grafo no contiene ciclos; es aciclico.\n";
        return;
    }

    cout << "\nEl grafo contiene ciclos.\nCiclo encontrado: ";
    imprimirRecorrido(ciclo, nombres);
    cout << "\nLongitud: " << ciclo.size() - 1 << " aristas.";
    if (ponderado) {
        cout << " Costo: " << formatearPeso(calcularCosto(ciclo, matriz)) << '.';
    }
    cout << "\nClasificacion: ciclo simple.\n";
}

bool generarArchivoDOT(const vector<vector<Conexion>>& matriz,
                       const vector<string>& nombres,
                       TipoGrafo tipo, bool ponderado,
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
            if (matriz[i][j].existe) {
                archivo << "    n" << i << conector << "n" << j;
                if (ponderado) {
                    archivo << " [label=" << quoted(formatearPeso(matriz[i][j].peso)) << ']';
                }
                archivo << ";\n";
            }
        }
    }

    archivo << "}\n";
    archivo.close();
    return !archivo.fail();
}

bool generarArchivoDatos(const vector<vector<Conexion>>& matriz,
                         const vector<string>& nombres, TipoGrafo tipo,
                         bool ponderado, const string& nombreArchivo) {
    ofstream archivo(nombreArchivo);

    if (!archivo) {
        return false;
    }

    bool dirigido = tipo == TipoGrafo::Dirigido;
    archivo << "{\n  \"dirigido\": " << (dirigido ? "true" : "false")
            << ",\n  \"ponderado\": " << (ponderado ? "true" : "false")
            << ",\n  \"nodos\": [";

    for (int i = 0; i < static_cast<int>(nombres.size()); i++) {
        if (i > 0) {
            archivo << ", ";
        }
        archivo << '"' << escaparJSON(nombres[i]) << '"';
    }

    archivo << "],\n  \"aristas\": [";
    bool primera = true;

    for (int i = 0; i < static_cast<int>(matriz.size()); i++) {
        int inicio = dirigido ? 0 : i + 1;

        for (int j = inicio; j < static_cast<int>(matriz.size()); j++) {
            if (!matriz[i][j].existe) {
                continue;
            }

            if (!primera) {
                archivo << ',';
            }
            archivo << "\n    {\"origen\": " << i << ", \"destino\": " << j;
            if (ponderado) {
                archivo << ", \"peso\": " << formatearPeso(matriz[i][j].peso);
            }
            archivo << '}';
            primera = false;
        }
    }

    if (!primera) {
        archivo << '\n';
    }
    archivo << "  ]\n}\n";
    archivo.close();
    return !archivo.fail();
}

struct AristaGrafo {
    int u;
    int v;
    double peso;
};

struct ResultadoRuta {
    bool ejecutado = false;
    bool errorPesosNegativos = false;
    bool cicloNegativo = false;
    bool alcanzable = false;
    double costo = 0.0;
    vector<int> ruta;
    long long relajaciones = 0;
    long long tiempo_us = 0;
};

bool tieneAristasNegativas(const vector<vector<Conexion>>& matriz) {
    for (const auto& fila : matriz) {
        for (const auto& celda : fila) {
            if (celda.existe && celda.peso < 0) {
                return true;
            }
        }
    }
    return false;
}

ResultadoRuta ejecutarDijkstra(int origen, int destino,
                               const vector<vector<Conexion>>& matriz) {
    ResultadoRuta res;
    res.ejecutado = true;
    if (tieneAristasNegativas(matriz)) {
        res.errorPesosNegativos = true;
        return res;
    }

    auto t1 = chrono::high_resolution_clock::now();
    int n = static_cast<int>(matriz.size());
    const double INF = numeric_limits<double>::infinity();
    vector<double> dist(n, INF);
    vector<int> prev(n, -1);
    using ElementoPQ = pair<double, int>;
    priority_queue<ElementoPQ, vector<ElementoPQ>, greater<ElementoPQ>> pq;

    dist[origen] = 0.0;
    pq.push({0.0, origen});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (d > dist[u]) continue;
        if (u == destino) {
            break;
        }

        for (int v = 0; v < n; v++) {
            if (!matriz[u][v].existe) continue;
            res.relajaciones++;
            double nuevo = dist[u] + matriz[u][v].peso;
            if (nuevo < dist[v]) {
                dist[v] = nuevo;
                prev[v] = u;
                pq.push({nuevo, v});
            }
        }
    }

    auto t2 = chrono::high_resolution_clock::now();
    res.tiempo_us = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();

    if (dist[destino] != INF) {
        res.alcanzable = true;
        res.costo = dist[destino];
        int cur = destino;
        vector<bool> visitado(n, false);
        while (cur != -1 && !visitado[cur]) {
            visitado[cur] = true;
            res.ruta.push_back(cur);
            if (cur == origen) break;
            cur = prev[cur];
        }
        if (!res.ruta.empty() && res.ruta.back() == origen) {
            reverse(res.ruta.begin(), res.ruta.end());
        } else {
            res.alcanzable = false;
            res.ruta.clear();
        }
    }

    return res;
}

ResultadoRuta ejecutarBellmanFord(int origen, int destino,
                                  const vector<vector<Conexion>>& matriz) {
    ResultadoRuta res;
    res.ejecutado = true;
    auto t1 = chrono::high_resolution_clock::now();
    int n = static_cast<int>(matriz.size());
    const double INF = numeric_limits<double>::infinity();
    vector<double> dist(n, INF);
    vector<int> prev(n, -1);

    vector<AristaGrafo> aristas;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (matriz[i][j].existe) {
                aristas.push_back({i, j, matriz[i][j].peso});
            }
        }
    }

    dist[origen] = 0.0;

    for (int it = 1; it <= n - 1; it++) {
        bool cambio = false;
        for (const auto& a : aristas) {
            if (dist[a.u] != INF) {
                res.relajaciones++;
                if (dist[a.u] + a.peso < dist[a.v]) {
                    dist[a.v] = dist[a.u] + a.peso;
                    prev[a.v] = a.u;
                    cambio = true;
                }
            }
        }
        if (!cambio) break;
    }

    for (const auto& a : aristas) {
        if (dist[a.u] != INF && dist[a.u] + a.peso < dist[a.v]) {
            res.cicloNegativo = true;
            break;
        }
    }

    auto t2 = chrono::high_resolution_clock::now();
    res.tiempo_us = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();

    if (!res.cicloNegativo && dist[destino] != INF) {
        res.alcanzable = true;
        res.costo = dist[destino];
        int cur = destino;
        vector<bool> visitado(n, false);
        while (cur != -1 && !visitado[cur]) {
            visitado[cur] = true;
            res.ruta.push_back(cur);
            if (cur == origen) break;
            cur = prev[cur];
        }
        if (!res.ruta.empty() && res.ruta.back() == origen) {
            reverse(res.ruta.begin(), res.ruta.end());
        } else {
            res.alcanzable = false;
            res.ruta.clear();
        }
    }

    return res;
}

void mostrarResultadoRuta(const string& nombreAlgoritmo,
                          const ResultadoRuta& res,
                          int origen, int destino,
                          const vector<string>& nombres) {
    cout << "\n=== " << nombreAlgoritmo << " ===\n";
    cout << "Origen: " << quoted(nombres[origen])
         << " -> Destino: " << quoted(nombres[destino]) << '\n';

    if (res.errorPesosNegativos) {
        cout << "Error: El grafo contiene aristas con peso negativo. "
             << "El algoritmo de Dijkstra no es aplicable.\n";
        return;
    }

    if (res.cicloNegativo) {
        cout << "ALERTA: Se detecto un ciclo de costo negativo alcanzable.\n";
        cout << "Relajaciones: " << res.relajaciones << '\n';
        cout << "Tiempo de ejecucion: " << res.tiempo_us << " us\n";
        return;
    }

    if (!res.alcanzable) {
        cout << "No existe camino entre " << quoted(nombres[origen])
             << " y " << quoted(nombres[destino]) << ".\n";
        cout << "Relajaciones: " << res.relajaciones << '\n';
        cout << "Tiempo de ejecucion: " << res.tiempo_us << " us\n";
        return;
    }

    cout << "Ruta: ";
    imprimirRecorrido(res.ruta, nombres);
    cout << "\nCosto total: " << formatearPeso(res.costo) << '\n';
    cout << "Relajaciones: " << res.relajaciones << '\n';
    cout << "Tiempo de ejecucion: " << res.tiempo_us << " us\n";
}

void mostrarBenchmark(int origen, int destino,
                      const vector<vector<Conexion>>& matriz,
                      const vector<string>& nombres) {
    cout << "\n=== Benchmark Comparativo: Dijkstra vs Bellman-Ford ===\n";
    cout << "Origen: " << quoted(nombres[origen])
         << " -> Destino: " << quoted(nombres[destino]) << "\n\n";

    ResultadoRuta dij = ejecutarDijkstra(origen, destino, matriz);
    ResultadoRuta bf = ejecutarBellmanFord(origen, destino, matriz);

    cout << left << setw(28) << "Metrica"
         << "| " << setw(25) << "Dijkstra"
         << "| " << setw(25) << "Bellman-Ford" << '\n';
    cout << string(82, '-') << '\n';

    string estadoDij = dij.errorPesosNegativos ? "Error (peso negativo)" :
                       (!dij.alcanzable ? "No alcanzable" : "Exito");
    string estadoBf = bf.cicloNegativo ? "Ciclo negativo" :
                      (!bf.alcanzable ? "No alcanzable" : "Exito");
    cout << left << setw(28) << "Estado"
         << "| " << setw(25) << estadoDij
         << "| " << setw(25) << estadoBf << '\n';

    string costoDij = (!dij.ejecutado || dij.errorPesosNegativos || !dij.alcanzable) ? "N/A" : formatearPeso(dij.costo);
    string costoBf = (bf.cicloNegativo || !bf.alcanzable) ? "N/A" : formatearPeso(bf.costo);
    cout << left << setw(28) << "Costo total"
         << "| " << setw(25) << costoDij
         << "| " << setw(25) << costoBf << '\n';

    string rutaDij = "N/A";
    if (dij.alcanzable && !dij.ruta.empty()) {
        ostringstream ss;
        for (size_t i = 0; i < dij.ruta.size(); i++) {
            if (i > 0) ss << "->";
            ss << nombres[dij.ruta[i]];
        }
        rutaDij = ss.str();
    }
    string rutaBf = "N/A";
    if (bf.alcanzable && !bf.ruta.empty()) {
        ostringstream ss;
        for (size_t i = 0; i < bf.ruta.size(); i++) {
            if (i > 0) ss << "->";
            ss << nombres[bf.ruta[i]];
        }
        rutaBf = ss.str();
    }
    cout << left << setw(28) << "Ruta"
         << "| " << setw(25) << (rutaDij.size() > 24 ? rutaDij.substr(0, 21) + "..." : rutaDij)
         << "| " << setw(25) << (rutaBf.size() > 24 ? rutaBf.substr(0, 21) + "..." : rutaBf) << '\n';

    string relDij = dij.errorPesosNegativos ? "N/A" : to_string(dij.relajaciones);
    string relBf = to_string(bf.relajaciones);
    cout << left << setw(28) << "Relajaciones"
         << "| " << setw(25) << relDij
         << "| " << setw(25) << relBf << '\n';

    string tiemDij = dij.errorPesosNegativos ? "N/A" : to_string(dij.tiempo_us) + " us";
    string tiemBf = to_string(bf.tiempo_us) + " us";
    cout << left << setw(28) << "Tiempo de ejecucion"
         << "| " << setw(25) << tiemDij
         << "| " << setw(25) << tiemBf << '\n';

    cout << string(82, '-') << '\n';

    if (dij.errorPesosNegativos) {
        cout << "Consistencia: Dijkstra no es aplicable debido a aristas con peso negativo.\n"
             << "              Bellman-Ford es el algoritmo adecuado para este escenario.\n";
    } else if (bf.cicloNegativo) {
        cout << "Consistencia: Bellman-Ford detecto un ciclo de costo negativo alcanzable.\n";
    } else if (dij.alcanzable && bf.alcanzable) {
        if (abs(dij.costo - bf.costo) < 1e-9 && dij.ruta == bf.ruta) {
            cout << "Consistencia: Ambos algoritmos produjeron exactamente el mismo costo y ruta.\n";
        } else if (abs(dij.costo - bf.costo) < 1e-9) {
            cout << "Consistencia: Ambos algoritmos produjeron el mismo costo minimo con rutas alternas.\n";
        } else {
            cout << "Consistencia: Diferencia detectada en costos calculados.\n";
        }
    } else if (!dij.alcanzable && !bf.alcanzable) {
        cout << "Consistencia: Ambos algoritmos determinaron que el destino es inalcanzable.\n";
    } else {
        cout << "Consistencia: Discrepancia en alcanzabilidad entre algoritmos.\n";
    }
}

void mostrarMenu() {
    cout << "\nMenu\n";
    cout << "1. Mostrar matriz de adyacencia\n";
    cout << "2. Mostrar representacion matematica\n";
    cout << "3. Consultar nodos adyacentes\n";
    cout << "4. Verificar camino o ciclo\n";
    cout << "5. Generar archivo grafico\n";
    cout << "6. Buscar todos los caminos entre dos nodos\n";
    cout << "7. Detectar ciclos automaticamente\n";
    cout << "8. Algoritmo de Dijkstra\n";
    cout << "9. Algoritmo de Bellman-Ford\n";
    cout << "10. Benchmark comparativo (Dijkstra vs Bellman-Ford)\n";
    cout << "0. Salir\n";
}

int main() {
    cout << "=== Laboratorio 3: Teoria de grafos ===\n";

    int cantidadNodos = pedirCantidadNodos();
    vector<string> nombresNodos;
    pedirNombresNodos(nombresNodos, cantidadNodos);

    TipoGrafo tipo = pedirTipoGrafo();
    bool ponderado = pedirPonderacion();
    vector<vector<Conexion>> matrizAdyacencia(
        cantidadNodos, vector<Conexion>(cantidadNodos));

    string errorMatriz;
    bool matrizValida;

    do {
        ingresarMatriz(matrizAdyacencia, nombresNodos, tipo, ponderado);
        matrizValida = validarMatriz(matrizAdyacencia, nombresNodos, tipo, ponderado, errorMatriz);

        if (!matrizValida) {
            cout << errorMatriz << "\nVuelva a ingresar la matriz.\n";
        }
    } while (!matrizValida);

    mostrarResumen(nombresNodos, tipo, ponderado);
    imprimirMatriz(matrizAdyacencia, nombresNodos, ponderado);

    int opcion;

    do {
        mostrarMenu();
        opcion = leerEnteroEnRango("Opcion: ", 0, 10);

        switch (opcion) {
            case 1:
                imprimirMatriz(matrizAdyacencia, nombresNodos, ponderado);
                break;

            case 2:
                mostrarRepresentacionMatematica(
                    matrizAdyacencia, nombresNodos, tipo, ponderado);
                break;

            case 3: {
                int nodo = pedirNodoExistente(
                    nombresNodos, "Nodo que desea consultar: ");
                mostrarAdyacentes(nodo, matrizAdyacencia, nombresNodos, tipo);
                break;
            }

            case 4: {
                vector<int> secuencia = pedirSecuenciaNodos(nombresNodos);
                analizarSecuencia(secuencia, matrizAdyacencia, tipo, ponderado);
                break;
            }

            case 5:
                if (generarArchivoDOT(matrizAdyacencia, nombresNodos, tipo,
                                      ponderado, "grafo.dot") &&
                    generarArchivoDatos(matrizAdyacencia, nombresNodos, tipo,
                                        ponderado, "grafo.json")) {
                    cout << "Archivo grafo.dot generado correctamente.\n";
                    cout << "Archivo grafo.json generado correctamente.\n";
                    cout << "Visualice el grafo con: python interfaz.py grafo.json\n";
                } else {
                    cout << "No se pudieron crear los archivos del grafo.\n";
                }
                break;

            case 6: {
                int origen = pedirNodoExistente(nombresNodos, "Nodo origen: ");
                int destino = pedirNodoExistente(nombresNodos, "Nodo destino: ");
                mostrarCaminosEntre(
                    origen, destino, matrizAdyacencia, nombresNodos, tipo, ponderado);
                break;
            }

            case 7:
                mostrarDeteccionCiclos(
                    matrizAdyacencia, nombresNodos, tipo, ponderado);
                break;

            case 8: {
                int origen = pedirNodoExistente(nombresNodos, "Nodo origen: ");
                int destino = pedirNodoExistente(nombresNodos, "Nodo destino: ");
                ResultadoRuta res = ejecutarDijkstra(origen, destino, matrizAdyacencia);
                mostrarResultadoRuta("Algoritmo de Dijkstra", res, origen, destino, nombresNodos);
                break;
            }

            case 9: {
                int origen = pedirNodoExistente(nombresNodos, "Nodo origen: ");
                int destino = pedirNodoExistente(nombresNodos, "Nodo destino: ");
                ResultadoRuta res = ejecutarBellmanFord(origen, destino, matrizAdyacencia);
                mostrarResultadoRuta("Algoritmo de Bellman-Ford", res, origen, destino, nombresNodos);
                break;
            }

            case 10: {
                int origen = pedirNodoExistente(nombresNodos, "Nodo origen: ");
                int destino = pedirNodoExistente(nombresNodos, "Nodo destino: ");
                mostrarBenchmark(origen, destino, matrizAdyacencia, nombresNodos);
                break;
            }

            case 0:
                cout << "Programa finalizado.\n";
                break;
        }
    } while (opcion != 0);

    return 0;
}
