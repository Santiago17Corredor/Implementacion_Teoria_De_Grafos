"""Pruebas de caminos y ciclos: python pruebas/probar_caminos_ciclos.py ./algoritmo.exe."""

import argparse
from pathlib import Path

from probar_hu02 import ejecutar


def probar(executable):
    ejecutar(
        executable,
        ["4", "A", "B", "C", "D", "2", "1",
         "0", "1", "1", "0", "0", "0", "1", "1",
         "0", "0", "0", "1", "0", "0", "0", "0",
         "6", "A", "D", "6", "D", "A", "7", "0"],
        ["Caminos encontrados: 3",
         "Camino 1: A -> B -> C -> D\n  Longitud: 3 aristas. Camino simple: si.",
         "Camino 2: A -> B -> D\n  Longitud: 2 aristas. Camino simple: si.",
         "Camino 3: A -> C -> D\n  Longitud: 2 aristas. Camino simple: si.",
         'No existe camino posible entre "D" y "A".',
         "El grafo no contiene ciclos; es aciclico."],
    )
    print("OK: todos los caminos de un digrafo y ausencia de camino y ciclos.")

    ejecutar(
        executable,
        ["3", "A", "B", "C", "2", "2",
         "x", "1", "5", "2", "x", "-1", "4", "x", "x",
         "6", "A", "A", "7", "0"],
        ["El origen y el destino son el mismo nodo. Se buscaran ciclos simples.",
         "Ciclos encontrados: 3",
         "Ciclo 1: A -> B -> A\n  Longitud: 2 aristas. Costo: 3. Ciclo simple: si.",
         "Ciclo 2: A -> B -> C -> A\n  Longitud: 3 aristas. Costo: 4. Ciclo simple: si.",
         "Ciclo 3: A -> C -> A\n  Longitud: 2 aristas. Costo: 9. Ciclo simple: si.",
         "El grafo contiene ciclos.\nCiclo encontrado: A -> B -> A",
         "Longitud: 2 aristas. Costo: 3.\nClasificacion: ciclo simple."],
    )
    print("OK: ciclos dirigidos desde origen, costos y deteccion automatica.")

    ejecutar(
        executable,
        ["4", "A", "B", "C", "D", "1", "1",
         "0", "1", "1", "0", "0", "1", "0", "0", "0", "0",
         "6", "A", "A", "6", "D", "D", "7", "0"],
        ["Ciclos encontrados: 1",
         "Ciclo 1: A -> B -> C -> A",
         'No existen ciclos que inicien y terminen en "D".',
         "El grafo contiene ciclos.\nCiclo encontrado: A -> B -> C -> A"],
        prohibidos=["Ciclo 2:"],
    )
    print("OK: ciclo no dirigido sin duplicar su orientacion y nodo aislado.")

    ejecutar(
        executable,
        ["5", "A", "B", "C", "D", "E", "1", "1",
         "0", "1", "0", "0", "0",
         "0", "0", "0", "0",
         "0", "1", "1",
         "0", "1",
         "0", "7", "0"],
        ["El grafo contiene ciclos.\nCiclo encontrado: C -> D -> E -> C"],
    )
    print("OK: ciclo encontrado en un componente desconectado.")

    ejecutar(
        executable,
        ["2", "A", "B", "1", "1", "0", "1", "0",
         "6", "A", "A", "7", "0"],
        ['No existen ciclos que inicien y terminen en "A".',
         "El grafo no contiene ciclos; es aciclico."],
        prohibidos=["Ciclo 1:"],
    )
    print("OK: regresar por la misma arista no es ciclo no dirigido.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("ejecutable", type=Path)
    ruta = parser.parse_args().ejecutable.resolve()
    if not ruta.is_file():
        parser.error(f"No existe el ejecutable: {ruta}. Compile primero algoritmo.cpp.")
    probar(ruta)
    print("Todas las pruebas de caminos y ciclos terminaron correctamente.")
