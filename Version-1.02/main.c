#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "students.h"

int main(void){
    if(!(initialize_database())){
        return 1;
    }
    int total_students = count_size();
    struct students *storage = create_storage(&total_students);
    if(total_students > 0){
        load_data(storage, &total_students);
    }
    int choice;
    while(1){
        printf("==================STUDENT'S DATABASE==================\n");
        printf("=======================ACTIONS========================\n");
        printf("1. View all students\n");
        printf("2. Add new students\n");
        printf("3. Update database\n");
        printf("4. Delete data\n");
        printf("5. Exit\n");
        printf("======================================================\n");
        if(!scanf("%d", &choice)){
            while(getchar() != '\n')
                ;
            continue;
        }
        if(choice == 5){
            break;
        }
        switch(choice){
            case 1:
                print_data(storage, &total_students);
                break;
                case 2:
                    storage = add_new_students(storage, &total_students);
                    break;
                    case 3:
                        storage = update(storage, &total_students);
                        break;
                        case 4:
                            storage = delete(storage, &total_students);
                            break;
                            default:
                                printf("Error: Invalid input.\n");
                                break;
                            }
    }
    if(storage != NULL){
        free_memory(storage);
        storage = NULL;
    }
    printf("Database closed. Have a good day!\n");
    return 0;
}