#include <stdio.h>

#define BUFSIZE 100
char buf[BUFSIZE]; /* buffer for ungetch */
int bufp = 0;      /* next free position in buf */

/* getch() and ungetch() are from K&R page 79 */
int getch(void) /* get a possibly pushed back character */
{
    return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c) /* push a character back on input */
{
    if (bufp >= BUFSIZE)
        printf("ungetch: too many characters\n");
    else
        buf[bufp++] = c;
}

int setDecNumber(int decVal);
int setBinNumber(int charVal);
int setOctNumber(int charVal);
int setHexNumber(int charVal);
void resetConvertedValues(void);
int isHexNumber(int charVal);
int isOctNumber(int charVal);
int isBinNumber(int charVal);
int isOperator(int charVal);
int calculate(int total, int operand2, int op);

/* Helper Functions for converting values to decimal */
int decimalTotal, binaryTotal, octalTotal, hexTotal;
int setDecNumber(int decVal)
{
    decimalTotal *= 10;
    decimalTotal += decVal;

    return decimalTotal;
}

int setBinNumber(int charVal)
{
    int binVal = charVal - '0';
    binaryTotal *= 2;
    binaryTotal += binVal;

    return binaryTotal;
}

int setOctNumber(int charVal)
{
    int octVal = charVal - '0';
    octalTotal *= 8;
    octalTotal += octVal;

    return octalTotal;
}

int setHexNumber(int charVal)
{
    int hexVal;

    /* Convert to integer values */
    if (charVal >= '0' && charVal <= '9') /* is a digit */
        hexVal = charVal - '0';
    else if (charVal >= 'A' && charVal <= 'F') /* is between A-F */
        hexVal = charVal - 55; /* 'A' - 55 = 10, 'B' - 55 = 11, etc. */
    else /* must be a-f */
        hexVal = charVal - 87; /* 'a' - 87 = 19, 'b' - 87 = 11, etc. */

    hexTotal *= 16;
    hexTotal += hexVal;

    return hexTotal;
}

void resetConvertedValues(void)
{
    decimalTotal = 0;
    binaryTotal = 0;
    octalTotal = 0;
    hexTotal = 0;
}

/* Helper functions for determining valid numbers/operators */
int isHexNumber(int charVal)
{
    if ((charVal >= '0' && charVal <= '9') ||
        (charVal >= 'A' && charVal <= 'F') ||
        (charVal >= 'a' && charVal <= 'f'))
        return 1;

    return 0;
}

int isOctNumber(int charVal)
{
    if (charVal >= '0' && charVal <= '7')
        return 1;

    return 0;
}

int isBinNumber(int charVal)
{
    if (charVal == '0' || charVal == '1')
        return 1;

    return 0;
}

int isOperator(int charVal)
{
    int temp;
    if ((charVal == '+') ||
        (charVal == '-') ||
        (charVal == '*') ||
        (charVal == '/') ||
        (charVal == '%') ||
        (charVal == '^') ||
        (charVal == '&') ||
        (charVal == '|'))
        return 1;

    else if (charVal == '<')
    {
        temp = getch();
        if (temp == '<')
            return 1;

        temp = 0;
        return 0;
    }
    else if (charVal == '>')
    {
        temp = getch();
        if (temp == '>')
            return 1;

        temp = 0;
        return 0;
    }
    return 0;
}

int calculate(int total, int operand2, int op)
{
    /* printf("total = %i\noperand2 = %i\n", total, operand2); */
    switch(op)
    {
        case '+':
            total += operand2;
            break;
        case '-':
            total -= operand2;
            break;
        case '*':
            total *= operand2;
            break;
        case '/':
            total /= operand2;
            break;
        case '%':
            total %= operand2;
            break;
        case '^':
        {
            if (operand2 == 0)
                total = 1;
            else
            {
                int baseVal = total;
                int i = 2;
                while (i <= operand2)
                {
                    total *= baseVal;
                    i++;
                }
            }
            break;
        }
        case '&':
            total &= operand2;
            break;
        case '|':
            total |= operand2;
            break;
        case '<':
            total <<= operand2;
            break;
        case '>':
            total >>= operand2;
            break;
        default:
            break;
    }

    return total;
}

int main ()
{
    int c;
    int total=0, operand2=0, op=0;

    printf("\n=== Calculator ===\n");

    while ( (c = getch()) != EOF)
    {
        /* Empty input, do nothing*/
        if ((c == '\n') && !total && !operand2 && !op)
            continue;

        /* Decimal Number */
        if ((c-'0') > 0 && (c-'0') <= 9)
        {
            if (total && op)
                operand2 = setDecNumber((c-'0'));
            else
                total = setDecNumber((c-'0'));
        }

        /* Hexadecimal, Binary, or Octal Number */
        else if (c == '0')
        {
            /* Make sure the 0 is not part of a decimal value first */
            if ((total && !op && !operand2) || operand2)
            {
                while (((c-'0') >= 0) && ((c-'0') <= 9))
                {
                    if (operand2)
                        operand2 = setDecNumber((c-'0'));
                    else
                        total = setDecNumber((c-'0'));
                    c = getch();
                }

                if (!isOperator(c) && c != '\n')
                {
                    printf("ERROR: expected number.\n");
                    return 0;
                }

                ungetch(c);
                continue;
            }

            /* Must be a hexadecimal, binary, or octal number */
            else
            {
                c = getch();
                /* printf("c = %c\n", c); */

                if (c == '\n')
                {
                    /* End of line, calculate */
                    if (total && op)
                    {
                        total = calculate(total, 0, op);

                        printf("= %i\n\n", total);

                        /* Reset everything */
                        resetConvertedValues();
                        total = 0;
                        operand2 = 0;
                        op = 0;
                    }
                    else
                        printf("= %i\n\n", total);
                }

                else if (isOperator(c))
                    ungetch(c);

                /* Octal number */
                else if (isOctNumber(c))
                {
                    while(isOctNumber(c))
                    {
                        if (total && op)
                            operand2 = setOctNumber(c);
                        else
                            total = setOctNumber(c);
                        c = getch();
                    }

                    if (!isOperator(c) && c != '\n')
                    {
                        printf("ERROR: expected number.\n");
                        return 0;
                    }

                    ungetch(c);
                }

                /* Hexadecimal number */
                else if (c == 'x' || c == 'X')
                {
                    c = getch();
                    while(isHexNumber(c))
                    {
                        if (total && op)
                            operand2 = setHexNumber(c);
                        else
                            total = setHexNumber(c);
                        c = getch();
                    }

                    if (!isOperator(c) && c != '\n')
                    {
                        printf("ERROR: expected number.\n");
                        return 0;
                    }

                    ungetch(c);
                }

                /* Binary number */
                else if (c == 'b' || c == 'B')
                {
                    c = getch();
                    while(isBinNumber(c))
                    {
                        if (total && op)
                            operand2 = setBinNumber(c);
                        else
                            total = setBinNumber(c);
                        c = getch();
                    }

                    if (!isOperator(c) && c != '\n')
                    {
                        printf("ERROR: expected number.\n");
                        return 0;
                    }

                    ungetch(c);
                }

                /* If just the number 0 is entered, print it out */
                else
                {
                    printf("ERROR: expected number.\n");
                    return 0;
                }
            }
        }

        /* Ignore whitespace */
        else if (c == ' ' || c == '\t')
            ;

        else if (isOperator(c))
        {
            resetConvertedValues();
            if (op && operand2)
            {
                total = calculate(total, operand2, op);
                operand2 = 0;
                op = 0;
            }

            if (op && !operand2)
            {
                printf("ERROR: unknown operator 0xFFFFFFFF\n");
                return 0;
            }

            op = c;
        }

        else if (c == '\n')
        {
            /* End of line, calculate final answer */
            if (op && operand2)
                total = calculate(total, operand2, op);

            printf("= %i\n\n", total);

            /* Reset everything */
            resetConvertedValues();
            total = 0;
            operand2 = 0;
            op = 0;
        }

        else
        {
            printf("ERROR: expected number.\n");
            return 0;
        }

    }

    return 0;
}
