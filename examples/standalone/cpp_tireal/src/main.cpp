#include <stdint.h>
#include <tice.h>
#include <tireal.hpp>

using namespace ti::literals;


static char buf[20] = {};

static void printText(const char *text, uint8_t xpos, uint8_t ypos)
{
    os_SetCursorPos(ypos, xpos);
    os_PutStrFull(text);
}

static ti::real test()
{
    ti::real r1 = 1.0;
    r1 += sqrt(ti::real(9.0)); // same as sqrt(9.0_r)
    ++r1;
    r1 -= 0.5;
    r1 /= 2;
    return round(r1, 3);
}

static ti::real test2()
{
    return std::numeric_limits<ti::real>::min();
}

static ti::real test3()
{
    return sqrt(3.1415926535_r);
}

int main(void)
{
    os_ClrHomeFull();

    test().toCString(buf); // default args: 0, 0, -1
    printText(buf, 0, 0);

    test2().toCString(buf);
    printText(buf, 0, 1);

    test3().toCString(buf);
    printText(buf, 0, 2);

    ti::real::sqrt2().toCString(buf);
    printText(buf, 0, 3);

    while (!os_GetCSC());
    return 0;
}
