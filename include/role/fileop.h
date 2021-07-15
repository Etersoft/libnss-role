#if !defined(LIBROLE_FILEOP_H)
#define LIBROLE_FILEOP_H 1

int librole_validate_filename_from_dir(const char *filename);
int librole_read_file_from_dir(const char * const directory,
    const char * const filename,
    struct librole_graph *role_graph);
int librole_get_directory_files(const char * const directory, struct librole_graph *role_graph);
int librole_get_system_roles(const char * const directory, char *system_roles[]);

typedef int (*librole_roles_filter)(const char *rolename);

int librole_writing(const char *, struct librole_graph *, int numeric_flag, int empty_flag, librole_roles_filter filter);
int librole_write(const char* pam_role, struct librole_graph *G, int empty_flag);
int librole_write_dir(const char* filename, const char* pam_role, struct librole_graph *G, int empty_flag);

#endif

