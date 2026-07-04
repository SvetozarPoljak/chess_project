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
    void moveMaker(int square, int figPickedUp, const int *new_state);//(int row, int col, QString field, bool figPickedUp);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    BoardScanner *scanner;
    QThread *scannerThread;
    QHBoxLayout *mainLayout;
    QGridLayout *boardLayout;
    QVBoxLayout *rightPanelLayout;
    QTextEdit *moveText;

    std::vector<std::vector<QLabel*>> guiBoard;
    std::vector<std::vector<QString>> logicBoard; 

    chess::Board board;
    chess::Move move;
    std::vector<chess::Move> legal_played_moves;
    std::vector<chess::Move> all_played_moves;
    int fromSquare[2] = {-1, -1};
    int toSquare = -1;
    bool illegal;
    int moveInProgress;
    int nextLegalFromSquare, nextLegalToSquare;
    bool exists, castlingSequenceInProgress;
   // QString lastHighlightedFigure;

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
    void on_square_changed(int row, int col, QString field, bool figPickedUp);
    void refreshField(int r, int c);
    void boardInit();
    void updateClockStyles(); 
};
#endif
