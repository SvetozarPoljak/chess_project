#include "dialog.h"
#include <iostream>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent),
      timeWhite(300), timeBlack(300), whiteToMove(true), 
      row(0), col(0)
{
    this->setFixedSize(650, 450);
    this->setWindowTitle("Sah Tabla 4x4");

    // GUI
    mainLayout = new QHBoxLayout(this);

    QPushButton *closeButton = new QPushButton("X", this);
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
    closeButton->setGeometry(750, 10, 40, 30);
    connect(closeButton, &QPushButton::clicked, this, [](){
        ::kill(::getpid(), SIGKILL);
    });    

    boardLayout = new QGridLayout();
    boardLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    boardLayout->setSpacing(0);
    boardLayout->setContentsMargins(0,0,0,0);
    mainLayout->addLayout(boardLayout);

    guiBoard.resize(2*BOARD_SIZE, std::vector<QLabel*>(2*BOARD_SIZE, nullptr));
    logicBoard.resize(2*BOARD_SIZE, std::vector<QString>(2*BOARD_SIZE, ""));

    boardInit();

    chessClockTimer = new QTimer(this);
    connect(chessClockTimer, &QTimer::timeout,
            this, &Dialog::update_chess_clocks);

    whiteClockLabel = new QLabel(this);
    blackClockLabel = new QLabel(this);
    statusLabel = new QLabel(this);

    // dodaj ih u layout, npr:
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->addWidget(whiteClockLabel);
    infoLayout->addWidget(blackClockLabel);
    infoLayout->addWidget(statusLabel);
    mainLayout->addLayout(infoLayout);

    evalBar = new QProgressBar(this);
    evalBar->setRange(0, 100);
    evalBar->setValue(50);
    evalBar->setOrientation(Qt::Vertical);
    mainLayout->addWidget(evalBar);

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
    if(row < 4){
	    if ((3 - this->row + this->col) % 2 == 0) {
	        guiBoard[3 - this->row][this->col]->setStyleSheet(
	            "background-color: #eeeed2; color: #769656; font-weight: bold;  ");
	    } else {
	        guiBoard[3 - this->row][this->col]->setStyleSheet(
	            "background-color: #769656; color: #eeeed2; font-weight: bold;  ");
	    }
    }else{

	    if ((7 - this->row + this->col) % 2 == 0) {
	        guiBoard[7 - this->row][this->col]->setStyleSheet(
	            "background-color: #eeeed2; color: #769656; font-weight: bold;  ");
	    } else {
	        guiBoard[7 - this->row][this->col]->setStyleSheet(
	            "background-color: #769656; color: #eeeed2; font-weight: bold;  ");
	    }
    }
    this->row = row;
    this->col = col;
    logicBoard[row][col] = field;
    refreshField(row, col); // reset pa highlight
    if(row < 4)
        guiBoard[3 - row][col]->setStyleSheet("background-color: yellow;  ");
    else
        guiBoard[7 - row][col]->setStyleSheet("background-color: yellow;  ");
    
    if (!figPickedUp) {
        whiteToMove = !whiteToMove;
    }

    int eval = 20 + (rand() % 60);
    evalBar->setValue(eval);
}

void Dialog::refreshField(int r, int c)
{
    if(row < 4){
	    if ((3 - this->row + this->col) % 2 == 0) {
	        guiBoard[3 - this->row][this->col]->setStyleSheet(
	            "background-color: #eeeed2; color: #769656; font-weight: bold;  ");
	    } else {
	        guiBoard[3 - this->row][this->col]->setStyleSheet(
	            "background-color: #769656; color: #eeeed2; font-weight: bold;  ");
	    }
            guiBoard[3 - r][c]->setText(logicBoard[r][c]);
    }else{

	    if ((7 - this->row + this->col) % 2 == 0) {
	        guiBoard[7 - this->row][this->col]->setStyleSheet(
	            "background-color: #eeeed2; color: #769656; font-weight: bold;  ");
	    } else {
	        guiBoard[7 - this->row][this->col]->setStyleSheet(
	            "background-color: #769656; color: #eeeed2; font-weight: bold;  ");
	    }
            guiBoard[7 - r][c]->setText(logicBoard[r][c]);
    }
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
        QString("BELI: %1:%2").arg(wMin, 2, 10, QChar('0')).arg(wSec, 2, 10, QChar('0')));
    blackClockLabel->setText(
        QString("CRNI: %1:%2").arg(bMin, 2, 10, QChar('0')).arg(bSec, 2, 10, QChar('0')));

    if (timeWhite == 0 || timeBlack == 0) {
        chessClockTimer->stop();
        statusLabel->setText("Timeout!");
    }

}

void Dialog::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Escape){
        ::kill(::getpid(), SIGKILL);
    }else{
        QDialog::keyPressEvent(event);
    }
}
