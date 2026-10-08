"""Pruebas de captura de matriz: python pruebas/probar_hu04.py ./algoritmo.exe."""

import argparse
from pathlib import Path

from probar_hu02 import ejecutar


def probar(executable):
    ejecutar(
        executable,
        ["2", "alpha", "Beta", "1", "1", "1", "texto", "-1", "0", "1",
         "1", "0", "2", "0"],
        ['Se detecto un lazo en el nodo "alpha"',
         'Se detecto un lazo en el nodo "Beta"',
         "Esta version usa grafos simples; ingrese 0.",
         "Ingrese un numero entre 0 y 1.",
         'E = {{"alpha","Beta"}}', "Programa finalizado."],
    )
    print("OK: lazos rechazados y corregidos en las dos diagonales no dirigidas.")

    ejecutar(
        executable,
        ["2", "inicio", "fin", "2", "1", "1", "0", "1", "0", "1", "0",
         "2", "3", "inicio", "0"],
        ['Se detecto un lazo en el nodo "inicio"',
         'Se detecto un lazo en el nodo "fin"',
         'A = {<"inicio","fin">}',
         "Sucesores de inicio: fin", "Predecesores de inicio: Ninguno",
         "Programa finalizado."],
    )
    print("OK: lazos corregidos y asimetria valida en grafo dirigido.")

    ejecutar(
        executable,
        ["3", "A", "B", "C", "1", "1", "0", "1", "0", "0", "1", "0", "1", "0"],
        ["     A  B  C\n  A  0  1  0\n  B  1  0  1\n  C  0  1  0\n",
         "Programa finalizado."],
    )
    print("OK: simetria automatica al capturar el triangulo superior.")

    ejecutar(
        executable, ["2", "A", "B", "1", "1", "1"],
        ['Se detecto un lazo en el nodo "A"', "Entrada finalizada. Programa cerrado."],
        codigo=1,
    )
    print("OK: cierre de entrada mientras se corrige un lazo.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("ejecutable", type=Path)
    opciones = parser.parse_args()
    ruta = opciones.ejecutable.resolve()
    if not ruta.is_file():
        parser.error(f"No existe el ejecutable: {ruta}. Compile primero algoritmo.cpp.")
    probar(ruta)
    print("Todas las pruebas de captura de HU-04 terminaron correctamente.")
