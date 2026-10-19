#pragma once

typedef enum {
    LD   = 0x0,
    ST   = 0x1,
    MOV  = 0x2,
    ADD  = 0x3,
    SUB  = 0x4,
    MUL  = 0x5,
    AND  = 0x6,
    OR   = 0x7,
    NOT  = 0x8,
    JMP  = 0x9,
    JZ   = 0xA,
    JNZ  = 0xB,
    CALL = 0xC,
    RET  = 0xD,
    LDI  = 0xE,
    HALT = 0xF
} Opcode;

typedef enum {
    R0 = 0,
    R1 = 1,
    R2 = 2,
    R3 = 3,
    R4 = 4,
    R5 = 5,
    R6 = 6,
    R7 = 7,
    ACC = 8,   
    INVALID_REG = 16
} RegisterName;

typedef enum {
    FileNotFound,
    FileNameTooLong,
    InvalidRegister,
    InvalidOpcode,
    InvalidOperand,
    DuplicateLabel,
    UnknownLabel,
    InvalidImmediate,
    InvalidTarget,
    MemoryOverflow
} ErrorType;

typedef enum{
    FALSE = 0,
    TRUE = 1
}Bool;