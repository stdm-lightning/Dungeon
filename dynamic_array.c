#include "dynamic_array.h"
#include <stdlib.h>

ArrayError array_create(DynamicArray *arr, size_t initial_capacity) {
    if (arr == NULL) return ARRAY_ERR_NULL_PTR;
    if (initial_capacity == 0) initial_capacity = 1;

    arr->data = (int *)malloc(initial_capacity * sizeof(int));
    if (arr->data == NULL) {
        arr->size = 0;
        arr->capacity = 0;
        return ARRAY_ERR_MEMORY;
    }
    arr->size = 0;
    arr->capacity = initial_capacity;
    return ARRAY_OK;
}

void array_destroy(DynamicArray *arr) {
    if (arr == NULL) return;
    free(arr->data);
    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
}

static ArrayError array_ensure_capacity(DynamicArray *arr) {
    if (arr->size < arr->capacity) return ARRAY_OK;

    size_t new_capacity = arr->capacity * 2;
    int *new_data = (int *)realloc(arr->data, new_capacity * sizeof(int));
    if (new_data == NULL) {
        return ARRAY_ERR_MEMORY;
    }
    arr->data = new_data;
    arr->capacity = new_capacity;
    return ARRAY_OK;
}

ArrayError array_push_back(DynamicArray *arr, int value) {
    if (arr == NULL) return ARRAY_ERR_NULL_PTR;

    ArrayError err = array_ensure_capacity(arr);
    if (err != ARRAY_OK) return err;

    arr->data[arr->size] = value;
    arr->size++;
    return ARRAY_OK;
}

ArrayError array_insert(DynamicArray *arr, size_t pos, int value) {
    if (arr == NULL) return ARRAY_ERR_NULL_PTR;
    if (pos > arr->size) return ARRAY_ERR_OUT_OF_RANGE;

    ArrayError err = array_ensure_capacity(arr);
    if (err != ARRAY_OK) return err;

    for (size_t i = arr->size; i > pos; --i) {
        arr->data[i] = arr->data[i - 1];
    }
    arr->data[pos] = value;
    arr->size++;
    return ARRAY_OK;
}

ArrayError array_erase(DynamicArray *arr, size_t pos) {
    if (arr == NULL) return ARRAY_ERR_NULL_PTR;
    if (pos >= arr->size) return ARRAY_ERR_OUT_OF_RANGE;

    for (size_t i = pos; i < arr->size - 1; ++i) {
        arr->data[i] = arr->data[i + 1];
    }
    arr->size--;
    return ARRAY_OK;
}

ArrayError array_get(const DynamicArray *arr, size_t index, int *out_value) {
    if (arr == NULL || out_value == NULL) return ARRAY_ERR_NULL_PTR;
    if (index >= arr->size) return ARRAY_ERR_OUT_OF_RANGE;
    *out_value = arr->data[index];
    return ARRAY_OK;
}

ArrayError array_set(DynamicArray *arr, size_t index, int value) {
    if (arr == NULL) return ARRAY_ERR_NULL_PTR;
    if (index >= arr->size) return ARRAY_ERR_OUT_OF_RANGE;
    arr->data[index] = value;
    return ARRAY_OK;
}

size_t array_size(const DynamicArray *arr) {
    return (arr != NULL) ? arr->size : 0;
}

size_t array_capacity(const DynamicArray *arr) {
    return (arr != NULL) ? arr->capacity : 0;
}
