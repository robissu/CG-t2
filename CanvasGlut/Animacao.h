#ifndef __ANIMACAO_H__
#define __ANIMACAO_H__

#include "Vector2.h"
#include "gl_canvas2d.h"
#include <vector>
#include <cstdlib>

// estrutura que representa cada fagulha
struct Particula {
    Vector2 pos;
    Vector2 vel;
    float vida;    // Vai de 1.0 (nova) até 0.0 (morta)
    float tamanho;
    int cor;
};

class Animacao {
public:
    float x, y;
    bool ativo;
    float duracao;     // Tempo total de vida da explosão (ex: 0.5 segundos)
    float tempoAtual;  // Cronômetro interno
    std::vector<Particula> particulas;

    // Construtor
    Animacao(float _x, float _y) {
        x = _x;
        y = _y;
        ativo = true;
        duracao = 0.5f; // Meio segundo
        tempoAtual = 0.0f;

        // 20 a 30 partículas para explosão
        int numParticulas = 20 + (rand() % 10);

        for (int i = 0; i < numParticulas; i++) {
            Particula p;
            p.pos = Vector2(x, y);

            // vetor de direção 
            float vx = (rand() % 300 - 150) * 1.5f;
            float vy = (rand() % 300 - 150) * 1.5f;
            p.vel = Vector2(vx, vy);

            p.vida = 1.0f;
            p.tamanho = 2.0f + (rand() % 4); // Tamanho aleatório 

            // Sorteia cores 
            int r = rand() % 3;
            if (r == 0) p.cor = 2; 
            else if (r == 1) p.cor = 7; 
            else p.cor = 1; 

            particulas.push_back(p);
        }
    }

    void atualizar(float dt) {
        if (!ativo) return;

        tempoAtual += dt;

        // Se o tempo da animação acabou, desativa
        if (tempoAtual >= duracao) {
            ativo = false;
            return;
        }

        // Move as partículas
        for (int i = 0; i < particulas.size(); i++) {
            particulas[i].pos.x += particulas[i].vel.x * dt;
            particulas[i].pos.y += particulas[i].vel.y * dt;

            // Calcula a "vida" restante da partícula (começa em 1 e cai até 0)
            particulas[i].vida = 1.0f - (tempoAtual / duracao);
        }
    }

    void desenhar() {
        if (!ativo) return;

        for (int i = 0; i < particulas.size(); i++) {
            if (particulas[i].vida > 0) {
                CV::color(particulas[i].cor);

                //encolhe tamanho
                float tamAtual = particulas[i].tamanho * particulas[i].vida;

                float px = particulas[i].pos.x;
                float py = particulas[i].pos.y;
                CV::rectFill(px - tamAtual, py - tamAtual, px + tamAtual, py + tamAtual);
            }
        }

        CV::color(1);
    }
};

#endif