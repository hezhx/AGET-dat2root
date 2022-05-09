#ifndef USER_DEFINED_FUNC
#define USER_DEFINED_FUNC

short high_low_exchange(short data1)
{
    short high = (data1 & 0xff00) >> 8;
    short low = data1 & 0xff;

    short data;
    data = (low << 8) | high;
    return data;
}

#endif