section .text
    global procesar_volumen
    global procesar_ecualizador

procesar_volumen:
    ; RDI = buffer de audio (floats)
    ; RSI = número de muestras
    ; XMM0 = factor de ganancia (pasado desde C)

.bucle:
    cmp rsi, 0
    jz .fin
    
    movss xmm1, [rdi]   ; Cargar un float del audio
    mulss xmm1, xmm0    ; Multiplicar por la ganancia
    movss [rdi], xmm1   ; Guardar de vuelta
    
    add rdi, 4          ; Avanzar 4 bytes (tamaño de un float)
    dec rsi             ; Decrementar contador
    jmp .bucle

.fin:
    ret

procesar_ecualizador:
    ; RDI = apuntador al buffer de audio
    ; RSI = long total_muestras
    ; RDX = EqConfig* config

    
    ;Generar 1.0
    mov rax, 1
    cvtsi2ss xmm14, rax         
    shufps xmm14, xmm14, 0x00    ; Broadcast: copiar 1.0 a los 4 slots del registro [1.0, 1.0, 1.0, 1.0]

    ; Generar -1.0 
    mov rax, -1
    cvtsi2ss xmm15, rax          
    shufps xmm15, xmm15, 0x00    ; Broadcast: [-1.0, -1.0, -1.0, -1.0]
 
    shr rsi, 1                 ; Dividimos 2 veces el tamaño porque son 2 bandas derecho e izquierdo 

.bucle_principal:
    movq xmm0, [rdi]            ; Cargar [xL, xR]
    pxor xmm11, xmm11           ; Limpiar acumulador final

    mov r8, rdx                 ; Puntero a la primera banda
    mov rcx, 3                  ; Contador de 3 bandas

.bucle_bandas:
    ; Cargar coeficientes y duplicarlos (Broadcast)
    movss xmm5              , [r8]            ; b0
    shufps xmm5, xmm5, 0x00
    movss xmm6, [r8 + 4]        ; b1
    shufps xmm6, xmm6, 0x00
    movss xmm7, [r8 + 8]        ; b2
    shufps xmm7, xmm7, 0x00
    movss xmm8, [r8 + 12]       ; a1
    shufps xmm8, xmm8, 0x00
    movss xmm9, [r8 + 16]       ; a2
    shufps xmm9, xmm9, 0x00

    ; Cargar estados actuales
    movq xmm1, [r8 + 20]        ; x1
    movq xmm2, [r8 + 28]        ; x2
    movq xmm3, [r8 + 36]        ; y1
    movq xmm4, [r8 + 44]        ; y2

    ; Matemática Biquad: y = b0*x + b1*x1 + b2*x2 - a1*y1 - a2*y2
    movups xmm10, xmm0
    mulps xmm10, xmm5           ; b0 * x

    movups xmm12, xmm1
    mulps xmm12, xmm6           ; b1 * x1
    addps xmm10, xmm12
          
    movups xmm12, xmm2
    mulps xmm12, xmm7           ; b2 * x2
    addps xmm10, xmm12

    movups xmm12,xmm3
    mulps xmm12, xmm8           ; a1 * y1
    subps xmm10, xmm12

    movups xmm12, xmm4
    mulps xmm12, xmm9           ; a2 * y2
    subps xmm10, xmm12

    ;actualizamos los datos
    movq [r8 + 28], xmm1       ; x2 = x1
    movq [r8+20], xmm0        ; x1 = x
    movq [r8 + 44], xmm3       ; y2 = y1
    movq [r8 + 36], xmm10      ; y1 = y

    addps xmm11, xmm10          ; Acumular resultado de esta banda

    add r8, 52 ;pasamos a la siguiente banda
    dec rcx
    jnz .bucle_bandas

    movlps [rdi], xmm11         ; Guardar resultado en buffer de audio
    add rdi, 8                  ; Siguiente par estéreo
    dec rsi
    jnz .bucle_principal
.fin:
    ret
