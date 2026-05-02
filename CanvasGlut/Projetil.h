#ifndef __PROJETIL_H__
#define __PROJETIL_H__

#include "Vector2.h"
#include "gl_canvas2d.h"

class Projetil {
public:
    float x, y;
    float width, height;
    float velocidade;
    float t;
    float passoT;
    Vector2 direcaoMovimento;
    bool ativo;
    Vector2 pts[4];

    bool curva;
    Projetil(float _x, float _y, Vector2 _direcao, float _velocidade, bool _ehcurva) {
        x = _x;
        y = _y;
        width = 4.0f;
        height = 12.0f;
        direcaoMovimento = _direcao;
        direcaoMovimento.normalize();
        velocidade = _velocidade;
        ativo = true;

        t = 0;
        passoT = velocidade / 500;

        curva = _ehcurva;
        if (curva) {
            pts[0] = Vector2(x, y);
            pts[1] = Vector2(x - 200, y + 200);
            pts[2] = Vector2(x + 200, y + 300);
            pts[3] = Vector2(x - 100, y + 500);
        }

    }

    void bezier(float dt) {
        if (!ativo) return;

        Vector2 old = Vector2(x, y);

        t += passoT * dt;
        if (t > 1.0f) {
            ativo = false;
            return;
        }

        float t0 = (1 - t) * (1 - t) * (1 - t);
        float t1 = 3 * t * (1 - t) * (1 - t);
        float t2 = 3 * t * t * (1 - t);
        float t3 = t * t * t;
        
        Vector2 ponto = (pts[0] * t0) + (pts[1] * t1) + (pts[2] * t2) + (pts[3] * t3);
        Vector2 novaDirecao = ponto - old;
        
        if (novaDirecao.magnitude() > 0) {
            direcaoMovimento = novaDirecao;
            direcaoMovimento.normalize();
        }
    }


    void atualizar(float dt) {
        if (curva) {
            bezier(dt);
        }
        direcaoMovimento.moverPonto(x, y, velocidade, dt);
    }

    void desenhar(int cor) {

        if(curva)
            CV::color(13);
        else {
            CV::color(cor);
        }
       

        float comprimento = height;

        float x2 = x + direcaoMovimento.x * comprimento;
        float y2 = y + direcaoMovimento.y * comprimento;

        CV::line(x, y, x2, y2);
    }
};

#endif