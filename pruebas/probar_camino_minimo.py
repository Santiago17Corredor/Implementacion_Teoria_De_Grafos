"""Pruebas de HU-10, HU-11, HU-12 y HU-13 ejecutadas directamente sobre algoritmo.exe."""

import os
import sys
from pathlib import Path

os.environ.setdefault("MPLBACKEND", "Agg")
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import interfaz
from interfaz import (
    MENSAJE_ERROR_CPP,
    crear_figura_grafo,
    dibujar_grafo,
    ejecutar_cpp,
)


def probar_dijkstra_y_bellman_ford():
    # Grafo ponderado dirigido con pesos no negativos
    grafo_estandar = {
        "dirigido": True,
        "ponderado": True,
        "nodos": ["A", "B", "C", "D"],
        "aristas": [
            {"origen": 0, "destino": 1, "peso": 4},
            {"origen": 0, "destino": 2, "peso": 2},
            {"origen": 2, "destino": 1, "peso": 1},
            {"origen": 1, "destino": 3, "peso": 5},
            {"origen": 2, "destino": 3, "peso": 8},
        ],
    }

    # Dijkstra en C++ (Opción 8)
    salida_dij = ejecutar_cpp(grafo_estandar, 8, ["A", "D"])
    assert salida_dij is not None, "algoritmo.exe no produjo salida para Dijkstra"
    assert "Ruta: A -> C -> B -> D" in salida_dij
    assert "Costo total: 8" in salida_dij
    print("OK: Dijkstra en C++ calcula la ruta minima correcta en grafo no negativo.")

    # Bellman-Ford en C++ (Opción 9)
    salida_bf = ejecutar_cpp(grafo_estandar, 9, ["A", "D"])
    assert salida_bf is not None, "algoritmo.exe no produjo salida para Bellman-Ford"
    assert "Ruta: A -> C -> B -> D" in salida_bf
    assert "Costo total: 8" in salida_bf
    print("OK: Bellman-Ford en C++ coincide con Dijkstra en grafo no negativo.")

    # Benchmark en C++ (Opción 10)
    bench_txt = ejecutar_cpp(grafo_estandar, 10, ["A", "D"])
    assert bench_txt is not None, "algoritmo.exe no produjo salida para Benchmark"
    assert "Dijkstra" in bench_txt
    assert "Bellman-Ford" in bench_txt
    assert "Consistencia: Ambos algoritmos produjeron exactamente el mismo costo y ruta." in bench_txt
    print("OK: Benchmark en C++ compara y valida consistencia de Dijkstra y Bellman-Ford.")


def probar_pesos_negativos():
    # Grafo con arista de peso negativo sin ciclos negativos
    grafo_negativo = {
        "dirigido": True,
        "ponderado": True,
        "nodos": ["A", "B", "C"],
        "aristas": [
            {"origen": 0, "destino": 1, "peso": 5},
            {"origen": 0, "destino": 2, "peso": 7},
            {"origen": 2, "destino": 1, "peso": -4},
        ],
    }

    # Dijkstra en C++ debe rechazar aristas negativas
    salida_dij = ejecutar_cpp(grafo_negativo, 8, ["A", "B"])
    assert salida_dij is not None
    assert "Error: El grafo contiene aristas con peso negativo" in salida_dij
    print("OK: Dijkstra en C++ rechaza adecuadamente grafos con aristas negativas.")

    # Bellman-Ford en C++ debe procesarlo y encontrar A -> C -> B (costo 7 + -4 = 3 < 5)
    salida_bf = ejecutar_cpp(grafo_negativo, 9, ["A", "B"])
    assert salida_bf is not None
    assert "Ruta: A -> C -> B" in salida_bf
    assert "Costo total: 3" in salida_bf
    print("OK: Bellman-Ford en C++ procesa aristas negativas y optimiza la ruta.")

    # Benchmark en C++ con arista negativa
    bench_txt = ejecutar_cpp(grafo_negativo, 10, ["A", "B"])
    assert bench_txt is not None
    assert "Error (peso negativo)" in bench_txt
    assert "Dijkstra no es aplicable debido a aristas con peso negativo" in bench_txt
    print("OK: Benchmark en C++ reporta que Dijkstra no es aplicable ante pesos negativos.")


def probar_ciclo_negativo():
    # Grafo con ciclo negativo alcanzable: A -> B (-5), B -> A (2)
    grafo_ciclo_negativo = {
        "dirigido": True,
        "ponderado": True,
        "nodos": ["A", "B"],
        "aristas": [
            {"origen": 0, "destino": 1, "peso": -5},
            {"origen": 1, "destino": 0, "peso": 2},
        ],
    }

    salida_bf = ejecutar_cpp(grafo_ciclo_negativo, 9, ["A", "B"])
    assert salida_bf is not None
    assert "ALERTA: Se detecto un ciclo de costo negativo alcanzable." in salida_bf
    print("OK: Bellman-Ford en C++ detecta ciclo negativo en la iteración |V|.")


def probar_visualizador_resaltado():
    grafo = {
        "dirigido": True,
        "ponderado": True,
        "nodos": ["A", "B", "C"],
        "aristas": [
            {"origen": 0, "destino": 1, "peso": 2},
            {"origen": 1, "destino": 2, "peso": 3},
        ],
    }

    figura = crear_figura_grafo(grafo, camino_resaltado=[0, 1, 2])
    assert figura is not None
    assert len(figura.axes) == 1
    titulo = figura.axes[0].get_title()
    assert "Ruta mínima: A → B → C" in titulo
    print("OK: Visualizador Matplotlib resalta el camino y añade título correspondiente.")


def probar_error_binario_no_disponible():
    grafo = {
        "dirigido": True,
        "ponderado": False,
        "nodos": ["A", "B"],
        "aristas": [{"origen": 0, "destino": 1}],
    }
    resultado = ejecutar_cpp(grafo, 1, exe_ruta="binario_inexistente.exe")
    assert resultado is None, "ejecutar_cpp debió retornar None ante binario inexistente"
    assert "ERROR: El motor de cálculo en C++ (algoritmo.exe) no está disponible" in MENSAJE_ERROR_CPP
    print("OK: Ausencia del binario C++ retorna None y activa el mensaje de error explícito.")


def probar_ausencia_fallback_python():
    nombres_prohibidos = [
        "dijkstra_python",
        "dijkstra_respaldo",
        "bellman_ford_python",
        "bellman_ford_respaldo",
        "benchmark_python",
        "benchmark_respaldo",
        "buscar_caminos",
        "buscar_un_ciclo",
        "texto_matriz",
        "texto_representacion",
        "texto_adyacencias",
        "texto_caminos",
        "texto_ciclo",
        "texto_secuencia",
    ]
    for nombre in nombres_prohibidos:
        assert not hasattr(interfaz, nombre), f"El módulo interfaz no debe contener '{nombre}' (sin fallback)."
    print("OK: Confirmada la eliminación total del motor de respaldo en Python.")


if __name__ == "__main__":
    probar_dijkstra_y_bellman_ford()
    probar_pesos_negativos()
    probar_ciclo_negativo()
    probar_visualizador_resaltado()
    probar_error_binario_no_disponible()
    probar_ausencia_fallback_python()
    print("\nTODAS LAS PRUEBAS DE CAMINO MÍNIMO (HU-10 A HU-13) PASARON CON ÉXITO CON ALGORITMO.EXE.")
