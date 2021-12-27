#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifndef WIN32
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#else
#include <conio.h>
#include <windows.h>
#endif

#define true 1
#define false 0

typedef int bool;

char text_buf;

double calcstr(char *);
double calc(double *, char *, int, int);
char *input();

#ifndef WIN32
int kbhit(void)
{
    struct termios oldt, newt;
    int ch, oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF)
    {
        text_buf = ch;
        return 1;
    }
    return 0;
}
#endif

bool reallocchar(char **str, int add)
{
    int length = strlen(*str);
    char *tmp = calloc(length + add + 1, sizeof(char));
    if (!tmp)
        return false;

    memcpy(tmp, *str, fmin(length + 1, length + add + 1));
    tmp[length + add] = '\0';
    free(*str);
    *str = tmp;
    return true;
}

int main(void)
{
    printf("Press Enter with no input to exit.\n\n");

#ifndef WIN32
    struct termios save_settings;
    struct termios settings;
    tcgetattr(fileno(stdin), &save_settings);
    settings = save_settings;

    settings.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(fileno(stdin), TCSANOW, &settings);
    fcntl(fileno(stdin), F_SETFL, O_NONBLOCK);
#endif

    while (1)
    {
        char *str = input();

        //Exit
        if (strlen(str) == 0)
        {
            free(str);
            break;
        }

        printf(" = %g\n", calcstr(str));
        free(str);
    }

#ifndef WIN32
    tcsetattr(fileno(stdin), TCSANOW, &save_settings);
#endif

    return 0;
}

char *input()
{
    int str_len = 0, bracket_count = 0;
    char *print_str = calloc(1, sizeof(char));

    while (1)
    {
        //負荷軽減
#ifndef WIN32
        usleep(1000);
#else
        Sleep(1);
#endif

        if (kbhit())
        {
#ifdef WIN32
            text_buf = getch();
            if (text_buf == 0x0d)
                text_buf = '\n';
            if (text_buf == 0x08)
                text_buf = 127;
#endif
            if (text_buf == '\n')
                break;

            if (text_buf == ')' && bracket_count <= 0)
                continue;

            if (text_buf == '(' || text_buf == ')' || text_buf == '+' || text_buf == '-' || text_buf == '/' || text_buf == '*' || text_buf == '%' || text_buf == '^')
            {
                if (str_len > 2 && (text_buf != '(' && text_buf != ')'))
                {
                    char prev = print_str[str_len - 2];
                    if (prev == '+' || prev == '-' || prev == '/' || prev == '*' || prev == '%' || prev == '^')
                        continue;
                }

                if (text_buf == '(')
                    bracket_count++;

                if (text_buf == ')')
                    bracket_count--;

                if (reallocchar(&print_str, 3))
                {
                    str_len += 3;
                    print_str[str_len - 3] = ' ';
                    print_str[str_len - 2] = text_buf;
                    print_str[str_len - 1] = ' ';
                }
            }
            else if (text_buf == '.' || (text_buf >= '0' && text_buf <= '9'))
            {
                if (reallocchar(&print_str, 1))
                {
                    str_len++;
                    print_str[str_len - 1] = text_buf;
                }
            }
            else if (text_buf == 127)
            {
                //Backspace
                if (str_len == 0)
                    continue;

                if (str_len > 2)
                {
                    if (print_str[str_len - 2] == '(')
                        bracket_count--;
                    else if (print_str[str_len - 2] == ')')
                        bracket_count++;
                }

                int delete_len = (print_str[str_len - 1] == ' ') ? -3 : -1;
                if (reallocchar(&print_str, delete_len))
                    str_len += delete_len;
            }
            printf("\r%s   \b\b\b", print_str);
        }
    }

    //Delete Space
    int space_count = 0;
    for (int j = 0; j < strlen(print_str); j++)
    {
        if (print_str[j] == ' ')
            space_count++;
    }

    if (space_count != 0)
    {
        int now_space_count = 0;
        str_len = strlen(print_str) - space_count;
        char *print_str_tmp = calloc(str_len + 1, sizeof(char));
        for (int j = 0; j < str_len; j++)
        {
            while (print_str[j + now_space_count] == ' ')
                now_space_count++;
            print_str_tmp[j] = print_str[j + now_space_count];
        }
        free(print_str);
        print_str = print_str_tmp;
    }
    return print_str;
}

double calcstr(char *str_in)
{
    int str_len = strlen(str_in);
    char *str_proc = calloc(str_len + 1, sizeof(char));
    strcpy(str_proc, str_in);
    bool bracket_judge = false;

    //括弧の判定
    do
    {
        bracket_judge = false;
        for (int i = 0; str_proc[i] != '\0'; i++)
        {
            if (str_proc[i] == '(')
            {
                int bracket_end, bracket_count = 1;
                bracket_judge = true;
                for (bracket_end = i + 1;; bracket_end++)
                {
                    if (str_proc[bracket_end] == '(')
                        bracket_count++;
                    else if (str_proc[bracket_end] == ')')
                        bracket_count--;
                    else if (str_proc[bracket_end] == '\0')
                    {
                        char *tmp = calloc(++str_len + 1, sizeof(char));
                        strcpy(tmp, str_proc);
                        tmp[str_len - 1] = ')';
                        free(str_proc);
                        str_proc = tmp;
                        break;
                    }

                    if (bracket_count == 0)
                        break;
                }
                bracket_end++;
                char *tmp = calloc(bracket_end - i - 2, sizeof(char));
                for (int j = i + 1; j < bracket_end - 1; j++)
                    tmp[j - i - 1] = str_proc[j];

                char in_bracket[256]; //ここまで大きくならないと踏んで256
                int in_bracket_str_len = sprintf(in_bracket, "%g", calcstr(tmp));

                char *cat_str = calloc(str_len - (bracket_end - i) + in_bracket_str_len, sizeof(char));

                for (int j = 0; j < str_len - (bracket_end - i) + in_bracket_str_len; j++)
                {
                    if (j < i)
                        cat_str[j] = str_proc[j];
                    else if (j < i + in_bracket_str_len)
                        cat_str[j] = in_bracket[j - i];
                    else
                        cat_str[j] = str_proc[(bracket_end - i - in_bracket_str_len) + j];
                }

                free(str_proc);
                str_proc = cat_str;
                free(tmp);
                break;
            }
        }
    } while (bracket_judge);

    //すべての項を取り出す。
    double *nums = calloc(1, sizeof(double));
    char *opes = NULL;

    int nums_count = 1, opes_count = 0, from_point_count = 0;
    bool is_decimal = false;
    char now_char;
    for (int i = 0; (now_char = str_proc[i]) != '\0'; i++)
    {
        if (now_char == '+' || now_char == '-' || now_char == '/' || now_char == '*' || now_char == '%' || now_char == '^')
        {
            double *numstmp = calloc(++nums_count, sizeof(double));
            for (int j = 0; j < nums_count - 1; j++)
                numstmp[j] = nums[j];
            free(nums);
            nums = numstmp;

            char *opestmp = calloc(++opes_count, sizeof(char));
            for (int j = 0; j < opes_count - 1; j++)
                opestmp[j] = opes[j];
            opestmp[opes_count - 1] = now_char;
            free(opes);
            opes = opestmp;

            is_decimal = false;
        }
        else if (now_char == '.')
        {
            is_decimal = true;
            from_point_count = -1;
        }
        else if (now_char >= '0' && now_char <= '9')
        {
            if (!is_decimal)
                nums[nums_count - 1] = nums[nums_count - 1] * 10 + atol(&now_char);
            else
                nums[nums_count - 1] = nums[nums_count - 1] + atol(&now_char) * powl(10, from_point_count--);
        }
    }

    double answer = calc(nums, opes, nums_count, opes_count);

    free(nums);
    free(opes);
    free(str_proc);

    return answer;
}

double calc(double *nums, char *opes, int nums_count, int opes_count)
{
    for (int j = 0; j < 3; j++)
    {
        for (int i = 0; i < opes_count; i++)
        {
            if (((j == 0) && (opes[i] == '^')) || (j == 1 && (opes[i] == '/' || opes[i] == '*' || opes[i] == '%')) || (j == 2 && (opes[i] == '+' || opes[i] == '-')))
            {
                char ope = opes[i];
                if (ope == '/')
                    nums[i] = nums[i] / nums[i + 1];
                else if (ope == '*')
                    nums[i] = nums[i] * nums[i + 1];
                else if (ope == '+')
                    nums[i] = nums[i] + nums[i + 1];
                else if (ope == '-')
                    nums[i] = nums[i] - nums[i + 1];
                else if (ope == '%')
                    nums[i] = fmod(nums[i], nums[i + 1]);
                else if (ope == '^')
                    nums[i] = pow(nums[i], nums[i + 1]);

                int nextnums_count = nums_count - 1, nextopes_count = opes_count - 1;
                for (int k = i + 1; k < nextnums_count; k++)
                    nums[k] = nums[k + 1];
                for (int k = i; k < nextopes_count; k++)
                    opes[k] = opes[k + 1];

                return calc(nums, opes, nextnums_count, nextopes_count);
            }
        }
    }
    return nums[0];
}
