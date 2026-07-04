#include "boardscanner.h"
#include <bcm2835.h>
#include <QThread>
#include <iostream>

BoardScanner::BoardScanner(QObject *parent)
    : QObject(parent), running(true)
{

    if (!bcm2835_init()) {
        std::cerr << "bcm init fail\n";
    }

    for (int i = 0; i < 4; i++)
        bcm2835_gpio_fsel(mux_sel_1[i], BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(mux_out_1, BCM2835_GPIO_FSEL_INPT);

    for (int i = 0; i < 4; i++)
        bcm2835_gpio_fsel(mux_sel_2[i], BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(mux_out_2, BCM2835_GPIO_FSEL_INPT);

    for (int i = 0; i < 4; i++)
        bcm2835_gpio_fsel(mux_sel_3[i], BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(mux_out_3, BCM2835_GPIO_FSEL_INPT);

    for (int i = 0; i < 4; i++)
        bcm2835_gpio_fsel(mux_sel_4[i], BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(mux_out_4, BCM2835_GPIO_FSEL_INPT);

/*    for (int i = 0; i < 32; i++) {

        int num;
        if(i < 16)
            num = i;
        else
            num = i - 16;

        dec2bin(num);

        if(i < 16){
            for (int j = 0; j < 4; j++) {
                bcm2835_gpio_write(mux_sel_1[j], nbin[j]);
            }
        }else{
            for (int j = 0; j < 4; j++) {
                bcm2835_gpio_write(mux_sel_2[j], nbin[j]);
            }
        }

        bcm2835_delay(5);
        
        int temp; 
        if(i < 16)
            temp = bcm2835_gpio_lev(mux_out_1);
        else
            temp = bcm2835_gpio_lev(mux_out_2);
        
        new_state[i] = temp;
        old_state[i] = temp;
    }
    
    for (int i = 63; i > 31; i--) {

        int num;
        if(63 - i < 16)
            num = 63 - i;
        else
            num = (63 - i) - 16;

        dec2bin(num);

        if(63 - i < 16){
            for (int j = 0; j < 4; j++) {
                bcm2835_gpio_write(mux_sel_3[j], nbin[j]);
            }
        }else{
            for (int j = 0; j < 4; j++) {
                bcm2835_gpio_write(mux_sel_4[j], nbin[j]);
            }
        }

        bcm2835_delay(5);
        
        int temp; 
        if(63 - i < 16)
            temp = bcm2835_gpio_lev(mux_out_3);
        else
            temp = bcm2835_gpio_lev(mux_out_4);
        
        new_state[i] = temp;
        old_state[i] = temp;
    }*/
    for(int i = 0; i < 64; i++){
        if(i > 15 && i < 48)
            old_state[i] = 1;
        else
            old_state[i] = 0;
    }

    board.resize(2*BOARD_SIZE);

    for (int i = 0; i < 2*BOARD_SIZE; i++) {
        board[i].resize(2*BOARD_SIZE);
    }
  
    for (int row = 0; row < 2*BOARD_SIZE; ++row) {
        for (int col = 0; col < 2*BOARD_SIZE; ++col) {
            board[row][col] = "";
        }
    }

    board[1][0] = wP;
    board[1][1] = wP;
    board[1][2] = wP;
    board[1][3] = wP;

    board[0][0] = wR;
    board[0][1] = wN;
    board[0][2] = wB;
    board[0][3] = wQ;

    board[1][4] = wP;
    board[1][5] = wP;
    board[1][6] = wP;
    board[1][7] = wP;

    board[0][4] = wK;
    board[0][5] = wB;
    board[0][6] = wN;
    board[0][7] = wR;

    board[6][0] = bP;
    board[6][1] = bP;
    board[6][2] = bP;
    board[6][3] = bP;

    board[7][0] = bR;
    board[7][1] = bN;
    board[7][2] = bB;
    board[7][3] = bQ;

    board[6][4] = bP;
    board[6][5] = bP;
    board[6][6] = bP;
    board[6][7] = bP;

    board[7][4] = bK;
    board[7][5] = bB;
    board[7][6] = bN;
    board[7][7] = bR;
}

void BoardScanner::stop()
{
    running = false;
    bcm2835_close();
}

void BoardScanner::process()
{
    while (running) {
        scanBoard();
        QThread::msleep(50);
    }
}

void BoardScanner::dec2bin(int n)
{
    nbin.assign(4, 0);

    int i = 0;
    while (n && i < 4) {
        nbin[i++] = n % 2;
        n /= 2;
    }
}

void BoardScanner::scanBoard()
{

    /*
    for (int i = 0; i < 32; i++) {

        int num;
        if(i < 16)
            num = i;
        else
            num = i - 16;

        dec2bin(num);

        if(i < 16){
            for (int j = 0; j < 4; j++) {
                bcm2835_gpio_write(mux_sel_1[j], nbin[j]);
            }
        }else{
            for (int j = 0; j < 4; j++) {
                bcm2835_gpio_write(mux_sel_2[j], nbin[j]);
            }
        }

        bcm2835_delay(5);
        
        int temp; 
        if(i < 16)
            temp = bcm2835_gpio_lev(mux_out_1);
        else
            temp = bcm2835_gpio_lev(mux_out_2);
        
        new_state[i] = temp;
    }
    
    for (int i = 63; i > 31; i--) {

        int num;
        if(63 - i < 16)
            num = 63 - i;
        else
            num = (63 - i) - 16;

        dec2bin(num);

        if(63 - i < 16){
            for (int j = 0; j < 4; j++) {
                bcm2835_gpio_write(mux_sel_3[j], nbin[j]);
            }
        }else{
            for (int j = 0; j < 4; j++) {
                bcm2835_gpio_write(mux_sel_4[j], nbin[j]);
            }
        }

        bcm2835_delay(5);
        
        int temp; 
        if(63 - i < 16)
            temp = bcm2835_gpio_lev(mux_out_3);
        else
            temp = bcm2835_gpio_lev(mux_out_4);
        
        new_state[i] = temp;
    }
*/
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 16; j++){
            int temp, idx;
            dec2bin(j);

            if(i == 0){
                for (int k = 0; k < 4; k++) {
                    bcm2835_gpio_write(mux_sel_1[k], nbin[k]);
                }
                bcm2835_delay(5);
                temp = bcm2835_gpio_lev(mux_out_1);
                idx = (j/4) + (j%4)*8;
            }else if(i == 1){ 
                for (int k = 0; k < 4; k++) {
                    bcm2835_gpio_write(mux_sel_2[k], nbin[k]);
                }
                bcm2835_delay(5);
                temp = bcm2835_gpio_lev(mux_out_2);
                idx = ((j/4)+4) + (j%4)*8;
            }else if(i == 2){
                for (int k = 0; k < 4; k++) {
                    bcm2835_gpio_write(mux_sel_3[k], nbin[k]);
                }                
                bcm2835_delay(5);
                temp = bcm2835_gpio_lev(mux_out_3);
                idx = ((15-j)/4) + (((15-j)%4)+4)*8;
            }else{
                for (int k = 0; k < 4; k++) {
                    bcm2835_gpio_write(mux_sel_4[k], nbin[k]);
                }                
                bcm2835_delay(5);
                temp = bcm2835_gpio_lev(mux_out_4);
                idx = (((15-j)/4) + 4) + (((15-j)%4)+4)*8;
            }
            new_state[idx] = temp;
        }
    }

    bool board_changed = false;
    for (int i = 0; i < 64; i++) {
      /*  int row, col;
        bool figPickedUp;
        row = i / 8;
        col = i % 8;
       */
        if (old_state[i] != new_state[i]) {
          /*  if(new_state[i]){
                field = board[row][col];
                board[row][col] = "";
                figPickedUp = true;
                emit square_changed(row, col, "", figPickedUp);
            }else{
                board[row][col] = field;
                figPickedUp = false;
                emit square_changed(row, col, field, figPickedUp);
            }*/
            board_changed = true;           
        }
        old_state[i] = new_state[i];
    }

    if(board_changed){
        emit boardChanged(new_state);
    }
/*
    for (int i = 0; i < 32; i++) {
        int row, col;
        bool figPickedUp;
        row = i % 4;
        col = i / 4;
        if (old_state[i] != new_state[i]) {
            if(new_state[i]){
                field = board[row][col];
                board[row][col] = "";
                figPickedUp = true;
                emit square_changed(row, col, "", figPickedUp);
            }else{
                board[row][col] = field;
                figPickedUp = false;
                emit square_changed(row, col, field, figPickedUp);
            }           
        }
        old_state[i] = new_state[i];
    }

    for (int i = 0; i < 32; i++) {
        int row, col;
        bool figPickedUp;
        row = 4 + (i % 4);
        col = i / 4;
        if (old_state[32 + i] != new_state[32 + i]) {
            if(new_state[32 + i]){
                field = board[row][col];
                board[row][col] = "";
                figPickedUp = true;
                emit square_changed(row, col, "", figPickedUp);
            }else{
                board[row][col] = field;
                figPickedUp = false;
                emit square_changed(row, col, field, figPickedUp);
            }           
        }
        old_state[32 + i] = new_state[32 + i];
    }*/
}
