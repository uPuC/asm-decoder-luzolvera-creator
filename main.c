
/******************************************************************************
Prac 2 - AVR ASM OpCode Decoder
*******************************************************************************/

#include <stdio.h>
#include <inttypes.h>

const uint8_t flash_mem[] ={ 
    0x00, 0x24, 
    0xA0, 0xE0, 
    0xB2, 0xE0, 
    0x0D, 0x91, 
    0x00, 0x30, 
    0xE9, 0xF7, 
    0x11, 0x97, 
    0xC0, 0xE0, 
    0xD2, 0xE0, 
    0x09, 0x91, 
    0x1E, 0x91, 
    0x01, 0x17, 
    0x51, 0xF4, 
    0x0A, 0x2F, 
    0x0A, 0x95, 
    0x1C, 0x2F, 
    0x01, 0x17, 
    0xB9, 0xF7, 
    0x0B, 0x2F, 
    0x1D, 0x2F, 
    0x01, 0x17, 
    0x99, 0xF7, 
    0x03, 0x94, 
    0x00, 0x00 
    
};

const uint16_t inst16_table[] = {
    0x2400,  // CLR 
    0xE0A0,  // LDI 
    0xE0B2,  // LDI 
    0x910D,  // LD Rd, X+
    0x3000,  // CPI
    0xF7E9,  // BRNE etiqueta
    0x9711,  // SBIW
    0xE0C0,  // LDI
    0xD2C4,  // RCALL etiqueta
    0x09E0,  // SBC
    0x1E91,  // ADC
    0x0191,  // MOVW
    0x5117,  // SUBI
    0x0AF4,  // SBC
    0x0A2F,  // SBC
    0x1C95,  // ADC
    0x652F,  // SBR
    0x9109, //LD Rd,Y+
    0x911E, //LD Rd,-X
    0x1701,  // CP
    0xF7B9,  // BRNE etiqueta
    0x2F0B,  // MOV
    0x950A, //DEC
    0x2F1D,  // MOV
    0x1701,  // CP
    0xF799,  // BRNE etiqueta
    0x9403,  // INC
    0x0000,  // NOP
};

enum{
    e_CLR,
    e_LDI,
    e_LD,
    e_CPI,
    e_BRNE,
    e_SBIW,
    e_RCALL,
    e_SBC,
    e_ADC,
    e_MOVW,
    e_SUBI,
    e_SBR,
    e_CP,
    e_MOV,
    e_INC,
    e_DEC,
    e_NOP,
};


// Op Code struct
typedef union {
    uint16_t op16; // e.g.: watchdog, nop
    struct{
        uint16_t op4:4;
        uint16_t d5:5;
        uint16_t op7:7;
    }type1; //LSR, LD
    struct{
        uint16_t r4:4;
        uint16_t d5:5;
        uint16_t r1:1;
        uint16_t op6:6;
    }type2; //MOV,MUL,ADC,ADD,AND,CP,SBC
    struct{
        uint16_t op4:4;
        uint16_t d5:5;
        uint16_t op7:7;
    }type3; //INC,DEC
    struct{
        uint16_t k4:4;
        uint16_t d4:4;
        uint16_t k4_1:4;
        uint16_t op4:4;
    }type4; //LDI,CPI,SUBI
    struct{
        uint16_t k4:4;
        uint16_t d2:2;
        uint16_t k2:2;
        uint16_t op8:8;
    }type5; //SBIW
    struct{
        uint16_t s3:3;
        uint16_t k7:7;
        uint16_t op6:6;
    }type6; //BRBS, BRBC
    struct{
        uint16_t r4:4;
        uint16_t d4:4;
        uint16_t op8:8;
    }type7; //MOVW
    struct{
        uint16_t k12:12;
        uint16_t op4:4;
    }type8; //RCALL
    struct{
        uint16_t k10:10;
        uint16_t op6:6;
    }type9; //CLR
    struct{
        uint16_t op4:4;
        uint16_t d5:5;
        uint16_t op7:7;
    }type10; //INC
    struct{
        uint16_t op3:3;
        int16_t k7:7;
        uint16_t op6:6;
    }type11; //BRNE
    // TO-DO: Add more types as needed
} Op_Code_t;


int main()
{
    Op_Code_t *instruction;

    printf("- Practica 2: AVR OpCode -\n");

    for (uint8_t idx = 0; idx < sizeof(flash_mem); idx += 2)
    {
        instruction = (Op_Code_t*) &flash_mem[idx];

        // 1. NOP
        if (instruction->op16 == 0x0000) {
            printf("NOP\n");
        } 
        // 2. CLR
        else if ((instruction->op16 & 0xFC00) == 0x2400) {
            printf("CLR R%d\n", instruction->type9.k10);
        }
        // 3. LDI (Varios casos)
        else if ((instruction->op16 & 0xF000) == 0xE000) {
            int Rd = 16 + instruction->type4.d4;
            int K = (instruction->type4.k4_1 << 4) | instruction->type4.k4;
            printf("LDI R%d,%d\n", Rd, K);
        }
        // 4. LD Rd, X+
        else if ((instruction->op16 & 0xFE0F) == 0x900D) {
            printf("LD R%d, X+\n", instruction->type1.d5);
        } 
        // 5. CPI
        else if ((instruction->op16 & 0xF000) == 0x3000) {
            int Rd = 16 + instruction->type4.d4;
            int K = (instruction->type4.k4_1 << 4) | instruction->type4.k4;
            printf("CPI R%d,%d\n", Rd, K);
        } 
        // 6. BRNE (Varios saltos a etiquetas) (+/-)
        else if ((instruction->op16 & 0xFC07) == 0xF401) {
            int k = instruction->type11.k7;
            printf("BRNE %+d\n", k + 1);
        } 
        // 7. SBIW
        else if ((instruction->op16 & 0xFF00) == 0x9700) {
            int Rd = 24 + (instruction->type5.d2 * 2);
            int K = (instruction->type5.k2 << 4) | instruction->type5.k4;
            printf("SBIW R%d,%d\n", Rd, K);
        } 
        // 8. RCALL
        else if ((instruction->op16 & 0xF000) == 0xD000) {
            printf("RCALL %d\n", instruction->type8.k12);
        } 
        // 9. SBC (Varios casos)
        else if ((instruction->op16 & 0xFC00) == 0x0800) {
            int Rd = instruction->type2.d5;
            int Rr = (instruction->type2.r1 << 4) | instruction->type2.r4;
            printf("SBC R%d,R%d\n", Rd, Rr);
        } 
        // 10. ADC (Varios casos)
        else if ((instruction->op16 & 0xFC00) == 0x1C00) {
            int Rd = instruction->type2.d5;
            int Rr = (instruction->type2.r1 << 4) | instruction->type2.r4;
            printf("ADC R%d,R%d\n", Rd, Rr);
        }
        // 11. MOVW
        else if ((instruction->op16 & 0xFf00) == 0x0100) {
            printf("MOVW R%d,R%d\n", instruction->type7.d4, instruction->type7.r4);
        } 
        // 12. SUBI
        else if ((instruction->op16 & 0xF000) == 0x5000) {
            int Rd = 16 + instruction->type4.d4;
            int K = (instruction->type4.k4_1 << 4) | instruction->type4.k4;
            printf("SUBI R%d,%d\n", Rd, K);
        }
        // 13. SBR
        else if ((instruction->op16 & 0xF000) == 0x6000) {
            int Rd = 16 + instruction->type4.d4;
            int K = (instruction->type4.k4_1 << 4) | instruction->type4.k4;
            printf("SBR R%d,%d\n", Rd, K);
        }
        // 14. CP (Varios casos)
        else if ((instruction->op16 & 0xFC00) == 0x1400) {
            int Rd = instruction->type2.d5;
            int Rr = (instruction->type2.r1 << 4) | instruction->type2.r4;
            printf("CP R%d,R%d\n", Rd, Rr);
        } 
        // 15. MOV (Varios casos)
        else if ((instruction->op16 & 0xFC00) == 0x2C00) {
            int Rd = instruction->type2.d5;
            int Rr = (instruction->type2.r1 << 4) | instruction->type2.r4;
            printf("MOV R%d,R%d\n", Rd, Rr);
        } 
        // 16. INC
        else if ((instruction->op16 & 0xFE0F) == 0x9403) {
            printf("INC R%d\n", instruction->type3.d5);
        } 
        // 17. DEC
        else if ((instruction->op16 & 0xFE0F) == 0x940A) {
            printf("INC R%d\n", instruction->type3.d5);
        } 
        // 18. LD Rd, Y+
        else if ((instruction->op16 & 0xFE0F) == 0x9009) {
            printf("LD R%d, Y+\n", instruction->type1.d5);
        } 
        // 19. LD Rd, X-
        else if ((instruction->op16 & 0xFE0F) == 0x900E) {
            printf("LD R%d, -X\n", instruction->type1.d5);
        } 
        else {
            printf("unknown (0x%04X)\n", instruction->op16);
        }
    }
    return 0;
}
