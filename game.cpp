#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>

#include "board.h"

const char symbols[2] = {'X', 'O'};
const int infinity = 999999;
const int top_value = 200;

struct cmd_args {
  bool help; // help 
  bool success; // parse successful
  int cols; // number of columns
  int height; // height of columns
  bool ai1; // is player 1 ai?
  int ai1d; // depth of ai1 alphabeta
  bool ai2; // is player 2 ai?
  int ai2d; // depth of ai2 alphabeta
};

struct ai_move {
  int value;
  int move;
};

cmd_args parse_cmd_args(int, char**);
int start_game(int,int,bool,int,bool,int);
int get_user_input(int);
ai_move alphabeta(Board&, int, int, int, int, int, bool);
int evaluate(const Board&,int);
int rollout_mc(const Board&);
int select_random_move(Board&);
int get_ai_input(Board&, int, int);
void print_help();

int start_game(int cols, int height, bool ai1, int ai1d, bool ai2, int ai2d)
{ 
  Board b = Board(cols, height);
  int current_player = 0;
  bool game_end = false;

  b.show();
  
  int move;
  while (!game_end) {
    if (current_player == 0) {
      if (ai1)
	move = get_ai_input(b, current_player, ai1d);
      else
	move = get_user_input(current_player);
    } else {
      if (ai2)
	move = get_ai_input(b, current_player, ai2d);
      else
	move = get_user_input(current_player);
    }

    if (move < 0 || move >= cols || b.is_col_full(move)) {
      continue;
    }

    b.do_move(current_player, move);

    int win = b.winner();
    if (win != -1) {
      std::cout << "Player " << symbols[win] << " won!" << std::endl;
      game_end = true;
    } else if (b.is_full()) {
      std::cout << "It's a draw!" << std::endl;
      game_end = true;
    }

    current_player = (current_player + 1) % 2;

    b.show();
  }
  return 0;
}

int get_user_input(int player)
{
  while (true) {
    int col;
    std::cout << "Player " << symbols[player] << " input:\n> ";
    std::cin >> col;
 
    if (std::cin.fail()) {
      std::cin.clear();
      std::cin.ignore(2,'\n');
    }
    else {
      return col; 
    }
  }
}

int get_ai_input(Board &b, int player, int depth)
{
  char symbol = symbols[player];
  std::cout << "AI " << symbol << ": thinking..." << std::endl;
  std::cout << "AI " << symbol << ": move: value, ";
  ai_move best_move = alphabeta(b, depth, depth, -infinity, infinity, \
				player, true);
  std::cout << std::endl;
  std::cout << "AI " << symbol << ": MOVE " << best_move.move \
	    << ", VALUE " << best_move.value << std::endl;
  return best_move.move;
}

ai_move alphabeta(Board &board, int initial_depth, int depth, int alpha, \
		  int beta, int calling_player, bool max_player)
{
  int winner = board.winner();
  if (winner != -1) {
    ai_move ret;
    if (winner == calling_player) {
      ret.value = 2*top_value;
      ret.move = -1; // I do not make any move as this board in win state
    } else {
      ret.value = -2*top_value;
      ret.move = -1;
    }
    return ret;
  }
  
  if (depth == 0 || board.is_full()) {
    int val = evaluate(board, calling_player);
    ai_move ret;
    ret.value = val;
    ret.move = -1;
    return ret;
  }

  if (max_player) {
    int max_value = -infinity;
    int max_move = -1;
    std::list<int> possible_moves = board.get_available_moves();
    for (int move : possible_moves) {
      board.do_move(calling_player, move);
      ai_move _move = alphabeta(board, initial_depth, depth-1, alpha, \
				beta, calling_player, false);
      board.undo_last_move();
      if (_move.value > max_value) {
	max_value = _move.value;
	max_move = move;
      }
      alpha = std::max(alpha, _move.value);

      // Some pretty UI
      if (depth == initial_depth)
	std::cout << move << ": " << _move.value << ", ";

      if (alpha >= beta)
	break;
    }
    ai_move ret;
    ret.move = max_move;
    ret.value = max_value;
    return ret;
    
  } else {
    int min_value = infinity;
    int min_move = -1;
    std::list<int> possible_moves = board.get_available_moves();
    for (int move : possible_moves) {
      board.do_move((calling_player+1)%2, move);
      ai_move _move = alphabeta(board, initial_depth, depth-1, alpha, \
				beta, calling_player, true);
      board.undo_last_move();
      if (_move.value < min_value) {
	min_value = _move.value;
	min_move = move;
      }
      beta = std::min(beta, _move.value);

      if (beta <= alpha)
	break;
    }
    ai_move ret;
    ret.move = min_move;
    ret.value = min_value;
    return ret;
  }
}
      
int evaluate(const Board &board, int player)
{
  int opponent = (player + 1) % 2;

  int wins = 0;
  int lose = 0;
  for (int i=0; i<top_value; ++i) {
    int winner = rollout_mc(board);
    if (winner == player) {
      wins += 1;
    } else if (winner == opponent) {
      lose += 1;
    }
  }
  return wins - lose;
}

int rollout_mc(const Board &board)
{
  Board b = Board(board);
  int win_player = -1;
  int first_player = (board.get_last_player() + 1) % 2;

  int player = first_player;
  while (!b.is_full()) {
    int move = select_random_move(b);
    b.do_move(player, move);
    int winner = b.winner();
    if (winner != -1) {
      win_player = winner;
      break;
    }
    player = (player + 1) % 2;
  }
  return win_player;
}

int select_random_move(Board &board)
{
  std::list<int> possible_moves = board.get_available_moves();
  int random_number = std::rand()/((RAND_MAX + 1u)/possible_moves.size());
  auto it1 = std::next(possible_moves.begin(), random_number);
  return *it1;
}


int main(int argc, char** argv)
{
  std::srand(std::time(nullptr));
  cmd_args in = parse_cmd_args(argc, argv);
  if (in.success) {
    start_game(in.cols, in.height, \
	       in.ai1, in.ai1d, \
	       in.ai2, in.ai2d);
  } else if (in.help) {
    print_help();
  } else {
    std::cout << "ERROR: could not parse arguments" << std::endl;
  }
}

cmd_args parse_cmd_args(int argc, char** argv)
{
  cmd_args ret;
  ret.help = false;
  ret.success = false;
  ret.cols = 7;
  ret.height = 6;
  ret.ai1 = false;
  ret.ai2 = false;
  ret.ai1d = 1;
  ret.ai2d = 1;
  
  if (argc == 2) {
    std::string arg = argv[1];
    if (arg == "-h") 
      ret.help = true;
    return ret;
  }
      
  for (int i=1; i < argc-1; ++i) {
    std::string arg = std::string(argv[i]);
    std::string narg = std::string(argv[i+1]);
    if (arg == "-h") {
      ret.help = true;
      return ret;
    }
    if (arg == "-cols") {
      int _col;
      try {
	_col = std::stoi(narg);
	ret.cols = _col;
      } catch (const std::exception &e) {
	std::cout << "ERROR: invalid input -cols " << narg << std::endl;
	return ret;
      }
    }
    if (arg == "-height") {
      int _height;
      try {
	_height = std::stoi(narg);
	ret.height = _height;
      } catch (const std::exception &e) {
	std::cout << "ERROR: invalid input -height " << narg << std::endl;
	return ret;
      }
    }
    if (arg == "-ai1") {
      int _ai1d;
      try {
	_ai1d = std::stoi(narg);
	ret.ai1 = true;
	ret.ai1d = _ai1d;
      } catch (const std::exception &e) {
	std::cout << "ERROR: invalid input -ai1d " << narg << std::endl;
	return ret;
      }
    }
    if (arg == "-ai2") {
      int _ai2d;
      try {
	_ai2d = std::stoi(narg);
	ret.ai2 = true;
	ret.ai2d = _ai2d;
      } catch (const std::exception &e) {
	std::cout << "ERROR: invalid input -ai2d " << narg << std::endl;
	return ret;
      }
    }
  }
  ret.success = true;
  return ret;
}

void print_help()
{
  std::cout << "SIMPLE CONNECT FOUR ALPHA-BETA PLAYER" << std::endl;
  std::cout << "OPTIONS:" << std::endl;
  std::cout << "-cols COLS      the number of columns of the board" << std::endl;
  std::cout << "-height HEIGHT  the height of the board" << std::endl;
  std::cout << "-ai1 DEPTH      player 1 is AI, thinking depth" << std::endl;
  std::cout << "-ai2 DEPTH      player 2 is AI, thinking depth" << std::endl;
}
