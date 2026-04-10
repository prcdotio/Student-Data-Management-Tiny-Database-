#ifndef STUDENTS_H
#define STUDENTS_H

struct students{
    int id;
    float gpa;
    char *name;
};

int initialize_database(void);
int count_size(void);
struct students *create_storage(int *size);
struct students *load_data(struct students *storage, int *size);
void trim_name(struct students *storage, int *size);
void print_data(struct students *storage, int *size);
void save_to_file(struct students *storage, int *size);
struct students *add_new_students(struct students *storage, int *size);
struct students *update(struct students *storage, int *size);
struct students *delete (struct students *storage, int *size);
void free_memory(struct students *storage, int *size);
int search_by_id(struct students *storage, int *size, char status[]);
int search_by_name(struct students *storage, int *size, char status[]);
struct students *edit(struct students *storage, int *size, int found_index);
struct students *remove_data(struct students *storage, int *size, int found_index);

#endif