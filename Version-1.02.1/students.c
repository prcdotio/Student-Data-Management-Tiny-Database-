#include "students.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int initialize_database(void){
    FILE *fptr = fopen("students_details.bin", "rb");
    if(fptr != NULL){
        fclose(fptr);
        return 1;
    }
    char choice;
    printf("Binary file named \"students_details.bin\" doesn't exist. Do you wanna create new one? [Y/N]:\t");
    if(!scanf(" %c", &choice)){
        return 0;
    }
    if(choice == 'y' || choice == 'Y'){
        FILE *fptr = fopen("students_details.bin", "wb");
        if(fptr == NULL){
            printf("Binary file creation unsuccessful. Please check for permissions.\n");
            return 0;
        }
        printf("Binary file creation success.\n");
        fclose(fptr);
        save_to_file(NULL, 0);
        return 1;
    }
    return 0;
}

void save_to_file(struct students *storage, int *size){
    FILE *fptr = fopen("students_details.bin", "wb");
    if(fptr == NULL){
        return;
    }
    fwrite(storage, sizeof(struct students), *size, fptr);
    fclose(fptr);
}

int count_size(void){
    FILE *fptr = fopen("students_details.bin", "rb");
    if(fptr == NULL){
        return 0;
    }
    fseek(fptr, 0, SEEK_END);
    long total_bytes = ftell(fptr);
    fclose(fptr);
    return (int) (total_bytes / sizeof(struct students));
}

struct students *create_storage(int *size){
    struct students *storage = (struct students *)malloc(*size * sizeof(struct students));
    if(storage == NULL){
        return NULL;
    }
    return storage;
}

struct students *load_data(struct students *storage, int *size){
    if(*size == 0){
        return storage;
    }
    FILE *fptr = fopen("students_details.bin", "rb");
    if(fptr == NULL){
        return storage;
    }
    size_t items_read = fread(storage, sizeof(struct students), *size, fptr);
    if(items_read != *size){
        printf("Warning: Expected to read %d records, but only read %zu.\n", *size, items_read);
    }
    fclose(fptr);
    return storage;
}

void print_data(struct students *storage, int *size){
    printf("%-10s | %-10s | %-20s | %s\n", "S.No.", "ID", "Name", "GPA");
    printf("_______________________________________________\n");
    for (int i = 0; i < *size; ++i){
        printf("%-10d | %-10d | %-20s | %.2f\n", i + 1, (storage + i)->id, (storage + i)->name, (storage + i)->gpa);
    }
}

struct students *add_new_students(struct students *storage, int *size){
    char choice;
    printf("Do you wanna add new student? [Y/N]:\t");
    if(!scanf(" %c", &choice)){
        printf("Error: Invalid input.\n");
        return storage;
    }
    while(choice == 'y' || choice == 'Y'){
        struct students *temp = realloc(storage, (*size + 1) * sizeof(struct students));
        if(temp == NULL){
            printf("Error: Unable to add new students.\n");
            return storage;
        }
        storage = temp;
        int new_id;
        int valid = 0;
do {
    printf("ID:\t");
    if (scanf("%d", &new_id) != 1) {
        printf("Error: Please enter a numeric ID.\n");
        while (getchar() != '\n');
        continue; 
    }
    if (!is_id_valid(new_id)) {
        continue;
    }
    if (is_id_exist(storage, size, new_id, -1)) {
        printf("Error: Duplicate ID. This ID is already taken.\n");
        continue;
    }
    valid = 1;
} while (!valid);
(storage + *size)->id = new_id;
        getchar();
        printf("Name:\t");
        fgets((storage + *size)->name, 50 * sizeof(char), stdin);
        (storage + *size)->name[strcspn((storage + *size)->name, "\n")] = '\0';
        do{
            printf("GPA:\t");
            if(!scanf("%f", &(storage + *size)->gpa)){
                printf("Error: Invalid input.\n");
                while (getchar() != '\n')
                    ;
                continue;
            }
            if(!is_gpa_valid((storage + *size)->gpa)){
                printf("Error: Invalid GPA. GPA must me 0.0 to 4.0\n");
            }
        } while (!is_gpa_valid((storage + *size)->gpa));

        (*size)++;
        save_to_file(storage, size);
        printf("Wanna add more? [Y/N]:\t");
        scanf(" %c", &choice);
    }
    return storage;
}

int is_id_valid(int id){
    if(id <= 0){
        printf("Error: ID must be positive integer.\n");
        return 0;
    }
    return 1;
}

int is_id_exist(struct students *storage, int *size, int new_id, int original_id){
    for (int i = 0; i < *size; ++i){
        if((storage + i)->id == original_id){
            continue;
        }
        if((storage + i)->id == new_id){
            return 1;
        }
    }
    return 0;
}

int is_gpa_valid(int current_gpa){
    if(current_gpa <= 4 && current_gpa >= 0){
        return 1;
    }
    return 0;
}

struct students *update(struct students *storage, int *size){
    char choice = 'y';
    while(choice == 'y' || choice == 'Y'){
        int search_option;
        printf("1. ID\n");
        printf("2. Name\n");
        printf("3. Cancel\n");
        printf("By which data you wanna update? [1/2]:\t");
        if(!scanf("%d", &search_option)){
            printf("Error: Invalid input.\n");
            while(getchar() != '\n')
                ;
            continue;
        }
        if(search_option == 3){
            printf("Update cancelled!\n");
            break;
        }
        if(search_option != 1 && search_option != 2){
            printf("Error: out of criteria.\n");
            continue;
        }
        switch(search_option){
            case 1:
                int found_index = search_by_id(storage, size, "edit");
                if(found_index != -1){
                   storage = edit(storage, size, found_index);
                }
                break;
                case 2:
                    found_index = search_by_name(storage, size, "edit");
                    if(found_index != -1){
                        storage = edit(storage, size, found_index);
                    }
                    break;
                default:
                    printf("Error: Invalid input.\n");
                    break;
                }
                printf("Wanna update more? [Y/N]:\t");
                scanf(" %c", &choice);
    }
    return storage;
}

int search_by_id(struct students *storage, int *size, char status[]){
    int search_id;
    printf("Enter the ID you wanna %s:\t", status);
    while(scanf("%d", &search_id) != 1){
        printf("Error: Invalid input.\n");
        while(getchar() != '\n')
            ;
    }
    int found_index = -1;
    for (int i = 0; i < *size; ++i){
        if(search_id == (storage + i)->id){
            found_index = i;
            break;
        }
    }
    if(found_index == -1){
        printf("Student with ID: %d not found.\n", search_id);
    }
    return found_index;
}

int search_by_name(struct students *storage, int *size, char status[]){
    char search_name[50];
    printf("Enter the full name of students you wanna %s:\t", status);
    getchar();
    if(!fgets(search_name, 50 * sizeof(char), stdin))
    {
        return -1;
    }
    search_name[strcspn(search_name, "\n")] = '\0';
    int found_index = -1;
    for (int i = 0; i < *size; ++i)
    {
        if(strcmp(search_name, (storage + i)->name) == 0){
            found_index = i;
            break;
        }
    }
    if(found_index == -1){
        printf("Student with Name: %s not found.\n", search_name);
    }
    return found_index;
}

struct students *edit(struct students *storage, int *size, int found_index){
    int original_id = (storage + found_index)->id;
   int new_id;
        int valid = 0;
do {
    printf("ID:\t");
    if (scanf("%d", &new_id) != 1) {
        printf("Error: Please enter a numeric ID.\n");
        while (getchar() != '\n');
        continue; 
    }
    if (!is_id_valid(new_id)) {
        continue;
    }
    if (is_id_exist(storage, size, new_id, original_id)) {
        printf("Error: Duplicate ID. This ID is already taken.\n");
        continue;
    }
    valid = 1;
} while (!valid);
(storage + found_index)->id = new_id;
    getchar();
    printf("Name:\t");
    fgets((storage + found_index)->name, 50 * sizeof(char), stdin);
    (storage + found_index)->name[strcspn((storage + found_index)->name, "\n")] = '\0';
    printf("GPA:\t");
    scanf("%f", &(storage + found_index)->gpa);
    char choice;
    printf("Are you sure to edit those file? [Y/N]\t");
    if(scanf(" %c", &choice)){
        if(choice == 'y' || choice == 'Y'){
            save_to_file(storage, size);
            printf("Database updated successfully.\n");
        }
        else
        {
            load_data(storage, size);
            printf("Database update cancelled.\n");
        }
        }
        return storage;
}

struct students *delete(struct students *storage, int *size){
    char choice = 'y';
    while(choice == 'y' || choice == 'Y'){
        int search_option;
        printf("1. ID:\n");
        printf("2. Name:\n");
        printf("3. Cancel\n");
        printf("By which data you wanna delete? [1/2]:\t");
        if(!scanf("%d", &search_option)){
            printf("Error: Invalid input.\n");
            while(getchar() != '\n')
                ;
            continue;
        }
        if(search_option == 3){
            printf("Deletion cancelled!\n");
            break;
        }
        if(search_option != 1 && search_option != 2){
            printf("Error: out of criteria.\n");
            continue;
        }
        switch(search_option){
            case 1:
                int found_index = search_by_id(storage, size, "delete");
                if (found_index != -1)
                {
                    storage = remove_data(storage, size, found_index);
                }
                break;
                case 2:
                    found_index = search_by_name(storage, size, "delete");
                    if(found_index != -1){
                        storage = remove_data(storage, size, found_index);
                    }
                    break;
                    default:
                        printf("Error: Invalid input.\n");
                    }
                    if(*size > 0){
                        printf("Wanna delete more? [Y/N]");
                        if(!scanf(" %c", &choice)){
                            printf("Invalid input.\n");
                            return storage;
                        }
                    }else{
                        printf("Database is empty. Now, returning to main menu.\n");
                        break;
                    }
    }
    return storage;
}

struct students *remove_data(struct students *storage, int *size, int found_index){
    char choice;
    printf("Are you sure to delete? [Y/N]:\t");
    if(!scanf(" %c", &choice)){
        printf("Error: Invalid input.\n");
        return storage;
    }
    if(choice == 'y' || choice == 'Y'){
        int num_to_move = *size - found_index - 1;
        memmove((storage + found_index), (storage + found_index + 1), num_to_move * sizeof(struct students));
        (*size)--;
        if(*size > 0){
            struct students *temp = realloc(storage, *size * sizeof(struct students));
            if(temp != NULL){
                storage = temp;
            }
        }else{
            free(storage);
            return NULL;
        }
        save_to_file(storage, size);
        return storage;
    }
    return storage;
}

void sort_database(struct students *storage, int *size){
    if(storage == NULL || *size < 2){
        printf("Not enough data to sort.\n");
        return;
    }
    int category;
    printf("====================\n");
    printf("1. By ID\n");
    printf("2. By Name\n");
    printf("3. By GPA\n");
    printf("=====================\n");
    if(!scanf("%d", &category) || category < 1 || category > 3){
        printf("Error: Invalid input.\n");
        while(getchar() != '\n')
            ;
        return;
    }
    int direction;
    printf("========================\n");
    printf("1. Ascending\n");
    printf("2. Descending\n");
    printf("=========================\n");
    if(!scanf("%d", &direction) || direction < 1 || direction > 2){
        printf("Error: Invalid input.\n");
        while(getchar() != '\n')
            ;
        return;
    }
    if(category == 1 && direction == 1){
        qsort(storage, *size, sizeof(struct students), compare_id_asc);
    }else if(category == 1 && direction == 2){
        qsort(storage, *size, sizeof(struct students), compare_id_desc);
    }else if(category == 2 && direction == 1){
        qsort(storage, *size, sizeof(struct students), compare_name_asc);
    }else if(category == 2 && direction == 2){
        qsort(storage, *size, sizeof(struct students), compare_name_desc);
    }else if(category == 3 && direction == 1){
        qsort(storage, *size, sizeof(struct students), compare_gpa_asc);
    }else if(category == 3 && direction == 2){
        qsort(storage, *size, sizeof(struct students), compare_gpa_desc);
    }
    print_data(storage, size);
    char choice;
    printf("Do you wanna sort to database also? [Y/N]\t");
    if(!scanf(" %c", &choice)){
        printf("Error: Invalid input.\n");
        return;
    }
    if(choice == 'Y' || choice == 'y'){
        save_to_file(storage, size);
        printf("Database also sorted (Highest to lowest) and saved.\n");
    }
}

int compare_id_asc(const void *a, const void *b){
    struct students *s1 = (struct students *)a;
    struct students *s2 = (struct students *)b;
    return s1->id - s2->id;
}

int compare_id_desc(const void *a, const void *b){
    struct students *s1 = (struct students *)a;
    struct students *s2 = (struct students *)b;
    return s2->id - s1->id;
}

int compare_name_asc(const void *a, const void *b){
    struct students *s1 = (struct students *)a;
    struct students *s2 = (struct students *)b;
    return strcmp(s1->name, s2->name);
}

int compare_name_desc(const void *a, const void *b){
    struct students *s1 = (struct students *)a;
    struct students *s2 = (struct students *)b;
    return strcmp(s2->name, s1->name);
}

int compare_gpa_asc(const void *a, const void *b){
    struct students *s1 = (struct students *)a;
    struct students *s2 = (struct students *)b;
    if(s1->gpa > s2->gpa){
        return 1;
    }
    if(s1->gpa < s2->gpa){
        return -1;
    }
    return 0;
}

int compare_gpa_desc(const void *a, const void *b){
    struct students *s1 = (struct students *)a;
    struct students *s2 = (struct students *)b;
    if(s1->gpa < s2->gpa){
        return 1;
    }
    if(s1->gpa > s2->gpa){
        return -1;
    }
    return 0;
}

void free_memory(struct students *storage){
    free(storage);
}