#include "AI/AstarPathfinding.h"

AstarPathfinding* AstarPathfinding::instance = NULL;

AstarPathfinding::AstarPathfinding(bool *matrix, int w, int h, int distMax)
{
    //ctor
    this->matrix = matrix;
    this->w = w;
    this->h = h;
    this->dist_max = distMax;
    AstarPathfinding::instance = this;
}

AstarPathfinding::~AstarPathfinding()
{
    //dtor
}

void AstarPathfinding::getPath(float srcX, float srcY, float destX, float destY, PointList_t &path){

    ///init
    this->srcP.x = srcX;
    this->srcP.y = srcY;
    this->destP.x = destX;
    this->destP.y = destY;
    clearList(path);
    clearList(openedList);
    clearList(closedList);

    ///set the first point
    currentP = new Point_t();
    currentP->x = srcX;
    currentP->y = srcY;
    currentP->costa = 0.f;
    currentP->costb = (destX - srcX)*(destX - srcX) + (destY - srcY)*(destY - srcY);
    currentP->totalcost = currentP->costa + currentP->costb;
    openedList.push_back(currentP);

    ///perform
    while(!openedList.empty() && !(currentP->x == destX && currentP->y == destY) ){
        //std::cout << "currentP = " << currentP->x << "; " << currentP->y << "\n";

        openedList.remove(currentP);
        closedList.push_back(currentP);
        perform();

        currentP = getBest();
    }


    ///return result
    if(currentP != NULL && currentP->x == destX && currentP->y == destY){
        do{
            Point_t* p = new Point_t();
            p->x = currentP->x;
            p->y = currentP->y;
            path.push_front(p);
            currentP = currentP->previous;
        }while(currentP != NULL);
    }

    clearList(openedList);
    clearList(closedList);
}


void AstarPathfinding::clearList(PointList_t &list){
    for(PointList_t::iterator it = list.begin(); it != list.end(); it++)
        delete (*it);

    list.clear();
}

void AstarPathfinding::perform(){
    for(int i=currentP->x-1; i<=currentP->x+1; i++)
    for(int j=currentP->y-1; j<=currentP->y+1; j++){


        ///elimination des cas 8-connexe
        /*if(i == currentP->x-1 && j == currentP->y-1)continue;
        if(i == currentP->x-1 && j == currentP->y+1)continue;
        if(i == currentP->x+1 && j == currentP->y-1)continue;
        if(i == currentP->x+1 && j == currentP->y+1)continue;*/

        ///elimination de la tuile courante
        if(i == currentP->x && j == currentP->y)continue;

        ///elimination des points hors zone
        if(i < 0 || i>w-1 || j<0 || j>h-1)continue;

        ///elimination des tuiles obstacles
        if(matrix[h * i + j])continue;

        ///elimination des tuiles deja visites
        if(existsIn(i, j, &closedList) != NULL)continue;


        //std::cout << ":::" << i << "; " << j << "\n";

        Point_t* p = NULL;
        p = existsIn(i, j, &openedList);
        float costa = currentP->costa+1;
        ///Cas ou la tuile est nouvelle
        if(p == NULL){
            ///On ajoute la tuile à la liste des tuiles à visiter
            p = new Point_t();
            p->x = i;
            p->y = j;
            p->costa = costa;
            p->costb = (destP.x - i)*(destP.x - i) + (destP.y - j)*(destP.y - j);
            p->totalcost = costa + p->costb;
            p->previous = currentP;
            openedList.push_back(p);
            //std::cout << "pas existant\n";
        }
        ///Cas ou la tuile a deja recu une valeur
        else{
            if(p->costa > costa){
                ///Si meilleur cout, alors mise a jour
                p->costa = costa;
                p->totalcost = costa + p->costb;
                p->previous = currentP;
                //std::cout << "existant\n";
            }
        }
    }
}


Point_t* AstarPathfinding::existsIn(float x, float y, PointList_t* list){
    for(PointList_t::iterator it = list->begin(); it != list->end(); it++){
        if((*it)->x == x && (*it)->y == y)return (*it);
    }
    return NULL;
}

Point_t* AstarPathfinding::getBest(){
    Point_t* best = NULL;
    for(PointList_t::iterator it = openedList.begin(); it != openedList.end(); it++){
        if(best == NULL || best->totalcost > (*it)->totalcost)best = (*it);
        //if((*it)->costb == 0)return (*it);
    }
    return best;

}
