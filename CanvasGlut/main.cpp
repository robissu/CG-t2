/*********************************************************************
// Canvas para desenho, criada sobre a API OpenGL. Nao eh necessario conhecimentos de OpenGL para usar.
//  Autor: Cesar Tadeu Pozzer
//         08/2025
//
//  Pode ser utilizada para fazer desenhos, animacoes, e jogos simples.
//  Tem tratamento de mouse e teclado
//  Estude o OpenGL antes de tentar compreender o arquivo gl_canvas.cpp
//
//  Versao 2.1
//
//  Instruções:
//	  Para alterar a animacao, digite numeros entre 1 e 3
//    Programa cheio de numeros magicos. Nao use isso nunca.
// *********************************************************************/

#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "Jogo.h"
#include "gl_canvas2d.h"
#include <ctime>


#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

//largura e altura inicial da tela . Alteram com o redimensionamento de tela.
int screenWidth = 600, screenHeight = 600;


int opcao  = 50;//variavel global para selecao do que sera exibido na canvas.
int mouseX, mouseY; //variaveis globais do mouse para poder exibir dentro da render().
int raio = 60;
float passo = 2 * PI / 12;
int direcaoTeclado = -1;
bool movimento = false;
Jogo jogo;
//funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis globais
//Todos os comandos para desenho na canvas devem ser chamados dentro da render().
//Deve-se manter essa função com poucas linhas de codigo.

Vector2 pts[4];
void setInicio() {
	jogo.inicializar(screenWidth, screenHeight);
}


void desenhaCart() {
	
	CV::color(2);
	for (int i = 0; i < screenWidth; i+=100) {
		CV::line(i, 0, i, screenHeight);
	}
	CV::color(2);
	for (int i = 0; i < screenHeight; i += 100) {
		CV::line(0, i, screenWidth, i);
	}
}

void desenhaQuadrado(int color) {
	CV::color(color);
	for (int i = 0; i < 4; i++) {
		CV::line(pts[i], pts[(i + 1) % 4]);
	}
}

void translateQuad(float _x, float _y) {

	for (int i = 0; i < 4; i++) {
		pts[i].set(pts[i].x + _x, pts[i].y + _y);
	}
}

void rotacionaQuad(float ang) {
	for (int i = 0; i < 4; i++) {

		float xRot = pts[i].x * cosf(ang) - pts[i].y * sinf(ang);
		float yRot = pts[i].x * sinf(ang) + pts[i].y * cosf(ang);

		pts[i].set(xRot, yRot);
	}
}

void escalar(int esc) {
	for (int i = 0; i < 4; i++) {
		pts[i].set(pts[i].x * esc, pts[i].y * esc);
	}
}

void maquita() {
	for (float ang = 0; ang < 2 * PI; ang += passo) {
		float x1 = raio * cosf(ang);
		float y1 = raio * sinf(ang);
		raio -= 10;
		float x2 = raio * cosf(ang + passo);
		float y2 = raio * sinf(ang + passo);
		raio += 10;
		CV::color(4);
		CV::line(x1, y1, x2, y2);
		raio -= 10;
		x2 = raio * cosf(ang);
		y2 = raio * sinf(ang);
		CV::line(x1, y1, x2, y2);
		raio += 10;
	}
}

float angP = 0;
void relogio() {
	for (float ang = 0; ang < 2 * PI; ang += 0.01) {
		float x = raio * cosf(ang);
		float y = raio * sinf(ang);

		CV::color(4);
		CV::point(x, y);
	}

	float xP = raio * cosf(angP);
	float yP = raio * sinf(angP);
	if (angP>=(2*PI)) {
		angP = 0;
	}
	CV::line(0,0, xP,yP);
	angP += 0.01;
}

void testAngulo() {
	Vector2 teste(mouseX - screenWidth*0.5, mouseY - screenHeight*0.5);
	Vector2 origemX(0, 1);
	teste.normalize();
	int tamanho = 50;
	CV::color(0);
	CV::line(origemX, teste * tamanho);
	float angulo = teste.angulo();
	CV::color(0);
	char buffer[20];
	snprintf(buffer, sizeof(buffer), "O angulo eh: %.2f", angulo);
	CV::text(0, 100, buffer);


}

void questaop1() {
	desenhaQuadrado(1);

	translateQuad(-100, -100);
	desenhaQuadrado(2);

	rotacionaQuad(45);
	desenhaQuadrado(3);
	rotacionaQuad(-45);

	translateQuad(100, 100);
	desenhaQuadrado(4);

	translateQuad(-100, -100);
	escalar(3);
	translateQuad(100, 100);
	desenhaQuadrado(5);
}

void drawBlending() {
	float x = 0;
	for (float t=0; t < 1; t += 0.01) {
		float y = (1 - t) * (1 - t) * (1-t);
		y = y * 200;
		x = x + 1;
		CV::color(0);
		CV::point(x, y);

		float y2 = 3 * t * (1 - t) * (1 - t);
		y2 = y2 * 200;
		CV::color(0);
		CV::point(x, y2);

		float y3 = 3 * t * t * (1 - t);
		y3 = y3 * 200;
		CV::color(0);
		CV::point(x, y3);

		float y4 = t * t * t;
		y4 = y4 * 200;
		CV::color(0);
		CV::point(x, y4);
	}
}
Vector2 arr[4];

void drawControlPoints() {
	CV::color(0);
	for (int i = 0; i < 4; i++) {
		CV::circle(arr[i].x, arr[i].y, 5, 5);
		if(i!=3)
			CV::line(arr[i], arr[i + 1]);
	}
}

void drawBezier() {

	CV::color(0);
	for (float t = 0; t < 1; t += 0.001) {
		float t0 = (1 - t) * (1 - t) * (1 - t);
		float t1 = 3 * t * (1 - t) * (1 - t);
		float t2 = 3 * t * t * (1 - t);
		float t3 = t * t * t;

		Vector2 ponto = (arr[0] * t0) + (arr[1] * t1) + (arr[2] * t2) + (arr[3] * t3);
		CV::point(ponto);
	}

}

void bSpline() {
	//blending f
	float x = 0;
	for (float t = 0; t < 1; t += 0.01) {
		float y1 = 1.0f/6.0f * (1 - t) * (1 - t) * (1 - t);
		y1 = y1 * 200;
		x = x + 1;
		CV::color(0);
		CV::point(x, y1);

		float y2 = 1.0f / 6.0f * ((3 * t * t * t) - (6 * t * t) + 4);
		y2 = y2 * 200;
		CV::color(0);
		CV::point(x, y2);

		float y3 = 1.0f / 6.0f * ((- 3 * t * t * t) + (3*t*t) + (3*t) + 1);
		y3 = y3 * 200;
		CV::color(0);
		CV::point(x, y3);

		float y4 = 1.0f / 6.0f *(t * t * t);
		y4 = y4 * 200;
		CV::color(0);
		CV::point(x, y4);
	}
}

void drawSpline() {

	CV::color(0);
	for (float t = 0; t < 1; t += 0.001) {
		float t0 = 1.0f / 6.0f * (1 - t) * (1 - t) * (1 - t);
		float t1 = 1.0f / 6.0f * ((3 * t * t * t) - (6 * t * t) + 4);
		float t2 = 1.0f / 6.0f * ((-3 * t * t * t) + (3 * t * t) + (3 * t) + 1);
		float t3 = 1.0f / 6.0f * (t * t * t);

		Vector2 ponto = (arr[0] * t0) + (arr[1] * t1) + (arr[2] * t2) + (arr[3] * t3);
		CV::point(ponto);
	}

}

void render()
{
	CV::clear(0, 0, 0);
	if (movimento) {
		if (direcaoTeclado == 0) jogo.jogador.direcaoMovimento.set(-1, 0); // Esq
		if (direcaoTeclado == 2) jogo.jogador.direcaoMovimento.set(1, 0);  // Dir
	}
	else {
		jogo.jogador.direcaoMovimento.set(0, 0); // Para o jogador
	}

	jogo.executarLoop();





	//CV::translate(screenWidth / 2, screenHeight / 2);
	//drawBlending(); 
	//drawControlPoints();
	//drawBezier();

	//bSpline();
	//drawControlPoints();
	//drawSpline();


	//desenhaCart();
	//maquita();
	//relogio();
	//testAngulo();
	//pts[0] = Vector2(100, 100);
	//pts[1] = Vector2(200, 100);
	//pts[2] = Vector2(200, 200);
	//pts[3] = Vector2(100, 200);
	//questaop1();
	

    //Sleep(10); //nao eh controle de FPS. Somente um limitador de FPS.
}

//funcao chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
   //printf("\nTecla: %d" , key);
   if( key < 200 )
   {
      opcao = key;
   }
   switch (key) {
   case 27://Esq
	   exit(0);
	   break;
   case 200://esquerda
	   direcaoTeclado = 0;
	   movimento = true;
	   break;
   case 201://cima
	   direcaoTeclado = 1;
	   movimento = true;
	   break;
   case 202://direita
	   direcaoTeclado = 2;
	   movimento = true;
	   break;
   case 203://baixo
	   direcaoTeclado = 3;
	   movimento = true;
	   break;
   case 114://r = reinicio
	   if (jogo.gameOver) {
		  jogo.inicializar(screenWidth, screenHeight);
	   }
			
   default:
	   direcaoTeclado = -1;
	   break;
   }

   if (key == 32) { // ESPAÇO
	   jogo.jogador.atirando = true;
   }
}

//funcao chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
	if (key >= 200 && key < 204) {
		movimento = false;
	}
	if (key == 32) {
		jogo.jogador.atirando = false; // Desliga o gatilho
	}
   //printf("\nLiberou: %d" , key);
}

//funcao para tratamento de mouse: cliques, movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{
   mouseX = x; //guarda as coordenadas do mouse para exibir dentro da render()
   mouseY = y;
   //arr[1].set(mouseX, mouseY);
   //printf("\nmouse %d %d %d %d %d %d", button, state, wheel, direction,  x, y);

   if( state == 0 ) //clicou
   {
   }
   if (state == 1) {

   }
}

int main(void)
{
	//arr[0] = Vector2(100, 100);
	//arr[1] = Vector2(200, 500);
	//arr[2] = Vector2(300, 200);
	//arr[3] = Vector2(400, 300);
	srand(time(NULL));
    CV::init(&screenWidth, &screenHeight, "Trabalho 2 - Robson Daniel Marchesan");
	setInicio();
    CV::run();

  
}
