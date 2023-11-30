#include "board.h"

Board::Board(int cols, int height)
{
  m_num_cols = cols;
  m_col_height = height;

  m_board_state = new int*[m_num_cols];

  for (int i=0; i < m_num_cols; ++i) {
    m_board_state[i] = new int[m_col_height];
    for (int j=0; j < m_col_height; ++j) {
      m_board_state[i][j] = -1;
    }
  }
}

Board::~Board()
{
  for (int i=0; i < m_num_cols; ++i) {
    delete m_board_state[i];
  }
  delete m_board_state;
}

Board::Board(const Board &b)
{
  m_num_cols = b.m_num_cols;
  m_col_height = b.m_col_height;

  m_move_history = std::list<int>(b.m_move_history);
  m_player_history = std::list<int>(b.m_player_history);

  m_board_state = new int*[m_num_cols];

  for (int i=0; i < m_num_cols; ++i) {
    m_board_state[i] = new int[m_col_height];
    for (int j=0; j < m_col_height; ++j) {
      m_board_state[i][j] = b.m_board_state[i][j];
    }
  }
}

void Board::show()
{
  for (int i=m_col_height-1; i >= 0; --i) {
    std::cout << i << " ";
    for (int j=0; j < m_num_cols; ++j) {
      int val = m_board_state[j][i];
      char symbol = ' ';
      if (val == 0) {
	symbol = 'X';
      } else if (val == 1) {
	symbol = 'O';
      }
      std::cout << '|' << symbol;
    }
    std::cout << '|' << std::endl;
  }

  std::cout << "  ";
  for (int i=0; i < m_num_cols; ++i) {
    std::cout << " " << i;
  }
  std::cout << std::endl;
}

bool Board::is_full()
{
  for (int i=0; i < m_num_cols; ++i) {
    if (m_board_state[i][m_col_height-1] == -1) {
      return false;
    }
  }
  return true;
}

bool Board::is_col_full(int col)
{
  return m_board_state[col][m_col_height-1] != -1;
}

void Board::do_move(int player, int move)
{
  for (int i=0; i < m_col_height; ++i) {
    if (m_board_state[move][i] == -1) {
      m_board_state[move][i] = player;
      break;
    }
  }

  m_move_history.push_front(move);
  m_player_history.push_front(move);
}

void Board::undo_last_move()
{
  int last_move = m_move_history.front();
  m_move_history.pop_front();

  for (int i=m_col_height-1; i >= 0; --i) {
    if (m_board_state[last_move][i] != -1) {
      m_board_state[last_move][i] = -1;
      break;
    }
  }

  m_player_history.pop_front();
}

std::list<int> Board::get_available_moves()
{
  std::list<int> ret;
  for (int i=0; i < m_num_cols; ++i) {
    if (m_board_state[i][m_col_height-1] == -1) {
      ret.push_front(i);
    }
  }
  return ret;
}


int Board::winner()
{
  if (m_move_history.size() == 0) {
    return -1;
  }
  
  int col = m_move_history.front();
  int height = -1;
  for (int i=m_col_height-1; i >= 0; --i) {
    if (m_board_state[col][i] != -1) {
      height = i;
      break;
    }
  }

  bool is_winner = _down_win(col, height) || _hori_win(col, height) \
    || _diag_up_win(col, height) || _diag_down_win(col, height);

  if (!is_winner)
    return -1;
  else
    return m_board_state[col][height];
}

bool Board::_down_win(int col, int height)
{
  int player = m_board_state[col][height];
  int count = 0;
  for (int i=0; i<4; ++i) {
    int _h = height-i;
    if (_h < 0) 
      break;
    else if (m_board_state[col][_h] == player)
      count += 1;
    else 
      break;
  }   
  return count == 4;
}

bool Board::_hori_win(int col, int height)
{
  int player = m_board_state[col][height];
  int count = 0;
  for (int i=0; i<4; ++i) {
    count = 0;
    for (int c=0; c<4; ++c) {
      int _col = col-i+c;
      if (_col < 0 || _col >= m_num_cols) 
	break;
      else if (m_board_state[_col][height] == player) 
	count += 1;
      else 
	break;
    }
    if (count == 4) 
      return true;
  }
  return false;
}

bool Board::_diag_up_win(int col, int height)
{
  int player = m_board_state[col][height];
  int count = 0;
  for (int i=0; i<4; ++i) {
    count = 0;
    for (int c=0; c<4; ++c) {
      int _col = col-i+c;
      int _height = height-i+c;
      if (_col < 0 || _col >= m_num_cols || \
	  _height < 0 || _height >= m_col_height) 
	break;
      else if (m_board_state[_col][_height] == player) 
	count += 1;
      else 
	break;
    }
    if (count == 4) 
      return true;
  }
  return false;
}

bool Board::_diag_down_win(int col, int height)
{
  int player = m_board_state[col][height];
  int count = 0;
  for (int i=0; i<4; ++i) {
    count = 0;
    for (int c=0; c<4; ++c) {
      int _col = col-i+c;
      int _height = height+i-c;
      if (_col < 0 || _col >= m_num_cols || \
	  _height < 0 || _height >= m_col_height) 
	break;
      else if (m_board_state[_col][_height] == player) 
	count += 1;
      else 
	break;
    }
    if (count == 4) 
      return true;
  }
  return false;
}

