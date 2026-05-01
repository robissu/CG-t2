#ifndef __JOGADOR_H__
#define __JOGADOR_H__

#include <vector>
#include "Vector2.h"
#include "Projetil.h"
#include "gl_canvas2d.h"
class Jogador {
public:
    float x, y;
    float width, height, raio;
    float velocidade;
    float tempoRecarga = 0.2f;  
    float cronometroTiro = 0.0f;
    bool atirando;
    Vector2 direcaoMovimento;

    // 3 Vetores de direção para os canhões
    Vector2 canhaoEsq;
    Vector2 canhaoCent;
    Vector2 canhaoDir;

    bool ativo;

    Jogador() {
        width = 40.0f;
        height = 40.0f;
        raio = 20.0f;
        velocidade = 300.0f; // 300 pixels por segundo
        ativo = true;
        atirando = false;
        direcaoMovimento.set(0, 0); // Fica parado inicialmente
        
        // Configura os ângulos dos 3 canhões 
        float rad90 = 90.0f * (PI / 180.0F);
        canhaoCent.set(cosf(rad90), sinf(rad90));
        canhaoCent.normalize();

        float rad65 = 65.0f * (PI / 180.0F);
        canhaoDir.set(cosf(rad65), sinf(rad65));
        canhaoDir.normalize();

        float rad115 = 115.0f * (PI / 180.0F);
        canhaoEsq.set(cosf(rad115), sinf(rad115));
        canhaoEsq.normalize();
    }

    // Colisão AABB (Axis-Aligned Bounding Box)
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

    bool colideCom(float outroX, float outroY, float outroRaio) {
        float c1X = x + raio;
        float c1Y = y + raio;
        float c2X = outroX + outroRaio;
        float c2Y = outroY + outroRaio;

        float distSq = (c1X - c2X) * (c1X - c2X) + (c1Y - c2Y) * (c1Y - c2Y);
        float somaRaios = raio + outroRaio;
        return distSq < (somaRaios * somaRaios);
    }


    void atirar(std::vector<Projetil>& disparosJogador) {
        // Dispara 3 tiros simultâneos usando as direções dos canhões
        float centroX = x + (width / 2.0f);
        float topoY = y + height;

        disparosJogador.push_back(Projetil(centroX, topoY, canhaoEsq, 500.0f, (rand()% 100 < 20)));
        disparosJogador.push_back(Projetil(centroX, topoY, canhaoCent, 500.0f, false));
        disparosJogador.push_back(Projetil(centroX, topoY, canhaoDir, 500.0f, false));
    }

    void atualizar(float dt, float screenWidth, float alturaCenario, std::vector<Projetil>& disparosJogador) {
        if (!ativo) return;

        // Move o jogador usando o Vector2
        direcaoMovimento.moverPonto(x, y, velocidade, dt);

        // Limita o jogador dentro da tela visível
        if (x < 0) x = 0;
        if (x + width > screenWidth) x = screenWidth - width;

        if (cronometroTiro < tempoRecarga) {
            cronometroTiro += dt;
        }

        if (atirando && cronometroTiro >= tempoRecarga) {
            atirar(disparosJogador);               // Chama sua função que adiciona os 3 tiros no vector
            cronometroTiro = 0.0f;                 // Reseta o cronômetro
        }
    }

    void desenhar() {
        if (!ativo) return;

        // Desenha o Jogador (Retângulo Azul)
        //CV::color(4);
        //CV::rectFill(x, y, x + width, y + height);

        float tamanhoCanhao = 20.0f; // Comprimento visual da linha do canhão
        CV::color(1); // Escolha uma cor (ex: branco ou cinza)

        float origemX = x + (width / 2.0f);
        float origemY = y + height;

        // Canhão Central
        CV::line(origemX, origemY,
            origemX + canhaoCent.x * tamanhoCanhao,
            origemY + canhaoCent.y * tamanhoCanhao);

        // Canhão Direito 
        CV::line(origemX, origemY,
            origemX + canhaoDir.x * tamanhoCanhao,
            origemY + canhaoDir.y * tamanhoCanhao);

        // Canhão Esquerdo
        CV::line(origemX, origemY,
            origemX + canhaoEsq.x * tamanhoCanhao,
            origemY + canhaoEsq.y * tamanhoCanhao);
    }
};

#endif