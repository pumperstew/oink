// Based on the example Winboard protocol driver by H.G.Muller
// http://www.open-aurec.com/wbforum/viewtopic.php?f=24&t=51739

#include <engine/Position.hpp>
#include <engine/Search.hpp>
#include <display/ConsoleDisplay.hpp>
#include <fen_parser/FenParser.hpp>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <chrono>

using namespace chess;
using namespace std;

static const int MAX_GAME_MOVES   = 500;
static const int MAX_SEARCH_DEPTH = 60;

// If "megabytes" is different from last time, resize all tables to make memory usage below "megabytes"
static void set_memory_size(int megabytes)
{
    return;
}        

// Search current position for stm, deepening forever until there is input.
//void ponder_util_input(Side side_to_move);

struct time_control_info
{
    int moves_per_session;
    int start_time; // TODO: not properly initialized.
    int time_control;
    int time_per_move;
    int never_exceed_limit;
    int no_new_move_limit;
    int no_new_iteration_limit; // used by Search()
    int time_left_millis;
};

class StopWatch
{
    chrono::high_resolution_clock::time_point start;
public:
    StopWatch()
    {
        start = chrono::high_resolution_clock::now();
    }
    
    chrono::milliseconds::rep elapsed_ms() const
    {
        auto elapsed = chrono::high_resolution_clock::now() - start;
        return chrono::duration_cast<chrono::milliseconds>(elapsed).count();
    }
};

Move parse_move(const Position &pos, const string &move_str)
{
    RankFile source_file = move_str[0] - 'a';
    RankFile source_rank = move_str[1] - '1';
    RankFile dest_file   = move_str[2] - 'a';
    RankFile dest_rank   = move_str[3] - '1';
    Move move;
    if (move_str.length() > 4)
    {
        Side side = (dest_rank == 0 ? sides::black : sides::white);

        if (move_str[4] == 'b')
            move.set_promotion_piece(pieces::BISHOPS[side]);
        else if (move_str[4] == 'n')
            move.set_promotion_piece(pieces::KNIGHTS[side]);
        else if (move_str[4] == 'r')
            move.set_promotion_piece(pieces::ROOKS[side]);
        else if (move_str[4] == 'q')
            move.set_promotion_piece(pieces::QUEENS[side]);
    }
    
    Square source_sq = rank_file_to_square(source_rank, source_file);
    Square dest_sq = rank_file_to_square(dest_rank, dest_file);
    move.set_source(source_sq);
    move.set_destination(dest_sq);
    move.set_piece(pos.squares[source_sq]);
    move.set_captured_piece(pos.squares[dest_sq]);

    if ((move.get_piece() == pieces::WHITE_PAWN || move.get_piece() == pieces::BLACK_PAWN) &&
        ((abs(source_sq - dest_sq) % 8) != 0) && (pos.squares[dest_sq] == pieces::NONE))
    {
        move.set_en_passant(pieces::PAWNS[get_piece_side(move.get_piece())]);
    }
    else if (move.get_piece() == pieces::WHITE_KING && ((source_sq == squares::e1 && dest_sq == squares::g1) || (source_sq == squares::e1 && dest_sq == squares::c1)))
    {
        move.set_castling(pieces::WHITE_KING);
    }
    else if (move.get_piece() == pieces::BLACK_KING && ((source_sq == squares::e8 && dest_sq == squares::g8) || (source_sq == squares::e8 && dest_sq == squares::c8)))
    {
        move.set_castling(pieces::BLACK_KING);
    }

    return move;
}

PosEvaluation search_best_move(const Position &pos, Side side_to_move, Move *move, Move *ponder_move)
{
    MoveAndEval result = alpha_beta(side_to_move, pos, 5, -2*evals::MATE_SCORE, 2*evals::MATE_SCORE);
    *move = result.best_move;
    return result.best_eval;
}

static const int MOVES_TO_GO_GUESS = 40;

static void set_time_limits(time_control_info &time_control_info, int move_number, int millis_left)
{
    int moves_to_go = time_control_info.moves_per_session - move_number / 2;

    if (time_control_info.time_per_move > 0)
        moves_to_go = 1;                 // In maximum-time-per-move mode, the time left is always for one move
    else if (time_control_info.moves_per_session == 0)
        moves_to_go = MOVES_TO_GO_GUESS; // Guess how many moves we still must do
    else
    {
        while (moves_to_go <= 0)
            moves_to_go +=  time_control_info.moves_per_session; // Calculate moves before next time control (TODO: session lengths?) 
    }

    millis_left -= 20; // keep absolute safety margin of 20 msec

    time_control_info.never_exceed_limit     = 10*millis_left        / (moves_to_go + 9);
    time_control_info.no_new_move_limit      = (int)(1.5*millis_left / (moves_to_go + 0.5));
    time_control_info.no_new_iteration_limit = (int)(0.5*millis_left / moves_to_go);
}

static void read_line(char input_buffer[80])
{
    int i, c;
    for (i = 0; (input_buffer[i] = c = getchar()) != '\n'; i++)
    {
        if (c == EOF || i > 77)
            exit(0);
    }
    input_buffer[i+1] = 0;
}

static bool peek_at_input(char input_buffer[80], char command[80], char ponder_move_text[20], int move_number, 
                          time_control_info &time_control_info, bool *pondering, const StopWatch &clock, bool root)
{
    while(1)
    {
        read_line(input_buffer);
        sscanf(input_buffer, "%s", command);

        if (!strcmp(command, "otim"))
        {
            // Do not resume pondering after receiving time commands, as move will follow immediately
            continue;
        }

        if (!strcmp(command, "time"))
        { 
            sscanf(input_buffer, "time %d", &time_control_info.time_left_millis);
            time_control_info.time_left_millis *= 10; // "time" sends value in hundrendths of a second.
            continue;
        }

        if (!strcmp(command, "."))
        { 
            // "ignore for now": wut?
            input_buffer[0] = 0; 
            return false; 
        } 

        if (!root && !strcmp(command, "usermove"))
        {
            if (!strcmp(input_buffer + 9, ponder_move_text))
            {
                // ponder hit
                input_buffer[0] = 0; // eat away command, as we will process it here
                *pondering = false;
                set_time_limits(time_control_info, move_number, time_control_info.time_left_millis + clock.elapsed_ms()); // turn into time-based search
                return false; // do not abort
            }
        }
        return true; // other commands (or ponder miss) abort search
    }
}

//int InputWaiting()
//{
//#ifdef WIN32
//    DWORD cnt;
//    static HANDLE hInp = NULL;
//    if(hInp == NULL) hInp = GetStdHandle(STD_INPUT_HANDLE);
//    return !PeekNamedPipe(hInp, NULL, 0, NULL, &cnt, NULL) || cnt > 0;
//#else
//    static fd_set rset;
//    static struct timeval timeout = {0, 0};
//    FD_ZERO(&rset);
//    FD_SET(0, &rset);
//    if(select(1, &rset, NULL, NULL, &timeout) < 0) printf("error X\n");
//    if (FD_ISSET(0, &rset))   return 1;
//#endif
//    return 0;
//}

//bool TerminalCondition()
//{
//    if (pondering)
//    { 
//        // only new input can cause abort
//        if(InputWaiting()) return peek_at_input(false); // but only abort if the input requires us
//    }
//    else 
//    { 
//        // when thinking only time can cause abort
//        if (TimeUsed() > neverExceedLimit) 
//            return true;
//    }
//    return false;
//}

//#ifdef EXAMPLE
//// these are only examples for where and how you should test for time-control enforcement in your search
//
//int rootScore;
//MOVE rootMove;
//
//int Search()
//{
//    nodeCount++;
//    if((nodeCount & 0x3FF) == 0 && TerminalCondition()) abortFlag = TRUE; // test if we should abort (not too often)
//    ...
//    for(ALL_MOVES) {
//        MakeMove();
//        score = -Search(...);
//        UnMake();
//        if(abortFlag) return 0; // here we put an abort to make the search unwind
//        ...
//    }
//    }
//}
//
//#define MARGIN 25
//
//void RootSearch()
//{
//    int depth, score, bestScore; MOVE rootMove
//    startTime = ReadClock();
//    GenerateMoves();
//    for(depth=1; depth<=max_depth && (pondering || TimeUsed()<noNewIterationLimit); depth++) { // iterative deepening loop
//    bestScore = -INFINITY;
//    for(ALL_MOVES) {
//        MakeMove();
//        score = -Search(...);
//        UnMake();
//        if(abortFlag) return;
//        // handle score, etc.
//        ...
//        if(!pondering && TimeUsed() > noNewMoveLimit && bestScore > rootScore - MARGIN) break; // extra time for fail low
//    }
//    // end of iteration
//    ...
//    rootMove = bestMove;
//    rootScore = bestScore;
//    }
//}
//#endif

static Position take_back(string last_fen, Move* game_history, int cur_move_number, int how_many, Side *new_side_to_move, int *new_move_number)
{
    // Reset the game and then replay it to the desired point
    Side side_to_move;
    Position pos = fen::parse_fen(last_fen, nullptr, &side_to_move);

    int last = cur_move_number - how_many;
    if (last < 0) last = 0;

    for (cur_move_number = 0; cur_move_number < last; cur_move_number++) 
    {
        pos.make_move(game_history[cur_move_number]);
        side_to_move = swap_side(side_to_move);
    }

    *new_move_number  = cur_move_number;
    *new_side_to_move = side_to_move;
    return pos;
}

static void send_result(Side side_to_move, PosEvaluation score)
{
    if (score == evals::DRAW_SCORE)
        printf("1/2-1/2\n");
    else if (score > 0 && side_to_move == sides::white || score < 0 && side_to_move == sides::black)
        printf("1-0\n");
    else
        printf("0-1\n");
}

int main()
{
    constants_initialize();

    Position pos;
    Side side_to_move = sides::white;
    Side engine_side  = sides::none;
    Move move, ponder_move;
    PosEvaluation score;

    string last_fen;
    Move game_history[MAX_GAME_MOVES];
    int move_number = 0;

    StopWatch clock;
    time_control_info time_control_info;
    memset(&time_control_info, 0, sizeof(time_control_info));

    // Options / settings
    bool should_ponder  = false;
    bool randomize      = false;
    bool post_thinking  = false;
    int max_depth       = MAX_SEARCH_DEPTH;
    int resign          = 0;
    int contempt_factor = 0;

    char input_buffer[80]     = "";
    char command[80]          = "";
    char ponder_move_text[20] = "";
    
    bool abort_flag = false;
    bool pondering  = false;

    // No output buffering
    setvbuf(stdout, NULL, _IONBF, 0);

    while(1)
    {
        input_buffer[0] = 0; // empty input buffer, so we can detect if something appears in it

        // calculate if we are expected to ponder
        pondering = (should_ponder               && 
                     engine_side != side_to_move &&
                     move_number != 0);

        if (engine_side == side_to_move || (pondering && ponder_move.data))
        {
            Side pre_ponder_side_to_move = side_to_move;
            Position pre_ponder_pos;
            if (pondering) 
            {
                // we are apparently pondering on a speculative move
                sprintf_s(ponder_move_text, "%s\n", move_to_coordtext(ponder_move).c_str()); // remember ponder move as text (for hit detection)
                pre_ponder_pos = pos;
                pos.make_move(ponder_move);
                side_to_move = swap_side(side_to_move);
                game_history[move_number++] = ponder_move;
            } 
            else
            {
                set_time_limits(time_control_info, move_number, time_control_info.time_left_millis);
            }

            Move new_ponder_move;
            score = search_best_move(pos, side_to_move, &move, &new_ponder_move);

            if (pondering) // pondering was aborted because of miss or other command
            { 
                pos = pre_ponder_pos;
                side_to_move = pre_ponder_side_to_move; 
                move_number--;
                pondering = false;
            } 
            else if (!move.data) // game apparently ended
            {  
                engine_side = sides::none;          // so stop playing
                send_result(side_to_move, score);
            } 
            else 
            {
                pos.make_move(move);
                side_to_move = swap_side(side_to_move);
                game_history[move_number++] = move;

                printf("move %s\n", move_to_coordtext(move).c_str());
                ponder_move = new_ponder_move;
                continue; // start pondering (if needed)
            }

        } 
        else if ((engine_side == sides::analyze) || pondering) // this catches pondering when we have no move
        { 
            Move dummy;
            pondering = true;          // in case we must analyze
            ponder_move_text[0] = 0;   // make sure we will never detect a ponder hit
            search_best_move(pos, side_to_move, &dummy, &dummy);
            pondering = false;
        }

        // the previous calls to SeachBestMove() could have left a command that interrupted it in inBuf !
        if (input_buffer[0] == 0)
            peek_at_input(input_buffer, command, ponder_move_text, move_number,
                          time_control_info, &pondering, clock, true); // handles time, otim

        if (!strcmp(command, "quit"))    { break; }
        if (!strcmp(command, "force"))   { engine_side = sides::none;    continue; }
        if (!strcmp(command, "analyze")) { engine_side = sides::analyze; continue; }
        if (!strcmp(command, "exit"))    { engine_side = sides::none;    continue; }

        if (!strcmp(command, "level"))
        {
            int minutes = 0;
            int seconds = 0;
            int increment = 0;
            if (sscanf(input_buffer, "level %d %d %d", &time_control_info.moves_per_session, &minutes, &increment) != 3)  // if this does not work, it must be min:sec format
            {
                if (sscanf(input_buffer, "level %d %d:%d %d", &time_control_info.moves_per_session, &minutes, &seconds, &increment) != 4)
                {
                    printf("Bad level command\n");
                    continue;
                }
            }
            time_control_info.time_control  = 60*minutes + seconds;
            time_control_info.time_per_move = -1;
            continue;
        }

        if (!strcmp(command, "protover"))
        {
            printf("feature ping=1 setboard=1 colors=0 usermove=1 memory=1 debug=1\n");
            //printf("feature option=\"Resign -check 0\"");           // example of an engine-defined option
            //printf("feature option=\"Contempt -spin 0 -200 200\""); // and another one
            printf("feature done=1\n");
            continue;
        }
    
        if (!strcmp(command, "option"))
        { 
            //if (sscanf(input_buffer + 7, "Resign=%d",   &resign)          == 1) continue;
            //if (sscanf(input_buffer + 7, "Contempt=%d", &contempt_factor) == 1) continue;
            continue;
        }

        if (!strcmp(command, "sd"))      { sscanf(input_buffer, "sd %d", &max_depth);     continue; }
        if (!strcmp(command, "st"))      { sscanf(input_buffer, "st %d", &time_control_info.time_per_move); continue; }
        if (!strcmp(command, "memory"))  { set_memory_size(atoi(input_buffer + 7));       continue; }
        if (!strcmp(command, "ping"))    { printf("pong%s", input_buffer + 4);            continue; }
        if (!strcmp(command, "easy"))    { should_ponder = false;                         continue; }
        if (!strcmp(command, "hard"))    { should_ponder = true;                          continue; }
        if (!strcmp(command, "go"))      { engine_side   = side_to_move;                  continue; }
        if (!strcmp(command, "post"))    { post_thinking = true;                          continue; }
        if (!strcmp(command, "nopost"))  { post_thinking = false;                         continue; }
        if (!strcmp(command, "random"))  { randomize     = true;                          continue; }

        if (!strcmp(command, "hint"))
        {
            if (ponder_move.data)
                printf("Hint: %s\n", move_to_coordtext(ponder_move).c_str());
            continue;
        }

        // Ignored commands
        if (!strcmp(command, "book"))     { continue; }
        if (!strcmp(command, "xboard"))   { continue; }
        if (!strcmp(command, "computer")) { continue; }
        if (!strcmp(command, "name"))     { continue; }
        if (!strcmp(command, "ics"))      { continue; }
        if (!strcmp(command, "accepted")) { continue; }
        if (!strcmp(command, "rejected")) { continue; }
        if (!strcmp(command, "variant"))  { continue; }
        if (!strcmp(command, ""))         { continue; }

        // Now process commands that do alter the position, and thus invalidate the ponder move
        ponder_move = Move();

        if (!strcmp(command, "new"))
        {
            last_fen    = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
            pos         = fen::parse_fen(last_fen, nullptr, &side_to_move);
            move_number = 0;
            engine_side = swap_side(side_to_move);
            max_depth   = MAX_SEARCH_DEPTH; 
            randomize   = false;
            // TODO: reset clocks?
            continue; 
        }

        if (!strcmp(command, "setboard"))
        { 
            engine_side = sides::none;
            last_fen = input_buffer + 9;
            pos = fen::parse_fen(last_fen, nullptr, &side_to_move);
            continue;
        }

        if (!strcmp(command, "undo"))
        { 
            pos = take_back(last_fen, game_history, move_number, 1, &side_to_move, &move_number); 
            continue;
        }

        if (!strcmp(command, "remove"))
        { 
            pos = take_back(last_fen, game_history, move_number, 2, &side_to_move, &move_number); 
            continue;
        }
        
        if (!strcmp(command, "usermove"))
        {
            Move move = parse_move(pos, input_buffer + 9);
            if (!move.data)
            {
                printf("Invalid move string\n");
            }
            else
            {
                if (!pos.make_move(move))
                    printf("Illegal move\n");

                side_to_move = swap_side(side_to_move);
                game_history[move_number++] = move;
            }

            continue;
        }

        printf("Error: unknown command\n");
    }

    return 0;
}
