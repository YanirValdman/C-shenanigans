#include <stdio.h>
#include <math.h>

#define M_PI 3.14159265358979323846

// calculator cause why not
int main() {
    char operator = '\0';
    double num1 = 0;
    double num2 = 0;
    double result = 0;

    printf("Enter num 1: ");
    scanf("%lf", &num1);

    printf("Enter an operator (+ - * / s c t S C T): ");
    scanf(" %c", &operator);

    if (operator == '+' || operator == '-' ||
        operator == '*' || operator == '/') {

        printf("Enter num 2: ");
        scanf("%lf", &num2);
    }

    switch (operator) {

        // Basic operations
        case '+':
            result = num1 + num2;
            break;

        case '-':
            result = num1 - num2;
            break;

        case '*':
            result = num1 * num2;
            break;

        case '/':
            if (num2 == 0) {
                printf("Error, you tried to divide by zero\n");
                return 1;
            }

            result = num1 / num2;
            break;


        // sin, cos, tan
        // User enters degrees
        case 's':
            result = sin(num1 * M_PI / 180.0);
            break;

        case 'c':
            result = cos(num1 * M_PI / 180.0);
            break;

        case 't':
            result = tan(num1 * M_PI / 180.0);
            break;


        // arcsin, arccos, arctan
        // Result converted from radians to degrees
        case 'S':
            if (num1 < -1 || num1 > 1) {
                printf("Error: asin input must be between -1 and 1\n");
                return 1;
            }

            result = asin(num1) * 180.0 / M_PI;
            break;

        case 'C':
            if (num1 < -1 || num1 > 1) {
                printf("Error: acos input must be between -1 and 1\n");
                return 1;
            }

            result = acos(num1) * 180.0 / M_PI;
            break;

        case 'T':
            result = atan(num1) * 180.0 / M_PI;
            break;


        default:
            printf("Invalid operator\n");
            return 1;
    }

    printf("Result: %.2f\n", result);

    return 0;
}