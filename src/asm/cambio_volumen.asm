section .text
    global procesar_volumen

procesar_volumen:
    ; RDI = buffer de audio (floats)
    ; RSI = número de muestras
    ; XMM0 = factor de ganancia (pasado desde C)

.bucle:
    test rsi, rsi       ; ¿Llegamos al final?
    jz .fin
    
    movss xmm1, [rdi]   ; Cargar un float del audio
    mulss xmm1, xmm0    ; Multiplicar por la ganancia
    movss [rdi], xmm1   ; Guardar de vuelta
    
    add rdi, 4          ; Avanzar 4 bytes (tamaño de un float)
    dec rsi             ; Decrementar contador
    jmp .bucle

.fin:
    ret