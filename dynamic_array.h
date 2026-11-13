#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stddef.h>

typedef enum {
    ARRAY_OK = 0,
    ARRAY_ERR_NULL_PTR = 1,
    ARRAY_ERR_OUT_OF_RANGE = 2,
    ARRAY_ERR_MEMORY = 3
} ArrayError;

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} DynamicArray;

ArrayError array_create(DynamicArray *arr, size_t initial_capacity);
void array_destroy(DynamicArray *arr);

ArrayError array_push_back(DynamicArray *arr, int value);
ArrayError array_insert(DynamicArray *arr, size_t pos, int value);
ArrayError array_erase(DynamicArray *arr, size_t pos);

ArrayError array_get(const DynamicArray *arr, size_t index, int *out_value);
ArrayError array_set(DynamicArray *arr, size_t index, int value);

size_t array_size(const DynamicArray *arr);
size_t array_capacity(const DynamicArray *arr);

#endif // DYNAMIC_ARRAY_H
