#ifndef CORE_CONFIG_H_
#define CORE_CONFIG_H_

#include <stdint.h>

#define EVENT_POOL_SIZE 24
// ======================================================
// USER CONFIGURATION
// ======================================================
//
// Format:
// X(priority_level, queue_size)
//
// priority_level : 0 = lowest
// priority_level : increasing value = higher priority
//

#define CORE_PRIORITY_TABLE \
	X(0, 128)               \
	X(1, 64)                \
	X(2, 32)                \
	X(3, 16)                \
	X(4, 16)                \
	X(5, 8)                 \
	X(6, 8)                 \
	X(7, 4)

// ======================================================
// AUTO GENERATED VALUES
// ======================================================

#define X(prio, size) +1
static constexpr uint8_t CORE_NUM_PRIORITIES = (0 CORE_PRIORITY_TABLE);
#undef X

#endif /* CORE_CONFIG_H_ */
