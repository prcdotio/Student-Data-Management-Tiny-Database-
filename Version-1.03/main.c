#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "students.h"

int main(void){
    // set_theme();
    if (!(initialize_database()))
    {
        return 1;
    }
    int total_students = count_size();
    struct students *storage = create_storage(&total_students);
    if(total_students > 0){
        load_data(storage, &total_students);
    }
    int choice;
    while(1){
        clear_screen();
        main_menu();
        if (!scanf("%d", &choice))
        {
            while(getchar() != '\n')
                ;
            continue;
        }
        if(choice == 7){
            // reset_theme();
            break;
        }
        switch(choice){
            case 1:
                print_data(storage, &total_students);
                pause_ui();
                break;
                case 2:
                    storage = add_new_students(storage, &total_students);
                    pause_ui();
                    break;
                    case 3:
                        storage = update(storage, &total_students);
                        pause_ui();
                        break;
                        case 4:
                            storage = delete(storage, &total_students);
                            pause_ui();
                            break;
                            case 5:
                                sort_database(storage, &total_students);
                                pause_ui();
                                break;
                                case 6:
                                    search(storage, &total_students);
                                    pause_ui();
                                    break;
                                default:
                                    printf("Error: Invalid input.\n");
                                    pause_ui();
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