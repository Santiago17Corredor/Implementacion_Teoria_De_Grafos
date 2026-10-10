"""Interfaz grafica del Laboratorio 3 de teoria de grafos."""

import argparse
import json
import math
import os
import shutil
import subprocess
import sys
from pathlib import Path


BASE_DIR = Path(__file__).resolve().parent
LIMITE_PESO = 1_000_000_000


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
    if len(nodos) > 26:
        raise ValueError("Se permiten como máximo 26 nodos.")
    if any(not isinstance(nombre, str) or not nombre.strip() for nombre in nodos):
        raise ValueError("Cada nodo debe tener un nombre no vacio.")
    if any(any(ord(caracter) < 32 for caracter in nombre) for nombre in nodos):
        raise ValueError("Los nombres no pueden contener caracteres de control.")
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
            if (type(peso) not in (int, float) or not math.isfinite(peso)
                    or abs(peso) > LIMITE_PESO):
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


def _dibujar_en_eje(datos, figura, eje, camino_resaltado=None):
    from matplotlib.patches import FancyArrowPatch

    nodos = datos["nodos"]
    posiciones = calcular_posiciones(len(nodos))
    fuente = max(6, min(11, 13 - len(nodos) // 4))
    pares = {(a["origen"], a["destino"]) for a in datos["aristas"]}
    eje.set_aspect("equal")
    eje.axis("off")

    aristas_camino = set()
    if camino_resaltado and len(camino_resaltado) > 1:
        for u, v in zip(camino_resaltado, camino_resaltado[1:]):
            aristas_camino.add((u, v))
            if not datos["dirigido"]:
                aristas_camino.add((v, u))

    for arista in datos["aristas"]:
        origen = arista["origen"]
        destino = arista["destino"]
        x1, y1 = posiciones[origen]
        x2, y2 = posiciones[destino]
        curva = 0
        if datos["dirigido"] and (destino, origen) in pares:
            curva = 0.17 if origen < destino else -0.17

        es_del_camino = (origen, destino) in aristas_camino or (
            not datos["dirigido"] and (destino, origen) in aristas_camino
        )

        if es_del_camino:
            color_arista = "#d90429"
            ancho_linea = 3.2
            escala_mutacion = 18
            orden_z = 3
            alpha_arista = 1.0
        elif camino_resaltado:
            color_arista = "#8d99ae"
            ancho_linea = 1.2
            escala_mutacion = 14
            orden_z = 1
            alpha_arista = 0.6
        else:
            color_arista = "#52606d"
            ancho_linea = 1.7
            escala_mutacion = 17
            orden_z = 1
            alpha_arista = 1.0

        flecha = FancyArrowPatch(
            (x1, y1), (x2, y2),
            arrowstyle="-|>" if datos["dirigido"] else "-",
            connectionstyle=f"arc3,rad={curva}", mutation_scale=escala_mutacion,
            linewidth=ancho_linea, color=color_arista, alpha=alpha_arista,
            shrinkA=min(58, 17 + len(nodos[origen]) * 2.2),
            shrinkB=min(58, 17 + len(nodos[destino]) * 2.2), zorder=orden_z,
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
            color_texto = "#d90429" if es_del_camino else "#7b341e"
            borde_caja = "#d90429" if es_del_camino else "none"
            eje.text(
                medio_x, medio_y, f'{arista["peso"]:g}', ha="center", va="center",
                fontsize=max(7, fuente - 1), color=color_texto, zorder=orden_z + 3,
                fontweight="bold" if es_del_camino else "normal",
                bbox={"boxstyle": "round,pad=0.18", "fc": "#fffaf0", "ec": borde_caja,
                      "lw": 1.2 if es_del_camino else 0},
            )

    nodos_camino = set(camino_resaltado) if camino_resaltado else set()
    for indice, nombre in enumerate(nodos):
        if indice in nodos_camino:
            fc = "#ffb703"
            ec = "#023047"
            tc = "#000000"
            lw = 2.0
        else:
            fc = "#2b6cb0"
            ec = "#1a365d"
            tc = "white"
            lw = 1.6
        eje.text(
            *posiciones[indice], nombre, ha="center", va="center", fontsize=fuente,
            color=tc, fontweight="bold", zorder=5,
            bbox={"boxstyle": "round,pad=0.55", "fc": fc, "ec": ec, "lw": lw},
        )

    tipo = "dirigido" if datos["dirigido"] else "no dirigido"
    pesos = "ponderado" if datos["ponderado"] else "no ponderado"
    titulo = f"Grafo {tipo} y {pesos}"
    if camino_resaltado and len(camino_resaltado) > 1:
        nombres_resaltados = " → ".join(nodos[i] for i in camino_resaltado)
        titulo += f"\nRuta mínima: {nombres_resaltados}"
    eje.set_title(titulo, fontsize=13 if camino_resaltado else 15, pad=18, color="#17324d")
    eje.set_xlim(-1.38, 1.38)
    eje.set_ylim(-1.38, 1.38)
    figura.tight_layout()


def crear_figura_grafo(datos, tamano=None, camino_resaltado=None):
    from matplotlib.figure import Figure

    validar_grafo(datos)
    lado = min(13, max(7, 6 + len(datos["nodos"]) * 0.23))
    figura = Figure(figsize=tamano or (lado, lado), dpi=100)
    _dibujar_en_eje(datos, figura, figura.add_subplot(111), camino_resaltado=camino_resaltado)
    return figura


def dibujar_grafo(datos, guardar=None, mostrar=True, camino_resaltado=None):
    validar_grafo(datos)
    if mostrar:
        import matplotlib.pyplot as plt

        lado = min(13, max(7, 6 + len(datos["nodos"]) * 0.23))
        figura, eje = plt.subplots(figsize=(lado, lado))
        _dibujar_en_eje(datos, figura, eje, camino_resaltado=camino_resaltado)
        if hasattr(figura.canvas.manager, "set_window_title"):
            figura.canvas.manager.set_window_title("Laboratorio 3 - Visualizacion del grafo")
        if guardar is not None:
            figura.savefig(guardar, dpi=180, bbox_inches="tight")
        plt.show()
        return

    figura = crear_figura_grafo(datos, camino_resaltado=camino_resaltado)
    if guardar is not None:
        figura.savefig(guardar, dpi=180, bbox_inches="tight")


def matriz_desde_datos(datos):
    validar_grafo(datos)
    cantidad = len(datos["nodos"])
    matriz = [[None for _ in range(cantidad)] for _ in range(cantidad)]
    for arista in datos["aristas"]:
        peso = arista["peso"] if datos["ponderado"] else 1
        matriz[arista["origen"]][arista["destino"]] = peso
        if not datos["dirigido"]:
            matriz[arista["destino"]][arista["origen"]] = peso
    return matriz


def formatear_numero(numero):
    return "0" if numero == 0 else format(numero, ".15g")
MENSAJE_ERROR_CPP = (
    "ERROR: El motor de cálculo en C++ (algoritmo.exe) no está disponible o falló la ejecución. "
    "Asegúrese de tener un compilador C++17 compatible o el binario compilado."
)


def guardar_archivos_grafo(datos, carpeta):
    validar_grafo(datos)
    carpeta = Path(carpeta)
    carpeta.mkdir(parents=True, exist_ok=True)
    ruta_json = carpeta / "grafo.json"
    ruta_dot = carpeta / "grafo.dot"
    ruta_json.write_text(json.dumps(datos, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    dirigido = datos["dirigido"]
    lineas = ["digraph G {" if dirigido else "graph G {", "  node [shape=circle];"]
    for indice, nombre in enumerate(datos["nodos"]):
        lineas.append(f"  n{indice} [label={json.dumps(nombre, ensure_ascii=False)}];")
    operador = "->" if dirigido else "--"
    for arista in datos["aristas"]:
        linea = f"  n{arista['origen']} {operador} n{arista['destino']}"
        if datos["ponderado"]:
            linea += f" [label={json.dumps(formatear_numero(arista['peso']))}]"
        lineas.append(linea + ";")
    lineas.append("}")
    ruta_dot.write_text("\n".join(lineas) + "\n", encoding="utf-8")
    return ruta_json, ruta_dot


def generar_entrada_cpp(datos, opcion, adicionales=None):
    """Construye el guion en texto plano que espera el stdin de algoritmo.cpp."""
    if adicionales is None:
        adicionales = []
    matriz = matriz_desde_datos(datos)
    lineas = [
        str(len(datos["nodos"])),
        *datos["nodos"],
        "2" if datos["dirigido"] else "1",
        "2" if datos["ponderado"] else "1",
    ]
    for fila in range(len(matriz)):
        inicio = 0 if datos["dirigido"] else fila
        for columna in range(inicio, len(matriz)):
            valor = matriz[fila][columna]
            if datos["ponderado"]:
                lineas.append("x" if valor is None else formatear_numero(valor))
            else:
                lineas.append("0" if valor is None else "1")
    lineas.extend([str(opcion), *adicionales, "0"])
    return "\n".join(lineas) + "\n"


def ejecutar_cpp(datos, opcion, adicionales=None, exe_ruta=None):
    """Ejecuta una consulta sobre algoritmo.exe y recorta el bloque de salida resultante."""
    exe = Path(exe_ruta) if exe_ruta else (BASE_DIR / "algoritmo.exe")
    if not exe.is_file():
        return None
    try:
        entrada = generar_entrada_cpp(datos, opcion, adicionales)
        proceso = subprocess.run(
            [str(exe)],
            input=entrada,
            cwd=BASE_DIR,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            timeout=20,
            creationflags=subprocess.CREATE_NO_WINDOW if os.name == "nt" else 0,
        )
    except (OSError, subprocess.SubprocessError):
        return None
    if proceso.returncode != 0:
        return None
    salida = proceso.stdout
    menu = salida.find("\nMenu\n")
    inicio = salida.find("Opcion: ", menu)
    if inicio == -1:
        return salida.strip()
    inicio += len("Opcion: ")
    fin = salida.find("\nMenu\n", inicio)
    return salida[inicio:fin if fin != -1 else None].strip()


class AplicacionGrafos:
    def __init__(self, raiz):
        import tkinter as tk
        from tkinter import ttk

        self.tk = tk
        self.ttk = ttk
        self.raiz = raiz
        self.datos = None
        self.matriz = None
        self.nombre_vars = []
        self.etiquetas_nombres = []
        self.celda_vars = []
        self.secuencia = []
        self.lienzo_figura = None
        self.barra_figura = None
        self.motor_cpp = False
        self.exe_cpp = BASE_DIR / "algoritmo.exe"
        self.cpp = BASE_DIR / "algoritmo.cpp"
        self.creando_matriz = False

        raiz.title("Laboratorio 3 - Teoria de grafos")
        raiz.geometry("1000x680")
        raiz.minsize(940, 620)
        self._configurar_estilo()
        self._crear_interfaz()
        self._crear_matriz_visual(confirmar=False)
        if os.name == "nt":
            raiz.after(80, lambda: raiz.state("zoomed"))
        raiz.after(150, self._preparar_motor_cpp)

    def _configurar_estilo(self):
        estilo = self.ttk.Style()
        if "clam" in estilo.theme_names():
            estilo.theme_use("clam")
        estilo.configure("TButton", padding=(8, 5))
        estilo.configure("Accent.TButton", background="#2b6cb0", foreground="white")
        estilo.map("Accent.TButton", background=[("active", "#1f4f82")])
        estilo.configure("TLabelframe.Label", font=("Segoe UI", 10, "bold"), foreground="#17324d")
        estilo.configure("Status.TLabel", foreground="#5a6570")

    def _crear_interfaz(self):
        tk = self.tk
        ttk = self.ttk
        encabezado = tk.Frame(self.raiz, bg="#17324d", height=66)
        encabezado.pack(fill="x")
        tk.Label(encabezado, text="LABORATORIO 3 · TEORÍA DE GRAFOS",
                 bg="#17324d", fg="white", font=("Segoe UI", 17, "bold")).pack(
                     side="left", padx=22, pady=14)
        tk.Label(encabezado, text="C++ + Python", bg="#17324d", fg="#d8e6f3",
                 font=("Segoe UI", 10)).pack(side="right", padx=22)

        panel = tk.PanedWindow(
            self.raiz, orient=tk.HORIZONTAL, sashwidth=6, sashrelief="raised",
            bd=0, bg="#d6dbe1")
        panel.pack(fill="both", expand=True, padx=10, pady=10)
        izquierda = ttk.Frame(panel, padding=4, width=550)
        derecha = ttk.Frame(panel, padding=4, width=610)
        izquierda.pack_propagate(False)
        derecha.pack_propagate(False)
        panel.add(izquierda, minsize=450, stretch="never")
        panel.add(derecha, minsize=450, stretch="always")
        self.raiz.after(300, lambda: panel.sash_place(0, 480, 1))

        configuracion = ttk.LabelFrame(izquierda, text="1. Configuración", padding=8)
        configuracion.pack(fill="x", pady=(0, 8))
        self.cantidad_var = tk.StringVar(value="3")
        self.tipo_var = tk.StringVar(value="No dirigido")
        self.ponderado_var = tk.BooleanVar(value=False)
        ttk.Label(configuracion, text="Nodos:").grid(row=0, column=0, sticky="w")
        ttk.Spinbox(configuracion, from_=2, to=26, width=6,
                    textvariable=self.cantidad_var).grid(row=0, column=1, padx=(5, 16))
        ttk.Label(configuracion, text="Tipo:").grid(row=0, column=2, sticky="w")
        ttk.Combobox(configuracion, textvariable=self.tipo_var, state="readonly", width=15,
                     values=("No dirigido", "Dirigido")).grid(row=0, column=3, padx=(5, 16))
        ttk.Checkbutton(configuracion, text="Ponderado", variable=self.ponderado_var).grid(
            row=0, column=4, padx=(0, 14))
        ttk.Button(configuracion, text="Crear / reiniciar matriz",
                   command=self._crear_matriz_visual).grid(
                       row=1, column=0, columnspan=5, sticky="ew", pady=(7, 0))

        matriz_marco = ttk.LabelFrame(izquierda, text="2. Nodos y matriz de adyacencia", padding=6)
        matriz_marco.pack(fill="both", expand=True, pady=(0, 8))
        ttk.Label(matriz_marco,
                  text="No ponderado: 0/1 · Ponderado: x = sin conexión; 0 sí es un peso.",
                  style="Status.TLabel").pack(anchor="w", padx=4, pady=(0, 5))
        contenedor_canvas = ttk.Frame(matriz_marco)
        contenedor_canvas.pack(fill="both", expand=True)
        self.canvas_matriz = tk.Canvas(contenedor_canvas, bg="white", highlightthickness=0)
        barra_v = ttk.Scrollbar(contenedor_canvas, orient="vertical", command=self.canvas_matriz.yview)
        barra_h = ttk.Scrollbar(contenedor_canvas, orient="horizontal", command=self.canvas_matriz.xview)
        self.canvas_matriz.configure(yscrollcommand=barra_v.set, xscrollcommand=barra_h.set)
        self.canvas_matriz.grid(row=0, column=0, sticky="nsew")
        barra_v.grid(row=0, column=1, sticky="ns")
        barra_h.grid(row=1, column=0, sticky="ew")
        contenedor_canvas.rowconfigure(0, weight=1)
        contenedor_canvas.columnconfigure(0, weight=1)
        self.matriz_interior = ttk.Frame(self.canvas_matriz, padding=8)
        self.canvas_matriz.create_window((0, 0), window=self.matriz_interior, anchor="nw")
        self.matriz_interior.bind("<Configure>", lambda _evento: self.canvas_matriz.configure(
            scrollregion=self.canvas_matriz.bbox("all")))

        pie_izquierdo = ttk.Frame(izquierda)
        pie_izquierdo.pack(fill="x")
        ttk.Button(pie_izquierdo, text="Validar y construir grafo", style="Accent.TButton",
                   command=self._construir_grafo).pack(side="left")
        self.estado_var = tk.StringVar(value="Configure el grafo y valide la matriz.")
        ttk.Label(
            pie_izquierdo, textvariable=self.estado_var, style="Status.TLabel",
            wraplength=320, justify="left").pack(side="left", padx=12)

        operaciones = ttk.LabelFrame(derecha, text="3. Consultas", padding=8)
        operaciones.pack(fill="x", pady=(0, 8))
        operaciones.columnconfigure(1, weight=1)
        operaciones.columnconfigure(3, weight=1)
        self.nodo_var = tk.StringVar()
        self.origen_var = tk.StringVar()
        self.destino_var = tk.StringVar()
        self.secuencia_nodo_var = tk.StringVar()
        ttk.Label(operaciones, text="Nodo:").grid(row=0, column=0, sticky="w", pady=3)
        self.combo_nodo = ttk.Combobox(
            operaciones, textvariable=self.nodo_var, state="readonly", width=13)
        self.combo_nodo.grid(row=0, column=1, sticky="w", padx=5)
        ttk.Button(operaciones, text="Consultar adyacentes",
                   command=self._consultar_adyacentes).grid(row=0, column=2, columnspan=2, sticky="ew")
        ttk.Label(operaciones, text="Origen:").grid(row=1, column=0, sticky="w", pady=3)
        self.combo_origen = ttk.Combobox(
            operaciones, textvariable=self.origen_var, state="readonly", width=10)
        self.combo_origen.grid(row=1, column=1, sticky="w", padx=5)
        ttk.Label(operaciones, text="Destino:").grid(row=1, column=2, sticky="w", padx=(8, 0))
        self.combo_destino = ttk.Combobox(
            operaciones, textvariable=self.destino_var, state="readonly", width=10)
        self.combo_destino.grid(row=1, column=3, sticky="w", padx=5)
        ttk.Button(operaciones, text="Buscar todos los caminos",
                   command=self._mostrar_caminos).grid(row=2, column=0, columnspan=4, sticky="ew", pady=3)
        ttk.Button(operaciones, text="Ruta Dijkstra",
                   command=self._ruta_dijkstra).grid(row=3, column=0, columnspan=2, sticky="ew", pady=2)
        ttk.Button(operaciones, text="Ruta Bellman-Ford",
                   command=self._ruta_bellman_ford).grid(row=3, column=2, columnspan=2, sticky="ew", pady=2, padx=(5, 0))
        ttk.Button(operaciones, text="Comparar Métodos (Benchmark)",
                   command=self._comparar_benchmark).grid(row=4, column=0, columnspan=4, sticky="ew", pady=2)
        ttk.Label(operaciones, text="Secuencia:").grid(row=5, column=0, sticky="w", pady=3)
        self.combo_secuencia = ttk.Combobox(
            operaciones, textvariable=self.secuencia_nodo_var,
            state="readonly", width=13)
        self.combo_secuencia.grid(row=5, column=1, sticky="w", padx=5)
        ttk.Button(operaciones, text="Agregar", command=self._agregar_secuencia).grid(
            row=5, column=2, sticky="ew")
        ttk.Button(operaciones, text="Quitar último", command=self._quitar_secuencia).grid(
            row=5, column=3, sticky="ew", padx=(5, 0))
        self.secuencia_texto_var = tk.StringVar(value="(vacía)")
        ttk.Label(operaciones, textvariable=self.secuencia_texto_var, style="Status.TLabel",
                  wraplength=460).grid(row=6, column=0, columnspan=4, sticky="w", pady=(3, 5))
        ttk.Button(operaciones, text="Verificar secuencia",
                   command=self._verificar_secuencia).grid(row=7, column=0, columnspan=2, sticky="ew")
        ttk.Button(operaciones, text="Limpiar secuencia",
                   command=self._limpiar_secuencia).grid(row=7, column=2, columnspan=2,
                                                         sticky="ew", padx=(5, 0))

        botones = ttk.Frame(derecha)
        botones.pack(fill="x", pady=(0, 8))
        for columna in range(2):
            botones.columnconfigure(columna, weight=1)
        acciones = [
            ("Mostrar matriz", self._mostrar_matriz),
            ("Representación formal", self._mostrar_representacion),
            ("Detectar ciclos", self._detectar_ciclos),
            ("Visualizar grafo", self._visualizar),
            ("Exportar JSON / DOT", self._exportar),
            ("Limpiar resultados", lambda: self._mostrar_resultado("Resultados", "")),
        ]
        for indice, (texto, comando) in enumerate(acciones):
            ttk.Button(botones, text=texto, command=comando).grid(
                row=indice // 2, column=indice % 2, sticky="ew", padx=2, pady=2)

        self.pestanas = ttk.Notebook(derecha)
        self.pestanas.pack(fill="both", expand=True)
        pestana_resultados = ttk.Frame(self.pestanas)
        self.pestana_grafica = ttk.Frame(self.pestanas)
        self.pestanas.add(pestana_resultados, text="Resultados")
        self.pestanas.add(self.pestana_grafica, text="Gráfica")
        barra_resultados = ttk.Scrollbar(pestana_resultados, orient="vertical")
        self.resultados = tk.Text(pestana_resultados, wrap="word", font=("Consolas", 10),
                                  bg="#fbfcfe", fg="#1a1a1a", padx=12, pady=12,
                                  yscrollcommand=barra_resultados.set)
        barra_resultados.configure(command=self.resultados.yview)
        self.resultados.pack(side="left", fill="both", expand=True)
        barra_resultados.pack(side="right", fill="y")
        self._mostrar_resultado(
            "Bienvenido",
            "1. Configure el número de nodos y el tipo de grafo.\n"
            "2. Cree la matriz, asigne nombres y escriba las conexiones.\n"
            "3. Pulse 'Validar y construir grafo'.\n"
            "4. Use las consultas y la pestaña de gráfica sin salir de esta ventana.")

    def _crear_matriz_visual(self, confirmar=True):
        from tkinter import messagebox

        try:
            val = str(self.cantidad_var.get()).strip()
            cantidad = int(val)
        except Exception:
            messagebox.showerror("Cantidad inválida", "Ingrese una cantidad entera entre 2 y 26.")
            return
        if not 2 <= cantidad <= 26:
            messagebox.showerror("Cantidad inválida", "El grafo requiere entre 2 y 26 nodos.")
            return
        if confirmar and self.celda_vars and not messagebox.askyesno(
                "Reiniciar matriz", "Se borrarán los datos actuales. ¿Desea continuar?"):
            return

        self.creando_matriz = True
        for control in self.matriz_interior.winfo_children():
            control.destroy()
        dirigido = self.tipo_var.get() == "Dirigido"
        ponderado = self.ponderado_var.get()
        ausencia = "x" if ponderado else "0"
        self.nombre_vars = []
        self.etiquetas_nombres = []
        self.celda_vars = [[None for _ in range(cantidad)] for _ in range(cantidad)]
        self.ttk.Label(self.matriz_interior, text="Nombre", foreground="#17324d").grid(
            row=0, column=0, padx=3, pady=3)
        for indice in range(cantidad):
            variable = self.tk.StringVar(value=chr(ord("A") + indice))
            variable.trace_add("write", lambda *_args, i=indice: self._actualizar_nombre(i))
            self.nombre_vars.append(variable)
            self.ttk.Entry(self.matriz_interior, textvariable=variable, width=9).grid(
                row=0, column=indice + 1, padx=2, pady=2)
        for columna in range(cantidad):
            etiqueta = self.ttk.Label(self.matriz_interior, text=self.nombre_vars[columna].get(),
                                      width=8, anchor="center", foreground="#17324d")
            etiqueta.grid(row=1, column=columna + 1, padx=2, pady=2)
            self.etiquetas_nombres.append([etiqueta])
        for fila in range(cantidad):
            etiqueta_fila = self.ttk.Label(self.matriz_interior,
                                            text=self.nombre_vars[fila].get(), width=9,
                                            anchor="e", foreground="#17324d")
            etiqueta_fila.grid(row=fila + 2, column=0, padx=3, pady=2)
            self.etiquetas_nombres[fila].append(etiqueta_fila)
            for columna in range(cantidad):
                if fila == columna:
                    variable, estado = self.tk.StringVar(value=ausencia), "disabled"
                elif not dirigido and columna < fila:
                    variable, estado = self.celda_vars[columna][fila], "readonly"
                else:
                    variable, estado = self.tk.StringVar(value=ausencia), "normal"
                self.celda_vars[fila][columna] = variable
                entrada = self.ttk.Entry(self.matriz_interior, textvariable=variable, width=8,
                                          justify="center", state=estado)
                entrada.grid(row=fila + 2, column=columna + 1, padx=2, pady=2)
                if estado == "normal":
                    entrada.bind("<KeyRelease>", self._marcar_pendiente)
        self.datos = None
        self.matriz = None
        self.secuencia = []
        self._actualizar_secuencia()
        self.estado_var.set("Matriz preparada. Complete los datos y valide.")
        self.creando_matriz = False
        self.raiz.after_idle(lambda: self.canvas_matriz.configure(
            scrollregion=self.canvas_matriz.bbox("all")))

    def _actualizar_nombre(self, indice):
        if indice >= len(self.etiquetas_nombres):
            return
        nombre = self.nombre_vars[indice].get().strip() or f"N{indice + 1}"
        for etiqueta in self.etiquetas_nombres[indice]:
            etiqueta.configure(text=nombre)
        if not self.creando_matriz:
            self._marcar_pendiente()

    def _marcar_pendiente(self, _evento=None):
        if self.datos is not None:
            self.datos = None
            self.matriz = None
            self.estado_var.set("Hay cambios pendientes. Vuelva a validar el grafo.")

    def _construir_grafo(self):
        from tkinter import messagebox

        try:
            nombres = []
            for variable in self.nombre_vars:
                nombre = variable.get().strip()
                if not nombre:
                    raise ValueError("Todos los nodos necesitan un nombre.")
                if any(ord(caracter) < 32 for caracter in nombre):
                    raise ValueError("Los nombres no pueden contener caracteres de control.")
                nombres.append(nombre)
            if len(set(nombres)) != len(nombres):
                raise ValueError("Los nombres de los nodos deben ser únicos.")

            cantidad = len(nombres)
            dirigido = self.tipo_var.get() == "Dirigido"
            ponderado = self.ponderado_var.get()
            matriz = [[None for _ in range(cantidad)] for _ in range(cantidad)]
            for fila in range(cantidad):
                inicio = 0 if dirigido else fila + 1
                for columna in range(inicio, cantidad):
                    if fila == columna:
                        continue
                    texto = self.celda_vars[fila][columna].get().strip()
                    if ponderado:
                        if not texto or texto.lower() == "x":
                            valor = None
                        else:
                            try:
                                valor = float(texto)
                            except ValueError as error:
                                raise ValueError(
                                    f"Peso inválido entre {nombres[fila]} y {nombres[columna]}.") from error
                            if not math.isfinite(valor) or abs(valor) > LIMITE_PESO:
                                raise ValueError(
                                    f"El peso entre {nombres[fila]} y {nombres[columna]} "
                                    "debe ser finito y estar entre ±1 000 000 000.")
                    else:
                        if texto not in ("0", "1"):
                            raise ValueError(
                                f"La conexión entre {nombres[fila]} y {nombres[columna]} debe ser 0 o 1.")
                        valor = 1 if texto == "1" else None
                    matriz[fila][columna] = valor
                    if not dirigido:
                        matriz[columna][fila] = valor

            aristas = []
            for fila in range(cantidad):
                inicio = 0 if dirigido else fila + 1
                for columna in range(inicio, cantidad):
                    valor = matriz[fila][columna]
                    if valor is None:
                        continue
                    arista = {"origen": fila, "destino": columna}
                    if ponderado:
                        arista["peso"] = valor
                    aristas.append(arista)
            datos = {"dirigido": dirigido, "ponderado": ponderado,
                     "nodos": nombres, "aristas": aristas}
            validar_grafo(datos)
        except ValueError as error:
            messagebox.showerror("No se pudo construir el grafo", str(error))
            return

        self.datos = datos
        self.matriz = matriz
        self.secuencia = []
        self._actualizar_secuencia()
        for combo in (self.combo_nodo, self.combo_origen, self.combo_destino,
                      self.combo_secuencia):
            combo.configure(values=nombres)
        self.nodo_var.set(nombres[0])
        self.origen_var.set(nombres[0])
        self.secuencia_nodo_var.set(nombres[0])
        tipo = "dirigido" if dirigido else "no dirigido"
        pesos = "ponderado" if ponderado else "no ponderado"
        if self.exe_cpp.is_file():
            self.motor_cpp = True
        motor = "motor C++ detectado" if self.motor_cpp else "C++ no disponible"
        self.estado_var.set(f"Grafo válido: {cantidad} nodos · {tipo} · {pesos} · {motor}.")
        resultado_cpp = self._ejecutar_cpp(1, [])
        if resultado_cpp:
            cuerpo = resultado_cpp
        else:
            cuerpo = f"Matriz lista.\n\n{MENSAJE_ERROR_CPP}"
        self._mostrar_resultado(
            "Grafo construido",
            f"Cantidad de nodos: {cantidad}\nTipo: {tipo}\nPonderación: {pesos}\n\n{cuerpo}")

    def _asegurar_grafo(self):
        from tkinter import messagebox

        if self.datos is None:
            messagebox.showwarning("Grafo pendiente",
                                   "Complete la matriz y pulse 'Validar y construir grafo'.")
            return False
        return True

    def _indice(self, nombre):
        return self.datos["nodos"].index(nombre)

    def _mostrar_resultado(self, titulo, contenido):
        self.resultados.configure(state="normal")
        self.resultados.delete("1.0", "end")
        if titulo:
            self.resultados.insert("end", titulo.upper() + "\n")
            self.resultados.insert("end", "=" * len(titulo) + "\n\n")
        self.resultados.insert("end", contenido)
        self.resultados.configure(state="disabled")
        self.pestanas.select(0)

    def _resultado_operacion(self, titulo, opcion, adicionales=None):
        if adicionales is None:
            adicionales = []
        resultado_cpp = self._ejecutar_cpp(opcion, adicionales)
        if resultado_cpp is None:
            from tkinter import messagebox

            messagebox.showerror("Error en motor C++", MENSAJE_ERROR_CPP)
            self._mostrar_resultado(titulo + " · ERROR", MENSAJE_ERROR_CPP)
            return None
        self._mostrar_resultado(titulo, resultado_cpp)
        return resultado_cpp

    def _mostrar_matriz(self):
        if self._asegurar_grafo():
            self._resultado_operacion("Matriz de adyacencia", 1)

    def _mostrar_representacion(self):
        if self._asegurar_grafo():
            self._resultado_operacion("Representación matemática", 2)

    def _consultar_adyacentes(self):
        if self._asegurar_grafo():
            nombre = self.nodo_var.get()
            self._resultado_operacion(f"Consulta de {nombre}", 3, [nombre])

    def _agregar_secuencia(self):
        if self._asegurar_grafo():
            nombre = self.secuencia_nodo_var.get()
            if nombre:
                self.secuencia.append(self._indice(nombre))
                self._actualizar_secuencia()

    def _quitar_secuencia(self):
        if self.secuencia:
            self.secuencia.pop()
            self._actualizar_secuencia()

    def _limpiar_secuencia(self):
        self.secuencia = []
        self._actualizar_secuencia()

    def _actualizar_secuencia(self):
        texto = (" → ".join(self.datos["nodos"][indice] for indice in self.secuencia)
                 if self.datos and self.secuencia else "(vacía)")
        self.secuencia_texto_var.set(texto)

    def _verificar_secuencia(self):
        from tkinter import messagebox

        if not self._asegurar_grafo():
            return
        if not self.secuencia:
            messagebox.showwarning("Secuencia vacía", "Agregue al menos un nodo a la secuencia.")
            return
        nombres = [self.datos["nodos"][indice] for indice in self.secuencia]
        self._resultado_operacion("Verificación de secuencia", 4,
                                  [str(len(nombres)), *nombres])

    def _mostrar_caminos(self):
        if self._asegurar_grafo():
            origen = self._indice(self.origen_var.get())
            destino = self._indice(self.destino_var.get())
            adicionales = [self.datos["nodos"][origen], self.datos["nodos"][destino]]
            self._resultado_operacion("Búsqueda de caminos", 6, adicionales)

    def _detectar_ciclos(self):
        if self._asegurar_grafo():
            self._resultado_operacion("Detección de ciclos", 7)

    def _ruta_dijkstra(self):
        if not self._asegurar_grafo():
            return
        origen = self._indice(self.origen_var.get())
        destino = self._indice(self.destino_var.get())
        n_orig = self.datos["nodos"][origen]
        n_dest = self.datos["nodos"][destino]
        resultado_cpp = self._ejecutar_cpp(8, [n_orig, n_dest])
        if resultado_cpp is None:
            from tkinter import messagebox

            messagebox.showerror("Error en motor C++", MENSAJE_ERROR_CPP)
            self._mostrar_resultado(f"Ruta Dijkstra ({n_orig} → {n_dest}) · ERROR", MENSAJE_ERROR_CPP)
            return

        texto = resultado_cpp
        for prefijo in ("Nodo origen: Nodo destino:", "Nodo origen:", "Nodo destino:"):
            if texto.startswith(prefijo):
                texto = texto[len(prefijo):].strip()
        camino = []
        for linea in texto.splitlines():
            if linea.strip().startswith("Ruta:"):
                partes = linea.split(":", 1)[1].strip().split("->")
                camino = [self._indice(p.strip()) for p in partes if p.strip() in self.datos["nodos"]]
                break

        self._mostrar_resultado(f"Ruta Dijkstra ({n_orig} → {n_dest})", texto)
        if camino:
            self._visualizar(camino_resaltado=camino)

    def _ruta_bellman_ford(self):
        if not self._asegurar_grafo():
            return
        origen = self._indice(self.origen_var.get())
        destino = self._indice(self.destino_var.get())
        n_orig = self.datos["nodos"][origen]
        n_dest = self.datos["nodos"][destino]
        resultado_cpp = self._ejecutar_cpp(9, [n_orig, n_dest])
        if resultado_cpp is None:
            from tkinter import messagebox

            messagebox.showerror("Error en motor C++", MENSAJE_ERROR_CPP)
            self._mostrar_resultado(f"Ruta Bellman-Ford ({n_orig} → {n_dest}) · ERROR", MENSAJE_ERROR_CPP)
            return

        texto = resultado_cpp
        for prefijo in ("Nodo origen: Nodo destino:", "Nodo origen:", "Nodo destino:"):
            if texto.startswith(prefijo):
                texto = texto[len(prefijo):].strip()
        camino = []
        for linea in texto.splitlines():
            if linea.strip().startswith("Ruta:"):
                partes = linea.split(":", 1)[1].strip().split("->")
                camino = [self._indice(p.strip()) for p in partes if p.strip() in self.datos["nodos"]]
                break

        self._mostrar_resultado(f"Ruta Bellman-Ford ({n_orig} → {n_dest})", texto)
        if camino:
            self._visualizar(camino_resaltado=camino)

    def _comparar_benchmark(self):
        if not self._asegurar_grafo():
            return
        origen = self._indice(self.origen_var.get())
        destino = self._indice(self.destino_var.get())
        n_orig = self.datos["nodos"][origen]
        n_dest = self.datos["nodos"][destino]
        resultado_cpp = self._ejecutar_cpp(10, [n_orig, n_dest])
        if resultado_cpp is None:
            from tkinter import messagebox

            messagebox.showerror("Error en motor C++", MENSAJE_ERROR_CPP)
            self._mostrar_resultado(f"Benchmark Comparativo ({n_orig} vs {n_dest}) · ERROR", MENSAJE_ERROR_CPP)
            return

        texto = resultado_cpp
        for prefijo in ("Nodo origen: Nodo destino:", "Nodo origen:", "Nodo destino:"):
            if texto.startswith(prefijo):
                texto = texto[len(prefijo):].strip()
        self._mostrar_resultado(f"Benchmark Comparativo ({n_orig} vs {n_dest})", texto)

    def _visualizar(self, camino_resaltado=None):
        if not self._asegurar_grafo():
            return
        from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk

        for control in self.pestana_grafica.winfo_children():
            control.destroy()
        figura = crear_figura_grafo(self.datos, tamano=(4.2, 2.5), camino_resaltado=camino_resaltado)
        self.lienzo_figura = FigureCanvasTkAgg(figura, master=self.pestana_grafica)
        self.lienzo_figura.draw()
        self.barra_figura = NavigationToolbar2Tk(self.lienzo_figura, self.pestana_grafica,
                                                 pack_toolbar=False)
        self.barra_figura.update()
        self.barra_figura.pack(side="bottom", fill="x")
        self.lienzo_figura.get_tk_widget().pack(fill="both", expand=True)
        self.pestanas.select(self.pestana_grafica)

    def _exportar(self):
        from tkinter import filedialog, messagebox

        if not self._asegurar_grafo():
            return
        carpeta = filedialog.askdirectory(
            title="Seleccione la carpeta para guardar grafo.json y grafo.dot",
            initialdir=str(BASE_DIR))
        if not carpeta:
            return
        try:
            ruta_json, ruta_dot = guardar_archivos_grafo(self.datos, carpeta)
        except OSError as error:
            messagebox.showerror("No se pudo exportar", str(error))
            return
        messagebox.showinfo("Archivos generados", f"Se guardaron:\n{ruta_json}\n{ruta_dot}")

    def _preparar_motor_cpp(self):
        if self.exe_cpp.is_file():
            self.motor_cpp = True
            self.estado_var.set("Interfaz lista · motor C++ detectado.")
            return
        compilador = shutil.which("g++") or shutil.which("clang++")
        if compilador and self.cpp.is_file():
            try:
                proceso = subprocess.run(
                    [compilador, "-std=c++17", "-O2", str(self.cpp), "-o", str(self.exe_cpp)],
                    cwd=BASE_DIR, capture_output=True, text=True, timeout=90,
                    creationflags=subprocess.CREATE_NO_WINDOW if os.name == "nt" else 0)
                self.motor_cpp = proceso.returncode == 0 and self.exe_cpp.is_file()
            except (OSError, subprocess.SubprocessError):
                self.motor_cpp = False
        if self.motor_cpp:
            self.estado_var.set("Interfaz lista · motor C++ compilado automáticamente.")
        else:
            self.estado_var.set("ATENCIÓN: motor C++ no disponible. Se requiere compilar algoritmo.cpp.")

    def _entrada_cpp(self, opcion, adicionales=None):
        return generar_entrada_cpp(self.datos, opcion, adicionales)

    def _ejecutar_cpp(self, opcion, adicionales=None):
        if self.exe_cpp.is_file():
            self.motor_cpp = True
        if not self.motor_cpp or not self.exe_cpp.is_file():
            return None
        return ejecutar_cpp(self.datos, opcion, adicionales, exe_ruta=self.exe_cpp)


def iniciar_interfaz():
    try:
        import tkinter as tk
    except ImportError as error:
        print(f"Error: Python no incluye Tkinter: {error}", file=sys.stderr)
        return 1
    try:
        raiz = tk.Tk()
    except tk.TclError as error:
        print(f"Error: no se pudo abrir la interfaz grafica: {error}", file=sys.stderr)
        return 1
    AplicacionGrafos(raiz)
    raiz.mainloop()
    return 0


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("archivo", nargs="?", type=Path,
                        help="JSON que se desea visualizar; sin archivo abre la interfaz completa")
    parser.add_argument("--guardar", type=Path, help="Guarda la grafica como PNG, SVG o PDF")
    parser.add_argument("--sin-mostrar", action="store_true", help="No abre una ventana grafica")
    opciones = parser.parse_args()
    if opciones.archivo is None:
        if opciones.guardar is not None or opciones.sin_mostrar:
            parser.error("--guardar y --sin-mostrar requieren un archivo JSON.")
        return iniciar_interfaz()
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
