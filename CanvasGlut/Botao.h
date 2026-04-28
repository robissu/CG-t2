#ifndef __BOTAO_H__
#define __BOTAO_H__

#include "gl_canvas2d.h"

#define _CRT_SECURE_NO_WARNINGS

#pragma warning(disable:4996)

class Botao{
  float altura, largura, x, y;
  char label[100];
  int cor, tipo;
  bool press;
public:
  Botao(float _x, float _y, float _larg, float _alt, const char *_label, int _cor)
  {
     altura  = _alt;
     largura = _larg;
     x = _x;
     y = _y;
     cor = _cor;
     strcpy(label, _label);
     press = false;
     tipo = 0;
  }
  Botao(float _x, float _y, float _larg, float _alt, const char* _label)
  {
      altura = _alt;
      largura = _larg;
      x = _x;
      y = _y;
      strcpy(label, _label);
      press = false;
      tipo = 1;
  }

  void setPos(float novoX, float novoY) {
      this->x = novoX;
      this->y = novoY;
  }

  const char* getLabel() {
      return label; 
  }

  bool getPress() {
      return press;
  }

  void alterna() {
      press = !press;
  }

  void Render(){
      if (tipo) {
          if (press) {
              CV::color(0);
              CV::line(x, y, x + largura, y + altura);
              CV::line(x + largura, y, x, y + altura);
          }
          CV::color(0);
          CV::rect(this->x, this->y, this->x + this->largura, this->y + this->altura);
          CV::color(0, 0, 0);
          CV::text(x - 8, y + altura + 5, label); 
      }
      else {
          CV::color(cor);
          CV::rectFill(x, y, x + largura, y + altura);
          CV::color(1, 1, 1);
          CV::text(x + 5, y + altura / 2, label);
      }
      
  }

  //recebe as coordenadas do mouse para tratar clique ou detectar quando o mouse esta em cima do botao
  bool hitClick(int mx, int my)
  {
      if( mx >= x && mx <= (x + largura) && my >= y && my <= (y + altura) )
      {
          return true;
      }
      return false;
  }
};

#endif
