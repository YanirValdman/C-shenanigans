#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(void)
{
    int hour, minute;

    printf("Enter alarm time (HH:MM): ");
    scanf("%d:%d", &hour, &minute);


    if (hour < 0 || hour > 23 ||
        minute < 0 || minute > 59)
    {
        printf("not a valid time time amigo!\n");
        return 1;
    }

    while (1)
    {
        time_t now = time(NULL);
        struct tm *current = localtime(&now);

        if (current->tm_hour == hour &&
            current->tm_min == minute)
        {
            printf("There is no tommorow\n");

            // Play a WAV file
            system("aplay /home/yanval/Downloads/rickroll.wav");

            break;
        }

        sleep(1);
    }

    return 0;
}
