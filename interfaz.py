"""Visualiza el grafo exportado por algoritmo.cpp."""

import argparse
import json
import math
import sys
from pathlib import Path


def cargar_grafo(ruta):
    try:
        with ruta.open(encoding="utf-8") as archivo:
            datos = json.load(archivo)
    except OSError as error:
        raise ValueError(f"No se pudo abrir {ruta}: {error}") from error
    except json.JSONDecodeError as error:
        raise ValueError(f"El archivo no contiene JSON valido: {error}") from error

    validar_grafo(datos)
    return datos


def validar_grafo(datos):
    if not isinstance(datos, dict):
        raise ValueError("El contenido principal debe ser un objeto JSON.")
    if type(datos.get("dirigido")) is not bool or type(datos.get("ponderado")) is not bool:
        raise ValueError("Los campos dirigido y ponderado deben ser booleanos.")

    nodos = datos.get("nodos")
    aristas = datos.get("aristas")
    if not isinstance(nodos, list) or len(nodos) < 2:
        raise ValueError("Se requieren al menos dos nodos.")
    if any(not isinstance(nombre, str) or not nombre.strip() for nombre in nodos):
        raise ValueError("Cada nodo debe tener un nombre no vacio.")
    if len(set(nodos)) != len(nodos):
        raise ValueError("Los nombres de los nodos deben ser unicos.")
    if not isinstance(aristas, list):
        raise ValueError("El campo aristas debe ser una lista.")

    repetidas = set()
    for numero, arista in enumerate(aristas, 1):
        if not isinstance(arista, dict):
            raise ValueError(f"La arista {numero} debe ser un objeto.")
        origen = arista.get("origen")
        destino = arista.get("destino")
        if type(origen) is not int or type(destino) is not int:
            raise ValueError(f"La arista {numero} debe usar indices enteros.")
        if not (0 <= origen < len(nodos) and 0 <= destino < len(nodos)):
            raise ValueError(f"La arista {numero} referencia un nodo inexistente.")
        if origen == destino:
            raise ValueError(f"La arista {numero} es un lazo no permitido.")

        clave = (origen, destino) if datos["dirigido"] else tuple(sorted((origen, destino)))
        if clave in repetidas:
            raise ValueError(f"La arista {numero} esta repetida.")
        repetidas.add(clave)

        if datos["ponderado"]:
            peso = arista.get("peso")
            if type(peso) not in (int, float) or not math.isfinite(peso):
                raise ValueError(f"La arista {numero} necesita un peso numerico finito.")
        elif "peso" in arista:
            raise ValueError(f"La arista {numero} no debe incluir peso en este grafo.")


def calcular_posiciones(cantidad):
    if cantidad == 2:
        return [(-1.0, 0.0), (1.0, 0.0)]
    return [
        (math.cos(math.pi / 2 - 2 * math.pi * i / cantidad),
         math.sin(math.pi / 2 - 2 * math.pi * i / cantidad))
        for i in range(cantidad)
    ]


def dibujar_grafo(datos, guardar=None, mostrar=True):
    try:
        import matplotlib.pyplot as plt
        from matplotlib.patches import FancyArrowPatch
    except ImportError as error:
        raise ValueError(
            "Falta Matplotlib. Instale las dependencias con: pip install -r requirements.txt"
        ) from error

    nodos = datos["nodos"]
    posiciones = calcular_posiciones(len(nodos))
    lado = min(13, max(7, 6 + len(nodos) * 0.23))
    figura, eje = plt.subplots(figsize=(lado, lado))
    eje.set_aspect("equal")
    eje.axis("off")
    if hasattr(figura.canvas.manager, "set_window_title"):
        figura.canvas.manager.set_window_title("Laboratorio 3 - Visualizacion del grafo")

    fuente = max(6, min(11, 13 - len(nodos) // 4))
    pares = {(a["origen"], a["destino"]) for a in datos["aristas"]}

    for arista in datos["aristas"]:
        origen = arista["origen"]
        destino = arista["destino"]
        x1, y1 = posiciones[origen]
        x2, y2 = posiciones[destino]
        curva = 0
        if datos["dirigido"] and (destino, origen) in pares:
            curva = 0.17 if origen < destino else -0.17

        flecha = FancyArrowPatch(
            (x1, y1), (x2, y2),
            arrowstyle="-|>" if datos["dirigido"] else "-",
            connectionstyle=f"arc3,rad={curva}",
            mutation_scale=17,
            linewidth=1.7,
            color="#52606d",
            shrinkA=min(58, 17 + len(nodos[origen]) * 2.2),
            shrinkB=min(58, 17 + len(nodos[destino]) * 2.2),
            zorder=1,
        )
        eje.add_patch(flecha)

        if datos["ponderado"]:
            medio_x = (x1 + x2) / 2
            medio_y = (y1 + y2) / 2
            distancia = math.hypot(x2 - x1, y2 - y1)
            if distancia:
                desplazamiento = 0.07 + abs(curva) * 0.35
                medio_x += -(y2 - y1) / distancia * desplazamiento
                medio_y += (x2 - x1) / distancia * desplazamiento
            eje.text(
                medio_x, medio_y, f'{arista["peso"]:g}',
                ha="center", va="center", fontsize=max(7, fuente - 1),
                color="#7b341e", zorder=4,
                bbox={"boxstyle": "round,pad=0.18", "fc": "#fffaf0", "ec": "none"},
            )

    for indice, nombre in enumerate(nodos):
        eje.text(*posiciones[indice], nombre, ha="center", va="center",
                 fontsize=fuente, color="white", fontweight="bold", zorder=4,
                 bbox={"boxstyle": "round,pad=0.55", "fc": "#2b6cb0",
                       "ec": "#1a365d", "lw": 1.6})

    tipo = "dirigido" if datos["dirigido"] else "no dirigido"
    pesos = "ponderado" if datos["ponderado"] else "no ponderado"
    eje.set_title(f"Grafo {tipo} y {pesos}", fontsize=15, pad=18)
    margen = 1.38
    eje.set_xlim(-margen, margen)
    eje.set_ylim(-margen, margen)
    figura.tight_layout()

    if guardar is not None:
        figura.savefig(guardar, dpi=180, bbox_inches="tight")
    if mostrar:
        plt.show()
    else:
        plt.close(figura)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("archivo", nargs="?", type=Path, default=Path("grafo.json"))
    parser.add_argument("--guardar", type=Path, help="Guarda la grafica como PNG, SVG o PDF")
    parser.add_argument("--sin-mostrar", action="store_true", help="No abre la ventana grafica")
    opciones = parser.parse_args()

    try:
        datos = cargar_grafo(opciones.archivo)
        if opciones.sin_mostrar and opciones.guardar is None:
            raise ValueError("--sin-mostrar requiere tambien --guardar.")
        dibujar_grafo(datos, opciones.guardar, not opciones.sin_mostrar)
    except ValueError as error:
        print(f"Error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
