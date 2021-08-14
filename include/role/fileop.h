#if !defined(LIBROLE_FILEOP_H)
#define LIBROLE_FILEOP_H 1

/*
 * \brief Validate single role file name from roles directory.
 *
 * \param[in] filename Validate File name in a roles directory.
 * \return
 *  - LIBROLE_OK: if filename is valid.
 *  - LIBROLE_INVALID_ROLE_FILENAME: if filename is invalid.
 */
int librole_validate_filename_from_dir(const char *filename);

/*
 * \brief Read single role file from specified directory right into
 * role graph node.
 *
 * \param[in] directory Directory to read file from.
 * \param[in] filename File name in a directory to read roles from.
 * \param[in,out] role_graph Role graph to expand with roles.
 * \return
 *  - LIBROLE_OK: Don't try to fail.
 */
int librole_read_file_from_dir(const char * const directory,
    const char * const filename,
    struct librole_graph *role_graph);

/**
 * \brief Read role entries from /etc/role.d catalog.
 *
 * \param[in] directory directory to open.
 * \return
 *  - LIBROLE_INCORRECT_VALUE: Incorrect values passed to function
 *  - LIBROLE_ERROR_OPENING_DIRECTORY: Error occured while trying to
 *    open directory.
 *  - LIBROLE_OK: Operation succeeded.
 *  - errno: Undefined error.
 */
int librole_get_directory_files(const char * const directory, struct librole_graph *role_graph);

/**
 * \brief Read system role entries from /etc/role.d catalog.
 *
 * \param[in] directory directory to open.
 * \return
 *  - LIBROLE_INCORRECT_VALUE: Incorrect values passed to function
 *  - LIBROLE_ERROR_OPENING_DIRECTORY: Error occured while trying to
 *    open directory.
 *  - LIBROLE_OK: Operation succeeded.
 *  - errno: Undefined error.
 */
int librole_get_system_roles(const char * const directory, char *system_roles[]);

#endif

