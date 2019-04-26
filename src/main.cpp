#include <stdio.h>
#include <stdint.h>

#define print printf
#define MEM_SIZE 4096

int32_t mem[MEM_SIZE];

void dump_mem(uint32_t addr, uint32_t wsize);

inline uint32_t get_address(uint32_t address, int32_t kte);
inline int32_t  extend_signal(int32_t word, uint32_t wsize);

inline int32_t  lw (uint32_t address, int32_t kte);
inline int32_t  lh (uint32_t address, int32_t kte);
inline int32_t  lhu(uint32_t address, int32_t kte);
inline int32_t  lb (uint32_t address, int32_t kte);
inline int32_t  lbu(uint32_t address, int32_t kte);

inline int32_t  set_field(int32_t word, uint32_t index, uint32_t mask, int32_t value);
inline void sw(uint32_t address, int32_t kte, int32_t dado);
inline void sh(uint32_t address, int32_t kte, int16_t dado);
inline void sb(uint32_t address, int32_t kte, int8_t dado);

int main(){
    sb(0,0,0x04);
    sb(0,1,0x03);
    sb(0,2,0x02);
    sb(0,3,0x01);

    sb(4,0,0xff);
    sb(4,1,0xfe);
    sb(4,2,0xfd);
    sb(4,3,0xfc);

    sh(8,0,0xfff0);
    sh(8,2,0x8c);

    sw(12,0,0xff);
    sw(16,0,0xffff);
    sw(20,0,0xffffffff);
    sw(24,0,0x80000000);
    dump_mem(0,7);

    print("%02x %d\n", lb(0,0), lb(0,0));
    print("%02x %d\n", lb(0,1), lb(0,1));
    print("%02x %d\n", lb(0,2), lb(0,2));
    print("%02x %d\n", lb(0,3), lb(0,3));
    
    print("%02x %d\n", lb(4,0) & 0xff, lb(4,0));
    print("%02x %d\n", lb(4,1) & 0xff, lb(4,1));
    print("%02x %d\n", lb(4,2) & 0xff, lb(4,2));
    print("%02x %d\n", lb(4,3) & 0xff, lb(4,3));

    print("%d\n", lbu(4,0));
    print("%d\n", lbu(4,1));
    print("%d\n", lbu(4,2));
    print("%d\n", lbu(4,3));

    print("%04x %d\n", lh(8,0) & 0xffff, lh(8,0) & 0xffff);
    print("%04x %d\n", lh(8,2) & 0xffff, lh(8,2) & 0xffff);

    print("%04x %d\n", lhu(8,0) & 0xffff, lhu(8,0) & 0xffff);
    print("%04x %d\n", lhu(8,2) & 0xffff, lhu(8,2) & 0xffff);

    print("%08x %d\n", lw(12,0));
    print("%08x %d\n", lw(16,0));
    print("%08x %d\n", lw(20,0));
    return 0;
}

void dump_mem(uint32_t addr, uint32_t wsize) {
    /**
     * Print \a{wsize} word' content as hex,
     * beginning from \a{addr}
     */
    int32_t konstante = 0;
    while(wsize--)
        // print("0x%x", 0); // print
        { print("mem[%d] = 0x%08x\n", (addr+konstante) >> 2, lw(addr,konstante)); konstante += 4; }
}

inline uint32_t get_address(uint32_t address, int32_t kte){
    /**
     * Calculate the memory address
     */
    return ( address + kte ) >> 2;
    // address//4 + kte
}

inline int32_t  extend_signal(int32_t word, uint32_t wsize){
    if( word >> (wsize-1) )
        return word | (-1 << wsize);
    return word;
}

inline int32_t  lw (uint32_t address, int32_t kte){
    /**
     * Extract a word from memory
     */
    if( address & 0x3 ) //if ( address % 4 != 0 )
        throw "Address not alligned with word boundaries";
    return mem[get_address(address,kte)];
    // mem[address//4 + kte]
}

inline int32_t  lh (uint32_t address, int32_t kte){
    return extend_signal(lhu(address,kte),16);
}

inline int32_t  lhu(uint32_t address, int32_t kte){
    return mem[get_address(address,kte)] >> 8 * ( ( address + kte ) & 0x3 ) & 0xffff;
}

inline int32_t  lb (uint32_t address, int32_t kte){
    return extend_signal(lbu(address,kte),8);
}

inline int32_t  lbu(uint32_t address, int32_t kte){
    /**
     * Extract a byte from a word
     * NOTE: operator precedence:
     * array subscripting([]) -> multiply(*) -> subtract(-) -> shift( >> ) -> bitwise AND (&)
     */
    return mem[get_address(address,kte)] >> 8 * ( (address + kte) & 0x3 ) & 0xff;
    // lw() >> 8 * (address % 4) & 0xff
}

inline int32_t  set_field(int32_t word, uint32_t index, uint32_t mask, int32_t value){
    return (word & ~(mask << index)) | ((value & mask) << index);
}

inline void sw(uint32_t address, int32_t kte, int32_t dado){
    /**
     * Write a word in memory
     */
    mem[get_address(address,kte)] = dado;
}

inline void sh(uint32_t address, int32_t kte, int16_t dado){
    mem[get_address(address,kte)] = set_field(mem[get_address(address,kte)], (kte >> 1)*16, 0xffff, dado);
}

inline void sb(uint32_t address, int32_t kte, int8_t dado){
    mem[get_address(address,kte)] = set_field(mem[get_address(address,kte)], kte*8, 0xff, dado);
}