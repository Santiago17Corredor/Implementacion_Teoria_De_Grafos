"""Pruebas de la visualizacion: python pruebas/probar_interfaz.py."""

import json
import os
import sys
import tempfile
from pathlib import Path

os.environ.setdefault("MPLBACKEND", "Agg")
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from interfaz import calcular_posiciones, cargar_grafo, dibujar_grafo, validar_grafo


def exigir_error(caso, datos, detalle):
    try:
        validar_grafo(datos)
    except ValueError as error:
        if detalle not in str(error):
            raise AssertionError(f"{caso}: mensaje inesperado: {error}") from error
        print(f"OK: {caso}")
        return
    raise AssertionError(f"{caso}: debio fallar")


def probar():
    no_dirigido = {
        "dirigido": False,
        "ponderado": True,
        "nodos": ['A "central"', "B", "Nodo aislado"],
        "aristas": [{"origen": 0, "destino": 1, "peso": 0}],
    }
    dirigido = {
        "dirigido": True,
        "ponderado": True,
        "nodos": ["A", "B", "C"],
        "aristas": [
            {"origen": 0, "destino": 1, "peso": -2.5},
            {"origen": 1, "destino": 0, "peso": 4},
            {"origen": 1, "destino": 2, "peso": 0},
        ],
    }

    with tempfile.TemporaryDirectory(prefix="lab3_interfaz_", dir=Path.cwd()) as carpeta:
        carpeta = Path(carpeta)
        for numero, datos in enumerate((no_dirigido, dirigido), 1):
            entrada = carpeta / f"grafo_{numero}.json"
            salida = carpeta / f"grafo_{numero}.png"
            entrada.write_text(json.dumps(datos, ensure_ascii=False), encoding="utf-8")
            cargado = cargar_grafo(entrada)
            dibujar_grafo(cargado, salida, mostrar=False)
            if not salida.is_file() or salida.stat().st_size < 1000:
                raise AssertionError("No se genero una imagen util.")
        print("OK: carga UTF-8 y renderizado de ambos tipos, pesos y nodo aislado.")

    posiciones = calcular_posiciones(26)
    if len(posiciones) != 26 or len(set(posiciones)) != 26:
        raise AssertionError("La distribucion no ubica todos los nodos por separado.")
    print("OK: distribucion para el maximo de 26 nodos.")

    base = {"dirigido": False, "ponderado": False,
            "nodos": ["A", "B"], "aristas": []}
    exigir_error("contenido principal", [], "objeto JSON")
    exigir_error("tipos booleanos", {**base, "dirigido": 1}, "deben ser booleanos")
    exigir_error("minimo de nodos", {**base, "nodos": ["A"]}, "al menos dos")
    exigir_error("nombres repetidos", {**base, "nodos": ["A", "A"]}, "unicos")
    exigir_error("indice inexistente", {**base, "aristas": [{"origen": 0, "destino": 2}]},
                 "nodo inexistente")
    exigir_error("lazo", {**base, "aristas": [{"origen": 0, "destino": 0}]}, "lazo")
    exigir_error("arista no dirigida repetida", {
        **base, "aristas": [{"origen": 0, "destino": 1}, {"origen": 1, "destino": 0}]
    }, "repetida")
    exigir_error("falta peso", {**base, "ponderado": True,
                 "aristas": [{"origen": 0, "destino": 1}]}, "peso numerico")
    exigir_error("peso no finito", {**base, "ponderado": True,
                 "aristas": [{"origen": 0, "destino": 1, "peso": float("inf")}]},
                 "peso numerico")
    exigir_error("peso inesperado", {**base,
                 "aristas": [{"origen": 0, "destino": 1, "peso": 1}]},
                 "no debe incluir peso")


if __name__ == "__main__":
    probar()
    print("Todas las pruebas de la interfaz terminaron correctamente.")
