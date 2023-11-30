#include <array>
#include <list>
#include <iostream>

class Board
{
  int m_num_cols;
  int m_col_height;
  
  int** m_board_state;

  std::list<int> m_move_history;
  std::list<int> m_player_history;
public:
  Board(int cols, int height);
  Board(const Board &b);
  ~Board();
  
  int get_num_cols() const { return m_num_cols; }
  int get_col_height() const { return m_col_height; } 
  int get_last_player() const { return m_player_history.front(); }
  int get_last_move() const { return m_move_history.front(); }
  
  bool is_full();
  bool is_col_full(int col);
  int winner();
  void do_move(int player, int move);
  void undo_last_move();
  std::list<int> get_available_moves();

  void show();

private:
  bool _down_win(int col, int height);
  bool _hori_win(int col, int height);
  bool _diag_up_win(int col, int height);
  bool _diag_down_win(int col, int height);
};
