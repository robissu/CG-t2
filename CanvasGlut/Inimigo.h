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
    float width, height, raio;
    float velocidade;
    float cronometroTiro = 0;
    float tempoRecarga = 1.5f;

    Vector2 direcaoMovimento;
    Vector2 canhao; // 1 vetor de disparo

    bool ativo;

    Inimigo() {
        width = 30.0f;
        height = 30.0f;
        raio = 15.0f;
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

    bool checaColisaoCircular(float pontoX, float pontoY) {
        // Calcula o centro do círculo (assumindo que x,y é o canto inferior esquerdo)
        float centroX = x + raio;
        float centroY = y + raio;

        float distSq = (centroX - pontoX) * (centroX - pontoX) + (centroY - pontoY) * (centroY - pontoY);
        return distSq < (raio * raio); // Colisão com um ponto (tiro)
    }
    void atirar(std::vector<Projetil>& disparosJogadorInimigos) {
        float centroX = x + (width / 2.0f) - 2.0f;
        float baseY = y;
        disparosJogadorInimigos.push_back(Projetil(centroX, baseY, canhao, 150.0f, false));
    }

    void atualizar(float dt, float screenWidth, float alturaCenario, std::vector<Projetil>& disparosInimigos) {
        if (!ativo) return;

        direcaoMovimento.moverPonto(x, y, velocidade, dt);

        cronometroTiro += dt;
        if (cronometroTiro >= tempoRecarga && (y + height) < (alturaCenario/10)) {
            atirar(disparosInimigos);
            cronometroTiro = 0;     
        }
    }

    void desenhar() {
        if (!ativo) return;

        // Desenha inimigo (Retângulo Vermelho Escuro)
        //CV::color(9);
        //CV::rectFill(x, y, x + width, y + height);
    }
};

#endif