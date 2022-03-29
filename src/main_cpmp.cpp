
#include <map>
#include <vector>
#include <list>
#include <iterator>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <random>
#include <stdexcept>
#include<ctime>
#include "dirent.h"
#include "Layout.h"
#include "Greedy.h"
#include "Bsg.h"
#include <queue>
#include <stack>

using namespace std;
using namespace cpmp;


//DEJAR DENTRO DEL LAYOUT
    
class Nodo
{
    public:
        Layout * actual;
        Nodo * padre = NULL;
        int n_children=0;
        list < pair<int, int> > valid_moves;
        list < pair<int, int> > simulation;

        int nivel;
        bool greedy_child;
        bool selected = false;
        double score = 0;
        int ub = 0;
        

    /*************************************************************/
    ///////////////////////BOB EL CONSTRUCTOR//////////////////////
    /*************************************************************/
    
    //HACER UN DESTRUCTOR
        Nodo(Layout * l, int lvl, Nodo * padre=NULL) : actual(new cpmp::Layout(*l)), 
        greedy_child(false), nivel(lvl), padre(padre)
        {
            
        }
    /*************************************************************/
    ///////////////////////Funciones///////////////////////////////
    /*************************************************************/

        ~Nodo()
        {
            delete(actual);
        }

        Nodo* next_child(int U, pair <int,int> default_action = make_pair(-1,-1) ){
            //cout << "** next child ("<< default_action.first << "," << default_action.second << ")**" << endl;
            //cout << n_children << endl;
            if(n_children == 0){
                int stacks = actual->stacks.size();
                int i,j;
                int h = actual->H;
                //Por cada stack
                for (i=0;i<stacks;i++)
                    for (j=0;j<stacks;j++){
                        if(default_action.first==i && default_action.second==j) continue;
                        //Si la columna actual no tiene tamaño 0 y Si la columna objetivo no esta llena
                        if (i != j && actual->stacks[i].size() != 0 && actual->stacks[j].size() != h && 
                                            actual->validate_move(i,j) &&  actual->validate_move2(i,j)){
                            int c = actual->stacks[i].back(); 
                            int top_dest = Layout::gvalue(actual->stacks[j]);

                            valid_moves.push_back( make_pair(i,j) );

                        }
                    }

                //std::random_device rd;
                //std::mt19937 generator(rd());
                //std::shuffle(valid_moves.begin(), valid_moves.end(), generator);  

                if (default_action.first!=-1) valid_moves.push_front(default_action);
            }

           
            while(valid_moves.size()>0){
                pair <int,int> move = valid_moves.front();
                valid_moves.pop_front();
                int i=move.first, j=move.second;

                int aux=actual->lb;
                actual->move(i,j,false);
                actual->lb2();
                int new_lb=actual->lb;
                actual->lb=aux; 
                actual->move(j,i,false); actual->steps-=2; 
               
            
                if (new_lb < U){
                    //Se crea un nuevo nodo
                    Nodo * niu = new Nodo(actual,(nivel)+1,this);
                    //Se realiza el movimiento
                    niu->actual->move(i,j,false);
                    niu->actual->lb = new_lb;

                    n_children++;
                    //Se retorna
                    return niu;
                }
            }

            return NULL;

        }

         void get_children(list <Nodo*> &children, int U)
        {
            int stacks = actual->stacks.size();
            int i,j;
            int h = actual->H;
            //Por cada stack
            for (i=0;i<stacks;i++)
            {
                //Se mueve al resto
                for (j=0;j<stacks;j++)
                {
                    //Si no es el mismo stack
                    //+
                    //Si la columna actual no tiene tamaño 0 y Si la columna objetivo no esta llena
                    if (i != j && actual->stacks[i].size() != 0 && actual->stacks[j].size() != h && 
                                        actual->validate_move(i,j) &&  actual->validate_move2(i,j))
                    {
                        int c = actual->stacks[i].back(); 
                        actual->move(i,j);
                        int aux=actual->lb;

                        actual->lb2();

                        actual->move(j,i); actual->steps-=2; 
                        actual->seq.pop_front(); actual->seq.pop_front(); 

                        if (actual->lb < U){

                            //Se crea un nuevo nodo
                            Nodo * niu = new Nodo(actual,(nivel)+1,this);
                            //Se realiza el movimiento
                            niu->actual->move(i,j);
                            niu->actual->lb = actual->lb;

                            //Se inserta en LA COLA
                            children.push_back(niu);
                        }

                        actual->lb=aux;
                            
                    }
                }
            }
           // return pendientes;
        }       
    //****************************************************************************
        
        
};

class compare_nodes2
{

public:
  bool operator() (const Nodo* lhs, const Nodo* rhs) const
  {
    if (lhs->actual->lb >= rhs->actual->lb) return (true);
    else return (false);
  }
};

class compare_nodes3
{

public:
  bool operator() (const Nodo* lhs, const Nodo* rhs) const
  {
    if (lhs->score <= rhs->score) return (true);
    else return (false);
  }
}; 

class Tree
{
    public:
        Nodo * base;
        int limite;
        //stack <Nodo*> S;
        priority_queue<Nodo*, vector<Nodo*>, compare_nodes2> S;

        int contador=0;
        int nivel;
        Nodo * mejor;
        //Constructores
    

    /*************************************************************/
    ///////////////////////Funciones///////////////////////////////
    /*************************************************************/
    //DICE LOS PASOS TOTALES
    static int greedy(Layout *  L, int u=1000)
        {
            int steps;
            int type=ATOMIC_MOVE;
            bool PIXIE=true;
            int beams=0;
            Layout h = *L;
            Layout * nuevo = new Layout(h);
            Layout sol = *nuevo;
            if(PIXIE) steps = pixie_solve(sol,u);
            else steps = greedy_solve(sol,u);
            
            delete(nuevo);
            
            return steps;
        }


    
    /*************************************************************/

    float eva(Nodo * n,int lower)
        {
            Layout * L = n->actual;
            //int l = (L->lb)-lower;
            //return l;
            return L->lb;
        }

    // comparison, not case sensitive.
    static bool compare_nodes (const Nodo* n1, const Nodo* n2)
    {
        return ( (*n1).score < (*n2).score );
    }


    Tree(Layout * l, int lvl)
    {
        search2(l, lvl);
        //cout << "FINISH HIM\n";
    }

    static void search2(Layout* l, int lvl)
    {
        map <int,int> lbs;
        Nodo* root = new Nodo(l,lvl,NULL);

        //Se calcula lb de la raiz
        root->actual->lb2();
        lbs[root->actual->lb]=1; 
        
        //Para almacenar nodos
        priority_queue<Nodo*, vector<Nodo*>, compare_nodes2> S;
        S.push(root);
            
        int L = root->actual->lb;
        int U = greedy(root->actual);
        
        int contadorDeNodos = 0;
        while (S.size()!=0)
        {
            //Se obtiene el elemento top del stack
            Nodo* temp = S.top(); S.pop();

            int l = temp->actual->lb;
            lbs[l]--; if(lbs[l]==0) lbs.erase(l);
            
            if (l >= U){
                if (L == U){
                    cout << U << " " << contadorDeNodos << " ";
                    return;
                }
                delete(temp);
                continue;
            }

            int u=1000;

            //Estado final
            if (temp->actual->unsorted_stacks==0) u=temp->actual->steps;
            
            if (u < U){
                U = u;
                if (l >= U){
                    delete(temp);
                    continue;
                }
            }
    
            if (L == U)
            {
                cout << U << " " << contadorDeNodos << " ";
                return;
            }

            contadorDeNodos ++;

            list <Nodo*> children;
            temp->get_children(children, U);

            for (Nodo* aux:children){
                S.push(aux);
                lbs[aux->actual->lb]++;
            }

            children.clear();
            delete(temp);

            //actualizar el lower bound
            L = lbs.begin()->first;
        }

        cout << U << " " << contadorDeNodos << " ";


        
    }
     /*************************************************************/


static int simulate(Nodo* n, map< int, priority_queue<Nodo*, vector<Nodo*>, compare_nodes3> >& Qs, int U){
    //cout << "** simulation **" << endl;
    
    Nodo* child = (n->padre == NULL && n->n_children==0)? n : n->next_child(U);
    if(!child) {
        //cout << "no child" << endl;
        return 1000;
    }

    int lb = child->actual->lb;
    int ub = n->ub;
    list < pair <int,int> > seq = n->simulation;
    n->simulation.clear();

    if(seq.size()==0){ //si no ha sido simulada la rama
        Layout lay=*child->actual;
        //lay.print();
        ub = greedy_solve(lay,U);
        //lay.print();

        if(ub==-1) ub = U+lay.unsorted_elements;
        seq=lay.seq;
    }

    Nodo* ch = child;
    while(seq.size()>0){
        //cout << seq.back().first << seq.back().second << endl;
        ch = child->next_child(U,seq.back()); seq.pop_back();
        if (!ch) break; //child lb equal or worst than UB

        ch->ub=ub; //last ub
        //prioriza menos hijos, más profundidad, menor ub
        ch->score  = -100*ch->n_children - ch->ub + 0.01*ch->actual->steps;
        Qs[ch->actual->lb].push(ch);

        if(ch->actual->lb > lb) {
            ch->simulation = seq;
            break;
        }
    }
    //cout << "** end simulation ("<< ub <<") **" << endl;
    return ub;
}


 //lb dynamic search
 static void search3(Layout* l, int lvl)
    {
        //colas para guardar nodos con distintos lb
        map< int, priority_queue<Nodo*, vector<Nodo*>, compare_nodes3> > Qs;
        map< int, int > sel;
        map< int, int > max_children;

        Nodo* root = new Nodo(l,lvl,NULL);
        root->actual->lb2();
        root->selected = true;
        int U = simulate(root, Qs, 1000), L=root->actual->lb; 
        Qs[root->actual->lb].push(root);


        int sims=0, current_lb=0;
        int contadorDeNodos = 0;

        cout << root->actual->lb << ";" << U << endl;

        while ( Qs.size()!=0 ){
            //selecting the node to simulate
            sims++;
            //TODO: condición para aumentar current_lb
            while(Qs.find(current_lb) == Qs.end() ) 
                current_lb = (current_lb+1)%U;

            if (max_children.find(current_lb) == max_children.end()) max_children[current_lb] = 1;

            //se verifica condición para seleccionar
            //que la cantidad de nodos seleccionados con = lb sea menor a max_children
            Nodo* n = NULL;
            bool done = false;
            //cout << 1 << endl;
            while(!done){
                n = Qs[current_lb].top(); Qs[current_lb].pop();
                

                done = true;
                //cout << current_lb << ":" << n->score << "," << sel[current_lb] << "," << max_children[current_lb] <<","<< Qs[current_lb].size() << endl;
                if (Qs[current_lb].size()==0) Qs.erase(current_lb);

                /*if (n->selected==false && sel[current_lb]>=max_children[current_lb]){
                    n->score  = -100*(sel[current_lb]+1) - n->ub + 0.01*n->actual->steps;
                    
                    Qs[current_lb].push(n); done=false;
                }else if (n->selected){
                    if (max_children.find(current_lb) == max_children.end() || n->n_children > max_children[current_lb])
                        max_children[current_lb]=n->n_children;
                }*/
            }
            //cout << 2 << endl;

            //count selected nodes with current_lb
            if(n->selected == false) sel[current_lb]++;      
            n->selected=true;
            if(sel.find(current_lb) == sel.end()) sel[current_lb]=0;
               

            //cout << "level:" << n->nivel << endl;
            int u = simulate(n, Qs, U);
            cout << current_lb <<"," << U << endl;

            if (u < U) {
                for(;U>u;U--)
                    Qs.erase(U);
                Qs.erase(U);
                

                cout << U << endl;
                if (current_lb >= U) {
                    sel[current_lb]--; 
                    delete(n);
                    continue;
                }
            }

            //compute Lower bound
            if(n->valid_moves.size()>0) {
                n->score = -100*n->n_children - n->ub + 0.01*n->actual->steps;
                //cout << n->score << endl;
                Qs[current_lb].push(n);
            }else
                sel[current_lb]--; 
            
            L=min(L,Qs.begin()->first);

            if (L == U) break;
            
        }


        cout << U << " " << sims << " ";


        
    }

};

int main(int argc, char * argv[]){
    Layout::H = atoi (argv[1]);
    string path(argv[2]);
    Layout L(path);

    Layout * nuevo = new Layout(L);
    //L.print(L.stacks);
    //cout << "lb:" << nuevo->lb2() << endl;
    //cout << endl;
    
    //exit(0);


  
    const clock_t begin_tree = clock();
    //Tree::search2(nuevo,0);
    Tree::search3(nuevo,0);

    cout << (float( clock () - begin_tree ) /  CLOCKS_PER_SEC) << endl;
    //delete arbolTest;
    return 0;    

}

