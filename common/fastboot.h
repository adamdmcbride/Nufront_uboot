
/* Global CSR Map */
#define GOTGCTL   0x000 //Control and Status Register
#define GOTGINT   0x004 //Interrupt Register
#define GAHBCFG   0x008 //AHB Configuration Register
#define GUSBCFG   0x00C //USB Configuration Register
#define GRSTCTL   0x010 //Reset Register
#define GINTSTS   0x014 //Interrupt Register
#define GINTMSK   0x018 //Interrupt Mask Register
#define GRXSTSR   0x01C //Receive Status Debug Read/Status Read and Pop Registers 
#define GRXSTSP   0x020 //
#define GRXFSIZ   0x024 //Receive FIFO Size Register
#define GNPTXFSIZ 0x028 //Non-Periodic Transmit FIFO Size Register
#define GNPTXSTS  0x02C //Non-Periodic Transmit FIFO/Queue Status Register
#define GI2CCTL   0x030 //I2C Access Register
#define GPVNDCTL  0x034  //PHY Vendor Control Register
#define GGPIO     0x038  //General Purpose Input/Output Register
#define GUID      0x03C  //User ID Register
#define GSNPSID   0x040  //Synopsys ID Register
#define GHWCFG1   0x044  //User HW Config1 Register
#define GHWCFG2   0x048  //User HW Config2 Register
#define GHWCFG3   0x04C  //User HW Config3 Register
#define GHWCFG4   0x050  //User HW Config4 Register
#define GLPMCFG   0x054  //Core LPM Configuration Register
#define GPWRDN    0x058  //Power Down Register
#define GDFIFOCFG 0x05C  //DFIFO Software Config Register
#define GADPCTL   0x060  //ADP Timer, Control and Status Register
#define HPTXFSIZ  0x100  //Host Periodic Transmit FIFO Size Register
// Device Periodic Transmit FIFO-n Size Register
#define DPTXFSIZ1  0x104
#define DPTXFSIZ2  0x108
#define DPTXFSIZ3  0x10c
#define DPTXFSIZ4  0x110
#define DPTXFSIZ5  0x114
#define DPTXFSIZ6  0x118
#define DPTXFSIZ7  0x11c
#define DPTXFSIZ8  0x120
#define DPTXFSIZ9  0x124
#define DPTXFSIZ10 0x128
#define DPTXFSIZ11 0x12c
#define DPTXFSIZ12 0x130
#define DPTXFSIZ13 0x134
#define DPTXFSIZ14 0x138
#define DPTXFSIZ15 0x13c
// Device IN Endpoint Transmit FIFO Size Register
#define DIEPTXF1  0x104
#define DIEPTXF2  0x108
#define DIEPTXF3  0x10c
#define DIEPTXF4  0x110
#define DIEPTXF5  0x114
#define DIEPTXF6  0x118
#define DIEPTXF7  0x11c
#define DIEPTXF8  0x120
#define DIEPTXF9  0x124
#define DIEPTXF10 0x128
#define DIEPTXF11 0x12c
#define DIEPTXF12 0x130
#define DIEPTXF13 0x134
#define DIEPTXF14 0x138
#define DIEPTXF15 0x13c

/* Device Mode CSR Map */
#define DCFG         0x800  //Device Configuration Register
#define DCTL         0x804  //Device Control Register
#define DSTS         0x808  //Device Status Register (DSTS)
#define DIEPMSK      0x810  //Device IN Endpoint Common Interrupt Mask Register
#define DOEPMSK      0x814  //Device OUT Endpoint Common Interrupt Mask Register
#define DAINT        0x818  //Device All Endpoints Interrupt Register
#define DAINTMSK     0x81C  //Device All Endpoints Interrupt Mask Register
#define DTKNQR1      0x820  //Device IN Token Sequence Learning Queue Read Register 1
#define DTKNQR2      0x824  //Device IN Token Sequence Learning Queue Read Register 2
#define DTKNQR3      0x830  //Device IN Token Sequence Learning Queue Read Register 3
#define DTKNQR4      0x834  //Device IN Token Sequence Learning Queue Read Register 4
#define DVBUSDIS     0x828  //Device VBUS Discharge Time Register
#define DVBUSPULSE   0x82C  //Device VBUS Pulsing Time Register
#define DTHRCTL      0x830  //Device Threshold Control Register
#define DIEPEMPMSK   0x834  //Device IN Endpoint FIFO Empty Interrupt Mask Register 
#define DEACHINT     0x838  //Device Each Endpoint Interrupt Register
#define DEACHINTMSK  0x83C  //Device Each Endpoint Interrupt Register Mask
//Device Each In Endpoint-n Interrupt Register
#define DIEPEACHMSK0  0x840
#define DIEPEACHMSK1  0x844
#define DIEPEACHMSK2  0x848
#define DIEPEACHMSK3  0x84c
#define DIEPEACHMSK4  0x850
#define DIEPEACHMSK5  0x854
#define DIEPEACHMSK6  0x858
#define DIEPEACHMSK7  0x85c
#define DIEPEACHMSK8  0x860
#define DIEPEACHMSK9  0x864
#define DIEPEACHMSK10 0x868
#define DIEPEACHMSK11 0x86c
#define DIEPEACHMSK12 0x870
#define DIEPEACHMSK13 0x874
#define DIEPEACHMSK14 0x878
#define DIEPEACHMSK15 0x87c
//Device Each Out Endpoint-n Interrupt Register
#define DOEPEACHMSK0    0x880
#define DOEPEACHMSK1    0x884
#define DOEPEACHMSK2    0x888
#define DOEPEACHMSK3    0x88c
#define DOEPEACHMSK4    0x890
#define DOEPEACHMSK5    0x894
#define DOEPEACHMSK6    0x898
#define DOEPEACHMSK7    0x89c
#define DOEPEACHMSK8    0x8a0
#define DOEPEACHMSK9    0x8a4
#define DOEPEACHMSK10   0x8a8
#define DOEPEACHMSK11   0x8ac
#define DOEPEACHMSK12   0x8b0
#define DOEPEACHMSK13   0x8b4
#define DOEPEACHMSK14   0x8b8
#define DOEPEACHMSK15   0x8bc
// Device IN Endpoint0 Registers
#define DIEPCTL0   0x900
#define DIEPINT0   0x908
#define DIEPTSIZ0  0x910
#define DIEPDMA0   0x914
#define DTXFSTS0   0x918
#define DIEPDMAB0  0x91c
// Device IN Endpoint1 Registers
#define DIEPCTL1   0x920
#define DIEPINT1   0x928
#define DIEPTSIZ1  0x930
#define DIEPDMA1   0x934
#define DTXFSTS1   0x938
#define DIEPDMAB1  0x93c
// Device IN Endpoint2 Registers
#define DIEPCTL2   0x940
#define DIEPINT2   0x948
#define DIEPTSIZ2  0x950
#define DIEPDMA2   0x954
#define DTXFSTS2   0x958
#define DIEPDMAB2  0x95c
// Device IN Endpoint3 Registers
#define DIEPCTL3   0x960
#define DIEPINT3   0x968
#define DIEPTSIZ3  0x970
#define DIEPDMA3   0x974
#define DTXFSTS3   0x978
#define DIEPDMAB3  0x97c
// Device IN Endpoint4 Registers
#define DIEPCTL4   0x980
#define DIEPINT4   0x988
#define DIEPTSIZ4  0x990
#define DIEPDMA4   0x994
#define DTXFSTS4   0x998
#define DIEPDMAB4  0x99c
// Device IN Endpoint5 Registers
#define DIEPCTL5   0x9a0
#define DIEPINT5   0x9a8
#define DIEPTSIZ5  0x9b0
#define DIEPDMA5   0x9b4
#define DTXFSTS5   0x9b8
#define DIEPDMAB5  0x9bc
// Device IN Endpoint6 Registers
#define DIEPCTL6   0x9c0
#define DIEPINT6   0x9c8
#define DIEPTSIZ6  0x9d0
#define DIEPDMA6   0x9d4
#define DTXFSTS6   0x9d8
#define DIEPDMAB6  0x9dc
// Device IN Endpoint7 Registers
#define DIEPCTL7   0x9e0
#define DIEPINT7   0x9e8
#define DIEPTSIZ7  0x9f0
#define DIEPDMA7   0x9f4
#define DTXFSTS7   0x9f8
#define DIEPDMAB7  0x9fc
// Device IN Endpoint8 Registers
#define DIEPCTL8   0xa00
#define DIEPINT8   0xa08
#define DIEPTSIZ8  0xa10
#define DIEPDMA8   0xa14
#define DTXFSTS8   0xa18
#define DIEPDMAB8  0xa1c
// Device IN Endpoint9 Registers
#define DIEPCTL9   0xa20
#define DIEPINT9   0xa28
#define DIEPTSIZ9  0xa30
#define DIEPDMA9   0xa34
#define DTXFSTS9   0xa38
#define DIEPDMAB9  0xa3c
// Device IN Endpoint10 Registers
#define DIEPCTL10   0xa40
#define DIEPINT10   0xa48
#define DIEPTSIZ10  0xa50
#define DIEPDMA10   0xa54
#define DTXFSTS10   0xa58
#define DIEPDMAB10  0xa5c
// Device IN Endpoint11 Registers
#define DIEPCTL11   0xa60
#define DIEPINT11   0xa68
#define DIEPTSIZ11  0xa70
#define DIEPDMA11   0xa74
#define DTXFSTS11   0xa78
#define DIEPDMAB11  0xa7c
// Device IN Endpoint12 Registers
#define DIEPCTL12   0xa80
#define DIEPINT12   0xa88
#define DIEPTSIZ12  0xa90
#define DIEPDMA12   0xa94
#define DTXFSTS12   0xa98
#define DIEPDMAB12  0xa9c
// Device IN Endpoint13 Registers
#define DIEPCTL13   0xaa0
#define DIEPINT13   0xaa8
#define DIEPTSIZ13  0xab0
#define DIEPDMA13   0xab4
#define DTXFSTS13   0xab8
#define DIEPDMAB13  0xabc
// Device IN Endpoint14 Registers
#define DIEPCTL14   0xac0
#define DIEPINT14   0xac8
#define DIEPTSIZ14  0xad0
#define DIEPDMA14   0xad4
#define DTXFSTS14   0xad8
#define DIEPDMAB14  0xadc
// Device IN Endpoint15 Registers
#define DIEPCTL15   0xae0
#define DIEPINT15   0xae8
#define DIEPTSIZ15  0xaf0
#define DIEPDMA15   0xaf4
#define DTXFSTS15   0xaf8
#define DIEPDMAB15  0xafc

// Device OUT Endpoint0 Registers
#define DOEPCTL0   0xb00
#define DOEPINT0   0xb08
#define DOEPTSIZ0  0xb10
#define DOEPDMA0   0xb14
#define DOEPDMAB0  0xb1c
// Device OUT Endpoint1 Registers
#define DOEPCTL1   0xb20
#define DOEPINT1   0xb28
#define DOEPTSIZ1  0xb30
#define DOEPDMA1   0xb34
#define DOEPDMAB1  0xb3c
// Device OUT Endpoint2 Registers
#define DOEPCTL2   0xb40
#define DOEPINT2   0xb48
#define DOEPTSIZ2  0xb50
#define DOEPDMA2   0xb54
#define DOEPDMAB2  0xb5c
// Device OUT Endpoint3 Registers
#define DOEPCTL3   0xb60
#define DOEPINT3   0xb68
#define DOEPTSIZ3  0xb70
#define DOEPDMA3   0xb74
#define DOEPDMAB3  0xb7c
// Device OUT Endpoint4 Registers
#define DOEPCTL4   0xb80
#define DOEPINT4   0xb88
#define DOEPTSIZ4  0xb90
#define DOEPDMA4   0xb94
#define DOEPDMAB4  0xb9c
// Device OUT Endpoint5 Registers
#define DOEPCTL5   0xba0
#define DOEPINT5   0xba8
#define DOEPTSIZ5  0xbb0
#define DOEPDMA5   0xbb4
#define DOEPDMAB5  0xbbc
// Device OUT Endpoint6 Registers
#define DOEPCTL6   0xbc0
#define DOEPINT6   0xbc8
#define DOEPTSIZ6  0xbd0
#define DOEPDMA6   0xbd4
#define DOEPDMAB6  0xbdc
// Device OUT Endpoint7 Registers
#define DOEPCTL7   0xbe0
#define DOEPINT7   0xbe8
#define DOEPTSIZ7  0xbf0
#define DOEPDMA7   0xbf4
#define DOEPDMAB7  0xbfc
// Device OUT Endpoint8 Registers
#define DOEPCTL8   0xc00
#define DOEPINT8   0xc08
#define DOEPTSIZ8  0xc10
#define DOEPDMA8   0xc14
#define DOEPDMAB8  0xc1c
// Device OUT Endpoint9 Registers
#define DOEPCTL9   0xc20
#define DOEPINT9   0xc28
#define DOEPTSIZ9  0xc30
#define DOEPDMA9   0xc34
#define DOEPDMAB9  0xc3c
// Device OUT Endpoint10 Registers
#define DOEPCTL10   0xc40
#define DOEPINT10   0xc48
#define DOEPTSIZ10  0xc50
#define DOEPDMA10   0xc54
#define DOEPDMAB10  0xc5c
// Device OUT Endpoint11 Registers
#define DOEPCTL11   0xc60
#define DOEPINT11   0xc68
#define DOEPTSIZ11  0xc70
#define DOEPDMA11   0xc74
#define DOEPDMAB11  0xc7c
// Device OUT Endpoint12 Registers
#define DOEPCTL12   0xc80
#define DOEPINT12   0xc88
#define DOEPTSIZ12  0xc90
#define DOEPDMA12   0xc94
#define DOEPDMAB12  0xc9c
// Device OUT Endpoint13 Registers
#define DOEPCTL13   0xca0
#define DOEPINT13   0xca8
#define DOEPTSIZ13  0xcb0
#define DOEPDMA13   0xcb4
#define DOEPDMAB13  0xcbc
// Device OUT Endpoint14 Registers
#define DOEPCTL14   0xcc0
#define DOEPINT14   0xcc8
#define DOEPTSIZ14  0xcd0
#define DOEPDMA14   0xcd4
#define DOEPDMAB14  0xcdc
// Device OUT Endpoint15 Registers
#define DOEPCTL15   0xce0
#define DOEPINT15   0xce8
#define DOEPTSIZ15  0xcf0
#define DOEPDMA15   0xcf4
#define DOEPDMAB15  0xcfc

// Fifos
#define DEVEP0_FF			0x00001000
#define DEVEP1_FF			0x00002000
#define DEVEP2_FF			0x00003000

