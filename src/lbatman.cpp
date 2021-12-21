#include "Layout.h"

using namespace std;

namespace cpmp {

    int Layout::min_bad_located(){
        int menor=8000000;
        
        for (int hh=0;hh<stacks.size();hh++)
        {
            if (H==sorted_elements[hh]) continue;

            int hhh =stacks[hh].size() - sorted_elements[hh];
            if (menor > hhh) menor = hhh;
        }

        return menor;
    }

    void Layout::compute_costs(int gv, vector<int>& costs)
        {
            
            for (int i = 0;i<stacks.size();i++)
            {
                int cost = 0;

                for(int k=sorted_elements[i]-1; k>=0;k--){
                    if (stacks[i][k]<gv) cost++;
                    else break;
                }

                costs[i]=cost;      
            }
        }


    void Layout::cummulative_demand(map <int, int, std::greater<int> >& demand){
        //demand for each gv
        for (int i=0;i<stacks.size();i++)
        {
            //unsorted items
            for(int j=sorted_elements[i]; j<stacks[i].size();j++){
                int c = stacks[i][j];
                if (demand.find(c)!=demand.end()) demand[c]+=1;
                else demand[c]=1;
            }
        }

        //cumulative demand
        int acum = 0;
        for(pair<int,int> d : demand){
            acum+=d.second;
            demand[d.first]=acum;
        }
    }

    void Layout::availability(map <int, int, std::greater<int>>& available){
        for (int i = 0;i<stacks.size();i++){
            if (sorted_elements[i]==0){
                int c=*gvalues.begin();
                if (available.find(c)!=available.end()) available[c] += H;
                else available[c] = H;
            }

            for(int j=0; j<sorted_elements[i]; j++){
                int c = stacks[i][j];
                if (available.find(c)!=available.end()) available[c]+= H-j-1;
                else available[stacks[i][j]] =  H-j;
            }
        }
    }

    pair<int,int> Layout::gv_max_lack(map <int, int, std::greater<int>>& demand, map <int, int, std::greater<int>>& available){
        int acum = 0;
        int Emax=0,gvv=-1;
        for(int gv : gvalues){
            if(available.find(gv)!=available.end()) acum+=available[gv];
            if(demand.find(gv)!=demand.end()){
                int E=demand[gv]-acum; //carencia(gv)
                if (E>Emax) {
                    Emax=E;
                    gvv = gv;
                }
            }   
        }
        return make_pair(gvv,Emax);
    }

    



    int Layout::lb2(){
        int bx = unsorted_elements;
        int Nbx = bx+min_nx(), Ngx=0;

        //faster and worse version
        //this->lb = Nbx+Ngx + steps;
        //return this->lb;


        
        map <int, int, std::greater<int> > demand;
        cummulative_demand(demand);
        
        map <int, int, std::greater<int>> available; //gv -> D
        availability(available);
        

        //maxima carencia
        pair<int,int> p=gv_max_lack(demand, available);
        int gvv=p.first;
        int Emax=p.second;
        //cout << "Emax:" << Emax << ", " << "gv:" << gvv <<  endl;

        //stacks que se deben desmantelar
        int n = ceil((double)Emax/(double)H-0.0001);

        
        //ordenar stacks menor a mayor cantidad de items ordenados con gv<gvv
        //recorrer los primeros n
        if(n>0){
            list<int> nrs;
            for (int i = 0;i<stacks.size();i++){
                //conisdered by availability
                if(sorted_elements[i] == 0 || stacks[i][sorted_elements[i]-1]>=gvv) continue; 

                int nr=1; //at least one item should be removed
                for(int j=sorted_elements[i]-2; j>=0; j--){
                    if (stacks[i][j] < gvv) nr++;
                    else break;
                }

                nrs.push_back(nr);
            }

            nrs.sort();
            for(int nr:nrs){
                n--;
                Ngx+=nr;
                if(n==0) break;
            }

        }

        //cout << Nbx << "+" << Ngx << "+" << steps << endl;
        this->lb = Nbx + Ngx + steps;
        return this->lb;


    }    

    int Layout::lbatman(bool verbose){
        int bx = unsorted_elements;
        int Nbx = bx+min_nx();

        int lb= Nbx+steps;
        this->lb = lb;
        return lb;

        vector <int> disponible(n_stacks);
        vector <double> multiplicador(n_stacks);
        vector <int> costs(n_stacks);
        vector <int> benefits(n_stacks);

        /* El resto no es correcto :c */
            
       
        //Contenedores que se deben colocar  (en orden decreciente)
        map <int, int, std::greater<int> > values;
        map <int, int, std::greater<int> > sort_values;
        
        for (int i=0;i<stacks.size();i++)
        {
            //unsorted items
            for(int j=sorted_elements[i]; j<stacks[i].size();j++){
                int c = stacks[i][j];
                if (values.find(c)!=values.end()) values[c]+=1;
                else values[c]=1;
            }

            //sorted items
            for(int j=0; j<sorted_elements[i];j++){
                int c = stacks[i][j];
                if (sort_values.find(c)!=sort_values.end()) sort_values[c]+=1;
                else sort_values[c]=1;
            }

            disponible[i]=(H-sorted_elements[i]);
            multiplicador[i]=1.0;
        }

        if(verbose){
            cout << "Disponible:";
            for(int d:disponible) cout << d << " ";  cout << endl;
            cout << "Multiplicador:";
            for(double d:multiplicador) cout << d << " ";  cout << endl;
            cout << "Values:";
            for(auto v:values) cout << v.first << " ";  cout << endl;
            cout << "----" << endl;
        }

        //values: keys->apariciones
        double costoTotal = 0;


        for (auto vit=values.begin(); vit!=values.end();)
        {
            //ACOTACION DEL PROFE
            //Juntar stacks con  lb(layout,gv) < gv <= ub(layout,gv)
            //Por ejemplo si en layout hay gvs 3 7 8, 
            //podemos juntar contenedores con gv: 7 6 5 4 y consideralos como si gv=4 
            //(ya que se pueden colocar sobre 7 pero no sobre 3)
            //busco valor anterior a v, v'
            //considero todos los valores gv>v' y los considero como v'+1

            int vv = -1;
            auto vvit = sort_values.upper_bound(vit->first);
            if(vvit!=sort_values.end()) vv = vvit->first;
            if (vv ==-1) break;

            int k = vit->first; // gv del contenedor
            int q = vit->second; // cantidad con mismo gv

            while(vit!=values.end()){
                vit++;
                if (vit->first <= vv || vit==values.end()) break;
                k = vit->first;
                q += vit->second;
            }
            
            //cout << k << " " << q << endl;
            

            while(q>0){
                //se obtienen los costos para cada stack
                compute_costs(k, costs);

                for (int i=0;i<stacks.size();i++)            
                    benefits[i] = costs[i]+disponible[i];
                

                //Se obtiene stack con mayor beneficio
                int bestIndex;
                float max_bdc = 0;
                float m = 0;
                for (int i=0;i<stacks.size();i++)
                {
                    if(multiplicador[i]==0) continue;

                    if (costs[i]==0){
                        bestIndex = i; break;
                    }

                    double bdc= (double)benefits[i]/(double)costs[i];
                    
                    if (bdc>max_bdc)
                    {
                        max_bdc = bdc;
                        bestIndex = i;
                    }
                }

                if(verbose){
                    cout << "Mejor compromiso("<<k<<"): stack " << bestIndex << endl;
                    cout << "bc: " << benefits[bestIndex] << "/" << costs[bestIndex] << endl;
                }

                //porcion de stack que se debe descartar
                double discard = std::min((double)q/(double)benefits[bestIndex],multiplicador[bestIndex]);
                costoTotal += discard*(double)costs[bestIndex];
                q -= discard*(double)benefits[bestIndex]; //cantidad de contenedores que se colocan
                multiplicador[bestIndex] -= discard;

                if(verbose){
                    cout << "Multiplicador:";
                    for(double d:multiplicador) cout << d << " ";  cout << endl;
                    cout << "Costo Total:" << costoTotal << endl;
                    cout << "----" << endl;
                }

                //se descarta el stack cuando el multiplicador llega a 0
                if(multiplicador[bestIndex] < 0.001)
                    multiplicador[bestIndex]=0;

                if(q<0.001) q=0.0;
                
            }
        
        }
        this->lb=lb + ceil(costoTotal-0.001);
        return lb + ceil(costoTotal-0.001);
    }
}