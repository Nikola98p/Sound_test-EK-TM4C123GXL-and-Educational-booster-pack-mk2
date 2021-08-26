#define SIZE 20
#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))
#define GPIO_PORTF_DATA_R   (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R    (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R  (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R    (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R    (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R   (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R     (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R  (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R   (*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R      (*((volatile unsigned long *)0x400FE108))

unsigned long Cnt;
unsigned long sound;

void PortF_Init(void){
    volatile unsigned long delay;
    SYSCTL_RCGC2_R |= 0x00000020; //F clock
    delay = SYSCTL_RCGC2_R; //DELAY
    GPIO_PORTF_LOCK_R = 0x4C4F434B; //Unlocks protF PF0
    GPIO_PORTF_CR_R = 0x1F; //Allow changes to PF4-0
    GPIO_PORTF_AMSEL_R = 0x00; //disabled analog funciton
    GPIO_PORTF_PCTL_R = 0x00000000; // GPIO clear bit PCTL
    GPIO_PORTF_DIR_R = 0x0E; //PF4, PF0 input, PF3, PF2, PF1 output
    GPIO_PORTF_AFSEL_R = 0x00; //No alternate function
    GPIO_PORTF_PUR_R = 0x11; //enable pull-up resistors on PF4,PF0
    GPIO_PORTF_DEN_R = 0x1F; //enable digital pins PF4-PF0
}

void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;              // 1) disable SysTick during setup
  NVIC_ST_RELOAD_R = 0x00FFFFFF;   // 2) maximum reload value
  NVIC_ST_CURRENT_R = 0;           // 3) any write to current clears it
  NVIC_ST_CTRL_R = 0x00000005;     // 4) enable SysTick with core clock
}

void Delay(void){
    unsigned long volatile time;
    time = 145448; //0.1
    while(time){
        time--;
    }
}

int main(void){
  unsigned long i,last,now;
  PortF_Init();   // make PF3-1 out
  SysTick_Init(); // initialize SysTick, runs at 16 MHz
  i = 0;          // array index
  last = NVIC_ST_CURRENT_R;
  while(1){
    sound = GPIO_PORTF_DATA_R;   // read previous
    sound = sound^0x06;
    GPIO_PORTF_DATA_R = sound;   // output
    if(i<50){
      now = NVIC_ST_CURRENT_R;
      Time[i] = (last-now)&0x00FFFFFF;  // 24-bit time difference
      Data[i] = GPIO_PORTF_DATA_R&0x02; // record PF1
      last = now;
      i++;
    }
    Delay();
  }
}
