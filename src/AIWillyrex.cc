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

  //Vector g represents a graph
  vector<Pos> g;

  map<int,bool> city_given;
  map<int,bool> path_given;

  //Checks if an orc has arrived at the final position
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
                else if(i == 1) v=Pos(u.i, u.j + 1); //right
                else if(i == 2) v=Pos(u.i - 1, u.j); //up
                else if(i == 3) v=Pos(u.i + 1, u.j); //down

                Cell aux = cell(v.i, v.j);
                if(aux.type != WATER){
                    int c = cost(aux.type); //candidate cost
                    if (d[v] > d[u] + c) { /**si la distancia es mas grande la actualizamos, buscamos un mínimo*/
                        d[v] = d[u] + c;
                        p[v] = u;  //guardamos en el map
                        Q.push(ArcP(d[v], v)); //encolamos Arcp (int,Pos)
                    }
                }
            }
        }
    }
}

void inicializar_grafo_tablero(vector<Pos> &g){
    for (int i = 0; i < rows(); ++i)
        for (int j = 0; j < cols(); ++j) {
        if (cell(i,j).type != WATER) {
            g.push_back(Pos(i,j));
        }
}
}

double distancia(Pos pos1, Pos pos2){
    return sqrt(pow(pos1.i-pos2.i,2)+pow(pos1.j-pos2.j,2));
}


Pos encuentra_destino_path(Pos spawned){
    double minima = infinity_double;
    Pos candidata = spawned;
    for(int i = 0; i<nb_paths();++i){
      if(not path_given[i]){
        Path path_info = path(i);
        vector <Pos> aux = path_info.second;
        double calcul;
            for (int i = 0;i<int(aux.size());++i){
                calcul = distancia(aux[i],spawned);
                if (calcul<minima){
                    minima = calcul;
                    candidata = aux[i];
                }
            }
        }
    }
    Cell c = cell(candidata);
    path_given[c.path_id] = true;
    return candidata;
}



Pos encuentra_destino_city(Pos spawned, int id){
    double minima = infinity_double;
    Pos candidata;
    bool found = false;
    bool danger = false;
    for(int i = 0; i<nb_cities();++i){
        if(not city_given[i]){
            found = true;
            City city_info = city(i);
            vector <Pos> aux = city_info;
            double calcul;
            for (int i = 0;i<int(aux.size());++i){
                Cell c = cell(aux[i].i,aux[i].j);/**/
                if(c.unit_id!=-1){
                    Unit enemy = unit(c.unit_id);
                    Unit my_ork = unit(id);
                    if(enemy.player!=me() and enemy.health>my_ork.health) danger = true;
                }
                calcul = distancia(aux[i],spawned);
                    if (calcul<minima and not danger){
                    minima = calcul;
                    candidata = aux[i];
                    }
                    if(danger){ /** desactiva peligro*/
                        danger = false;
                        found=false;
                        break;
                    }
            }
        }
    }
    if (not found) return encuentra_destino_path(spawned);
    Cell c = cell(candidata);
    city_given[c.city_id] = true;
    return candidata;
}

  bool check(int id){
    for(map<int, stack<Pos>>::iterator it = Caminos.begin(); it!=Caminos.end(); ++it){
        if(it->first == id) return true;
    }
    return false;
  }

  void recalculate_path(int id, Pos actual, Pos fin){
	map<Pos,int> d;
	map<Pos,Pos> p;
	path_finding(g,actual,fin,d,p);
	Caminos[id] = generate_path(p,actual,fin);
}

 bool pos_correcta(Pos pos, Vector &my_orks){
	 Cell c = cell(pos.i,pos.j);
	 for (int i = 0; i<int(my_orks.size());++i){
		if(c.unit_id == my_orks[i]) return false;
	 }
	 return true;
}


  /**
   * Play method, invoked once per each round.
   */

  void enemy_near(int id){
    Unit ork = unit(id);
    Pos near;
    for (int i = 0; i<8;++i){
        if(i==0) near = Pos(ork.pos.i+1,ork.pos.j+1);
        else if(i==1) near = Pos(ork.pos.i+1,ork.pos.j);
        else if(i==2) near = Pos(ork.pos.i+1,ork.pos.j-1);
        else if(i==3) near = Pos(ork.pos.i,ork.pos.j-1);
        else if(i==4) near = Pos(ork.pos.i-1,ork.pos.j-1);
        else if(i==5) near = Pos(ork.pos.i-1,ork.pos.j);
        else if(i==6) near = Pos(ork.pos.i-1,ork.pos.j+1);
        else if(i==7) near = Pos(ork.pos.i,ork.pos.j+1);

        Cell c = cell(near);
        if(c.unit_id!=-1) {
            Unit enemy = unit(c.unit_id);
            if((c.type==PATH or c.type==CITY) and enemy.player!=me() and enemy.health<ork.health){
                recalculate_path(id,ork.pos,near);/**Ves a donde esta el rival*/
                city_reached[id]=near;
                return;
            }
        }
  }
  }




  virtual void play () {

    if(status(me())>=0.96) return;


     /**Declaraciones que actuan*/
     Vector my_orks = orks(me()); // Get the id's of my orks.

      //Process orks in random order.
      //Vector perm = random_permutation(my_orks.size())


    /**Actualiza donde se estan moviendo los orcos*/
    for (int i = 0;i<nb_cities();++i){
        if (city_owner(i)!=me()) city_given[i]=false;
    }

    for (int i = 0; i<nb_paths();++i){
        if(path_owner(i)!=me()) path_given[i]=false;
    }


     if (round() == 0) {
      //localizacion_orcos = Matrix(rows(), Vector(cols(), -1));
      inicializar_grafo_tablero(g);
      //cerr << "grafo inicializado" << endl;

      /**for (int i = 0; i < rows(); ++i)
        for (int j = 0; j < cols(); ++j)
          localizacion_orcos[i][j] = cell(i, j).unit_id; // Update for the next round.
    }
    */

      /** Calculo de todos los caminos ha seguir por los orcos*/

      for (int k = 0; k < int(my_orks.size()); ++k){
        Unit ork = unit(my_orks[k]);
        Pos initial = ork.pos;
        Pos fin = encuentra_destino_city(ork.pos,ork.id);
        map<Pos,int> d;
        map<Pos,Pos> p;
        path_finding(g,initial,fin,d,p);
        stack<Pos> aux = generate_path(p,initial,fin);
        Caminos[my_orks[k]] = aux;
        city_reached[my_orks[k]] = fin;
      }
}

    /**Mandamos los orcos a la ciudad con una estrategia greedy*/
    for (int i = 0; i < int(my_orks.size()); ++i){
        int id = my_orks[i];
        Unit ork = unit(id);
        Pos actual = ork.pos;

			if(not check(id)){
			    Pos fin = encuentra_destino_city(actual,id);
                recalculate_path(id,actual,fin); /** Comprueba que este en el mapa, si es nuevo dale un camino*/
                city_reached[id] = fin;
			}

        if (actual == city_reached[id]){ /**Ya ha acabado su recorrido inicial*/
                Pos fin = encuentra_destino_city(actual,id);
				recalculate_path(id,actual,fin);
				city_reached[id] = fin;
                stack<Pos> now = Caminos[id];
        }

        Cell c = cell(ork.pos);
        if (c.type == CITY or c.type == PATH) enemy_near(id);

        stack<Pos> now = Caminos[id];
        if(not now.empty()){

            Pos destino = now.top();
            bool informacion_incorrecta = false;
            Dir dir;

                for (int d = 0; d!=DIR_SIZE;++d){ /** averigua cual es la dirección que el orco tiene que segir*/
                    Dir posible_dir = Dir(d);
                    if(d==NONE){
                        Pos fin = encuentra_destino_city(actual,id);
                        recalculate_path(id,actual,fin); /** Si tiene información que no toca vuelve a calcular el path*/
                        informacion_incorrecta = true;
                    }
                    if(destino == actual + posible_dir){
                        dir = posible_dir;
                        break;
                    }
                }

            bool correcto = pos_correcta(actual+dir,my_orks);
            if(correcto and not informacion_incorrecta){
                execute(Command(id,dir));
                now.pop();
                Caminos[id] = now;
            }
            if (not correcto) {
              vector <int> perm = random_permutation(4);
              for (int i = 0; i<4;++i){
                Dir d = Dir(perm[i]);
                Cell intento = cell(actual+d);
                if ((intento.type == CITY or intento.type == PATH) and pos_correcta(actual+d,my_orks)){
                    execute(Command(id,d));
                    Pos fin = encuentra_destino_city(actual,id);
                    recalculate_path(id,actual,fin);
                    city_reached[id] = fin;
                    break;
                }
              }
            }
        }
        else{
             Pos fin = encuentra_destino_city(actual,id);
              recalculate_path(id,actual,fin); /** Comprueba que este en el mapa, si es nuevo dale un camino*/
                city_reached[id] = fin;
        }
    }

}

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);

