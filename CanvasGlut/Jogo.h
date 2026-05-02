#ifndef __JOGO_H__
#define __JOGO_H__

#include <GL/glut.h>
#include <vector>
#include "Jogador.h"
#include "Inimigo.h"
#include "Frames.h"
#include "Animacao.h"
#include "Bmp.h"


class Jogo {
public:
    int scrWidth, scrHeight;
    float larguraCenario, alturaCenario;
    int pontuacao;
    int vidas;
    bool gameOver;
    int ondas;
    float proxOnda;
    float interInit;
    // Controle de FPS / Delta Time
    Frames* frames;
    float fpsReal;

    Jogador jogador;
    Bmp* imgJogador;

    std::vector<Inimigo> inimigos;
    Bmp* imgInimigo;

    std::vector<Projetil> disparosJogador;
    std::vector<Projetil> disparosInimigos;

    std::vector<Animacao> animacoes;

    std::vector<Vector2> estrelas;
    float velEstrelas;

    Jogo() {
    }

void gerarOnda(int quantidade, float& offsetY) {
    // Calcula quantas colunas cabem na tela (ex: um inimigo a cada 100 pixels)
    int colunas = scrWidth / 100; 
    if (colunas < 2) colunas = 2; // Garante pelo menos 2 colunas para não ficarem em fila única

    // Define o espaço horizontal de cada "trilha" de inimigos
    float espacamentoX = (float)scrWidth / colunas;
    
    // A variação agora é proporcional ao espaço disponível para não haver sobreposição excessiva
    float variacaoX = espacamentoX * 0.3f; 
    float variacaoY = 40.0f;

    for (int i = 0; i < quantidade; i++) {
        Inimigo ini;

        // Calcula a posição base centralizada na coluna
        float baseX = (i % colunas) * espacamentoX + (espacamentoX / 2.0f);
        float baseY = offsetY + (i / colunas) * 130.0f;

        // Aplica a variação aleatória
        ini.x = baseX + (rand() % (int)(variacaoX * 2)) - variacaoX;
        ini.y = baseY + (rand() % (int)(variacaoY * 2)) - variacaoY;

        // Clamp de segurança: evita que inimigos nasçam fora da tela lateralmente
        if (ini.x < 30) ini.x = 30;
        if (ini.x > scrWidth - 30) ini.x = scrWidth - 30;

        // Configurações de tiro permanecem iguais
        ini.tempoRecarga = 1.0f + (rand() % 200) / 100.0f;
        ini.cronometroTiro = (0.8f + (rand() % 20) / 100.0f) * ini.tempoRecarga;

        inimigos.push_back(ini);
    }

    // O offsetY continua subindo conforme o número de linhas geradas[cite: 8]
    int linhas = (quantidade + colunas - 1) / colunas;
    offsetY += linhas * 130.0f;
}

    void inicializar(int w, int h) {
        scrWidth = w;
        scrHeight = h;

        imgJogador = new Bmp("img/gremio_f.bmp");
        imgJogador->resize(40, 40);
        imgInimigo = new Bmp("img/inter_f.bmp");
        imgInimigo->resize(30,30);

        estrelas.clear();
        inimigos.clear();           
        disparosJogador.clear();   
        disparosInimigos.clear();  
        animacoes.clear();
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

        // Configuração do fluxo das ondas
        proxOnda = h + 250.0f;
        interInit = 15;
        ondas = 1;
        gerarOnda(interInit, proxOnda);

        velEstrelas = 50.0f;
        for (int i = 0; i < 100; i++) {
            Vector2 pos(rand() % w, rand() % h);
            estrelas.push_back(pos);
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
        if (gameOver)
            return;

        for (int i = 0; i < estrelas.size(); i++) {
            // Subtraímos a velocidade do componente Y do Vector2
            estrelas[i].y -= velEstrelas * dt;

            // Reset ao sair da tela
            if (estrelas[i].y < 0) {
                estrelas[i].y = scrHeight;
                estrelas[i].x = rand() % scrWidth;
            }
        }


        float maiorY = 0.0f;
        bool semInimigos = inimigos.empty();
        for (int i = 0; i < inimigos.size(); i++) {
            if (inimigos[i].y > maiorY) {
                maiorY = inimigos[i].y;
            }
        }

        // CONDIÇÕES PARA NOVA ONDA:
        // 1. Todos os inimigos foram derrotados (vetor vazio).
        // 2. O último inimigo (o que tem menor Y) está perto do limite inferior (ex: y < 150).
        if (semInimigos || maiorY < 0.0f) {
            ondas++;

            // Spawnamos a nova onda bem acima da tela para dar tempo ao jogador
            proxOnda = scrHeight + 250.0f;

            // Dificuldade progressiva: base + (ondas * incremento)
            int qtdInimigosNovaOnda = interInit + (ondas * 5);

            gerarOnda(qtdInimigosNovaOnda, proxOnda);
        }

        jogador.atualizar(dt, scrWidth, alturaCenario, disparosJogador);

        for (int i = 0; i < inimigos.size(); i++) {
            inimigos[i].atualizar(dt, scrWidth, alturaCenario, disparosInimigos);

            // Remove inimigo se ele passou do fundo da tela ou morreu
            if (inimigos[i].y < -50.0f || !inimigos[i].ativo) {
                inimigos.erase(inimigos.begin() + i);
                i--;
            }
        }

        // 1. Atualiza os Projéteis do Jogador
        for (int i = 0; i < disparosJogador.size(); i++) {
            disparosJogador[i].atualizar(dt);
            if (disparosJogador[i].y > alturaCenario || disparosJogador[i].x < 0 || disparosJogador[i].x > scrWidth || !disparosJogador[i].ativo) {
                disparosJogador.erase(disparosJogador.begin() + i);
                i--;
            }
        }

        // 2. Atualiza os Projéteis dos Inimigos
        for (int i = 0; i < disparosInimigos.size(); i++) {
            disparosInimigos[i].atualizar(dt);
            if (disparosInimigos[i].y < 0 || !disparosInimigos[i].ativo) {
                disparosInimigos.erase(disparosInimigos.begin() + i);
                i--;
            }
        }

        // 3. Atualiza animacoes
        for (int i = 0; i < animacoes.size(); i++) {
            animacoes[i].atualizar(dt);
        }
    }

    void checarColisoes() {
        // 1. Checa se algum TIRO DO JOGADOR acertou um INIMIGO
        for (int t = 0; t < disparosJogador.size(); t++) {
            if (!disparosJogador[t].ativo) continue;

            for (int i = 0; i < inimigos.size(); i++) {
                if (!inimigos[i].ativo) continue;

                if (inimigos[i].y < scrHeight && inimigos[i].checaColisaoCircular(disparosJogador[t].x, disparosJogador[t].y)) {
                    animacoes.push_back(Animacao(inimigos[i].x, inimigos[i].y));
                    inimigos[i].ativo = false;      // Mata Inimigo
                    disparosJogador[t].ativo = false; // Destrói bala
                    pontuacao++;
                    break;
                }
            }
        }
        if (jogador.ativo) {
            for (int i = 0; i < inimigos.size(); i++) {
                for (int t = 0; t < disparosInimigos.size(); t++) {
                    if (!disparosInimigos[t].ativo) continue;

                    if (jogador.checaColisaoCircular(disparosInimigos[t].x, disparosInimigos[t].y)) {// 2. Checa se algum TIRO DO INIMIGO acertou o JOGADOR
                        disparosInimigos[t].ativo = false;
                        vidas--;

                        if (vidas <= 0) {
                            jogador.ativo = false; // Game Over
                            gameOver = true;
                        }
                        
                        
                    }
                }
                if (!inimigos[i].ativo) continue;
                if (jogador.colideCom(inimigos[i].x, inimigos[i].y, inimigos[i].raio)) {// 3. Checa se algum inimigo colidiu com o jogador
                    inimigos[i].ativo = false;
                    vidas--;

                    if (vidas <= 0) {
                        jogador.ativo = false; // Game Over
                        gameOver = true;
                    }
                }
            }
        }
    }

    void desenhar() {
        // Desenha as estrelas antes de tudo
        CV::color(1, 1, 1); // Branco
        for (int i = 0; i < estrelas.size(); i++) {     
            CV::point(estrelas[i].x, estrelas[i].y);
        }

        jogador.desenhar();
        imgJogador->render(jogador.x, jogador.y);

        for (int i = 0; i < inimigos.size(); i++) {
            inimigos[i].desenhar();
            imgInimigo->render(inimigos[i].x, inimigos[i].y);
        }

        // Desenha todos os projéteis centralizados
        for (int i = 0; i < disparosJogador.size(); i++) {
            disparosJogador[i].desenhar(2); 
        }
        for (int i = 0; i < disparosInimigos.size(); i++) {
            disparosInimigos[i].desenhar(2); 
        }

        for (int i = 0; i < animacoes.size(); i++) {
            animacoes[i].desenhar();
        }

        float largAvis = 110;
        float altAvis = 20;
        float espaco = 5;

        float posXPlacar = 13;
        float posYPlacar = 15;
        CV::color(1);
        CV::rectFill(posXPlacar, posYPlacar, posXPlacar + largAvis, posYPlacar + altAvis);
        CV::color(0); 
        char buffer[30];
        snprintf(buffer, sizeof(buffer), "Placar: %d", pontuacao);
        CV::text(posXPlacar + espaco, posYPlacar + espaco, buffer);

        // VIDAS 
        float posXVidas = (scrWidth / 2) - jogador.width;
        float posYVidas = 15;
        char bufferVidas[30];
        snprintf(bufferVidas, sizeof(bufferVidas), "Vidas: %d", vidas);
        CV::color(1);
        CV::rectFill(posXVidas, posYVidas, posXVidas + largAvis - 20, posYVidas + altAvis);
        CV::color(0);
        CV::text(posXVidas + espaco, posYVidas + espaco, bufferVidas);

        // FPS
        float posXFPS = scrWidth - 120;
        float posYFPS = 15;
        char bufferFPS[20];
        snprintf(bufferFPS, sizeof(bufferFPS), "FPS: %.1f", fpsReal);
        CV::color(1);
        CV::rectFill(posXFPS, posYFPS, posXFPS + largAvis, posYFPS + altAvis);
        CV::color(0);
        CV::text(posXFPS + espaco, posYFPS + espaco, bufferFPS);

        if (gameOver) {
            float posXaviso = scrWidth / 2 - 130;
            float posYaviso = scrHeight / 2 - 30;
            float alturaAviso = 50;
            float larguraAviso = 290;
            CV::color(1);
            CV::rectFill(posXaviso, posYaviso, posXaviso + larguraAviso, posYaviso + alturaAviso);
            CV::color(2); 
            CV::text(posXaviso + 80, posYaviso + 30, "GAME OVER");
            CV::color(0);
            CV::text(posXaviso + 20, posYaviso + 10, "Pressione R para reiniciar");
        }
    }
};

#endif