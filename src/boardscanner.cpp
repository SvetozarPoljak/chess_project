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

    for(int i = 0; i < 64; i++){
        if(i > 15 && i < 47)
            old_state[i] = 1;
        else
            old_state[i] = 0;
    }
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
    std::array<int, 64> snapshot;
    for (int i = 0; i < 64; i++) {
        if (old_state[i] != new_state[i]) {
            for(int i = 0; i<64;i++)
                snapshot[i] = new_state[i];
            emit square_changed(i, new_state[i], snapshot);           
        }
        old_state[i] = new_state[i];
    }
}
