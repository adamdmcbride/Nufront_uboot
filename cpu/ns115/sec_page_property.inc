// Definitions for section descriptors

#define SEC_NG            (1 << 17)
#define SEC_S             (1 << 16)
#define SEC_AP2           (1 << 15)
#define SEC_TEX0          (1 << 12)
#define SEC_TEX1          (1 << 13)
#define SEC_TEX2          (1 << 14)
#define SEC_AP0           (1 << 10)
#define SEC_AP1           (1 << 11)
#define SEC_P             (1 << 9)
#define SEC_XN            (1 << 4)
#define SEC_C             (1 << 3)
#define SEC_B             (1 << 2)

#define SECTION           2
#define FAULT		  0

#define SEC_SO_MEM        (SEC_S | SECTION)
#define SEC_S_DEVICE      (SEC_S | SEC_B | SECTION)
#define SEC_WT     	  (SEC_C | SECTION)
#define SEC_WB		  (SEC_C | SEC_B | SECTION)
#define SEC_NC		  (SEC_TEX0 | SECTION)
#define SEC_DEVICE 	  (SEC_TEX1 | SECTION)
#define SEC_INOUT	  (SEC_TEX2 | SECTION)

#define SEC_I_NC	  (0)
#define SEC_I_WBWA	  (SEC_B)
#define SEC_I_WT	  (SEC_C)
#define SEC_I_WB	  (SEC_C | SEC_B)
#define SEC_O_NC	  (0)
#define SEC_O_WBWA	  (SEC_TEX0)
#define SEC_O_WT	  (SEC_TEX1)
#define SEC_O_WB	  (SEC_TEX1 | SEC_TEX0)

#define SEC_PROT_N	  (0)
#define SEC_PROT_RW_NA   (SEC_AP0)
#define SEC_PROT_RW_RO	 (SEC_AP1)
#define SEC_PROT_RW_RW	 (SEC_AP1 | SEC_AP0)
#define SEC_PROT_RO_NA	 (SEC_AP2 | SEC_AP0)
#define SEC_PROT_RO_RO	 (SEC_AP2 | SEC_AP1)

#define SEC_DESC_RW_RW_CBA     (SEC_TEX0|SEC_WB|SEC_PROT_RW_RW)    
#define SEC_DESC_RW_RW_CB      (SEC_WB|SEC_PROT_RW_RW)    
#define SEC_DESC_RW_NA_B       (SEC_PROT_RW_NA | SEC_B | SECTION)  
#define SEC_DESC_RW_NA         (SEC_PROT_RW_NA | SECTION)            

// Definitions for page descriptors

#define P_NGLOBAL            (1 << 11)
#define P_SHARED             (1 << 10)
#define P_APX                (1 << 9)
#define P_TEX1               (1 << 6)
#define P_AP1                (1 << 5)
#define P_AP0                (1 << 4)
#define P_CACHE              (1 << 3)
#define P_BUFFER             (1 << 2)
#define IS_A_PAGE            (1 << 1)
#define P_XN                 (1 << 0)

#define IS_A_PAGETABLE       (1 << 0)

#define P_NORMAL                   (P_TEX1 | P_CACHE | P_BUFFER | IS_A_PAGE | P_AP0 | P_AP1)
#define P_SHARED_NORMAL            (P_NORMAL | P_SHARED)
#define P_SHARED_STRONGLY_ORDERED  (P_SHARED | P_AP0 | P_AP1 | IS_A_PAGE)
