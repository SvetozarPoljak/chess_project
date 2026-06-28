#ifndef DIALOG_HPP
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <vector>
#include "boardscanner.h"
#include <QThread>
#include <QPushButton>
#include <QKeyEvent>
#include <unistd.h>
#include <signal.h>

class Dialog : public QDialog {
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:     
    void update_chess_clocks();
    void on_square_changed(int row, int col, QString field, bool figPickedUp); 

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    BoardScanner *scanner;
    QThread *scannerThread;
    QHBoxLayout *mainLayout;
    QGridLayout *boardLayout;
    QVBoxLayout *rightPanelLayout;

    std::vector<std::vector<QLabel*>> guiBoard;
    std::vector<std::vector<QString>> logicBoard; 

    QTimer *scanningTimer;
    QTimer *chessClockTimer;
	
    int timeWhite;
    int timeBlack;
    bool whiteToMove;

    QLabel *whiteClockLabel;
    QLabel *blackClockLabel;
    QLabel *statusLabel; 
    QProgressBar *evalBar;

    int row;
    int col;

    void refreshField(int r, int c);
    void boardInit(); 
};
#endif
