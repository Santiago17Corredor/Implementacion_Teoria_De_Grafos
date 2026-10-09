"""Pruebas del motor integrado que respalda la interfaz grafica."""

import sys
import tempfile
from pathlib import Path


sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from interfaz import (
    buscar_caminos,
    buscar_un_ciclo,
    guardar_archivos_grafo,
    texto_adyacencias,
    texto_caminos,
    texto_ciclo,
    texto_matriz,
    texto_representacion,
    texto_secuencia,
)


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

    assert "x" in texto_matriz(dirigido)
    assert '<"A","B",0>' in texto_representacion(dirigido)
    assert "Grado de salida: 2" in texto_adyacencias(dirigido, 0)
    assert "nodo aislado" in texto_adyacencias(dirigido, 3)
    assert "Costo total: -2.5" in texto_secuencia(dirigido, [0, 1, 2])
    assert len(buscar_caminos(dirigido, 0, 2)) == 2
    assert "2 resultado" in texto_caminos(dirigido, 0, 2)
    assert buscar_un_ciclo(dirigido)
    assert "contiene al menos un ciclo" in texto_ciclo(dirigido)
    print("OK: matriz, representación, grados, caminos, costos y ciclos dirigidos.")

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
    assert len(buscar_caminos(no_dirigido, 0, 0)) == 1
    assert "ciclo" in texto_secuencia(no_dirigido, [0, 1, 2, 0]).lower()
    assert "no forma un ciclo" in texto_secuencia(no_dirigido, [0, 1, 0]).lower()
    print("OK: ciclos no dirigidos sin duplicar el regreso inmediato.")

    with tempfile.TemporaryDirectory(prefix="lab3_gui_", dir=Path.cwd()) as temporal:
        ruta_json, ruta_dot = guardar_archivos_grafo(dirigido, temporal)
        assert ruta_json.stat().st_size > 100
        assert ruta_dot.stat().st_size > 100
    print("OK: exportación JSON y DOT desde la interfaz.")


if __name__ == "__main__":
    probar()
    print("Todas las pruebas de la interfaz completa terminaron correctamente.")
