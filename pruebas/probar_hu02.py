"""Pruebas de la consola: python pruebas/probar_hu02.py ./algoritmo.exe."""

import argparse
import subprocess
import tempfile
from pathlib import Path


def ejecutar(executable, datos, esperados, codigo=0, dot_esperado=None,
             json_esperado=None, prohibidos=()):
    with tempfile.TemporaryDirectory(prefix="lab3_hu02_", dir=Path.cwd()) as carpeta:
        resultado = subprocess.run(
            [str(executable)],
            input="\n".join(datos) + ("\n" if datos else ""),
            capture_output=True,
            text=True,
            encoding="utf-8",
            cwd=carpeta,
            timeout=5,
        )

        if resultado.returncode != codigo:
            raise AssertionError(
                f"Codigo esperado: {codigo}; recibido: {resultado.returncode}.\n"
                + resultado.stdout + resultado.stderr
            )

        salida = resultado.stdout + resultado.stderr
        for texto in esperados:
            if texto not in salida:
                raise AssertionError(f"No aparece {texto!r} en la salida:\n{salida}")

        for texto in prohibidos:
            if texto in salida:
                raise AssertionError(f"Aparece un texto incorrecto {texto!r}:\n{salida}")

        if dot_esperado is not None:
            contenido = (Path(carpeta) / "grafo.dot").read_text(encoding="utf-8")
            if contenido != dot_esperado:
                raise AssertionError(f"DOT inesperado:\n{contenido}")

        if json_esperado is not None:
            contenido = (Path(carpeta) / "grafo.json").read_text(encoding="utf-8")
            if contenido != json_esperado:
                raise AssertionError(f"JSON inesperado:\n{contenido}")


def probar(executable):
    ejecutar(
        executable,
        ["texto", "1", "0", "-1", "27", "2", "", "   ", "  v1  ",
         "v1", "15", "1", "1", "0", "error", "2", "1", "0", "99", "2", "3",
         "inexistente", "v1", "4", "2", "v1", "15", "0"],
        ["El grafo requiere al menos 2 nodos", "Ingrese un numero entre 2 y 26",
         "El nombre del nodo no puede estar vacio", "El nombre ya pertenece",
         'V = {"v1", "15"}', 'E = {{"v1","15"}}',
         '      v1  15\n  v1   0   1\n  15   1   0\n',
         "El nodo no existe", "Adyacentes de v1: 15",
         "Longitud: 1 aristas.", "No forma un ciclo.", "Programa finalizado."],
    )
    print("OK: limites inferiores, datos invalidos, duplicados y consultas.")

    ejecutar(
        executable,
        ["2", "A", "a", "2", "1", "0", "1", "0", "0", "2", "3", "A", "4", "2",
         "A", "a", "4", "2", "a", "A", "0"],
        ['V = {"A", "a"}', 'A = {<"A","a">}',
         "Sucesores de A: a", "Predecesores de A: Ninguno",
         "La secuencia es un camino simple", "La secuencia no es un camino."],
    )
    print("OK: mayusculas, minusculas y direccion de las conexiones.")

    ejecutar(
        executable,
        ["3", "Nodo central", "@", "v1", "1", "1", "0", "1", "1", "0", "1", "0",
         "4", "4", "Nodo central", "@", "v1", "Nodo central", "0"],
        ["Longitud: 3 aristas.", "Tambien forma un ciclo simple."],
    )
    print("OK: camino y ciclo con nombres compuestos y simbolos.")

    ejecutar(
        executable,
        ["2", "nombre\tconcontrol", "v1", "v2", "1", "1", "0", "0", "0", "0"],
        ["El nombre no puede contener caracteres de control", "Programa finalizado."],
    )
    print("OK: rechazo de caracteres de control internos.")

    datos = ["26"] + [f"N{i}" for i in range(26)] + ["1", "1"] + ["0"] * 351 + ["0"]
    ejecutar(executable, datos, ["Cantidad de nodos: 26", "Programa finalizado."])
    print("OK: limite superior de 26 nodos.")

    ejecutar(
        executable,
        ["3", 'A "B"', "C:\\ruta", "Bogotá", "1", "1", "0", "1", "0", "0", "0", "0", "5", "0"],
        ["Archivo grafo.dot generado correctamente."],
        dot_esperado=(
            'graph G {\n    rankdir=LR;\n    node [shape=circle];\n'
            '    n0 [label="A \\"B\\""];\n'
            '    n1 [label="C:\\\\ruta"];\n'
            '    n2 [label="Bogotá"];\n'
            '    n0 -- n1;\n}\n'
        ),
        json_esperado=(
            '{\n  "dirigido": false,\n  "ponderado": false,\n'
            '  "nodos": ["A \\"B\\"", "C:\\\\ruta", "Bogotá"],\n'
            '  "aristas": [\n    {"origen": 0, "destino": 1}\n  ]\n}\n'
        ),
    )
    print("OK: DOT con comillas, barras invertidas, acentos y nodo aislado.")

    ejecutar(
        executable,
        ["2", "graph", "x,y", "2", "1", "0", "1", "0", "0", "5", "0"],
        ["Archivo grafo.dot generado correctamente."],
        dot_esperado=(
            'digraph G {\n    rankdir=LR;\n    node [shape=circle];\n'
            '    n0 [label="graph"];\n    n1 [label="x,y"];\n'
            '    n0 -> n1;\n}\n'
        ),
        json_esperado=(
            '{\n  "dirigido": true,\n  "ponderado": false,\n'
            '  "nodos": ["graph", "x,y"],\n'
            '  "aristas": [\n    {"origen": 0, "destino": 1}\n  ]\n}\n'
        ),
    )
    print("OK: DOT dirigido con palabras reservadas y puntuacion.")

    for datos in ([], ["2"], ["2", "a", "b", "1"], ["2", "a", "b", "1", "1"],
                  ["2", "a", "b", "1", "1", "0", "0", "0"]):
        ejecutar(executable, datos, ["Entrada finalizada. Programa cerrado."], codigo=1)
    print("OK: fin de entrada durante cantidad, nombres, matriz y menu.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("ejecutable", type=Path, help="Ruta al programa C++ ya compilado")
    opciones = parser.parse_args()
    ruta = opciones.ejecutable.resolve()
    if not ruta.is_file():
        parser.error(f"No existe el ejecutable: {ruta}. Compile primero algoritmo.cpp.")
    probar(ruta)
    print("Todas las pruebas de HU-02 terminaron correctamente.")
