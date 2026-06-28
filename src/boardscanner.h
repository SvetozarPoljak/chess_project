#ifndef BOARDSCANNER_H
#define BOARDSCANNER_H

#include <QObject>
#include <vector>
#include <atomic>
#include <QVector>

// figures
const QString wK = QString::fromUtf8("\u2654");
const QString wQ = QString::fromUtf8("\u2655");
const QString wR = QString::fromUtf8("\u2656");
const QString wB = QString::fromUtf8("\u2657");
const QString wN = QString::fromUtf8("\u2658");
const QString wP = QString::fromUtf8("\u2659");

const QString bK = QString::fromUtf8("\u265A");
const QString bQ = QString::fromUtf8("\u265B");
const QString bR = QString::fromUtf8("\u265C");
const QString bB = QString::fromUtf8("\u265D");
const QString bN = QString::fromUtf8("\u265E");
const QString bP = QString::fromUtf8("\u265F");
const int BOARD_SIZE = 4;

class BoardScanner : public QObject
{
    Q_OBJECT

public:
    explicit BoardScanner(QObject *parent = nullptr);
    void stop();
    QVector<QVector<QString>> board;

signals:
    void square_changed(int row, int col, QString field, bool figPickedUp);

public slots:
    void process();

private:
    void scanBoard();
    void dec2bin(int n);

private:
    std::atomic<bool> running;

    int mux_sel_1[4] = {5, 6, 13, 19}; 
    int mux_out_1 = 26;

    int mux_sel_2[4] = {12, 16, 20, 21}; 
    int mux_out_2 = 25;

    int mux_sel_3[4] = {2, 3, 4, 17};
    int mux_out_3 = 27;
 
    int mux_sel_4[4] = {14, 15, 18, 23};
    int mux_out_4 = 24;

    QString field;
	
    int old_state[64];
    int new_state[64];

    std::vector<unsigned char> nbin;
};

#endif
