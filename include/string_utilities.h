/*
This code is taken from 21st Century C ad verbatin. The license for the code allows it.
However please pay homage wherenever possible if you use thewse.

Tools for string and a secure-printf
*/

#define Sasprintf(write_to,  ...) {          \
    char *tmp_string_for_extend = write_to;  \
    asprintf(&(write_to), __VA_ARGS__);      \
    free(tmp_string_for_extend);             \
}

char *string_from_file(char const *filename);

typedef struct ok_array {
    char **elements;
    char *base_string;
    int length;
} ok_array;

ok_array *ok_array_new(char *instring, char const *delimiters);

void ok_array_free(ok_array *ok_in);