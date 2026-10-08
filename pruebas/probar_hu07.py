"""Pruebas de grados y aislamiento: python pruebas/probar_hu07.py ./algoritmo.exe."""

import argparse
from pathlib import Path

from probar_hu02 import ejecutar


def probar(executable):
    for ponderado in (False, True):
        ausencia = "x" if ponderado else "0"
        arista = "0" if ponderado else "1"
        modo = "2" if ponderado else "1"
        ejecutar(
            executable,
            ["3", "A", "B", "C", "1", modo,
             ausencia, arista, ausencia, ausencia, ausencia, ausencia,
             "3", "no existe", "A", "3", "C", "0"],
            ["El nodo no existe", "Adyacentes de A: B\nGrado: 1",
             'Adyacentes de C: Ninguno\nGrado: 0\nEl nodo "C" es un nodo aislado.'],
            prohibidos=['El nodo "A" es un nodo aislado.'],
        )
        ejecutar(
            executable,
            ["3", "A", "B", "C", "2", modo,
             ausencia, arista, ausencia, ausencia, ausencia, ausencia,
             ausencia, ausencia, ausencia, "3", "A", "3", "B", "3", "C", "0"],
            ["Sucesores de A: B\nPredecesores de A: Ninguno\n"
             "Grado de salida: 1\nGrado de entrada: 0",
             "Sucesores de B: Ninguno\nPredecesores de B: A\n"
             "Grado de salida: 0\nGrado de entrada: 1",
             'Grado de salida: 0\nGrado de entrada: 0\nEl nodo "C" es un nodo aislado.'],
            prohibidos=['El nodo "A" es un nodo aislado.', 'El nodo "B" es un nodo aislado.'],
        )
    print("OK: grados, entrada/salida, aislados y nombres invalidos en los cuatro tipos.")

    ejecutar(
        executable,
        ["3", "A", "B", "C", "1", "2", "x", "-4", "0", "x", "2.5", "x",
         "3", "A", "0"],
        ["Adyacentes de A: B, C\nGrado: 2"],
        prohibidos=["es un nodo aislado."],
    )
    print("OK: el grado cuenta aristas, no suma pesos.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("ejecutable", type=Path)
    ruta = parser.parse_args().ejecutable.resolve()
    if not ruta.is_file():
        parser.error(f"No existe el ejecutable: {ruta}. Compile primero algoritmo.cpp.")
    probar(ruta)
    print("Todas las pruebas de HU-07 terminaron correctamente.")
