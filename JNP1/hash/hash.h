/** @file
 * Plik nagłówkowy biblioteki udostępniającej
 * nieuporządkowane zbiory ciągów liczb dla języka C.
 *
 * @authors Michał Płachta, Mikołaj Szymański
 */

#ifndef HASH_H
#define HASH_H

#ifdef __cplusplus

#include <cinttypes>
#include <cstdlib>

namespace jnp1 {
extern "C" {

#else

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

#endif

#ifdef __cplusplus
    using hash_function_t = uint64_t (*) (uint64_t const*, size_t);
#else
    typedef uint64_t (*hash_function_t)(const uint64_t *, size_t);
#endif

unsigned long hash_create(hash_function_t hash_function);

void hash_delete(unsigned long id);

size_t hash_size(unsigned long id);

bool hash_insert(unsigned long id, uint64_t const * seq, size_t size);

bool hash_remove(unsigned long id, uint64_t const * seq, size_t size);

void hash_clear(unsigned long id);

bool hash_test(unsigned long id, uint64_t const * seq, size_t size);


#ifdef __cplusplus

} // extern "C"
} // namespace jnp1

#endif

#endif // HASH_H