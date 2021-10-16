#include "pch.h"

#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>
#include <iostream>


typedef float point3[3];
typedef float point4[4];
#define M_PI 3.14159265
int model = 3;  // 1- punkty, 2- siatka, 3 - wypełnione trójkaty, 4- czajnik
int numer = 0;
struct colour //kolor punktu
{
	float r;
	float g;
	float b;
	float a;
};
const int n = 100;
int czulosc = 10;
struct colour colors[n][n];
static GLfloat viewer[] = { 20.0, 0.0, 15.0 }; //tablica składająca się z trzech liczb, które określają wspolrzedne x, y, z, położenia obserwatora
static GLfloat theta = 0.0;   // kąt obrotu obiektu
static GLfloat beta = 0.0;
static GLfloat thetaLight1 = 0.0;   // kąt obrotu swiatla1
static GLfloat betaLight1 = 0.0;
static GLfloat thetaLight2 = 0.0;   // kąt obrotu swiatla2
static GLfloat betaLight2 = 0.0;
static GLfloat pix2angle;     // przelicznik pikseli na stopnie
static GLfloat pix_angle;
static GLfloat zoom = 10.0;
static GLfloat R = 20.0;
GLfloat light1[3] = { 0.0,0.0,10.0 };
GLfloat light2[3] = { 0.0,0.0,10.0 };
bool mode = 0;
int lightmode = 3;

struct wektor
{
	GLfloat Nx;
	GLfloat Ny;
	GLfloat Nz;
};


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

float calculateXu(float u, float v)
{
	float piv = M_PI * v;
	return ((-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*cos(piv));
}

float calculateXv(float u, float v)
{
	float piv = M_PI * v;
	return (M_PI*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*sin(piv));
}

float calculateYu(float u, float v)
{
	return(640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u);
}

float calculateYv(float u, float v)
{
	return 0;
}

float calculateZu(float u, float v)
{
	float piv = M_PI * v;
	return((-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*sin(piv));
}

float calculateZv(float u, float v)
{
	float piv = M_PI * v;
	return((-M_PI)*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*cos(piv));
}

float length(float xu, float xv, float yu, float yv, float zu, float zv)
{
	return(sqrt(pow((yu*zv - zu * yv), 2) + pow((zu*xv - xu * zv), 2) + pow((xu*yv - yu * xv), 2)));
}

float normalizeX(float xu, float xv, float yu, float yv, float zu, float zv)
{
	return((yu*zv - zu * yv) / length(xu, xv, yu, yv, zu, zv));
}

float normalizeY(float xu, float xv, float yu, float yv, float zu, float zv)
{
	return((zu*xv - xu * zv) / length(xu, xv, yu, yv, zu, zv));
}

float normalizeZ(float xu, float xv, float yu, float yv, float zu, float zv)
{
	return((xu*yv - yu * xv) / length(xu, xv, yu, yv, zu, zv));
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

void triangle1() {
	wektor vec[2];
	vec[0] = { -5, 0, 5 };
	vec[1] = { 5, 0, 5 };
	wektor normalny;
	normalny = { vec[0].Ny*vec[1].Nz - vec[1].Ny*vec[0].Nz, vec[1].Nx*vec[0].Nz - vec[0].Nx*vec[1].Nz, vec[0].Nx*vec[1].Ny - vec[1].Nx*vec[0].Ny };
	GLfloat dlugosc0 = sqrt(pow(normalny.Nx, 2) + pow(normalny.Ny, 2) + pow(normalny.Nz, 2));
	normalny = { normalny.Nx / dlugosc0, normalny.Ny / dlugosc0, normalny.Nz / dlugosc0 };

	glBegin(GL_TRIANGLES);
	glNormal3f(-normalny.Nx, -normalny.Ny, -normalny.Nz);
	glTexCoord2f(0.0f, 1.0);
	glVertex3f(0, 5, 0);
	glNormal3f(-normalny.Nx, -normalny.Ny, -normalny.Nz);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(vec[0].Nx, vec[0].Ny, vec[0].Nz);
	glNormal3f(-normalny.Nx, -normalny.Ny, -normalny.Nz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(vec[1].Nx, vec[1].Ny, vec[1].Nz);
	glEnd();
}




void triangle2() {

	wektor vec[2];
	vec[0] = { 5 , 0 , -5 };
	vec[1] = { -5 , 0 , -5 };
	wektor normalny;
	normalny = { vec[0].Ny*vec[1].Nz - vec[1].Ny*vec[0].Nz, vec[1].Nx*vec[0].Nz - vec[0].Nx*vec[1].Nz, vec[0].Nx*vec[1].Ny - vec[1].Nx*vec[0].Ny };
	GLfloat dlugosc0 = sqrt(pow(normalny.Nx, 2) + pow(normalny.Ny, 2) + pow(normalny.Nz, 2));
	normalny = { normalny.Nx / dlugosc0, normalny.Ny / dlugosc0, normalny.Nz / dlugosc0 };

	glBegin(GL_TRIANGLES);
	glNormal3f(normalny.Nx, normalny.Ny, normalny.Nz);
	glTexCoord2f(0.0f, 1.0);
	glVertex3f(0, 5, 0);
	glNormal3f(normalny.Nx, normalny.Ny, normalny.Nz);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(vec[0].Nx, vec[0].Ny, vec[0].Nz);
	glNormal3f(normalny.Nx, normalny.Ny, normalny.Nz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(vec[1].Nx, vec[1].Ny, vec[1].Nz);
	glEnd();
}

void triangle3() {

	wektor vec[2];
	vec[0] = { 5 , 0 , 5 };
	vec[1] = { 5 , 0 , -5 };
	wektor normalny;
	normalny = { vec[0].Ny*vec[1].Nz - vec[1].Ny*vec[0].Nz, vec[1].Nx*vec[0].Nz - vec[0].Nx*vec[1].Nz, vec[0].Nx*vec[1].Ny - vec[1].Nx*vec[0].Ny };
	GLfloat dlugosc0 = sqrt(pow(normalny.Nx, 2) + pow(normalny.Ny, 2) + pow(normalny.Nz, 2));
	normalny = { normalny.Nx / dlugosc0, normalny.Ny / dlugosc0, normalny.Nz / dlugosc0 };

	glBegin(GL_TRIANGLES);
	glNormal3f(normalny.Nx, normalny.Ny, normalny.Nz);
	glTexCoord2f(0.0f, 1.0);
	glVertex3f(0, 5, 0);
	glNormal3f(normalny.Nx, normalny.Ny, normalny.Nz);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(vec[0].Nx, vec[0].Ny, vec[0].Nz);
	glNormal3f(normalny.Nx, normalny.Ny, normalny.Nz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(vec[1].Nx, vec[1].Ny, vec[1].Nz);
	glEnd();
}

void triangle4() {

	wektor vec[2];
	vec[0] = { -5 , 0 , -5 };
	vec[1] = { -5 , 0 , 5 };
	wektor normalny;
	normalny = { vec[0].Ny*vec[1].Nz - vec[1].Ny*vec[0].Nz, vec[1].Nx*vec[0].Nz - vec[0].Nx*vec[1].Nz, vec[0].Nx*vec[1].Ny - vec[1].Nx*vec[0].Ny };
	GLfloat dlugosc0 = sqrt(pow(normalny.Nx, 2) + pow(normalny.Ny, 2) + pow(normalny.Nz, 2));
	normalny = { normalny.Nx / dlugosc0, normalny.Ny / dlugosc0, normalny.Nz / dlugosc0 };

	glBegin(GL_TRIANGLES);
	glNormal3f(normalny.Nx, normalny.Ny, normalny.Nz);
	glTexCoord2f(0.0f, 1.0);
	glVertex3f(0, 5, 0);
	glNormal3f(normalny.Nx, normalny.Ny, normalny.Nz);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(vec[0].Nx, vec[0].Ny, vec[0].Nz);
	glNormal3f(normalny.Nx, normalny.Ny, normalny.Nz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(vec[1].Nx, vec[1].Ny, vec[1].Nz);
	glEnd();
}

void triangle5() {

	wektor vec[2];
	vec[0] = { 5 , 0 , 5 };
	vec[1] = { 5 , 0 , -5 };
	wektor normalny;
	normalny = { vec[0].Ny*vec[1].Nz - vec[1].Ny*vec[0].Nz, vec[1].Nx*vec[0].Nz - vec[0].Nx*vec[1].Nz, vec[0].Nx*vec[1].Ny - vec[1].Nx*vec[0].Ny };
	GLfloat dlugosc0 = sqrt(pow(normalny.Nx, 2) + pow(normalny.Ny, 2) + pow(normalny.Nz, 2));
	normalny = { normalny.Nx / dlugosc0, normalny.Ny / dlugosc0, normalny.Nz / dlugosc0 };

	glBegin(GL_TRIANGLES);
	glNormal3f(normalny.Nx, normalny.Ny, normalny.Nz);
	glTexCoord2f(0.0f, 1.0);
	glVertex3f(-5, 0, -5);
	glNormal3f(normalny.Nx, normalny.Ny, normalny.Nz);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(5, 0, -5);
	glNormal3f(normalny.Nx, normalny.Ny, normalny.Nz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(5, 0, 5);

	glNormal3f(normalny.Nx, normalny.Ny, normalny.Nz);
	glTexCoord2f(0.0f, 1.0);
	glVertex3f(-5, 0, -5);
	glNormal3f(normalny.Nx, normalny.Ny, normalny.Nz);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(5, 0, 5);
	glNormal3f(normalny.Nx, normalny.Ny, normalny.Nz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-5, 0, 5);

	glEnd();
}




void triangle() {
	if (numer == 0)
	{
		triangle1();
		triangle2();
		triangle3();
		triangle4();
		triangle5();
	}
	else if (numer == 1) {
		triangle1();
	}
	else if (numer == 2) {
		triangle2();
	}
	else if (numer == 3) {
		triangle3();
	}
	else if (numer == 4) {
		triangle4();
	}
	else if (numer == 5) {
		triangle5();
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
		float xu;
		float xv;
		float yu;
		float yv;
		float zu;
		float zv;

	};
	struct wektor
	{
		GLfloat Nx;
		GLfloat Ny;
		GLfloat Nz;
	};
	struct point tab[n][n]; //macierz punktów 
	struct wektor wek[n][n]; //macierz wektorow
	float step = 1.0 / n;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			//poniewaz we wzorach i oraz j naleza do [0,1] nalezy przeskalowac wartosci tak aby nalezaly do tego przedzialu
			tab[i][j].x = calculateX(i*step, j*step);
			tab[i][j].y = calculateY(i*step, j*step);
			tab[i][j].z = calculateZ(i*step, j*step);
			tab[i][j].xu = calculateXu(i*step, j*step);
			tab[i][j].xv = calculateXv(i*step, j*step);
			tab[i][j].yu = calculateYu(i*step, j*step);
			tab[i][j].yv = calculateYv(i*step, j*step);
			tab[i][j].zu = calculateZu(i*step, j*step);
			tab[i][j].zv = calculateZv(i*step, j*step);
			wek[i][j].Nx = normalizeX(tab[i][j].xu, tab[i][j].xv, tab[i][j].yu, tab[i][j].yv, tab[i][j].zu, tab[i][j].zv);
			wek[i][j].Ny = normalizeY(tab[i][j].xu, tab[i][j].xv, tab[i][j].yu, tab[i][j].yv, tab[i][j].zu, tab[i][j].zv);
			wek[i][j].Nz = normalizeZ(tab[i][j].xu, tab[i][j].xv, tab[i][j].yu, tab[i][j].yv, tab[i][j].zu, tab[i][j].zv);
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
			if (roznica < n - 1)
			{
				glVertex3f(tab[i][0].x, tab[i][0].y - 5, tab[i][0].z);
				glVertex3f(tab[roznica + 1][n - 1].x, tab[roznica + 1][n - 1].y - 5, tab[roznica + 1][n - 1].z);
				//po��czenie na ukos skrajnych wierzcho�k�w
			}


		}
		glEnd();
	}
	/****************************************	SIATKA TROJKATOW	************************************************************************************/
	else if (model == 3)
	{
		float ta[3] = { 0,0,0 };
		glBegin(GL_TRIANGLES);

		//jajko zostało podzielone na dwie czesci


		for (int i = 0; i < n / 2; i++)
		{
			int roznica = n - i;
			for (int j = 0; j < n - 1; j++)
			{
				{
					glColor4f(colors[i + 1][j].r, colors[i + 1][j].g, colors[i + 1][j].b, colors[i + 1][j].a);
					glNormal3f(wek[i + 1][j].Nx, wek[i + 1][j].Ny, wek[i + 1][j].Nz);
					glTexCoord2f((i + 1)*step, j*step);
					glVertex3f(tab[i + 1][j].x, tab[i + 1][j].y - 5, tab[i + 1][j].z);

					glColor4f(colors[i + 1][j + 1].r, colors[i + 1][j + 1].g, colors[i + 1][j + 1].b, colors[i + 1][j + 1].a);
					glNormal3f(wek[i + 1][j + 1].Nx, wek[i + 1][j + 1].Ny, wek[i + 1][j + 1].Nz);
					glTexCoord2f((i + 1)*step, (j + 1)*step);
					glVertex3f(tab[i + 1][j + 1].x, tab[i + 1][j + 1].y - 5, tab[i + 1][j + 1].z);

					glColor4f(colors[i][j].r, colors[i][j].g, colors[i][j].b, colors[i][j].a);
					glNormal3f(wek[i][j].Nx, wek[i][j].Ny, wek[i][j].Nz);
					glTexCoord2f(i*step, j*step);
					glVertex3f(tab[i][j].x, tab[i][j].y - 5, tab[i][j].z);


					glColor4f(colors[i + 1][j + 1].r, colors[i + 1][j + 1].g, colors[i + 1][j + 1].b, colors[i + 1][j + 1].a);
					glNormal3f(wek[i + 1][j + 1].Nx, wek[i + 1][j + 1].Ny, wek[i + 1][j + 1].Nz);
					glTexCoord2f((i + 1)*step, (j + 1)*step);
					glVertex3f(tab[i + 1][j + 1].x, tab[i + 1][j + 1].y - 5, tab[i + 1][j + 1].z);

					glColor4f(colors[i][j + 1].r, colors[i][j + 1].g, colors[i][j + 1].b, colors[i][j + 1].a);
					glNormal3f(wek[i][j + 1].Nx, wek[i][j + 1].Ny, wek[i][j + 1].Nz);
					glTexCoord2f(i*step, (j + 1)*step);
					glVertex3f(tab[i][j + 1].x, tab[i][j + 1].y - 5, tab[i][j + 1].z);

					glColor4f(colors[i][j].r, colors[i][j].g, colors[i][j].b, colors[i][j].a);
					glNormal3f(wek[i][j].Nx, wek[i][j].Ny, wek[i][j].Nz);
					glTexCoord2f(i*step, j*step);
					glVertex3f(tab[i][j].x, tab[i][j].y - 5, tab[i][j].z);




					//narysowanie dwóch trójkątów tak aby się połączyły i tworzyły kwadrat
				}
			}
			if (roznica < n - 1)
			{

				glColor4f(colors[i - 1][0].r, colors[i - 1][0].g, colors[i - 1][0].b, colors[i - 1][0].a);
				glNormal3f(wek[i - 1][0].Nx, wek[i - 1][0].Ny, wek[i - 1][0].Nz);
				glTexCoord2f((roznica + 1)*step, (n - 1)*step);
				glVertex3f(tab[i - 1][0].x, tab[i - 1][0].y - 5, tab[i - 1][0].z);

				glColor4f(colors[roznica + 1][n - 1].r, colors[roznica + 1][n - 1].g, colors[roznica + 1][n - 1].b, colors[roznica + 1][n - 1].a);
				glNormal3f(-wek[roznica + 1][n - 1].Nx, -wek[roznica + 1][n - 1].Ny, -wek[roznica + 1][n - 1].Nz);
				glTexCoord2f((roznica + 1)*step, (n - 1)*step);
				glVertex3f(tab[roznica + 1][n - 1].x, tab[roznica + 1][n - 1].y - 5, tab[roznica + 1][n - 1].z);

				glColor4f(colors[i][0].r, colors[i][0].g, colors[i][0].b, colors[i][0].a);
				glNormal3f(wek[i][0].Nx, wek[i][0].Ny, wek[i][0].Nz);
				glTexCoord2f(roznica*step, (n - 1)*step);
				glVertex3f(tab[i][0].x, tab[i][0].y - 5, tab[i][0].z);


				glColor4f(colors[roznica + 1][n - 1].r, colors[roznica + 1][n - 1].g, colors[roznica + 1][n - 1].b, colors[roznica + 1][n - 1].a);
				glNormal3f(-wek[roznica + 1][n - 1].Nx, -wek[roznica + 1][n - 1].Ny, -wek[roznica + 1][n - 1].Nz);
				glTexCoord2f((roznica + 1)*step, (n - 1)*step);
				glVertex3f(tab[roznica + 1][n - 1].x, tab[roznica + 1][n - 1].y - 5, tab[roznica + 1][n - 1].z);

				glColor4f(colors[roznica][n - 1].r, colors[roznica][n - 1].g, colors[roznica][n - 1].b, colors[roznica][n - 1].a);
				glNormal3f(-wek[roznica][n - 1].Nx, -wek[roznica][n - 1].Ny, -wek[roznica][n - 1].Nz);
				glTexCoord2f(roznica*step, (n - 1)*step);
				glVertex3f(tab[roznica][n - 1].x, tab[roznica][n - 1].y - 5, tab[roznica][n - 1].z);

				glColor4f(colors[i][0].r, colors[i][0].g, colors[i][0].b, colors[i][0].a);
				glNormal3f(wek[i][0].Nx, wek[i][0].Ny, wek[i][0].Nz);
				glTexCoord2f((roznica)*step, (n - 1)*step);
				glVertex3f(tab[i][0].x, tab[i][0].y - 5, tab[i][0].z);

			}
		}

		for (int i = n / 2; i < n; i++)
		{
			int roznica = n - i;
			for (int j = 0; j < n - 1; j++)
			{
				if (i == n - 1)
				{
					glColor4f(colors[0][0].r, colors[0][0].g, colors[0][0].b, colors[0][0].a);
					glNormal3f(wek[0][0].Nx, wek[0][0].Ny, wek[0][0].Nz);
					glTexCoord2f(0, 0);
					glVertex3f(tab[0][0].x, tab[0][0].y - 5, tab[0][0].z);

					glColor4f(colors[i][j].r, colors[i][j].g, colors[i][j].b, colors[i][j].a);
					glNormal3f(-wek[i][j].Nx, -wek[i][j].Ny, -wek[i][j].Nz);
					glTexCoord2f(i*step, j*step);
					glVertex3f(tab[i][j].x, tab[i][j].y - 5, tab[i][j].z);

					glColor4f(colors[i][j + 1].r, colors[i][j + 1].g, colors[i][j + 1].b, colors[i][j + 1].a);
					glNormal3f(-wek[i][j + 1].Nx, -wek[i][j + 1].Ny, -wek[i][j + 1].Nz);
					glTexCoord2f(i*step, (j + 1)*step);
					glVertex3f(tab[i][j + 1].x, tab[i][j + 1].y - 5, tab[i][j + 1].z);

					//połączenie ze sobą skrajnych wierzchołków				
				}
				else
				{

					glColor4f(colors[i][j].r, colors[i][j].g, colors[i][j].b, colors[i][j].a);
					glNormal3f(-wek[i][j].Nx, -wek[i][j].Ny, -wek[i][j].Nz);
					glTexCoord2f(i*step, j*step);
					glVertex3f(tab[i][j].x, tab[i][j].y - 5, tab[i][j].z);

					glColor4f(colors[i + 1][j + 1].r, colors[i + 1][j + 1].g, colors[i + 1][j + 1].b, colors[i + 1][j + 1].a);
					glNormal3f(-wek[i + 1][j + 1].Nx, -wek[i + 1][j + 1].Ny, -wek[i + 1][j + 1].Nz);
					glTexCoord2f((i + 1)*step, (j + 1)*step);
					glVertex3f(tab[i + 1][j + 1].x, tab[i + 1][j + 1].y - 5, tab[i + 1][j + 1].z);

					glColor4f(colors[i + 1][j].r, colors[i + 1][j].g, colors[i + 1][j].b, colors[i + 1][j].a);
					glNormal3f(-wek[i + 1][j].Nx, -wek[i + 1][j].Ny, -wek[i + 1][j].Nz);
					glTexCoord2f((i + 1)*step, j*step);
					glVertex3f(tab[i + 1][j].x, tab[i + 1][j].y - 5, tab[i + 1][j].z);



					glColor4f(colors[i][j].r, colors[i][j].g, colors[i][j].b, colors[i][j].a);
					glNormal3f(-wek[i][j].Nx, -wek[i][j].Ny, -wek[i][j].Nz);
					glTexCoord2f(i*step, j*step);;
					glVertex3f(tab[i][j].x, tab[i][j].y - 5, tab[i][j].z);

					glColor4f(colors[i][j + 1].r, colors[i][j + 1].g, colors[i][j + 1].b, colors[i][j + 1].a);
					glNormal3f(-wek[i][j + 1].Nx, -wek[i][j + 1].Ny, -wek[i][j + 1].Nz);
					glTexCoord2f(i*step, (j + 1)*step);
					glVertex3f(tab[i][j + 1].x, tab[i][j + 1].y - 5, tab[i][j + 1].z);

					glColor4f(colors[i + 1][j + 1].r, colors[i + 1][j + 1].g, colors[i + 1][j + 1].b, colors[i + 1][j + 1].a);
					glNormal3f(-wek[i + 1][j + 1].Nx, -wek[i + 1][j + 1].Ny, -wek[i + 1][j + 1].Nz);
					glTexCoord2f((i + 1)*step, (j + 1)*step);
					glVertex3f(tab[i + 1][j + 1].x, tab[i + 1][j + 1].y - 5, tab[i + 1][j + 1].z);

					//narysowanie dwóch trójkątów tak aby się połączyły i tworzyły kwadrat
				}
			}
			if (roznica < n - n / 2)
			{
				glColor4f(colors[i][0].r, colors[i][0].g, colors[i][0].b, colors[i][0].a);
				glNormal3f(-wek[i][0].Nx, -wek[i][0].Ny, -wek[i][0].Nz);
				glTexCoord2f((roznica)*step, (n - 1)*step);
				glVertex3f(tab[i][0].x, tab[i][0].y - 5, tab[i][0].z);

				glColor4f(colors[roznica][n - 1].r, colors[roznica][n - 1].g, colors[roznica][n - 1].b, colors[roznica][n - 1].a);
				glNormal3f(wek[roznica][n - 1].Nx, wek[roznica][n - 1].Ny, wek[roznica][n - 1].Nz);
				glTexCoord2f((roznica)*step, (n - 1)*step);
				glVertex3f(tab[roznica][n - 1].x, tab[roznica][n - 1].y - 5, tab[roznica][n - 1].z);

				glColor4f(colors[roznica + 1][n - 1].r, colors[roznica + 1][n - 1].g, colors[roznica + 1][n - 1].b, colors[roznica + 1][n - 1].a);
				glNormal3f(wek[roznica + 1][n - 1].Nx, wek[roznica + 1][n - 1].Ny, wek[roznica + 1][n - 1].Nz);
				glTexCoord2f((roznica)*step, (n - 1)*step);
				glVertex3f(tab[roznica + 1][n - 1].x, tab[roznica + 1][n - 1].y - 5, tab[roznica + 1][n - 1].z);




				glColor4f(colors[i - 1][0].r, colors[i - 1][0].g, colors[i - 1][0].b, colors[i - 1][0].a);
				glNormal3f(-wek[i - 1][0].Nx, -wek[i - 1][0].Ny, -wek[i - 1][0].Nz);
				glTexCoord2f((roznica + 1)*step, (n - 1)*step);
				glVertex3f(tab[i - 1][0].x, tab[i - 1][0].y - 5, tab[i - 1][0].z);

				glColor4f(colors[i][0].r, colors[i][0].g, colors[i][0].b, colors[i][0].a);
				glNormal3f(-wek[i][0].Nx, -wek[i][0].Ny, -wek[i][0].Nz);
				glTexCoord2f((roznica + 1)*step, (n - 1)*step);
				glVertex3f(tab[i][0].x, tab[i][0].y - 5, tab[i][0].z);

				glColor4f(colors[roznica + 1][n - 1].r, colors[roznica + 1][n - 1].g, colors[roznica + 1][n - 1].b, colors[roznica + 1][n - 1].a);
				glNormal3f(wek[roznica + 1][n - 1].Nx, wek[roznica + 1][n - 1].Ny, wek[roznica + 1][n - 1].Nz);
				glTexCoord2f((roznica)*step, (n - 1)*step);
				glVertex3f(tab[roznica + 1][n - 1].x, tab[roznica + 1][n - 1].y - 5, tab[roznica + 1][n - 1].z);



			}
			else if (roznica == n / 2)
			{
				glColor4f(colors[roznica + 1][n - 1].r, colors[roznica + 1][n - 1].g, colors[roznica + 1][n - 1].b, colors[roznica + 1][n - 1].a);
				glNormal3f(-wek[roznica + 1][n - 1].Nx, -wek[roznica + 1][n - 1].Ny, -wek[roznica + 1][n - 1].Nz);
				glTexCoord2f((roznica + 1)*step, (n - 1)*step);
				glVertex3f(tab[roznica + 1][n - 1].x, tab[roznica + 1][n - 1].y - 5, tab[roznica + 1][n - 1].z);

				glColor4f(colors[i][0].r, colors[i][0].g, colors[i][0].b, colors[i][0].a);
				glNormal3f(wek[i][0].Nx, wek[i][0].Ny, wek[i][0].Nz);
				glTexCoord2f(i*step, 0);
				glVertex3f(tab[i][0].x, tab[i][0].y - 5, tab[i][0].z);

				glColor4f(colors[i - 1][0].r, colors[i - 1][0].g, colors[i - 1][0].b, colors[i - 1][0].a);
				glNormal3f(wek[i - 1][0].Nx, wek[i - 1][0].Ny, wek[i - 1][0].Nz);
				glTexCoord2f((i - 1)*step, 0);
				glVertex3f(tab[i - 1][0].x, tab[i - 1][0].y - 5, tab[i - 1][0].z);


			}
		}


		glColor4f(colors[0][0].r, colors[0][0].g, colors[0][0].b, colors[0][0].a);
		glNormal3f(-wek[0][0].Nx, -wek[0][0].Ny, -wek[0][0].Nz);
		glTexCoord2f(0, 0);
		glVertex3f(tab[0][0].x, tab[0][0].y - 5, tab[0][0].z);

		glColor4f(colors[1][n - 1].r, colors[1][n - 1].g, colors[1][n - 1].b, colors[1][n - 1].a);
		glNormal3f(-wek[1][n - 1].Nx, -wek[1][n - 1].Ny, -wek[1][n - 1].Nz);
		glTexCoord2f(1 * step, (n - 1)*step);
		glVertex3f(tab[1][n - 1].x, tab[1][n - 1].y - 5, tab[1][n - 1].z);

		glColor4f(colors[n - 1][0].r, colors[n - 1][0].g, colors[n - 1][0].b, colors[n - 1][0].a);
		glNormal3f(wek[n - 1][0].Nx, wek[n - 1][0].Ny, wek[n - 1][0].Nz);
		glTexCoord2f((n - 1)*step, 0);
		glVertex3f(tab[n - 1][0].x, tab[n - 1][0].y - 5, tab[n - 1][0].z);



		glColor4f(colors[0][0].r, colors[0][0].g, colors[0][0].b, colors[0][0].a);
		glNormal3f(-wek[0][0].Nx, -wek[0][0].Ny, -wek[0][0].Nz);
		glTexCoord2f(0, 0);
		glVertex3f(tab[0][0].x, tab[0][0].y - 5, tab[0][0].z);

		glColor4f(colors[n - 1][n - 1].r, colors[n - 1][n - 1].g, colors[n - 1][n - 1].b, colors[n - 1][n - 1].a);
		glNormal3f(wek[n - 1][n - 1].Nx, wek[n - 1][n - 1].Ny, wek[n - 1][n - 1].Nz);
		glTexCoord2f((n - 1)*step, (n - 1)*step);
		glVertex3f(tab[n - 1][n - 1].x, tab[n - 1][n - 1].y - 5, tab[n - 1][n - 1].z);

		glColor4f(colors[1][0].r, colors[1][0].g, colors[1][0].b, colors[1][0].a);
		glNormal3f(-wek[1][0].Nx, -wek[1][0].Ny, -wek[1][0].Nz);
		glTexCoord2f(1 * step, 0);
		glVertex3f(tab[1][0].x, tab[1][0].y - 5, tab[1][0].z);



		//po��czenie punkt�w ze skrajnych przedzia��w kt�re nie zosta�y wcze�niej ze sob� po��czone
		glEnd();

	}
	else if (model == 4)
	{
		glutSolidTeapot(3.0);//rysowanie czajnika
	}
	else if (model == 5)
	{
		triangle();
	}
}






/*************************************************************************************/
 // Funkcja wczytuje dane obrazu zapisanego w formacie TGA w pliku o nazwie
 // FileName, alokuje pamięć i zwraca wskaźnik (pBits) do bufora w którym
 // umieszczone są dane.
 // Ponadto udostępnia szerokość (ImWidth), wysokość (ImHeight) obrazu
 // tekstury oraz dane opisujące format obrazu według specyfikacji OpenGL
 // (ImComponents) i (ImFormat).
 // Jest to bardzo uproszczona wersja funkcji wczytującej dane z pliku TGA.
 // Działa tylko dla obrazów wykorzystujących 8, 24, or 32 bitowy kolor.
 // Nie obsługuje plików w formacie TGA kodowanych z kompresją RLE.
/*************************************************************************************/

GLbyte *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat)
{
	/*************************************************************************************/
	// Struktura dla nagłówka pliku  TGA
#pragma pack(1)           
	typedef struct
	{
		GLbyte    idlength;
		GLbyte    colormaptype;
		GLbyte    datatypecode;
		unsigned short    colormapstart;
		unsigned short    colormaplength;
		unsigned char     colormapdepth;
		unsigned short    x_orgin;
		unsigned short    y_orgin;
		unsigned short    width;
		unsigned short    height;
		GLbyte    bitsperpixel;
		GLbyte    descriptor;
	}TGAHEADER;
#pragma pack(8)

	FILE *pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte    *pbitsperpixel = NULL;
	/*************************************************************************************/
	// Wartości domyślne zwracane w przypadku błędu
	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;

	pFile = fopen(FileName, "rb");
	if (pFile == NULL)
		return NULL;
	/*************************************************************************************/
	// Przeczytanie nagłówka pliku 
	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);
	/*************************************************************************************/
	// Odczytanie szerokości, wysokości i głębi obrazu
	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;
	/*************************************************************************************/
	// Sprawdzenie, czy głębia spełnia założone warunki (8, 24, lub 32 bity)
	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;
	/*************************************************************************************/
	// Obliczenie rozmiaru bufora w pamięci
	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;
	/*************************************************************************************/
	// Alokacja pamięci dla danych obrazu
	pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));
	if (pbitsperpixel == NULL)
		return NULL;
	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
	{
		free(pbitsperpixel);
		return NULL;
	}
	/*************************************************************************************/
	// Ustawienie formatu OpenGL
	switch (sDepth)
	{
	case 3:
		*ImFormat = GL_BGR_EXT;
		*ImComponents = GL_RGB8;
		break;
	case 4:
		*ImFormat = GL_BGRA_EXT;
		*ImComponents = GL_RGBA8;
		break;
	case 1:
		*ImFormat = GL_LUMINANCE;
		*ImComponents = GL_LUMINANCE8;
		break;
	};
	fclose(pFile);
	return pbitsperpixel;
}

/*************************************************************************************/






void RenderScene(void)
{
	GLfloat viewoption = 1.0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Czyszczenie okna aktualnym kolorem czyszczacym
	glLoadIdentity();
	if (model != 4)	  //jesli wybrany model nie jest czajnikiem 
	{
		if (mode == 1)		//ruch zrodla swiatla
		{
			if (status == 1)	//wcisniety lewy przycisk myszy - obrot pierwszego zrodla swiatla
			{
				thetaLight1 += delta_x * pix2angle / czulosc;   // modyfikacja kąta obrotu o kat proporcjonalny
																//im wieksza wartosc czulosci tym mniejszy bedzie obrot
				betaLight1 += delta_y * pix2angle / czulosc;	// modyfikacja kąta obrotu o kat proporcjonalny

				if (betaLight1 > 2 * M_PI)  //beta musi byc w przedziale [0,2 PI]
					betaLight1 = 0;		//wartosc funkcji sin i cos bedzie taka sama w 0 jak i w 2 PI
				if (betaLight1 < 0)
					betaLight1 = 2 * M_PI;
			}
			else if (status == 2) // wcisniety prawy przycisk myszy - obrot drugiego zrodla
			{
				thetaLight2 += delta_x * pix2angle / czulosc;   // modyfikacja kąta obrotu o kat proporcjonalny
														 //im wieksza wartosc czulosci tym mniejszy bedzie obrot
				betaLight2 += delta_y * pix2angle / czulosc;	// modyfikacja kąta obrotu o kat proporcjonalny

				if (betaLight2 > 2 * M_PI)  //beta musi byc w przedziale [0,2 PI]
					betaLight2 = 0;		//wartosc funkcji sin i cos bedzie taka sama w 0 jak i w 2 PI
				if (betaLight2 < 0)
					betaLight2 = 2 * M_PI;
			}
		}
		else  //ruch obserwatora
		{
			if (status == 1)   // jeśli lewy klawisz myszy wciśnięty
			{
				theta += delta_x * pix2angle / czulosc;  // modyfikacja kąta obrotu o kat proporcjonalny
														 //im wieksza wartosc czulosci tym mniejszy bedzie obrot
				beta += delta_y * pix2angle / czulosc;	// modyfikacja kąta obrotu o kat proporcjonalny

				if (beta > 2 * M_PI)  //beta musi byc w przedziale [0,2 PI]
					beta = 0;		//wartosc funkcji sin i cos bedzie taka sama w 0 jak i w 2 PI
				if (beta < 0)
					beta = 2 * M_PI;
				if (beta > M_PI / 2 && beta < (1.5)*M_PI) //ze wzgledu na okresowowsc funkcji sin wystepuje problem z pelnym obrotem
					viewoption = -1.0;	//dlatego nalezy w okreslonym przypadku zmienic wartosc wektora kamery
				else
					viewoption = 1.0;
			}
			else if (status == 2) //wcisniety prawy przycisk myszy
			{
				R += delta_y * pix2angle / czulosc;  //ustalanie promienia (odleglosci od obiektu)
			}
			viewer[0] = R * cos(theta)*cos(beta);  //wpisanie pozycji obserwatora wedlug zadanych wzorow
			viewer[1] = R * sin(beta);
			viewer[2] = R * sin(theta)*cos(beta);
			glColor3f(1.0f, 1.0f, 1.0f); // Ustawienie koloru rysowania na bialy

		}
	}
	else    // czajnik
	{
		if (mode == 1) //ruch zrodla swiatla
		{
			if (status == 1) //wcisniety lewy przycisk myszy
			{
				thetaLight1 += delta_x * pix2angle / czulosc; // modyfikacja kąta obrotu o kat proporcjonalny
														 //im wieksza wartosc czulosci tym mniejszy bedzie obrot
				betaLight1 += delta_y * pix2angle / czulosc; // modyfikacja kąta obrotu o kat proporcjonalny

				if (betaLight1 > 2 * M_PI)  //beta musi byc w przedziale [0,2 PI]
					betaLight1 = 0;		//wartosc funkcji sin i cos bedzie taka sama w 0 jak i w 2 PI
				if (betaLight1 < 0)
					betaLight1 = 2 * M_PI;
			}
			else if (status == 2) //wcisniety prawy przycisk myszy
			{
				thetaLight2 += delta_x * pix2angle / czulosc; // modyfikacja kąta obrotu o kat proporcjonalny
														 //im wieksza wartosc czulosci tym mniejszy bedzie obrot
				betaLight2 += delta_y * pix2angle / czulosc; // modyfikacja kąta obrotu o kat proporcjonalny

				if (betaLight2 > 2 * M_PI)  //beta musi byc w przedziale [0,2 PI]
					betaLight2 = 0;		//wartosc funkcji sin i cos bedzie taka sama w 0 jak i w 2 PI
				if (betaLight2 < 0)
					betaLight2 = 2 * M_PI;
			}
		}
		else //obrot czajnika
		{
			if (status == 1)  // jesli lewy klawisz myszy wcisniety
			{
				theta += delta_x * pix2angle;  // modyfikacja kąta obrotu o kat proporcjonalny
				beta += delta_y * pix2angle;  // do ronzicy polozenia kursora myszy

			}
			else if (status == 2)
			{
				zoom += delta_y * pix2angle / czulosc;
			}
			viewer[2] = zoom;  //ustalanie zoomu			
		}
	}
	light1[0] = R * cos(thetaLight1)*cos(betaLight1);  //wpisanie pozycji zrodla 1 wedlug zadanych wzorow
	light1[1] = R * sin(betaLight1);
	light1[2] = R * sin(thetaLight1)*cos(betaLight1);
	light2[0] = R * cos(thetaLight2)*cos(betaLight2);  //wpisanie pozycji zrodla 2 wedlug zadanych wzorow
	light2[1] = R * sin(betaLight2);
	light2[2] = R * sin(thetaLight2)*cos(betaLight2);
	GLfloat light_position2[] = { light2[0], light2[1], light2[2], 1.0 }; //nowa pozycja zrodla swiatla light 0
	GLfloat light_position1[] = { light1[0], light1[1], light1[2], 1.0 }; //nowa pozycja zrodla swiatla light 1
	glLightfv(GL_LIGHT0, GL_POSITION, light_position1); //zaktualizowanie pozycji zrodla swiatla
	glLightfv(GL_LIGHT1, GL_POSITION, light_position2); //zaktualizowanie pozycji zrodla swiatla
	if (model != 4) //gdy rysowany obiekt nie jest czajnikiem 
	{
		gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, viewoption, 0.0);  // pozycje obserwatora 
	}
	else //gdy obiekt jest czajnikiem
	{
		gluLookAt(0.0, 0.0, viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		glRotatef(theta, 0.0, 1.0, 0.0);  //obrot obiektu o nowy kat
		glRotatef(beta, 1.0, 0.0, 0.0);  //obrot obiektu o nowy kat
	}
	Axes();

	Egg();
	glColor3f(1.0f, 1.0f, 1.0f);

	glFlush();
	glutSwapBuffers();
}

/*************************************************************************************/

// Funkcja ustalająca stan renderowania

void MyInit(void)
{
	/*************************************************************************************/

// Zmienne dla obrazu tekstury
	GLbyte *pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;

	/*************************************************************************************/

	//Wlaczenie teksturowania tyko po jednej stronie ściany
	glEnable(GL_CULL_FACE);


	/*************************************************************************************/

	//  Przeczytanie obrazu tekstury z pliku o nazwie tekstura.tga
	pBytes = LoadTGAImage("D7_t.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);


	/*************************************************************************************/

   // Zdefiniowanie tekstury 2-D
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);

	/*************************************************************************************/

	// Zwolnienie pamięci
	free(pBytes);

	/*************************************************************************************/

	// Włączenie mechanizmu teksturowania
	glEnable(GL_TEXTURE_2D);

	/*************************************************************************************/

	// Ustalenie trybu teksturowania
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/*************************************************************************************/

	// Określenie sposobu nakładania tekstur
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);




	/*************************************************************************************/



	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszc�cy (wype�nienia okna) ustawiono na czarny
	/*************************************************************************************/

//  Definicja materiału z jakiego zrobiony jest czajnik
//  i definicja źródła światła

/*************************************************************************************/


/*************************************************************************************/
// Definicja materiału z jakiego zrobiony jest obiekt

	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	// współczynniki ka =[kar,kag,kab] dla światła otoczenia

	GLfloat mat_diffuse[] = { 1.0,1.0,1.0,1.0 };
	// współczynniki kd =[kdr,kdg,kdb] światła rozproszonego

	GLfloat mat_specular[] = { 0.1, 0.1, 0.1, 1.0 };
	// współczynniki ks =[ksr,ksg,ksb] dla światła odbitego               

	GLfloat mat_shininess = { 20.0 };
	// współczynnik n opisujący połysk powierzchni


/*************************************************************************************/
// Definicja źródła światła


	GLfloat light_position1[] = { 10.0, 0.0, 0.0, 1.0 };
	// położenie źródła


	GLfloat light_ambient1[] = { 0.1, 0.1, 0.1, 1.0 };
	// składowe intensywności świecenia źródła światła otoczenia
	// Ia = [Iar,Iag,Iab]

	GLfloat light_diffuse1[] = { 1.0, 1.0, 1.0, 1.0 };
	// składowe intensywności świecenia źródła światła powodującego
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

	GLfloat light_specular1[] = { 1.0, 1.0, 1.0, 1.0 };
	// składowe intensywności świecenia źródła światła powodującego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]

	GLfloat att_constant1 = { 1.0 };
	// składowa stała ds dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

	GLfloat att_linear1 = { 0.05 };
	// składowa liniowa dl dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

	GLfloat att_quadratic1 = { 0.001 };
	// składowa kwadratowa dq dla modelu zmian oświetlenia w funkcji
	// odległości od źródła


	/**************************************************************************/
	//drugie swiatlo

	GLfloat light_position2[] = { 0.0, 0.0, 10.0, 1.0 };
	// położenie źródła


	GLfloat light_ambient2[] = { 0.1, 0.1, 0.1, 1.0 };
	// składowe intensywności świecenia źródła światła otoczenia
	// Ia = [Iar,Iag,Iab]

	GLfloat light_diffuse2[] = { 0.0, 1.0, 0.0, 1.0 };
	// składowe intensywności świecenia źródła światła powodującego
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

	GLfloat light_specular2[] = { 0.0, 1.0, 0.0, 1.0 };
	// składowe intensywności świecenia źródła światła powodującego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]

	GLfloat att_constant2 = { 1.0 };
	// składowa stała ds dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

	GLfloat att_linear2 = { 0.05 };
	// składowa liniowa dl dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

	GLfloat att_quadratic2 = { 0.001 };
	// składowa kwadratowa dq dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

/*************************************************************************************/
// Ustawienie parametrów materiału i źródła światła

/*************************************************************************************/
// Ustawienie patrametrów materiału


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


	/*************************************************************************************/
	// Ustawienie parametrów źródła

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular1);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant1);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear1);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic1);

	/*************************************************************************************/
	//drugie źródło


	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular2);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant2);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear2);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic2);


	/*************************************************************************************/
	// Ustawienie opcji systemu oświetlania sceny

	glShadeModel(GL_SMOOTH); // właczenie łagodnego cieniowania
	glEnable(GL_LIGHTING);   // właczenie systemu oświetlenia sceny
	glEnable(GL_LIGHT0);     // włączenie źródła o numerze 0
	//glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora

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
	} while (czulosc <= 0);
}

void instrukcja()
{
	std::cout << std::endl << std::endl << "************** INSTRUKCJA **************" << std::endl << std::endl;
	std::cout << "o - wyswietlenie ostroslupa" << std::endl;
	std::cout << "0 - wyswietlenie calego ostroslupa" << std::endl;
	std::cout << "1 - wyswietlenie piewszej sciany ostroslupa" << std::endl;
	std::cout << "2 - wyswietlenie drugiej sciany ostroslupa" << std::endl;
	std::cout << "3 - wyswietlenie trzeciej sciany ostroslupa" << std::endl;
	std::cout << "4 - wyswietlenie czwartej sciany ostroslupa" << std::endl;
	std::cout << "5 - wyswietlenie podstawy ostroslupa" << std::endl << std::endl;
	std::cout << "*** Obracac jajko mozna przy wcisnietym LPM, natomiast przyblizac i oddalac je przy wcisnietym PPM ***" << std::endl;
	std::cout << "p - wyswietlenie jajka w postaci chmury punktow" << std::endl;
	std::cout << "w - wyswietlenie jajka w postaci modelu siatki" << std::endl;
	std::cout << "s - wyswietlenie jajka w postaci modelu 3D" << std::endl << std::endl;
	std::cout << "*** Obracac czajnik mozna przy wcisnietym LPM, natomiast przyblizac i oddalac go przy wcisnietym PPM ***" << std::endl;
	std::cout << "c - wyswietlenie czajnika" << std::endl << std::endl << std::endl;
	std::cout << "***" << std::endl;
	std::cout << "m - zmiana czulosci myszy" << std::endl;
	std::cout << "e - wyjscie z programu" << std::endl;
}

void keys(unsigned char key, int x, int y)
{
	if (key == 'p') model = 1; mode = 0;
	if (key == 'w') model = 2; mode = 0;
	if (key == 's') model = 3; mode = 0;
	if (key == 'c') model = 4; mode = 0;
	if (key == 'o') model = 5; mode = 0; numer = 0;
	if (key == '0') numer = 0;
	if (key == '1') numer = 1;
	if (key == '2') numer = 2;
	if (key == '3') numer = 3;
	if (key == '4') numer = 4;
	if (key == '5') numer = 5;

	if (key == 'm') ustawienie();
	if (key == 'i') instrukcja();
	if (key == 'e') exit(0);
	RenderScene(); // przerysowanie obrazu sceny
}


void main(void)
{
	std::cout << "*** i - Instrukcja ***" << std::endl << "p - punkty, w - siatka, s - wypelnione trojkaty, o- ostroslup" << std::endl << "0 - caly ostroslup, " << "1-4 wyswietlenie kolejnych scian, " << "5 - wyswietlenie podstawy" << std::endl << "m - zmien czulosc myszy, e - wyjscie z programu" << std::endl;
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
	glutMouseFunc(Mouse);
	MyInit();
	// Funkcja MyInit() (zdefiniowana powy�ej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przyst�pieniem do renderowania
	glEnable(GL_DEPTH_TEST);
	// Wlaczenie mechanizmu usuwania powierzchni niewidocznych
	glutKeyboardFunc(keys);

	// Ustala funkcję zwrotną odpowiedzialną za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcję zwrotną odpowiedzialną za badanie ruchu myszy
	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}

/*************************************************************************************/



