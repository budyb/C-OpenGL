// ConsoleApplication13.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>
#include <iostream>


typedef float point3[3];
#define M_PI 3.14159265
int model = 3;  // 1- punkty, 2- siatka, 3 - wypełnione trójkaty, 4- czajnik
struct colour //kolor punktu
{
	float r;
	float g;
	float b;
	float a;
};
const int n = 50;
int czulosc = 10;
struct colour colors[n][n];
static GLfloat viewer[] = { 0.0, 0.0, 15.0 }; //tablica składająca się z trzech liczb, które określają wspolrzedne x, y, z, położenia obserwatora
static GLfloat theta = 0.0;   // kąt obrotu obiektu
static GLfloat beta = 0.0;
static GLfloat pix2angle;     // przelicznik pikseli na stopnie
static GLfloat pix_angle;
static GLfloat zoom = 0.0;
static GLfloat R = 15.0;

static GLint status = 0;       // stan klawiszy myszy
							   // 0 - nie naciśnięto żadnego klawisza
							   // 1 - naciśnięty zostać lewy klawisz
							   // 2 - naciśnięty został prawy klawisz	
static int x_pos_old = 0;       // poprzednia pozycja kursora myszy
static int delta_x = 0;        // różnica pomiędzy pozycją bieżącą
								 // i poprzednią kursora myszy
static int y_pos_old = 0;       // poprzednia pozycja kursora myszy
static int delta_y = 0;
/*************************************************************************************/


float calculateX(float u, float v)
{
	float piv = M_PI * v;
	return((-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u)*cos(piv));
}

float calculateY(float u, float v)
{
	return(160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2));
}

float calculateZ(float u, float v)
{
	float piv = M_PI * v;
	return ((-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u)*sin(piv));
}

void color()
{
	// losowanie koloru dla kazdego punktu
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			colors[i][j].r = ((rand() % 1001)*0.001);
			colors[i][j].g = ((rand() % 1001)*0.001);
			colors[i][j].b = ((rand() % 1001)*0.001);
			colors[i][j].a = ((rand() % 1001)*0.001);			
		}
	}

}



void Axes(void)
{

	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// pocz�tek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	// pocz�tek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	//  pocz�tek i koniec obrazu osi y
	glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
	glBegin(GL_LINES); // rysowanie osi x
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
	glBegin(GL_LINES);  // rysowanie osi y

	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
	glBegin(GL_LINES); // rysowanie osi z

	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();

}

void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)  //sprawdzenie czy lewy przycisk myszy został wciśnięty
	{
		x_pos_old = x;         // przypisanie aktualnie odczytanej pozycji kursora
		y_pos_old = y;		  // jako pozycji poprzedniej
		status = 1;          // wciśnięty został lewy klawisz myszy
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)  //sprawdzenie czy prawy przycisk myszy został wciśnięty
	{
		y_pos_old = y;			// przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
		status = 2;				// wciśnięty został prawy klawisz myszy
	}
	else
		status = 0;          // nie został wciśnięty żaden klawisz
}

void Motion(GLsizei x, GLsizei y)
{

	delta_x = x - x_pos_old;     // obliczenie różnicy położenia kursora myszy
	delta_y = y - y_pos_old;	//
	x_pos_old = x;             // podstawienie bieżącego położenia jako poprzednie
	y_pos_old = y;			  //
	glutPostRedisplay();     // przerysowanie obrazu sceny
}

/*************************************************************************************/

void Egg()
{
	//wspolrzedne punktu
	struct point
	{
		float x;
		float y;
		float z;
	};

	struct point tab[n][n]; //macierz punktów 

	float step = 1.0 / n;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			tab[i][j].x = calculateX(i*step, j*step);
			tab[i][j].y = calculateY(i*step, j*step);
			tab[i][j].z = calculateZ(i*step, j*step);
			//poniewa� we wzorze u oraz v nale�� do [0,1] nalezy przeskalowac wartosci tak aby nalezaly do tego przedzialu
		}
	}

	/****************************************	CHMURA PUNKT�W	************************************************************************************/
	if (model == 1)
	{
		//same punkty
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++)
			{
				glBegin(GL_POINTS);
				glColor3f(1.0f, 1.0f, 0.0f);
				glVertex3f(tab[i][j].x, tab[i][j].y - 5, tab[i][j].z); //-5 od Y aby jajko bylo na srodku
				glEnd();
			}
		}
	}
	/****************************************	SIATKA LINII	************************************************************************************/
	else if (model == 2)
	{	//linie
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		//linie poziome
		for (int i = 0; i < n; i++)
		{
			int roznica = n - i;
			for (int j = 0; j < n - 1; j++) //n-1 poniewaz tab[i][j+1] b�dzie maksymalnie tab[i][n-1] czyli ostatnim punktem 
			{
				glVertex3f(tab[i][j].x, tab[i][j].y - 5, tab[i][j].z);
				glVertex3f(tab[i][j + 1].x, tab[i][j + 1].y - 5, tab[i][j + 1].z); //��czenie s�siednich punkt�w
			}
			if (roznica != n)
			{
				glVertex3f(tab[i][0].x, tab[i][0].y - 5, tab[i][0].z);
				glVertex3f(tab[roznica][n - 1].x, tab[roznica][n - 1].y - 5, tab[roznica][n - 1].z);
				//po��czenie pierwszego i ostatniego punktu w danej iteracji 
			}

		}
		//linie pionowe
		for (int i = 0; i < n; i++)
		{
			int roznica = n - i;
			for (int j = 0; j < n; j++)
			{
				if (i == n - 1)
				{
					glVertex3f(tab[i][j].x, tab[i][j].y - 5, tab[i][j].z);
					glVertex3f(tab[0][0].x, tab[0][0].y - 5, tab[0][0].z);
					//po��czenie pierwszego i ostantiego wierzcho�ka
				}
				else
				{
					glVertex3f(tab[i][j].x, tab[i][j].y - 5, tab[i][j].z);
					glVertex3f(tab[i + 1][j].x, tab[i + 1][j].y - 5, tab[i + 1][j].z);
					//po��czenie s�siednich wierzcho�k�w
				}

			}
		}

		//linie uko�ne
		for (int i = 0; i < n; i++)
		{
			int roznica = n - i;
			for (int j = 0; j < n - 1; j++)
			{
				if (i == n - 1)
				{
					glVertex3f(tab[i][j].x, tab[i][j].y - 5, tab[i][j].z);
					glVertex3f(tab[0][0].x, tab[0][0].y - 5, tab[0][0].z);
					//po��czenie pierwszego i ostantiego wierzcho�ka
				}
				else
				{
					glVertex3f(tab[i][j].x, tab[i][j].y - 5, tab[i][j].z);
					glVertex3f(tab[i + 1][j + 1].x, tab[i + 1][j + 1].y - 5, tab[i + 1][j + 1].z);
					//po��czenie wierzchlokow na skos
				}
			}
			if (roznica != n)
			{
				glVertex3f(tab[i][0].x, tab[i][0].y - 5, tab[i][0].z);
				glVertex3f(tab[roznica + 1][n - 1].x, tab[roznica + 1][n - 1].y - 5, tab[roznica + 1][n - 1].z);
				//po��czenie na ukos skrajnych wierzcho�k�w
			}


		}
		glEnd();
	}
	/****************************************	SIATKA TR�JK�T�W	************************************************************************************/
	else if (model == 3)
	{

		glBegin(GL_TRIANGLES);
		for (int i = 0; i < n; i++)
		{
			int roznica = n - i;
			for (int j = 0; j < n - 1; j++)
			{
				if (i == n - 1)
				{
					glColor4f(colors[i][j].r, colors[i][j].g, colors[i][j].b, colors[i][j].a);
					glVertex3f(tab[i][j].x, tab[i][j].y - 5, tab[i][j].z);
					glColor4f(colors[i][j + 1].r, colors[i][j + 1].g, colors[i][j + 1].b, colors[i][j + 1].a);
					glVertex3f(tab[i][j + 1].x, tab[i][j + 1].y - 5, tab[i][j + 1].z);
					glColor4f(colors[0][0].r, colors[0][0].g, colors[0][0].b, colors[0][0].a);
					glVertex3f(tab[0][0].x, tab[0][0].y - 5, tab[0][0].z);
					//po��czenie ze sob� skrajnych wierzcho�k�w				
				}
				else
				{
					glColor4f(colors[i][j].r, colors[i][j].g, colors[i][j].b, colors[i][j].a);
					glVertex3f(tab[i][j].x, tab[i][j].y - 5, tab[i][j].z);
					glColor4f(colors[i + 1][j].r, colors[i + 1][j].g, colors[i + 1][j].b, colors[i + 1][j].a);
					glVertex3f(tab[i + 1][j].x, tab[i + 1][j].y - 5, tab[i + 1][j].z);
					glColor4f(colors[i + 1][j + 1].r, colors[i + 1][j + 1].g, colors[i + 1][j + 1].b, colors[i + 1][j + 1].a);
					glVertex3f(tab[i + 1][j + 1].x, tab[i + 1][j + 1].y - 5, tab[i + 1][j + 1].z);

					glColor4f(colors[i][j].r, colors[i][j].g, colors[i][j].b, colors[i][j].a);
					glVertex3f(tab[i][j].x, tab[i][j].y - 5, tab[i][j].z);
					glColor4f(colors[i][j + 1].r, colors[i][j + 1].g, colors[i][j + 1].b, colors[i][j + 1].a);
					glVertex3f(tab[i][j + 1].x, tab[i][j + 1].y - 5, tab[i][j + 1].z);
					glColor4f(colors[i + 1][j + 1].r, colors[i + 1][j + 1].g, colors[i + 1][j + 1].b, colors[i + 1][j + 1].a);
					glVertex3f(tab[i + 1][j + 1].x, tab[i + 1][j + 1].y - 5, tab[i + 1][j + 1].z);

					//narysowanie dw�ch tr�jk�t�w tak aby si� po��czy�y i tworzy�y kwadrat
				}
			}
			if (roznica != n)
			{
				glColor4f(colors[i][0].r, colors[i][0].g, colors[i][0].b, colors[i][0].a);
				glVertex3f(tab[i][0].x, tab[i][0].y - 5, tab[i][0].z);
				glColor4f(colors[roznica][n - 1].r, colors[roznica][n - 1].g, colors[roznica][n - 1].b, colors[roznica][n - 1].a);
				glVertex3f(tab[roznica][n - 1].x, tab[roznica][n - 1].y - 5, tab[roznica][n - 1].z);
				glColor4f(colors[roznica + 1][n - 1].r, colors[roznica + 1][n - 1].g, colors[roznica + 1][n - 1].b, colors[roznica + 1][n - 1].a);
				glVertex3f(tab[roznica + 1][n - 1].x, tab[roznica + 1][n - 1].y - 5, tab[roznica + 1][n - 1].z);

				//po��czenie punktu pocz�tkowego(dla j) z koncowym oraz jego s�siadem

				glColor4f(colors[i][0].r, colors[i][0].g, colors[i][0].b, colors[i][0].a);
				glVertex3f(tab[i][0].x, tab[i][0].y - 5, tab[i][0].z);
				glColor4f(colors[i - 1][0].r, colors[i - 1][0].g, colors[i - 1][0].b, colors[i - 1][0].a);
				glVertex3f(tab[i - 1][0].x, tab[i - 1][0].y - 5, tab[i - 1][0].z);
				glColor4f(colors[roznica + 1][n - 1].r, colors[roznica + 1][n - 1].g, colors[roznica + 1][n - 1].b, colors[roznica + 1][n - 1].a);
				glVertex3f(tab[roznica + 1][n - 1].x, tab[roznica + 1][n - 1].y - 5, tab[roznica + 1][n - 1].z);

			}


		}

		glColor4f(colors[0][0].r, colors[0][0].g, colors[0][0].b, colors[0][0].a);
		glVertex3f(tab[0][0].x, tab[0][0].y - 5, tab[0][0].z);
		glColor4f(colors[n - 1][0].r, colors[n - 1][0].g, colors[n - 1][0].b, colors[n - 1][0].a);
		glVertex3f(tab[n - 1][0].x, tab[n - 1][0].y - 5, tab[n - 1][0].z);
		glColor4f(colors[1][n - 1].r, colors[1][n - 1].g, colors[1][n - 1].b, colors[1][n - 1].a);
		glVertex3f(tab[1][n - 1].x, tab[1][n - 1].y - 5, tab[1][n - 1].z);

		glColor4f(colors[0][0].r, colors[0][0].g, colors[0][0].b, colors[0][0].a);
		glVertex3f(tab[0][0].x, tab[0][0].y - 5, tab[0][0].z);
		glColor4f(colors[1][0].r, colors[1][0].g, colors[1][0].b, colors[1][0].a);
		glVertex3f(tab[1][0].x, tab[1][0].y - 5, tab[1][0].z);
		glColor4f(colors[n - 1][n - 1].r, colors[n - 1][n - 1].g, colors[n - 1][n - 1].b, colors[n - 1][n - 1].a);
		glVertex3f(tab[n - 1][n - 1].x, tab[n - 1][n - 1].y - 5, tab[n - 1][n - 1].z);

		//po��czenie punkt�w ze skrajnych przedzia��w kt�re nie zosta�y wcze�niej ze sob� po��czone
		glEnd();

	}
}



void RenderScene(void)
{
	GLfloat viewoption = 1.0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczacym
	glLoadIdentity();
	if (model != 4)	  //jesli wybrany model nie jest czajnikiem 
	{
		if (status == 1)     // jeśli lewy klawisz myszy wciśnięty
		{
			theta += delta_x * pix2angle / czulosc;   // modyfikacja kąta obrotu o kat proporcjonalny
													 //im wieksza wartosc czulosci tym mniejszy bedzie obrot
			beta += delta_y * pix2angle / czulosc;	// modyfikacja kąta obrotu o kat proporcjonalny

			if (beta > 2 * M_PI)  //beta musi byc w przedziale [0,2 PI]
				beta = 0;		//wartosc funkcji sin i cos bedzie taka sama w 0 jak i w 2 PI
			if (beta < 0)
				beta = 2 * M_PI;
			if (beta > M_PI / 2 && beta<(1.5)*M_PI)	//ze wzgledu na okresowowsc funkcji sin wystepuje problem z pelnym obrotem
				viewoption = -1.0;	//dlatego nalezy w okreslonym przupadku zmienic wartosc wektora kamery
			else
				viewoption = 1.0;
		}
			else if (status == 2)
			{
				R += delta_y * pix2angle / czulosc;  //ustalanie promienia (odleglosci od obiektu)
			}
			viewer[0] = R * cos(theta)*cos(beta);  //wpisanie pozycji obserwatora wedlug zadanych wzorow
			viewer[1] = R * sin(beta);
			viewer[2] = R * sin(theta)*cos(beta);
			gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, viewoption, 0.0); //nowe pozycje obserwatora 
			Axes();

			glColor3f(1.0f, 1.0f, 1.0f); // Ustawienie koloru rysowania na bialy
			Egg();
		}
		else    // czajnik
		{
			if (status == 1)  // jesli lewy klawisz myszy wcisniety
			{
				theta += delta_x * pix2angle;  // modyfikacja kąta obrotu o kat proporcjonalny
				beta += delta_y * pix2angle;  // do ronzicy polozenia kursora myszy
			}                                  
			else if (status == 2)
			{
				zoom += delta_y * pix2angle / 100;
				viewer[2] += zoom;  //ustalanie zoomu
			}
			gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
			Axes();
			glRotatef(theta, 0.0, 1.0, 0.0);  //obrot obiektu o nowy kat
			glRotatef(beta, 1.0, 0.0, 0.0);  //obrot obiektu o nowy kat

			glColor3f(1.0f, 1.0f, 1.0f);
			// Ustawienie koloru rysowania na bialy

			glutWireTeapot(3.0); //rysowanie czajnika
		}
		glFlush();
		glutSwapBuffers();
	}

/*************************************************************************************/

// Funkcja ustalająca stan renderowania



void MyInit(void)
{

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszc�cy (wype�nienia okna) ustawiono na czarny

}




void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie
	pix_angle = 360.0 / (float)vertical;
	GLfloat AspectRatio;
	// Deklaracja zmiennej AspectRatio  okre�laj�cej proporcj�
	// wymiar�w okna 

	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielko�ciokna okna widoku (viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)  
	glMatrixMode(GL_PROJECTION);
	// Prze��czenie macierzy bie��cej na macierz projekcji 
	glLoadIdentity();
	gluPerspective(70, 1.0, 1.0, 30.0);
	// Czyszcznie macierzy bie��cej            
	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie wsp�czynnika  proporcji okna
	// Gdy okno nie jest kwadratem wymagane jest okre�lenie tak zwanej
	// przestrzeni ograniczaj�cej pozwalaj�cej zachowa� w�a�ciwe
	// proporcje rysowanego obiektu.
	// Do okreslenia przestrzeni ograniczj�cej s�u�y funkcja
	// glOrtho(...)            
	if (horizontal <= vertical)

		glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio, 10.0, -10.0);
	else

		glOrtho(-7.5*AspectRatio, 7.5*AspectRatio, -7.5, 7.5, 10.0, -10.0);
	glMatrixMode(GL_MODELVIEW);
	// Prze��czenie macierzy bie��cej na macierz widoku modelu                                   

	glLoadIdentity();
	// Czyszcenie macierzy bie��cej
}

/*************************************************************************************/
void ustawienie()
{	
	std::cout << "Im wieksza podana wartosc tym wolniejszy bedzie obrot" << std::endl;
	do
	{
		std::cout << "Podaj czulosc" << std::endl;
		std::cin >> czulosc;
	}
	while (czulosc <= 0);
}


void keys(unsigned char key, int x, int y)
{
	if (key == 'p') model = 1;
	if (key == 'w') model = 2;
	if (key == 's') model = 3;
	if (key == 'c') model = 4;
	if (key == 'm') ustawienie();
	if (key == 'e') exit(0);
	RenderScene(); // przerysowanie obrazu sceny
}


void main(void)
{
	std::cout << "p- punkty, w- siatka, s - wypelnione trojkaty, c- czajnik, m- zmien czulosc myszy, e- wyjscie z programu" << std::endl;
	color();

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(300, 300);


	glutCreateWindow("Uklad wspolrzednych 3-D");

	glutDisplayFunc(RenderScene);
	// Okre�lenie, �e funkcja RenderScene b�dzie funkcj� zwrotn�
	// (callback function).  Bedzie ona wywo�ywana za ka�dym razem
	// gdy zajdzie potrzba przeryswania okna 
	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn�
	// zazmiany rozmiaru okna      
	MyInit();
	// Funkcja MyInit() (zdefiniowana powy�ej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przyst�pieniem do renderowania
	glEnable(GL_DEPTH_TEST);
	// Wlaczenie mechanizmu usuwania powierzchni niewidocznych
	glutKeyboardFunc(keys);
	glutMouseFunc(Mouse);
	// Ustala funkcję zwrotną odpowiedzialną za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcję zwrotną odpowiedzialną za badanie ruchu myszy
	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}

/*************************************************************************************/


