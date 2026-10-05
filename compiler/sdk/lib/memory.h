#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

#define USER_HEAP_RESERVED		0x200000

#ifdef __cplusplus
extern "C" {
#endif
	/**
	 * @brief Allocate `size` bytes in heap.
	 *
	 * @param size Total amount in `bytes` to allocate.
	 * @return A pointer to the first byte allocated memory.
	 */
	void	*malloc(size_t size);

	/**
	 * @brief Free an allocation.
	 *
	 * @param ptr The pointer that need to be freed.
	 */
	void	free(void *ptr);

	/**
	 * @brief Allocate and cleanup a buffer.
	 *
	 * Act as a malloc and a memset, allocates nmemb*size bytes and set them to zero.
	 *
	 * @param nmemb The number of members of the asked type.
	 * @param size Memory size of a single member.
	 * @return A pointer to the first byte of the newly allocated buffer.
	 */
	void	*calloc(size_t nmemb, size_t size);

	/**
	 * @brief Re-allocates a buffer to change its size.
	 *
	 * Copy your old allocation content into a newer one with more space available.
	 *
	 * @param ptr Old allocation.
	 * @param b New buffer size.
	 * @return A pointer to the first byte of the re-allocated buffer.
	 */
	void	*realloc(void *ptr, size_t size);
#ifdef __cplusplus
}
#endif

#endif
