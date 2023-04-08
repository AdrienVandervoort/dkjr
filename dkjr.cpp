// 	afficherCage(2);
// 	afficherCage(3);
// 	afficherCage(4);

// 	afficherRireDK();

// 	afficherCle(3);

// 	afficherCroco(11, 2);
// 	afficherCroco(17, 1);
// 	afficherCroco(0, 3);
// 	afficherCroco(12, 5);
// 	afficherCroco(18, 4);

// 	afficherDKJr(11, 9, 1);
// 	afficherDKJr(6, 19, 7);
// 	afficherDKJr(0, 0, 9);
	
// 	afficherCorbeau(10, 2);
// 	afficherCorbeau(16, 1);
	
// //	effacerCarres(9, 10, 2, 1);

// 	//afficherEchec(1);
// 	//afficherScore(1999);

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <SDL/SDL.h>
#include "./presentation/presentation.h"

#define VIDE        		0
#define DKJR       		1
#define CROCO       		2
#define CORBEAU     		3
#define CLE 			4

#define AUCUN_EVENEMENT    	0

#define LIBRE_BAS		1
#define LIANE_BAS		2
#define DOUBLE_LIANE_BAS	3
#define LIBRE_HAUT		4
#define LIANE_HAUT		5

void* FctThreadEvenements(void *);
void* FctThreadCle(void *);
void* FctThreadDK(void *);
void* FctThreadDKJr(void *);
void* FctThreadScore(void *);
void* FctThreadEnnemis(void *);
void* FctThreadCorbeau(void *);
void* FctThreadCroco(void *);
void  SpawnShield();

void initGrilleJeu();
void setGrilleJeu(int l, int c, int type = VIDE, pthread_t tid = 0);
void afficherGrilleJeu();

void HandlerSIGUSR1(int);
void HandlerSIGUSR2(int);
void HandlerSIGALRM(int);
void HandlerSIGINT(int);
void HandlerSIGQUIT(int);
void HandlerSIGCHLD(int);
void HandlerSIGHUP(int);

void DestructeurVS(void *p);

pthread_t threadCle;
pthread_t threadDK;
pthread_t threadDKJr;
pthread_t threadEvenements;
pthread_t threadScore;
pthread_t threadEnnemis;

pthread_cond_t condDK;
pthread_cond_t condScore;

pthread_mutex_t mutexGrilleJeu;
pthread_mutex_t mutexDK;
pthread_mutex_t mutexEvenement;
pthread_mutex_t mutexScore;

pthread_key_t keySpec;

bool MAJDK = false;
int  score = 0;
bool MAJScore = true;
int  delaiEnnemis = 4000;
int  positionDKJr = 1;
int  evenement = AUCUN_EVENEMENT;
int etatDKJr;
int nbrVie=0;

typedef struct
{
  int type;
  pthread_t tid;
} S_CASE;

S_CASE grilleJeu[4][8];

typedef struct
{
  bool haut;
  int position;
} S_CROCO;

// ------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	int evt;

	ouvrirFenetreGraphique();
	initGrilleJeu();

	pthread_mutex_init(&mutexGrilleJeu,NULL);






	pthread_key_create(&keySpec,NULL);




	pthread_create(&threadCle,NULL,FctThreadCle,NULL);//cree
	pthread_create(&threadEvenements,NULL,FctThreadEvenements,NULL);//cree
	pthread_create(&threadDK,NULL,FctThreadDK,NULL);
	pthread_create(&threadScore,NULL,FctThreadScore,NULL);
	pthread_create(&threadEnnemis,NULL,FctThreadEnnemis,NULL);
	do
	{
			pthread_create(&threadDKJr,NULL,FctThreadDKJr,NULL);//cree
			pthread_join(threadDKJr,NULL);//att fin du threadDKJr
			SpawnShield();
			afficherEchec(nbrVie);

			nbrVie++;


	}while(nbrVie <3);

	pthread_join(threadEvenements,NULL);//att la fin du thread
	pthread_key_delete(keySpec);//supprime la cle utilisé par sigusr1 pour corbeau

	



}

// -------------------------------------

void initGrilleJeu()
{
  int i, j;   
  
  pthread_mutex_lock(&mutexGrilleJeu);

  for(i = 0; i < 4; i++)
    for(j = 0; j < 7; j++)
      setGrilleJeu(i, j);

  pthread_mutex_unlock(&mutexGrilleJeu);
}

// -------------------------------------

void setGrilleJeu(int l, int c, int type, pthread_t tid)
{
  grilleJeu[l][c].type = type;
  grilleJeu[l][c].tid = tid;
}

// -------------------------------------

void afficherGrilleJeu()
{   
   for(int j = 0; j < 4; j++)
   {
       for(int k = 0; k < 8; k++)
          printf("%d  ", grilleJeu[j][k].type);
       printf("\n");
   }

   printf("\n");
}

void *FctThreadCle(void *param)
{
	int i=1;
	struct timespec temps = { 0, 700000000 };
	while(1)
	{

		for(i;i<=4;i++)
		{
			//printf("i1ereb %d\n",i);
		
			pthread_mutex_lock(&mutexGrilleJeu);
			afficherCle(i);
			if(i==1)
			{
				grilleJeu[0][1].type=CLE;
			}

			else
			{
			  grilleJeu[0][1].type=VIDE;
			}

			pthread_mutex_unlock(&mutexGrilleJeu);
			nanosleep(&temps, NULL);
			effacerCarres(3,12,2,3);
		}
		


		for(i=3;i>1;i--)
		{
			//printf("i2ereb %d\n",i);
			pthread_mutex_lock(&mutexGrilleJeu);
			afficherCle(i);
			if(i==1)
			{
				grilleJeu[0][1].type=CLE;
			}

			else
			{
				grilleJeu[0][1].type=VIDE;
			}

			pthread_mutex_unlock(&mutexGrilleJeu);
			nanosleep(&temps, NULL);
			effacerCarres(3,12,3,3);

		}
		
	}
}

void *FctThreadEvenements(void*)
{
	int evt;
	struct timespec time ={0,100'000'000};
	while (1)
	{
	    evt = lireEvenement();

	    switch (evt)
	    {
				case SDL_QUIT:
					exit(0);
				case SDLK_UP:
					printf("KEY_UP\n");
					break;
				case SDLK_DOWN:
					printf("KEY_DOWN\n");
					break;
				case SDLK_LEFT:
					printf("KEY_LEFT\n");
					break;
				case SDLK_RIGHT:
					printf("KEY_RIGHT\n");
	    }
	    pthread_mutex_lock(&mutexEvenement);
	    evenement=evt;
	    pthread_mutex_unlock(&mutexEvenement);

	    pthread_kill(threadDKJr,SIGQUIT);
	    
	    nanosleep(&time,NULL);
	    pthread_mutex_lock(&mutexEvenement);
	    evenement=AUCUN_EVENEMENT;
	    pthread_mutex_unlock(&mutexEvenement);


	}
}


void* FctThreadDK(void *)
{
	int NbPartieCage;
	struct timespec sourire={0,700000000};

	while(1)
	{
		NbPartieCage=4;
		afficherCage(1);
		afficherCage(2);
  	afficherCage(3);
  	afficherCage(4);
		while(NbPartieCage>=1)
		{
			pthread_mutex_lock(&mutexDK);

			pthread_cond_wait(&condDK,&mutexDK);
			if(MAJDK)
			{
				MAJDK=false;
				switch(NbPartieCage)
				{
					case 1:
							effacerCarres(4,9,2,2);
					break;

					case 2:
							effacerCarres(4,7,2,2);
							afficherCage(4);
							
					break;
					case 3:
							effacerCarres(2,9,2,2);
							afficherCage(4);
							
					break;
							
					case 4:
							effacerCarres(2,7,2,2);
							afficherCage(4);
							
					break;

				}
				NbPartieCage--;
			}
			pthread_mutex_unlock(&mutexDK);
		}
		afficherRireDK();
		nanosleep(&sourire,NULL);
		effacerCarres(3,8,2,2);
	}
	
}
void* FctThreadDKJr(void*){


	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask,SIGQUIT);
	sigaddset(&mask,SIGINT);
	sigaddset(&mask,SIGHUP);
	sigaddset(&mask,SIGCHLD);
	sigprocmask(SIG_UNBLOCK,&mask,NULL);

	struct sigaction signal;

	sigemptyset(&signal.sa_mask);
	signal.sa_flags = 0;
	signal.sa_handler = HandlerSIGQUIT;
	sigaction(SIGQUIT,&signal,NULL);

	sigemptyset(&signal.sa_mask);
	signal.sa_flags = 0;
	signal.sa_handler = HandlerSIGINT;
	sigaction(SIGINT,&signal,NULL);

	sigemptyset(&signal.sa_mask);
	signal.sa_flags = 0;
	signal.sa_handler = HandlerSIGHUP;
	sigaction(SIGHUP,&signal,NULL);

	sigemptyset(&signal.sa_mask);
	signal.sa_flags = 0;
	signal.sa_handler = HandlerSIGCHLD;
	sigaction(SIGCHLD,&signal,NULL);

	bool on = true;
	pthread_mutex_lock(&mutexGrilleJeu);
  setGrilleJeu(3,1,DKJR,pthread_self());
  afficherDKJr(11, 9, 1);
  etatDKJr = LIBRE_BAS;
  positionDKJr = 1;
  pthread_mutex_unlock(&mutexGrilleJeu);


	while(on){
		pause();
		pthread_mutex_lock(&mutexEvenement);
		pthread_mutex_lock(&mutexGrilleJeu);


		if(etatDKJr == LIBRE_BAS){
			switch(evenement){
				case SDLK_LEFT:
				if(positionDKJr>1)
				{
					setGrilleJeu(3,positionDKJr);
					effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);

					positionDKJr--;
					setGrilleJeu(3,positionDKJr,DKJR,pthread_self());
					afficherDKJr(11,(positionDKJr * 2) + 7,((positionDKJr - 1) % 4) + 1);
				}
				break;

				case SDLK_RIGHT:
				if(positionDKJr<7)
				{
					setGrilleJeu(3,positionDKJr);
					effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);

					positionDKJr++;
					setGrilleJeu(3,positionDKJr,DKJR,pthread_self());
					afficherDKJr(11,(positionDKJr * 2) + 7,((positionDKJr - 1) % 4) + 1);
				}
					
				break;

				case SDLK_UP:
					struct timespec temps = { 1, 400000000 };
				setGrilleJeu(3,positionDKJr);
				effacerCarres(11, (positionDKJr * 2) + 7, 2, 2);

				//babydk hit bird
				if(grilleJeu[2][positionDKJr].type ==CORBEAU)
				{
					pthread_kill(grilleJeu[2][positionDKJr].tid,SIGUSR1);
					pthread_mutex_unlock(&mutexGrilleJeu);
					pthread_mutex_unlock(&mutexEvenement);

					pthread_exit(NULL);
				}


				if(positionDKJr ==1 || positionDKJr ==5)
				{
					setGrilleJeu(2,positionDKJr,DKJR,pthread_self());
					afficherDKJr(10,(positionDKJr*2)+7,7);
					etatDKJr=LIANE_BAS;
				}
			else if(positionDKJr==7)
				{
					setGrilleJeu(2,positionDKJr,DKJR,pthread_self());
					afficherDKJr(10,(positionDKJr*2)+7,5);
					etatDKJr=DOUBLE_LIANE_BAS;
				}
				else
				{
						setGrilleJeu(2,positionDKJr,DKJR,pthread_self());
						afficherDKJr(10,(positionDKJr*2)+7,8);

						pthread_mutex_unlock(&mutexGrilleJeu);
						nanosleep(&temps,NULL);
						pthread_mutex_lock(&mutexGrilleJeu);

					 setGrilleJeu(2,positionDKJr);
					 effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);
					
					 if(grilleJeu[3][positionDKJr].type==CROCO)//tombe dessus en bas
							 {
							 	pthread_kill(grilleJeu[3][positionDKJr].tid,SIGUSR2);
								pthread_mutex_unlock(&mutexGrilleJeu);
								pthread_mutex_unlock(&mutexEvenement);
							 	pthread_exit(NULL);
							 }

					 setGrilleJeu(3,positionDKJr,DKJR,pthread_self());
					 afficherDKJr(11,(positionDKJr * 2) + 7,((positionDKJr - 1) % 4) + 1);

				}
				
				

				break;
				
				
			}
		} 
		else if(etatDKJr == LIANE_BAS){
			if(evenement == SDLK_DOWN)
			{
				setGrilleJeu(2,positionDKJr);
				effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);
				if (grilleJeu[3][positionDKJr].type==CROCO)
				{
					pthread_kill(grilleJeu[3][positionDKJr].tid,SIGUSR2);//liane en bas et tombe sur un croco
					pthread_mutex_unlock(&mutexGrilleJeu);
					pthread_mutex_unlock(&mutexEvenement);

					pthread_exit(NULL);
				}

				setGrilleJeu(3,positionDKJr,DKJR,pthread_self());
				afficherDKJr(11,(positionDKJr * 2) + 7,((positionDKJr - 1) % 4) + 1);
				etatDKJr=LIBRE_BAS;
			}

		}
		else if(etatDKJr == DOUBLE_LIANE_BAS){
			if(evenement == SDLK_DOWN)
			{
				setGrilleJeu(2,positionDKJr);
				effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);

				if (grilleJeu[3][positionDKJr].type==CROCO)
				{
					pthread_kill(grilleJeu[3][positionDKJr].tid,SIGUSR2);//liane en bas et tombe sur un croco
					pthread_mutex_unlock(&mutexGrilleJeu);
					pthread_mutex_unlock(&mutexEvenement);

					pthread_exit(NULL);
				}

				setGrilleJeu(3,positionDKJr,DKJR,pthread_self());
				afficherDKJr(11,(positionDKJr * 2) + 7,((positionDKJr - 1) % 4) + 1);	
				etatDKJr=LIBRE_BAS;
								
			}
			else if(evenement == SDLK_UP)
			{
				setGrilleJeu(2,positionDKJr);
				effacerCarres(10, (positionDKJr * 2) + 7, 2, 2);

				if (grilleJeu[1][positionDKJr].type==CROCO)
				{
					pthread_kill(grilleJeu[1][positionDKJr].tid,SIGUSR2);//liane en bas et tombe sur un croco
					pthread_mutex_unlock(&mutexGrilleJeu);
					pthread_mutex_unlock(&mutexEvenement);

					pthread_exit(NULL);
				}

				setGrilleJeu(1,positionDKJr,DKJR,pthread_self());
				afficherDKJr(7,21,6);//position=7
				etatDKJr=LIBRE_HAUT;
			
					
			}
		}
		else if(etatDKJr == LIBRE_HAUT){
			switch(evenement){
				case SDLK_LEFT:
				if (positionDKJr>=3)
				{
					struct timespec time=timespec{0,500000000};
					setGrilleJeu(1,positionDKJr);
					effacerCarres(7,(positionDKJr*2)+7,2,2);

					positionDKJr--;
					if(positionDKJr==2 &&grilleJeu[0][1].type==CLE)
						 {
						 	afficherDKJr(6,11,9);

						 	nanosleep(&time,NULL);
						 	effacerCarres(5,12,3,2);
						 	SpawnShield();
						 	afficherDKJr(6,11,10);
						 	

						 	nanosleep(&time,NULL);

						 	effacerCarres(3,11,3,2);
						 	afficherDKJr(6,10,11);

						 //signal du signal a donkey Kong//

						 pthread_mutex_lock(&mutexDK);
						 MAJDK=true;
						 pthread_mutex_unlock(&mutexDK);
						 pthread_cond_signal(&condDK);
						 //fin du signal a donkey kong//


						 	//signal score
						 pthread_mutex_lock(&mutexScore);
						 score=score+10;
						 MAJScore=true;
						 pthread_mutex_unlock(&mutexScore);
						 pthread_cond_signal(&condScore);

						 //fin signal score

						 	nanosleep(&time,NULL);

						 	effacerCarres(6,10,2,3);

						 	setGrilleJeu(3,1,DKJR,pthread_self());
						 	afficherDKJr(11,9,1);
						 	etatDKJr=LIBRE_BAS;
						 	positionDKJr=1;
						 }
						 else if(positionDKJr==2 &&grilleJeu[0][1].type==VIDE)
						 {
						 	afficherDKJr(5,11,9);

						 	nanosleep(&time,NULL);

						 	effacerCarres(5,11,3,3);
						 	afficherCage(4);
						 	afficherDKJr(6,(positionDKJr*2)+7,12);//ligne,colonne,img

						 	nanosleep(&time,NULL);
						 	effacerCarres(6,(positionDKJr*2)+7,3,2);//ligne courrante,colonne courrante,dim

						 	afficherDKJr(11,0+7,13);

						 	nanosleep(&time,NULL);
						 	effacerCarres(11,7,2,2);

							pthread_mutex_unlock(&mutexGrilleJeu);
							pthread_mutex_unlock(&mutexEvenement);

						 
						 	pthread_exit(0);

						 }
						 else
						 {
						 	setGrilleJeu(1,positionDKJr,DKJR,pthread_self());
						 	afficherDKJr(7,(positionDKJr*2)+7,((positionDKJr-1)%4)+1);
						 }
				}
					
				break;

				case SDLK_RIGHT:
					if (positionDKJr <7)
					{
						setGrilleJeu(1,positionDKJr);
						effacerCarres(7,(positionDKJr*2)+7,2,2);
						positionDKJr++;

						setGrilleJeu(1,positionDKJr,DKJR,pthread_self());
						if(positionDKJr==7)
							afficherDKJr(10,(positionDKJr*2)+7,6);
						else
							afficherDKJr(7,(positionDKJr*2)+7,((positionDKJr-1)%4)+1);
					}
					
				break;

				case SDLK_DOWN :
				if(positionDKJr==7)
				{
					setGrilleJeu(1,positionDKJr);
					effacerCarres(7,(positionDKJr*2)+7,2,2);

					setGrilleJeu(2,positionDKJr,DKJR,pthread_self());
					afficherDKJr(10,(positionDKJr*2)+7,5);

					etatDKJr=DOUBLE_LIANE_BAS;
				}

				break;

				case SDLK_UP:

				struct timespec temps = { 1, 400000000 };
				setGrilleJeu(1,positionDKJr);
				if(positionDKJr ==6)
				{
					effacerCarres(7, (positionDKJr * 2) + 7, 2, 2);
					setGrilleJeu(0,positionDKJr,DKJR,pthread_self());
					afficherDKJr(6,(positionDKJr*2)+7,7);
					etatDKJr=LIANE_HAUT;
				}
				else if(positionDKJr!=2 &&positionDKJr!=7)
				{
					effacerCarres(7,(positionDKJr*2)+7,2,2);
					setGrilleJeu(0,positionDKJr,DKJR,pthread_self());
					afficherDKJr(6,(positionDKJr*2)+7,8);

					pthread_mutex_unlock(&mutexGrilleJeu);
					nanosleep(&temps,NULL);
					pthread_mutex_lock(&mutexGrilleJeu);

					setGrilleJeu(0,positionDKJr);
					effacerCarres(6,(positionDKJr*2)+7,2,2);

					if (grilleJeu[1][positionDKJr].type==CROCO)//tombe dessus en haut
						{
						 	pthread_kill(grilleJeu[1][positionDKJr].tid,SIGUSR2);
							pthread_mutex_unlock(&mutexGrilleJeu);
							pthread_mutex_unlock(&mutexEvenement);
						 	pthread_exit(NULL);
						}

					setGrilleJeu(1,positionDKJr,DKJR,pthread_self());
					afficherDKJr(7,(positionDKJr*2)+7,((positionDKJr-1)%4)+1);
				}

				
					
				break;
			}

		}
		else if(etatDKJr == LIANE_HAUT){
			if(evenement == SDLK_DOWN)
			{
				setGrilleJeu(0,positionDKJr);
				effacerCarres(6,(positionDKJr*2)+7,2,2);
				 if (grilleJeu[1][positionDKJr].type==CROCO)//sur la liane si on tombe dessus
				{
					pthread_kill(grilleJeu[1][positionDKJr].tid,SIGUSR2);
					pthread_mutex_unlock(&mutexGrilleJeu);
					pthread_mutex_unlock(&mutexEvenement);
					pthread_exit(NULL);
				 }

				setGrilleJeu(1,positionDKJr,DKJR,pthread_self());
				afficherDKJr(7,(positionDKJr*2)+7,((positionDKJr-1)%4)+1);

				etatDKJr=LIBRE_HAUT;
			
			}
		}		

		//printGrille();

		pthread_mutex_unlock(&mutexEvenement);
		pthread_mutex_unlock(&mutexGrilleJeu);		
	}

	pthread_exit(0);
}


void* FctThreadScore(void *)
{
	while(1)
	{
		pthread_mutex_lock(&mutexScore);
		pthread_cond_wait(&condScore,&mutexScore);
		if(MAJScore)
		{
			MAJScore=false;
			afficherScore(score);
		}
		pthread_mutex_unlock(&mutexScore);
	}
	pthread_exit(NULL);
}



void* FctThreadEnnemis(void *)
{
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask,SIGALRM);
	sigprocmask(SIG_UNBLOCK,&mask,NULL);

	struct sigaction signal;
	sigemptyset(&signal.sa_mask);
	signal.sa_flags=0;
	signal.sa_handler=HandlerSIGALRM;
	sigaction(SIGALRM,&signal,NULL);

	struct timespec wait={4,0};
	alarm(15);



	srand(time(0));
	while(1)
	{
		int nombreAleatoire = (rand() % 2) + 1;

		if(nombreAleatoire==1)
		{
			pthread_t ThreadCorbeau;
			pthread_create(&ThreadCorbeau,NULL,FctThreadCorbeau,NULL);
		
		}
		else
		{
			pthread_t ThreadCroco;
			pthread_create(&ThreadCroco,NULL,FctThreadCroco,NULL);
		}

		wait.tv_sec = delaiEnnemis/1000;
		printf("\nwait.tv_sec: %d",wait.tv_sec);
		wait.tv_nsec = (delaiEnnemis%1000)*1'000'000;
		printf("\nwait.tv_nsec: %d",wait.tv_nsec);


		nanosleep(&wait,NULL);

	}
	pthread_exit(NULL);
}
void* FctThreadCorbeau(void *)
{
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask,SIGUSR1);
	sigprocmask(SIG_UNBLOCK,&mask,NULL);

	struct sigaction signal;
	sigemptyset(&signal.sa_mask);
	signal.sa_flags=0;
	signal.sa_handler= HandlerSIGUSR1;
	sigaction(SIGUSR1,&signal,NULL);





	printf("\nCORBEAU");
	struct timespec corb={0,700000000};
	int *PositionCourante=NULL;

	PositionCourante=(int*)malloc(sizeof(int));
	*PositionCourante=0;//ou a 8
	if(PositionCourante==NULL)
		printf("\nerreur du malloc\n");

	pthread_setspecific(keySpec,PositionCourante);// Un thread accède à sa donnée spécifique associée à la clé passée comme paramètre en utilisant la primitive

	while(*PositionCourante<=7)//ou a 22 mais 2++
	{
		pthread_mutex_lock(&mutexGrilleJeu);
		afficherCorbeau(*PositionCourante*2+8,(*PositionCourante)%2+1);//affiche de la colonne 8 a 22 et imagine diff pour chaque etat
		setGrilleJeu(2,*PositionCourante,CORBEAU,pthread_self());
		pthread_mutex_unlock(&mutexGrilleJeu);

		nanosleep(&corb,NULL);

		pthread_mutex_lock(&mutexGrilleJeu);
		effacerCarres(9,*PositionCourante*2+8,2,1);
		setGrilleJeu(2,*PositionCourante);
		pthread_mutex_unlock(&mutexGrilleJeu);


		pthread_mutex_lock(&mutexGrilleJeu);
		if(grilleJeu[2][(*PositionCourante)+1].type==DKJR)
			{
			
				pthread_kill(grilleJeu[2][(*PositionCourante)+1].tid,SIGINT);
				effacerCarres(9,(*PositionCourante*2+8),2,1);
				setGrilleJeu(2,*PositionCourante);

				pthread_mutex_unlock(&mutexGrilleJeu);
				
				free(PositionCourante);
				pthread_exit(NULL);
				
			}

			printf("suite");

			pthread_mutex_unlock(&mutexGrilleJeu);
	
			printf("\nPositionCourante=%d\n",*PositionCourante);
			(*PositionCourante)++;
			
	}
	free(PositionCourante);
	pthread_exit(NULL);
	
}
void* FctThreadCroco(void *)
{
	printf("\nCROCO");

	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask,SIGUSR2);
	sigprocmask(SIG_UNBLOCK,&mask,NULL);

	struct sigaction signal;
	sigemptyset(&signal.sa_mask);
	signal.sa_flags=0;
	signal.sa_handler= HandlerSIGUSR2;
	sigaction(SIGUSR2,&signal,NULL);



	struct timespec croc={0,700000000};

	S_CROCO *PositionCourante=(S_CROCO*)malloc(sizeof(S_CROCO));
	PositionCourante->haut=true;
	PositionCourante->position=2;

	pthread_setspecific(keySpec,PositionCourante);

	while(PositionCourante->haut)
	{
		if(PositionCourante->position<=7)
		{
			pthread_mutex_lock(&mutexGrilleJeu);
			afficherCroco(PositionCourante->position*2+7,PositionCourante->position%2+1);
			setGrilleJeu(1,PositionCourante->position,CROCO,pthread_self());
			pthread_mutex_unlock(&mutexGrilleJeu);
		}

		nanosleep(&croc,NULL);

		pthread_mutex_lock(&mutexGrilleJeu);
		effacerCarres(8,PositionCourante->position*2+7,1,1);
		setGrilleJeu(1,PositionCourante->position);
		pthread_mutex_unlock(&mutexGrilleJeu);



		pthread_mutex_lock(&mutexGrilleJeu);

		if(grilleJeu[1][(PositionCourante->position)+1].type==DKJR)
		{
			pthread_kill(grilleJeu[1][(PositionCourante->position)+1].tid,SIGHUP);

			effacerCarres(8,PositionCourante->position*2+7,1,1);
			setGrilleJeu(1,PositionCourante->position);
			pthread_mutex_unlock(&mutexGrilleJeu);
			free(PositionCourante);
			pthread_exit(NULL);

		}
		pthread_mutex_unlock(&mutexGrilleJeu);





		(PositionCourante->position)++;
		//fin croco en haut

		if(PositionCourante->position==8)
		{
			pthread_mutex_lock(&mutexGrilleJeu);
			afficherCroco(23,3);//case 22 image 3
			pthread_mutex_unlock(&mutexGrilleJeu);

			nanosleep(&croc,NULL);

			pthread_mutex_lock(&mutexGrilleJeu);
			effacerCarres(9,23,1,1);
			pthread_mutex_unlock(&mutexGrilleJeu);

			PositionCourante->position=7;
			PositionCourante->haut=false;//on va en bas
		}

	}
		  pthread_mutex_lock(&mutexGrilleJeu);
			if(grilleJeu[3][7].type==DKJR)
			{
				pthread_kill(grilleJeu[3][7].tid,SIGCHLD);

				effacerCarres(12,PositionCourante->position*2+8,1,1);
				setGrilleJeu(3,PositionCourante->position);
				pthread_mutex_unlock(&mutexGrilleJeu);
				free(PositionCourante);
				pthread_exit(NULL);

			}		
		  pthread_mutex_unlock(&mutexGrilleJeu);	



	while(PositionCourante->haut==false)
	{
		if(PositionCourante->position>0)
		{
			pthread_mutex_lock(&mutexGrilleJeu);
			afficherCroco(PositionCourante->position*2+8,PositionCourante->position%2+1+3);//+3car croco dans l'autre sens donc imagine diff
			setGrilleJeu(3,PositionCourante->position,CROCO,pthread_self());
			pthread_mutex_unlock(&mutexGrilleJeu);

		nanosleep(&croc,NULL);

		pthread_mutex_lock(&mutexGrilleJeu);
		effacerCarres(12,PositionCourante->position*2+8,1,1);
		setGrilleJeu(3,PositionCourante->position);
		pthread_mutex_unlock(&mutexGrilleJeu);	

		pthread_mutex_lock(&mutexGrilleJeu);
			if(grilleJeu[3][(PositionCourante->position)-1].type==DKJR)
			{
				pthread_kill(grilleJeu[3][(PositionCourante->position)-1].tid,SIGCHLD);

				effacerCarres(12,PositionCourante->position*2+8,1,1);
				setGrilleJeu(3,PositionCourante->position);
				pthread_mutex_unlock(&mutexGrilleJeu);
				free(PositionCourante);
				pthread_exit(NULL);

			}		
		pthread_mutex_unlock(&mutexGrilleJeu);	

		(PositionCourante->position)--;
		}
		else
		{
				free(PositionCourante);
				pthread_exit(NULL);
		}
	}
	pthread_exit(NULL);

}



void HandlerSIGQUIT(int)
{

}



void HandlerSIGALRM(int)
{
	delaiEnnemis=delaiEnnemis-250;
	if(delaiEnnemis>2500)
	{
		alarm(15);
	}
	printf("2delaiennemis: %d\n",delaiEnnemis);
}
void HandlerSIGUSR1(int)
{

	int*positon=(int*)pthread_getspecific(keySpec);

	pthread_mutex_lock(&mutexGrilleJeu);
		effacerCarres(9,*positon*2+8,2,1);//supp bird;
		setGrilleJeu(2,*positon);
	pthread_mutex_unlock(&mutexGrilleJeu);

	
	pthread_exit(NULL);

}
void HandlerSIGINT(int)
{
	pthread_mutex_lock(&mutexGrilleJeu);
	if(etatDKJr==LIBRE_BAS)
	{
		printf("\nlb");
		setGrilleJeu(2,positionDKJr);
		effacerCarres(10,(positionDKJr*2)+7,2,2);
		pthread_mutex_unlock(&mutexEvenement);

	
	}
	else if(etatDKJr==LIANE_BAS)
	{
		printf("\nlib");
			setGrilleJeu(2,positionDKJr);
		effacerCarres(10,(positionDKJr*2)+7,2,2);

	}
	else
	{
		printf("\naute");
		setGrilleJeu(2,positionDKJr);
		effacerCarres(10,(positionDKJr*2)+7,2,2);
		
	}
	pthread_mutex_unlock(&mutexGrilleJeu);
	pthread_exit(NULL);
}

void HandlerSIGUSR2(int)
{
		S_CROCO*positon=(S_CROCO*)pthread_getspecific(keySpec);
		if(positon->haut==true)
		{
			pthread_mutex_lock(&mutexGrilleJeu);
			effacerCarres(8,positon->position*2+7,1,1);//impair
			setGrilleJeu(1,positon->position);
			pthread_mutex_unlock(&mutexGrilleJeu);
		}
		else
		{
			pthread_mutex_lock(&mutexGrilleJeu);
			effacerCarres(12,positon->position*2+8,1,1);//pair
			setGrilleJeu(1,positon->position);
			pthread_mutex_unlock(&mutexGrilleJeu);
		}
		free(positon);
		pthread_exit(NULL);
		
}
void HandlerSIGHUP(int)
{
	pthread_mutex_lock(&mutexGrilleJeu);
	setGrilleJeu(1,positionDKJr);
	effacerCarres(7,(positionDKJr*2)+7,2,2);
	pthread_mutex_unlock(&mutexGrilleJeu);
	pthread_exit(NULL);

}
void HandlerSIGCHLD(int)
{
	pthread_mutex_lock(&mutexGrilleJeu);
	setGrilleJeu(3,positionDKJr);
	effacerCarres(11,(positionDKJr*2)+7,2,2);
	pthread_mutex_unlock(&mutexGrilleJeu);
	pthread_exit(NULL);
}
void SpawnShield()
{
	if(grilleJeu[2][0].type==CORBEAU)
		{
		printf("\nSIGUSR1 mort 2 0");
		pthread_kill(grilleJeu[2][0].tid,SIGUSR1);
		}
	if(grilleJeu[2][1].type==CORBEAU)
	{
		printf("\nSIGUSR1 mort 2 1");
		pthread_kill(grilleJeu[2][1].tid,SIGUSR1);
	}
	if(grilleJeu[2][2].type==CORBEAU)
	{
		printf("SIGUSR1 mort 2 2");
		pthread_kill(grilleJeu[2][2].tid,SIGUSR1);
	}
	if(grilleJeu[3][1].type==CROCO)
	{
		printf("SIGUSR1 mort 3 1");
		pthread_kill(grilleJeu[3][1].tid,SIGUSR2);
	}
	if(grilleJeu[3][2].type==CROCO)
	{
		printf("SIGUSR1 mort 3 2");
		pthread_kill(grilleJeu[3][2].tid,SIGUSR2);
	}
	if(grilleJeu[3][3].type==CROCO)
	{
		printf("SIGUSR1 mort 3 3");
		pthread_kill(grilleJeu[3][3].tid,SIGUSR2);
	}
}