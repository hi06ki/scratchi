#include <stdio.h>
#include <unistd.h>

/* PROTOTYPE DEFINITIONS */
int app_mgr(void);
int move_model(void);
int reverse_model(void);
int update_display(void);

unsigned int MODEL_DATA[16] = {
    0x0000,
    0x0080,
    0x0040,
    0x0080,
    0x07e0,
    0x0810,
    0x1dd8,
    0x201c,
    0x288c,
    0x2004,
    0x2784,
    0x1868,
    0x2b34,
    0x47e2,
    0x2004,
    0x700e,
};

unsigned int EXT_MODEL_DATA[16] = {};
unsigned int DISP_MODEL[16] = {};
unsigned char PRT[16][32] = {};

int COUNT = 0;
int POSITION = 0;
unsigned int REV_FLAG = 0x0;

int main(void)
{
    /* Initialize model */
    for (int i = 0; i < 16; ++i)
    {
        EXT_MODEL_DATA[i] = MODEL_DATA[i] & 0x0000ffff;
        EXT_MODEL_DATA[i] = EXT_MODEL_DATA[i] << 8;
        DISP_MODEL[i] = EXT_MODEL_DATA[i];
    }

    while (1)
    {
        app_mgr();
        sleep(1);
    }

    return 0;
}

int app_mgr(void)
{
    if ((POSITION > -8) && (POSITION < 8))
    {
        move_model();
    }
    else if ((POSITION == -8) || (POSITION == 8))
    {
        reverse_model();
        if (POSITION == -8)
        {
            ++POSITION;
        }
        else if (POSITION == 8)
        {
            --POSITION;
        }
    }

    update_display();

    ++COUNT;
    return 0;
}

int move_model(void)
{
    if (REV_FLAG == 0x0)
    {
        --POSITION;
        for (int i = 0; i < 16; ++i)
        {
            DISP_MODEL[i] = DISP_MODEL[i] << 1;
        }
    }

    else
    {
        ++POSITION;
        for (int i = 0; i < 16; ++i)
        {
            DISP_MODEL[i] = DISP_MODEL[i] >> 1;
        }
    }

    return 0;
}

int reverse_model(void)
{
    unsigned int buffer;
    for (int i = 0; i < 16; ++i)
    {
        if (REV_FLAG == 0x0) {
            buffer = MODEL_DATA[i];
            buffer = ((buffer & 0x00ff) << 8) | ((buffer & 0xff00) >> 8);
            buffer = ((buffer & 0x0f0f) << 4) | ((buffer & 0xf0f0) >> 4);
            buffer = ((buffer & 0x3333) << 2) | ((buffer & 0xcccc) >> 2);
            buffer = ((buffer & 0x5555) << 1) | ((buffer & 0xaaaa) >> 1);
            buffer = (buffer & 0x0000ffff) << 16;
            DISP_MODEL[i] = buffer;
        }
        else {
            DISP_MODEL[i] = MODEL_DATA[i] & 0x0000ffff;
        }

    }
    REV_FLAG = ~REV_FLAG;

    return 0;
}

int update_display(void)
{
    // Generate print data
    unsigned int digit;
    for (int i = 0; i < 16; ++i)
    {
        int hex_num = DISP_MODEL[i];
        for (int j = 0; j < 32; ++j)
        {
            digit = 0x80000000 >> j;
            if ((hex_num & digit) != 0)
            {
                PRT[i][j] = '#';
            }
            else
            {
                PRT[i][j] = ' ';
            }
        }
    }

    printf("\x1b[2J");
    printf(
        "\r%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n",
        PRT[0], PRT[1], PRT[2], PRT[3], PRT[4], PRT[5], PRT[6], PRT[7], PRT[8], PRT[9], PRT[10], PRT[11], PRT[12], PRT[13], PRT[14], PRT[15]);

    return 0;
}
