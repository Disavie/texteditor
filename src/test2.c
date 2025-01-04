#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Helper to check if the current sequence is an ANSI escape code
bool is_ansi_code(const char *str, size_t *length) {
    if (str[0] == '\033' && str[1] == '[') {
        *length = 2; // Start counting after "\033["
        while (str[*length] && (str[*length] < '@' || str[*length] > '~')) {
            (*length)++;
        }
        if (str[*length]) {
            (*length)++; // Include the final character of the escape code
            return true;
        }
    }
    return false;
}

char *extract_ansi_preserving_substring(const char *s2, size_t start, size_t end) {
    size_t logical_index = 0;  // Logical position (ignoring ANSI codes)
    size_t i = 0;             // Iterator for s2
    size_t ansi_length = 0;   // Length of ANSI code if found
    size_t buffer_size = strlen(s2) + 1;
    char *result = malloc(buffer_size);
    size_t result_index = 0;

    if (!result) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    while (s2[i] && logical_index <= end) {
        if (is_ansi_code(&s2[i], &ansi_length)) {
            // Copy the ANSI code to result
            strncpy(&result[result_index], &s2[i], ansi_length);
            result_index += ansi_length;
            i += ansi_length; // Skip the ANSI code
        } else {
            if (logical_index >= start && logical_index <= end) {
                result[result_index++] = s2[i];
            }
            i++;
            logical_index++;
        }
    }

    result[result_index] = '\0'; // Null-terminate the resulting string
    return result;
}

int main() {
    const char *s = "int main burger cheeese return 5;";
    const char *s2 = "\033[38;5;39mint\033[38;5;255m main burger cheeese return 5;";

    // Extract logical substring equivalent to s[1] - s[5]
    char *substring = extract_ansi_preserving_substring(s2, 2,5);

    if (substring) {
        printf("Extracted substring: %s\n", substring);
        free(substring); // Free the allocated memory
    }

    return 0;
}

