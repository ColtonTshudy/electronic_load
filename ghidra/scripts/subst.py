import re,sys,struct
d=open(sys.argv[1],'rb').read()
src=open(sys.argv[2]).read()
P={0x40000000:"TIM2",0x40000400:"TIM3",0x40000800:"TIM4",0x40001000:"TIM6",0x40001400:"TIM7",0x40002800:"RTC",0x40002C00:"WWDG",0x40003000:"IWDG",0x40003800:"SPI2",0x40004400:"USART2",0x40004800:"USART3",0x40005400:"I2C1",0x40005800:"I2C2",0x40006C00:"BKP",0x40007000:"PWR",0x40007400:"DAC",0x40010000:"AFIO",0x40010400:"EXTI",0x40010800:"GPIOA",0x40010C00:"GPIOB",0x40011000:"GPIOC",0x40011400:"GPIOD",0x40012400:"ADC1",0x40012C00:"TIM1",0x40013000:"SPI1",0x40013800:"USART1",0x40014000:"TIM15",0x40014400:"TIM16",0x40014800:"TIM17",0x40020000:"DMA1",0x40021000:"RCC",0x40022000:"FLASH",0xE000E010:"SysTick",0xE000E100:"NVIC",0xE000ED00:"SCB"}
def name(v):
    for b,n in sorted(P.items(),reverse=True):
        if b<=v<b+0x400: return n if v==b else "%s+0x%x"%(n,v-b)
    return None
def rep(m):
    a=int(m.group(1),16); o=a-0x08000000
    if o<0 or o+4>0xA000: return m.group(0)
    v=struct.unpack_from('<I',d,o)[0]
    n=name(v)
    return "0x%x/*%s*/"%(v,n) if n else "0x%x"%v
src=re.sub(r'\b(?:DAT|PTR_DAT|PTR_FUN|PTR_LAB|puRam|pcRam|piRam|psRam|pbRam|uRam|iRam)_?(0800[0-9a-f]{4})\b',rep,src)
open(sys.argv[3],'w').write(src)
