#ifndef __INIMIGO_H__
#define __INIMIGO_H__

#include <vector>
#include <stdlib.h>
#include "Vector2.h"
#include "Projetil.h"
#include "gl_canvas2d.h"

class Inimigo {
public:
    float x, y;
    float width, height;
    float velocidade;
    float cronometroTiro = 0;
    float tempoRecarga = 1.5f;
    Vector2 direcaoMovimento;
    Vector2 canhao; // 1 vetor de disparo

    std::vector<Projetil> disparos;
    bool ativo;

    Inimigo() {
        width = 30.0f;
        height = 30.0f;
        velocidade = 30.0f; // Mais lento que o jogador
        ativo = true;

        // Move-se para baixo (-y)
        direcaoMovimento.set(0.0f, -1.0f);

        // Tiro vai para baixo (-y)
        float rad270 = 270.0f * (PI / 180.0F);
        canhao.set(cosf(rad270), sinf(rad270));
        canhao.normalize();
    }

    bool checaColisao(float oX, float oY, float oWidth, float oHeight) {
        return (x < oX + oWidth &&
            x + width > oX &&
            y < oY + oHeight &&
            y + height > oY);
    }

    void atirar() {
        float centroX = x + (width / 2.0f) - 2.0f;
        float baseY = y;
        disparos.push_back(Projetil(centroX, baseY, canhao, 150.0f));
    }

    void atualizar(float dt, float screenWidth, float alturaCenario) {
        if (!ativo) return;

        direcaoMovimento.moverPonto(x, y, velocidade, dt);

        cronometroTiro += dt;
        if (cronometroTiro >= tempoRecarga && (y + height) < (alturaCenario/10)) {
            atirar();               
            cronometroTiro = 0;     
        }

        // Atualiza disparos
        for (int i = 0; i < disparos.size(); i++) {
            disparos[i].atualizar(dt);

            // Remove se saiu por baixo da tela
            if (disparos[i].y < 0 || !disparos[i].ativo) {
                disparos.erase(disparos.begin() + i);
                i--;
            }
        }
    }

    void desenhar() {
        if (!ativo) return;

        // Desenha tiros
        for (int i = 0; i < disparos.size(); i++) {
            disparos[i].desenhar(2); // Vermelho
        }

        // Desenha inimigo (Retângulo Vermelho Escuro)
        CV::color(9);
        CV::rectFill(x, y, x + width, y + height);
    }
};

#endif