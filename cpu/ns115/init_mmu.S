#include "sec_page_property.inc"
#define DDR2_SEC_INDEX     0x800
#define AHB_SUB_SEC_INDEX  0x52
#define APB_SEC_INDEX      0x51
#define AXI_AHB_MAIN_SEC_INDEX	0x50
#define AHB2APB_SEC_INDEX	0x40
#define IRAM_SEC_INDEX		0x20
#define BOOT_FLASH_SEC_INDEX	0x10
#define BOOT_ROM_SEC_INDEX	0x0

//// --------------------------------------------
//// init mmu
//// Set translation table base and generate page table
//// Build a flat translation table for the whole address space.
	.global init_mmu
init_mmu:

        MRC     p15,0,r0,c1,c0,0
        MOV     r1, #0x800000      // bit 23 [XP] Specify v6 format pagetables with no subpages
        ORR     r0,r0,r1
        MCR     p15,0,r0,c1,c0,0

        MOV     r0,#0x0  // Cortex-A9 supports two translation tables, 0 indicates we are using TTB register 0.
        MCR     p15, 0, r0, c2, c0, 2

        .global _TTB_BASE	//IMPORT ||Image$$TTB$$ZI$$Base||   // from scatter file.
        LDR     r0,=_TTB_BASE
        MOV     r1, #0x08                 // RGN=b01  (outer cacheable write-back cached, write allocate)
                                                        // S=0      (translation table walk to non-shared memory)
        ORR     r1,r1,#0x40                             // IRGN=b01 (inner cacheability for the translation table walk is Write-back Write-allocate)
                                                        // IRGN=b00 (inner cacheability for the translation table walk is non-cacheable)  
        ORR     r0,r0,r1                                // write the address of our page table base to TTB register 0.
        MCR     p15, 0, r0, c2, c0, 0 


        LDR     r0,=_TTB_BASE	         // r0: page table base
        LDR     r1,=0xfff                               // r1: loop counter
        LDR     r2,=SEC_DESC_RW_RW_CBA                  // r2: section page property descriptor
							// Privileged-RW, User-RW, Cacheable, Bufferable, write-allocate

        // use loop counter to create 4096 individual table entries.
	// if you add any device into TTB, be sure where it place before or after periphpage
	
	// DDR2(0x800~0xFFF) normal memory
init_ttb_ddr2:
        ORR     r3, r2, r1, LSL#20                      // r3: page address(page_index*1M) + page descriptor
        STR     r3, [r0, r1, LSL#2]        		// store to r0+r1*4 = TTBBASE + page_index*4
        SUBS    r1, r1, #1                              // page_index--
        CMP     r1, #DDR2_SEC_INDEX                     // check if the 1st DDR2 page has come
        BGE     init_ttb_ddr2                           // if not, back

	// (0x53~0x7FF) fault page
	MOV	r4, #FAULT				// Fault page
init_ttb_fault1:
        ORR     r3, r4, r1, LSL#20                      // page_index*1M + page descriptor
        STR     r3, [r0, r1, LSL#2]                     // store to r0+r1*4 = TTBBASE + page_index*4
        SUBS    r1, r1, #1                              // page_index--
        CMP     r1, #AHB_SUB_SEC_INDEX			// check if the AHB_Sub page has come
        BGT     init_ttb_fault1                         // if not, back
	
	// AHB_Sub(0x52), APB(0x51), AXI&AHB_Main(0x50), (0x41~0x4F), AHB2APB(0x40) device memory page
        LDR     r5,=SEC_DESC_RW_NA_B			// Privileged-RW, User-NA, Bufferable
init_ttb_device:
        ORR     r3, r5, r1, LSL#20                      // page_index*1M + page descriptor
        STR     r3, [r0, r1, LSL#2]                     // store to r0+r1*4 = TTBBASE + page_index*4
        SUBS    r1, r1, #1                              // page_index--
        CMP     r1, #AHB2APB_SEC_INDEX			// check if the AHB2APB page has come
        BGE     init_ttb_device                         // if not, back
	
	// (0x21~0x3F) fault page
init_ttb_fault3:
        ORR     r3, r4, r1, LSL#20                      // page_index*1M + page descriptor
        STR     r3, [r0, r1, LSL#2]                     // store to r0+r1*4 = TTBBASE + page_index*4
        SUBS    r1, r1, #1                              // page_index--
        CMP     r1, #IRAM_SEC_INDEX			// check if the IRAM page has come
        BGT     init_ttb_fault3                         // if not, back

	// IRAM(0x20) normal memory page	
        LDR     r4, =SEC_DESC_RW_RW_CB			// this section includes TTB: Privileged-RW, User-RW, Cacheable, Bufferable, no write-allocate
        ORR     r3, r4, r1, LSL#20                      // page_index*1M + page descriptor
        STR     r3, [r0, r1, LSL#2]                     // store to r0+r1*4 = TTBBASE + page_index*4
        SUBS    r1, r1, #1                              // page_index--

	// BOOT Memory(0x0~0x1F) normal memory page
init_ttb_boot_mem:
        ORR     r3, r2, r1, LSL#20                      // normal memory page
        STR     r3, [r0, r1, LSL#2]                     
        SUBS    r1, r1, #1                              // page_index--
        CMP     r1, #0                                  // check if the 1st page has come
        BGE     init_ttb_boot_mem                       // if not, back

	// Set the highest address of DDR2 memory is noncacheable
        LDR     r1,=0xfff                               // r1: loop counter
        LDR     r5,=SEC_DESC_RW_NA_B			// Privileged-RW, User-NA, Bufferable
        ORR     r3, r5, r1, LSL#20                      // page_index*1M + page descriptor
        STR     r3, [r0, r1, LSL#2]                     // store to r0+r1*4 = TTBBASE + page_index*4
	
        MRC     p15, 0, r0, c3, c0, 0                   // Read Domain Access Control Register
        LDR     r0, =0x55555555                         // Enable every domain entry to b01 (client mode)
        MCR     p15, 0, r0, c3, c0, 0                   // Write Domain Access Control Register          
	
	BX lr
