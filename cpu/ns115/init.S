// Standard definitions of mode bits and interrupt (I & F) flags in PSRs

#define Mode_USR        0x10
#define Mode_FIQ        0x11
#define Mode_IRQ        0x12
#define Mode_SVC        0x13
#define Mode_ABT        0x17
#define Mode_UNDEF      0x1B
#define Mode_SYS        0x1F
  
#define EQU             0x80 // when I bit is set, IRQ is disabled
#define F_Bit           0x40 // when F bit is set, FIQ is disabled
.global  _InitCache
_InitCahce:
//==================================================================
// if MMU/MPU enabled - disable 
//==================================================================
        MRC     p15, 0, r0, c1, c0, 0	// read CP15 register 1
        BIC     r0, r0, #0x1		// clear bit 0
        MCR     p15, 0, r0, c1, c0, 0	// write value back

 //===================================================================        
// Invalidate instruction cache
//===================================================================     
        MOV	r0, #0			// SBZ
        MCR	p15, 0, r0, c7, c5, 0	// Invalidate all instruction caches
        
//===================================================================        
// Invalidate data cache
//===================================================================     
	MOV	r0, #0
	MCR	p15, 2, r0, c0, c0, 0	// Cache size selection register, select data cache
        MRC	p15, 1, r3, c0, c0, 0	// Read cache size from the Cache Size Identification Register
        ldr	r1, =0x1FF
        AND	r3, r1, r3, LSR #13	// r3 = (number of sets - 1)
        MOV	r0, #0			// r0 -> way counter
way_loop:
        MOV	r1, #0			// r1 -> set counter
set_loop:
        MOV	r2, r0, LSL #30		//
        ORR	r2, r1, LSL #5		// r2 -> set/way cache-op format
        MCR	p15, 0, r2, c7, c6, 2	// Invalidate line described by r2
        ADD	r1, r1, #1		// Increment set counter
        CMP	r1, r3			// Check if the last set is reached...
        BLE	set_loop		// ...if not, continue the set_loop...
        ADD	r0, r0, #1		// ...else, Increment way counter
        CMP	r0, #4			// Check if the last way is reached...
        BLT	way_loop		// ...if not, continue the way_loop
   
//===================================================================        
// Invalidate entire Unified main TLB
//===================================================================
	MCR	p15, 0, r0, c8, c7, 0	// (Note: r0 value is ignored)
               
//===================================================================
// init mmu and generate page table
//===================================================================
	.global init_mmu
	BL init_mmu

//==================================================================
// Enable caches
// Enable Program Flow Prediction
// Enable D-side prefetch
//==================================================================
	MRC	p15, 0, r0, c1, c0, 0	// Read System Control Register
	ORR	r0, r0, #(0x1 <<12)	// enable I Cache
	ORR	r0, r0, #(0x1 <<2)	// enable D Cache
	ORR	r0, r0, #(0x1 <<11)	// enable program flow prediction
	MCR	p15, 0, r0, c1, c0, 0	// Write System Control Register

	MRC	p15, 0, r0, c1, c0, 1	// Read Auxiliary Control Register
	ORR	r0, r0, #(0x1 <<2)	// Enable L1 Dside prefetch
	MCR	p15, 0, r0, c1, c0, 1	// Write Auxiliary Control Register

//===================================================================
// Enable MMU
//===================================================================
	MRC     p15, 0, r0, c1, c0, 0      // read CP15 register 1
	ORR     r0, r0, #0x1               // enable MMU before scatter loading
        MCR     p15, 0, r0, c1, c0, 0      // write CP15 register 1

//==================================================================
// init L2 cache controller
//==================================================================
//	IMPORT init_l2cc
//	BL init_l2cc

//==================================================================
// Enbale some properties in Cortex-A9 about L2
//==================================================================
//	MRC     p15, 0, r0, c1, c0, 1       // Read Auxiliary Control Register
//	ORR     r0, r0, #(0x1 <<1)          // Enable L2 prefetch
//	ORR     r0, r0, #(0x1 <<3)          // Enable write full line of zeros
//	ORR     r0, r0, #(0x1 <<7)          // Enable exclusive cache mode
//	MCR     p15, 0, r0, c1, c0, 1       // Write Auxiliary Control Register

//===================================================================
// Initialize genrenal interrupt controller if you want
//===================================================================
//	IMPORT init_gic
//	BL init_gic

//===================================================================
// Initialize DDR 
//===================================================================
//	IMPORT ddr_init
//	BL ddr_init

//==================================================================
// Now the MMU is enabled, virtual to physical address translations will occur. This will affect the next
// instruction fetch.
//
// The two instructions currently in the ARM pipeline will have been fetched before the MMU was enabled.
// This property is useful because the next two instructions are safe even if new instruction fetches fail -
// If this routine was mapped out of the new virtual memory map, the branch to __main would still succeed.

        BX      lr                        // branch to __main  C library entry point



