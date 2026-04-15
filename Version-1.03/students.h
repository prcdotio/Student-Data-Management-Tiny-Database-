#ifndef STUDENTS_H
#define STUDENTS_H
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define WHITE "\033[1;37"
#define RESET "\033[0m"

struct students{
    int id;
    float gpa;
    char name[50];
};

void main_menu();
int initialize_database(void);
int count_size(void);
struct students *create_storage(int *size);
struct students *load_data(struct students *storage, int *size);
void print_data(struct students *storage, int *size);
void save_to_file(struct students *storage, int *size);
struct students *add_new_students(struct students *storage, int *size);
struct students *update(struct students *storage, int *size);
struct students *delete (struct students *storage, int *size);
void free_memory(struct students *storage);
int search_by_id(struct students *storage, int *size, char status[]);
int search_by_name(struct students *storage, int *size, char status[]);
struct students *edit(struct students *storage, int *size, int found_index);
struct students *remove_data(struct students *storage, int *size, int found_index);
int is_id_exist(struct students *storage, int *size, int new_id, int original_id);
int is_id_valid(int id);
int is_gpa_valid(int current_gpa);
void sort_database(struct students *storage, int *size);
int compare_gpa_asc(const void *a, const void *b);
int compare_gpa_desc(const void *a, const void *b);
int compare_id_asc(const void *a, const void *b);
int compare_id_desc(const void *a, const void *b);
int compare_name_asc(const void *a, const void *b);
int compare_name_desc(const void *a, const void *b);
void make_lowercase(char *dest, const char *src);
void clear_screen();
void pause_ui();
void print_error(char *msg);
void print_success(char *msg);
void print_yellow_msg(char *msg);
// void set_theme();
// void reset_theme();
void search(struct students *storage, int *size);
void search_by_name_advanced(struct students *storage, int *size);
void search_by_id_advanced(struct students *storage, int *size);
void search_by_gpa_advanced(struct students *storage, int *size);
void search_by_gpa_advanced_range(struct students *storage, int *size);
void search_by_gpa_advanced_proximity(struct students *storage, int *size);

#endif