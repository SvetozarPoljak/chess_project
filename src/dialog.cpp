#include "dialog.h"
#include <iostream>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent),
      timeWhite(300), timeBlack(300), whiteToMove(true), 
      row(0), col(0)
{
    const int BOARD_X = 600;
    const int BOARD_Y = 200;
    const int BOARD_LEN = 80*8;

    this->setFixedSize(200+BOARD_LEN, 100+BOARD_LEN);
    this->setWindowTitle("Chess Board");
    // this->setStyleSheet("QDialog { background-color: #2c3e50; }");

    // GUI
    //mainLayout = new QHBoxLayout(this);
    QWidget *boardContainer = new QWidget(this);
    boardContainer->setGeometry(BOARD_X, BOARD_Y, BOARD_LEN, BOARD_LEN);

    QPushButton *closeButton = new QPushButton("X", this);
    closeButton->setFocusPolicy(Qt::NoFocus);
    closeButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #ff4d4d;"
        "  color: white;"
        "  font-weight: bold;"
        "  border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #ff1a1a;"
        "}"
    );
    closeButton->setGeometry(160+BOARD_X +BOARD_LEN, 10, 40, 30);
    connect(closeButton, &QPushButton::clicked, this, [](){
        ::kill(::getpid(), SIGKILL);
    });    

    boardLayout = new QGridLayout(boardContainer);
    boardLayout->setAlignment(Qt::AlignCenter);
    boardLayout->setSpacing(0);
    boardLayout->setContentsMargins(0,0,0,0);
    //mainLayout->addLayout(boardLayout);

    guiBoard.resize(2*BOARD_SIZE, std::vector<QLabel*>(2*BOARD_SIZE, nullptr));
    logicBoard.resize(2*BOARD_SIZE, std::vector<QString>(2*BOARD_SIZE, ""));

    boardInit();

    const int CLK_X = BOARD_X + BOARD_LEN + 30;
    const int CLK_Y = BOARD_Y + (BOARD_LEN/2);

    Clock = new QLabel(this);
    Clock->setGeometry(CLK_X, CLK_Y, 70, 85);
    Clock->setStyleSheet(
        "QWidget { "
        "  background-color: #aaaaaa; "
        "}"
    );
    whiteClock = new QLabel(Clock);
    whiteClock->setGeometry(5, 5, 60, 35);

    blackClock = new QLabel(Clock);
    blackClock->setGeometry(5, 45, 60, 35);
 
    updateClockStyles();

    whiteClockLabel = new QLabel(whiteClock);
    whiteClockLabel->setGeometry(0, 0, 60, 35);
    whiteClockLabel->setAlignment(Qt::AlignCenter);
    
    blackClockLabel = new QLabel(blackClock);
    blackClockLabel->setGeometry(0, 0, 60, 35);
    blackClockLabel->setAlignment(Qt::AlignCenter);


    chessClockTimer = new QTimer(this);
    connect(chessClockTimer, &QTimer::timeout,
            this, &Dialog::update_chess_clocks);

    statusLabel = new QLabel(this);
    statusLabel->setGeometry(CLK_X, CLK_Y + 100, 70, 40);
    statusLabel->setStyleSheet("color: black; font-weight: bold;");

    // dodaj ih u layout, npr:
   // QVBoxLayout *infoLayout = new QVBoxLayout();
   // infoLayout->addWidget(whiteClockLabel);
   // infoLayout->addWidget(blackClockLabel);
    //infoLayout->addWidget(statusLabel);
   // mainLayout->addLayout(infoLayout);

    evalBar = new QProgressBar(this);
//    evalBar->setGeometry()
    
    evalBar->setValue(50);
    evalBar->setOrientation(Qt::Vertical);
    evalBar->setGeometry(BOARD_X-30, BOARD_Y, 30, BOARD_LEN);
   // mainLayout->addWidget(evalBar);

    // Scanner thread
    scanner = new BoardScanner(nullptr);
    scannerThread = new QThread();

    scanner->moveToThread(scannerThread);

    connect(scannerThread, &QThread::started,
            scanner, &BoardScanner::process);

    connect(scanner, &BoardScanner::square_changed,
            this, &Dialog::on_square_changed,
            Qt::QueuedConnection);

    // tek onda pokreni timer
    chessClockTimer->start(1000);
    scannerThread->start();
}

Dialog::~Dialog()
{ 
    scanner->stop();
    scannerThread->quit();
    scannerThread->wait();

    delete scanner;
    delete scannerThread;

}

void Dialog::on_square_changed(int row, int col, QString field, bool figPickedUp)
{
    
//logicBoard[this->row][this->col] = field;
   /* QString figure_color;
    if(field == wP || field == wK || field == wQ || field == wB || field == wN || field == wR){
        figure_color = "white";
    }else{
        figure_color = "black";
    }
    //if(this->row < 4){
	    if ((7 - this->row + this->col) % 2 == 0) {
	        guiBoard[7 - this->row][this->col]->setStyleSheet(
	            "background-color: #eeeed2; color:" + figure_color + " #769656; font-weight: bold;  ");
	    } else {
	        guiBoard[7 - this->row][this->col]->setStyleSheet(
	            "background-color: #769656; color:" + figure_color + " #eeeed2; font-weight: bold;  ");
	    }
    //}else{
/*
	    if ((7 - this->row + this->col) % 2 == 0) {
	        guiBoard[4 + 7 - this->row][this->col]->setStyleSheet(
	            "background-color: #eeeed2; color: #769656; font-weight: bold;  ");
	    } else {
	        guiBoard[4 + 7 - this->row][this->col]->setStyleSheet(
	            "background-color: #769656; color: #eeeed2; font-weight: bold;  ");
	    }
    }*/
    refreshField(this->row, this->col);
    
    this->row = row;
    this->col = col;
    logicBoard[row][col] = field;
    //QString figure_color;
  //  if(field == wP || field == wK || field == wQ || field == wB || field == wN || field == wR){
    //    figure_color = "white";
   // }else{
     //   figure_color = "black";
  //  }
    refreshField(row, col); // reset pa highlight
    //if(row < 4)
       // guiBoard[3 - row][col]->setStyleSheet("background-color: yellow;  ");
   // else

    QString figure_color;
    if(field == wP || field == wK || field == wQ || field == wB || field == wN || field == wR){
        figure_color = "white";
    }else{
        figure_color = "black";
    }
    guiBoard[7 - row][col]->setStyleSheet(
        "QLabel { "
            "background-color: yellow; " 
            "color: " + figure_color + "; " 
            "font-weight: bold" + 
            "padding: 0px; margin: 0px; border: 0px;"
        "}"
    );
    
    //if (!figPickedUp) {
      //  whiteToMove = !whiteToMove;
   // }

    int eval = 20 + (rand() % 60);
    evalBar->setValue(eval);
}

void Dialog::refreshField(int r, int c)
{
    QString backgroundColor = ((7 - r + c) % 2 == 0) ? "#eeeed2" : "#769656";

    QString figure = logicBoard[r][c];
    QString figure_color, figure_style;
    
    if(figure == wP || figure == wK || figure == wQ || figure == wB || figure == wN || figure == wR){
        figure_color = "white";
    }else{
        figure_color = "black";
    }

    guiBoard[7 - r][c]->setStyleSheet(
        "QLabel { "
            "background-color: " + backgroundColor + ";" + 
            "color: " + figure_color + "; " + 
            "font-weight: bold" + 
            "padding: 0px; margin: 0px; border: 0px;"
        "}"
    );

    if(figure == wK || figure == bK)
        figure_style = QString::fromUtf8("\u265A");       
    else if(figure == wQ || figure == bQ)
        figure_style = QString::fromUtf8("\u265B");        
    else if(figure == wR || figure == bR)
        figure_style = QString::fromUtf8("\u265C");        
    else if(figure == wB || figure == bB)
        figure_style = QString::fromUtf8("\u265D");       
    else if(figure == wN || figure == bN)
        figure_style = QString::fromUtf8("\u265E");   
    else if(figure == wP || figure == bP)
        figure_style = QString::fromUtf8("\u265F");   
    else    
        figure_style = "";

    guiBoard[7 - r][c]->setText(figure_style);
}

void Dialog::boardInit()
{
    for (int row = 0; row < 2*BOARD_SIZE; ++row) {
        for (int col = 0; col < 2*BOARD_SIZE; ++col) {
            QLabel *field = new QLabel;

            field->setFixedSize(80, 80);
            field->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            field->setMargin(0); 
            field->setIndent(0);
            field->setAlignment(Qt::AlignCenter);
            
            field->setStyleSheet("QLabel { padding: 0px; margin: 0px; border: 0px; }");

            QFont f;
            f.setPixelSize(56);
            field->setFont(f);
            guiBoard[row][col] = field;
            boardLayout->addWidget(field, row, col);
            logicBoard[row][col] = "";
        }
    }

    logicBoard[1][0] = wP;
    logicBoard[1][1] = wP;
    logicBoard[1][2] = wP;
    logicBoard[1][3] = wP;

    logicBoard[0][0] = wR;
    logicBoard[0][1] = wN;
    logicBoard[0][2] = wB;
    logicBoard[0][3] = wQ;
    
    logicBoard[1][4] = wP;
    logicBoard[1][5] = wP;
    logicBoard[1][6] = wP;
    logicBoard[1][7] = wP;

    logicBoard[0][4] = wK;
    logicBoard[0][5] = wB;
    logicBoard[0][6] = wN;
    logicBoard[0][7] = wR;
           
    logicBoard[6][0] = bP;
    logicBoard[6][1] = bP;
    logicBoard[6][2] = bP;
    logicBoard[6][3] = bP;

    logicBoard[7][0] = bR;
    logicBoard[7][1] = bN;
    logicBoard[7][2] = bB;
    logicBoard[7][3] = bQ;
          
    logicBoard[6][4] = bP;
    logicBoard[6][5] = bP;
    logicBoard[6][6] = bP;
    logicBoard[6][7] = bP;

    logicBoard[7][4] = bK;
    logicBoard[7][5] = bB;
    logicBoard[7][6] = bN;
    logicBoard[7][7] = bR;
          
    for (int r = 0; r < 2*BOARD_SIZE; r++)
        for (int c = 0; c < 2*BOARD_SIZE; c++)
            refreshField(r, c);
}

void Dialog::update_chess_clocks()
{
    if (whiteToMove){
        if (timeWhite > 0) timeWhite--;
    } else {
        if (timeBlack > 0) timeBlack--;
    }

    int wMin = timeWhite / 60, wSec = timeWhite % 60;
    int bMin = timeBlack / 60, bSec = timeBlack % 60;

    whiteClockLabel->setText(
        QString("%1:%2").arg(wMin, 2, 10, QChar('0')).arg(wSec, 2, 10, QChar('0')));
    blackClockLabel->setText(
        QString("%1:%2").arg(bMin, 2, 10, QChar('0')).arg(bSec, 2, 10, QChar('0')));

    if (timeWhite == 0) {
        chessClockTimer->stop();
        statusLabel->setText("Timeout\n White!");
    }else if(timeBlack == 0){
        chessClockTimer->stop();
        statusLabel->setText("Timeout\n Black!");
    }
}

void Dialog::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Escape){
        ::kill(::getpid(), SIGKILL);
    }else if(event->key() == Qt::Key_Space){
        whiteToMove = !whiteToMove;
        updateClockStyles();
        //update_chess_clocks();
    }else{
        QDialog::keyPressEvent(event);
    }
}

void Dialog::updateClockStyles(){
    QString highlightedWhiteStyle = "border: 4px solid #000000; background-color: #ffffff; color: #000000;";
    QString highlightedBlackStyle = "border: 4px solid #ffffff; background-color: #000000; color: #ffffff;";
    QString defaultWhiteStyle = "border: 1px solid #000000; background-color: #888888; color: #000000;";
    QString defaultBlackStyle = "border: 1px solid #ffffff; background-color: #222222; color: #ffffff;";

    if(whiteToMove){
        whiteClock->setStyleSheet(highlightedWhiteStyle);
        blackClock->setStyleSheet(defaultBlackStyle);
    }else{
        whiteClock->setStyleSheet(defaultWhiteStyle);
        blackClock->setStyleSheet(highlightedBlackStyle);
    }
}
