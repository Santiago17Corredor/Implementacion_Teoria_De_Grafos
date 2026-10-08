#include <limits>

#define main mainLaboratorio
#include "../algoritmo.cpp"
#undef main

int comprobar(const string& caso, bool condicion) {
    cout << (condicion ? "OK: " : "FALLO: ") << caso << '\n';
    return condicion ? 0 : 1;
}

int revisarMatriz(const string& caso, const vector<vector<Conexion>>& matriz,
                  TipoGrafo tipo, bool ponderado, bool esperado,
                  const string& detalle = "") {
    string error = "Error anterior";
    bool resultado = validarMatriz(matriz, {"A", "B"}, tipo, ponderado, error);
    bool correcto = resultado == esperado &&
        (resultado ? error.empty() : !error.empty() && error.find(detalle) != string::npos);
    if (!correcto) {
        cerr << "Detalle: " << error << '\n';
    }
    return comprobar(caso, correcto);
}

int main() {
    int fallos = 0;
    double peso = 123;
    vector<pair<string, double>> validos = {
        {"0", 0}, {"-0", 0}, {"2.5", 2.5}, {"-.5", -.5}, {"+4", 4},
        {"1e9", 1e9}, {"-1e9", -1e9}, {"-1.25e2", -125}, {"1e-10", 1e-10}
    };
    for (const auto& caso : validos) {
        bool aceptado = interpretarPeso(caso.first, peso);
        fallos += comprobar("peso valido " + caso.first, aceptado && peso == caso.second);
    }

    vector<string> invalidos = {"", "x", "abc", "nan", "inf", "-inf", "1e309",
                               "1e-999", "1000000001", "-1000000001", "2,5", "2abc", "1 2"};
    for (const string& texto : invalidos) {
        peso = 123;
        fallos += comprobar("peso rechazado " + texto, !interpretarPeso(texto, peso) && peso == 123);
    }

    TipoGrafo noDirigido = TipoGrafo::NoDirigido;
    TipoGrafo dirigido = TipoGrafo::Dirigido;
    Conexion ausencia = {0, 0};
    Conexion cero = {1, 0};
    Conexion negativo = {1, -2.5};
    vector<vector<Conexion>> matriz = {{ausencia, cero}, {cero, ausencia}};

    fallos += revisarMatriz("cero no es ausencia", matriz, noDirigido, true, true);
    fallos += comprobar("camino de costo cero", esCamino({0, 1}, matriz));
    fallos += comprobar("retroceso no es ciclo no dirigido", !esCiclo({0, 1, 0}, matriz, noDirigido));
    fallos += comprobar("arcos opuestos forman ciclo dirigido", esCiclo({0, 1, 0}, matriz, dirigido));
    fallos += comprobar("formatos de cero y ausencia",
                         textoConexion(cero, true) == "0" && textoConexion(ausencia, true) == "x");
    fallos += revisarMatriz("cero incompatible con modo binario", matriz, noDirigido,
                            false, false, "Peso inconsistente");

    matriz[1][0] = ausencia;
    fallos += revisarMatriz("cero frente a ausencia asimetrica", matriz, noDirigido,
                            true, false, "hay 0, pero de 'B' a 'A' hay x");
    fallos += revisarMatriz("asimetria dirigida permitida", matriz, dirigido, true, true);
    fallos += comprobar("camino inverso inexistente", !esCamino({1, 0}, matriz));
    matriz[1][0] = negativo;
    fallos += revisarMatriz("pesos diferentes", matriz, noDirigido, true, false, "simetrico");
    matriz[0][1] = negativo;
    fallos += revisarMatriz("pesos negativos simetricos", matriz, noDirigido, true, true);

    matriz[0][0] = cero;
    fallos += revisarMatriz("lazo de costo cero", matriz, noDirigido, true, false, "lazo");
    matriz[0][0] = negativo;
    fallos += revisarMatriz("lazo negativo", matriz, dirigido, true, false, "lazo");
    matriz[0][0] = ausencia;
    matriz[0][1] = {0, 7};
    fallos += revisarMatriz("peso oculto sin arista", matriz, dirigido, true, false, "Peso inconsistente");

    for (double invalido : {numeric_limits<double>::infinity(),
                            numeric_limits<double>::quiet_NaN(), 1e10}) {
        matriz[0][1] = {1, invalido};
        fallos += revisarMatriz("peso no permitido en matriz", matriz, dirigido, true,
                                false, "Peso invalido en ['A']['B']");
    }
    matriz[1].clear();
    fallos += revisarMatriz("dimension antes de pesos", matriz, dirigido, true, false, "fila de 'B'");

    cout << "Fallos: " << fallos << '\n';
    return fallos == 0 ? 0 : 1;
}
