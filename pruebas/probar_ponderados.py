"""Pruebas de pesos: python pruebas/probar_ponderados.py ./algoritmo.exe."""

import argparse
from pathlib import Path

from probar_hu02 import ejecutar


def probar(executable):
    matriz = "".join(
        "".join(f"{celda:>6}" for celda in fila) + "\n"
        for fila in (("", "A", "B", "C"), ("A", "x", "0", "x"),
                     ("B", "0", "x", "-2.5"), ("C", "x", "-2.5", "x"))
    )
    ejecutar(
        executable,
        ["3", "A", "B", "C", "1", "2", "x", "0", "x", "x", "-2.5", "x",
         "2", "3", "A", "4", "3", "A", "B", "C",
         "4", "2", "A", "C", "4", "3", "A", "B", "A", "5", "0"],
        ["Ponderacion: Ponderado", matriz, 'E = {("A","B",0), ("B","C",-2.5)}',
         "Adyacentes de A: B", "Longitud: 2 aristas.", "Costo total: -2.5",
         "Costo total: 0", "La secuencia no es un camino.", "No forma un ciclo."],
        dot_esperado=(
            'graph G {\n    rankdir=LR;\n    node [shape=circle];\n'
            '    n0 [label="A"];\n    n1 [label="B"];\n    n2 [label="C"];\n'
            '    n0 -- n1 [label="0"];\n    n1 -- n2 [label="-2.5"];\n}\n'
        ),
        prohibidos=["Tambien forma un ciclo"],
    )
    print("OK: cero distinto de ausencia, simetria, matriz, costo negativo y DOT.")

    ejecutar(
        executable,
        ["3", "A", "B", "C", "2", "2", "x", "0", "10", "x", "x", "-2.5",
         "4", "x", "x", "2", "4", "4", "A", "B", "C", "A",
         "4", "2", "C", "B", "5", "0"],
        ['A = {<"A","B",0>, <"A","C",10>, <"B","C",-2.5>, <"C","A",4>}',
         "Costo total: 1.5", "Tambien forma un ciclo simple.",
         "La secuencia no es un camino."],
        dot_esperado=(
            'digraph G {\n    rankdir=LR;\n    node [shape=circle];\n'
            '    n0 [label="A"];\n    n1 [label="B"];\n    n2 [label="C"];\n'
            '    n0 -> n1 [label="0"];\n    n0 -> n2 [label="10"];\n'
            '    n1 -> n2 [label="-2.5"];\n    n2 -> n0 [label="4"];\n}\n'
        ),
    )
    print("OK: direccion, tuplas, ciclo y etiquetas ponderadas.")

    invalidos = ["", "texto", "1 2", "nan", "inf", "-inf", "1e309", "1e-999",
                 "2,5", "1000000001", "-1000000001", "2abc", "x extra"]
    ejecutar(
        executable,
        ["2", "A", "B", "1", "error", "0", "3", "2", "0", "x"] + invalidos
        + ["  -1.25e+2  ", "0", "X", "2", "0"],
        ["Ingrese un numero entre 1 y 2.", 'Se detecto un lazo en el nodo "A"',
         'Se detecto un lazo en el nodo "B"', "ingrese x.",
         "Ingrese un solo peso o x.", "Peso invalido.", 'E = {("A","B",-125)}',
         "Programa finalizado."],
    )
    print("OK: opciones, lazos de costo cero, correccion de pesos y notacion cientifica.")

    ejecutar(
        executable,
        ["2", "A", "B", "2", "2", "x", "1e9", "-1e9", "x",
         "2", "4", "3", "A", "B", "A", "4", "1", "A", "0"],
        ['A = {<"A","B",1000000000>, <"B","A",-1000000000>}',
         "Costo total: 0", "Longitud: 0 aristas.", "Programa finalizado."],
    )
    print("OK: extremos del rango y secuencia de un solo nodo.")

    datos = ["26"] + [f"N{i}" for i in range(26)] + ["1", "2"] + ["x"] * 351
    ejecutar(executable, datos + ["2", "0"], ["Cantidad de nodos: 26", "E = {}"])
    print("OK: 26 nodos ponderados sin conexiones.")

    ejecutar(executable, ["2", "A", "B", "1", "2", "0"],
             ['Se detecto un lazo en el nodo "A"', "Entrada finalizada."], codigo=1)
    print("OK: fin de entrada durante correccion ponderada.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("ejecutable", type=Path)
    ruta = parser.parse_args().ejecutable.resolve()
    if not ruta.is_file():
        parser.error(f"No existe el ejecutable: {ruta}. Compile primero algoritmo.cpp.")
    probar(ruta)
    print("Todas las pruebas de pesos terminaron correctamente.")
