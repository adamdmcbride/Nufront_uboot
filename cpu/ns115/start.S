/*
 *  armboot - Startup Code for unknown CPU-core
 *
 *  Copyright (c) 2003  Texas Instruments
 *
 *  ----- Adapted for OMAP1610 OMAP730 from ARM925t code ------
 *
 *  Copyright (c) 2001	Marius Gröger <mag@sysgo.de>
 *  Copyright (c) 2002	Alex Züpke <azu@sysgo.de>
 *  Copyright (c) 2002	Gary Jennejohn <gj@denx.de>
 *  Copyright (c) 2003	Richard Woodruff <r-woodruff2@ti.com>
 *  Copyright (c) 2003	Kshitij <kshitij@ti.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


#include <config.h>
#include <version.h>

/*
 *************************************************************************
 *
 * Jump vector table
 *
 *************************************************************************
 */

.globl _start
_start:
	b	start_code
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

.global _nufront_serial_no
_nufront_serial_no:
    .fill 2, 128, 0

_undefined_instruction:
	.word undefined_instruction
_software_interrupt:
	.word software_interrupt
_prefetch_abort:
	.word prefetch_abort
_data_abort:
	.word data_abort
_not_used:
	.word not_used
_irq:
	.word irq
_fiq:
	.word fiq

	.balignl 16,0xdeadbeef

/*
 *************************************************************************
 *
 * Startup Code (called from the ARM reset exception vector)
 *
 * do important init only if we don't start from memory!
 * setup memory and board specific bits prior to relocation.
 * relocate armboot to ram
 * setup stack
 *
 *************************************************************************
 */

_TEXT_BASE:
	.word	TEXT_BASE /* address of _start in the linked image */

.globl _armboot_start
_armboot_start:
	.word _start


.global _xloader_boot_mode
_xloader_boot_mode:
    .fill 4, 1, 0

/*
 * These are defined in the board-specific linker script.
 */
.globl _bss_start
_bss_start:
	.word __bss_start

.globl _bss_end
_bss_end:
	.word _end

#ifdef CONFIG_USE_IRQ
/* IRQ stack memory (calculated at run-time) */
.globl IRQ_STACK_START
IRQ_STACK_START:
	.word	0x0badc0de

/* IRQ stack memory (calculated at run-time) */
.globl FIQ_STACK_START
FIQ_STACK_START:
	.word 0x0badc0de
#endif


/*
 * the actual start code
 */
.globl start_code
start_code:

    ldr r1, =_xloader_boot_mode
    str r0, [r1]

	/*
	 * set the cpu to SVC32 mode
	 */
	mrs	r0,cpsr
	bic	r0,r0,#0x1f
	orr	r0,r0,#0xd3
	msr	cpsr,r0


relocate:				/* relocate U-Boot to RAM	    */
	adr	r0, _start		/* pc relative  address of label    */
	ldr	r1, _TEXT_BASE		/* linked image address of label    */
	cmp	r0, r1                  /* test if we run from flash or RAM */
	beq	stack_setup             /* ifeq we are in the RAM copy      */

	ldr	r2, _armboot_start
	ldr	r3, _bss_start
	sub	r2, r3, r2		/* r2 <- size of armboot            */
	add	r2, r0, r2		/* r2 <- source end address         */

copy_loop:
	ldmia	r0!, {r3-r10}		/* copy from source address [r0]    */
	stmia	r1!, {r3-r10}		/* copy to   target address [r1]    */
	cmp	r0, r2			/* until source end addreee [r2]    */
	ble	copy_loop

	/* Set up the stack						    */
stack_setup:
	ldr	r0, _TEXT_BASE		/* upper 128 KiB: relocated uboot   */
	sub	r0, r0, #CONFIG_SYS_MALLOC_LEN	/* malloc area                      */
	sub	r0, r0, #CONFIG_SYS_GBL_DATA_SIZE /* bdinfo                        */
#ifdef CONFIG_USE_IRQ
	sub	r0, r0, #(CONFIG_STACKSIZE_IRQ+CONFIG_STACKSIZE_FIQ)
#endif
	sub	sp, r0, #12		/* leave 3 words for abort-stack    */

clear_bss:
	ldr	r0, _bss_start		/* find start of bss segment        */
	ldr	r1, _bss_end		/* stop here                        */
	mov 	r2, #0x00000000		/* clear                            */

clbss_l:str	r2, [r0]		/* clear loop...                    */
	add	r0, r0, #4
	cmp	r0, r1
	ble	clbss_l


	bl	_InitCache
	ldr	pc, _start_armboot

_start_armboot:
	.word start_armboot

/*
 *************************************************************************
 *
 * CPU_init_critical registers
 *
 * setup important registers
 * setup memory timing
 *
 *************************************************************************
 */
	.global _wfi_asm
_wfi_asm:
	.word   0xE320F003	

/*coreb_entry:
        //disable smp mode
        MRC p15,0,r0,c1,c0,1
        BIC r0, r0, #1 << 6                     // Coherency bit
        BIC r0, r0, #1                          // broadcast
        MCR p15,0,r0,c1,c0,1


	.word   0xE320F003	

	LDR r1,=0x05040000

	LDR r0,[r1,#0x10C]		//; Get ID value from CPU interface
	STR r0,[r1,#0x110]		//; Write EOI to CPU interface

	LDR   r1,=0x051C0038
	LDR   r12,[r1]
	BX    r12
*/
	
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
_InitCache:
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
        

//	MRC	p15, 0, r0, c1, c0, 0	// Read System Control Register
//	ORR	r0, r0, #(0x1 <<12)	// enable I Cache
//	ORR	r0, r0, #(0x1 <<11)	// enable program flow prediction
//	MCR	p15, 0, r0, c1, c0, 0	// Write System Control Register

      
        BX      lr                        // branch to __main  C library entry point
//===================================================================
// init mmu and generate page table
//===================================================================
	.global init_mmu
	push {lr}
	BL init_mmu
	pop  {lr}
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




/*
 *************************************************************************
 *
 * Interrupt handling
 *
 *************************************************************************
 */

@
@ IRQ stack frame.
@
#define S_FRAME_SIZE	72

#define S_OLD_R0	68
#define S_PSR		64
#define S_PC		60
#define S_LR		56
#define S_SP		52

#define S_IP		48
#define S_FP		44
#define S_R10		40
#define S_R9		36
#define S_R8		32
#define S_R7		28
#define S_R6		24
#define S_R5		20
#define S_R4		16
#define S_R3		12
#define S_R2		8
#define S_R1		4
#define S_R0		0

#define MODE_SVC 0x13
#define I_BIT	 0x80

/*
 * use bad_save_user_regs for abort/prefetch/undef/swi ...
 * use irq_save_user_regs / irq_restore_user_regs for IRQ/FIQ handling
 */

	.macro	bad_save_user_regs
	@ carve out a frame on current user stack
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}	@ Save user registers (now in svc mode) r0-r12

	ldr	r2, _armboot_start
	sub	r2, r2, #(CONFIG_STACKSIZE+CONFIG_SYS_MALLOC_LEN)
	sub	r2, r2, #(CONFIG_SYS_GBL_DATA_SIZE+8)  @ set base 2 words into abort stack
	@ get values for "aborted" pc and cpsr (into parm regs)
	ldmia	r2, {r2 - r3}
	add	r0, sp, #S_FRAME_SIZE		@ grab pointer to old stack
	add	r5, sp, #S_SP
	mov	r1, lr
	stmia	r5, {r0 - r3}	@ save sp_SVC, lr_SVC, pc, cpsr
	mov	r0, sp		@ save current stack into r0 (param register)
	.endm

	.macro	irq_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			@ Calling r0-r12
	@ !!!! R8 NEEDS to be saved !!!! a reserved stack spot would be good.
	add	r8, sp, #S_PC
	stmdb	r8, {sp, lr}^		@ Calling SP, LR
	str	lr, [r8, #0]		@ Save calling PC
	mrs	r6, spsr
	str	r6, [r8, #4]		@ Save CPSR
	str	r0, [r8, #8]		@ Save OLD_R0
	mov	r0, sp
	.endm

	.macro	irq_restore_user_regs
	ldmia	sp, {r0 - lr}^			@ Calling r0 - lr
	mov	r0, r0
	ldr	lr, [sp, #S_PC]			@ Get PC
	add	sp, sp, #S_FRAME_SIZE
	subs	pc, lr, #4		@ return & move spsr_svc into cpsr
	.endm

	.macro get_bad_stack
	ldr	r13, _armboot_start		@ setup our mode stack
	sub	r13, r13, #(CONFIG_STACKSIZE+CONFIG_SYS_MALLOC_LEN)
	sub	r13, r13, #(CONFIG_SYS_GBL_DATA_SIZE+8) @ reserved a couple spots in abort stack

	str	lr, [r13]	@ save caller lr in position 0 of saved stack
	mrs	lr, spsr	@ get the spsr
	str	lr, [r13, #4]	@ save spsr in position 1 of saved stack
	mov	r13, #MODE_SVC	@ prepare SVC-Mode
	@ msr	spsr_c, r13
	msr	spsr, r13	@ switch modes, make sure moves will execute
	mov	lr, pc		@ capture return pc
	movs	pc, lr		@ jump to next instruction & switch modes.
	.endm

	.macro get_irq_stack			@ setup IRQ stack
	ldr	sp, IRQ_STACK_START
	.endm

	.macro get_fiq_stack			@ setup FIQ stack
	ldr	sp, FIQ_STACK_START
	.endm

/*
 * exception handlers
 */
	.align  5
.globl undefined_instruction
undefined_instruction:
	get_bad_stack
	bad_save_user_regs
	bl	do_undefined_instruction

	.align	5
.globl software_interrupt
software_interrupt:
	get_bad_stack
	bad_save_user_regs
	bl	do_software_interrupt

	.align	5
.globl prefetch_abort
prefetch_abort:
	get_bad_stack
	bad_save_user_regs
	bl	do_prefetch_abort

	.align	5
.globl data_abort
data_abort:
	get_bad_stack
	bad_save_user_regs
	bl	do_data_abort

	.align	5
.globl not_used
not_used:
	get_bad_stack
	bad_save_user_regs
	bl	do_not_used

#ifdef CONFIG_USE_IRQ
	.align	5
.globl irq
irq:
	get_irq_stack
	irq_save_user_regs
	bl 	do_irq
	irq_restore_user_regs

	.align	5
.globl fiq
fiq:
	get_fiq_stack
	/* someone ought to write a more effiction fiq_save_user_regs */
	irq_save_user_regs
	bl 	do_fiq
	irq_restore_user_regs

#else

	.align	5
.globl irq
irq:
	get_bad_stack
	bad_save_user_regs
	bl	do_irq

	.align	5
.globl fiq
fiq:
	get_bad_stack
	bad_save_user_regs
	bl	do_fiq

#endif


#include "sec_page_property.inc"
#define DDR_START_INDEX     0x800   //0x80000000 ~ 0xFFFFFFFF is ddr
#define EMPTY1_START_INDEX     0x081   //0x08100000 ~ 0x7FFFFFFF is empty
#define DEV_START_INDEX     0x040   //0x04000000 ~ 0x080FFFFF is device
#define EMPTY2_START_INDEX     0x000   //0x00000000 ~ 0x039FFFFF is device
//#define AHB_SUB_SEC_INDEX  0x62
//#define APB_SEC_INDEX      0x51
//#define AXI_AHB_MAIN_SEC_INDEX	0x50
//#define AHB2APB_SEC_INDEX	0x40
//#define IRAM_SEC_INDEX		0x20
//#define BOOT_FLASH_SEC_INDEX	0x10
//#define BOOT_ROM_SEC_INDEX	0x0

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
	
init_ttb_ddr:
        ORR     r3, r2, r1, LSL#20                      // r3: page address(page_index*1M) + page descriptor
        STR     r3, [r0, r1, LSL#2]        		// store to r0+r1*4 = TTBBASE + page_index*4
        SUBS    r1, r1, #1                              // page_index--
        CMP     r1, #DDR_START_INDEX                     // check if the 1st DDR2 page has come
        BGE     init_ttb_ddr                           // if not, back

	MOV	r4, #FAULT				// Fault page
init_ttb_fault1:
        ORR     r3, r4, r1, LSL#20                      // page_index*1M + page descriptor
        STR     r3, [r0, r1, LSL#2]                     // store to r0+r1*4 = TTBBASE + page_index*4
        SUBS    r1, r1, #1                              // page_index--
        CMP     r1, #EMPTY1_START_INDEX			// check if the AHB_Sub page has come
        BGT     init_ttb_fault1                         // if not, back
	
        LDR     r5,=SEC_DESC_RW_NA_B			// Privileged-RW, User-NA, Bufferable
init_ttb_device:
        ORR     r3, r5, r1, LSL#20                      // page_index*1M + page descriptor
        STR     r3, [r0, r1, LSL#2]                     // store to r0+r1*4 = TTBBASE + page_index*4
        SUBS    r1, r1, #1                              // page_index--
        CMP     r1, #DEV_START_INDEX			// check if the AHB2APB page has come
        BGE     init_ttb_device                         // if not, back
	
	// (0x21~0x3F) fault page
init_ttb_fault2:
        ORR     r3, r4, r1, LSL#20                      // page_index*1M + page descriptor
        STR     r3, [r0, r1, LSL#2]                     // store to r0+r1*4 = TTBBASE + page_index*4
        SUBS    r1, r1, #1                              // page_index--
        CMP     r1, #EMPTY2_START_INDEX			// check if the IRAM page has come
        BGE     init_ttb_fault2                         // if not, back


        MRC     p15, 0, r0, c3, c0, 0                   // Read Domain Access Control Register
        LDR     r0, =0x55555555                         // Enable every domain entry to b01 (client mode)
        MCR     p15, 0, r0, c3, c0, 0                   // Write Domain Access Control Register          
	
	BX lr
