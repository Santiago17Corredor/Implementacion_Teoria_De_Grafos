// Reutiliza el programa real con otro nombre para su main; no copia la validacion.
#define main mainLaboratorio
#include "../algoritmo.cpp"
#undef main

int comprobar(const string& caso,
              const vector<vector<int>>& matriz,
              const vector<string>& nombres,
              TipoGrafo tipo,
              bool esperado,
              const string& detalle = "") {
    vector<vector<Conexion>> conexiones(matriz.size());
    for (size_t i = 0; i < matriz.size(); i++) {
        for (int valor : matriz[i]) {
            conexiones[i].push_back({valor, static_cast<double>(valor)});
        }
    }
    string error = "Error de una validacion anterior";
    bool valido = validarMatriz(conexiones, nombres, tipo, false, error);

    if (valido != esperado || (valido && !error.empty()) ||
        (!valido && (error.empty() || error.find(detalle) == string::npos))) {
        cerr << "FALLO: " << caso << ". Error recibido: " << error << '\n';
        return 1;
    }

    cout << "OK: " << caso << '\n';
    return 0;
}

int main() {
    int fallos = 0;
    vector<string> nombres = {"A", "B"};
    TipoGrafo noDirigido = TipoGrafo::NoDirigido;
    TipoGrafo dirigido = TipoGrafo::Dirigido;

    fallos += comprobar("matriz vacia", {}, {}, noDirigido, false, "entre 2 y 26");
    fallos += comprobar("un nodo", {{0}}, {"A"}, noDirigido, false, "entre 2 y 26");
    fallos += comprobar("mas de 26 nodos", vector<vector<int>>(27, vector<int>(27)),
                        vector<string>(27), noDirigido, false, "entre 2 y 26");
    fallos += comprobar("faltan nombres", {{0, 1}, {1, 0}}, {"A"}, noDirigido,
                        false, "cantidad de nodos");
    fallos += comprobar("segunda fila vacia", {{0, 1}, {}}, nombres, noDirigido,
                        false, "fila de 'B' tiene 0 valores");
    fallos += comprobar("ultima fila incompleta", {{0, 1, 0}, {1, 0, 1}, {0}},
                        {"A", "B", "C"}, noDirigido, false, "fila de 'C'");
    fallos += comprobar("fila con datos de mas", {{0, 1}, {1, 0, 0}}, nombres,
                        noDirigido, false, "fila de 'B' tiene 3 valores");
    fallos += comprobar("valor fuera de rango", {{0, 2}, {1, 0}}, nombres,
                        noDirigido, false, "['A']['B']: 2");
    fallos += comprobar("valor negativo", {{0, 0}, {-1, 0}}, nombres,
                        dirigido, false, "['B']['A']: -1");
    fallos += comprobar("lazo no dirigido", {{1, 0}, {0, 0}}, nombres,
                        noDirigido, false, "lazo en el nodo 'A'");
    fallos += comprobar("lazo dirigido", {{0, 0}, {0, 1}}, nombres,
                        dirigido, false, "lazo en el nodo 'B'");
    fallos += comprobar("asimetria no dirigida", {{0, 1}, {0, 0}}, nombres,
                        noDirigido, false, "de 'A' a 'B' hay 1, pero de 'B' a 'A' hay 0");
    fallos += comprobar("tipo desconocido", {{0, 0}, {0, 0}}, nombres,
                        static_cast<TipoGrafo>(9), false, "tipo de grafo");
    fallos += comprobar("grafo no dirigido valido", {{0, 1}, {1, 0}}, nombres,
                        noDirigido, true);
    fallos += comprobar("asimetria dirigida valida", {{0, 1}, {0, 0}}, nombres,
                        dirigido, true);
    fallos += comprobar("nodos aislados validos", {{0, 0}, {0, 0}}, nombres,
                        noDirigido, true);

    cout << "Fallos: " << fallos << '\n';
    return fallos == 0 ? 0 : 1;
}
