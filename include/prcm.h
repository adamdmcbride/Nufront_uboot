#ifndef PRCM_H___
#define PRCM_H___


// Section-1: PRCM/SCU Base Address Definition
#define PRCM_BASE          (volatile void*)(0x05821000)
#define SCU_BASE           (volatile void*)(0x05040000)


// Section-2: PRCM Register Address Definition
#define PRCM_SYS_CLK_CTRL           (volatile void*)((PRCM_BASE) + 0x00)
#define PRCM_SYS_RST_CTRL           (volatile void*)((PRCM_BASE) + 0x04)
#define PRCM_SYS_PWR_CTRL           (volatile void*)((PRCM_BASE) + 0x08)
#define PRCM_NUSTAR_PWR_CTRL        (volatile void*)((PRCM_BASE) + 0x0C)
#define PRCM_SYS_PWR_STATUS         (volatile void*)((PRCM_BASE) + 0x10)
#define PRCM_WAKEUP_IRQ_CTRLk       (volatile void*)((PRCM_BASE) + 0x14)
#define PRCM_ACK_ISO                (volatile void*)((PRCM_BASE) + 0x18)
//                                  (volatile void*)((PRCM_BASE) + 0x1C)
#define PRCM_MISC_REG0              (volatile void*)((PRCM_BASE) + 0x20)
#define PRCM_MISC_REG1              (volatile void*)((PRCM_BASE) + 0x24)
#define PRCM_MISC_REG2              (volatile void*)((PRCM_BASE) + 0x28)
#define PRCM_MISC_REG3              (volatile void*)((PRCM_BASE) + 0x2C)
//                                  (volatile void*)((PRCM_BASE) + 0x30)
//                                  (volatile void*)((PRCM_BASE) + 0x34)
#define PRCM_SYS_PLL_CTRL           (volatile void*)((PRCM_BASE) + 0x38)
#define PRCM_SYS_PLL_STATUS         (volatile void*)((PRCM_BASE) + 0x3C)
#define PRCM_PLL0_CFG0              (volatile void*)((PRCM_BASE) + 0x40)
#define PRCM_PLL0_CFG1              (volatile void*)((PRCM_BASE) + 0x44)
#define PRCM_PLL1_CFG0              (volatile void*)((PRCM_BASE) + 0x48)
#define PRCM_PLL1_CFG1              (volatile void*)((PRCM_BASE) + 0x4C)
#define PRCM_PLL2_CFG0              (volatile void*)((PRCM_BASE) + 0x50)
#define PRCM_PLL2_CFG1              (volatile void*)((PRCM_BASE) + 0x54)
#define PRCM_PLL3_CFG0              (volatile void*)((PRCM_BASE) + 0x58)
#define PRCM_PLL3_CFG1              (volatile void*)((PRCM_BASE) + 0x5C)
#define PRCM_PLL4_CFG0              (volatile void*)((PRCM_BASE) + 0x60)
#define PRCM_PLL4_CFG1              (volatile void*)((PRCM_BASE) + 0x64)
#define PRCM_PLL5_CFG0              (volatile void*)((PRCM_BASE) + 0x68)
#define PRCM_PLL5_CFG1              (volatile void*)((PRCM_BASE) + 0x6C)
#define PRCM_PLL6_CFG0              (volatile void*)((PRCM_BASE) + 0x70)
#define PRCM_PLL6_CFG1              (volatile void*)((PRCM_BASE) + 0x74)
//                                  (volatile void*)((PRCM_BASE) + 0x78)
//                                  (volatile void*)((PRCM_BASE) + 0x7C)
#define PRCM_CPU_CLK_DIV            (volatile void*)((PRCM_BASE) + 0x80)
#define PRCM_CPU_CLK_SWITCH         (volatile void*)((PRCM_BASE) + 0x84)
#define PRCM_CORESIGHT_CLK_CTRL     (volatile void*)((PRCM_BASE) + 0x88)
#define PRCM_DDR_CLK_CTRL           (volatile void*)((PRCM_BASE) + 0x8C)
#define PRCM_BUS_CLK_CTRL           (volatile void*)((PRCM_BASE) + 0x90)
#define PRCM_DMA_CLK_CTRL           (volatile void*)((PRCM_BASE) + 0x94)
#define PRCM_MALI_CLK_CTRL          (volatile void*)((PRCM_BASE) + 0x98)
#define PRCM_GC300_CLK_CTRL         (volatile void*)((PRCM_BASE) + 0x9C)
#define PRCM_VPU_G1_CLK_CTRL        (volatile void*)((PRCM_BASE) + 0xA0)
#define PRCM_VPU_H1_CLK_CTRL        (volatile void*)((PRCM_BASE) + 0xA4)
#define PRCM_ISP_CLK_CTRL           (volatile void*)((PRCM_BASE) + 0xA8)
#define PRCM_SD_CLK_CTRL            (volatile void*)((PRCM_BASE) + 0xAC)
#define PRCM_ZSP_CLK_CTRL           (volatile void*)((PRCM_BASE) + 0xB0)
#define PRCM_SPI_CLK_CTRL           (volatile void*)((PRCM_BASE) + 0xB4)
#define PRCM_I2C_CLK_CTRL           (volatile void*)((PRCM_BASE) + 0xB8)
#define PRCM_UART_CLK_CTRL0         (volatile void*)((PRCM_BASE) + 0xBC)
#define PRCM_UART_CLK_CTRL1         (volatile void*)((PRCM_BASE) + 0xC0)
#define PRCM_DIS0_CLK_CTRL          (volatile void*)((PRCM_BASE) + 0xC4)
#define PRCM_DIS1_CLK_CTRL          (volatile void*)((PRCM_BASE) + 0xC8)
#define PRCM_I2S_CLK_CTRL           (volatile void*)((PRCM_BASE) + 0xCC)
#define PRCM_USB_CLK_CTRL           (volatile void*)((PRCM_BASE) + 0xD0)
#define PRCM_CORE_MEM_CLK_CTRL      (volatile void*)((PRCM_BASE) + 0xD4)
#define PRCM_GPIO_CORE_CLK_CTRL     (volatile void*)((PRCM_BASE) + 0xD8)
#define PRCM_TIMER_CORE_CLK_CTRL    (volatile void*)((PRCM_BASE) + 0xDC)
#define PRCM_WKUP_MEM_CLK_CTRL      (volatile void*)((PRCM_BASE) + 0xE0)
#define PRCM_GPIO_WKUP_CLK_CTRL     (volatile void*)((PRCM_BASE) + 0xE4)
#define PRCM_TIMER_WKUP_CLK_CTRL    (volatile void*)((PRCM_BASE) + 0xE8)
#define PRCM_ALT_CLK_CTRL           (volatile void*)((PRCM_BASE) + 0xEC)
#define PRCM_AUX_CLK_CTRL           (volatile void*)((PRCM_BASE) + 0xF0)


// Section-3: PRCM Register Bit(s) Definition

// PRCM_SYS_CLK_CTRL           
#define PRCM_SYS_CLK_CTRL_PRESERVE_12M                      (1 << 15)
#define PRCM_SYS_CLK_CTRL_STABLE_CNT_MASK                   (0x7F << 8)
#define PRCM_SYS_CLK_CTRL_GPIO_12M_SEL                      (1 << 1)
#define PRCM_SYS_CLK_CTRL_SYSCLK_32KEN                      (1 << 0)

// PRCM_SYS_RST_CTRL       
#define PRCM_SYS_RST_CTRL_NUSTAR_RST_DEBUG1_RST             (1 << 11)
#define PRCM_SYS_RST_CTRL_NUSTAR_RST_DEBUG0_RST             (1 << 10)
#define PRCM_SYS_RST_CTRL_NUSTAR_RST_WATCHDOG1_RST          (1 << 9)
#define PRCM_SYS_RST_CTRL_NUSTAR_RST_WATCHDOG0_RST          (1 << 8)
#define PRCM_SYS_RST_CTRL_NUSTAR_RST_NEON1_RST              (1 << 7)
#define PRCM_SYS_RST_CTRL_NUSTAR_RST_NEON0_RST              (1 << 6)
#define PRCM_SYS_RST_CTRL_NUSTAR_RST_CPU1_RST               (1 << 5)
#define PRCM_SYS_RST_CTRL_NUSTAR_RST_CPU0_RST               (1 << 4)
#define PRCM_SYS_RST_CTRL_WATCHDOG_RST_EN                   (1 << 1)
#define PRCM_SYS_RST_CTRL_GLB_SW_RST                        (1 << 0)

// PRCM_SYS_PWR_CTRL       
#define PRCM_SYS_PWR_CTRL_SYSTEM_PWR_CTRL_EN                (1 << 31)
#define PRCM_SYS_PWR_CTRL_BP_EXT_SUS                        (1 << 30)
#define PRCM_SYS_PWR_CTRL_INTERNAL_SUS                      (1 << 29)
#define PRCM_SYS_PWR_CTRL_PLL_PWR_STATE_MASK                (0x7F << 8)
#define PRCM_SYS_PWR_CTRL_ZSP_PWR_STATE                     (1 << 7)
#define PRCM_SYS_PWR_CTRL_ISP_PWR_STATE                     (1 << 6)
#define PRCM_SYS_PWR_CTRL_VPU_H1_PWR_STATE                  (1 << 5)
#define PRCM_SYS_PWR_CTRL_VPU_G1_PWR_STATE                  (1 << 4)
#define PRCM_SYS_PWR_CTRL_GC300_PWR_STATE                   (1 << 3)
#define PRCM_SYS_PWR_CTRL_MALI_PWR_STATE_PP                 (1 << 2)
#define PRCM_SYS_PWR_CTRL_MALI_PWR_STATE_L2C                (1 << 1)
#define PRCM_SYS_PWR_CTRL_MALI_PWR_STATE_GP                 (1 << 0)

// PRCM_NUSTAR_PWR_CTRL        
#define PRCM_NUSTAR_PWR_CTRL_BP_CLKSTOPPED                  (1 << 31)
#define PRCM_NUSTAR_PWR_CTRL_BP_PWRCTRLO_MASK               (0x3 << 29)
#define PRCM_NUSTAR_PWR_CTRL_BP_IRQ_OUT_MASK                (0x3 << 27)
#define PRCM_NUSTAR_PWR_CTRL_NUSTAR_LS_BP                   (1 << 26)
#define PRCM_NUSTAR_PWR_CTRL_L2_PD                          (1 << 25)
#define PRCM_NUSTAR_PWR_CTRL_NUSTAR_SLEEP_EN                (1 << 8)
#define PRCM_NUSTAR_PWR_CTRL_NUSTAR_PWR_STATE_MASK          (0xFF << 0)
#define PRCM_NUSTAR_PWR_CTRL_L2C_PWR_CTRL		    (1 << 4)
#define PRCM_NUSTAR_PWR_CTRL_FALCON1_PWR_CTRL		    (1 << 3)
#define PRCM_NUSTAR_PWR_CTRL_FALCON0_PWR_CTRL		    (1 << 2)
#define PRCM_NUSTAR_PWR_CTRL_NEON1_PWR_CTRL		    (1 << 1)
#define PRCM_NUSTAR_PWR_CTRL_NEON0_PWR_CTRL		    (1 << 0)

// PRCM_SYS_PWR_STATUS         
#define PRCM_SYS_PWR_STATUS_PLL6_PWRDN                      (1 << 23)
#define PRCM_SYS_PWR_STATUS_PLL5_PWRDN                      (1 << 22)
#define PRCM_SYS_PWR_STATUS_PLL4_PWRDN                      (1 << 21)
#define PRCM_SYS_PWR_STATUS_PLL3_PWRDN                      (1 << 20)
#define PRCM_SYS_PWR_STATUS_PLL2_PWRDN                      (1 << 19)
#define PRCM_SYS_PWR_STATUS_PLL1_PWRDN                      (1 << 18)
#define PRCM_SYS_PWR_STATUS_PLL0_PWRDN                      (1 << 17)
#define PRCM_SYS_PWR_STATUS_ZSP_STATUS                      (1 << 16)
#define PRCM_SYS_PWR_STATUS_ISP_STATUS                      (1 << 15)
#define PRCM_SYS_PWR_STATUS_VPU_H1_STATUS                   (1 << 14)
#define PRCM_SYS_PWR_STATUS_VPU_G1_STATUS                   (1 << 13)
#define PRCM_SYS_PWR_STATUS_GC300_PWR_STATUS                (1 << 12)
#define PRCM_SYS_PWR_STATUS_MALI_PWR_STATUS_TOTAL           (1 << 11)
#define PRCM_SYS_PWR_STATUS_MALI_PWR_STATUS_L2C             (1 << 10)
#define PRCM_SYS_PWR_STATUS_MALI_PWR_STATUS_GP              (1 << 9)
#define PRCM_SYS_PWR_STATUS_MALI_PWR_STATUS_PP              (1 << 8)
#define PRCM_SYS_PWR_STATUS_NUSTAR_PWR_STATUS               (1 << 5)
#define PRCM_SYS_PWR_STATUS_L2C_PWR_STATUS                  (1 << 4)
#define PRCM_SYS_PWR_STATUS_FALCON1_PWR_STATUS              (1 << 3)
#define PRCM_SYS_PWR_STATUS_FALCON0_PWR_STATUS              (1 << 2)
#define PRCM_SYS_PWR_STATUS_NEON1_PWR_STATUS                (1 << 1)
#define PRCM_SYS_PWR_STATUS_NEON0_PWR_STATUS                (1 << 0)

// PRCM_WAKEUP_IRQ_CTRL    
#define PRCM_WAKEUP_IRQ_CTRL_WAKEUP_CYCLE_MASK              (0x3FF << 20)
#define PRCM_WAKEUP_IRQ_CTRL_WAKEUP_IRQ_EN_MASK             (0x1FFFF << 0)

// PRCM_ACK_ISO     
#define PRCM_ACK_ISO_NUSTAR_ACK_ISO_MASK                    (0x1FF << 8)
#define PRCM_ACK_ISO_APP_ACK_ISO_MASK                       (0xFF << 0)

// PRCM_MISC_REG0   
// PRCM_MISC_REG1    
// PRCM_MISC_REG2    
// PRCM_MISC_REG3

// PRCM_SYS_PLL_CTRL           
#define PRCM_SYS_PLL_CTRL_PLL2_SW_CFG_EN                    (1 << 26)
#define PRCM_SYS_PLL_CTRL_PLL1_SW_CFG_EN                    (1 << 25)
#define PRCM_SYS_PLL_CTRL_PLL0_SW_CFG_EN                    (1 << 24)
#define PRCM_SYS_PLL_CTRL_PLL6_CLKEN                        (1 << 22)
#define PRCM_SYS_PLL_CTRL_PLL5_CLKEN                        (1 << 21)
#define PRCM_SYS_PLL_CTRL_PLL4_CLKEN                        (1 << 20)
#define PRCM_SYS_PLL_CTRL_PLL3_CLKEN                        (1 << 19)
#define PRCM_SYS_PLL_CTRL_PLL2_CLKEN                        (1 << 18)
#define PRCM_SYS_PLL_CTRL_PLL1_CLKEN                        (1 << 17)
#define PRCM_SYS_PLL_CTRL_PLL0_CLKEN                        (1 << 16)
#define PRCM_SYS_PLL_CTRL_PLL6_RST                          (1 << 14)
#define PRCM_SYS_PLL_CTRL_PLL5_RST                          (1 << 13)
#define PRCM_SYS_PLL_CTRL_PLL4_RST                          (1 << 12)
#define PRCM_SYS_PLL_CTRL_PLL3_RST                          (1 << 11)
#define PRCM_SYS_PLL_CTRL_PLL2_RST                          (1 << 10)
#define PRCM_SYS_PLL_CTRL_PLL1_RST                          (1 << 9)
#define PRCM_SYS_PLL_CTRL_PLL0_RST                          (1 << 8)
#define PRCM_SYS_PLL_CTRL_PLL6_PWR_DN                       (1 << 6)
#define PRCM_SYS_PLL_CTRL_PLL5_PWR_DN                       (1 << 5)
#define PRCM_SYS_PLL_CTRL_PLL4_PWR_DN                       (1 << 4)
#define PRCM_SYS_PLL_CTRL_PLL3_PWR_DN                       (1 << 3)
#define PRCM_SYS_PLL_CTRL_PLL2_PWR_DN                       (1 << 2)
#define PRCM_SYS_PLL_CTRL_PLL1_PWR_DN                       (1 << 1)
#define PRCM_SYS_PLL_CTRL_PLL0_PWR_DN                       (1 << 0)

// PRCM_SYS_PLL_STATUS    
#define PRCM_SYS_PLL_STATUS_PLL6_STABLE                     (1 << 6)
#define PRCM_SYS_PLL_STATUS_PLL5_STABLE                     (1 << 5)
#define PRCM_SYS_PLL_STATUS_PLL4_STABLE                     (1 << 4)
#define PRCM_SYS_PLL_STATUS_PLL3_STABLE                     (1 << 3)
#define PRCM_SYS_PLL_STATUS_PLL2_STABLE                     (1 << 2)
#define PRCM_SYS_PLL_STATUS_PLL1_STABLE                     (1 << 1)
#define PRCM_SYS_PLL_STATUS_PLL0_STABLE                     (1 << 0)

// PRCM_PLL0_CFG0              
#define PRCM_PLL0_CFG0_PLL0_CLKF_MASK                       (0x1FFF << 8)
#define PRCM_PLL0_CFG0_PLL0_CLKR_MASK                       (0x3F << 0)

// PRCM_PLL0_CFG1              
#define PRCM_PLL0_CFG1_PLL0_TEST                            (1 << 19)
#define PRCM_PLL0_CFG1_PLL0_ENSAT                           (1 << 18)
#define PRCM_PLL0_CFG1_PLL0_FASTEN                          (1 << 17)
#define PRCM_PLL0_CFG1_PLL0_BYPASS                          (1 << 16)
#define PRCM_PLL0_CFG1_PLL0_BWADJ_MASK                      (0xFFF << 4)
#define PRCM_PLL0_CFG1_PLL0_CLKOD_MASK                      (0xF << 0)

// PRCM_PLL1_CFG0              
#define PRCM_PLL1_CFG0_PLL1_CLKF_MASK                       (0x1FFF << 8)
#define PRCM_PLL1_CFG0_PLL1_CLKR_MASK                       (0x3F << 0)

// PRCM_PLL1_CFG1              
#define PRCM_PLL1_CFG1_PLL1_TEST                            (1 << 19)
#define PRCM_PLL1_CFG1_PLL1_ENSAT                           (1 << 18)
#define PRCM_PLL1_CFG1_PLL1_FASTEN                          (1 << 17)
#define PRCM_PLL1_CFG1_PLL1_BYPASS                          (1 << 16)
#define PRCM_PLL1_CFG1_PLL1_BWADJ_MASK                      (0xFFF << 4)
#define PRCM_PLL1_CFG1_PLL1_CLKOD_MASK                      (0xF << 0)

// PRCM_PLL2_CFG0              
#define PRCM_PLL2_CFG0_PLL2_CLKF_MASK                       (0x1FFF << 8)
#define PRCM_PLL2_CFG0_PLL2_CLKR_MASK                       (0x3F << 0)

// PRCM_PLL2_CFG1              
#define PRCM_PLL2_CFG1_PLL2_TEST                            (1 << 19)
#define PRCM_PLL2_CFG1_PLL2_ENSAT                           (1 << 18)
#define PRCM_PLL2_CFG1_PLL2_FASTEN                          (1 << 17)
#define PRCM_PLL2_CFG1_PLL2_BYPASS                          (1 << 16)
#define PRCM_PLL2_CFG1_PLL2_BWADJ_MASK                      (0xFFF << 4)
#define PRCM_PLL2_CFG1_PLL2_CLKOD_MASK                      (0xF << 0)

// PRCM_PLL3_CFG0              
#define PRCM_PLL3_CFG0_PLL3_CLKF_MASK                       (0x1FFF << 8)
#define PRCM_PLL3_CFG0_PLL3_CLKR_MASK                       (0x3F << 0)

// PRCM_PLL3_CFG1              
#define PRCM_PLL3_CFG1_PLL3_TEST                            (1 << 19)
#define PRCM_PLL3_CFG1_PLL3_ENSAT                           (1 << 18)
#define PRCM_PLL3_CFG1_PLL3_FASTEN                          (1 << 17)
#define PRCM_PLL3_CFG1_PLL3_BYPASS                          (1 << 16)
#define PRCM_PLL3_CFG1_PLL3_BWADJ_MASK                      (0xFFF << 4)
#define PRCM_PLL3_CFG1_PLL3_CLKOD_MASK                      (0xF << 0)

// PRCM_PLL4_CFG0              
#define PRCM_PLL4_CFG0_PLL4_CLKF_MASK                       (0x1FFF << 8)
#define PRCM_PLL4_CFG0_PLL4_CLKR_MASK                       (0x3F << 0)

// PRCM_PLL4_CFG1              
#define PRCM_PLL4_CFG1_PLL4_TEST                            (1 << 19)
#define PRCM_PLL4_CFG1_PLL4_ENSAT                           (1 << 18)
#define PRCM_PLL4_CFG1_PLL4_FASTEN                          (1 << 17)
#define PRCM_PLL4_CFG1_PLL4_BYPASS                          (1 << 16)
#define PRCM_PLL4_CFG1_PLL4_BWADJ_MASK                      (0xFFF << 4)
#define PRCM_PLL4_CFG1_PLL4_CLKOD_MASK                      (0xF << 0)

// PRCM_PLL5_CFG0              
#define PRCM_PLL5_CFG0_PLL5_CLKF_MASK                       (0x1FFF << 8)
#define PRCM_PLL5_CFG0_PLL5_CLKR_MASK                       (0x3F << 0)

// PRCM_PLL5_CFG1              
#define PRCM_PLL5_CFG1_PLL5_TEST                            (1 << 19)
#define PRCM_PLL5_CFG1_PLL5_ENSAT                           (1 << 18)
#define PRCM_PLL5_CFG1_PLL5_FASTEN                          (1 << 17)
#define PRCM_PLL5_CFG1_PLL5_BYPASS                          (1 << 16)
#define PRCM_PLL5_CFG1_PLL5_BWADJ_MASK                      (0xFFF << 4)
#define PRCM_PLL5_CFG1_PLL5_CLKOD_MASK                      (0xF << 0)

// PRCM_PLL6_CFG0              
#define PRCM_PLL6_CFG0_PLL6_CLKF_MASK                       (0x1FFF << 8)
#define PRCM_PLL6_CFG0_PLL6_CLKR_MASK                       (0x3F << 0)

// PRCM_PLL6_CFG1              
#define PRCM_PLL6_CFG1_PLL6_TEST                            (1 << 19)
#define PRCM_PLL6_CFG1_PLL6_ENSAT                           (1 << 18)
#define PRCM_PLL6_CFG1_PLL6_FASTEN                          (1 << 17)
#define PRCM_PLL6_CFG1_PLL6_BYPASS                          (1 << 16)
#define PRCM_PLL6_CFG1_PLL6_BWADJ_MASK                      (0xFFF << 4)
#define PRCM_PLL6_CFG1_PLL6_CLKOD_MASK                      (0xF << 0)

// PRCM_CPU_CLK_DIV            
#define PRCM_CPU_CLK_DIV_CPU_BUS_DIV_EN                     (1 << 20)
#define PRCM_CPU_CLK_DIV_CPU_BUS_DIV_MASK                   (0x3 << 16)
#define PRCM_CPU_CLK_DIV_CPU_DIV1_EN                        (1 << 12)
#define PRCM_CPU_CLK_DIV_CPU_DIV1_MASK                      (0xF << 8)
#define PRCM_CPU_CLK_DIV_CPU_DIV0_EN                        (1 << 4)
#define PRCM_CPU_CLK_DIV_CPU_DIV0_MASK                      (0xF << 0)
#define PRCM_CPU_CLK_DIV_CPU_DIV1_2                         (0x2 << 8)
#define PRCM_CPU_CLK_DIV_CPU_DIV1_4                         (0x4 << 8)
#define PRCM_CPU_CLK_DIV_CPU_DIV1_8                         (0x8 << 8)
#define PRCM_CPU_CLK_DIV_CPU_DIV1_15                        (0xf << 8)
#define PRCM_CPU_CLK_DIV_CPU_DIV0_2                         (0x2 << 0)
#define PRCM_CPU_CLK_DIV_CPU_DIV0_4                         (0x4 << 0)
#define PRCM_CPU_CLK_DIV_CPU_DIV0_8                         (0x8 << 0)
#define PRCM_CPU_CLK_DIV_CPU_DIV0_15                        (0xf << 0)

// PRCM_CPU_CLK_SWITCH         
#define PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_EN                 (1 << 28)
#define PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_MASK               (0x7 << 24)
#define PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_MODE               (1 << 20)
#define PRCM_CPU_CLK_SWITCH_CPU_MUX_SWITCH                  (1 << 16)
#define PRCM_CPU_CLK_SWITCH_CPU_MU1_SEL_EN                  (1 << 12)
#define PRCM_CPU_CLK_SWITCH_CPU_MUX1_SEL_MASK               (0x7 << 8)
#define PRCM_CPU_CLK_SWITCH_CPU_MU0_SEL_EN                  (1 << 4)
#define PRCM_CPU_CLK_SWITCH_CPU_MUX0_SEL_MASK               (0x7 << 0)

#define PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_0                  (0x0 << 24)
#define PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_1                  (0x1 << 24)
#define PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_2                  (0x2 << 24)
#define PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_3                  (0x3 << 24)
#define PRCM_CPU_CLK_SWITCH_CPU_AUTO_SEL_4                  (0x4 << 24)
#define PRCM_CPU_CLK_SWITCH_CPU_MUX1_SEL_0                  (0x0 << 8)
#define PRCM_CPU_CLK_SWITCH_CPU_MUX1_SEL_1                  (0x1 << 8)
#define PRCM_CPU_CLK_SWITCH_CPU_MUX1_SEL_2                  (0x2 << 8)
#define PRCM_CPU_CLK_SWITCH_CPU_MUX1_SEL_3                  (0x3 << 8)
#define PRCM_CPU_CLK_SWITCH_CPU_MUX1_SEL_4                  (0x4 << 8)
#define PRCM_CPU_CLK_SWITCH_CPU_MUX0_SEL_0                  (0x0 << 0)
#define PRCM_CPU_CLK_SWITCH_CPU_MUX0_SEL_1                  (0x1 << 0)
#define PRCM_CPU_CLK_SWITCH_CPU_MUX0_SEL_2                  (0x2 << 0)
#define PRCM_CPU_CLK_SWITCH_CPU_MUX0_SEL_3                  (0x3 << 0)
#define PRCM_CPU_CLK_SWITCH_CPU_MUX0_SEL_4                  (0x4 << 0)

// PRCM_CORESIGHT_CLK_CTRL     
#define PRCM_CORESIGHT_CLK_CTRL_CS_RSTN                     (1 << 12)
#define PRCM_CORESIGHT_CLK_CTRL_CS_AT_DIV_EN                (1 << 8)
#define PRCM_CORESIGHT_CLK_CTRL_CS_AT_DIV_MASK              (0xF << 4)
#define PRCM_CORESIGHT_CLK_CTRL_CS_ATCLKEN                  (1 << 1)
#define PRCM_CORESIGHT_CLK_CTRL_CS_HCLKEN                   (1 << 0)

// PRCM_DDR_CLK_CTRL           
#define PRCM_DDR_CLK_CTRL_DDR_RSTN                          (1 << 12)
#define PRCM_DDR_CLK_CTRL_DDR_HCLKEN                        (1 << 9)
#define PRCM_DDR_CLK_CTRL_DDR_CLKEN                         (1 << 8)
#define PRCM_DDR_CLK_CTRL_DDR_DIV_EN                        (1 << 4)
#define PRCM_DDR_CLK_CTRL_DDR_DIV_MASK                      (0xF << 0)

// PRCM_BUS_CLK_CTRL           
#define PRCM_BUS_CLK_CTRL_BUS_RATIO_EN                      (1 << 12)
#define PRCM_BUS_CLK_CTRL_BUS_RATIO_MASK                    (0x7 << 8)

// PRCM_DMA_CLK_CTRL           
#define PRCM_DMA_CLK_CTRL_DMA_RSTN                          (1 << 4)
#define PRCM_DMA_CLK_CTRL_DMA_ACLKEN                        (1 << 0)

// PRCM_MALI_CLK_CTRL          
#define PRCM_MALI_CLK_CTRL_MALI_RSTN                        (1 << 20)
#define PRCM_MALI_CLK_CTRL_MALI_CLKEN                       (1 << 17)
#define PRCM_MALI_CLK_CTRL_MALI_CLK_SEL                     (1 << 16)
#define PRCM_MALI_CLK_CTRL_MALI_DIV1_EN                     (1 << 12)
#define PRCM_MALI_CLK_CTRL_MALI_DIV1_MASK                   (0xF << 8)
#define PRCM_MALI_CLK_CTRL_MALI_DIV0_EN                     (1 << 4)
#define PRCM_MALI_CLK_CTRL_MALI_DIV0_MASK                   (0xF << 0)

// PRCM_GC300_CLK_CTRL         
#define PRCM_GC300_CLK_CTRL_GC300_RSTN                      (1 << 16)
#define PRCM_GC300_CLK_CTRL_GC300_CLK_SEL                   (1 << 12)
#define PRCM_GC300_CLK_CTRL_GC300_HCLKEN                    (1 << 10)
#define PRCM_GC300_CLK_CTRL_GC300_ACLKEN                    (1 << 9)
#define PRCM_GC300_CLK_CTRL_GC300_CLKEN                     (1 << 8)
#define PRCM_GC300_CLK_CTRL_GC300_DIV_EN                    (1 << 4)
#define PRCM_GC300_CLK_CTRL_GC300_DIV_MASK                  (0xF << 0)

// PRCM_VPU_G1_CLK_CTRL        
#define PRCM_VPU_G1_CLK_CTRL_VPU_G1_RSTN                    (1 << 24)
#define PRCM_VPU_G1_CLK_CTRL_VPU_G1_CLKEN                   (1 << 20)
#define PRCM_VPU_G1_CLK_CTRL_VPU_G1_CLK_SEL                 (1 << 16)
#define PRCM_VPU_G1_CLK_CTRL_VPU_G1_DIV1_EN                 (1 << 12)
#define PRCM_VPU_G1_CLK_CTRL_VPU_G1_DIV1_MASK               (0xF << 8)
#define PRCM_VPU_G1_CLK_CTRL_VPU_G1_DIV0_EN                 (1 << 4)
#define PRCM_VPU_G1_CLK_CTRL_VPU_G1_DIV0_MASK               (0xF << 0)

// PRCM_VPU_H1_CLK_CTRL        
#define PRCM_VPU_H1_CLK_CTRL_VPU_H1_RSTN                    (1 << 24)
#define PRCM_VPU_H1_CLK_CTRL_VPU_H1_CLKEN                   (1 << 20)
#define PRCM_VPU_H1_CLK_CTRL_VPU_H1_CLK_SEL                 (1 << 16)
#define PRCM_VPU_H1_CLK_CTRL_VPU_H1_DIV1_EN                 (1 << 12)
#define PRCM_VPU_H1_CLK_CTRL_VPU_H1_DIV1_MASK               (0xF << 8)
#define PRCM_VPU_H1_CLK_CTRL_VPU_H1_DIV0_EN                 (1 << 4)
#define PRCM_VPU_H1_CLK_CTRL_VPU_H1_DIV0_MASK               (0xF << 0)

// PRCM_ISP_CLK_CTRL           
#define PRCM_ISP_CLK_CTRL_ISP_RSTN                          (1 << 12)
#define PRCM_ISP_CLK_CTRL_ISP_S_HCLKEN                      (1 << 11)
#define PRCM_ISP_CLK_CTRL_ISP_M_HCLKEN                      (1 << 10)
#define PRCM_ISP_CLK_CTRL_ISP_CLKEN                         (1 << 9)
#define PRCM_ISP_CLK_CTRL_ISP_JPEGCLKEN                     (1 << 8)
#define PRCM_ISP_CLK_CTRL_ISP_JPEG_DIV_EN                   (1 << 4)
#define PRCM_ISP_CLK_CTRL_ISP_JPEG_DIV_MASK                 (0xF << 0)

// PRCM_SD_CLK_CTRL            
#define PRCM_SD_CLK_CTRL_SD_RSTN                            (1 << 12)
#define PRCM_SD_CLK_CTRL_SD_HCLKEN                          (1 << 9)
#define PRCM_SD_CLK_CTRL_SD_CLKEN                           (1 << 8)
#define PRCM_SD_CLK_CTRL_SD_DIV_EN                          (1 << 4)
#define PRCM_SD_CLK_CTRL_SD_DIV_MASK                        (0xF << 0)

// PRCM_ZSP_CLK_CTRL           
#define PRCM_ZSP_CLK_CTRL_ZSP_RSTN                          (1 << 14)
#define PRCM_ZSP_CLK_CTRL_ZSP_ARSTN                         (1 << 13)
#define PRCM_ZSP_CLK_CTRL_ZSP_SYS_RSTN                      (1 << 12)
#define PRCM_ZSP_CLK_CTRL_ZSP_CLKEN                         (1 << 9)
#define PRCM_ZSP_CLK_CTRL_ZSP_ACLKEN                        (1 << 8)

// PRCM_SPI_CLK_CTRL           
#define PRCM_SPI_CLK_CTRL_SPI3_RSTN                         (1 << 31)
#define PRCM_SPI_CLK_CTRL_SPI2_RSTN                         (1 << 30)
#define PRCM_SPI_CLK_CTRL_SPI1_RSTN                         (1 << 29)
#define PRCM_SPI_CLK_CTRL_SPI0_RSTN                         (1 << 28)
#define PRCM_SPI_CLK_CTRL_SPI3_PCLKEN                       (1 << 27)
#define PRCM_SPI_CLK_CTRL_SPI2_PCLKEN                       (1 << 26)
#define PRCM_SPI_CLK_CTRL_SPI1_PCLKEN                       (1 << 25)
#define PRCM_SPI_CLK_CTRL_SPI0_PCLKEN                       (1 << 24)
#define PRCM_SPI_CLK_CTRL_SPI3_CLKEN                        (1 << 23)
#define PRCM_SPI_CLK_CTRL_SPI2_CLKEN                        (1 << 22)
#define PRCM_SPI_CLK_CTRL_SPI1_CLKEN                        (1 << 21)
#define PRCM_SPI_CLK_CTRL_SPI0_CLKEN                        (1 << 20)
#define PRCM_SPI_CLK_CTRL_SPI2_DIV_EN                       (1 << 16)
#define PRCM_SPI_CLK_CTRL_SPI2_DIV_MASK                     (0x7 << 12)
#define PRCM_SPI_CLK_CTRL_SPI01_DIV_EN                      (1 << 8)
#define PRCM_SPI_CLK_CTRL_SPI01_DIV_MASK                    (0x3F << 0)

// PRCM_I2C_CLK_CTRL           
#define PRCM_I2C_CLK_CTRL_I2C3_RSTN                         (1 << 31)
#define PRCM_I2C_CLK_CTRL_I2C2_RSTN                         (1 << 30)
#define PRCM_I2C_CLK_CTRL_I2C1_RSTN                         (1 << 29)
#define PRCM_I2C_CLK_CTRL_I2C0_RSTN                         (1 << 28)
#define PRCM_I2C_CLK_CTRL_I2C3_PCLKEN                       (1 << 27)
#define PRCM_I2C_CLK_CTRL_I2C2_PCLKEN                       (1 << 26)
#define PRCM_I2C_CLK_CTRL_I2C1_PCLKEN                       (1 << 25)
#define PRCM_I2C_CLK_CTRL_I2C0_PCLKEN                       (1 << 24)
#define PRCM_I2C_CLK_CTRL_I2C3_CLKEN                        (1 << 23)
#define PRCM_I2C_CLK_CTRL_I2C2_CLKEN                        (1 << 22)
#define PRCM_I2C_CLK_CTRL_I2C1_CLKEN                        (1 << 21)
#define PRCM_I2C_CLK_CTRL_I2C0_CLKEN                        (1 << 20)
#define PRCM_I2C_CLK_CTRL_I2C_HS_DIV_EN                     (1 << 16)
#define PRCM_I2C_CLK_CTRL_I2C_HS_DIV_MASK                   (0xF << 12)
#define PRCM_I2C_CLK_CTRL_I2C_FS_DIV_EN                     (1 << 8)
#define PRCM_I2C_CLK_CTRL_I2C_FS_DIV_MASK                   (0x3F << 0)

// PRCM_UART_CLK_CTRL0         
#define PRCM_UART_CLK_CTRL0_UART3_DIV_EN                    (1 << 21)
#define PRCM_UART_CLK_CTRL0_UART3_DIV_MASK                  (0x1F << 16)
#define PRCM_UART_CLK_CTRL0_UART2_DIV_EN                    (1 << 13)
#define PRCM_UART_CLK_CTRL0_UART2_DIV_MASK                  (0x1F << 8)
#define PRCM_UART_CLK_CTRL0_UART01_DIV_EN                   (1 << 5)
#define PRCM_UART_CLK_CTRL0_UART01_DIV_MASK                 (0x1F << 0)

// PRCM_UART_CLK_CTRL1         
#define PRCM_UART_CLK_CTRL1_UART3_RSTN                      (1 << 11)
#define PRCM_UART_CLK_CTRL1_UART2_RSTN                      (1 << 10)
#define PRCM_UART_CLK_CTRL1_UART1_RSTN                      (1 << 9)
#define PRCM_UART_CLK_CTRL1_UART0_RSTN                      (1 << 8)
#define PRCM_UART_CLK_CTRL1_UART3_PCLKEN                    (1 << 7)
#define PRCM_UART_CLK_CTRL1_UART2_PCLKEN                    (1 << 6)
#define PRCM_UART_CLK_CTRL1_UART1_PCLKEN                    (1 << 5)
#define PRCM_UART_CLK_CTRL1_UART0_PCLKEN                    (1 << 4)
#define PRCM_UART_CLK_CTRL1_UART3_CLKEN                     (1 << 3)
#define PRCM_UART_CLK_CTRL1_UART2_CLKEN                     (1 << 2)
#define PRCM_UART_CLK_CTRL1_UART1_CLKEN                     (1 << 1)
#define PRCM_UART_CLK_CTRL1_UART0_CLKEN                     (1 << 0)

// PRCM_DIS0_CLK_CTRL          
#define PRCM_DIS0_CLK_CTRL_LCDC0_RSTN                       (1 << 16)
#define PRCM_DIS0_CLK_CTRL_RGBO_INV_CLK_SEL                 (1 << 13)
#define PRCM_DIS0_CLK_CTRL_RGBO_CLKEN                       (1 << 12)
#define PRCM_DIS0_CLK_CTRL_LCDC0_HCLKEN                     (1 << 10)
#define PRCM_DIS0_CLK_CTRL_LCDC0_ACLKEN                     (1 << 9)
#define PRCM_DIS0_CLK_CTRL_LCDC0_CLKEN                      (1 << 8)
#define PRCM_DIS0_CLK_CTRL_PXL0_DIV_EN                      (1 << 4)
#define PRCM_DIS0_CLK_CTRL_PXL0_DIV_MASK                    (0xF << 0)

// PRCM_DIS1_CLK_CTRL          
#define PRCM_DIS1_CLK_CTRL_HDMI_RSTN                        (1 << 17)
#define PRCM_DIS1_CLK_CTRL_LCDC1_RSTN                       (1 << 16)
#define PRCM_DIS1_CLK_CTRL_HDMI_CEC_CLKEN                   (1 << 14)
#define PRCM_DIS1_CLK_CTRL_HDMI_BCLKEN                      (1 << 13)
#define PRCM_DIS1_CLK_CTRL_HDMI_PXL_CLKEN                   (1 << 12)
#define PRCM_DIS1_CLK_CTRL_LCDC1_HCLKEN                     (1 << 10)
#define PRCM_DIS1_CLK_CTRL_LCDC1_ACLKEN                     (1 << 9)
#define PRCM_DIS1_CLK_CTRL_LCDC1_CLKEN                      (1 << 8)
#define PRCM_DIS1_CLK_CTRL_PXL1_DIV_EN                      (1 << 4)
#define PRCM_DIS1_CLK_CTRL_PXL1_DIV_MASK                    (0xF << 0)

// PRCM_I2S_CLK_CTRL           
#define PRCM_I2S_CLK_CTRL_MCLKEN                            (1 << 16)
#define PRCM_I2S_CLK_CTRL_I2S1_RSTN                         (1 << 13)
#define PRCM_I2S_CLK_CTRL_I2S0_RSTN                         (1 << 12)
#define PRCM_I2S_CLK_CTRL_I2S1_PCLKEN                       (1 << 11)
#define PRCM_I2S_CLK_CTRL_I2S0_PCLKEN                       (1 << 10)
#define PRCM_I2S_CLK_CTRL_I2S1_CLKEN                        (1 << 9)
#define PRCM_I2S_CLK_CTRL_I2S0_CLKEN                        (1 << 8)
#define PRCM_I2S_CLK_CTRL_I2S_DIV_EN                        (1 << 4)
#define PRCM_I2S_CLK_CTRL_I2S_DIV_MASK                      (0xF << 0)

// PRCM_USB_CLK_CTRL           
#define PRCM_USB_CLK_CTRL_USB_RST_READY                     (0xF << 28)
#define PRCM_USB_CLK_CTRL_HSIC_PHY_RSTN                     (1 << 14)
#define PRCM_USB_CLK_CTRL_USB_PHY_RSTN_MASK                 (0x3 << 12)
#define PRCM_USB_CLK_CTRL_OTG_HRSTN                         (1 << 9)
#define PRCM_USB_CLK_CTRL_USB2_HRSTN                        (1 << 8)
#define PRCM_USB_CLK_CTRL_UTMI_PHY_CLKEN                    (1 << 5)
#define PRCM_USB_CLK_CTRL_OTG_HCLKEN                        (1 << 4)
#define PRCM_USB_CLK_CTRL_USB2_HCLKEN                       (1 << 3)
#define PRCM_USB_CLK_CTRL_USB_REF_CLKEN                     (1 << 2)
#define PRCM_USB_CLK_CTRL_OHCI_48M_CLKEN                    (1 << 1)
#define PRCM_USB_CLK_CTRL_HSIC_480M_CLKEN                   (1 << 0)

// PRCM_CORE_MEM_CLK_CTRL      
#define PRCM_CORE_MEM_CLK_CTRL_ROM_RSTN                     (1 << 4)
#define PRCM_CORE_MEM_CLK_CTRL_ROM_HCLKEN                   (1 << 0)

// PRCM_GPIO_CORE_CLK_CTRL     
#define PRCM_GPIO_CORE_CLK_CTRL_GPIO_CORE_RSTN              (1 << 4)
#define PRCM_GPIO_CORE_CLK_CTRL_GPIO_CORE_PCLKEN            (1 << 4)

// PRCM_TIMER_CORE_CLK_CTRL    
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_RSTN_MASK       (0xFF << 20)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER_CORE_PCLKEN          (1 << 16)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER7_CORE_CLK_SEL        (1 << 15)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER6_CORE_CLK_SEL        (1 << 14)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER5_CORE_CLK_SEL        (1 << 13)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER4_CORE_CLK_SEL        (1 << 12)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER3_CORE_CLK_SEL        (1 << 11)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER2_CORE_CLK_SEL        (1 << 10)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER1_CORE_CLK_SEL        (1 << 9)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER0_CORE_CLK_SEL        (1 << 8)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER7_CORE_CLKEN          (1 << 7)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER6_CORE_CLKEN          (1 << 6)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER5_CORE_CLKEN          (1 << 5)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER4_CORE_CLKEN          (1 << 4)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER3_CORE_CLKEN          (1 << 3)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER2_CORE_CLKEN          (1 << 2)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER1_CORE_CLKEN          (1 << 1)
#define PRCM_TIMER_CORE_CLK_CTRL_TIMER0_CORE_CLKEN          (1 << 0)

// PRCM_WKUP_MEM_CLK_CTRL      
#define PRCM_WKUP_MEM_CLK_CTRL_RAM_WKUP_RSTN                (1 << 4)
#define PRCM_WKUP_MEM_CLK_CTRL_RAM_WKUP_HCLKEN              (1 << 0)

// PRCM_GPIO_WKUP_CLK_CTRL     
#define PRCM_GPIO_WKUP_CLK_CTRL_GPIO_WKUP_RSTN              (1 << 17)
#define PRCM_GPIO_WKUP_CLK_CTRL_GPIO_DBCLK_32K_DIV_EN       (1 << 16)
#define PRCM_GPIO_WKUP_CLK_CTRL_GPIO_DBCLK_32K_DIV_MASK     (0xFF << 8)
#define PRCM_GPIO_WKUP_CLK_CTRL_GPIO_WKUP_PCLK_SEL          (1 << 2)
#define PRCM_GPIO_WKUP_CLK_CTRL_GPIO_DBCLKEN                (1 << 1)
#define PRCM_GPIO_WKUP_CLK_CTRL_GPIO_WKUP_PCLKEN            (1 << 0)

// PRCM_TIMER_WKUP_CLK_CTRL    
#define PRCM_TIMER_WKUP_CLK_CTRL_TIMER_WKUP_RSTN_MASK       (0x3 << 8)
#define PRCM_TIMER_WKUP_CLK_CTRL_TIMER1_WKUP_CLK_SEL        (1 << 5)
#define PRCM_TIMER_WKUP_CLK_CTRL_TIMER0_WKUP_CLK_SEL        (1 << 4)
#define PRCM_TIMER_WKUP_CLK_CTRL_TIMER_WKUP_PCLKEN          (1 << 2)
#define PRCM_TIMER_WKUP_CLK_CTRL_TIMER1_WKUP_CLKEN          (1 << 1)
#define PRCM_TIMER_WKUP_CLK_CTRL_TIMER0_WKUP_CLKEN          (1 << 0)

// PRCM_ALT_CLK_CTRL           
#define PRCM_ALT_CLK_CTRL_ALT1_CLKEN                        (1 << 29)
#define PRCM_ALT_CLK_CTRL_ALT0_CLKEN                        (1 << 28)
#define PRCM_ALT_CLK_CTRL_ALT1_32K_CLK_SEL                  (1 << 24)
#define PRCM_ALT_CLK_CTRL_ALT1_DIV_EN                       (1 << 23)
#define PRCM_ALT_CLK_CTRL_ALT1_DIV_MASK                     (0x7FF << 12)
#define PRCM_ALT_CLK_CTRL_ALT0_DIV_EN                       (1 << 11)
#define PRCM_ALT_CLK_CTRL_ALT0_DIV_MASK                     (0x7FF << 0)

// PRCM_AUX_CLK_CTRL           
#define PRCM_AUX_CLK_CTRL_AUX_PRSTN                         (1 << 5)
#define PRCM_AUX_CLK_CTRL_AUX_PCLKEN                        (1 << 4)
#define PRCM_AUX_CLK_CTRL_EFUSE_RSTN                        (1 << 3)
#define PRCM_AUX_CLK_CTRL_PWM_RSTN                          (1 << 2)
#define PRCM_AUX_CLK_CTRL_EFUSE_CLKEN                       (1 << 1)
#define PRCM_AUX_CLK_CTRL_PWM_CLKEN                         (1 << 0)


// Section-4: PRCM Configure Parameter Definition
// PLL configuration
// 50MHz
#define PRCM_PLL_NR_50MHZ                                          6
#define PRCM_PLL_NF_50MHZ                                          400
#define PRCM_PLL_OD_50MHZ                                          16  
#define PRCM_PLL_NB_50MHZ                                          200
// 200MHz
#define PRCM_PLL_NR_200MHZ                                         3
#define PRCM_PLL_NF_200MHZ                                         300
#define PRCM_PLL_OD_200MHZ                                         6   
#define PRCM_PLL_NB_200MHZ                                         150
// 250MHz
#define PRCM_PLL_NR_250MHZ                                         1
#define PRCM_PLL_NF_250MHZ                                         125
#define PRCM_PLL_OD_250MHZ                                         6   
#define PRCM_PLL_NB_250MHZ                                         62
// 297MHz
#define PRCM_PLL_NR_297MHZ                                         1
#define PRCM_PLL_NF_297MHZ                                         99
#define PRCM_PLL_OD_297MHZ                                         4   
#define PRCM_PLL_NB_297MHZ                                         50
// 300MHz
#define PRCM_PLL_NR_300MHZ                                         1
#define PRCM_PLL_NF_300MHZ                                         100
#define PRCM_PLL_OD_300MHZ                                         4   
#define PRCM_PLL_NB_300MHZ                                         50
// 350MHz
#define PRCM_PLL_NR_350MHZ                                         3
#define PRCM_PLL_NF_350MHZ                                         350
#define PRCM_PLL_OD_350MHZ                                         4   
#define PRCM_PLL_NB_350MHZ                                         175
// 400MHz
#define PRCM_PLL_NR_400MHZ                                         3
#define PRCM_PLL_NF_400MHZ                                         200
#define PRCM_PLL_OD_400MHZ                                         2   
#define PRCM_PLL_NB_400MHZ                                         100
// 450MHz
#define PRCM_PLL_NR_450MHZ                                         1
#define PRCM_PLL_NF_450MHZ                                         75
#define PRCM_PLL_OD_450MHZ                                         2   
#define PRCM_PLL_NB_450MHZ                                         38
// 480MHz
#define PRCM_PLL_NR_480MHZ                                         1
#define PRCM_PLL_NF_480MHZ                                         80
#define PRCM_PLL_OD_480MHZ                                         2   
#define PRCM_PLL_NB_480MHZ                                         40
// 500MHz
#define PRCM_PLL_NR_500MHZ                                         3
#define PRCM_PLL_NF_500MHZ                                         250
#define PRCM_PLL_OD_500MHZ                                         2   
#define PRCM_PLL_NB_500MHZ                                         125
// 533MHz
#define PRCM_PLL_NR_533MHZ                                         6
#define PRCM_PLL_NF_533MHZ                                         533
#define PRCM_PLL_OD_533MHZ                                         2   
#define PRCM_PLL_NB_533MHZ                                         266
// 550MHz
#define PRCM_PLL_NR_550MHZ                                         3
#define PRCM_PLL_NF_550MHZ                                         275
#define PRCM_PLL_OD_550MHZ                                         2   
#define PRCM_PLL_NB_550MHZ                                         138
// 600MHz
#define PRCM_PLL_NR_600MHZ                                         1
#define PRCM_PLL_NF_600MHZ                                         100
#define PRCM_PLL_OD_600MHZ                                         2   
#define PRCM_PLL_NB_600MHZ                                         50
// 650MHz
#define PRCM_PLL_NR_650MHZ                                         3
#define PRCM_PLL_NF_650MHZ                                         325
#define PRCM_PLL_OD_650MHZ                                         2   
#define PRCM_PLL_NB_650MHZ                                         162
// 666MHz
#define PRCM_PLL_NR_666MHZ                                         1
#define PRCM_PLL_NF_666MHZ                                         111
#define PRCM_PLL_OD_666MHZ                                         2   
#define PRCM_PLL_NB_666MHZ                                         56
// 700MHz
#define PRCM_PLL_NR_700MHZ                                         3
#define PRCM_PLL_NF_700MHZ                                         350
#define PRCM_PLL_OD_700MHZ                                         2   
#define PRCM_PLL_NB_700MHZ                                         175
// 720MHz
#define PRCM_PLL_NR_720MHZ                                         1
#define PRCM_PLL_NF_720MHZ                                         120
#define PRCM_PLL_OD_720MHZ                                         2   
#define PRCM_PLL_NB_720MHZ                                         60
// 750MHz
#define PRCM_PLL_NR_750MHZ                                         1
#define PRCM_PLL_NF_750MHZ                                         125
#define PRCM_PLL_OD_750MHZ                                         2   
#define PRCM_PLL_NB_750MHZ                                         62
// 800MHz
#define PRCM_PLL_NR_800MHZ                                         3
#define PRCM_PLL_NF_800MHZ                                         200
#define PRCM_PLL_OD_800MHZ                                         1   
#define PRCM_PLL_NB_800MHZ                                         100
// 850MHz
#define PRCM_PLL_NR_850MHZ                                         6
#define PRCM_PLL_NF_850MHZ                                         425
#define PRCM_PLL_OD_850MHZ                                         1   
#define PRCM_PLL_NB_850MHZ                                         212
// 891MHz
#define PRCM_PLL_NR_891MHZ                                         4
#define PRCM_PLL_NF_891MHZ                                         297
#define PRCM_PLL_OD_891MHZ                                         1   
#define PRCM_PLL_NB_891MHZ                                         148
// 900MHz
#define PRCM_PLL_NR_900MHZ                                         1
#define PRCM_PLL_NF_900MHZ                                         75
#define PRCM_PLL_OD_900MHZ                                         1   
#define PRCM_PLL_NB_900MHZ                                         38
// 950MHz
#define PRCM_PLL_NR_950MHZ                                         6
#define PRCM_PLL_NF_950MHZ                                         475
#define PRCM_PLL_OD_950MHZ                                         1   
#define PRCM_PLL_NB_950MHZ                                         238
// 983.04MHz
#define PRCM_PLL_NR_983M04HZ                                       25
#define PRCM_PLL_NF_983M04HZ                                       2048
#define PRCM_PLL_OD_983M04HZ                                       1   
#define PRCM_PLL_NB_983M04HZ                                       1024
// 1GHz
#define PRCM_PLL_NR_1000MHZ                                        3
#define PRCM_PLL_NF_1000MHZ                                        250
#define PRCM_PLL_OD_1000MHZ                                        1   
#define PRCM_PLL_NB_1000MHZ                                        125
// 1.05GHz
#define PRCM_PLL_NR_1050MHZ                                        2
#define PRCM_PLL_NF_1050MHZ                                        175
#define PRCM_PLL_OD_1050MHZ                                        1   
#define PRCM_PLL_NB_1050MHZ                                        88
// 1.1GHz
#define PRCM_PLL_NR_1100MHZ                                        3
#define PRCM_PLL_NF_1100MHZ                                        275
#define PRCM_PLL_OD_1100MHZ                                        1   
#define PRCM_PLL_NB_1100MHZ                                        138
// 1.15GHz
#define PRCM_PLL_NR_1150MHZ                                        6
#define PRCM_PLL_NF_1150MHZ                                        575
#define PRCM_PLL_OD_1150MHZ                                        1   
#define PRCM_PLL_NB_1150MHZ                                        288
// 1.2GHz
#define PRCM_PLL_NR_1200MHZ                                        1
#define PRCM_PLL_NF_1200MHZ                                        100
#define PRCM_PLL_OD_1200MHZ                                        1   
#define PRCM_PLL_NB_1200MHZ                                        50
// 1.25GHz
#define PRCM_PLL_NR_1250MHZ                                        6
#define PRCM_PLL_NF_1250MHZ                                        625
#define PRCM_PLL_OD_1250MHZ                                        1   
#define PRCM_PLL_NB_1250MHZ                                        312
// 1.3GHz
#define PRCM_PLL_NR_1300MHZ                                        3
#define PRCM_PLL_NF_1300MHZ                                        325
#define PRCM_PLL_OD_1300MHZ                                        1   
#define PRCM_PLL_NB_1300MHZ                                        162
// 1.35GHz
#define PRCM_PLL_NR_1350MHZ                                        2
#define PRCM_PLL_NF_1350MHZ                                        225
#define PRCM_PLL_OD_1350MHZ                                        1   
#define PRCM_PLL_NB_1350MHZ                                        112
// 1.4GHz
#define PRCM_PLL_NR_1400MHZ                                        3
#define PRCM_PLL_NF_1400MHZ                                        350
#define PRCM_PLL_OD_1400MHZ                                        1   
#define PRCM_PLL_NB_1400MHZ                                        175
// 1.45GHz
#define PRCM_PLL_NR_1450MHZ                                        6
#define PRCM_PLL_NF_1450MHZ                                        725
#define PRCM_PLL_OD_1450MHZ                                        1   
#define PRCM_PLL_NB_1450MHZ                                        362
// 1.5GHz
#define PRCM_PLL_NR_1500MHZ                                        1
#define PRCM_PLL_NF_1500MHZ                                        125
#define PRCM_PLL_OD_1500MHZ                                        1   
#define PRCM_PLL_NB_1500MHZ                                        62
// 1.55GHz
#define PRCM_PLL_NR_1550MHZ                                        6
#define PRCM_PLL_NF_1550MHZ                                        775
#define PRCM_PLL_OD_1550MHZ                                        1   
#define PRCM_PLL_NB_1550MHZ                                        388
// 1.6GHz
#define PRCM_PLL_NR_1600MHZ                                        3
#define PRCM_PLL_NF_1600MHZ                                        400
#define PRCM_PLL_OD_1600MHZ                                        1   
#define PRCM_PLL_NB_1600MHZ                                        200
// 1.65GHz
#define PRCM_PLL_NR_1650MHZ                                        2
#define PRCM_PLL_NF_1650MHZ                                        275
#define PRCM_PLL_OD_1650MHZ                                        1   
#define PRCM_PLL_NB_1650MHZ                                        138
// 1.7GHz
#define PRCM_PLL_NR_1700MHZ                                        3
#define PRCM_PLL_NF_1700MHZ                                        425
#define PRCM_PLL_OD_1700MHZ                                        1   
#define PRCM_PLL_NB_1700MHZ                                        212
// 1.75GHz
#define PRCM_PLL_NR_1750MHZ                                        6
#define PRCM_PLL_NF_1750MHZ                                        825
#define PRCM_PLL_OD_1750MHZ                                        1   
#define PRCM_PLL_NB_1750MHZ                                        438
// 1.8GHz
#define PRCM_PLL_NR_1800MHZ                                        1
#define PRCM_PLL_NF_1800MHZ                                        150
#define PRCM_PLL_OD_1800MHZ                                        1   
#define PRCM_PLL_NB_1800MHZ                                        75

// LCDC used
//25.175MHz*2
#define PRCM_PLL_NR_50M35HZ                                        15
#define PRCM_PLL_NF_50M35HZ                                        1007
#define PRCM_PLL_OD_50M35HZ                                        16   
#define PRCM_PLL_NB_50M35HZ                                        504
//33.264MHz*2
#define PRCM_PLL_NR_66M528HZ                                       25
#define PRCM_PLL_NF_66M528HZ                                       1386
#define PRCM_PLL_OD_66M528HZ                                       10   
#define PRCM_PLL_NB_66M528HZ                                       693
//28.000MHz*2
#define PRCM_PLL_NR_56MHZ                                          3
#define PRCM_PLL_NF_56MHZ                                          224
#define PRCM_PLL_OD_56MHZ                                          16   
#define PRCM_PLL_NB_56MHZ                                          112
//40.000MHz*2
#define PRCM_PLL_NR_80MHZ                                          3
#define PRCM_PLL_NF_80MHZ                                          329
#define PRCM_PLL_OD_80MHZ                                          16
#define PRCM_PLL_NB_80MHZ                                          160
//65.000MHz*2
#define PRCM_PLL_NR_130MHZ                                         1
#define PRCM_PLL_NF_130MHZ                                         65
#define PRCM_PLL_OD_130MHZ                                         6 
#define PRCM_PLL_NB_130MHZ                                         32
//85.500MHz*2
#define PRCM_PLL_NR_171MHZ                                         1
#define PRCM_PLL_NF_171MHZ                                         114
#define PRCM_PLL_OD_171MHZ                                         8   
#define PRCM_PLL_NB_171MHZ                                         57
//88.750MHz*2
#define PRCM_PLL_NR_177M5HZ                                        3
#define PRCM_PLL_NF_177M5HZ                                        355
#define PRCM_PLL_OD_177M5HZ                                        8  
#define PRCM_PLL_NB_177M5HZ                                        178
//162.000MHz*2
#define PRCM_PLL_NR_324MHZ                                         1
#define PRCM_PLL_NF_324MHZ                                         108
#define PRCM_PLL_OD_324MHZ                                         4   
#define PRCM_PLL_NB_324MHZ                                         54
//119.000MHz*2
#define PRCM_PLL_NR_238MHZ                                         1
#define PRCM_PLL_NF_238MHZ                                         119 
#define PRCM_PLL_OD_238MHZ                                         6   
#define PRCM_PLL_NB_238MHZ                                         60
//43.750MHz*2
#define PRCM_PLL_NR_87M5HZ                                         2
#define PRCM_PLL_NF_87M5HZ                                         175
#define PRCM_PLL_OD_87M5HZ                                         12
#define PRCM_PLL_NB_87M5HZ                                         88
//50.4MHz*2 
#define PRCM_PLL_NR_100M8HZ                                        5 
#define PRCM_PLL_NF_100M8HZ                                        588 
#define PRCM_PLL_OD_100M8HZ                                        14 
#define PRCM_PLL_NB_100M8HZ                                        294 
//148.50MHz*2
//#define PRCM_PLL_NR_297MHZ                                        1
//#define PRCM_PLL_NF_297MHZ                                        99
//#define PRCM_PLL_OD_297MHZ                                        4   
//#define PRCM_PLL_NB_297MHZ                                       50
//74.250MHz*2
#define PRCM_PLL_NR_148M5HZ                                        1
#define PRCM_PLL_NF_148M5HZ                                        99
#define PRCM_PLL_OD_148M5HZ                                        8 
#define PRCM_PLL_NB_148M5HZ                                        50 
//83.50MHz*2
#define PRCM_PLL_NR_167MHZ                                         2
#define PRCM_PLL_NF_167MHZ                                         167
#define PRCM_PLL_OD_167MHZ                                         6  
#define PRCM_PLL_NB_167MHZ                                         84 
//71.000MHz*2
#define PRCM_PLL_NR_142MHZ                                          1
#define PRCM_PLL_NF_142MHZ                                          71
#define PRCM_PLL_OD_142MHZ                                          6 
#define PRCM_PLL_NB_142MHZ                                          36 


// Section-5: PRCM Task/Function(s) Prototype Definition

extern void reg_clr_bits(volatile void *addr, unsigned int mask);
extern void reg_set_bits(volatile void *addr, unsigned int set);

extern void ns2815_pll0_pwr_up(void);
extern void ns2815_pll0_pwr_down(void);
extern void ns2815_pll3_pwr_up(void);
extern void ns2815_pll3_pwr_down(void);
extern void ns2815_pll4_pwr_up(void);
extern void ns2815_pll4_pwr_down(void);
extern void ns2815_pll5_pwr_up(void);
extern void ns2815_pll5_pwr_down(void);
extern void ns2815_pll6_pwr_up(void);
extern void ns2815_pll6_pwr_down(void);
extern void ns2815_mali_pwr_up(void);
extern void ns2815_mali_pwr_down(void);
extern void ns2815_gc300_pwr_up(void);
extern void ns2815_gc300_pwr_down(void);
extern void ns2815_vpu_g1_pwr_up(void);
extern void ns2815_vpu_g1_pwr_down(void);
extern void ns2815_vpu_h1_pwr_up(void);
extern void ns2815_vpu_h1_pwr_down(void);
extern void ns2815_isp_pwr_up(void);
extern void ns2815_isp_pwr_down(void);
extern void ns2815_zsp_pwr_up(void);
extern void ns2815_zsp_pwr_down(void);
extern void ns2815_neon0_pwr_up(void);
extern void ns2815_neon0_pwr_down(void);
extern void ns2815_neon1_pwr_up(void);
extern void ns2815_neon1_pwr_down(void);
extern void ns2815_l2c_pwr_up(void);
extern void ns2815_l2c_pwr_down(void);
extern void ns2815_falcon0_pwr_up(void);
extern void ns2815_falcon0_pwr_down(void);
extern void ns2815_falcon1_pwr_up(void);
extern void ns2815_falcon1_pwr_down(void);

extern int ns2815_cpu_clk_enable(void);
extern int ns2815_cpu_clk_disable(void);
extern int ns2815_pll0_clk_enable(void);
extern int ns2815_pll0_clk_disable(void);
extern int ns2815_pll1_clk_enable(void);
extern int ns2815_pll1_clk_disable(void);
extern int ns2815_pll2_clk_enable(void);
extern int ns2815_pll2_clk_disable(void);
extern int ns2815_pll3_clk_enable(void);
extern int ns2815_pll3_clk_disable(void);
extern int ns2815_pll4_clk_enable(void);
extern int ns2815_pll4_clk_disable(void);
extern int ns2815_pll5_clk_enable(void);
extern int ns2815_pll5_clk_disable(void);
extern int ns2815_pll6_clk_enable(void);
extern int ns2815_pll6_clk_disable(void);
extern int ns2815_coresight_clk_enable(void);
extern int ns2815_coresight_clk_disable(void);
extern int ns2815_ddr_clk_enable(void);
extern int ns2815_ddr_clk_disable(void);
extern int ns2815_pl330dma_clk_enable(void);
extern int ns2815_pl330dma_clk_disable(void);
extern int ns2815_mali_clk_enable(void);
extern int ns2815_mali_clk_disable(void);
extern int ns2815_gc300_clk_enable(void);
extern int ns2815_gc300_clk_disable(void);
extern int ns2815_vpu_g1_clk_enable(void);
extern int ns2815_vpu_g1_clk_disable(void);
extern int ns2815_vpu_h1_clk_enable(void);
extern int ns2815_vpu_h1_clk_disable(void);
extern int ns2815_isp_jpeg_clk_enable(void);
extern int ns2815_isp_jpeg_clk_disable(void);
extern int ns2815_isp_clk_enable(void);
extern int ns2815_isp_clk_disable(void);
extern int ns2815_sd_clk_enable(void);
extern int ns2815_sd_clk_disable(void);
extern int ns2815_zsp_clk_enable(void);
extern int ns2815_zsp_clk_disable(void);
extern int ns2815_spi0_clk_enable(void);
extern int ns2815_spi0_clk_disable(void);
extern int ns2815_spi1_clk_enable(void);
extern int ns2815_spi1_clk_disable(void);
extern int ns2815_spi2_clk_enable(void);
extern int ns2815_spi2_clk_disable(void);
extern int ns2815_spi3_clk_enable(void);
extern int ns2815_spi3_clk_disable(void);
extern int ns2815_i2c0_clk_enable(void);
extern int ns2815_i2c0_clk_disable(void);
extern int ns2815_i2c1_clk_enable(void);
extern int ns2815_i2c1_clk_disable(void);
extern int ns2815_i2c2_clk_enable(void);
extern int ns2815_i2c2_clk_disable(void);
extern int ns2815_i2c3_clk_enable(void);
extern int ns2815_i2c3_clk_disable(void);
extern int ns2815_uart0_clk_enable(void);
extern int ns2815_uart0_clk_disable(void);
extern int ns2815_uart1_clk_enable(void);
extern int ns2815_uart1_clk_disable(void);
extern int ns2815_uart2_clk_enable(void);
extern int ns2815_uart2_clk_disable(void);
extern int ns2815_uart3_clk_enable(void);
extern int ns2815_uart3_clk_disable(void);
extern int ns2815_rgbo_clk_enable(void);
extern int ns2815_rgbo_clk_disable(void);
extern int ns2815_lcdc0_clk_enable(void);
extern int ns2815_lcdc0_clk_disable(void);
extern int ns2815_lcdc0bus_clk_enable(void);
extern int ns2815_lcdc0bus_clk_disable(void);
extern int ns2815_lcdc1_clk_enable(void);
extern int ns2815_lcdc1_clk_disable(void);
extern int ns2815_lcdc1bus_clk_enable(void);
extern int ns2815_lcdc1bus_clk_disable(void);
extern int ns2815_hdmi_pxl_clk_enable(void);
extern int ns2815_hdmi_pxl_clk_disable(void);
extern int ns2815_i2s0_clk_enable(void);
extern int ns2815_i2s0_clk_disable(void);
extern int ns2815_i2s1_clk_enable(void);
extern int ns2815_i2s1_clk_disable(void);
extern int ns2815_mclk_clk_enable(void);
extern int ns2815_mclk_clk_disable(void);
extern int ns2815_usb_otg_clk_enable(void);
extern int ns2815_usb_otg_clk_disable(void);
extern int ns2815_usb_utmi_clk_enable(void);
extern int ns2815_usb_utmi_clk_disable(void);
extern int ns2815_usb_ohci_clk_enable(void);
extern int ns2815_usb_ohci_clk_disable(void);
extern int ns2815_usb_hsic_clk_enable(void);
extern int ns2815_usb_hsic_clk_disable(void);
extern int ns2815_core_mem_clk_enable(void);
extern int ns2815_core_mem_clk_disable(void);
extern int ns2815_gpio_core_clk_enable(void);
extern int ns2815_gpio_core_clk_disable(void);
extern int ns2815_timer0_core_clk_enable(void);
extern int ns2815_timer0_core_clk_disable(void);
extern int ns2815_timer1_core_clk_enable(void);
extern int ns2815_timer1_core_clk_disable(void);
extern int ns2815_timer2_core_clk_enable(void);
extern int ns2815_timer2_core_clk_disable(void);
extern int ns2815_timer3_core_clk_enable(void);
extern int ns2815_timer3_core_clk_disable(void);
extern int ns2815_timer4_core_clk_enable(void);
extern int ns2815_timer4_core_clk_disable(void);
extern int ns2815_timer5_core_clk_enable(void);
extern int ns2815_timer5_core_clk_disable(void);
extern int ns2815_timer6_core_clk_enable(void);
extern int ns2815_timer6_core_clk_disable(void);
extern int ns2815_timer7_core_clk_enable(void);
extern int ns2815_timer7_core_clk_disable(void);
extern int ns2815_wkup_mem_clk_enable(void);
extern int ns2815_wkup_mem_clk_disable(void);
extern int ns2815_gpio_wkup_clk_enable(void);
extern int ns2815_gpio_wkup_clk_disable(void);
extern int ns2815_timer0_wkup_clk_enable(void);
extern int ns2815_timer0_wkup_clk_disable(void);
extern int ns2815_timer1_wkup_clk_enable(void);
extern int ns2815_timer1_wkup_clk_disable(void);
extern int ns2815_alt0_clk_enable(void);
extern int ns2815_alt0_clk_disable(void);
extern int ns2815_alt1_clk_enable(void);
extern int ns2815_alt1_clk_disable(void);
extern int ns2815_aux_clk_enable(void);
extern int ns2815_aux_clk_disable(void);
extern int ns2815_efuse_clk_enable(void);
extern int ns2815_efuse_clk_disable(void);
extern int ns2815_pwm_clk_enable(void);
extern int ns2815_pwm_clk_disable(void);
extern int ns2815_pll0_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_pll1_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_pll3_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_pll4_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_ddr_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_mali_pll2_set_rate(unsigned long rate); 
extern int ns2815_mali_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_gc300_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_vpu_g1_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_vpu_h1_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_isp_jpeg_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_sd_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_spi01_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_spi23_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_i2c_fs_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_i2c_hs_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_uart01_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_uart2_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_uart3_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_pxl0_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_pxl1_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_i2s_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_timer0_core_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_timer1_core_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_timer2_core_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_timer3_core_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_timer4_core_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_timer5_core_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_timer6_core_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_timer7_core_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_gpio_wkup_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_timer0_wkup_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_timer1_wkup_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_alt0_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_alt1_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int ns2815_cpu_clk_set_rate(unsigned long rate, unsigned int nouse);
extern int pmic_set(unsigned long volt);
extern void pmic_i2c_set_reg(void* i2c_base, unsigned int addr, unsigned int value);

extern void i2c_dw_init(void* i2c_base);
extern int dw_i2c_send_bytes(void* i2c_base, unsigned char * out_buf, unsigned int len);
extern int ns2815_cpu_dvfs(unsigned long rate, unsigned long volt);
extern int data_flow_test(unsigned int * addr, unsigned int data, unsigned int mask);
extern void ns2815_dataflow_test(void);
extern unsigned long get_current_freq(void);

// Section-6: SCU Register Address/Bits Definition
#define SCU_CPU_POWER_STATUS        (volatile void*)((SCU_BASE) + 0x08)

#define SCU_CPU_POWER_STATUS_CPU1_STATUS_MASK               (0x3 <<8)
#define SCU_CPU_POWER_STATUS_CPU0_STATUS_MASK               (0x3 <<0)

#endif //PRCM_H__
