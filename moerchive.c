#include "moerchive.h"
void archive_file(FILE* dest, char* file_address);
uint8_t get_file_name_start_index(char* file_address); //return the index of filename's beginning
char* get_full_path(char* file_directory, char* file_name, char* dest, int dest_size);
static inline void write_signature(FILE* target_file)
{
    fwrite(mar_signature, sizeof(char), SIGNATURE_LENGTH, target_file);
}
static inline void write_header(FILE* target_file, int file_amount)
{
    fwrite(&file_amount, sizeof(int), 1, target_file);
}
int64_t get_file_size(FILE* file);
char what_slash(char* str);
void write_file(FILE* dest, FILE* source);
const char mar_signature[SIGNATURE_LENGTH] = {0x32, 0x12, 0xE2, 0xD7};
int main(int argc, char* argv[])
{
    char* directory = NULL;
    char* full_path = NULL;
    DIR* input_dir = NULL;
    struct dirent* dir_info = NULL;
    int file_count = 0;
    if (argc < 3)
    {
        printf("Error: Please input enough parameters.\n");
        printf("Usage: mar [FILE_NAME] [FILE_NAME] ... [TARGET_PATH]\n");
        printf("       mar -a [SOURCE_PATH] [TARGET_PATH]\n");
        printf("-a archive all files in [SOURCE_PATH]\n");
        return 0;
    }
    if (strcmp(argv[1], "-a") == 0) //all file in a target directory
    {
        input_dir = opendir(argv[2]);
        if (!input_dir)
        {
            printf("Error: Cannot find directory.\n");
            return 0;
        }
        else
        {
            directory = argv[2];
        }
    }
    FILE* mar_file = fopen(argv[argc - 1], "wb"); //using last parameter as file name
    if (!mar_file)
    {
        printf("Error: Cannot create file.\n");
        return 0;
    }
    if (directory) // -a mode
    {
        while ((dir_info = readdir(input_dir)) != NULL)
        {
            if (strcmp(dir_info->d_name, ".") != 0 && strcmp(dir_info->d_name, "..") != 0)
            {
                file_count++;
            }
        }
        rewinddir(input_dir);
    }
    else
    {
        file_count = argc - 2;
    }
    write_signature(mar_file);
    write_header(mar_file, file_count);
    if (!directory) //not -a mode
    {
        for (int i = 1; i < argc - 1; i++)
        {
            archive_file(mar_file, argv[i]);
        }
    }
    else //-a mode
    {
        
        while ((dir_info = readdir(input_dir)) != NULL)
        {
            int size = strlen(directory) + strlen(dir_info->d_name) + 2; // +2 for '\0' and slash (if it have);
            full_path = malloc(size);
            if (strcmp(dir_info->d_name, ".") != 0 && strcmp(dir_info->d_name, "..") != 0)
            {
                archive_file(mar_file, get_full_path(directory, dir_info->d_name, full_path, size));
            }
            free(full_path);
        }
    }
    fclose(mar_file);
    if (input_dir)
    {
        closedir(input_dir);
    }
    return 0;
}
void archive_file(FILE* dest, char* file_address)
{
    FILE* source = fopen(file_address, "rb");
    if (!source)
    {
        printf("Error: Cannot find file: %s\n", file_address);
        return;
    }
    uint8_t name_start_index = get_file_name_start_index(file_address);
    char* file_name = file_address + name_start_index;
    uint8_t file_name_length = strlen(file_name);
    fwrite(&file_name_length, sizeof(uint8_t), 1, dest); //write file name length
    fwrite(file_name, 1, file_name_length + 1, dest); //write file name
    int64_t file_size = get_file_size(source);
    fwrite(&file_size, sizeof(int64_t), 1, dest); //write file size
    write_file(dest, source);
    fclose(source);
}
char* get_full_path(char* file_directory, char* file_name, char* dest, int dest_size)
{
    BOOL dir_last_is_slash = FALSE;
    int dir_length = strlen(file_directory);
    if (file_directory[dir_length - 1] == '\\' || file_directory[dir_length - 1] == '/')
    {
        dir_last_is_slash = TRUE;
    }
    if (dir_last_is_slash)
    {
        sprintf(dest, "%s%s", file_directory, file_name);
    }
    else
    {
        sprintf(dest, "%s%c%s", file_directory, what_slash(file_directory), file_name);
    }
    return dest;
}
uint8_t get_file_name_start_index(char* file_address)
{
    char* last_left_slash = strrchr(file_address, '\\');
    char* last_right_slash = strrchr(file_address, '/');
    char* last_separator = NULL;
    if (last_left_slash && last_right_slash)
    {
        last_separator = ((last_left_slash > last_right_slash) ? last_left_slash : last_right_slash);
    }
    else if (last_left_slash)
    {
        last_separator = last_left_slash;
    }
    else if (last_right_slash)
    {
        last_separator = last_right_slash;
    }
    if (last_separator)
    {
        return (uint8_t)(last_separator - file_address + 1);
    }
    else
    {
        return 0;
    }
}
char what_slash(char* str)
{
    if (strchr(str, '/') != NULL)
    {
        return '/';
    }
    else if (strchr(str, '\\') != NULL)
    {
        return '\\';
    }
    else
    {
        #ifdef _WIN32
        return '\\';
        #else
        return '/';
        #endif
    }
}
int64_t get_file_size(FILE* file)
{
    int64_t cur_file_pos = ftell(file);
    fseek(file, 0, SEEK_END); //move to EOF
    int64_t file_size = ftell(file);
    fseek(file, cur_file_pos, SEEK_SET); //reset
    return (int64_t)file_size;
}
void write_file(FILE* dest, FILE* source)
{
    static char buffer[DEFAULT_BUFFER_SIZE];
    size_t count;
    do
    {
        count = fread(buffer, 1, DEFAULT_BUFFER_SIZE, source);
        fwrite(buffer, 1, count, dest);
    }while(count == DEFAULT_BUFFER_SIZE && !feof(source));
}
