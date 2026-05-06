from maix import camera, display, nn
import uart

# Inicializar UART hacia ESP-32
u = uart.UART(uart.UART.UART1, 115200)

# Tu modelo de detección
detector = nn.YOLOv5(model="/root/modelo.mud")
cam = camera.Camera(320, 224)

CLASES_CON_AUDIO = {
    "condor":        "ANIMAL_001",
    "plaza_bolivar": "FOTO_001",
    "montaña":       "PAISAJE_001",
}

ultimo_enviado = ""

while True:
    img = cam.read()
    resultados = detector.detect(img)

    for obj in resultados:
        nombre_clase = detector.labels[obj.class_id]

        if nombre_clase in CLASES_CON_AUDIO:
            codigo = CLASES_CON_AUDIO[nombre_clase]

            # Evitar enviar el mismo objeto repetidamente
            if codigo != ultimo_enviado:
                u.write(f"{codigo}\n".encode())
                ultimo_enviado = codigo
                print(f"Enviado: {codigo}")

    # Limpiar último enviado si no se detecta nada
    if len(resultados) == 0:
        ultimo_enviado = ""