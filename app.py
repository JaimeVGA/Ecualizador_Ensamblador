import customtkinter as ctk
from tkinter import filedialog
import ctypes
import os
import sys

# 1. CONFIGURACIÓN DE RUTAS 
# Ruta interna: Donde PyInstaller guarda la librería .so (temporal)
if hasattr(sys, '_MEIPASS'):
    ruta_interna = sys._MEIPASS
else:
    ruta_interna = os.path.dirname(os.path.abspath(__file__))

# Ruta externa: Donde el usuario ve el ejecutable (para el archivo de salida)
if hasattr(sys, '_MEIPASS'):
    ruta_ejecutable = os.path.dirname(os.path.abspath(sys.executable))
else:
    ruta_ejecutable = os.path.dirname(os.path.abspath(__file__))

# Cargar la librería desde la ruta interna
ruta_lib = os.path.join(ruta_interna, "libecualizador.so")

try:
    motor_eq = ctypes.CDLL(ruta_lib)
    # Definir la firma de la función C: int ecualizar_archivo(char*, char*, float, float, float)
    motor_eq.ecualizar_archivo.argtypes = [
        ctypes.c_char_p, ctypes.c_char_p, 
        ctypes.c_float, ctypes.c_float, ctypes.c_float
    ]
    motor_eq.ecualizar_archivo.restype = ctypes.c_int
except OSError:
    print(f"ERROR: No se encontró la librería en {ruta_lib}")
    # Si falla al cargar, intentamos la carpeta actual como último recurso
    try:
        motor_eq = ctypes.CDLL("./libecualizador.so")
    except:
        exit(1)

# =================================================================
# 2. DISEÑO DE LA INTERFAZ GRÁFICA (DARK MINIMALIST)
# =================================================================

COLOR_FONDO = "#0F0F0F"      # Negro casi puro
COLOR_TARJETA = "#1A1A1A"    # Gris oscuro profundo
COLOR_ACENTO = "#00E5FF"     # Cyan Neón
COLOR_ACENTO_HOVER = "#00B8CC"
COLOR_TEXTO = "#FFFFFF"

ctk.set_appearance_mode("dark")

class EcualizadorApp(ctk.CTk):
    def __init__(self):
        super().__init__()

        # Configuración de ventana
        self.title("Ecualizador ASM v1.0")
        self.geometry("460x680")
        self.resizable(False, False)
        self.configure(fg_color=COLOR_FONDO)

        self.ruta_entrada = ""
        # El archivo de salida se guardará siempre junto al ejecutable
        self.ruta_salida = os.path.join(ruta_ejecutable, "salida_eq.wav")

        # Fuentes
        fuente_header = ctk.CTkFont(family="Inter", size=26, weight="bold")
        fuente_sub = ctk.CTkFont(family="Inter", size=14, weight="bold")
        fuente_val = ctk.CTkFont(family="Consolas", size=13)

        # --- HEADER ---
        self.lbl_titulo = ctk.CTkLabel(self, text="AUDIO ENGINE", font=fuente_header, text_color=COLOR_ACENTO)
        self.lbl_titulo.pack(pady=(40, 5))
        self.lbl_tag = ctk.CTkLabel(self, text="X86_64 ASSEMBLY OPTIMIZED", font=ctk.CTkFont(size=10), text_color="#555555")
        self.lbl_tag.pack(pady=(0, 25))

        # --- TARJETA DE ARCHIVO ---
        self.card_file = ctk.CTkFrame(self, fg_color=COLOR_TARJETA, corner_radius=12)
        self.card_file.pack(pady=10, padx=30, fill="x")

        self.btn_cargar = ctk.CTkButton(
            self.card_file, text="CARGAR WAV", font=fuente_sub,
            fg_color="transparent", border_color=COLOR_ACENTO, border_width=1,
            hover_color=COLOR_TARJETA, text_color=COLOR_ACENTO,
            command=self.cargar_archivo
        )
        self.btn_cargar.pack(pady=(20, 10), padx=20, fill="x")

        self.lbl_archivo = ctk.CTkLabel(self.card_file, text="Esperando archivo...", font=fuente_val, text_color="#888888")
        self.lbl_archivo.pack(pady=(0, 20))

        # --- TARJETA DE EQ ---
        self.card_eq = ctk.CTkFrame(self, fg_color=COLOR_TARJETA, corner_radius=12)
        self.card_eq.pack(pady=10, padx=30, fill="x")

        self.lbl_eq_title = ctk.CTkLabel(self.card_eq, text="FILTROS BIQUAD", font=fuente_sub, text_color="#AAAAAA")
        self.lbl_eq_title.pack(pady=(15, 10))

        # Sliders
        self.lbl_bajos, self.slider_bajos = self.crear_slider_group(self.card_eq, "LOW (100Hz)")
        self.lbl_medios, self.slider_medios = self.crear_slider_group(self.card_eq, "MID (1kHz)")
        self.lbl_agudos, self.slider_agudos = self.crear_slider_group(self.card_eq, "HIGH (5kHz)")

        ctk.CTkLabel(self.card_eq, text="").pack(pady=5) # Espaciador

        # --- BOTÓN RENDER ---
        self.btn_render = ctk.CTkButton(
            self, text="PROCESAR AUDIO", font=fuente_sub, height=50,
            fg_color=COLOR_ACENTO, hover_color=COLOR_ACENTO_HOVER, text_color=COLOR_FONDO,
            corner_radius=8, command=self.procesar_audio
        )
        self.btn_render.pack(pady=(30, 10), padx=30, fill="x")

        self.lbl_status = ctk.CTkLabel(self, text="Estado: Listo", font=fuente_val, text_color="#555555")
        self.lbl_status.pack()

    def crear_slider_group(self, parent, label_text):
        lbl = ctk.CTkLabel(parent, text=f"{label_text}: 0.0 dB", font=ctk.CTkFont(size=12))
        lbl.pack(pady=(10, 0))
        slider = ctk.CTkSlider(
            parent, from_=-20, to=20, number_of_steps=400,
            progress_color=COLOR_ACENTO, button_color=COLOR_ACENTO,
            command=self.actualizar_textos
        )
        slider.set(0)
        slider.pack(pady=(0, 10), padx=25, fill="x")
        return lbl, slider

    def actualizar_textos(self, _=None):
        self.lbl_bajos.configure(text=f"LOW (100Hz): {self.slider_bajos.get():+.1f} dB")
        self.lbl_medios.configure(text=f"MID (1kHz): {self.slider_medios.get():+.1f} dB")
        self.lbl_agudos.configure(text=f"HIGH (5kHz): {self.slider_agudos.get():+.1f} dB")

    def cargar_archivo(self):
        ruta = filedialog.askopenfilename(filetypes=[("Audio WAV", "*.wav")])
        if ruta:
            self.ruta_entrada = ruta
            name = os.path.basename(ruta)
            display_name = (name[:25] + '..') if len(name) > 25 else name
            self.lbl_archivo.configure(text=display_name, text_color=COLOR_ACENTO)
            self.lbl_status.configure(text="Archivo cargado correctamente", text_color="#AAAAAA")

    def procesar_audio(self):
        if not self.ruta_entrada:
            self.lbl_status.configure(text="Error: Carga un archivo .wav", text_color="#FF4444")
            return

        self.lbl_status.configure(text="Procesando en ASM...", text_color=COLOR_ACENTO)
        self.update()

        # Obtener valores de los sliders
        b, m, a = self.slider_bajos.get(), self.slider_medios.get(), self.slider_agudos.get()

        # LLAMADA AL MOTOR C/ASM
        res = motor_eq.ecualizar_archivo(
            self.ruta_entrada.encode('utf-8'),
            self.ruta_salida.encode('utf-8'),
            float(b), float(m), float(a)
        )

        if res == 0:
            self.lbl_status.configure(text="¡Éxito! salida_eq.wav generado", text_color="#00FF88")
        else:
            self.lbl_status.configure(text=f"Error en el motor (Código {res})", text_color="#FF4444")

if __name__ == "__main__":
    app = EcualizadorApp()
    app.mainloop()