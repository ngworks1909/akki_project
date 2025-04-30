#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cnt = 0;

char **readfromfile(char *filename) {
    char **lines = NULL;
    char str[1000];
    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("Unable to open the file\n");
        exit(1);
    }

    while (fgets(str, sizeof(str), fp)) {
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
        if (ptr) {
            *ptr = '\0';  // Truncate line at //
        }
    }
    return lines;
}

char **remove_multiple_line_comment(char **lines) {
    int in_comment = 0;

    for (int i = 0; i < cnt; i++) {
        char *start, *end;

        if (!in_comment) {
            start = strstr(lines[i], "/*");
            if (start) {
                end = strstr(start + 2, "*/");
                if (end) {
                    memmove(start, end + 2, strlen(end + 2) + 1);
                } else {
                    *start = '\0';
                    in_comment = 1;
                }
            }
        } else {
            end = strstr(lines[i], "*/");
            if (end) {
                in_comment = 0;
                memmove(lines[i], end + 2, strlen(end + 2) + 1);
            } else {
                lines[i][0] = '\0';  // Blank the line
            }
        }
    }

    return lines;
}

void writetothefile(char *filename, char **lines) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Unable to write to file\n");
        exit(1);
    }

    for (int i = 0; i < cnt; i++) {
        if (lines[i][0] != '\0') {  // skip blank lines
            fputs(lines[i], fp);
            fputc('\n', fp);
        }
    }

    fclose(fp);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s input.c output.c\n", argv[0]);
        exit(1);
    }

    char **lines = readfromfile(argv[1]);
    lines = remove_single_line_comment(lines);
    lines = remove_multiple_line_comment(lines);
    writetothefile(argv[2], lines);

    // Free memory
    for (int i = 0; i < cnt; i++) {
        free(lines[i]);
    }
    free(lines);

    return 0;
}
