#include<stdio.h>

int main()
{
    int line;
    char c;
    char input;
    FILE *f;

    do
    {
        printf("Which information do you want?\n");
        printf("Version(v),CPU(c),Memory(m),Time(t),All(a),Exit(e)?\n");
        scanf("%s",&input);
        f=fopen("/proc/my_info","r");
        line = 0;
        if(input == 'v')
        {
            while((c = fgetc(f)) != EOF)
            {
                if(c=='=')
                {
                    line++;
                }
                if(line == 24 && c!= '=')
                {
                    printf("%c",c);
                }
            }
            printf("\n");
        }


        else if(input == 'c')
        {
            while((c = fgetc(f)) != EOF)
            {
                if(c=='=')
                {
                    line++;
                }
                if(line == 51 && c!= '=')
                {
                    printf("%c",c);
                }
            }
        }

        else if(input == 'm')
        {
            while((c = fgetc(f)) != EOF)
            {
                if(c=='=')
                {
                    line++;
                }
                if(line ==77 && c!= '=')
                {
                    printf("%c",c);
                }
            }
        }

        else if(input == 't')
        {
            while((c = fgetc(f)) != EOF)
            {
                if(c=='=')
                {
                    line++;
                }
                if(line ==103 && c!= '=')
                {
                    printf("%c",c);
                }
            }
            printf("\n\n");
        }

        else if( input == 'a')
        {
            while((c = fgetc(f)) != EOF)
            {
                printf("%c",c);
            }
            printf("\n");
        }

        else if( input == 'e')
        {
            fclose(f);
            return 0;
        }

        fclose(f);
        printf("--------------------------------------\n");
    }
    while(1);

}