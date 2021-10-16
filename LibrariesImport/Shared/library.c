#include "library.h"

int dynamicPow(int base, int power) {
    if(power == 0)
        return 1;
    return base * dynamicPow(base, power - 1);
}
