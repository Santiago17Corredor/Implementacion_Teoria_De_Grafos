#define main mainLaboratorio
#include "../algoritmo.cpp"
#undef main

int comprobar(const string& caso, bool condicion) {
    cout << (condicion ? "OK: " : "FALLO: ") << caso << '\n';
    return condicion ? 0 : 1;
}

vector<vector<Conexion>> crearMatriz(int cantidad) {
    return vector<vector<Conexion>>(cantidad, vector<Conexion>(cantidad));
}

void conectar(vector<vector<Conexion>>& matriz, int origen, int destino,
              double peso = 1, bool dirigido = true) {
    matriz[origen][destino] = {1, peso};
    if (!dirigido) {
        matriz[destino][origen] = {1, peso};
    }
}

int main() {
    int fallos = 0;
    vector<vector<Conexion>> dirigido = crearMatriz(4);
    conectar(dirigido, 0, 1);
    conectar(dirigido, 0, 2);
    conectar(dirigido, 1, 2);
    conectar(dirigido, 1, 3);
    conectar(dirigido, 2, 3);

    vector<vector<int>> caminos = buscarCaminosSimples(
        0, 3, dirigido, TipoGrafo::Dirigido);
    fallos += comprobar("tres caminos dirigidos", caminos == vector<vector<int>>{
        {0, 1, 2, 3}, {0, 1, 3}, {0, 2, 3}});
    fallos += comprobar("destino inalcanzable", buscarCaminosSimples(
        3, 0, dirigido, TipoGrafo::Dirigido).empty());
    fallos += comprobar("grafo dirigido aciclico",
        buscarUnCiclo(dirigido, TipoGrafo::Dirigido).empty());

    conectar(dirigido, 3, 0, -2);
    vector<int> cicloDirigido = buscarUnCiclo(dirigido, TipoGrafo::Dirigido);
    fallos += comprobar("ciclo dirigido encontrado",
        esCiclo(cicloDirigido, dirigido, TipoGrafo::Dirigido));

    vector<vector<Conexion>> triangulo = crearMatriz(4);
    conectar(triangulo, 0, 1, 0, false);
    conectar(triangulo, 0, 2, 2, false);
    conectar(triangulo, 1, 2, -1, false);
    vector<vector<int>> ciclos = buscarCaminosSimples(
        0, 0, triangulo, TipoGrafo::NoDirigido);
    bool cicloEsperado = ciclos == vector<vector<int>>{{0, 1, 2, 0}};
    fallos += comprobar("un ciclo no dirigido sin orientacion repetida", cicloEsperado);
    fallos += comprobar("costo del ciclo con peso cero",
        !ciclos.empty() && calcularCosto(ciclos[0], triangulo) == 1);
    fallos += comprobar("origen aislado no produce recorrido vacio",
        buscarCaminosSimples(3, 3, triangulo, TipoGrafo::NoDirigido).empty());

    vector<vector<Conexion>> unaArista = crearMatriz(2);
    conectar(unaArista, 0, 1, 1, false);
    fallos += comprobar("ida y vuelta no es ciclo no dirigido",
        buscarUnCiclo(unaArista, TipoGrafo::NoDirigido).empty() &&
        buscarCaminosSimples(0, 0, unaArista, TipoGrafo::NoDirigido).empty());

    vector<vector<Conexion>> desconectado = crearMatriz(5);
    conectar(desconectado, 0, 1, 1, false);
    conectar(desconectado, 2, 3, 1, false);
    conectar(desconectado, 2, 4, 1, false);
    conectar(desconectado, 3, 4, 1, false);
    vector<int> cicloDesconectado = buscarUnCiclo(
        desconectado, TipoGrafo::NoDirigido);
    fallos += comprobar("ciclo en componente desconectado",
        cicloDesconectado == vector<int>({2, 3, 4, 2}));

    cout << "Fallos: " << fallos << '\n';
    return fallos == 0 ? 0 : 1;
}
