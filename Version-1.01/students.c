#include "students.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int initialize_database(void){
    FILE *fptr = fopen("students_details.txt", "r");
    if(fptr != NULL){
        fclose(fptr);
        return 1;
    }
    char choice;
    printf("File named \"students_details.txt\" doesn't exist. Do you wanna create new one? [Y/N]:\t");
    if(!scanf(" %c", &choice)){
        return 0;
    }
    if(choice == 'y' || choice == 'Y'){
        FILE *fptr = fopen("students_details.txt", "w");
        if(fptr == NULL){
            printf("File creation unsuccessful. Please check for permissions.\n");
            return 0;
        }
        printf("File creation success.\n");
        fclose(fptr);
        save_to_file(NULL, 0);
        return 1;
    }
    return 0;
}

void save_to_file(struct students *storage, int *size){
    FILE *fptr = fopen("students_details.txt", "w");
    if(fptr == NULL){
        return;
    }
    fprintf(fptr, "%-10s | %-10s | %-20s | %s\n", "S.No.", "ID", "Name", "GPA");
    fprintf(fptr, "__________________________________________________________\n");
    if(storage != NULL && *size > 0){
        for (int i = 0; i < *size; ++i){
            fprintf(fptr, "%-10d | %-10d | %-20s | %.2f\n", i + 1, (storage + i)->id, (storage + i)->name, (storage + i)->gpa);
        }
    }
    fclose(fptr);
}

int count_size(void){
    FILE *fptr = fopen("students_details.txt", "r");
    if(fptr == NULL){
        return 0;
    }
    int count = 0;
    char junk[256];
    fgets(junk, sizeof(junk), fptr);
    fgets(junk, sizeof(junk), fptr);
    while(fgets(junk, sizeof(junk), fptr)){
        if(strlen(junk) > 10){
            count++;
        }
    }
    fclose(fptr);
    return count;
}

struct students *create_storage(int *size){
    struct students *storage = (struct students *)malloc(*size * sizeof(struct students));
    if(storage == NULL){
        return NULL;
    }
    for (int i = 0; i < *size; ++i){
        (storage + i)->name = (char *)malloc(50 * sizeof(char));
        if((storage + i)->name == NULL){
            for (int k = 0; k < i; ++k){
                free((storage + k)->name);
            }
            free(storage);
            return NULL;
        }
        (storage + i)->id = 0;
        (storage + i)->name[0] = '\0';
        (storage + i)->gpa = 0.0;
    }
    return storage;
}

struct students *load_data(struct students *storage, int *size){
    FILE *fptr = fopen("students_details.txt", "r");
    if(fptr == NULL){
        return storage;
    }
    char junk[256];
    fgets(junk, sizeof(junk), fptr);
    fgets(junk, sizeof(junk), fptr);
    for (int i = 0; i < *size; ++i){
        int sn;
        fscanf(fptr, "%d | %d | %[^|] | %f\n", &sn, &(storage + i)->id, (storage + i)->name, &(storage + i)->gpa);
    }
    trim_name(storage, size);
    return storage;
}

void trim_name(struct students *storage, int *size){
    for (int i = 0; i < *size; ++i){
        int len = strlen((storage + i)->name);
        while(len > 0 && ((storage + i)->name[len - 1] == ' ' || (storage + i)->name[len - 1] == '\t')){
            (storage + i)->name[len - 1] = '\0';
            len--;
        }
        int start = 0;
        while((storage + i)->name[start] == ' ' || (storage + i)->name[start] == '\t'){
            start++;
        }
        if(start > 0){
            memmove((storage + i)->name, (storage + i)->name + start, strlen((storage + i)->name + start) + 1);
        }
    }
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
        (storage + *size)->name = (char *)malloc(50 * sizeof(char));
        printf("ID:\t");
        scanf("%d", &(storage + *size)->id);
        getchar();
        printf("Name:\t");
        fgets((storage + *size)->name, 50 * sizeof(char), stdin);
        (storage + *size)->name[strcspn((storage + *size)->name, "\n")] = '\0';
        printf("GPA:\t");
        scanf("%f", &(storage + *size)->gpa);
        (*size)++;
        save_to_file(storage, size);
        printf("Wanna add more? [Y/N]:\t");
        scanf(" %c", &choice);
    }
    return storage;
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
    char *search_name;
    search_name = (char *)malloc(50 * sizeof(char));
    printf("Enter the full name of students you wanna %s:\t", status);
    getchar();
    if(!fgets(search_name, 50 * sizeof(char), stdin))
    {
        free(search_name);
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
    free(search_name);
    return found_index;
}

struct students *edit(struct students *storage, int *size, int found_index){
    printf("ID:\t");
    scanf("%d", &(storage + found_index)->id);
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
        free((storage + found_index)->name);
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

void free_memory(struct students *storage, int *size){
    for (int i = 0; i < *size; ++i){
        free((storage + i)->name);
    }
    free(storage);
}