/** 
* @file ozb_compiler_c30.h
* @brief Microchip C30 support for OpenZB.
* @author Christian Nastasi
* @date 2009-03-22
*/
#ifndef __ozb_compiler_c30_h__
#define __ozb_compiler_c30_h__

/** 
* @name Basic type representations.
* @{ */
#ifndef UINT8_T
#define UINT8_T
typedef unsigned char uint8_t;		/**< Unsigned 8 bit intgerer. */
#endif

#ifndef UINT16_T
#define UINT16_T
typedef unsigned short int uint16_t;	/**< Unsigned 16 bit intgerer. */
#endif

#ifndef UINT32_T
#define UINT32_T
typedef unsigned long int uint32_t;	/**< Unsigned 32 bit intgerer. */
#endif

#ifndef INT8_T
#define INT8_T
typedef signed char int8_t;		/**< Unsigned 8 bit intgerer. */
#endif

#ifndef INT16_T
#define INT16_T
typedef signed short int int16_t;	/**< Unsigned 16 bit intgerer. */
#endif

#ifndef INT32_T
#define INT32_T
typedef signed long int int32_t;	/**< Unsigned 32 bit intgerer. */
#endif
/**  @} */

#ifndef HAL_INLINE 
#define HAL_INLINE static inline
#endif

#endif /* Header Protection */
