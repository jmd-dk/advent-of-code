import numpy as np

# Part one
boards = []
with open('input.txt') as f:
    numbers_drawn = [int(i) for i in f.readline().split(',')]
    board = []
    for line in f:
        if line.strip():
            board.append(np.array([int(i) for i in line.split()]))
        else:
            if board:
                boards.append(np.array(board))
            board = []
    boards.append(np.array(board))
def check(board):
    def sum_unmarked():
        return board[board != -1].sum()
    for row in board:
        if row.sum() == -board.shape[1]:
            return sum_unmarked()
    for col in board.T:
        if col.sum() == -board.shape[0]:
            return sum_unmarked()
def play(until='first win'):
    wins = 0
    for n in numbers_drawn:
        for i, board in enumerate(boards):
            if board is None:
                continue
            board[board == n] = -1
            if (s := check(board)):
                wins += 1
                score = s*n
                if until == 'first win' or wins == len(boards):
                    return score
                boards[i] = None
score = play()
print('part one:', score)

# Part two
score = play('last win')
print('part two:', score)

