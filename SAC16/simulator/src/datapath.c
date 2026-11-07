#include "datapath.h"

#include "register_file.h"
#include "alu.h"
#include "flags.h"

Bool datapath_execute(Sac16State *cpu,
                      const DecodedInstruction *instruction)
{
    if (cpu == NULL || instruction == NULL)
        return FALSE;

    word A;
    word B;
    word result;

    ALUResult alu_result;

    switch (instruction->opcode)
    {
        // LD Rd, [Ra]
        // Rd = MEM[Ra]
        case LD:
        {
            if (instruction->reserved != 0)
                return FALSE;

            if (!register_file_read(cpu, instruction->rs, &A))
                return FALSE;

            result = cpu->dmem.data[A];

            if (!register_file_write(cpu, instruction->rd, result))
                return FALSE;

            break;
        }

        // ST Rs, [Ra]
        // MEM[Ra] = Rs
        case ST:
        {
            if (instruction->reserved != 0)
                return FALSE;

            if (!register_file_read(cpu, instruction->rd, &A))
                return FALSE;

            if (!register_file_read(cpu, instruction->rs, &B))
                return FALSE;

            cpu->dmem.data[B] = A;

            break;
        }

        // MOV Rd, Rs
        // Rd = Rs
        case MOV:
        {
            if (instruction->reserved != 0)
                return FALSE;

            if (!register_file_read(cpu, instruction->rs, &A))
                return FALSE;

            if (!register_file_write(cpu, instruction->rd, A))
                return FALSE;

            break;
        }

        // ADD Rs
        // ACC = ACC + Rs
        case ADD:

        // SUB Rs
        // ACC = ACC - Rs
        case SUB:

        // MUL Rs
        // ACC = ACC * Rs
        case MUL:

        // AND Rs
        // ACC = ACC & Rs
        case AND:

        // OR Rs
        // ACC = ACC | Rs
        case OR:
        {
            if (instruction->reserved != 0)
                return FALSE;

            // Operand B must be zero for ALU instructions.
            if (instruction->rs != 0)
                return FALSE;

            if (!register_file_read(cpu, instruction->rd, &A))
                return FALSE;

            alu_result = alu_execute(
                cpu->regs.ACC,
                A,
                instruction->opcode
            );

            if (!register_file_write(cpu, ACC, alu_result.result))
                return FALSE;

            flags_update(cpu, &alu_result);

            break;
        }

        // NOT
        // ACC = ~ACC
        case NOT:
        {
            if (instruction->reserved != 0)
                return FALSE;

            alu_result = alu_execute(
                cpu->regs.ACC,
                0,
                NOT
            );

            if (!register_file_write(cpu, ACC, alu_result.result))
                return FALSE;

            flags_update(cpu, &alu_result);

            break;
        }

        // LDI Rd, imm8
        // Rd = imm8
        case LDI:
        {
            if (!register_file_write(
                    cpu,
                    instruction->rd,
                    instruction->imm))
                return FALSE;

            break;
        }

        // Control-flow instructions belong to Control Unit.
        case JMP:
        case JZ:
        case JNZ:
        case CALL:
        case RET:
        case HALT:
        default:
            return FALSE;
    }

    return TRUE;
}