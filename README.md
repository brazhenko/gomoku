# gomoku

One of my quality pet-projects at `42` written in C++17. 

## About

[`Gomoku`](https://en.wikipedia.org/wiki/Gomoku) is a strategy board game. Goal of the game is to put 5 of your 
stones in a row (It is also called Five-in-a-Row). This game has lots of variations. 
The original one is no more applied, because first player can win the game
[regardless of the opponent's moves](https://habr.com/ru/post/437064/).  

My aim was to code game interface and an Engine for one of variations.

#### Rules

+ Gomoku is a zero-sum game (If one wins the other won loses) of two players.
+ The goal is to win. A player wins if and only if they produced a five
    (continugous alignment of five stones of one color) OR they produced 5 captures.
 
+ Stones are put one at a time. Red starts.
+ Captures. If one puts a stone like this: `XOO_ -> XOOX` trapped zeros are removed from the board :
`X__X`. These two free cells behave like they have never been occupied. Summary:
2 squeezed stoned (by two other stones) must be removed from board.
+ Playes must NOT put the stone which produces two or more free-threes because this figure is unstoppable.
Free three is a figure that becomes a free four in one move: `_XX_X_ -> _XXXX_`.
Exception: a capture which leads to 2 or more free threes *is not fobidden*.
+ Rules can be checked in original [subject](https://cdn.intra.42.fr/pdf/pdf/13352/en.subject.pdf) of the project. 

#### Engine

An engine which analizes, evaluates and looks up for "best" move was written during this project.
The engine is based on [minimax algorithm with alpha-beta pruning optimization](http://web.cs.ucla.edu/~rosen/161/notes/alphabeta.html),
evaluation heuristics, selection heuristics and a bit more.

## Build

```bash
# Build was tested only on MacOS, clang-1100.0.33.17
bash install_dependencies.sh
mkdir build_dir
cd build_dir
cmake -DCMAKE_BUILD_TYPE="Release" .. && make
```

## Usage

```bash
./gomoku
```

## Docs
```
|- build_dir
   |- html
      > index.html 
```

## Screenshots

![example 2](resourses/gomoku1.png)


## License
[MIT](https://choosealicense.com/licenses/mit/)
