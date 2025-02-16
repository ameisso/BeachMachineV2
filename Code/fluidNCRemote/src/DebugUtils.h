#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT_PARAM(name,value) Serial.print(name); Serial.print(": "); Serial.print(value)
#define DEBUG_PRINTLN_PARAM(name,value) Serial.print(name); Serial.print(": "); Serial.println(value)
#define DEBUG_PRINT_HEX(x) Serial.print(x, HEX)
#define DEBUG_PRINTLN_HEX(x) Serial.println(x, HEX)
#define DEBUG_PRINT_BIT(x) Serial.println(x, BIN)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT_PARAM(name,value)
#define DEBUG_PRINTLN_PARAM(name,value)
#define DEBUG_PRINT_HEX(x)
#define DEBUG_PRINTLN_HEX(x)
#define DEBUG_PRINT_BIT(x)
#endif

