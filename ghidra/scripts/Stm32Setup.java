// Pre-analysis: map RAM/peripherals, seed vector-table functions
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.symbol.*;
import ghidra.program.model.data.*;

public class Stm32Setup extends GhidraScript {
    String[] vec = {"_estack","Reset_Handler","NMI_Handler","HardFault_Handler","MemManage_Handler","BusFault_Handler","UsageFault_Handler",null,null,null,null,"SVC_Handler","DebugMon_Handler",null,"PendSV_Handler","SysTick_Handler",
     "WWDG_IRQHandler","PVD_IRQHandler","TAMPER_IRQHandler","RTC_IRQHandler","FLASH_IRQHandler","RCC_IRQHandler","EXTI0_IRQHandler","EXTI1_IRQHandler","EXTI2_IRQHandler","EXTI3_IRQHandler","EXTI4_IRQHandler",
     "DMA1_Channel1_IRQHandler","DMA1_Channel2_IRQHandler","DMA1_Channel3_IRQHandler","DMA1_Channel4_IRQHandler","DMA1_Channel5_IRQHandler","DMA1_Channel6_IRQHandler","DMA1_Channel7_IRQHandler","ADC1_IRQHandler",
     null,null,null,null,"EXTI9_5_IRQHandler","TIM1_BRK_TIM15_IRQHandler","TIM1_UP_TIM16_IRQHandler","TIM1_TRG_COM_TIM17_IRQHandler","TIM1_CC_IRQHandler","TIM2_IRQHandler","TIM3_IRQHandler","TIM4_IRQHandler",
     "I2C1_EV_IRQHandler","I2C1_ER_IRQHandler","I2C2_EV_IRQHandler","I2C2_ER_IRQHandler","SPI1_IRQHandler","SPI2_IRQHandler","USART1_IRQHandler","USART2_IRQHandler","USART3_IRQHandler","EXTI15_10_IRQHandler","RTC_Alarm_IRQHandler","CEC_IRQHandler",
     null,null,null,null,null,null,null,null,null,null,null,"TIM6_DAC_IRQHandler","TIM7_IRQHandler"};
    void blk(String n,long a,int sz) throws Exception {
        Memory m=currentProgram.getMemory();
        MemoryBlock b=m.createUninitializedBlock(n,toAddr(a),sz,false);
        b.setRead(true);b.setWrite(true);
        if(a>=0x40000000L){b.setVolatile(true);b.setExecute(false);}
    }
    public void run() throws Exception {
        blk("RAM",0x20000000L,0x2000);
        blk("APB1",0x40000000L,0x8000);
        blk("APB2",0x40010000L,0x4000);
        blk("AHB",0x40020000L,0x4000);
        blk("SCS",0xE000E000L,0x1000);
        Memory m=currentProgram.getMemory();

        Object[][] P={{"TIM2",0x40000000L},{"TIM3",0x40000400L},{"TIM4",0x40000800L},{"TIM6",0x40001000L},{"TIM7",0x40001400L},
          {"RTC",0x40002800L},{"WWDG",0x40002C00L},{"IWDG",0x40003000L},{"SPI2",0x40003800L},{"USART2",0x40004400L},{"USART3",0x40004800L},
          {"I2C1",0x40005400L},{"I2C2",0x40005800L},{"BKP",0x40006C00L},{"PWR",0x40007000L},{"DAC",0x40007400L},{"CEC",0x40007800L},
          {"AFIO",0x40010000L},{"EXTI",0x40010400L},{"GPIOA",0x40010800L},{"GPIOB",0x40010C00L},{"GPIOC",0x40011000L},{"GPIOD",0x40011400L},{"GPIOE",0x40011800L},
          {"ADC1",0x40012400L},{"TIM1",0x40012C00L},{"SPI1",0x40013000L},{"USART1",0x40013800L},{"TIM15",0x40014000L},{"TIM16",0x40014400L},{"TIM17",0x40014800L},
          {"DMA1",0x40020000L},{"DMA1_Ch1",0x40020008L},{"DMA1_Ch2",0x4002001CL},{"DMA1_Ch3",0x40020030L},{"DMA1_Ch4",0x40020044L},{"DMA1_Ch5",0x40020058L},{"DMA1_Ch6",0x4002006CL},{"DMA1_Ch7",0x40020080L},
          {"RCC",0x40021000L},{"FLASH_R",0x40022000L},{"CRC",0x40023000L},{"SysTick",0xE000E010L},{"NVIC_ISER",0xE000E100L},{"NVIC_ICER",0xE000E180L},{"NVIC_IP",0xE000E400L},{"SCB",0xE000ED00L}};
        for(Object[] e:P) createLabel(toAddr((Long)e[1]),(String)e[0],true);
        String[] rcc={"CR","CFGR","CIR","APB2RSTR","APB1RSTR","AHBENR","APB2ENR","APB1ENR","BDCR","CSR","AHBRSTR","CFGR2"};
        for(int i=1;i<rcc.length;i++) createLabel(toAddr(0x40021000L+4*i),"RCC_"+rcc[i],true);
        String[] g={"CRL","CRH","IDR","ODR","BSRR","BRR","LCKR"};
        String[] gn={"GPIOA","GPIOB","GPIOC","GPIOD"};
        for(int k=0;k<4;k++) for(int i=1;i<g.length;i++) createLabel(toAddr(0x40010800L+0x400*k+4*i),gn[k]+"_"+g[i],true);
        { MemoryBlock fb=m.getBlock(toAddr(0x08000000L)); m.split(fb,toAddr(0x0800A000L)); MemoryBlock nb=m.getBlock(toAddr(0x0800A000L)); nb.setName("FLASH_DATA"); nb.setWrite(true); nb.setVolatile(true); nb.setExecute(false);}
        for(MemoryBlock b: m.getBlocks()) if(b.getStart().getOffset()==0x08000000L){ b.setWrite(false); b.setExecute(true);}
        SymbolTable st=currentProgram.getSymbolTable();
        for(int i=0;i<vec.length;i++){
            Address va=toAddr(0x08000000L+i*4);
            createDWord(va);
            long v=m.getInt(va)&0xffffffffL;
            if(vec[i]==null||i==0||v==0) continue;
            long t=v&~1L;
            Address ta=toAddr(t);
            // set TMode
            try{ currentProgram.getProgramContext().setValue(currentProgram.getProgramContext().getRegister("TMode"),ta,ta,java.math.BigInteger.ONE);}catch(Exception ex){}
            Symbol s=st.getPrimarySymbol(ta);
            if(s==null||s.getSource()==SourceType.DEFAULT) createLabel(ta,vec[i],true);
            else createLabel(ta,vec[i],false);
            disassemble(ta);
            if(getFunctionAt(ta)==null) createFunction(ta,vec[i]);
            st.addExternalEntryPoint(ta);
        }
    }
}
