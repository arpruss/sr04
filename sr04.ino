#define BITBAND_PERIPHERAL_BASE  0x42000000u
#define PERIPHERAL_START         0x40000000u
#define BITBAND_PERIPHERAL(address, bit) *( (volatile uint32*)( BITBAND_PERIPHERAL_BASE + ((uint32)(address)-PERIPHERAL_START) * 32u + (uint32)(bit)*4u) )
#define GPIO_READ(gpioBase, bit) BITBAND_PERIPHERAL((uint32)&(gpioBase->IDR), (bit))

static const uint32_t cyclesPerUS = (F_CPU / 1000000ul);
volatile uint32* DWT_CTRL = (uint32*)(0xE0001000UL);                           
volatile uint32* DWT_CYCCNT = (uint32*)(0xE0001000UL+4);    
volatile uint32* CoreDebug_DEMCR = (uint32*)(0xE000EDF0UL+12);                       
#define CoreDebug_DEMCR_TRCENA_Msk (1u<<24)
#define CYCCNT *DWT_CYCCNT

#define ECHO_GPIO_BASE GPIOB_BASE // PB4
#define ECHO_BIT  4    // PB4
const uint32 echoPin = PB4;
const uint32 triggerPin = PB3;

void setup() {
  *CoreDebug_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  *DWT_CTRL = 1;
  pinMode(triggerPin,OUTPUT);
  pinMode(echoPin,INPUT);
  Serial.begin(9600);
}

int32 measureEcho(int samples) {
  uint32 total = 0;
  for (int i=0; i<samples; i++) {  
    digitalWrite(triggerPin,1);
    delayMicroseconds(10);
    digitalWrite(triggerPin,0);
    
    CYCCNT = 0;
    do {
      if (CYCCNT >= cyclesPerUS * 100000)
        return -1;
    }
    while (! GPIO_READ(ECHO_GPIO_BASE, ECHO_BIT));
    CYCCNT = 0;
    do {
      if (CYCCNT >= cyclesPerUS * 100000)
        return -1;
    }
    while (GPIO_READ(ECHO_GPIO_BASE, ECHO_BIT));
    total += CYCCNT;
  }

  return total / samples / cyclesPerUS;
}

void loop() {
  int32 t = measureEcho(10);
  if (0 <= t) 
    Serial.println(t);
  else
    Serial.println("No echo");
  delay(100);
}

