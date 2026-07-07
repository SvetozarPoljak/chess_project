#ifndef DIALOG_HPP
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QTextEdit>
#include <vector>
#include "boardscanner.h"
#include <QMenu>
#include <QThread>
#include <QPushButton>
#include <QKeyEvent>
#include <unistd.h>
#include <signal.h>
#include "chess.hpp"

class Dialog : public QDialog {
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:     
    void update_chess_clocks();
    void moveMaker(const int *new_state);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    BoardScanner *scanner;
    QThread *scannerThread;
   // QHBoxLayout *mainLayout;
    QGridLayout *boardLayout;
   // QVBoxLayout *rightPanelLayout;
    QTextEdit *moveText;

    std::vector<std::vector<QLabel*>> guiBoard;
    
    int boardState[64];
    chess::Board board;
    chess::Move move;
    
    int fromSquare = -1;
    int toSquare = -1;
    bool illegal;
   
    int nextLegalFromSquare, nextLegalToSquare;
    bool exists, castlingSequenceInProgress;
 

    QTimer *scanningTimer;
    QTimer *chessClockTimer;
	
    int timeWhite;
    int timeBlack;
    bool whiteToMove;

    QLabel *whiteClockLabel;
    QLabel *blackClockLabel;
    QLabel *statusLabel; 
    QProgressBar *evalBar;
    QLabel *Clock;
    QLabel *whiteClock, *blackClock;

    int row;
    int col; 

    QString boardToQStringPiece(const chess::Board& board, int row, int col);
  
    void refreshField(int r, int c);
    void boardInit();
    void updateClockStyles(); 
};
#endif
