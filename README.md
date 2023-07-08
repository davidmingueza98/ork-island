# ork-island

## Introduction
Ork Island was a project from EDA course (Estructura de Dades i Algorismes) of my college back in 2017.
This game provides a platform to learn about data structures and design an algorithm with the most
optimal strategy on a multiplayer competitive game.

My own player is saved in [AIWillyrex.cc](./src/AIWillyrex.cc). Feel free to create your own player
and try to beat me!

The original creator of this game is Enric Rodríguez. Check more details in [author.txt](./author.txt).

## Game rules
The power of Sauron, the Lord of the Rings, is spreading throughout Middle
Earth. People flee in terror from his bloodthirsty ork soldiers. As a result, once
prosperous cities are now abandoned, and vast areas of land have become as
lifeless as a desert. Sauron’s troops would quickly conquer all Middle Earth...
were it not for their own greed and ambition.

In this game, four players compete to conquer an island of Middle Earth for
Sauron. The winner of a match is the one that gets the highest score at the end.
The map of the island is represented with a (randomly generated) square board.

The cells on this board can be of different types:
- WATER
- GRASS
- FOREST
- SAND
- CITY
- PATH

Being an island, the board is surrounded with WATER. Moreover, cells of type CITY
are grouped into rectangles representing cities, hence the name. Similarly, cells
of type PATH are grouped into sequences that form paths.

**Paths connect different cities and never cross each other.**

In order to conquer the island, each player runs an army of ork soldiers. At
each round of the match, players command their orks. Any player that tries
to give more than 1000 instructions in the same round will be aborted.

An ork can be instructed to remain still or move one cell towards:
 - North
 - South
 - East
 - West

If an ork receives more than one instruction, all but the first one are ignored.
**Orks cannot move to cells with WATER** (since otherwise they would lose the layer
of dirt on their skin). On the other hand, they can move
to all other cells. However, when an ork moves, its health (an integer value)
decreases. Depending on the type of cell where an ork goes, this decrement
in health may be different. When an ork reaches a negative health, it dies and
regenerates under the control of the same player. Initially, all orks have the
same health and when they regenerate they get this same amount of health again.

Each cell of the board can be occupied by a single ork at most. In the particular
situation that an ork A attempts to move to a cell where there is already another ork B
(who has moved there previously in the same round, or was there earlier), the following cases are considered:
- If A and B belong to the same player, the instruction is ignored.
- Otherwise, there is a fight after which one of the two orks will die. If the
health of A (after the decrement due to the movement) is strictly greater
than the health of B, then B dies. Symmetrically, if it is strictly less than
the health of B, then A dies. If there is a tie, then the ork that dies is decided
randomly with uniform probability, that is, 50%. The ork that dies
regenerates under the control of the other player with the initial amount
of health. The winner of the fight keeps the same amount of health.
When an ork dies, it regenerates at the next round on a random position on the
shore of the island, that is, on a cell adjacent to the sea which is not WATER, CITY
or PATH. Similarly, initially all players have their orks randomly distributed on
the shore.

At the beginning of a match, all cities and paths are empty, i.e., do not have
any orks on their cells. However, once the game starts, orks can move to them.

Points are then computed as follows. At the end of a round, for each city the
number of orks of each player on its cells is counted. If there is a player who
has strictly more orks on the city than any other player, then this player conquers
the city; in case of a tie, the conqueror of the city (if any) does not change from
the previous round.

In any case, for each city currently conquered by a player (i.e., currently under their control),
this player accumulates a number of points which is bonus per city *cell() × the size of the city*
(that is, the number of its cells); for paths the same applies as for cities, but the number of accumulated
points are bonus per path *cell() × the size of the path*.

Finally, for each player, their **graph of conquests** is considered. In this graph, the vertices are the conquered cities,
and the edges are the conquered paths that connect conquered cities. For each connected component of the graph with i vertices,
*additional factor connected component() × 2 i* points are obtained.

<p align="center">
  <a>
    <img alt="Screenshot of the game" title="Screenshot of the game"
         src="https://github.com/davidmingueza98/ork-island/blob/master/doc/images/screenshot1.png?raw=true"
         width="562" height="594">
  </a>
</p>

Let us illustrate with an example how the score is computed. Figure 1 shows
a screenshot of the game. Blue represents WATER, light green represents GRASS,
deep green represents FOREST, light yellow represents SAND, deep grey represents
CITY and light grey represents PATH. The orks of a player are identified
with small squares of the same color. Conquered cities and paths are filled with
a crossed grid of the color of the player that conquered them.

Let us now count the score of the red player accumulated in the current round:

- **Cities**: The red player has conquered cities (from top to bottom) with
dimensions *5 × 2, 6 × 5, 2 × 4, 4 × 2, 3 × 2, 2 × 5, 5 × 5* and *5 × 2*. In
total: 107 points if *bonus per city cell()* = 1.

- **Paths**: The red player has conquered paths (from top to bottom) with
sizes 11, 3, 38 and 18. In total, *( 11 + 3 + 38 + 18 ) × bonus per path cell()* = 2.

370 points if *bonus per path cell()* = 1.

Figure 1: Screenshot of the game.

- **Graph of conquests**: The graph of the red player has five connected
components: one with 3 cities, another one with 2 cities, and three components
consisting of an isolated vertex. In total, *( 2 3 + 2 2 + 3 × 2 1 ) × factor connected component()* = 36 points
if bonus per city cell () = 2.

Thus, in total the red player has accumulated **213 points** in this round.

In general, the execution of a round follows the next steps:
1. All instructions of all players are registered according to the above rules.
2. The instructions are selected randomly and executed (if valid).
3. Dead orks are regenerated.
4. For each player, the points obtained at the end of the round are computed
and added to the score.

### Game parameters
A game is defined by a board and the following set of parameters, whose default values are shown in parentheses:

- *nb players()*: number of players (4)
- *rows()*: number of rows of the board (70)
- *columns()*: number of columns of the board (70)
- *nb rounds()*: number of rounds of the match (200)
- *initial health()*: initial health of each ork (100)
- *nb orks()*: number of orks each player controls initially (15)
- *cost grass()*: cost in health of moving to a cell of type GRASS (1)
- *cost forest()*: cost in health of moving to a cell of type FOREST (2)
- *cost sand()*: cost in health of moving to a cell of type SAND (3)
- *cost city()*: cost in health of moving to a cell of type CITY (0)
- *cost path()*: cost in health of moving to a cell of type PATH (0)
- *bonus per city cell()*: bonus in points for each cell in a conquered city (1)
- *bonus per path cell()*: bonus in points for each cell in a conquered path (1)
- *factor connected component()*: factor multiplying the size of connected components (2)

## Programming
This source code includes a C++ program that runs the matches and also an HTML viewer to
watch them in a nice animated format. Also, a *Null* player and a *Demo* player are
provided to make it easier to start coding your own player.

## Running your first match
Here we will explain how to run the game under Linux, but a similar procedure
should work in alternative OS. The only requirements are `g++`, `make` and a browser like Mozilla
Firefox or Google Chrome. To run your first match, follow the next steps:

1. Open a console and go to the directory where you extracted the source code.
2. Run `make all` to build the game and all the players. Note that Makefile identifies any file matching `AI*.cc` as a player.
3. The executable `Game` allows you to run a match using a command like:

```
./Game Demo Demo Demo Demo -s 30 -i default.cnf -o default.out
```

In this case, this runs a match with random seed 30 where four instances of the player *Demo* play
with the parameters defined in `default.cnf` (the default parameters).

The output of this match is redirected to the file `default.out`.

4. To watch a match, open the viewer file `viewer.html` with your browser
and load the file `default.out`. Or alternatively use the script `viewer.sh`,
e.g. `./viewer.sh default.out`.

Use
```
./Game --help
```
to see the list of parameters that you can use. Particularly useful is
```
./Game --list
```
to show all the registered player names.
If needed, remember that you can run
```
make clean
```
to delete the executable and object files and start over the build.

### Adding your player
To create a new player with, say, name Sauron, copy `AINull.cc` (an empty
player that is provided as a template) to a new file `AISauron.cc`. Then, edit the
new file and change the
`#define PLAYER NAME Null`
line to
`#define PLAYER NAME Sauron`

It will be used to define a new class `PLAYER NAME`, which will be referred to below as your player class.
Now you can start implementing the method `play()`. This method will be called
every round and is where your player should decide what to do, and do it.
Of course, you can define auxiliary methods and variables inside your player
class, but the entry point of your code will always be this `play()` method.

From your player class you can also call functions to access the board state, as
defined in the `State` class in `State.hh`, and to command your units, as defined
in the `Action` class in `Action.hh`. These functions are made available to your
code using multiple inheritance.

The documentation on the available functions can be found in the aforementioned header files.
You can also examine the code of the *Demo* player in `AIDemo.cc` as an example of how to use these functions.

Finally, it may be worth as well to have a look at the files `Structs.hh` for
useful data structures, `Random.hh` for random generation utilities, `Settings.hh`
for looking up the game settings and `Player.hh` for the `me()` method.

Note that you should not modify the `factory()` method from your player class,
nor the last line that adds your player to the list of registered players.

### Playing against the *Dummy* player
To test your strategy against the *Dummy* player, we provide the `AIDummy.o`
object file. This way you still will not have the source code of our *Dummy*,
but you will be able to add it as a player and compete against it locally.

To add the *Dummy* player to the list of registered players, you will have to
edit the Makefile file and set the variable `DUMMY OBJ` to the appropriate value.

## Conclusions

My player follows a basic rules to achieve a good score in the game:

1. Calculates and saves the shortest path to a city or path for each ork.
2. Moves the orks following the calculations.
3. Greedy strategy: once a city is conquered try to conquer other adjacent cities and paths.
4. Attack enemies in surrounding cells if they have less life.

My player got high scores in the competition against other classmates. However it's far away from
the best possible strategy. The most important points I missed are:

1. Avoiding enemies with more life at any moment
2. Calculate density of enemies in the board
3. Detect which cities and paths are not conquered
4. Kill enemies in the route to paths or cities
5. Protect orks with less life with powerful units

On the other hand, strategies are very useful to work with different algorithms and learn how to apply them
while enjoying the competition. Nowadays, the most powerful solution to get the best automatized
player can be achieved applying **reinforced learning**. A good designed trained phase could lead the agent
to achieve a strategy never thought before that beats all the points listed. Certainly, a nice challenge
could be to adapt the machine learning model to be used in the Player class because is written mainly in C++.

Sources:
- [AlphaGo](https://www.deepmind.com/research/highlighted-research/alphago)
- [MuZero](https://www.deepmind.com/blog/muzero-mastering-go-chess-shogi-and-atari-without-rules)
