
/******************************************************************************
Prac 2 - AVR ASM OpCode Decoder
*******************************************************************************/

#include <stdio.h>
#include <inttypes.h>

const uint8_t flash_mem[] ={ 
    0x00, 0x24, //0x2400 --> 0010 0100 0000 0000 --> 0010 01dd dddd dddd --> CLR Rd       d->0+00 0000 0000=0
    0xA0, 0xE0, //0xE0A0 --> 1110 0000 1010 0000 --> 1110 kkkk dddd kkkk --> LDI Rd,K     d->16+A(1010)=26, K->0+0000 0000=0
    0xB2, 0xE0, //0xE0B2 --> 1110 0000 1011 0010 --> 1110 kkkk dddd kkkk --> LDI Rd,K     d->16+B(1011)=27, K->0+2(0000 0010)=2
    0x0D, 0x91, //0x910D --> 1001 0001 0000 1101 --> 1001 000d dddd 1101 --> LD Rd, X+    d->16+0(0000)=16         
    0x00, 0x30, //0x3000 --> 0011 0000 0000 0000 --> 0011 kkkk dddd kkkk --> CPI Rd,K     d->16+0(0000)=16, K->0+0(0000 0000)=0
    
    0xE9, 0xF7, //0xF7E9 --> 1111 0111 1110 1001 --> 1111 01kk kkkk ksss --> BRBC s,k     s->0+1(001)=1, K->-64+125(11 1110 1)=61
                                                  // 1111 01kk kkkk k001 --> BRNE k
                                                  
    0x11, 0x97, //0x9711 --> 1001 0111 0001 0001 --> 1001 0111 KKdd KKKK --> SBIW Rd,K    d->01=26,     K->0+1(00 0001)=1               --
    0xC0, 0xE0, //0xE0C0 --> 1110 0000 1100 0000 --> 1110 KKKK dddd KKKK --> LDI Rd,K     d->16+C(1100)=28, K->0+0(0000 0000)=0
    0xC4, 0xD2, //0xD2C4 --> 1101 0010 1100 0100 --> 1101 kkkk kkkk kkkk --> RCALL k
    0xE0, 0x09, //0x09E0 --> 0000 1001 1110 0000 --> 0000 10rd dddd rrrr --> SBC Rd,Rr    d->0+1E(1 1110)=30,  r->0+0(0 0000)=0
    0x91, 0x1E, //0x1E91 --> 0001 1110 1001 0001 --> 0001 11rd dddd rrrr --> ADC Rd,Rr    d->0+9(0 1001)=9,   r->0+11(1 0001)=17
    0x91, 0x01, //0x0191 --> 0000 0001 1001 0001 --> 0000 0001 dddd rrrr --> MOVW Rd,Rr   d->0+9(1001)=9    r->0+1(0001)=1
    0x17, 0x51, //0x5117 --> 0101 0001 0001 0111 --> 0101 KKKK dddd KKKK --> SUBI Rd,K    d->16+1(0001)=17  K->0+17(0001 0111)=23
    0xF4, 0x0A, //0x0AF4 --> 0000 1010 1111 0100 --> 0000 10rd dddd rrrr --> SBC Rd,Rr    d->0+F(0 1111)=15   r->0+14(1 0100)=20
    0x2F, 0x0A, //0x0A2F --> 0000 1010 0010 1111 --> 0000 10rd dddd rrrr --> SBC Rd,Rr    d->0+2(0 0010)=2    r->0+1F(1 1111)=31
    0x95, 0x1C, //0x1C95 --> 0001 1100 1001 0101 --> 0001 11rd dddd rrrr --> ADC Rd,Rr    d->0+9(0 1001)=9,   r->0+5(0 0101)=5
    
    0x2F, 0x65, //0x652F --> 0110 0101 0010 1111 --> 0110 KKKK dddd KKKK --> ORI Rd,K     d->16+2(0010)=18, K->0+5F(0101 1111)=95
                                                  // 0110 KKKK dddd KKKK --> SBR Rd,K     d->16+2(0010)=18, K->0+5F(0101 1111)=95
                                                  
    0x01, 0x17, //0x1701 --> 0001 0111 0000 0001 --> 0001 01rd dddd rrrr --> CP Rd,Rr     d->0+10(1 0000)=16, r->0+11(1 0001)=17
    
    0xB9, 0xF7, //0xF7B9 --> 1111 0111 1011 1001 --> 1111 01kk kkkk ksss --> BRBC s,k     s->0+1(001)=1, K->-64+77(11 1011 1)=37
                                                  // 1111 01kk kkkk k001 --> BRNE k    
    
    0x0B, 0x2F, //0x2F0B --> 0010 1111 0000 1011 --> 0010 11rd dddd rrrr --> MOV Rd,Rr    d->0+10(1 0000)=16, r->0+1B(1 1011)=27
    0x1D, 0x2F, //0x2F1D --> 0010 1111 0001 1101 --> 0010 11rd dddd rrrr --> MOV Rd,Rr    d->0+11(1 0001)=17, r->0+1D(1 1101)=29
    0x01, 0x17, //0x1701 --> 0001 0111 0000 0001 --> 0001 01rd dddd rrrr --> CP Rd,Rr     d->0+10(1 0000)=16, r->0+11(1 0001)=17
    
    0x99, 0xF7, //0xF799 --> 1111 0111 1001 1001 --> 1111 01kk kkkk ksss --> BRBC s,k     s->0+1(001)=1, K->-64+73(11 1001 1)=51
                                                  // 1111 01kk kkkk k001 --> BRNE k   
    
    0x03, 0x94, //0x9403 --> 1001 0100 0000 0011 --> 1001 010d dddd 0011 --> INC Rd        d->0+0(0 0000)=0
    0x00, 0x00  //0x0000 --> 0000 0000 0000 0000 --> 0000 0000 0000 0000 --> NOP
    
};

const uint16_t inst16_table[] = {
    0x2400,  // CLR 
    0xE0A0,  // LDI 
    0xE0B2,  // LDI 
    0x910D,  // LD
    0x3000,  // CPI
    0xF7E9,  // BRNE next1
    0x9711,  // SBIW
    0xE0C0,  // LDI
    0xD2C4,  // RCALL
    0x09E0,  // SBC
    0x1E91,  // ADC
    0x0191,  // MOVW
    0x5117,  // SUBI
    0x0AF4,  // SBC
    0x0A2F,  // SBC
    0x1C95,  // ADC
    0x652F,  // SBR
    0x1701,  // CP
    0xF7B9,  // BRNE next2
    0x2F0B,  // MOV
    0x2F1D,  // MOV
    0x1701,  // CP
    0xF799,  // BRNE next2
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
        uint16_t k7:7;
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
        // 4. LD
        else if ((instruction->op16 & 0xFE0F) == 0x900D) {
            printf("LD R%d, X+\n", instruction->type1.d5);
        } 
        // 5. CPI
        else if ((instruction->op16 & 0xF000) == 0x3000) {
            int Rd = 16 + instruction->type4.d4;
            int K = (instruction->type4.k4_1 << 4) | instruction->type4.k4;
            printf("CPI R%d,%d\n", Rd, K);
        } 
        // 6. BRNE (Varios saltos a etiquetas)
        else if ((instruction->op16 & 0xFC07) == 0xF401) {
            printf("BRNE etiqueta\n");
        } 
        // 7. SBIW
        else if ((instruction->op16 & 0xFF00) == 0x9700) {
            int Rd = 24 + (instruction->type5.d2 * 2);
            int K = (instruction->type5.k2 << 4) | instruction->type5.k4;
            printf("SBIW R%d,%d\n", Rd, K);
        } 
        // 8. RCALL
        else if ((instruction->op16 & 0xF000) == 0xD000) {
            printf("RCALL etiqueta\n");
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
        else {
            printf("unknown (0x%04X)\n", instruction->op16);
        }
    }
    return 0;
}
