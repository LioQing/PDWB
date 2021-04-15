#include <cstdio>
#include <vector>
#include <fstream>
#include <streambuf>

int main(int argc, char* argv[])
{
    std::ifstream bf_file;

    if (argc == 1)
    {
        printf("No argument is entered for loading the file.\n");
        return 0;
    }

    bf_file.open(argv[1]);

    std::string buffer((std::istreambuf_iterator<char>(bf_file)),
                        std::istreambuf_iterator<char>());

    bf_file.close();

    std::vector<std::pair<size_t, size_t>> loop_pos;
    std::vector<char> data(1, 0);
    size_t ptr = 0;

    for (size_t i = 0; i < buffer.size(); ++i)
    {
        switch (buffer[i])
        {
            case '>':
            {
                if (ptr + 1 == data.size())
                {
                    data.emplace_back(0);
                }

                ++ptr;
                break;
            }
            case '<':
            {
                if (ptr == 0)
                {
                    printf("Pointer out of range error.\n");
                    return 0;
                }

                --ptr;
                break;
            }
            case '+':
            {
                if (data[ptr] == 255)
                {
                    data[ptr] = 0;
                    break;
                }

                ++data[ptr];
                break;
            }
            case '-':
            {
                if (data[ptr] == 0)
                {
                    data[ptr] = 255;
                    break;
                }

                --data[ptr];
                break;
            }
            case '.':
            {
                putchar(data[ptr]);
                break;
            }
            case ',':
            {
                data[ptr] = getchar();
                break;
            }
            case '[':
            {
                if (loop_pos.size() == 0 || loop_pos.at(loop_pos.size() - 1).first != i)
                {
                    size_t j;
                    for (j = i; j < buffer.size(); ++j)
                    {
                        if (buffer[j] != ']') continue;

                        loop_pos.emplace_back(i, j);
                        break;
                    }

                    if (j == buffer.size())
                    {
                        printf("Missing ']' syntax error.\n");
                        return 0;
                    }
                }

                if (data[ptr] == 0)
                {
                    i = loop_pos.at(loop_pos.size() - 1).second;
                    loop_pos.erase(loop_pos.end() - 1);
                }
                
                break;
            }
            case ']':
            {
                if (loop_pos.size() == 0)
                {
                    printf("Unexpected ']' syntax error.\n");
                    return 0;
                }

                i = loop_pos.at(loop_pos.size() - 1).first - 1;
                break;
            }
            case '#':
            {
                switch (buffer[++i])
                {
                    case 'D':
                    case 'd':
                    {
                        printf("\n\n|");

                        for (auto cell : data)
                        {
                            printf("%03d|", cell);
                        }
                        printf("\n ");

                        for (size_t j = 0; j < ptr; ++j)
                        {
                            printf("    ");
                        }
                        printf(" ^ \n ");

                        for (size_t j = 0; j < ptr; ++j)
                        {
                            printf("    ");
                        }
                        printf("ptr\n\n");

                        printf("Press enter to continue...");
                        getchar();
                        putchar('\n');
                        fflush(stdin);

                        break;
                    }
                    case 'H':
                    case 'h':
                    {
                        printf("\n\n|");

                        for (auto cell : data)
                        {
                            printf("%02X|", cell);
                        }
                        printf("\n ");

                        for (size_t j = 0; j < ptr; ++j)
                        {
                            printf("   ");
                        }
                        printf("^ \n");

                        for (size_t j = 0; j < ptr; ++j)
                        {
                            printf("   ");
                        }
                        printf("ptr\n\n");

                        printf("Press enter to continue...");
                        getchar();
                        putchar('\n');
                        fflush(stdin);

                        break;
                    }
                }
            }
            default:
                break;
        }
    }

    return 0;
}