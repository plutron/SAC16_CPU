.text
    LDI R0, 10
    LDI R1, 20
    ADD R0
    ADD R1
    LDI R2, 0
    ST ACC, R2
    LD R3, R2
    ADD R3
    HALT

.data
    .word 100
    .word 200
    .word 300
    .word 400
