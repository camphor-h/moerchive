#include "moerchive.h"
const char mar_signature[SIGNATURE_LENGTH] = {0x32, 0x12, 0xE2, 0xD7};
BOOL read_signature(FILE* target);
char what_slash(char* str);
int read_header(FILE* target);
void unpack(FILE* target, char* output_dir, int amount, BOOL onlyinfo);
int main(int argc, char* argv[])
{
    BOOL onlyinfo = FALSE;
    if (argc < 2)
    {
        printf("Error: Please input enough parameters.\n");
        printf("Usage: unmar [FILE_NAME] -- unarchive to current directory\n");
        printf("       unmar [FILE_NAME] [TARGET_DIRECTORY] -- unarchive to target directory\n");
        printf("       unmar [FILE_NAME] -i -- only check archived files' infomation\n");
        return 0;
    }
    else
    {
        char* mar_file_ptr = NULL;
        char* output_dir_ptr = NULL;
        FILE* target_file;
        DIR* output_directory = NULL;
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "-i") == 0) //info only mode?
            {
                onlyinfo = TRUE;
            }
            else
            {
                if (!mar_file_ptr) //get .mar file first
                {
                    target_file = fopen(argv[i], "rb");
                    if (target_file != NULL)
                    {
                        mar_file_ptr = argv[i];
                    }
                }
                else if (!output_dir_ptr) //then get target output directory
                {
                    output_directory = opendir(argv[i]);
                    if (!output_directory)
                    {
                        #ifdef _WIN32
                        if (mkdir(argv[i]) != 0)
                        #else
                        if (mkdir(argv[i], 0755) != 0)
                        #endif
                        {
                            printf("Error: Cannot create directory.\n");
                            if (mar_file_ptr)
                            {
                                fclose(target_file);
                            }
                            return 0;
                        }
                    }
                    output_dir_ptr = argv[i];
                }
            }
        }
        if (!mar_file_ptr)
        {
            printf("Error: Cannot find file.\n");
            return 0;
        }
        if (!output_dir_ptr)
        {
            output_dir_ptr = strdup("./");
        }
        if (!read_signature(target_file))
        {
            printf("Error: Not a valid .mar file.\n");
            fclose(target_file);
            return 0;
        }
        int files_amount = read_header(target_file);
        unpack(target_file, output_dir_ptr, files_amount, onlyinfo);
        fclose(target_file);
        if (output_directory != NULL)
        {
            closedir(output_directory);
        }
    }
    return 0;
}
BOOL read_signature(FILE* target)
{
    char signature[SIGNATURE_LENGTH];
    fread(signature, sizeof(char), SIGNATURE_LENGTH, target);
    return (memcmp(signature, mar_signature, SIGNATURE_LENGTH) == 0 ? TRUE : FALSE);
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
/* old-fashioned, related function has been integrated in unpack()
void get_all_file_name(FILE* target, int amount)
{
    char* file_name;
    for (int i = 0; i < amount; i++)
    {
        uint8_t file_name_length;
        fread(&file_name_length, sizeof(uint8_t), 1, target);
        file_name = malloc(file_name_length + 1);
        fread(file_name, sizeof(char), file_name_length + 1, target);
        long file_size;
        fread(&file_size, sizeof(long), 1, target); 
        printf("File %d: %s\n    Size: %ld Bytes\n", i + 1, file_name, file_size);
        free(file_name);
        //then skip the data
        fseek(target, file_size, SEEK_CUR);
    }
}
*/    
void unpack(FILE* target, char* output_dir, int amount, BOOL onlyinfo)
{
    char* file_name;
    FILE* single_file;
    char buffer[DEFAULT_BUFFER_SIZE];
    for (int i = 0; i < amount; i++)
    {
        uint8_t file_name_length;
        fread(&file_name_length, sizeof(uint8_t), 1, target);
        file_name = malloc(file_name_length + 1);
        fread(file_name, sizeof(char), file_name_length + 1, target);
        int64_t file_size;
        fread(&file_size, sizeof(int64_t), 1, target); 
        printf("File %d: %s\n    Size: %" PRId64 " Bytes\n", i + 1, file_name, file_size);
        if (onlyinfo == TRUE)
        {
            fseek(target, file_size, SEEK_CUR);
        }
        else
        {
            size_t path_len = strlen(output_dir) + strlen(file_name) + 2; // +2 for '\0' and slash
            char* full_path = malloc(path_len); 
            snprintf(full_path, path_len, "%s%c%s", output_dir, what_slash(output_dir), file_name);
            single_file = fopen(full_path, "wb");
            if (!single_file)
            {
                printf("Error: Cannot unpack %s\n", file_name);
                free(file_name);
                continue;
            }
            int64_t remain_size = file_size;
            while (remain_size > 0)
            {
                int64_t plan_read_size = (remain_size < DEFAULT_BUFFER_SIZE ? remain_size : DEFAULT_BUFFER_SIZE);
                int64_t real_read_size = fread(buffer, 1, plan_read_size, target);
                if (real_read_size == 0)
                {
                    break;
                }
                fwrite(buffer, 1, real_read_size, single_file);
                remain_size -= real_read_size;
            }
            free(full_path);
            fclose(single_file);
        }
        free(file_name);
    }
}
int read_header(FILE* target)
{
    int file_amount;
    fread(&file_amount, sizeof(int), 1, target);
    return file_amount;
}