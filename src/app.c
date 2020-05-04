#include <stdio.h>
#include <unistd.h>

/* PROTOTYPE DEFINITIONS */
int app_mgr(void);
int move_model(void);
int reverse_model(void);
int update_display(void);

struct MOD_LIB {
    unsigned int mod_no;
    unsigned int move[2][16];
};

struct MOD_LIB MOD_LIB[2] = {
    {
        0,
        {
            {
                0xe007, 0x8001, 0x8201, 0x0400,
                0x0ff0, 0x0400, 0x0200, 0x0000,
                0x0000, 0x0180, 0x0240, 0x03c0,
                0x0240, 0x8241, 0x8001, 0xe007,
            },
            {
                0xe007, 0x8001, 0x8201, 0x0400,
                0x0ff0, 0x0400, 0x0200, 0x0000,
                0x0000, 0x0380, 0x0240, 0x0380,
                0x0240, 0x8381, 0x8001, 0xe007,
            }
        },
    },
    {
        1,
        {
            {
                0x0000, 0x0060, 0x0f20, 0x0120,
                0x07e0, 0x0810, 0x7008, 0x41c8,
                0x7808, 0x2008, 0x3830, 0x07c0,
                0x0100, 0x0104, 0x0154, 0x00a8,
            },
            {
                0x0000, 0x00c0, 0x1e40, 0x0240,
                0x0fc0, 0xf020, 0x8110, 0xf290,
                0x1110, 0x7010, 0x4060, 0x7f80,
                0x0200, 0x0204, 0x0264, 0x0198,
            }
        },
    },
};

unsigned int MOD[2][16] = {};
unsigned int DISP[16] = {};
unsigned char PRT[16][32] = {};

int COUNT = 0;
int MOD_NO = 0;
int POSITION = 0;
int STATE = 0x0;
unsigned int REV_FLAG = 0x0;

int main(void) {
    /* Initialize model and first display*/
    for (int i = 0; i < 16; ++i) {
        MOD[0][i] = MOD_LIB[MOD_NO].move[0][i]; 
        MOD[1][i] = MOD_LIB[MOD_NO].move[1][i]; 
        DISP[i] = (MOD[STATE][i] & 0x0000ffff) << 8;
    }
    update_display();
    sleep(1);

    while (1) {
        app_mgr();
        sleep(1);
    }

    return 0;
}

int app_mgr(void) {
    if (((POSITION == -8) && REV_FLAG == 0x0) || ((POSITION == 8) && REV_FLAG == 0x1)) {
        reverse_model();
    }

    else if (-8 <= POSITION <= 8) {
        move_model();
    }

    update_display();

    ++COUNT;
    return 0;
}

int move_model(void) {
    STATE^=0x1;

    if (REV_FLAG == 0x0) {
        --POSITION;
        if (POSITION >= 0) {
            for (int i = 0; i < 16; ++i) {
                DISP[i] = (MOD[STATE][i] & 0x0000ffff) << (8 - POSITION);
            }
        }

        else {
            for (int i = 0; i < 16; ++i) {
                DISP[i] = (MOD[STATE][i] & 0x0000ffff) << (8 - POSITION);
           } 
        }
    }

    else {
        ++POSITION;
        if (POSITION < 0) {
            for (int i = 0; i < 16; ++i) {
                DISP[i] = (MOD[STATE][i] & 0x0000ffff) << (8 - POSITION);
            }
        }

        else {
            for (int i = 0; i < 16; ++i) {
                DISP[i] = (MOD[STATE][i] & 0x0000ffff) << (8 - POSITION);
           } 
        }
    }

    return 0;
}

int reverse_model(void) {
    unsigned int buffer;
    for (int i = 0; i < 16; ++i) {
        if (REV_FLAG == 0x0) {
            /* Reverse model: state 0 */
            buffer = MOD_LIB[MOD_NO].move[0][i];
            buffer = ((buffer & 0x00ff) << 8) | ((buffer & 0xff00) >> 8);
            buffer = ((buffer & 0x0f0f) << 4) | ((buffer & 0xf0f0) >> 4);
            buffer = ((buffer & 0x3333) << 2) | ((buffer & 0xcccc) >> 2);
            buffer = ((buffer & 0x5555) << 1) | ((buffer & 0xaaaa) >> 1);
            MOD[0][i] = buffer;

            /* Reverse model: state 1 */
            buffer = MOD_LIB[MOD_NO].move[1][i];
            buffer = ((buffer & 0x00ff) << 8) | ((buffer & 0xff00) >> 8);
            buffer = ((buffer & 0x0f0f) << 4) | ((buffer & 0xf0f0) >> 4);
            buffer = ((buffer & 0x3333) << 2) | ((buffer & 0xcccc) >> 2);
            buffer = ((buffer & 0x5555) << 1) | ((buffer & 0xaaaa) >> 1);
            MOD[1][i] = buffer;

            DISP[i] = (MOD[STATE][i] & 0x0000ffff) << 16;
        }

        else {
            MOD[0][i] = MOD_LIB[MOD_NO].move[0][i];
            MOD[1][i] = MOD_LIB[MOD_NO].move[1][i];
            DISP[i] = MOD[STATE][i] & 0x0000ffff;
        }

    }
    REV_FLAG^=0x1;

    return 0;
}

int update_display(void) {
    // Generate print data
    unsigned int digit;
    for (int i = 0; i < 16; ++i) {
        int hex_num = DISP[i];
        for (int j = 0; j < 32; ++j) {
            digit = 0x80000000 >> j;
            if ((hex_num & digit) != 0) {
                PRT[i][j] = '#';
            }

            else {
                PRT[i][j] = '.';
            }
        }
    }

    /* Reset stdout */
    printf("\x1b[2J");
    /* FOR DEBUG */
    printf("\rCOUNT=%d\nPOSITION=%d\nSTATE=%d\nREV_FLAG=%d\n", COUNT, POSITION, STATE, REV_FLAG);
    printf(
        "\r%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n%.32s\n",
        PRT[0], PRT[1], PRT[2], PRT[3], PRT[4], PRT[5], PRT[6], PRT[7], PRT[8], PRT[9], PRT[10], PRT[11], PRT[12], PRT[13], PRT[14], PRT[15]);

    return 0;
}
