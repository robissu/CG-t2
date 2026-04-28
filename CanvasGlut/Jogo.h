#ifndef __JOGO_H__
#define __JOGO_H__

#include <GL/glut.h>
#include <vector>
#include "Jogador.h"
#include "Inimigo.h"

class Jogo {
public:
    int scrWidth, scrHeight;
    float larguraCenario, alturaCenario;

    // Controle de FPS / Delta Time
    int lastTime;

    Jogador jogador;
    std::vector<Inimigo> inimigos;

    Jogo() {
        lastTime = 0;
    }

    void inicializar(int w, int h) {
        scrWidth = w;
        scrHeight = h;

        // Cenário 10x a altura da janela
        larguraCenario = w;
        alturaCenario = h * 10.0f;

        // Inicia o relogio
        lastTime = glutGet(GLUT_ELAPSED_TIME);

        // Posição inicial do jogador
        jogador.x = w / 2.0f - (jogador.width / 2.0f);
        jogador.y = 50.0f; // Perto da base

        // Instancia os Inimigos FORA da visão inicial (descendo)
        // Começaremos a espalhar a partir de h + 100 até o topo do cenário (alturaCenario)
        for (int i = 0; i < 40; i++) {
            Inimigo ini;

            float variacaoX = 30.0f;
            float variacaoY = 40.0f;

            float baseX = 50.0f + (i % 6) * 90.0f;
            float baseY = h + 100.0f + (i / 6) * 130.0f;

            ini.x = baseX + (rand() % (int)(variacaoX * 2)) - variacaoX;
            ini.y = baseY + (rand() % (int)(variacaoY * 2)) - variacaoY;
            ini.cronometroTiro = (rand() % 100) / 100.0f * ini.tempoRecarga;
            ini.tempoRecarga = 1.0f + (rand() % 200) / 100.0f; // Algo entre 1.0 e 3.0 segundos
            inimigos.push_back(ini);
        }
    }

    // Calcula a fração de tempo passada (1 / framerate)
    float calcularDeltaTime() {
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        float dt = (currentTime - lastTime) / 1000.0f; // dt em segundos
        lastTime = currentTime;

        // Trava de segurança: se o PC der um lag muito grande (ex: arrastou a janela)
        // o dt não passa de 0.1s para não fazer os objetos "teletransportarem".
        if (dt > 0.1f) dt = 0.1f;

        return dt;
    }

    // Chamado dentro da sua void render()
    void executarLoop() {
        float dt = calcularDeltaTime();

        atualizarLogica(dt);
        checarColisoes();
        desenhar();
    }

    void atualizarLogica(float dt) {
        jogador.atualizar(dt, scrWidth, alturaCenario);

        for (int i = 0; i < inimigos.size(); i++) {
            inimigos[i].atualizar(dt, scrWidth, alturaCenario);

            // Remove inimigo se ele passou do fundo da tela ou morreu
            if (inimigos[i].y < -50.0f || !inimigos[i].ativo) {
                inimigos.erase(inimigos.begin() + i);
                i--;
            }
        }
    }

    void checarColisoes() {
        // 1. Checa se algum TIRO DO JOGADOR acertou um INIMIGO
        for (int t = 0; t < jogador.disparos.size(); t++) {
            if (!jogador.disparos[t].ativo) continue;

            for (int i = 0; i < inimigos.size(); i++) {
                if (!inimigos[i].ativo) continue;

                if (inimigos[i].checaColisao(jogador.disparos[t].x, jogador.disparos[t].y, jogador.disparos[t].width, jogador.disparos[t].height)) {
                    inimigos[i].ativo = false;      // Mata Inimigo
                    jogador.disparos[t].ativo = false; // Destrói bala
                    break;
                }
            }
        }

        // 2. Checa se algum TIRO DO INIMIGO acertou o JOGADOR
        if (jogador.ativo) {
            for (int i = 0; i < inimigos.size(); i++) {
                for (int t = 0; t < inimigos[i].disparos.size(); t++) {
                    if (!inimigos[i].disparos[t].ativo) continue;

                    if (jogador.checaColisao(inimigos[i].disparos[t].x, inimigos[i].disparos[t].y, inimigos[i].disparos[t].width, inimigos[i].disparos[t].height)) {
                        jogador.ativo = false; // Game Over
                        inimigos[i].disparos[t].ativo = false;
                    }
                }
            }
        }
    }

    void desenhar() {
        jogador.desenhar();
        for (int i = 0; i < inimigos.size(); i++) {
            inimigos[i].desenhar();
        }
    }
};

#endif