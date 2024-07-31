#ifndef CORE_UTILS_H
#define CORE_UTILS_H

/*encode-decode*/
#define ENCODE_16(val, buf, index)\
buf[index] = ((val>>8) & 0xFF);\
buf[index+1] = (val & 0xFF);

#define ENCODE_32(val, buf, index)\
buf[index] = ((val>>24) & 0xFF);\
buf[index+1] = ((val>>16) & 0xFF);\
buf[index+2] = ((val>>8) & 0xFF);\
buf[index+3] = (val & 0xFF);
#define DECODE_32(data, index)  (((uint32_t)data[index]<<24) + ((uint32_t)data[index+1]<<16) + ((uint32_t)data[index+2]<<8) + data[index+3])
#define DECODE_16(data, index)  (((uint16_t)data[index]<<8) + data[index+1])

#endif // COMPONENT_H
