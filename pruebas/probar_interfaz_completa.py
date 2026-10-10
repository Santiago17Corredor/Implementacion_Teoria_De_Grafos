"""Pruebas de la integración de la interfaz con el motor nativo algoritmo.exe."""

import sys
import tempfile
from pathlib import Path


sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from interfaz import ejecutar_cpp, guardar_archivos_grafo


def probar():
    dirigido = {
        "dirigido": True,
        "ponderado": True,
        "nodos": ["A", "B", "C", "Aislado"],
        "aristas": [
            {"origen": 0, "destino": 1, "peso": 0},
            {"origen": 0, "destino": 2, "peso": 5},
            {"origen": 1, "destino": 2, "peso": -2.5},
            {"origen": 2, "destino": 0, "peso": 1},
        ],
    }

    # Opción 1: Matriz
    res_mat = ejecutar_cpp(dirigido, 1)
    assert res_mat is not None and "x" in res_mat

    # Opción 2: Representación formal
    res_rep = ejecutar_cpp(dirigido, 2)
    assert res_rep is not None and '<"A","B",0>' in res_rep

    # Opción 3: Adyacencias
    res_ady = ejecutar_cpp(dirigido, 3, ["A"])
    assert res_ady is not None and "Grado de salida: 2" in res_ady
    res_aislado = ejecutar_cpp(dirigido, 3, ["Aislado"])
    assert res_aislado is not None and "nodo aislado" in res_aislado

    # Opción 4: Secuencia
    res_sec = ejecutar_cpp(dirigido, 4, ["3", "A", "B", "C"])
    assert res_sec is not None and "Costo total: -2.5" in res_sec

    # Opción 6: Caminos
    res_cam = ejecutar_cpp(dirigido, 6, ["A", "C"])
    assert res_cam is not None and "Caminos encontrados: 2" in res_cam

    # Opción 7: Ciclos
    res_cic = ejecutar_cpp(dirigido, 7)
    assert res_cic is not None and "contiene ciclos" in res_cic
    print("OK: matriz, representación, grados, caminos, costos y ciclos dirigidos con algoritmo.exe.")

    no_dirigido = {
        "dirigido": False,
        "ponderado": False,
        "nodos": ["A", "B", "C"],
        "aristas": [
            {"origen": 0, "destino": 1},
            {"origen": 1, "destino": 2},
            {"origen": 0, "destino": 2},
        ],
    }
    res_cam_nd = ejecutar_cpp(no_dirigido, 6, ["A", "A"])
    assert res_cam_nd is not None and "Ciclos encontrados: 1" in res_cam_nd

    res_sec_ciclo = ejecutar_cpp(no_dirigido, 4, ["4", "A", "B", "C", "A"])
    assert res_sec_ciclo is not None and "ciclo simple" in res_sec_ciclo.lower()

    res_sec_no_ciclo = ejecutar_cpp(no_dirigido, 4, ["3", "A", "B", "A"])
    assert res_sec_no_ciclo is not None and "no forma un ciclo" in res_sec_no_ciclo.lower()
    print("OK: ciclos no dirigidos sin duplicar el regreso inmediato con algoritmo.exe.")

    with tempfile.TemporaryDirectory(prefix="lab3_gui_", dir=Path.cwd()) as temporal:
        ruta_json, ruta_dot = guardar_archivos_grafo(dirigido, temporal)
        assert ruta_json.stat().st_size > 100
        assert ruta_dot.stat().st_size > 100
    print("OK: exportación JSON y DOT desde la interfaz.")


if __name__ == "__main__":
    probar()
    print("Todas las pruebas de la interfaz completa terminaron correctamente.")
