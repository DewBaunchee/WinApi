#include "library.h"

int customPow(int base, int power) {
    if(power == 0)
        return 1;
    return base * customPow(base, power - 1);
}
