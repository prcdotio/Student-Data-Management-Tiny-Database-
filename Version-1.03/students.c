#include "students.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// void set_theme(){
//     printf("\033[100m\033[H\033[3J");
// }

// void reset_theme(){
//     printf("\033[0m\033[H\033[3J");
// }

void main_menu(){
        printf("==================STUDENT'S DATABASE==================\n");
        printf("=======================ACTIONS========================\n");
        printf("1. View all students\n");
        printf("2. Add new students\n");
        printf("3. Update database\n");
        printf("4. Delete data\n");
        printf("5. Sort\n");
        printf("6. Search students\n");
        printf("7. Exit\n");
        printf("======================================================\n");
}

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
            print_error("Binary file creation unsuccessful. Please check for permissions.");
            return 0;
        }
        print_success("Binary file creation success.");
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
        print_error("Error: Invalid input.");
        return storage;
    }
    while(choice == 'y' || choice == 'Y'){
        struct students *temp = realloc(storage, (*size + 1) * sizeof(struct students));
        if(temp == NULL){
            print_error("Error: Unable to add new students.");
            return storage;
        }
        storage = temp;
        int new_id;
        int valid = 0;
do {
    printf("ID:\t");
    if (scanf("%d", &new_id) != 1) {
        print_error("Error: Please enter a numeric ID.");
        while (getchar() != '\n');
        continue; 
    }
    if (!is_id_valid(new_id)) {
        continue;
    }
    if (is_id_exist(storage, size, new_id, -1)) {
        print_error("Error: Duplicate ID. This ID is already taken.");
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
                print_error("Error: Invalid input.");
                while (getchar() != '\n')
                    ;
                continue;
            }
            if(!is_gpa_valid((storage + *size)->gpa)){
                print_error("Error: Invalid GPA. GPA must me 0.0 to 4.0");
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
        print_error("Error: ID must be positive integer.");
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
        clear_screen();
        main_menu();
        if (search_option == 3)
        {
            print_yellow_msg("Update cancelled!");
            break;
        }
        if(search_option != 1 && search_option != 2){
            print_error("Error: out of criteria.");
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
                    print_error("Error: Invalid input.");
                    break;
                }
                printf(BLUE "Wanna update more? [Y/N]:" RESET "\t");
                scanf(" %c", &choice);
    }
    return storage;
}

int search_by_id(struct students *storage, int *size, char status[]){
    int search_id;
    printf("Enter the ID you wanna %s:\t", status);
    while(scanf("%d", &search_id) != 1){
        print_error("Error: Invalid input.");
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
        printf(RED "Student with ID: %d not found."RESET"\n", search_id);
    }
    return found_index;
}

int search_by_name(struct students *storage, int *size, char status[]){
    clear_screen();
    main_menu();
    char search_name[50];
    char lower_search_name[50];
    int *match_indices = (int *)malloc(*size * sizeof(int));
    int match_count = 0;
    printf("Enter the full or partial name of students you wanna %s:\t", status);
    getchar();
    if(!fgets(search_name, 50 * sizeof(char), stdin))
    {
        return -1;
    }
    search_name[strcspn(search_name, "\n")] = '\0';
    make_lowercase(lower_search_name, search_name);
    for (int i = 0; i < *size; ++i)
    {
        char lower_searching[50];
        make_lowercase(lower_searching, (storage + i)->name);
        if (strstr(lower_searching, lower_search_name) != NULL)
        {
            match_indices[match_count] = i;
            match_count++;
        }
    }
        if(match_count == 0){
            printf(YELLOW"No result for %s."RESET"\n", search_name);
            return -1;
        }
        printf(YELLOW "--------Search Result for '%s' -------"RESET"\n", search_name);
        printf(GREEN "%-10s | %-10s | %-20s | %s"RESET"\n", "S.No.", "ID", "Name", "GPA");
        printf(YELLOW"_____________________________________________________"RESET"\n");
        for (int i = 0; i < match_count; ++i){
            int idx = match_indices[i];
            printf("%-10d | %-10d | %-20s | %.2f\n", i + 1, (storage + idx)->id, (storage + idx)->name, (storage + idx)->gpa);
        }
        if(match_count == 1){
            printf(GREEN "Only one student found named '%s'. Proceeding..."RESET"\n", search_name);
            return match_indices[0];
        }
        int selected_id;
        printf(BLUE"Multiple matches. Please select ID to proceed %s."RESET"\n", status);
        while(scanf("%d", &selected_id) != 1){
            print_error("Please, enter valid ID.");
            while(getchar() != '\n')
                ;
        }
        for (int i = 0; i < match_count; ++i){
            int idx = match_indices[i];
            if((storage + idx)->id == selected_id){
                free(match_indices);
                return idx;
            }
        }
        printf(RED"Error: The ID %d was not found on search list."RESET"\n", selected_id);
        return -1;
}

void make_lowercase(char *dest, const char *src){
    for (int i = 0; src[i]; ++i){
        dest[i] = tolower((unsigned char)src[i]);
    }
    dest[strlen(src)] = '\0';
}

struct students *edit(struct students *storage, int *size, int found_index){
    int original_id = (storage + found_index)->id;
   int new_id;
        int valid = 0;
do {
    printf("ID:\t");
    if (scanf("%d", &new_id) != 1) {
        print_error("Error: Please enter a numeric ID.");
        while (getchar() != '\n');
        continue; 
    }
    if (!is_id_valid(new_id)) {
        continue;
    }
    if (is_id_exist(storage, size, new_id, original_id)) {
        print_error("Error: Duplicate ID. This ID is already taken.");
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
    printf(BLUE"Are you sure to edit those file? "RESET""WHITE"[Y/N]"RESET"\t");
    if(scanf(" %c", &choice)){
        if(choice == 'y' || choice == 'Y'){
            save_to_file(storage, size);
            clear_screen();
            main_menu();
            print_success("Database updated successfully.");
        }
        else
        {
            load_data(storage, size);
            clear_screen();
            main_menu();
            print_yellow_msg("Database update cancelled.");
        }
        }
        return storage;
}

struct students *delete(struct students *storage, int *size){
    char choice = 'y';
    while(choice == 'y' || choice == 'Y'){
        clear_screen();
        main_menu();
        int search_option;
        printf("1. ID:\n");
        printf("2. Name:\n");
        printf("3. Cancel\n");
        printf("By which data you wanna delete? [1/2]:\t");
        if(!scanf("%d", &search_option)){
            print_error("Error: Invalid input.");
            while(getchar() != '\n')
                ;
            continue;
        }
        if(search_option == 3){
            print_yellow_msg("Deletion cancelled!");
            break;
        }
        if(search_option != 1 && search_option != 2){
            print_error("Error: out of criteria.");
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
                        print_error("Error: Invalid input.");
                    }
                    if(*size > 0){
                        printf("Wanna delete more? [Y/N]");
                        if(!scanf(" %c", &choice)){
                            print_error("Invalid input.");
                            return storage;
                        }
                    }else{
                        printf(YELLOW"Database is empty."RESET" "GREEN"Now, returning to main menu."RESET"\n");
                        break;
                    }
    }
    return storage;
}

struct students *remove_data(struct students *storage, int *size, int found_index){
    char choice;
    printf(YELLOW"Are you sure to delete?"RESET" "WHITE"[Y/N]:"RESET"\t");
    if(!scanf(" %c", &choice)){
        print_error("Error: Invalid input.");
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
        print_yellow_msg("Not enough data to sort.");
        return;
    }
    int category;
    printf("====================\n");
    printf("1. By ID\n");
    printf("2. By Name\n");
    printf("3. By GPA\n");
    printf("=====================\n");
    if(!scanf("%d", &category) || category < 1 || category > 3){
        print_error("Error: Invalid input.");
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
        print_error("Error: Invalid input.");
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
    printf("Do you wanna sort to database also? "WHITE"[Y/N]"RESET"\t");
    if(!scanf(" %c", &choice)){
        print_error("Error: Invalid input.");
        return;
    }
    if(choice == 'Y' || choice == 'y'){
        save_to_file(storage, size);
        print_success("Database also sorted (Highest to lowest) and saved.\n");
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

void clear_screen(){
    // set_theme();
    printf("\033[H\033[2J");
}

void pause_ui(){
    printf("\n\n....Press [Enter] to continue....\n");
    int c;
    while((c = getchar()) != '\n' && c != EOF)
        ;
    getchar();
}

void print_error(char *msg){
    printf(RED "%s" RESET "\n", msg);
}

void print_success(char *msg){
    printf(GREEN "%s" RESET "\n", msg);
}

void print_yellow_msg(char *msg){
    printf(YELLOW "%s" RESET "\n", msg);
}

void search(struct students *storage, int *size){
    char choice = 'y';
    while(choice == 'y' || choice == 'Y'){
        clear_screen();
        main_menu();
        printf("=============SEARCH DASHBOARD============\n");
        printf("1. ID\n");
        printf("2. Name\n");
        printf("3. GPA\n");
        printf("4. Cancel\n");
        printf("==========================================\n");
        int choose;
        printf("Enter by which info you wanna search:\t");
    while(!scanf("%d", &choose));
    if(choose == 4){
        break;
    }
    switch(choose){
        case 1:
            search_by_id_advanced(storage, size);
            break;
            case 2:
                search_by_name_advanced(storage, size);
                break;
                case 3:
                    search_by_gpa_advanced(storage, size);
                    break;
                    default:
                        print_error("Invalid input.");
                        break;
                    }
                    printf(YELLOW "Do you wanna search other" RESET " " WHITE "[Y/N]:" RESET "\t");
                    scanf(" %c", &choice);
    }
}

void search_by_name_advanced(struct students *storage, int *size){
    char search_name[50];
    char lower_search[50];
    int *match_indices = (int *)malloc(*size * sizeof(int));
    int match_count = 0;

    printf("\nEnter the name you wanna Search:\t");
    (void)getchar();
    if(!fgets(search_name, 50 * sizeof(char), stdin)){
        free(match_indices);
    }
    search_name[strcspn(search_name, "\n")] = '\0';
    make_lowercase(lower_search, search_name);

    for (int i = 0; i < *size; ++i){
        char current_lower[50];
        make_lowercase(current_lower, (storage + i)->name);
        if(strcmp(current_lower, lower_search) == 0){
            match_indices[match_count++] = i;
        }
    }
    for (int i = 0; i < *size; ++i){
        char current_lower[50];
        make_lowercase(current_lower, (storage + i)->name);
        if(strstr(current_lower, lower_search) == current_lower && strcmp(current_lower, lower_search) != 0){
            match_indices[match_count++] = i;
        }
    }
    for (int i = 0; i < *size; ++i){
        char current_lower[50];
        make_lowercase(current_lower, (storage + i)->name);
        if(strstr(current_lower, lower_search) != NULL && strstr(current_lower, lower_search) != current_lower){
            match_indices[match_count++] = i;
        }
    }
    if(match_count == 0){
        printf(RED "No rusult for %s." RESET "\n", search_name);
        free(match_indices);
        return;
    }
    printf(YELLOW"\n---------SEARCH RESULT----------""\n");
    printf("%-10s | %-10s | %-20s | %s\n", "S.No.", "ID", "Name", "GPA");
    printf("_____________________________________________"RESET"\n");
    for (int i = 0; i < match_count; ++i)
    {
        int idx = match_indices[i];
        printf(GREEN"%-10d | %-10d | %-20s | %.2f"RESET"\n", i + 1, (storage + idx)->id, (storage + idx)->name, (storage + idx)->gpa);
    }
    free(match_indices);
}

void search_by_id_advanced(struct students *storage, int *size){
    char search_term[10];
    int match = 0;
    int *match_indices = (int *)malloc(*size * sizeof(int));
    printf("Enter ID or part of ID to search:\t");
    scanf("%s", search_term);
    while(getchar() != '\n')
        ;
    for (int i = 0; i < *size; ++i){
        char current_id_str[15];
        sprintf(current_id_str, "%d", (storage + i)->id);
        if(strcmp(current_id_str, search_term) == 0){
            match_indices[match++] = i;
        }
    }
    for (int i = 0; i < *size; ++i){
        char current_id_str[15];
        sprintf(current_id_str, "%d", (storage + i)->id);
        if(strstr(current_id_str, search_term) == current_id_str && strcmp(current_id_str, search_term) != 0){
            match_indices[match++] = i;
        }
    }
    for (int i = 0; i < *size; ++i){
        char current_id_str[15];
        sprintf(current_id_str, "%d", (storage + i)->id);
        if(strstr(current_id_str, search_term) != NULL && strstr(current_id_str, search_term) != current_id_str){
            match_indices[match++] = i;
        }
    }
    if(match == 0){
        printf(RED "No ID matches for %s" RESET "\n", search_term);
        return;
    }else{
        printf(YELLOW"-------SEARCH RESULT-------\n");
        printf("%-10s | %-10s | %-20s | %s\n", "S.No.", "ID", "Name", "GPA");
        printf("_____________________________________"RESET"\n");
        for (int i = 0; i < match; ++i){
            int idx = match_indices[i];
            printf(GREEN "%-10d | %-10d | %-20s | %.2f" RESET "\n", i + 1, (storage + idx)->id, (storage + idx)->name, (storage + idx)->gpa);
        }
    }
    free(match_indices);
}

void search_by_gpa_advanced(struct students *storage, int *size){
    int gpa_choice;
    printf("---------GPA SEARCH MODE----------\n");
    printf("1. Range Search (Min to Max)\n");
    printf("2. Proximity Search (Target or Similarity)\n");
    printf("3. Return to Search Menue\n");
    printf("Enter the corresponding number to proceed:\t");
    if(scanf("%d", &gpa_choice) != 1){
        while(getchar() != '\n')
            ;
        return;
    }
    switch(gpa_choice){
        case 1:
            search_by_gpa_advanced_range(storage, size);
            break;
            case 2:
                search_by_gpa_advanced_proximity(storage, size);
                break;
                default:
                    return;
                }
}

void search_by_gpa_advanced_range(struct students *storage, int *size){
    float min_gpa, max_gpa;
    int match_count = 0;
    int *match_indices = (int *)malloc(*size * sizeof(int));
    printf("Enter the Minimum GPA:\t");
    if(scanf("%f", &min_gpa) != 1){
        free(match_indices);
        while(getchar() != '\n')
            ;
        return;
    }
    printf("Enter the Maximum GPA:\t");
   if(scanf("%f", &max_gpa) != 1){
       free(match_indices);
       while(getchar() != '\n')
           ;
       return;
   }
   if(min_gpa < 0.0 || max_gpa > 4.0){
       print_error("Invalid range. Range must be [0.0 - 4.0]");
       return;
   }
    if(min_gpa > max_gpa){
        float temp = max_gpa;
        max_gpa = min_gpa;
        min_gpa = temp;
        print_yellow_msg("Note: Minimum and Maximum is swapped to make data range valid.");
    }
    for (int i = 0; i < *size; ++i){
        if((storage + i)->gpa >= min_gpa && (storage + i)->gpa <= max_gpa){
            match_indices[match_count++] = i;
        }
    }
    if(match_count == 0){
        print_error("No match found in that range.");
        return;
    }
    printf("---------RESULT OF RANGE [%f - %f]-------\n", min_gpa, max_gpa);
    printf("%-10s | %-10s | %-20s | %s\n", "S.No.", "ID", "Name", "GPA");
    for (int i = 0; i < match_count; ++i){
        int idx = match_indices[i];
        printf("%-10d | %-10d | %-20s | %.2f\n", i + 1, (storage + idx)->id, (storage + idx)->name, (storage + idx)->gpa);
    }
    free(match_indices);
}

void search_by_gpa_advanced_proximity(struct students *storage, int *size){
    float target;
    int match_count = 0;
    int *match_indices = (int *)malloc(*size * sizeof(int));
    printf("Enter target gpa to find exact or similar performer:\t");
    if(scanf("%f", &target) != 1){
        free(match_indices);
        while(getchar() != '\n')
            ;
        return;
    }
    for (int i = 0; i < *size; ++i){
        if(fabs((storage + i)->gpa - target) < 0.001){
            match_indices[match_count++] = i;
        }
    }
    for (int i = 0; i < *size; ++i){
        float diff = fabs((storage + i)->gpa - target);
        if(diff > 0.001 && diff <= 0.1){
            match_indices[match_count++] = i;
        }
    }
    for (int i = 0; i < *size; ++i){
        float diff = fabs((storage + i)->gpa - target);
        if(diff > 0.1 && diff <= 0.5){
            match_indices[match_count++] = i;
        }
    }
    if(match_count == 0){
        print_error("No match found.");
    }else{
        printf("--------SEARCH RESULT [%.2f]--------\n", target);
        printf("%-10s | %-10s | %-20s | %s\n", "S.No.", "ID", "Name", "GPA");
        for (int i = 0; i < match_count; ++i){
            int idx = match_indices[i];
            printf("%-10d | %-10d | %-20s | %.2f\n", i + 1, (storage + idx)->id, (storage + idx)->name, (storage + idx)->gpa);
        }
    }
    free(match_indices);
}

void free_memory(struct students *storage){
    free(storage);
}