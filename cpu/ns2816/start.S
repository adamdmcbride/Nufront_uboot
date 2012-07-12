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
	/*
	 * set the cpu to SVC32 mode
	 */
	mrs	r0,cpsr
	bic	r0,r0,#0x1f
	orr	r0,r0,#0xd3
	msr	cpsr,r0

	/*
	 * we do sys-critical inits only at reboot,
	 * not when booting from ram!
	 */

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

	bl	cpu_init_crit
	BL  __set_page_table

	BL  __config_mmu
	BL  __set_ttb0

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
	

	.global cpu_init_crit
cpu_init_crit:
	/* Assumes the ARM boot monitor, or user software,
	 * has initialized the platform
	 * To run direct from reset any such 
	 * initialisation code must be added here
	 */

	//disable SCU
	LDR r0, =0x05040000
	LDR r1, [r0,#0]
	BIC r1, #0x1
	STR r1, [r0,#0]
	


	//disable smp mode
	MRC p15,0,r0,c1,c0,1	
	BIC r0, r0, #1 << 6			// Coherency bit
	BIC r0, r0, #1				// broadcast
	MCR p15,0,r0,c1,c0,1

	//wake up cpu 1 to do the same
	//LDR   r1,=0x051C0038
	//LDR   r0,=coreb_entry
	//STR   r0, [r1]
	
	//MRC p15,4,r0,c15,c0,0   //CONFIGBASE
        //ADD r1,r0,#0x1000       //0x1F001000

        //setup ID0 Priority
        //LDRB r0,[r1,#0x400]
        //AND r0,r0,#0
        //STRB r0,[r1,#0x400]

        //LDR r0,=0x00020000      // Target is CPU 1, with ID0
        //STR r0,[r1,#0xF00]      // Write SGI register to send cpu1 an interrupt ;0x1F001F00
	


//if MMU/MPU enabled - disable 
//=================================================================
        MRC     p15, 0, r0, c1, c0, 0	//; read CP15 register 1
        BIC     r0, r0, #0x1		//; clear bit 0
        MCR     p15, 0, r0, c1, c0, 0	//; write value back

//===================================================================        
// Invalidate instruction cache
//===================================================================     
        MOV	r0, #0			//; SBZ
        MCR	p15, 0, r0, c7, c5, 0	//; Invalidate all instruction caches
        
//===================================================================        
// Invalidate data cache
//===================================================================     
/***********************
        MRC	p15, 1, r3, c0, c0, 0	//; Read cache size from the Cache Size Identification Register
        MOV	r1, #0x200
	SUB     r1, r1,#1
        AND	r3, r1, r3, LSR #13	//; r3 = (number of sets - 1)
        MOV	r0, #0			//; r0 -> way counter
way_loop:
        MOV	r1, #0			//; r1 -> set counter
set_loop:
        MOV	r2, r0, LSL #30		//;
        ORR	r2, r1, LSL #5		//; r2 -> set/way cache-op format
        MCR	p15, 0, r2, c7, c6, 2	//; Invalidate line described by r2
        ADD	r1, r1, #1		//; Increment set counter
        CMP	r1, r3			//; Check if the last set is reached...
        BLE	set_loop		//; ...if not, continue the set_loop...
        ADD	r0, r0, #1		//; ...else, Increment way counter
        CMP	r0, #4			//; Check if the last way is reached...
        BLT	way_loop		//; ...if not, continue the way_loop	mov	pc, lr	
*/

//;===================================================================        
//; Invalidate entire Unified main TLB
//;===================================================================
	MCR	p15, 0, r0, c8, c7, 0	//; (Note: r0 value is ignored)
               

//==================================================================
// Enable caches
// Enable Program Flow Prediction
// Enable D-side prefetch
//==================================================================
	MRC	p15, 0, r0, c1, c0, 0	//; Read System Control Register
	ORR	r0, r0, #(0x1 <<12)	//; enable I Cache
	ORR	r0, r0, #(0x1 <<2)	//; enable D Cache
	ORR	r0, r0, #(0x1 <<11)	//; enable program flow prediction
	MCR	p15, 0, r0, c1, c0, 0	//; Write System Control Register

	MRC	p15, 0, r0, c1, c0, 1	//; Read Auxiliary Control Register
	ORR	r0, r0, #(0x1 <<2)	//; Enable L1 Dside prefetch
	MCR	p15, 0, r0, c1, c0, 1	///; Write Auxiliary Control Register


//===================================================================
// Enable MMU
//===================================================================
//	MRC     p15, 0, r0, c1, c0, 0      //; read CP15 register 1
//      ORR     r0, r0, #0x1               //; enable MMU before scatter loading
//      MCR     p15, 0, r0, c1, c0, 0      //; write CP15 register 1

	MOV 	pc,lr


/*;===================================================================        
; Create page table in ram 
;
; 31                 20 19  18  17  16 15  14   12 11 10  9  8     5   4    3 2   1 0
; |section base address| 0  0  |nG| S |AP2|  TEX  |  AP | P | Domain | XN | C B | 1 0|
;
; Bits[31:20]   - Top 12 bits of VA is pointer into table
; nG[17]=0      - Non global, enables matching against ASID in the TLB when set.
; S[16]=0       - Indicates normal memory is shared when set.
; AP2[15]=0  
; AP[11:10]=11  - Configure for full read/write acess in all modes
; TEX[14:12]=001
; CB[3:2]= 00   - Set attributes to normal memory, outer and inner non-cacheable.
;                 (except for the first descriptor, see below)
; IMPP[9]=0     - Ignored
; Domain[5:8]=1111   - Set all pages to use domain 15
; XN[4]=0       - Execute never disabled
; Bits[1:0]=10  - Indicate entry is a 1MB section
;===================================================================*/
__set_page_table:

//	IMPORT 	||Image$$TTB$$ZI$$Base||

	//LDR     r0,=||Image$$TTB$$ZI$$Base||
	LDR     r0, =0xBFF00000
        LDR     r1,=0xfff                  //; loop counter
        LDR     r2,=0x00000DEE  //2_00000000000000000000110111100010

        //; r0 contains the address of the translation table base
        //; r1 is loop counter
        //; r2 is level1 descriptor (bits 19:0)

        //; use loop counter to create 4096 individual table entries.
        //; this writes from address 'Image$$TTB$$ZI$$Base' + 
        //; offset 0x3FFC down to offset 0x0 in word steps (4 bytes)

init_ttb_1:

        ORR     r3, r2, r1, LSL#20         //; r3 now contains full level1 descriptor to write
        STR     r3, [r0, r1, LSL#2]        //; str table entry at TTB base + loopcount*4
        SUBS    r1, r1, #1                 //; decrement loop counter
        BPL     init_ttb_1
	
	BX	r14


__config_mmu:
        MRC     p15,0,r0,c1,c0,0
        MOV     r1, #0x800000
        ORR     r0,r0,r1
        MCR     p15,0,r0,c1,c0,0

        //; Cortex-A9 supports two translation tables
        //; Configure translation table base (TTB) control register cp15,c2
        //; to a value of all zeros, then N = 0, indicates we are using TTB register 0.

        MOV     r0,#0x0
        MCR     p15, 0, r0, c2, c0, 2
	
	BX	r14

//;===================================================================        
//; Setup TTB0
//;===================================================================      
__set_ttb0:     
        LDR     r0,=0xBFF00000  //||Image$$TTB$$ZI$$Base||		   ; our TTB base
        MOV     r1, #0x08        //          ; RGN=b01  (outer cacheable write-back cached, write allocate)
                                   //        ; S=0      (translation table walk to non-shared memory)
        ORR     r1,r1,#0x40            //    ; IRGN=b01 (inner cacheability for the translation table walk is Write-back Write-allocate)
                                     //      ; IRGN=b00 (inner cacheability for the translation table walk is non-cacheable)  
        ORR     r0,r0,r1                    
        MCR     p15, 0, r0, c2, c0, 0     

	BX	r14   

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
