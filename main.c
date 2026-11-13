#include <stdio.h>
#include <locale.h>
#include "dynamic_array.h"

#ifdef _WIN32
#include <windows.h>
#endif

static void print_status(const char *message, size_t current_size, size_t current_capacity) {
    printf("%s: размер = %zu, вместимость = %zu\n", message, current_size, current_capacity);
}

int main(void) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    DynamicArray array;
    ArrayError error_code;
    int fetched_value;

    printf("=== ДЕМОНСТРАЦИЯ РАБОТЫ ДИНАМИЧЕСКОГО МАССИВА ===\n\n");

    error_code = array_create(&array, 1);
    if (error_code != ARRAY_OK) {
        printf("Ошибка: не удалось создать массив!\n");
        return 1;
    }

    print_status("1. После создания", array_size(&array), array_capacity(&array));

    printf("\n2. Тест роста вместимости (30 операций добавления):\n");
    size_t last_capacity = 0;
    for (int i = 0; i < 30; ++i) {
        error_code = array_push_back(&array, i * 10);
        if (error_code != ARRAY_OK) {
            printf("Ошибка: сбой push_back на шаге %d\n", i);
            break;
        }
        size_t current_capacity = array_capacity(&array);
        if (current_capacity != last_capacity) {
            last_capacity = current_capacity;
            print_status("   -> Вместимость перевыделена", array_size(&array), current_capacity);
        }
    }

    DynamicArray empty_array;
    array_create(&empty_array, 1);
    printf("\n3. Тест граничного случая для пустого массива:\n");
    error_code = array_get(&empty_array, 0, &fetched_value);
    if (error_code == ARRAY_ERR_OUT_OF_RANGE) {
        printf("   Успешно перехвачена ожидаемая ошибка: ARRAY_ERR_OUT_OF_RANGE (чтение из пустого массива)\n");
    }
    array_destroy(&empty_array);

    printf("\n4. Вставка в начало (позиция 0):\n");
    error_code = array_insert(&array, 0, -999);
    if (error_code == ARRAY_OK) {
        print_status("   После вставки в начало", array_size(&array), array_capacity(&array));
        array_get(&array, 0, &fetched_value);
        printf("   Проверка: первый элемент равен %d (ожидается -999)\n", fetched_value);
    }

    printf("\n5. Удаление последнего элемента:\n");
    size_t current_size = array_size(&array);
    error_code = array_erase(&array, current_size - 1);
    if (error_code == ARRAY_OK) {
        print_status("   После удаления последнего элемента", array_size(&array), array_capacity(&array));
    }

    printf("\n6. Проверка границ (обращение за пределы):\n");
    size_t invalid_index = array_size(&array) + 5;
    error_code = array_get(&array, invalid_index, &fetched_value);
    if (error_code == ARRAY_ERR_OUT_OF_RANGE) {
        printf("   Успешно перехвачена ошибка ARRAY_ERR_OUT_OF_RANGE при чтении по индексу %zu\n", invalid_index);
    }

    printf("\n7. Изменение и чтение по индексу:\n");
    size_t target_index = 5;
    if (target_index < array_size(&array)) {
        array_set(&array, target_index, 777);
        array_get(&array, target_index, &fetched_value);
        printf("   Элемент по индексу %zu успешно изменен, значение: %d (ожидается 777)\n", target_index, fetched_value);
    }

    printf("\n8. Вставка в позицию, равную размеру (в самый конец):\n");
    size_t insert_position = array_size(&array);
    array_insert(&array, insert_position, 888);
    print_status("   После вставки по позиции размера", array_size(&array), array_capacity(&array));
    array_get(&array, array_size(&array) - 1, &fetched_value);
    printf("   Проверка: последний элемент равен %d (ожидается 888)\n", fetched_value);

    array_destroy(&array);
    printf("\n9. Массив уничтожен, память успешно освобождена.\n");

    return 0;
}
