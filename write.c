#include <stdio.h>
#include <stdlib.h>

// Function to write an integer to a file
int write_integer_to_file(const char *file_path, int number) {
  // Open the file in write mode ("w")
  FILE *file = fopen(file_path, "w");
    
  // Check if the file was opened successfully
  if (file == NULL) {
    perror("Error opening file");
    return 1;  // Return error code if file opening fails
  }
  
  // Write the integer to the file
  fprintf(file, "%d\n", number);
  
  // Close the file
  fclose(file);

  printf("Integer %d has been written to the file: %s\n", number, file_path);
  return 0;  // Return success code
}

int main() {
  // Example usage: write the number 42 to "example.txt"
  const char *file_path = "example.txt";
  int number = 42;
  
  // Call the function to write the integer to the file
  return write_integer_to_file(file_path, number);
}
