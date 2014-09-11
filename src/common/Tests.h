#ifndef TESTS
#define TESTS

#include <string>
#include <fstream>
#include "ColorParse.h"
#include <unistd.h> // for usleep

// Constantes liées à l'exécution des tests
#define	TESTS_DUREE_AFFICHAGE_MESSAGE_ERREUR	8	// en secondes
#define	TESTS_COULEUR_MESSAGE_ERREUR			"red"
#define TESTS_COULEUR_MESSAGE_VALIDE			"green"
#define TESTS_DESACTIVER_COUT					1

using namespace std;

class Tests
{
	public:
		Tests(string nomApplication)
		{
			this->nomApplication = nomApplication;
			this->nombreDeTests = 0;
			this->nombreDeTestsValides = 0;
			this->testsValides = true;
			
			inhiberCout();
		}
		
		void inhiberCout()
		{
			if(TESTS_DESACTIVER_COUT)
			{
				this->filestr.open("/dev/null");
				this->backup = cout.rdbuf();
				this->psbuf = this->filestr.rdbuf();
				cout.rdbuf(this->psbuf);
			}
		}
		
		void reactiverCout()
		{
			if(TESTS_DESACTIVER_COUT)
			{
				cout.rdbuf(backup);
				filestr.close();
			}
		}
		
		void afficherConclusionTests()
		{
			reactiverCout();
			
			if(this->nombreDeTests != 0)
			{
				if(this->testsValides)
				{
					cout << termColor(TESTS_COULEUR_MESSAGE_VALIDE);
					
					if(this->nombreDeTests == 1)
						cout << endl << "\tLe test a été validé !" << endl;
					
					else
						cout << endl << "\tLes " << this->nombreDeTests << " tests ont été validés !" << endl;
					
					cout << termColor();
					usleep(1000 * 1000 * TESTS_DUREE_AFFICHAGE_MESSAGE_ERREUR);
					exit(0);
				}
				
				else
				{
					cout << termColor(TESTS_COULEUR_MESSAGE_ERREUR) << endl << "\t";
					
					if(this->nombreDeTestsValides == 0)
						cout << "Aucun test n'est valide !";
					
					else
						cout << (this->nombreDeTests - this->nombreDeTestsValides) << "/" << this->nombreDeTests << " tests ne sont pas valides !";
					
					cout << endl << termColor();
					cin.sync();
					cin.ignore();
				}
			}
		}
		
		void tester(bool resultat, string presentation)
		{
			reactiverCout();
			this->nombreDeTests ++;
			cout << this->nomApplication << " - " << this->nombreDeTests << " \t";
			
			if(resultat)
			{
				this->nombreDeTestsValides ++;
				cout << termColor(TESTS_COULEUR_MESSAGE_VALIDE);
				cout << "[ OK ]";
			}
			
			else
			{
				this->testsValides = false;
				cout << termColor(TESTS_COULEUR_MESSAGE_ERREUR);
				cout << "[ !! ]";
			}
			
			cout << termColor();
			cout << " " << presentation << endl;
			inhiberCout();
		}
		
		~Tests()
		{
			
		}

	private:
		bool testsValides;
		int nombreDeTests;
		int nombreDeTestsValides;
		string nomApplication;
		
		// Pour la désactivation temporaire de std::cout :
		streambuf *psbuf, *backup;
		ofstream filestr;
};

#endif 