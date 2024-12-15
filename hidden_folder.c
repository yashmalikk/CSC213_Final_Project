#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Function to create a hidden folder in the user's home directory
int create_hidden_folder(const char *folder_name) {
  // Get the home directory path from the environment variable
  const char *home_dir = getenv("HOME");

  // If HOME environment variable is not set, return an error
  if (home_dir == NULL) {
    fprintf(stderr, "Error: HOME environment variable is not set.\n");
    return 1;  // Return error code
  }

  // Create the full path for the hidden folder
  char folder_path[1024];
  snprintf(folder_path, sizeof(folder_path), "%s/.%s", home_dir, folder_name);

  // Check if the directory already exists
  struct stat st = {0};
  if (stat(folder_path, &st) == -1) {
    // Directory does not exist, so create it
    if (mkdir(folder_path, 0700) == -1) {
      perror("Error creating the hidden folder");
      return 1;  // Return error code if mkdir fails
    } else {
        printf("Hidden folder created successfully: %s\n", folder_path);
    }
  }
  return 0;  // Return success code
}