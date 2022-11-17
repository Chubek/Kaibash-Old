typedef struct {
    char cert_hash[5000];
    char cert_file[50000];
    char key_file[50000];
    char password[2000];

} config_parsed_s;

typedef enum {
    ConfCertHash,
    ConfCertFile,
    ConfKeyFile,
    ConfPassword,
} config_labels;

typedef struct {
    config_parsed_s configp;
    int *ref_counter;
} configp_wrapper_s;

void assert_config_label(config_labels *label, char *current_line, char *current_content);

configp_wrapper_s *parse_config(char const *filepath);
configp_wrapper_s *copy_config(configp_wrapper_s const *in);
void free_config(configp_wrapper_s *in);

#define IS_COLON(c) ((c == ':') ? 1 : 0)
#define IS_CAPITAL(c) ((c > 64 && c <= 90) ? 1 : 0)
#define IS_SPACE(c) ((c == ' ') ? 1 : 0)
#define IS_SEMICOLON(c) ((c == ';') ? 1 : 0)
#define STRING_EQ(s1, s2) ((strcmp(s1, s2) == 0 ? 1 : 0))

#define CERT_HASH   "CERT_HASH"
#define CERT_FILE   "CERT_FILE"
#define KEY_FILE    "KEY_FILE"
#define PASSWORD    "PASSWORD"