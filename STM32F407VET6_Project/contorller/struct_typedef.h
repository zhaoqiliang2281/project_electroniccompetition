/**
 ******************************************************************************
 * @file    struct_typedef.h
 * @brief   定长整数与浮点别名（RM 工程惯例：fp32、int16_t 等）
 * @note
 *   这里的 int8_t/uint16_t 等与 <stdint.h> 重复，能被编译通过是依赖 C11
 *   “同类型 typedef 允许重复”的规则；若闻到相关重定义告警，可直接删掉本文件
 *   的整型别名段，只保留 fp32 / fp64 / bool_t。
 ******************************************************************************
 */

#ifndef STRUCT_TYPEDEF_H
#define STRUCT_TYPEDEF_H


typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

/* exact-width unsigned integer types */
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned char bool_t;
typedef float fp32;
typedef double fp64;


#endif



