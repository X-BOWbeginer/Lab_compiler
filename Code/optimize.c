#include "optimize.h"
void optimize(char *input, char *output)
{

    readfile(input, output);
}
void optimize_code(char *input[], int line_count)
{
    int has_optimized = 1;

    while (has_optimized)
    {
        has_optimized = 0;

        for (int i = 0; i < line_count; i++)
        {
            char *line = input[i];

            // 常量折叠
            // printf("linecount%s\n", line);
            if (strstr(line, " := #") != NULL)
            // if (strstr(line, "+") != NULL || strstr(line, "-") != NULL || strstr(line, "*") != NULL || strstr(line, "/") != NULL)
            {
                // printf("常量折叠\n");
                char var[10], op[3], num1[10], num2[10];

                int cnt = sscanf(line, "%s := #%s %s #%s", var, num1, op, num2);
                if (cnt != 4) // no match
                {
                    // printf("%d\n", cnt);
                    goto l1;
                }
                int n1 = atoi(num1);
                int n2 = atoi(num2);
                int result;
                // printf("常量折叠\n");
                //  printf("var%s := n1%s op%s n2%s\n", var, num1, op, num2);
                if (strcmp(op, "+") == 0)
                {
                    result = n1 + n2;
                }
                else if (strcmp(op, "-") == 0)
                {
                    result = n1 - n2;
                }
                else if (strcmp(op, "*") == 0)
                {
                    result = n1 * n2;
                }
                else if (strcmp(op, "/") == 0)
                {
                    result = n1 / n2;
                }
                else
                {
                    goto l1;
                }

                sprintf(line, "%s := #%d\n", var, result);
                has_optimized = 1;
            }
            // 替换
        l1:
            if (strstr(line, " := #") != NULL)
                if (strstr(line, "+") == NULL && strstr(line, "-") == NULL && strstr(line, "*") == NULL && strstr(line, "/") == NULL)
                {
                    char var[10], num[10];
                    int cnt = sscanf(line, "%s := #%s", var, num);
                    if (cnt != 2) // no match
                    {
                        printf("%d\n", cnt);
                        continue;
                    }

                    for (int j = i + 1; j < line_count; j++)
                    {

                        if (strstr(input[j], var) != NULL)
                        {
                            char left[10], op[3], num1[10], num2[10];

                            int cnt = sscanf(input[j], "%s := %s %s %s", left, num1, op, num2);
                            // printf("left%s := %s op%s %s\n", left, num1, op, num2);
                            // printf("line=%s\n", input[j]);
                            if (!strcmp(var, left))
                            {
                                break;
                            }
                            if (cnt != 4) // no match
                            {
                                char right[10];
                                int acnt = sscanf(input[j], "%s := %s\n", left, right);
                                if (acnt != 2)
                                {
                                    // printf("nomatch%d\n", cnt);
                                    continue;
                                }
                                else if (!strcmp(right, var))
                                {
                                    sprintf(input[j], "%s := #%s\n", left, num);
                                    has_optimized = 1;
                                    // continue;
                                }
                            }
                            else
                            {
                                if (!strcmp(num1, var))
                                {
                                    // printf("ok\n");
                                    sprintf(input[j], "%s := #%s %s %s\n", left, num, op, num2);
                                    has_optimized = 1;
                                }
                                if (!strcmp(num2, var))
                                {
                                    // printf("ok\n");
                                    sprintf(input[j], "%s := %s %s #%s\n", left, num1, op, num);
                                    has_optimized = 1;
                                }
                            }
                        }
                    }
                }
        }
    }
}
void readfile(char *filename, char *out)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Unable to open file '%s'\n", filename);
        return;
    }

    char *input[MAX_LINES];
    int line_count = 0;

    for (int i = 0; i < MAX_LINES; i++)
    {
        input[i] = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
    }

    while (fgets(input[line_count], MAX_LINE_LENGTH, file) != NULL)
    {
        line_count++;
    }

    fclose(file);

    optimize_code(input, line_count);
    printf("line_count=%d\n", line_count);
    // printf("\nOptimized code:\n");
    FILE *outf = fopen(out, "w");
    for (int i = 0; i < line_count; i++)
    {
        if (input[i][0] != '\0')
        {
            fputs(input[i], outf);
        }
    }

    for (int i = 0; i < MAX_LINES; i++)
    {
        free(input[i]);
    }
}
