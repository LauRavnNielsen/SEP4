#pragma once

//measure bits
#define CO2_MEASURE_BIT 1 << 0
#define TEMP_HUM_MEASURE_BIT 1 << 1
#define SOUND_MEASURE_BIT 1 << 2
#define ALL_MEASURE_BITS CO2_MEASURE_BIT | TEMP_HUM_MEASURE_BIT | SOUND_MEASURE_BIT


//ready bits
#define CO2_READY_BIT 1 << 3
#define TEMP_HUM_READY_BIT 1 << 4
#define SOUND_READY_BIT 1 << 5
#define ALL_READY_BITS TEMP_HUM_READY_BIT | CO2_READY_BIT | SOUND_READY_BIT

//Connected bits
#define LORA_CONNECTED_BIT 1 << 6

//loraWAN 
#define LORA_appEUI "20B49DA07ECA0355"
#define LORA_appKEY "906848809F04C80074DD032C1CADDF84"

//delays
#define oneSecond pdMS_TO_TICKS(1000UL)
#define tenSeconds pdMS_TO_TICKS(10000UL)
#define oneMinute pdMS_TO_TICKS(60000UL)
#define oneHour pdMS_TO_TICKS(360000UL)

 