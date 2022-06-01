
/*base64.h*/  
#ifndef _BASE64_H  
#define _BASE64_H  
  
#include <stdlib.h>  
#include <string.h>  
#include <stdio.h>  

#ifdef __cplusplus
extern "C" {
#endif

char *base64_encode(uint8_t *str,uint32_t img_len);  
  
uint8_t *bae64_decode(uint8_t *code);  

#ifdef __cplusplus
}
#endif
#endif  