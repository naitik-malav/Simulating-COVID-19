#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define MAX_VERTICES 100
#define MAX_EDGES 30


typedef struct vertex{ //Struct that represents a vertex in the graph
  int id; //ID of the vertex
  char status; //Current Status od the vertex with regard to the infection
  int pred_inf_time; //Predicted time to get infected
  int rec_time; //Time required to recover
}vertex;

typedef struct event{ //Struct for the events
  vertex* node; //The node that the event corresponds to
  int time; //The time after which it will be executed.
  char action; //The action the event corresponds to.
}event;

typedef struct listNode{ //Struct to create linked list for storing the S,I and R vertices.
  vertex* vert;
  struct listNode* next;
}lNode;


event **priorityQ; //Priority Queue
int q_size=0; //variable to store the size of priority queue

lNode* s_list=NULL; //Susceptible List
lNode* i_list=NULL; //Infected List
lNode* r_list=NULL; //Recovered List

int s_size=0;
int i_size=0;
int r_size=0;

int max_time=6; //Max time the simulation will run for.

int currDay=0; //Variable to store the current day.

int adj_matrix[10000][10000]; //Create Adjacency matrix to create the graph

void Insert(lNode** head,vertex* v); //Prototype
void print_list(lNode* head); //Prototype
void Shift(lNode** from_head,lNode** to_head,vertex* v); //Prototype
void Fast_SIR(vertex *G,float tao,float gamma,int* initial_infecteds,int g_size,int (*adj_matrix)[g_size],int t_max,int inf_size); //Prototype
void Swap(event** a,event** b); //Prototype
void min_heapify(event **q,int i,int *size); //Prototype
event extract(event **q,int* size); //Prototype
void InsertQ(event **q,event *e,int *size); //Prototype
void PrintEventQ(event** pq,int size,int day); //Prototype
int getDays(float prob); //Prototype
void process_trans_SIR(vertex *G,vertex* target,float tao,float gamma,int time,int g_size,int (*adj_matrix)[g_size]); //Prototype
void process_rec_SIR(vertex* v,int time); //Prototype
int minimum(int a,int b,int c); //Prototype
void printAll(); //Prototype



int main()
{

  priorityQ = (event**)malloc(MAX_VERTICES*sizeof(event*)); //Allocate Memory to the priority queue double pointer.
  float tao=0.5; //Probability of getting infected.
  float gamma=0.2; //Probability of getting recovered.

  srand(time(NULL)); //Seed Random function with time to increase randomness.
  int noOfVert = 75;//rand()%MAX_VERTICES; //Generate random no that gives number of vertices in graph.

  int maxNoOfEdges = 7;//rand()%MAX_EDGES; //Generate random no that gives max number of edges in graph.

  int minVert=6; //Minimum vertices
  int minEdges=2; //Minimum edges of a vertix

  if(noOfVert<minVert)
    noOfVert=minVert;

  if(maxNoOfEdges<minEdges)
    maxNoOfEdges=minEdges;


  int noOfEdges[noOfVert];
  for(int i=0;i<noOfVert;i++)
    noOfEdges[i]=0;

  for(int i=0;i<noOfVert;i++){
    for(int j=0;j<maxNoOfEdges;j++){
      if(rand()%2==1){
        if(noOfEdges[i]>=maxNoOfEdges)
          break;
        int rndm = rand()%noOfVert;
        if(adj_matrix[i][rndm]==1 || rndm==i){
          j--;
        }else{
          if(noOfEdges[rndm]>=maxNoOfEdges){
            j--;
            continue;
          }
          adj_matrix[i][rndm]=1;
          adj_matrix[rndm][i]=1;
          noOfEdges[i]++;
          noOfEdges[rndm]++;
        }
      }
    }
  }


  vertex graph[noOfVert]; //Create array of verteces
  int *already_infected; //Create pointer to store the id of the already infected people
  int noOfInitInf=0; //variable to store the number of vertices initially infected.

  printf("********************************************SIR Epidemic Simulation**************************************************************\n");
  printf("\nTotal Vertices : %d\n",noOfVert); //Show total vertices tot he user.
  printf("Max Number of Edges : %d\n\n",maxNoOfEdges); //Show max edges to the user.
  for(int i=0;i<noOfVert;i++){
    printf("%d: ",i); //Print the current vertex id.
    for(int j=0;j<noOfVert;j++){
      if(adj_matrix[i][j]==1)
        printf("%d, ",j); //Print the IDs of the neighbours of the current vertex.
    }
    puts("");
  }
  printf("\nEnter the Number of Initial Infecteds You wish to Enter OR Enter -1 To randomly generate non-zero Initial Infecteds : ");
  int choice; //Var to store the number of initial infecteds user wishes to have.
  scanf(" %d",&choice);
  if(choice==-1){ //Generate the initial infecteds randomly
    int r = rand()%(noOfVert/3);
    if(r==0)
      r++; //Should be at least 1
    if(r>15)
      r=15; //Should be at max 15/
    already_infected = (int*)malloc(r*sizeof(int)); //Allocate memory for the array.
    for(int i=0;i<r;i++){
      int rndm = rand()%noOfVert; //Generate the ids of vertices randomly.
      already_infected[i]=rndm;
      for(int j=0;j<i;j++){
        if(rndm==already_infected[j]){ //Check if the current vertex has been selected previously.
          i--;
          break;
        }
      }
    }
    printf("\nRandomly Generated Initial Infecteds: "); //Print the randomly selected initial infecteds.
    for(int i=0;i<r;i++){
      printf("%2d ",already_infected[i]);
    }
    noOfInitInf=r;
    puts("");
  }else{
    printf("\nEnter the IDs of %d Initial Infected Vertices :",choice); //Ask user for number of initial infecteds.
    for(int i=0;i<choice;i++){
      int input;
      scanf(" %d",&input);
      if(input<0 ||input>=noOfVert){ //Check if the input is in the range or not
        printf("Invalid Input, Please Enter a Number Between %d and %d",0,noOfVert-1); //Print error message
        i--;
      }
      already_infected[i]=input;
      for(int j=0;j<i;j++){
        if(already_infected[i]==already_infected[j]){ //Check if the input is in the range or not
          i--;
          break;
        }
      }
    }
    noOfInitInf=choice;
  }

  Fast_SIR(graph,tao,gamma,already_infected,noOfVert,adj_matrix,max_time,noOfInitInf); //Begin the Simulation

  return 0;
}

void print_list(lNode* head) //Print the linked list containing IDs of the vertices having that status
{

  lNode* temp=head;
  while(temp!=NULL){
    printf("%2d ",temp->vert->id);
    temp=temp->next;
  }

}

void printAll() //Print the S,I and R lists
{
  puts("");
  printf("%13s[%d] : ","Susceptible",s_size);
  print_list(s_list);
  puts("");
  printf("%13s[%d] : ","Infected",i_size);
  print_list(i_list);
  puts("");
  printf("%13s[%d] : ","Recovered",r_size);
  print_list(r_list);
  puts("\n\n");
}

void PrintEventQ(event** pq,int size,int day) //Print the events due on the day fiven to the function
{
  puts("");
  printf("Event Priority Queue\n");
  if(size==0){ //Check if Priority Q is empty
    printf("Empty\n");
    return;
  }

  printf("ID Status   Time   Action\n");
  for(int i=0;i<size;i++){ //Loop for number of times as its size.
    if(pq[i]->time==day)
      printf("%2d %c      %5d      %c\n",pq[i]->node->id,pq[i]->node->status,pq[i]->time,pq[i]->action);
  }
  puts("");
}

void Insert(lNode** head,vertex* v) //Insert node ID in the linked list.
{
  lNode* newNode = (lNode*)malloc(sizeof(lNode)); //Allocate memory for new node.
  newNode->vert=v;
  newNode->next=NULL;


  if(*head==NULL){ //Check if head is null
    *head=newNode;
  }else{
    lNode* temp=*head;
    while(temp->next!=NULL){ //Loop until end of linked list.
      temp=temp->next;
    }
    temp->next=newNode;
  }

}

void Shift(lNode** from_head,lNode** to_head,vertex* v) //Shift a vertex from one linked list to another.
{
  lNode* prev = NULL;
  lNode* curr = *from_head;
  while(curr!=NULL&&curr->vert->id!=v->id){ //Loop until end is reached or the node is found.
    prev=curr;
    curr=curr->next;
  }
  if(curr==NULL) //Exit if the end is reached.
    return;

  lNode* cont=NULL;
  vertex* temp=NULL;
  if(prev==NULL){ //If the node to be shifted is at the beginning of the list.
    cont=*from_head;
    temp=cont->vert;
    *from_head=(*from_head)->next;
    free(cont);
  }else{ //If the node is somewhere else but at the beginning
    prev->next=curr->next;
    temp = curr->vert;
    free(curr);
  }

  //free(curr);
  Insert(to_head,temp); //Insert the node into the other list.
}

void Swap(event** a,event** b){ //Swap the pointers of the event struct in the Priority Queue
  event* temp=*a;
  *a=*b;
  *b=temp;
}

void min_heapify(event **q,int i,int *size)
{
  int left = (2*i)+1; //Left child of ith element
  int right = (2*i)+2; //rightt child of ith element
  int smallest;
  if(left<*size &&q[left]->time<q[i]->time)
    smallest=left;
  else
    smallest = i;

  if(right<*size && q[right]->time<q[smallest]->time)
    smallest=right;

  if(smallest!=i){
    Swap(&q[i],&q[smallest]); //Swap the current event and the event with smallest priority
    min_heapify(q,smallest,size); //run the function again recursively, until the current event is one with the smallest priority.
  }
}

event extract(event **q,int *size)
{
  event ret = *q[0]; //store the event at the 0th index of Priority Queue in ret variable
  q[0]=q[*size-1]; //Set last element of the array as the first element.
  (*size)--; //Decrement the size of the array.
  min_heapify(q,0,size);
  return ret;
}

void InsertQ(event **q,event *e,int *size) //Insert Event into the Priority Queue
{
  q[*size]=e; //Add event pointer to the end of the Priority Queue.
  int i=*size;
  (*size)++; //Increment the size of the Priority Queue
  while(i!=0&& q[(i-1)/2]->time>q[i]->time){
    Swap(&q[i],&q[(i-1)/2]);
    i=(i-1)/2;
  }
}

int getDays(float prob) //Generate Random Number of days with probability prob.
{
  int days=1;
  int fav = (int)10000*prob;
  int rndm;
  do{
    rndm=rand()%10000;
    if(rndm>=fav){
      days++;
    }
  }while(rndm>=fav);

  return days;

}

int minimum(int a,int b,int c) //Returns the smallest item from 3 items.
{
  int min=a;
  if(b<min)
    min=b;
  if(c<min)
    min=c;

  return min;
}

void process_trans_SIR(vertex *G,vertex* target,float tao,float gamma,int time,int g_size,int (*adj_matrix)[g_size])
{
  Shift(&s_list,&i_list,target); //Shift the node target from susceptible to infected.
  s_size--;
  i_size++;
  target->status='i'; //Change the status of that node to infected.
  target->rec_time=time+getDays(gamma); //Set the recovery time.
  if(target->rec_time<max_time){ //Check if it will recover before maximum time.
    event* e=(event*)malloc(sizeof(event));
    e->node=target;
    e->time=target->rec_time;
    e->action='r';
    InsertQ(priorityQ,e,&q_size); //Add its recovery event in the PRiority Queue
  }


  for(int i=0;i<g_size;i++){
    if(adj_matrix[target->id][i]==1){ //Find all the neighbour vertices of the current infected vertex.
      vertex* v = &G[i];
      if(v->status=='s'){ //Check if that neighbour vertex is susceptible
        int inf_time = time+getDays(tao); //Set its infection time.
        if(inf_time<minimum(target->rec_time,v->pred_inf_time,max_time)){
          event* newEvent=(event*)malloc(sizeof(event));
          newEvent->node=v;
          newEvent->time=inf_time;
          newEvent->action='t';
          InsertQ(priorityQ,newEvent,&q_size); //Insert its infection event in the Priority Queue
          v->pred_inf_time=inf_time;
        }
      }
    }
  }

}

void process_rec_SIR(vertex* v,int time)
{
  v->status='r'; //Set Status as recovered.
  v->pred_inf_time=32767; //Set predicted infection time as infinty
  v->rec_time=32767; //Set recovery time as infinty
  Shift(&i_list,&r_list,v); //Shift it from infected list to recovered list.
  i_size--;
  r_size++;
}

void Fast_SIR(vertex *G,float tao,float gamma,int* initial_infecteds,int g_size,int (*adj_matrix)[g_size],int t_max,int inf_size)
{

  for(int i=0;i<g_size;i++){ //Set the values of the members of the vertices in the graph.
    G[i].id=i;
    G[i].status='s';
    G[i].pred_inf_time=32767;
    G[i].rec_time=32767;
    Insert(&s_list,&G[i]); //Insert All Vertices in the susceptible list initially.
    s_size++;
  }
  puts("");


  for(int i=0;i<inf_size;i++){
    int index = initial_infecteds[i];
    event* e=(event*)malloc(sizeof(event));
    e->node=&G[index];
    e->time=0;
    e->action='t';
    G[index].pred_inf_time=0;
    InsertQ(priorityQ,e,&q_size); //Insert the infection event for the initial infected in the Priority Queue.
  }

  while(currDay<=max_time && q_size!=0){ //Loop until the max time is reached or the Priority Queue becomes empty.
    while(currDay==priorityQ[0]->time){ //Loop until events that are due today are remaining.
      if(q_size==0)
        break;
      event earliest = extract(priorityQ,&q_size); //Ectract the earliest event.
      if(earliest.action=='t'){ //Check if the action the event is transmission
        if((earliest.node)->status=='s'){ //Check if the node on which event is applied is susceptible
          process_trans_SIR(G,earliest.node,tao,gamma,earliest.time,g_size,adj_matrix); //Run the Transmission function
        }
      }else{
        process_rec_SIR(earliest.node,earliest.time); //Run the recovery Function
      }
    }
    printf("State At End of Day %d:\n",currDay);
    printAll(); //Print the Nodes at the end of the day.
    currDay++; //Go to the next day.

  }
}