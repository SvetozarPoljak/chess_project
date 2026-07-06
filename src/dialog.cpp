#include "dialog.h"
#include <iostream>
#include <string>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent),
      timeWhite(300), timeBlack(300), whiteToMove(true), 
      row(0), col(0), illegal(false),// moveInProgress(0),
      castlingSequenceInProgress(false), exists(false)
{
    const int BOARD_X = 600;
    const int BOARD_Y = 200;
    const int BOARD_LEN = 80*8;

    this->setFixedSize(200+BOARD_LEN, 100+BOARD_LEN);
    this->setWindowTitle("Chess Board");

    // GUI
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

    guiBoard.resize(2*BOARD_SIZE, std::vector<QLabel*>(2*BOARD_SIZE, nullptr));
   // logicBoard.resize(2*BOARD_SIZE, std::vector<QString>(2*BOARD_SIZE, ""));

    boardInit();

    const int CLK_X = BOARD_X + BOARD_LEN + 30;
    const int CLK_Y = BOARD_Y + (BOARD_LEN/2);

    moveText = new QTextEdit(this);
    moveText->setGeometry(CLK_X + 100, CLK_Y, 200, 300);
    moveText->setReadOnly(true);
    moveText->setLineWrapMode(QTextEdit::NoWrap);

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
    statusLabel->setGeometry(BOARD_X, BOARD_Y + BOARD_LEN, BOARD_LEN, 50);
    statusLabel->setStyleSheet("color: black; font-weight: bold;");

    evalBar = new QProgressBar(this);    
    evalBar->setValue(50);
    evalBar->setOrientation(Qt::Vertical);
    evalBar->setGeometry(BOARD_X-30, BOARD_Y, 30, BOARD_LEN);

    // Scanner thread
    scanner = new BoardScanner(nullptr);
    scannerThread = new QThread();

    scanner->moveToThread(scannerThread);

    connect(scannerThread, &QThread::started,
            scanner, &BoardScanner::process);
    /*
    connect(scanner, &BoardScanner::square_changed,
            this, &Dialog::on_square_changed,
            Qt::QueuedConnection);
   */
    connect(scanner, &BoardScanner::square_changed,
            this, &Dialog::moveMaker,
            Qt::QueuedConnection);

    for(int i = 0; i < 64; i++){
        if(i < 16 || i > 47){
            boardState[i] = 0;
        }else{
            boardState[i] = 1;
        }
    }

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

void Dialog::moveMaker(int square, int figPickedUp, std::array<int, 64> new_state)//(int row, int col, QString field, bool figPickedUp)
{
   /* for(int i = 0; i < 64; i++){
        boardState[i] = new_state[i];
        if(i % 8 == 0)
            statusLabel->setText("\n");
        statusLabel->setText(QString::fromStdString(std::to_string(new_state[i])));
        statusLabel->setText(" ");
    }*/
    
    if(!illegal){
            if(figPickedUp){
               /* if(!moveInProgress){
	            fromSquare[0] = square;
                    moveInProgress = 1;
                }else{
                    fromSquare[1] = square;
                }*/
                   // QString figure = boardToQStringPiece(board, fromSquare / 8, fromSquare % 8);
                   // on_square_changed(fromSquare / 8, fromSquare % 8, figure, true);
                   // break;
                fromSquare = square;
            }else{
                toSquare = square;
              /*  int moveFromSquare;
                if(fromSquare[1] != -1)
                    moveFromSquare = fromSquare[1];
                else if(fromSquare[0] != -1)
                    moveFromSquare = fromSquare[0];
                else
                    return;*/

               // if(moveFromSquare != toSquare){
                if(fromSquare != toSquare){ 
                    illegal = true;
                    
                    exists = false;
                    if(castlingSequenceInProgress == false){
                        chess::Movelist legal;
                        chess::movegen::legalmoves(legal, board);
                        chess::Square from(fromSquare);
                        chess::Square to(toSquare);
                        chess::Move candidate;
                        if((fromSquare == 4 || fromSquare == 60) && (toSquare == 6 || toSquare == 62))
                            candidate = chess::Move::make<chess::Move::CASTLING>(from, to);
                        else
                            candidate = chess::Move::make<chess::Move::NORMAL>(from, to);
                        for(const auto &m : legal){
                            if(m == candidate){
                                move = m;
                                exists = true;
                                std::cout<<"found!\n";
                                break;
                            }
                        }


                        std::cout<<"legal moves:\n";
                        for(const auto &m : legal){
                           // std::cout<<"---------------------------------\n";
                            std::cout << chess::uci::moveToUci(m) << " type=" << m.typeOf() << "\n";
                            std::cout<<" from = "<<m.from() <<"\n";
                            std::cout<<" to = "<<m.to() <<"\n";
                            std::cout<<"---------------------------------\n";
                        }

                        std::cout<<"played move: * "<<from<<" "<<to<<" *"<<std::endl;;
                        
                        std::cout<<"exists= "<<exists<<"\n";
                        if(exists){
                            std::cout<<"found move: "<<chess::uci::moveToUci(move) << std::endl;
                            std::cout<<"type: "<<int(move.typeOf())<<std::endl;}
                        if(move.typeOf() == chess::Move::CASTLING && exists){
                      /*      if(castlingSequenceInProgress){
                                castlingSequenceInProgress = false;
                            }else*/
                            std::cout<<"castling sequence is in progress\n";
                            castlingSequenceInProgress = true;

                            if(chess::uci::moveToUci(move) == "e1g1"){
                                nextLegalFromSquare = 7;
                                nextLegalToSquare = 5;
                            }else if(chess::uci::moveToUci(move) == "e1c1"){
                                nextLegalFromSquare = 0;
                                nextLegalToSquare = 3;
                            }else if(chess::uci::moveToUci(move) == "e8g8"){
                                nextLegalFromSquare = 63;
                                nextLegalToSquare = 61;
                            }else if(chess::uci::moveToUci(move) == "e8c8"){
                                nextLegalFromSquare = 56;
                                nextLegalToSquare = 59;
                            }
                        }

                    }else{
                        if(nextLegalFromSquare != fromSquare || nextLegalToSquare != toSquare){
                            exists = false;
                           // illegal = true;
                        }else{
                            exists = true;
                           // illegal = false;
                            castlingSequenceInProgress = false;
                        }
                    }

                    if(exists){
                        std::cout<<"played move exists\n";
                        illegal = false;
                        if(castlingSequenceInProgress == false){
                            statusLabel->clear();
                            std::string uci_mv_s = chess::uci::moveToUci(move); 
                            moveText->append(QString::fromStdString(uci_mv_s));
                            QTextCursor c = moveText->textCursor();
                            c.movePosition(QTextCursor::End);
                            moveText->setTextCursor(c);   
                    
                            board.makeMove(move);
                           // updateGuiBoard();
                            
                             
                            whiteToMove = !whiteToMove;
                            updateClockStyles();
                        }
                        for(int i = 0; i < 64; i++)
                            refreshField(i/8, i%8);

                       // chess::Move move_text_debug = chess::Move::make<chess::Move::NORMAL>(moveFromSquare, toSquare);
                      /*  if(castlingSequenceInProgress){
                            statusLabel->setText("Castling\n");
                            statusLabel->setText(QString::fromStdString(chess::uci::moveToUci(move_text_debug)));
                        }else{
                            statusLabel->setText("No Castling\n");
                            statusLabel->setText(QString::fromStdString(chess::uci::moveToUci(move_text_debug)));
                        }*/
                    }else{
                        statusLabel->setStyleSheet("color: red; font-weight: bold;");
                        if(move.typeOf() == chess::Move::CASTLING && castlingSequenceInProgress){
                            if(chess::uci::moveToUci(move) == "e1g1"){
                                statusLabel->setText("Illegal action! White kingside castling is in the progress.\n Only legal action is placing rook on h1 to f1.");
                            }else if(chess::uci::moveToUci(move) == "e1c1"){
                                statusLabel->setText("Illegal action! White queenside castling is in the progress.\n Only legal action is placing rook on a1 to d1.");
                            }else if(chess::uci::moveToUci(move) == "e8g8"){
                                statusLabel->setText("Illegal action! Black kingside castling is in the progress.\n Only legal action is placing rook on h8 to f8.");
                            }else if(chess::uci::moveToUci(move) == "e8c8"){
                                statusLabel->setText("Illegal action! Black queenside castling is in the progress.\n Only legal action is placing rook on a8 to d8.");
                            }
                        }else{
                            statusLabel->setText("Illegal move! Undo move on the board and make legal move.");
                        }
                        illegal = true;
                    }   
                }/*
                moveInProgress = 0;
                fromSquare[0] = -1;
                fromSquare[1] = -1;*/
            }
        //} 
    }else{
        int occupied;
        bool check_pass = true;
        for(int sq = 0; sq < 64; sq++){
            chess::Piece piece = board.at(chess::Square(sq));
            
            if(castlingSequenceInProgress){
                if(chess::uci::moveToUci(move) == "e1g1"){
                    if(sq == 4)
                        piece = chess::Piece::NONE;
                    else if(sq == 6)
                        piece = chess::Piece::WHITEKING;
                }else if(chess::uci::moveToUci(move) == "e1c1"){
                    if(sq == 4)
                        piece = chess::Piece::NONE;
                    else if(sq == 2)
                        piece = chess::Piece::WHITEKING;
                }else if(chess::uci::moveToUci(move) == "e8g8"){
                   if(sq == 60)
                        piece = chess::Piece::NONE;
                    else if(sq == 62)
                        piece = chess::Piece::BLACKKING;
                }else if(chess::uci::moveToUci(move) == "e8c8"){
                    if(sq == 60)
                        piece = chess::Piece::NONE;
                    else if(sq == 58)
                        piece = chess::Piece::BLACKKING;
                }
            }
            if(piece != chess::Piece::NONE) occupied = 0;
            else occupied = 1;
            for(int i = 0; i < 8; i++){
                for(int j = 0; j < 8 ; j++){
                    std::cout<<new_state[i*8+j]<<" ";
                }
                std::cout<<std::endl;
            }
            std::cout<<"====="<<std::endl;
            std::cout<<"debug"<<std::endl;
            std::cout<<"====="<<std::endl;
            if(new_state[sq] != occupied){
                check_pass = false;
               // scanner->blockSignals(true);
               // break;
                
            }
        }
        if(check_pass) illegal = false;
        else illegal = true;
         
        if(!illegal){
            scanner->blockSignals(false);
            statusLabel->clear();
            statusLabel->setStyleSheet("color: black; font-weight: bold;");
            statusLabel->setText("Now make a legal move.");
        }else if(!castlingSequenceInProgress){
            statusLabel->clear();
            statusLabel->setStyleSheet("color: red; font-weight: bold;");
            statusLabel->setText("Illegal move! Set table as depicted on the monitor.\nThat is last legal position.");
        }
    }
   /* if(!illegal){
        for(int i = 0; i < 64; i++)
            refreshField(i/8, i%8);*/
   // }
  
}

QString Dialog::boardToQStringPiece(const chess::Board& board, int row, int col)
{
    QString pieceString = "";
    int sq = 8 * row + col;
    chess::Piece piece = board.at(chess::Square(sq));
    
    if(castlingSequenceInProgress){
        if(chess::uci::moveToUci(move) == "e1g1"){
            if(sq == 4)
                 piece = chess::Piece::NONE;
            else if(sq == 6)
                 piece = chess::Piece::WHITEKING;
        }else if(chess::uci::moveToUci(move) == "e1c1"){
            if(sq == 4)
                 piece = chess::Piece::NONE;
            else if(sq == 2)
                 piece = chess::Piece::WHITEKING;
        }else if(chess::uci::moveToUci(move) == "e8g8"){
            if(sq == 60)
                 piece = chess::Piece::NONE;
            else if(sq == 62)
                 piece = chess::Piece::BLACKKING;
        }else if(chess::uci::moveToUci(move) == "e8c8"){
            if(sq == 60)
                 piece = chess::Piece::NONE;
            else if(sq == 58)
                 piece = chess::Piece::BLACKKING;
        }
    }


    if(piece == chess::Piece::NONE){
        pieceString = "";
    }else{
        QString color = (piece.color() == chess::Color::WHITE) ? "w" : "b";

        switch(piece.type().internal()){
            case chess::PieceType::underlying::PAWN:
                pieceString = color + "P";
                break;

            case chess::PieceType::underlying::KNIGHT:
                pieceString = color + "N";
                break;

            case chess::PieceType::underlying::BISHOP:
                pieceString = color + "B";
                break;

            case chess::PieceType::underlying::ROOK:
                pieceString = color + "R";
                break;

            case chess::PieceType::underlying::QUEEN:
                pieceString = color + "Q";
                break;

            case chess::PieceType::underlying::KING:
                pieceString = color + "K";
                break;

            default:
                pieceString = "";
                break;
        }
    }
    return pieceString;
}

void Dialog::on_square_changed(int row, int col, QString field, bool figPickedUp)
{  

    refreshField(this->row, this->col);
    this->row = row;
    this->col = col;

   // logicBoard[row][col] = field;
    refreshField(row, col); // reset pa highlight    

    QString figure_color;
    if(field == wP || field == wK || field == wQ || field == wB || field == wN || field == wR){
        figure_color = "white";
    }else{
        figure_color = "black";
    }
    if(!illegal){
        guiBoard[7 - row][col]->setStyleSheet(
            "background-color: green; "
            "color: " + figure_color + "; "
            "font-weight: bold; "
            "padding: 0px; margin: 0px; border: 0px;"
        );
    }else{
        guiBoard[7 - row][col]->setStyleSheet(
            "background-color: red; "
            "color: " + figure_color + "; "
            "font-weight: bold; "
            "padding: 0px; margin: 0px; border: 0px;"
        );
    }
   /* lastHighlightedFigure = figure;
    this->row = toSquareRow;
    this->col = toSquareCol;
    /*
    int eval = 20 + (rand() % 60);
    evalBar->setValue(eval);**/
}

void Dialog::refreshField(int r, int c)
{
    QString backgroundColor = ((7 - r + c) % 2 == 0) ? "#a0a0a0" : "#505050"; //"#eeeed2" : "#769656";

    QString figure = boardToQStringPiece(board, r, c); // 
    //QString figure = logicBoard[r][c];
    QString figure_color, figure_shape;
   // if(boardState[8*r + c]) figure = "";
    
    if(figure == "wP" || figure == "wK" || figure == "wQ" || figure == "wB" || figure == "wN" || figure == "wR"){
        figure_color = "white";
    }else{
        figure_color = "black";
    }

    guiBoard[7 - r][c]->setStyleSheet(
        "background-color: " + backgroundColor + "; "
        "color: " + figure_color + "; "
        "font-weight: bold; padding: 0px; margin: 0px; border: 0px;"
    );

    if(figure == "wK" || figure == "bK")
        figure_shape = QString::fromUtf8("\u265A");       
    else if(figure == "wQ" || figure == "bQ")
        figure_shape = QString::fromUtf8("\u265B");        
    else if(figure == "wR" || figure == "bR")
        figure_shape = QString::fromUtf8("\u265C");        
    else if(figure == "wB" || figure == "bB")
        figure_shape = QString::fromUtf8("\u265D");       
    else if(figure == "wN" || figure == "bN")
        figure_shape = QString::fromUtf8("\u265E");   
    else if(figure == "wP" || figure == "bP")
        figure_shape = QString::fromUtf8("\u265F");   
    else    
        figure_shape = "";

    guiBoard[7 - r][c]->setText(figure_shape);
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
            
            field->setStyleSheet("padding: 0px; margin: 0px; border: 0px;");

            QFont f;
            f.setPixelSize(56);
            field->setFont(f);
            guiBoard[row][col] = field;
            boardLayout->addWidget(field, row, col);
           // logicBoard[row][col] = "";
        }
    }

   /* logicBoard[1][0] = wP;
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
            refreshField(r, c);*/
    for(int i = 0; i < 64; i++)
        refreshField(i/8, i%8);
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
        statusLabel->setStyleSheet("color: black; font-weight: bold;");
        statusLabel->setText("Timeout\n White!");
    }else if(timeBlack == 0){
        chessClockTimer->stop();
        statusLabel->setStyleSheet("color: black; font-weight: bold;");
        statusLabel->setText("Timeout\n Black!");
    }
}

void Dialog::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Escape){
        ::kill(::getpid(), SIGKILL);
    }/*else if(event->key() == Qt::Key_Space){
        whiteToMove = !whiteToMove;
        updateClockStyles();
        //update_chess_clocks();
    }*/else{
        QDialog::keyPressEvent(event);
    }
}

void Dialog::updateClockStyles(){
    QString highlightedWhiteStyle = "border: 4px solid #000000; background-color: #ffffff; color: #000000;";
    QString highlightedBlackStyle = "border: 4px solid #ffffff; background-color: #000000; color: #ffffff;";
    QString defaultWhiteStyle = "border: 1px solid #000000; background-color: #666666; color: #000000;";
    QString defaultBlackStyle = "border: 1px solid #ffffff; background-color: #222222; color: #ffffff;";

    if(whiteToMove){
        whiteClock->setStyleSheet(highlightedWhiteStyle);
        blackClock->setStyleSheet(defaultBlackStyle);
    }else{
        whiteClock->setStyleSheet(defaultWhiteStyle);
        blackClock->setStyleSheet(highlightedBlackStyle);
    }
}
