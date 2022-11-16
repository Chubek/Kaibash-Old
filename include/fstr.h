/*
The followingf utility is taken ad-verbatim form the book 21st Century C
by Ben Klemens. Please give credit where it's due.

This utility aims to split a string based on matching regex

*/

typedef struct {
    char *data;
    size_t start, end;
    int* refs; //it uses this propertty to keep the track of references
} fstr_s;

fstr_s *fstr_new(char const *filename);
fstr_s *fstr_new_from_buffer(char const *buffer);

fstr_s *fstr_copy(fstr_s const *in, size_t start, size_t len);
void fstr_show(fstr_s const *fstr);
void fstr_free(fstr_s *in);

typedef struct {
    fstr_s **strings;
    int count;
} fstr_list;

fstr_list fstr_split (fstr_s const *in, gchar const *start_pattern);
void fstr_list_free(fstr_list in);