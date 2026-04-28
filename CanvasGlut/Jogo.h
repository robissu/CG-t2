#ifndef __JOGO_H__
#define __JOGO_H__

#include <GL/glut.h>
#include <vector>
#include "Jogador.h"
#include "Inimigo.h"
#include "Frames.h"


class Jogo {
public:
    int scrWidth, scrHeight;
    float larguraCenario, alturaCenario;
    int pontuacao;
    int vidas;
    bool gameOver;
    // Controle de FPS / Delta Time
    Frames* frames;
    float fpsReal;
    Jogador jogador;
    std::vector<Inimigo> inimigos;

    Jogo() {
    }

    void inicializar(int w, int h) {
        scrWidth = w;
        scrHeight = h;

        inimigos.clear();           
        jogador.disparos.clear();   
        pontuacao = 0;              

        frames = new Frames();
        vidas = 3;
        gameOver = false;
        jogador.ativo = true;
        fpsReal = 30.0f;


        // Cenário 10x a altura da janela
        larguraCenario = w;
        alturaCenario = h * 10.0f;

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

    void executarLoop() {
        //fps
        fpsReal = frames->getFrames();
        float dt;
        if (fpsReal <= 20.1f) {
            dt = 0.016f;
        }
        else {
            dt = 1.0f / fpsReal;
        }

        if (dt > 0.1f) 
            dt = 0.1f;

        if (!gameOver) {
            atualizarLogica(dt);
            checarColisoes();
        }
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

                if (inimigos[i].y < scrHeight && inimigos[i].checaColisao(jogador.disparos[t].x, jogador.disparos[t].y, jogador.disparos[t].width, jogador.disparos[t].height)) {
                    inimigos[i].ativo = false;      // Mata Inimigo
                    jogador.disparos[t].ativo = false; // Destrói bala
                    pontuacao++;
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
                        inimigos[i].disparos[t].ativo = false;
                        vidas--;

                        if (vidas <= 0) {
                            jogador.ativo = false; // Game Over
                            gameOver = true;
                        }
                        
                        
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

        CV::color(1); 
        char buffer[30];
        snprintf(buffer, sizeof(buffer), "Placar: %d", pontuacao);
        CV::text(20, scrHeight - 30, buffer); 

        char bufferFPS[20];
        snprintf(bufferFPS, sizeof(bufferFPS), "FPS: %.1f", fpsReal);
        CV::text(scrWidth - 120, scrHeight - 30, bufferFPS);

        if (gameOver) {
            CV::color(2); // Vermelho
            CV::text(scrWidth / 2 - 50, scrHeight / 2, "GAME OVER");
            CV::color(1);
            CV::text(scrWidth / 2 - 80, scrHeight / 2 - 20, "Pressione R para reiniciar");
        }
    }
};

#endif