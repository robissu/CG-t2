#ifndef __PROJETIL_H__
#define __PROJETIL_H__

#include "Vector2.h"
#include "gl_canvas2d.h"

class Projetil {
public:
    float x, y;
    float width, height;
    float velocidade;
    Vector2 direcaoMovimento;
    bool ativo;

    Projetil(float _x, float _y, Vector2 _direcao, float _velocidade) {
        x = _x;
        y = _y;
        width = 4.0f;
        height = 12.0f;
        direcaoMovimento = _direcao;
        direcaoMovimento.normalize();
        velocidade = _velocidade;
        ativo = true;
    }

    void atualizar(float dt) {
        direcaoMovimento.moverPonto(x, y, velocidade, dt);
    }

    void desenhar(int cor) {
        CV::color(cor);

        float comprimento = height;

        float x2 = x + direcaoMovimento.x * comprimento;
        float y2 = y + direcaoMovimento.y * comprimento;

        CV::line(x, y, x2, y2);
    }
};

#endif