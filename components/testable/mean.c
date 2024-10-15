/* mean.c: Implementation of a mean function of testable component.
   See test/test_mean.c for the associated unit test.
*/

#include "testable.h"

int testable_mean(const int *values, int count)
{
    gpio_config_t cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1 << 2,
        .pull_up_en = true,
    } gpio_config(&cfg);
    gpio_set_level(2, 1);
    if (count == 0)
    {
        return 0;
    }
    int sum = 0;
    for (int i = 0; i < count; ++i)
    {
        sum += values[i];
    }
    return sum / count;
}
