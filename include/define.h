#ifndef __DEFINE_H__
#define __DEFINE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h> 

typedef unsigned int 	uint32;
typedef signed int		sint32;
typedef unsigned short  uint16;
typedef signed short	sint16;
typedef unsigned char	uint8;
typedef signed char	    sint8;	

#define COM_SUCCESS 0
#define COM_FAILURE -1

#define COM_TRUE    1
#define COM_FALSE   0

typedef enum _COM_LEVEL_{
    COM_INFO = 0,
    COM_DEBUG,
    COM_WARNNING,
    COM_ERROR,
} COM_LEVEL;

/* 定义打印颜色值 */
#define COM_COLOR_GREEN       "\033[1;32m"
#define COM_COLOR_BLUE        "\033[1;34m"
#define COM_COLOR_CYAN        "\033[1;36m"
#define COM_COLOR_RED         "\033[1;31m"
#define COM_COLOR_WHITE       "\033[1;37m"
#define COM_COLOR_END         "\033[0m"

#define print(level, fmt, arg...) do { \
    char *pColor; \
    switch(level) \
    { \
        case COM_INFO: \
            pColor = COM_COLOR_GREEN; \
            break; \
        case COM_DEBUG: \
            pColor = COM_COLOR_BLUE; \
            break; \
        case COM_WARNNING: \
            pColor = COM_COLOR_CYAN; \
            break; \
        case COM_ERROR:\
            pColor = COM_COLOR_RED; \
            break; \
        default:\
            pColor = COM_COLOR_WHITE; \
            break; \
    } \
    printf("%s %s>%s[%d]: " COM_COLOR_END " " fmt, pColor, __FILE__, __FUNCTION__, __LINE__, ## arg); \
} while(0)


#ifdef __cplusplus
}
#endif
#endif