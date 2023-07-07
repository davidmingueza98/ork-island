#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <limits>
#include "Player.hh"
using namespace std;

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Willyrex


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */
  typedef vector<int> Vector;
  typedef vector<Vector> Matrix;
  typedef pair<int, Pos> ArcP;

  //Paths are saved inside a map
  map <int, stack<Pos> > Caminos;

  //Vector represents the entire board
  vector<Pos> g; 

  map<int,bool> city_given;
  map<int,bool> path_given;

  //Saves if an orc has arrived at a city
  map <int,Pos> city_reached;

  int infinity = numeric_limits<int>::max();
  double infinity_double = numeric_limits<double>::max();

  /**
   * Traverses a path in a map from a positon y to a position x and saves
   * the steps in a stack.
   */
  stack<Pos> generate_path(map<Pos,Pos> &p, Pos x, Pos y) {
      stack<Pos> pila;
      while (y != x) { //not path end
          pila.push(y);
          y = p[y]; //explore map containing path order
      }
      return pila;
  }

  /** Graph ahora es un mapa de Pos a una struct de sus casillas adjacentes que son cuatro exactamente*/

  void path_finding(const vector<Pos> &G, Pos initial, Pos end, map<Pos,int> &d, map<Pos,Pos> &p) {

    map<Pos,bool> S; //found map

    /**
     * Initialize map D, contaning the distance related at each position, to infinity
     * Initialize map P, contaning the minimum path implicitily in the map, to -1
     * Initialize map S, contaning whether the position is visited or not, to false
     */
    for (int i = 0; i < int(G.size()); ++i){
      Pos iteradora = G[i];
      d[iteradora] = infinity;
      p[iteradora].i = -1;
      p[iteradora].j = -1;
      S[iteradora] = false;
    }

    //Defines a priority queue with ArcP, pairs of int and Pos
    priority_queue<ArcP, vector<ArcP>, greater<ArcP> > Q;

    //Initial position has distance 0 and identifier 0
    d[initial] = 0;
    Q.push(ArcP(0, initial));

    while (not Q.empty()) {
      Pos u = Q.top().second;
      if (u == end) return;
      Q.pop();

      //position has not been visited
      if (not S[u]) {
        S[u] = true;
        for (int i = 0; i < 4; ++i) {
          Pos v; //candidate position
          if (i == 0) v = Pos(u.i, u.j - 1); //left
          else if (i == 1) v = Pos(u.i, u.j + 1); //right
          else if (i == 2) v = Pos(u.i - 1, u.j); //up
          else if (i == 3) v = Pos(u.i + 1, u.j); //down

          Cell aux = cell(v.i, v.j);
          if (aux.type != WATER) {
            int c = cost(aux.type);
            //if distance is greater then update, look for a minimum
            if (d[v] > d[u] + c) {
              d[v] = d[u] + c;
              p[v] = u; //save position in map
              Q.push(ArcP(d[v], v)); //enqueue cost and position
            }
          }
        }
      }
    }
  }


  //Saves all the positions of the board in the vector g
  void init_board_graph(vector<Pos> &g) {
    for (int i = 0; i < rows(); ++i) {
      for (int j = 0; j < cols(); ++j) {
        if (cell(i,j).type != WATER) {
          g.push_back(Pos(i,j));
        }
      }
    }
  }


  //Calculates the Euclidean distance between two positions
  double distance(Pos pos1, Pos pos2) {
    return sqrt(pow(pos1.i - pos2.i, 2) + pow(pos1.j - pos2.j, 2));
  }


  //Finds the nearest position of a path from the orc's initial position
  Pos find_destiny_path(Pos spawned) {
      double minimum = infinity_double;
      Pos candidate = spawned;
      for (int i = 0; i < nb_paths(); ++i) {
        if (not path_given[i]) {
          Path path_info = path(i);
          vector<Pos> aux = path_info.second;
          double calculated_distance;
          for (int i = 0; i < int(aux.size()); ++i){
            calculated_distance = distance(aux[i], spawned);
            if (calculated_distance < minimum){
              minimum = calculated_distance;
              candidate = aux[i];
            }
          }
        }
      }

      Cell c = cell(candidate);
      path_given[c.path_id] = true;
      return candidate;
  }


  //Finds the nearest path or city with enemies with less life than the current orc
  Pos find_destiny_civilization(Pos spawned, int id) {
    double minimum = infinity_double;
    bool found = false;
    bool danger = false;

    Pos candidate;
    for(int i = 0; i < nb_cities(); ++i) {
      if (not city_given[i]) {
        found = true;
        City city_info = city(i);
        vector<Pos> aux = city_info;
        double calculated_distance;
        for (int i = 0; i < int(aux.size()); ++i) {
          Cell c = cell(aux[i].i, aux[i].j);
          if (c.unit_id != -1) {
            Unit enemy = unit(c.unit_id);
            Unit my_ork = unit(id);
            if (enemy.player != me() and enemy.health > my_ork.health) danger = true;
          }
          calculated_distance = distance(aux[i], spawned);
          //go for the minimum and safest distance
          if (calculated_distance < minimum and not danger) {
            minimum = calculated_distance;
            candidate = aux[i];
          }
          //deactivate danger flag
          if (danger) {
            danger = false;
            found = false;
            break;
          }
        }
      }
    }

    //alternative go to a path
    if (not found) return find_destiny_path(spawned);

    Cell c = cell(candidate);
    city_given[c.city_id] = true;
    return candidate;
  }


  //Checks if an ork has a path assigned
  bool ork_has_path(int id) {
    for(map<int, stack<Pos>>::iterator it = Caminos.begin(); it != Caminos.end(); ++it) {
      if (it->first == id) return true;
    }
    return false;
  }


  //Regenerates a path with id from actual to end position
  void recalculate_path(int id, Pos actual, Pos end) {
	  map<Pos,int> d;
	  map<Pos,Pos> p;
	  path_finding(g, actual, end, d, p);
	  Caminos[id] = generate_path(p, actual, end);
  }


  //Checks if pos is not already occupied from an orc of our army
  bool position_is_available(Pos pos, Vector &my_orks) {
	  Cell c = cell(pos.i, pos.j);
	  for (int i = 0; i < int(my_orks.size()); ++i) {
		  if (c.unit_id == my_orks[i]) return false;
	  }
	  return true;
  }


  /*
   * Checks if an enemy is surrouding the orc and creates
   * a new path to attack if the enemy is in a path or city
   * and has less health
   */
  void enemy_near(int id) {
    Unit ork = unit(id);
    Pos near;
    for (int i = 0; i < 8; ++i) {
      if (i == 0) near = Pos(ork.pos.i + 1, ork.pos.j + 1);
      else if (i == 1) near = Pos(ork.pos.i + 1, ork.pos.j);
      else if (i == 2) near = Pos(ork.pos.i + 1, ork.pos.j - 1);
      else if (i == 3) near = Pos(ork.pos.i, ork.pos.j - 1);
      else if (i == 4) near = Pos(ork.pos.i - 1, ork.pos.j - 1);
      else if (i == 5) near = Pos(ork.pos.i - 1, ork.pos.j);
      else if (i == 6) near = Pos(ork.pos.i - 1, ork.pos.j + 1);
      else if (i == 7) near = Pos(ork.pos.i, ork.pos.j + 1);

      Cell c = cell(near);
      if (c.unit_id != -1) {
        Unit enemy = unit(c.unit_id);
        if ( (c.type == PATH or c.type == CITY) and
             enemy.player != me() and
             enemy.health < ork.health) {
          recalculate_path(id, ork.pos, near); //new shortest path to kill the enemy
          city_reached[id] = near;
          return;
        }
      }
    }
  }


  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {

    //Cut the execution if we are very near of the maximum computation limit
    if(status(me()) >= 0.96) return;

    //Get the ids of the orks with life
    Vector my_orks = orks(me());

    //Process orks in random order
    //Vector perm = random_permutation(my_orks.size())

    //Update the ownership of cities and paths
    for (int i = 0; i < nb_cities(); ++i) {
      if (city_owner(i) != me()) city_given[i] = false;
    }

    for (int i = 0; i < nb_paths(); ++i) {
        if(path_owner(i) != me()) path_given[i] = false;
    }

    if (round() == 0) {
      //Initialize the board and calculate the initial routes of the orks
      init_board_graph(g);

      for (int k = 0; k < int(my_orks.size()); ++k) {
        Unit ork = unit(my_orks[k]);
        Pos initial = ork.pos;
        Pos end = find_destiny_civilization(ork.pos, ork.id);

        map<Pos,int> d;
        map<Pos,Pos> p;
        //find and generate the shortest path to a city
        path_finding(g, initial, end, d, p);
        stack<Pos> aux = generate_path(p, initial, end);
        Caminos[my_orks[k]] = aux; //save the path of each ork
        city_reached[my_orks[k]] = end; //save the target position of each ork
      }
    }

    //Send all the orks to cities with a greedy strategy
    for (int i = 0; i < int(my_orks.size()); ++i) {
      int id = my_orks[i];
      Unit ork = unit(id);
      Pos actual = ork.pos;

      //if it's a new ork or the initial route is over, regenerate a new path
			if (not ork_has_path(id) or actual == city_reached[id]) {
			  Pos target = find_destiny_civilization(actual, id);
        recalculate_path(id, actual, target);
        city_reached[id] = target;
        if (actual == city_reached[id]) stack<Pos> now = Caminos[id];
			}

      //check if enemies are near inside a city or path
      Cell c = cell(ork.pos);
      if (c.type == CITY or c.type == PATH) enemy_near(id);
      stack<Pos> now = Caminos[id];

      //route is assigned
      if (not now.empty()) {
        Pos destiny = now.top();
        bool incorrect_info = false;

        //Extract which direction the orc has to follow based on
        //our path finding algorithm
        Dir dir;
        for (int d = 0; d != DIR_SIZE; ++d) {
          Dir possible_dir = Dir(d);
          //if the information is incorrect, recalculate path
          if (d == NONE) {
            Pos fin = find_destiny_civilization(actual, id);
            recalculate_path(id, actual, fin);
            incorrect_info = true;
          }
          if(destiny == actual + possible_dir){
              dir = possible_dir;
              break;
          }
        }

        //if the position is available execute command
        bool position_available = position_is_available(actual + dir, my_orks);
        if (position_available and not incorrect_info) {
          execute(Command(id, dir));
          now.pop();
          Caminos[id] = now;
        }

        //if occupied, go to a random available position inside city or path
        //and recalculate new path for the ork
        if (not position_available) {
          vector <int> perm = random_permutation(4);
          for (int i = 0; i < 4; ++i) {
            Dir d = Dir(perm[i]);
            Cell possible_cell = cell(actual + d);
            if ((possible_cell.type == CITY or possible_cell.type == PATH) and
                position_is_available(actual + d, my_orks)) {
              execute(Command(id, d));
              Pos fin = find_destiny_civilization(actual, id);
              recalculate_path(id, actual, fin);
              city_reached[id] = fin;
              break;
            }
          }
        }
      }
      else {
        //create new route if the orc has spawned recently
        Pos fin = encuentra_destino_city(actual, id);
        recalculate_path(id, actual, fin);
        city_reached[id] = fin;
      }
    }
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
