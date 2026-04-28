#ifndef __JOGADOR_H__
#define __JOGADOR_H__

#include <vector>
#include "Vector2.h"
#include "Projetil.h"
#include "gl_canvas2d.h"
class Jogador {
public:
    float x, y;
    float width, height;
    float velocidade;
    Vector2 direcaoMovimento;

    // 3 Vetores de direção para os canhões
    Vector2 canhaoEsq;
    Vector2 canhaoCent;
    Vector2 canhaoDir;

    std::vector<Projetil> disparos;
    bool ativo;

    Jogador() {
        width = 40.0f;
        height = 40.0f;
        velocidade = 300.0f; // 300 pixels por segundo
        ativo = true;

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

    void atirar() {
        // Dispara 3 tiros simultâneos usando as direções dos canhões
        float centroX = x + (width / 2.0f) - 2.0f; // Centraliza a bala
        float topoY = y + height;

        disparos.push_back(Projetil(centroX, topoY, canhaoEsq, 500.0f));
        disparos.push_back(Projetil(centroX, topoY, canhaoCent, 600.0f));
        disparos.push_back(Projetil(centroX, topoY, canhaoDir, 500.0f));
    }

    void atualizar(float dt, float screenWidth, float alturaCenario) {
        if (!ativo) return;

        // Move o jogador usando o Vector2
        direcaoMovimento.moverPonto(x, y, velocidade, dt);

        // Limita o jogador dentro da tela visível
        if (x < 0) x = 0;
        if (x + width > screenWidth) x = screenWidth - width;

        // Atualiza os disparos
        for (int i = 0; i < disparos.size(); i++) {
            disparos[i].atualizar(dt);

            // Remove o tiro se sair da tela ou do cenário (limite Y superior)
            if (disparos[i].y > alturaCenario || disparos[i].x < 0 || disparos[i].x > screenWidth || !disparos[i].ativo) {
                disparos.erase(disparos.begin() + i);
                i--; // Ajusta o índice após a remoção
            }
        }
    }

    void desenhar() {
        if (!ativo) return;

        // Desenha os disparos
        for (int i = 0; i < disparos.size(); i++) {
            disparos[i].desenhar(2); // Vermelho
        }

        // Desenha o Jogador (Retângulo Azul)
        CV::color(4);
        CV::rectFill(x, y, x + width, y + height);
    }
};

#endif