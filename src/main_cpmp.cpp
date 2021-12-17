
#include <map>
#include <vector>
#include <list>
#include <iterator>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
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
        Nodo * padre;
        list <Nodo *> hijos;
        int nivel;
        bool greedy_child;
        int score = 0;

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

                        actual->lbatman();

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
        root->actual->lbatman();
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
            //if (l-temp->actual->steps < 10){
            //    u = greedy(temp->actual, temp->actual->steps+10);
            //    if (u==-1) u=1000;}

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
};

int main(int argc, char * argv[]){
    Layout::H = atoi (argv[1]);
    string path(argv[2]);
    Layout L(path);

    Layout * nuevo = new Layout(L);
  
    const clock_t begin_tree = clock();
    Tree::search2(nuevo,0);
    //Tree * arbolTest= new Tree(nuevo,0);

    cout << (float( clock () - begin_tree ) /  CLOCKS_PER_SEC) << endl;
    //delete arbolTest;
    return 0;    

}

