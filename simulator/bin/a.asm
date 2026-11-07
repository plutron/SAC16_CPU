
.text

    LDI R0, 10
    LDI R1, 20

    ; ACC = 10 + 20
    ADD R0
    ADD R1

    ; Store ACC into memory
    LDI R2, 0
    ST ACC, R2

    ; Load memory value into R3
    LD R3, R2

    ; ACC = ACC + R3
    ADD R3

    HALT


.data

    .word 100
    .word 200
    .word 300
    .word 400
