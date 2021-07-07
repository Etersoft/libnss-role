#if !defined(LIBROLE_FILEOP_H)
#define LIBROLE_FILEOP_H 1

int librole_validate_filename_from_dir(const char *filename);
int librole_read_file_from_dir(const char const *directory,
    const char const *filename,
    struct librole_graph *role_graph);
int librole_get_directory_files(const char const *directory, struct librole_graph *role_graph);

#endif

