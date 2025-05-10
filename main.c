#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>

int cnt = 0;

char **readfromfile(char *filename) {
    char **lines = NULL;
    char str[100];
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Unable to open the file\n");
        exit(0);
    }

    while (fgets(str, 100, fp)) {
        lines = realloc(lines, (cnt + 1) * sizeof(*lines));
        lines[cnt] = malloc(strlen(str) + 1);
        strcpy(lines[cnt], str);
        cnt++;
    }

    fclose(fp);
    return lines;
}

char **remove_single_line_comment(char **lines) {
    for (int i = 0; i < cnt; i++) {
        char *ptr = strstr(lines[i], "//");
        if (ptr) *ptr = '\0';
    }
    return lines;
}

char **remove_multiple_line_comment(char **lines) {
    int comment = 0;
    for (int i = 0; i < cnt; i++) {
        char *start, *end;
        if (!comment) {
            start = strstr(lines[i], "/*");
            if (start) {
                end = strstr(start + 2, "*/");
                if (end) {
                    memmove(start, end + 2, strlen(end + 2) + 1);
                } else {
                    *start = '\0';
                    comment = 1;
                }
            }
        } else {
            end = strstr(lines[i], "*/");
            if (end) {
                comment = 0;
                memmove(lines[i], end + 2, strlen(end + 2) + 1);
            } else {
                lines[i][0] = '\0';
            }
        }
    }
    return lines;
}

// Store macros
typedef struct {
    char key[50];
    char value[100];
} Macro;

int macro_count = 0;
Macro macros[100];

void extract_macros(char **lines) {
    for (int i = 0; i < cnt; i++) {
        if (strncmp(lines[i], "#define", 7) == 0) {
            char name[50], value[100];
            if (sscanf(lines[i], "#define %s %[^\n]", name, value) == 2) {
                strcpy(macros[macro_count].key, name);
                strcpy(macros[macro_count].value, value);
                macro_count++;
                lines[i][0] = '\0';  // remove macro from code
            }
        }
    }
}

void replace_macros(char **lines) {
    for (int i = 0; i < cnt; i++) {
        char temp[1000] = "";
        int in_string = 0;

        for (int j = 0; lines[i][j] != '\0';) {
            if (lines[i][j] == '"') {
                in_string = !in_string;
                strncat(temp, &lines[i][j], 1);
                j++;
            } else if (!in_string) {
                int replaced = 0;
                for (int k = 0; k < macro_count; k++) {
                    int len = strlen(macros[k].key);
                    if (strncmp(&lines[i][j], macros[k].key, len) == 0 &&
                        !isalnum(lines[i][j + len]) && lines[i][j + len] != '_') {
                        strcat(temp, macros[k].value);
                        j += len;
                        replaced = 1;
                        break;
                    }
                }
                if (!replaced) {
                    strncat(temp, &lines[i][j], 1);
                    j++;
                }
            } else {
                strncat(temp, &lines[i][j], 1);
                j++;
            }
        }
        strcpy(lines[i], temp);
    }
}

void writetothefile(char *filename, char **lines) {
    FILE *fp = fopen(filename, "w");

    if (fp == NULL) {
        printf("Unable to write to file\n");
        exit(0);
    }

    for (int i = 0; i < cnt; i++) {
        if (lines[i][0] != '\0') {
            fputs(lines[i], fp);
            fputc('\n', fp);
        }
    }

    fclose(fp);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Invalid input\n");
        exit(0);
    }

    char **lines = readfromfile(argv[1]);
    extract_macros(lines);
    lines = remove_single_line_comment(lines);
    lines = remove_multiple_line_comment(lines);
    replace_macros(lines);
    writetothefile(argv[2], lines);

    for (int i = 0; i < cnt; i++) {
        free(lines[i]);
    }
    free(lines);
    return 0;
}
