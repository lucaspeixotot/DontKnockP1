#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "botoes.h"
volatile int fechar_prog=FALSE;
volatile int tempo;
volatile int tickrate;
volatile int score;

float maior(float n1, float n2)
{
    if (n2 >= n1)
        return n2;
    else
        return n1;
}
float menor(float n1, float n2)
{
    if (n2 >= n1)
        return n1;
    else
        return n2;
}
int colisao_pixel_per_pixel(BITMAP* car, float car_x, float car_y, BITMAP* obstaculo, float obs_x, float obs_y)
{
    int x,y;
    int lim_superior, lim_inferior, lim_esquerda, lim_direita;
    int colisao = FALSE;
    if ( !(car_x > obs_x + obstaculo->w || car_y > obs_y + obstaculo->h ||
        obs_x > car_x + car->w || obs_y > car_y + car->h))
    {
        lim_superior = maior(car_y,obs_y);
        lim_inferior = menor(car_y + car->h,obs_y + obstaculo->h);
        lim_esquerda = maior(car_x,obs_x);
        lim_direita = menor(car_x + car->w,obs_x + obstaculo->w);
        for (y = lim_superior ; y < lim_inferior && !colisao ; y++)
        {
            for (x = lim_esquerda ; x < lim_direita && !colisao ; x++)
            {
                if (getpixel(car, x-car_x, y-car_y) != makecol(255,0,255) && getpixel(obstaculo, x-obs_x, y-obs_y) != makecol(255,0,255))
                    colisao=TRUE;
            }
        }
    }
    return colisao;
}
void decorrer_do_tempo()
{
    tempo++;
}
END_OF_FUNCTION(decorrer_do_tempo)
void decorrer_tickrate()
{
    tickrate++;
}
END_OF_FUNCTION(decorrer_tickrate)
void fechar_programa()
{
    fechar_prog=TRUE;
}
END_OF_FUNCTION(fechar_programa)
enum{PREGAME,ESCOLHA_NIVEL,OLHE_OS_CREDITOS,OLHE_AS_INSTRUCOES,FASE_INICIANTE,FASE_FACIL,FASE_RAZOAVEL,FASE_DIFICIL};
int estado_de_tela;

///FUNÇÃO PARA INICIALIZAÇÃO

void inicio()
{
    ///INICIALIZAÇÃO

    int depth;
    allegro_init(); // Inicializa coisas básicas da biblioteca.
    install_timer(); // Inicializa timers necessários para audio e mouse, por exemplo...
    install_mouse(); // Inicializa o mouse.
    install_keyboard(); // Instala o teclado
    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL); // Instala o som, e detecta seu drievr de audio...
    depth=desktop_color_depth(); // Pega a profundidade de cor do desktop...
    set_color_depth(depth); // Profundidade das cores. O parâmetro é a quantidade de cores, ou seja, 8 bits, 16 bits, 32 bits, etc...
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0); // Inicializa o vídeo. Parâmetros (DRIVER DE VIDEO(recomendado autodetect),res(x),res(y),0,0);
    set_window_title("DON'T KNOCK"); // Titulo da janela
    tempo=0;
    LOCK_FUNCTION(decorrer_do_tempo);
    LOCK_VARIABLE(tempo);
    tickrate=0;
    LOCK_FUNCTION(decorrer_tickrate);
    LOCK_VARIABLE(tickrate);
    install_int_ex(decorrer_tickrate,BPS_TO_TIMER(100));
    install_int_ex(decorrer_do_tempo, MSEC_TO_TIMER(1));
    LOCK_VARIABLE(fechar_prog);
    LOCK_FUNCTION(fechar_programa);


    estado_de_tela = PREGAME;

}

///FUNÇÃO PARA TELA INICIAL


void pregame()
{
    int fechar_tela = FALSE;
    set_close_button_callback(fechar_programa);

    ///BITMAP P/ A TELA E SONS

    BITMAP *telafake = create_bitmap(800, 600);
    BITMAP *bg = load_bitmap("imagens/bg.bmp",NULL);
    BITMAP *iniciar = load_bitmap("imagens/jogar__1.bmp",NULL);
    BITMAP *iniciar_s = load_bitmap("imagens/jogar__2.bmp",NULL);
    BITMAP *instr = load_bitmap("imagens/instrucoes__1.bmp",NULL);
    BITMAP *instr_s = load_bitmap("imagens/instrucoes__2.bmp",NULL);
    BITMAP *credi = load_bitmap("imagens/creditos__1.bmp",NULL);
    BITMAP *credi_s = load_bitmap("imagens/creditos__2.bmp",NULL);
    BITMAP *s = load_bitmap("imagens/sair__1.bmp",NULL);
    BITMAP *s_s = load_bitmap("imagens/sair__2.bmp",NULL);
    BITMAP *cursor = load_bitmap("imagens/cursor.bmp",NULL);
    BITMAP *cursor_selecionado = load_bitmap ("imagens/cursor_selecionado.bmp",NULL);
    SAMPLE *click_selecionado = load_sample("sons/click_selecionado.wav");
    SAMPLE *click_ativado = load_sample("sons/escolhido.wav");
    SAMPLE *musica_de_fundo = load_sample("sons/fundo.wav");

    ///BOTOES

    botao* jogar = create_button(iniciar,iniciar_s,click_selecionado,click_ativado,-1,21);
    botao* instrucoes = create_button(instr,instr_s,click_selecionado,click_ativado,200,21);
    botao* sair = create_button(s,s_s,click_selecionado,click_ativado,601,21);
    botao* creditos = create_button(credi,credi_s,click_selecionado,click_ativado,400,21);

    while (!fechar_prog && !fechar_tela)
    {
        ///INICIALIZAÇÃO DOS BOTÕES

        button_input(jogar);
        button_input(sair);
        button_input(creditos);
        button_input(instrucoes);

        /// DESENHO

        draw_sprite(telafake,bg,0,0);
        button_draw(jogar,telafake);
        button_draw(instrucoes,telafake);
        button_draw(creditos,telafake);
        button_draw(sair,telafake);


        if (jogar->select || sair->select || creditos->select || instrucoes->select)
            draw_sprite(telafake,cursor_selecionado,mouse_x-9,mouse_y);
        else
            draw_sprite(telafake,cursor,mouse_x-7,mouse_y);
        if (key[KEY_ESC])
        {
            fechar_prog=TRUE;
            fechar_tela=TRUE;
        }
        if (jogar->ativado)
        {
            stop_sample(musica_de_fundo);
            estado_de_tela = ESCOLHA_NIVEL;
            fechar_tela=TRUE;
        }
        if (sair->ativado)
        {
            fechar_tela=TRUE;
            fechar_prog=TRUE;
        }
        if (creditos->ativado)
        {
            fechar_tela = TRUE;
            estado_de_tela = OLHE_OS_CREDITOS;
        }
        if (instrucoes->ativado)
        {
            fechar_tela = TRUE;
            estado_de_tela = OLHE_AS_INSTRUCOES;
        }
        draw_sprite(screen,telafake,0,0);
        clear(telafake);
    }

    ///FINALIZAÇÃO

    destroy_bitmap(telafake);
    destroy_bitmap(bg);
    destroy_bitmap(iniciar);
    destroy_bitmap(iniciar_s);
    destroy_bitmap(credi);
    destroy_bitmap(credi_s);
    destroy_bitmap(instr);
    destroy_bitmap(instr_s);
    destroy_bitmap(s);
    destroy_bitmap(s_s);
    destroy_sample(click_ativado);
    destroy_sample(click_selecionado);
    destroy_sample(musica_de_fundo);
    destroy_bitmap(cursor);
    destroy_bitmap(cursor_selecionado);
}

///FUNÇÃO PARA ESCOLHER A DIFICULDADE

void escolha_da_dificuldade()
{
    int fechar_escolha = FALSE;
    set_close_button_callback(fechar_programa);

    /// BITMAP PARA TELA

    BITMAP *telafake = create_bitmap(800,600);
    BITMAP *bg = load_bitmap("imagens/bg.bmp",NULL);
    BITMAP *iniciante = load_bitmap("imagens/INICIANTE.BMP",NULL);
    BITMAP *iniciante_s = load_bitmap("imagens/INICIANTE2.BMP",NULL);
    BITMAP *facil = load_bitmap("imagens/FACIL.BMP",NULL);
    BITMAP *facil_s = load_bitmap("imagens/FACIL2.BMP",NULL);
    BITMAP *razoavel = load_bitmap("imagens/RAZOAVEL.BMP",NULL);
    BITMAP *razoavel_s = load_bitmap("imagens/RAZOAVEL2.BMP",NULL);
    BITMAP *dificil = load_bitmap("imagens/DIFICIL.BMP",NULL);
    BITMAP *dificil_s = load_bitmap("imagens/DIFICIL2.BMP",NULL);
    BITMAP *cursor = load_bitmap("imagens/cursor.bmp",NULL);
    BITMAP *cursor_selecionado = load_bitmap ("imagens/cursor_selecionado.bmp",NULL);
    SAMPLE *click_selecionado = load_sample("sons/click_selecionado.wav");
    SAMPLE *click_ativado = load_sample("sons/escolhido.wav");

    ///BOTÕES

    botao* but_iniciante = create_button(iniciante,iniciante_s,click_selecionado,click_ativado,0,20);
    botao* but_facil = create_button(facil,facil_s,click_selecionado,click_ativado,200,20);
    botao* but_razoavel = create_button(razoavel,razoavel_s,click_selecionado,click_ativado,401,20);
    botao* but_dificil = create_button(dificil,dificil_s,click_selecionado,click_ativado,600,20);

    while (!fechar_prog && !fechar_escolha)
    {

        /// Inicialização dos botões

        button_input(but_iniciante);
        button_input(but_facil);
        button_input(but_razoavel);
        button_input(but_dificil);

        ///Desenho

        draw_sprite(telafake,bg,0,0);
        button_draw(but_iniciante,telafake);
        button_draw(but_facil,telafake);
        button_draw(but_razoavel,telafake);
        button_draw(but_dificil,telafake);

        if (but_iniciante->select || but_facil->select || but_razoavel->select || but_dificil->select)
            draw_sprite(telafake,cursor_selecionado,mouse_x-9,mouse_y);
        else
            draw_sprite(telafake,cursor,mouse_x-7,mouse_y);

        if (key[KEY_ESC])
        {
            fechar_escolha = TRUE;
            estado_de_tela = PREGAME;
        }
        if (but_iniciante->ativado)
        {
            fechar_escolha = TRUE;
            estado_de_tela = FASE_INICIANTE;
        }
        if (but_facil->ativado)
        {
            fechar_escolha = TRUE;
            estado_de_tela = FASE_FACIL;
        }
        if (but_razoavel->ativado)
        {
            fechar_escolha = TRUE;
            estado_de_tela = FASE_RAZOAVEL;
        }
        if (but_dificil->ativado)
        {
            fechar_escolha = TRUE;
            estado_de_tela = FASE_DIFICIL;
        }
        draw_sprite(screen,telafake,0,0);
        clear(telafake);
    }

    ///FINALIZAÇÃO

    destroy_bitmap(telafake);
    destroy_bitmap(bg);
    destroy_bitmap(iniciante);
    destroy_bitmap(iniciante_s);
    destroy_bitmap(facil);
    destroy_bitmap(facil_s);
    destroy_bitmap(razoavel);
    destroy_bitmap(razoavel_s);
    destroy_bitmap(dificil);
    destroy_bitmap(dificil_s);
    destroy_bitmap(cursor);
    destroy_bitmap(cursor_selecionado);
    destroy_sample(click_selecionado);
    destroy_sample(click_ativado);


}

///FUNÇÃO PARA TELA DE CREDITOS

void creditos()
{
    int cred = FALSE;
    set_close_button_callback(fechar_programa);

    ///BITMAP P/ A TELA

    BITMAP *telafake = create_bitmap(800, 600);
    BITMAP *credito = load_bitmap("imagens/cred.bmp",NULL);

    ///INICIO DOS CREDITOS

    while (!fechar_prog && !cred)
    {
        if (key[KEY_ESC])
        {
            cred=TRUE;
            estado_de_tela = PREGAME;
        }
        draw_sprite(telafake,credito,0,0);
        draw_sprite(screen,telafake,0,0);
        clear(telafake);
    }

    ///FINALIZAÇÃO

    destroy_bitmap(telafake);
    destroy_bitmap(credito);
}

///INSTRUÇÕES

void instrus()
{
    int fechar_instru = FALSE;
    set_close_button_callback(fechar_programa);

    ///BITMAPS

    BITMAP *telafake = create_bitmap(800, 600);
    BITMAP *inst = load_bitmap("imagens/instru.bmp",NULL);

    while (!fechar_prog && !fechar_instru)
    {
        if (key[KEY_ESC])
        {
            fechar_instru = TRUE;
            estado_de_tela = PREGAME;
        }
        draw_sprite(telafake,inst,0,0);
        draw_sprite(screen,telafake,0,0);
        clear(telafake);
    }

    ///FINALIZAÇÃO

    destroy_bitmap(telafake);
    destroy_bitmap(inst);
}


/// FUNÇÃO PARA RODAR O JOGO ( FASE INICIANTE )

void jogo_iniciante()
{
    ///INICIALIZAÇÃO DAS VARIAVEIS

    int fechar_jogo=FALSE;
    int aux_aumentar_dificuldade=0;
    float car_x=400,car_y=490;
    float velocidade=5.0;
    float velocidade_car=3.0;
    float pneu_x,pneu_y=600,banana_x,banana_y=600;
    int marcador_obstaculos1,marcador_obstaculos2;
    set_close_button_callback(fechar_programa);

    srand(time(NULL));
    marcador_obstaculos1=tempo;
    marcador_obstaculos2=tempo;
    score=-2;
    tickrate=0;

    ///CRIAÇÃO DOS BITMAPS

    BITMAP *telafake = create_bitmap(800,600);
    BITMAP *pista = load_bitmap("imagens/pista_iniciante.bmp",NULL);
    BITMAP *pneu = load_bitmap("imagens/pneu.bmp",NULL);
    BITMAP *carro = load_bitmap("imagens/carro_teste.bmp",NULL);
    BITMAP *banana = load_bitmap("imagens/casca_de_banana.bmp",NULL);
    BITMAP *best = load_bitmap("imagens/best.bmp",NULL);
    BITMAP *atual = load_bitmap("imagens/score.bmp",NULL);
    BITMAP *cursor = load_bitmap("imagens/cursor.bmp",NULL);
    BITMAP *cursor_selecionado = load_bitmap ("imagens/cursor_selecionado.bmp",NULL);
    BITMAP *tentar = load_bitmap("imagens/tentar.bmp",NULL);
    SAMPLE *click_selecionado = load_sample("sons/click_selecionado.wav");
    SAMPLE *click_ativado = load_sample("sons/escolhido.wav");

    ///CRIAÇÃO DOS ELEMENTOS NECESSARIOS PARA O SCORE

    int pontuacao;
    FILE *arq_pontuacao;
    arq_pontuacao = fopen("score_iniciante.txt","r");
    fscanf(arq_pontuacao,"%d",&pontuacao);
    fclose(arq_pontuacao);
    arq_pontuacao = fopen("score_iniciante.txt","w");
    fprintf(arq_pontuacao,"%d",pontuacao);
    fclose(arq_pontuacao);

    ///FONTS

    FONT *edessa_score = load_font("edessa_score.pcx",NULL,NULL);
    FONT *score_freeze = load_font("score_freeze.pcx",NULL,NULL);
    FONT *gameover = load_font("gameover.pcx",NULL,NULL);

    ///BOTÃO DE TRY

    botao* denovo = create_button(tentar,tentar,click_selecionado,click_ativado,700,240);

    ///INICIALIZAÇÃO DO LOOP DO JOGO

    while (!fechar_jogo && !fechar_prog)
    {
        while (tickrate > 0)
        {
            ///INPUT
            if (key[KEY_ESC])
            {
                fechar_jogo=TRUE;
                estado_de_tela = ESCOLHA_NIVEL;
                key[KEY_ESC] = FALSE;
            }
            if(key[KEY_RIGHT])
            {
                car_x=car_x+velocidade_car;
            }
            else if(key[KEY_LEFT])
            {
                car_x=car_x-velocidade_car;
            }

            ///UPDATE DAS VARIAVEIS

            pneu_y = pneu_y+velocidade;
            banana_y = banana_y+velocidade;
            if (car_x>=619)
            {
                car_x=car_x-velocidade_car;
            }
            if (car_x<=125)
            {
                car_x=car_x+velocidade_car;
            }

            ///JOGO NA TELA

            draw_sprite(telafake,pista,0,0);
            draw_sprite(telafake,carro,car_x,car_y);
            if (tempo - marcador_obstaculos1 >= 500 && pneu_y >= 600)
            {
                score++;
                pneu_x = rand()%619;
                while (pneu_x < 125)
                    pneu_x = rand()%619;
                pneu_y = 0;
                marcador_obstaculos1 = tempo;
            }
            if (tempo - marcador_obstaculos2 >= 900 && banana_y >= 600)
            {
                score++;
                banana_x = rand()%619;
                while (banana_x < 125)
                    banana_x = rand()%619;
                banana_y = 0;
                marcador_obstaculos2 = tempo;
            }
            draw_sprite(telafake,pneu,pneu_x,pneu_y);
            draw_sprite(telafake,banana,banana_x,banana_y);
            if (score < 0)
            {
                textprintf_centre_ex(telafake,edessa_score,45,250,makecol(255,255,255),-1,"Score:");
                textprintf_centre_ex(telafake,edessa_score,40,285,makecol(255,255,255),-1,"0");
            }
            else
            {
                textprintf_centre_ex(telafake,edessa_score,45,250,makecol(255,255,255),-1,"Score:");
                textprintf_centre_ex(telafake,edessa_score,40,285,makecol(255,255,255),-1,"%d",score);
            }
            if (score - aux_aumentar_dificuldade >=20)
            {
                if (velocidade < 8.5)
                {
                    velocidade = velocidade + 0.5;
                    aux_aumentar_dificuldade=score;
                }
            }
            if (colisao_pixel_per_pixel(carro,car_x,car_y,pneu,pneu_x,pneu_y) == TRUE ||
                colisao_pixel_per_pixel(carro,car_x,car_y,banana,banana_x,banana_y)== TRUE)
            {
                if (pontuacao <= score)
                {
                    arq_pontuacao = fopen("score_iniciante.txt","w");
                    fprintf(arq_pontuacao,"%d",score);
                    fclose(arq_pontuacao);
                }

                ///FREEZE-GAME

                while (!fechar_jogo)
                {
                    ///INPUT

                    button_input(denovo);
                    if (denovo->ativado)
                    {
                        fechar_jogo = TRUE;
                        estado_de_tela = FASE_INICIANTE;
                    }
                    else if (key[KEY_ESC])
                    {
                        key[KEY_ESC] = FALSE;
                        fechar_jogo = TRUE;
                        estado_de_tela = ESCOLHA_NIVEL;
                    }

                    ///DESENHO

                    draw_sprite(telafake,pista,0,0);
                    draw_sprite(telafake,carro,car_x,car_y);
                    draw_sprite(telafake,pneu,pneu_x,pneu_y);
                    draw_sprite(telafake,banana,banana_x,banana_y);
                    textout_ex(telafake,gameover,"GAME OVER",240,50,makecol(255,255,255),-1);
                    textprintf_centre_ex(telafake,edessa_score,45,250,makecol(255,255,255),-1,"Score:");
                    textprintf_centre_ex(telafake,edessa_score,40,285,makecol(255,255,255),-1,"%d",score);
                    button_draw(denovo,telafake);
                    if (pontuacao <= score)
                    {
                        draw_sprite(telafake,best,700,80);
                        draw_sprite(telafake,atual,700,400);
                        textprintf_centre_ex(telafake,score_freeze,743,143,makecol(255,255,255),-1,"%d",score);
                        textprintf_centre_ex(telafake,score_freeze,743,450,makecol(255,255,255),-1,"%d",score);
                    }
                    else
                    {
                        draw_sprite(telafake,best,700,80);
                        draw_sprite(telafake,atual,700,400);
                        textprintf_centre_ex(telafake,score_freeze,743,143,makecol(255,255,255),-1,"%d",pontuacao);
                        textprintf_centre_ex(telafake,score_freeze,743,450,makecol(255,255,255),-1,"%d",score);
                    }
                    if (denovo->select)
                        draw_sprite(telafake,cursor_selecionado,mouse_x-9,mouse_y);
                    else
                        draw_sprite(telafake,cursor,mouse_x-7,mouse_y);
                    draw_sprite(screen,telafake,0,0);
                    clear(telafake);
                }
            }
            draw_sprite(screen,telafake,0,0);
            clear(telafake);
            tickrate=0;
        }
    }

    ///FINALIZAÇÃO

    destroy_bitmap(telafake);
    destroy_bitmap(pista);
    destroy_bitmap(carro);
    destroy_bitmap(pneu);
    destroy_bitmap(banana);
    destroy_bitmap(best);
    destroy_bitmap(atual);
    destroy_bitmap(cursor);
    destroy_bitmap(cursor_selecionado);
    destroy_bitmap(tentar);
    destroy_button(denovo);
    destroy_sample(click_ativado);
    destroy_sample(click_selecionado);
    destroy_font(edessa_score);
    destroy_font(score_freeze);
    destroy_font(gameover);
}
///FUNÇÃO PARA RODAR O JOGO ( FASE FACIL)

void jogo_facil()
{
    ///INICIALIZAÇÃO DAS VARIAVEIS

    int fechar_jogo=FALSE;
    int aux_aumentar_dificuldade=0;
    float car_x=400,car_y=490;
    float velocidade=6.0;
    float velocidade_car=3.0;
    float pneu_x,pneu_y=600,agua_x,agua_y=600,banana_x,banana_y=600;
    int marcador_obstaculos,marcador_obstaculos2,marcador_obstaculos3;
    set_close_button_callback(fechar_programa);

    srand(time(NULL));
    marcador_obstaculos=tempo;
    marcador_obstaculos2=tempo;
    marcador_obstaculos3=tempo;
    score=-2;
    tickrate=0;

    ///CRIAÇÃO DOS BITMAPS

    BITMAP *telafake = create_bitmap(800, 600);
    BITMAP *pista = load_bitmap("imagens/pista_facil.bmp",NULL);
    BITMAP *pneu = load_bitmap("imagens/pneu.bmp",NULL);
    BITMAP *carro = load_bitmap("imagens/carro_teste.bmp",NULL);
    BITMAP *banana = load_bitmap("imagens/casca_de_banana.bmp",NULL);
    BITMAP *agua = load_bitmap("imagens/water.bmp",NULL);
    BITMAP *best = load_bitmap("imagens/best.bmp",NULL);
    BITMAP *atual = load_bitmap("imagens/score.bmp",NULL);
    BITMAP *cursor = load_bitmap("imagens/cursor.bmp",NULL);
    BITMAP *cursor_selecionado = load_bitmap ("imagens/cursor_selecionado.bmp",NULL);
    BITMAP *tentar = load_bitmap("imagens/tentar.bmp",NULL);
    SAMPLE *click_selecionado = load_sample("sons/click_selecionado.wav");
    SAMPLE *click_ativado = load_sample("sons/escolhido.wav");

    ///CRIAÇÃO DOS ELEMENTOS NECESSARIOS PARA O SCORE

    int pontuacao;
    FILE *arq_pontuacao;
    arq_pontuacao = fopen("score_facil.txt","r");
    fscanf(arq_pontuacao,"%d",&pontuacao);
    fclose(arq_pontuacao);
    arq_pontuacao = fopen("score_facil.txt","w");
    fprintf(arq_pontuacao,"%d",pontuacao);
    fclose(arq_pontuacao);

    ///FONTS

    FONT *edessa_score = load_font("edessa_score.pcx",NULL,NULL);
    FONT *score_freeze = load_font("score_freeze.pcx",NULL,NULL);
    FONT *gameover = load_font("gameover.pcx",NULL,NULL);
    ///BOTÃO DE TRY

    botao* denovo = create_button(tentar,tentar,click_selecionado,click_ativado,700,240);

    ///INICIALIZAÇÃO DO LOOP DO JOGO

    while (!fechar_prog && !fechar_jogo)
    {
        while(tickrate>0)
        {
            ///INPUT
            if (key[KEY_ESC])
            {
                fechar_jogo=TRUE;
                estado_de_tela = ESCOLHA_NIVEL;
                key[KEY_ESC] = FALSE;
            }
            if(key[KEY_RIGHT])
            {
                car_x=car_x+velocidade_car;
            }
            else if(key[KEY_LEFT])
            {
                car_x=car_x-velocidade_car;
            }

            ///UPDATE DAS VARIAVEIS

            pneu_y=pneu_y+velocidade;
            agua_y=agua_y+velocidade;
            banana_y=banana_y+velocidade;
            if (car_x>=619)
            {
                car_x=car_x-velocidade_car;
            }
            if (car_x<=125)
            {
                car_x=car_x+velocidade_car;
            }

            ///JOGO NA TELA

            draw_sprite(telafake,pista,0,0);
            draw_sprite(telafake,carro,car_x,car_y);
            if (tempo - marcador_obstaculos >= 600 && pneu_y>=600)
            {
                score++;
                pneu_x=rand()%619;
                while (pneu_x<125)
                    pneu_x=rand()%619;
                pneu_y=0;
                marcador_obstaculos=tempo;
            }
            if (tempo - marcador_obstaculos2 >=1000 && agua_y>=600)
            {
                score++;
                agua_x=rand()%619;
                while (agua_x<125)
                    agua_x=rand()%619;
                agua_y=0;
                marcador_obstaculos2=tempo;
            }
            if (tempo - marcador_obstaculos3 >=1400 && banana_y>=600)
            {
                score++;
                banana_x=rand()%619;
                while (banana_x<125)
                    banana_x=rand()%619;
                banana_y=0;
                marcador_obstaculos3=tempo;
            }
            draw_sprite(telafake,pneu,pneu_x,pneu_y);
            draw_sprite(telafake,agua,agua_x,agua_y);
            draw_sprite(telafake,banana,banana_x,banana_y);
            if (score < 0)
            {
                textprintf_centre_ex(telafake,edessa_score,45,250,makecol(255,255,255),-1,"Score:");
                textprintf_centre_ex(telafake,edessa_score,40,285,makecol(255,255,255),-1,"0");
            }
            else
            {
                textprintf_centre_ex(telafake,edessa_score,45,250,makecol(255,255,255),-1,"Score:");
                textprintf_centre_ex(telafake,edessa_score,40,285,makecol(255,255,255),-1,"%d",score);
            }
            if (score - aux_aumentar_dificuldade >=25)
            {
                if (velocidade < 9.5)
                {
                    velocidade=velocidade+0.5;
                    aux_aumentar_dificuldade=score;
                }
            }
            if (colisao_pixel_per_pixel(carro,car_x,car_y,pneu,pneu_x,pneu_y) == TRUE ||
                colisao_pixel_per_pixel(carro,car_x,car_y,agua,agua_x,agua_y)== TRUE ||
                colisao_pixel_per_pixel(carro,car_x,car_y,banana,banana_x,banana_y) == TRUE)
            {
                if (pontuacao <= score)
                {
                    arq_pontuacao = fopen("score_facil.txt","w");
                    fprintf(arq_pontuacao,"%d",score);
                    fclose(arq_pontuacao);
                }

                ///FREEZE-GAME


                while (!fechar_jogo)
                {
                    ///INPUT

                    button_input(denovo);
                    if (denovo->ativado)
                    {
                        fechar_jogo = TRUE;
                        estado_de_tela = FASE_FACIL;
                    }
                    else if (key[KEY_ESC])
                    {
                        key[KEY_ESC] = FALSE;
                        fechar_jogo = TRUE;
                        estado_de_tela = ESCOLHA_NIVEL;
                    }

                    ///DESENHO
                    draw_sprite(telafake,pista,0,0);
                    draw_sprite(telafake,carro,car_x,car_y);
                    draw_sprite(telafake,pneu,pneu_x,pneu_y);
                    draw_sprite(telafake,agua,agua_x,agua_y);
                    draw_sprite(telafake,banana,banana_x,banana_y);
                    textout_ex(telafake,gameover,"GAME OVER",240,50,makecol(255,255,255),-1);
                    textprintf_centre_ex(telafake,edessa_score,45,250,makecol(255,255,255),-1,"Score:");
                    textprintf_centre_ex(telafake,edessa_score,40,285,makecol(255,255,255),-1,"%d",score);
                    button_draw(denovo,telafake);
                    if (pontuacao <= score)
                    {
                        draw_sprite(telafake,best,700,80);
                        draw_sprite(telafake,atual,700,400);
                        textprintf_centre_ex(telafake,score_freeze,743,143,makecol(255,255,255),-1,"%d",score);
                        textprintf_centre_ex(telafake,score_freeze,743,450,makecol(255,255,255),-1,"%d",score);
                    }
                    else
                    {
                        draw_sprite(telafake,best,700,80);
                        draw_sprite(telafake,atual,700,400);
                        textprintf_centre_ex(telafake,score_freeze,743,143,makecol(255,255,255),-1,"%d",pontuacao);
                        textprintf_centre_ex(telafake,score_freeze,743,450,makecol(255,255,255),-1,"%d",score);
                    }
                    if (denovo->select)
                        draw_sprite(telafake,cursor_selecionado,mouse_x-9,mouse_y);
                    else
                        draw_sprite(telafake,cursor,mouse_x-7,mouse_y);
                    draw_sprite(screen,telafake,0,0);
                    clear(telafake);
                }
            }
            draw_sprite(screen,telafake,0,0);
            clear(telafake);
            tickrate=0;
        }
    }

    ///FINALIZAÇÃO

    destroy_bitmap(telafake);
    destroy_bitmap(pista);
    destroy_bitmap(pneu);
    destroy_bitmap(carro);
    destroy_bitmap(banana);
    destroy_bitmap(agua);
    destroy_bitmap(best);
    destroy_bitmap(atual);
    destroy_bitmap(cursor);
    destroy_bitmap(cursor_selecionado);
    destroy_bitmap(tentar);
    destroy_button(denovo);
    destroy_sample(click_ativado);
    destroy_sample(click_selecionado);
    destroy_font(edessa_score);
    destroy_font(score_freeze);
    destroy_font(gameover);
}

///FUNÇÃO PARA RODAR O JOGO ( FASE RAZOAVEL )

void jogo_razoavel()
{
    ///INICIALIZAÇÃO DAS VARIAVEIS

    int fechar_jogo=FALSE;
    int aux_aumentar_dificuldade=0;
    float car_x=400,car_y=490;
    float velocidade=6.0;
    float velocidade_car=3.0;
    float pneu_x,pneu_y=600,pneu2_x,pneu2_y=600,banana_x,banana_y=600,banana2_x,banana2_y=600;
    int marcador_obstaculos,marcador_obstaculos2,marcador_obstaculos3,marcador_obstaculos4;
    set_close_button_callback(fechar_programa);

    srand(time(NULL));
    marcador_obstaculos=tempo;
    marcador_obstaculos2=tempo;
    marcador_obstaculos3=tempo;
    marcador_obstaculos4=tempo;
    score=-2;
    tickrate=0;

    ///CRIAÇÃO DOS BITMAPS

    BITMAP *telafake = create_bitmap(800, 600);
    BITMAP *pista = load_bitmap("imagens/pista_razoavel.bmp",NULL);
    BITMAP *pneu = load_bitmap("imagens/pneu.bmp",NULL);
    BITMAP *carro = load_bitmap("imagens/carro_teste.bmp",NULL);
    BITMAP *banana = load_bitmap("imagens/casca_de_banana.bmp",NULL);
    BITMAP *best = load_bitmap("imagens/best.bmp",NULL);
    BITMAP *atual = load_bitmap("imagens/score.bmp",NULL);
    BITMAP *cursor = load_bitmap("imagens/cursor.bmp",NULL);
    BITMAP *cursor_selecionado = load_bitmap ("imagens/cursor_selecionado.bmp",NULL);
    BITMAP *tentar = load_bitmap("imagens/tentar.bmp",NULL);
    SAMPLE *click_selecionado = load_sample("sons/click_selecionado.wav");
    SAMPLE *click_ativado = load_sample("sons/escolhido.wav");

    ///CRIAÇÃO DOS ELEMENTOS NECESSARIOS PARA O SCORE

    int pontuacao;
    FILE *arq_pontuacao;
    arq_pontuacao = fopen("score_razoavel.txt","r");
    fscanf(arq_pontuacao,"%d",&pontuacao);
    fclose(arq_pontuacao);
    arq_pontuacao = fopen("score_razoavel.txt","w");
    fprintf(arq_pontuacao,"%d",pontuacao);
    fclose(arq_pontuacao);

    ///FONTS

    FONT *edessa_score = load_font("edessa_score.pcx",NULL,NULL);
    FONT *score_freeze = load_font("score_freeze.pcx",NULL,NULL);
    FONT *gameover = load_font("gameover.pcx",NULL,NULL);

    ///BOTÃO DE TRY

    botao* denovo = create_button(tentar,tentar,click_selecionado,click_ativado,700,240);


    ///INICIALIZAÇÃO DO LOOP DO JOGO

    while (!fechar_prog && !fechar_jogo)
    {
        while(tickrate>0)
        {
            ///INPUT

            if (key[KEY_ESC])
            {
                fechar_jogo=TRUE;
                estado_de_tela = ESCOLHA_NIVEL;
                key[KEY_ESC] = FALSE;
            }
            if(key[KEY_RIGHT])
            {
                car_x=car_x+velocidade_car;
            }
            else if(key[KEY_LEFT])
            {
                car_x=car_x-velocidade_car;
            }

            ///UPDATE DAS VARIAVEIS

            pneu_y=pneu_y+velocidade;
            pneu2_y=pneu2_y+velocidade;
            banana_y=banana_y+velocidade;
            banana2_y=banana2_y+velocidade;
            if (car_x>=619)
            {
                car_x=car_x-velocidade_car;
            }
            if (car_x<=125)
            {
                car_x=car_x+velocidade_car;
            }

            ///JOGO NA TELA

            draw_sprite(telafake,pista,0,0);
            draw_sprite(telafake,carro,car_x,car_y);
            if (tempo - marcador_obstaculos >= 600 && pneu_y>=600)
            {
                score++;
                pneu_x=rand()%619;
                while (pneu_x<125)
                    pneu_x=rand()%619;
                pneu_y=0;
                marcador_obstaculos=tempo;
            }
            if (tempo - marcador_obstaculos2 >=1000 && pneu2_y>=600)
            {
                score++;
                pneu2_x=rand()%619;
                while (pneu2_x<125)
                    pneu2_x=rand()%619;
                pneu2_y=0;
                marcador_obstaculos2=tempo;
            }
            if (tempo - marcador_obstaculos3 >=1400 && banana_y>=600)
            {
                score++;
                banana_x=rand()%619;
                while (banana_x<125)
                    banana_x=rand()%619;
                banana_y=0;
                marcador_obstaculos3=tempo;
            }
            if (tempo - marcador_obstaculos4 >= 1700 && banana2_y >= 600)
            {
                score++;
                banana2_x=rand()%619;
                while (banana2_x<125)
                    banana2_x=rand()%619;
                banana2_y=0;
                marcador_obstaculos4=tempo;
            }
            draw_sprite(telafake,pneu,pneu_x,pneu_y);
            draw_sprite(telafake,pneu,pneu2_x,pneu2_y);
            draw_sprite(telafake,banana,banana_x,banana_y);
            draw_sprite(telafake,banana,banana2_x,banana2_y);
            if (score < 0)
            {
                textprintf_centre_ex(telafake,edessa_score,45,250,makecol(255,255,255),-1,"Score:");
                textprintf_centre_ex(telafake,edessa_score,40,285,makecol(255,255,255),-1,"0");
            }
            else
            {
                textprintf_centre_ex(telafake,edessa_score,45,250,makecol(255,255,255),-1,"Score:");
                textprintf_centre_ex(telafake,edessa_score,40,285,makecol(255,255,255),-1,"%d",score);
            }
            if (score - aux_aumentar_dificuldade >=30)
            {
                if (velocidade < 9.5)
                {
                    velocidade=velocidade+0.5;
                    aux_aumentar_dificuldade=score;
                }
            }
            if (colisao_pixel_per_pixel(carro,car_x,car_y,pneu,pneu_x,pneu_y) == TRUE ||
                colisao_pixel_per_pixel(carro,car_x,car_y,pneu,pneu2_x,pneu2_y)== TRUE ||
                colisao_pixel_per_pixel(carro,car_x,car_y,banana,banana_x,banana_y) == TRUE ||
                colisao_pixel_per_pixel(carro,car_x,car_y,banana,banana2_x,banana2_y) == TRUE)
            {
                if (pontuacao <= score)
                {
                    arq_pontuacao = fopen("score_razoavel.txt","w");
                    fprintf(arq_pontuacao,"%d",score);
                    fclose(arq_pontuacao);
                }

                ///FREEZE-GAME


                while (!fechar_jogo)
                {
                    ///INPUT

                    button_input(denovo);
                    if (denovo->ativado)
                    {
                        fechar_jogo = TRUE;
                        estado_de_tela = FASE_RAZOAVEL;
                    }
                    else if (key[KEY_ESC])
                    {
                        key[KEY_ESC] = FALSE;
                        fechar_jogo = TRUE;
                        estado_de_tela = ESCOLHA_NIVEL;
                    }

                    ///DESENHO

                    draw_sprite(telafake,pista,0,0);
                    draw_sprite(telafake,carro,car_x,car_y);
                    draw_sprite(telafake,pneu,pneu_x,pneu_y);
                    draw_sprite(telafake,pneu,pneu2_x,pneu2_y);
                    draw_sprite(telafake,banana,banana_x,banana_y);
                    draw_sprite(telafake,banana,banana2_x,banana2_y);
                    textout_ex(telafake,gameover,"GAME OVER",240,50,makecol(255,255,255),-1);
                    textprintf_centre_ex(telafake,edessa_score,45,250,makecol(255,255,255),-1,"Score:");
                    textprintf_centre_ex(telafake,edessa_score,40,285,makecol(255,255,255),-1,"%d",score);
                    button_draw(denovo,telafake);
                    if (pontuacao <= score)
                    {
                        draw_sprite(telafake,best,700,80);
                        draw_sprite(telafake,atual,700,400);
                        textprintf_centre_ex(telafake,score_freeze,743,143,makecol(255,255,255),-1,"%d",score);
                        textprintf_centre_ex(telafake,score_freeze,743,450,makecol(255,255,255),-1,"%d",score);
                    }
                    else
                    {
                        draw_sprite(telafake,best,700,80);
                        draw_sprite(telafake,atual,700,400);
                        textprintf_centre_ex(telafake,score_freeze,743,143,makecol(255,255,255),-1,"%d",pontuacao);
                        textprintf_centre_ex(telafake,score_freeze,743,450,makecol(255,255,255),-1,"%d",score);
                    }
                    if (denovo->select)
                        draw_sprite(telafake,cursor_selecionado,mouse_x-9,mouse_y);
                    else
                        draw_sprite(telafake,cursor,mouse_x-7,mouse_y);
                    draw_sprite(screen,telafake,0,0);
                    clear(telafake);
                }
            }
            draw_sprite(screen,telafake,0,0);
            clear(telafake);
            tickrate=0;
        }
    }

    ///FINALIZAÇÃO

    destroy_bitmap(telafake);
    destroy_bitmap(pista);
    destroy_bitmap(carro);
    destroy_bitmap(pneu);
    destroy_bitmap(banana);
    destroy_bitmap(best);
    destroy_bitmap(atual);
    destroy_bitmap(cursor);
    destroy_bitmap(cursor_selecionado);
    destroy_bitmap(tentar);
    destroy_button(denovo);
    destroy_sample(click_ativado);
    destroy_sample(click_selecionado);
    destroy_font(edessa_score);
    destroy_font(score_freeze);
    destroy_font(gameover);
}


///FUNÇÃO PARA RODAR O JOGO ( FASE DIFICIL(CAPIROTO) )

void jogo_dificil(SAMPLE *fundo_dificil)
{
    ///INICIALIZAÇÃO DAS VARIAVEIS

    int fechar_jogo=FALSE;
    int aux_aumentar_dificuldade=0;
    float car_x=400,car_y=490;
    float velocidade=6.5;
    float velocidade_car=3.0;
    float pneu_x,pneu_y=600,agua_x,agua_y=600,banana_x,banana_y=600,banana2_x,banana2_y=600,agua2_x,agua2_y=600;
    int marcador_obstaculos,marcador_obstaculos2,marcador_obstaculos3,marcador_obstaculos4,marcador_obstaculos5;
    set_close_button_callback(fechar_programa);

    srand(time(NULL));
    marcador_obstaculos=tempo;
    marcador_obstaculos2=tempo;
    marcador_obstaculos3=tempo;
    marcador_obstaculos4=tempo;
    marcador_obstaculos5=tempo;
    score=-2;
    tickrate=0;

    ///CRIAÇÃO DOS BITMAPS

    BITMAP *telafake = create_bitmap(800, 600);
    BITMAP *pista = load_bitmap("imagens/pista_dificil.bmp",NULL);
    BITMAP *pneu = load_bitmap("imagens/pneu.bmp",NULL);
    BITMAP *carro = load_bitmap("imagens/carro_teste.bmp",NULL);
    BITMAP *banana = load_bitmap("imagens/casca_de_banana.bmp",NULL);
    BITMAP *agua = load_bitmap("imagens/water.bmp",NULL);
    BITMAP *best = load_bitmap("imagens/best.bmp",NULL);
    BITMAP *atual = load_bitmap("imagens/score.bmp",NULL);
    BITMAP *cursor = load_bitmap("imagens/cursor.bmp",NULL);
    BITMAP *cursor_selecionado = load_bitmap ("imagens/cursor_selecionado.bmp",NULL);
    BITMAP *tentar = load_bitmap("imagens/tentar.bmp",NULL);
    SAMPLE *click_selecionado = load_sample("sons/click_selecionado.wav");
    SAMPLE *click_ativado = load_sample("sons/escolhido.wav");

    ///CRIAÇÃO DOS ELEMENTOS NECESSARIOS PARA O SCORE

    int pontuacao;
    FILE *arq_pontuacao;
    arq_pontuacao = fopen("score_dificil.txt","r");
    fscanf(arq_pontuacao,"%d",&pontuacao);
    fclose(arq_pontuacao);
    arq_pontuacao = fopen("score_dificil.txt","w");
    fprintf(arq_pontuacao,"%d",pontuacao);
    fclose(arq_pontuacao);

    ///FONTS

    FONT *edessa_score = load_font("edessa_score.pcx",NULL,NULL);
    FONT *score_freeze = load_font("score_freeze.pcx",NULL,NULL);
    FONT *gameover = load_font("gameover.pcx",NULL,NULL);

    ///BOTÃO DE TRY

    botao* denovo = create_button(tentar,tentar,click_selecionado,click_ativado,700,240);

    ///INICIALIZAÇÃO DO LOOP DO JOGO


    while (!fechar_prog && !fechar_jogo)
    {
        while(tickrate>0)
        {
            ///INPUT

            if (key[KEY_ESC])
            {
                fechar_jogo=TRUE;
                estado_de_tela = ESCOLHA_NIVEL;
                key[KEY_ESC] = FALSE;
            }
            if(key[KEY_RIGHT])
            {
                car_x=car_x+velocidade_car;
            }
            else if(key[KEY_LEFT])
            {
                car_x=car_x-velocidade_car;
            }

            ///UPDATE DAS VARIAVEIS

            pneu_y=pneu_y+velocidade;
            agua_y=agua_y+velocidade;
            banana_y=banana_y+velocidade;
            banana2_y=banana2_y+velocidade;
            agua2_y=agua2_y+velocidade;
            if (car_x>=619)
            {
                car_x=car_x-velocidade_car;
            }
            if (car_x<=127)
            {
                car_x=car_x+velocidade_car;
            }

            ///JOGO NA TELA

            draw_sprite(telafake,pista,0,0);
            draw_sprite(telafake,carro,car_x,car_y);
            if (tempo - marcador_obstaculos >= 700 && pneu_y>=600)
            {
                score++;
                pneu_x=rand()%619;
                while (pneu_x<125)
                    pneu_x=rand()%619;
                pneu_y=0;
                marcador_obstaculos=tempo;
            }
            if (tempo - marcador_obstaculos2 >=1100 && agua_y>=600)
            {
                score++;
                agua_x=rand()%619;
                while (agua_x<125)
                    agua_x=rand()%619;
                agua_y=0;
                marcador_obstaculos2=tempo;
            }
            if (tempo - marcador_obstaculos3 >=1500 && banana_y>=600)
            {
                score++;
                banana_x=rand()%619;
                while (banana_x<125)
                    banana_x=rand()%619;
                banana_y=0;
                marcador_obstaculos3=tempo;
            }
            if (tempo - marcador_obstaculos4 >= 1800 && banana2_y >= 600)
            {
                score++;
                banana2_x=rand()%619;
                while (banana2_x<125)
                    banana2_x=rand()%619;
                banana2_y=0;
                marcador_obstaculos4=tempo;
            }
            if (tempo - marcador_obstaculos5 >=2100 && agua2_y >=600)
            {
                score++;
                agua2_x = rand()%619;
                while (agua2_x < 125)
                    agua2_x = rand()%619;
                agua2_y=0;
                marcador_obstaculos5=tempo;
            }
            draw_sprite(telafake,pneu,pneu_x,pneu_y);
            draw_sprite(telafake,agua,agua_x,agua_y);
            draw_sprite(telafake,banana,banana_x,banana_y);
            draw_sprite(telafake,banana,banana2_x,banana2_y);
            draw_sprite(telafake,agua,agua2_x,agua2_y);
            if (score < 0)
            {
                textprintf_centre_ex(telafake,edessa_score,45,250,makecol(255,255,255),-1,"Score:");
                textprintf_centre_ex(telafake,edessa_score,40,285,makecol(255,255,255),-1,"0");
            }
            else
            {
                textprintf_centre_ex(telafake,edessa_score,45,250,makecol(255,255,255),-1,"Score:");
                textprintf_centre_ex(telafake,edessa_score,40,285,makecol(255,255,255),-1,"%d",score);
            }
            if (score - aux_aumentar_dificuldade >=30)
            {
                if (velocidade < 9.5)
                {
                    velocidade=velocidade+0.5;
                    aux_aumentar_dificuldade=score;
                }
            }
            if (colisao_pixel_per_pixel(carro,car_x,car_y,pneu,pneu_x,pneu_y) == TRUE ||
                colisao_pixel_per_pixel(carro,car_x,car_y,agua,agua_x,agua_y)== TRUE ||
                colisao_pixel_per_pixel(carro,car_x,car_y,banana,banana_x,banana_y) == TRUE ||
                colisao_pixel_per_pixel(carro,car_x,car_y,banana,banana2_x,banana2_y) == TRUE ||
                colisao_pixel_per_pixel(carro,car_x,car_y,agua,agua2_x,agua2_y) == TRUE)
            {
                if (pontuacao <= score)
                {
                    arq_pontuacao = fopen("score_dificil.txt","w");
                    fprintf(arq_pontuacao,"%d",score);
                    fclose(arq_pontuacao);
                }

                ///FREEZE-GAME


                while (!fechar_jogo)
                {
                    ///INPUT

                    button_input(denovo);
                    if (denovo->ativado)
                    {
                        fechar_jogo = TRUE;
                        estado_de_tela = FASE_DIFICIL;
                    }
                    else if (key[KEY_ESC])
                    {
                        key[KEY_ESC] = FALSE;
                        fechar_jogo = TRUE;
                        estado_de_tela = ESCOLHA_NIVEL;
                    }

                    ///DESENHO

                    draw_sprite(telafake,pista,0,0);
                    draw_sprite(telafake,carro,car_x,car_y);
                    draw_sprite(telafake,pneu,pneu_x,pneu_y);
                    draw_sprite(telafake,agua,agua_x,agua_y);
                    draw_sprite(telafake,banana,banana_x,banana_y);
                    draw_sprite(telafake,banana,banana2_x,banana2_y);
                    draw_sprite(telafake,agua,agua2_x,agua2_y);
                    textout_ex(telafake,gameover,"GAME OVER",240,50,makecol(255,255,255),-1);
                    textprintf_centre_ex(telafake,edessa_score,45,250,makecol(255,255,255),-1,"Score:");
                    textprintf_centre_ex(telafake,edessa_score,40,285,makecol(255,255,255),-1,"%d",score);
                    button_draw(denovo,telafake);
                    if (pontuacao <= score)
                    {
                        draw_sprite(telafake,best,700,80);
                        draw_sprite(telafake,atual,700,400);
                        textprintf_centre_ex(telafake,score_freeze,743,143,makecol(255,255,255),-1,"%d",score);
                        textprintf_centre_ex(telafake,score_freeze,743,450,makecol(255,255,255),-1,"%d",score);
                    }
                    else
                    {
                        draw_sprite(telafake,best,700,80);
                        draw_sprite(telafake,atual,700,400);
                        textprintf_centre_ex(telafake,score_freeze,743,143,makecol(255,255,255),-1,"%d",pontuacao);
                        textprintf_centre_ex(telafake,score_freeze,743,450,makecol(255,255,255),-1,"%d",score);
                    }
                    if (denovo->select)
                        draw_sprite(telafake,cursor_selecionado,mouse_x-9,mouse_y);
                    else
                        draw_sprite(telafake,cursor,mouse_x-7,mouse_y);
                    draw_sprite(screen,telafake,0,0);
                    clear(telafake);
                }
            }
            draw_sprite(screen,telafake,0,0);
            clear(telafake);
            tickrate=0;
        }
    }

    ///FINALIZAÇÃO

    destroy_bitmap(telafake);
    destroy_bitmap(pista);
    destroy_bitmap(carro);
    destroy_bitmap(pneu);
    destroy_bitmap(banana);
    destroy_bitmap(agua);
    destroy_bitmap(best);
    destroy_bitmap(atual);
    destroy_bitmap(cursor);
    destroy_bitmap(cursor_selecionado);
    destroy_bitmap(tentar);
    destroy_button(denovo);
    destroy_sample(click_ativado);
    destroy_sample(click_selecionado);
    destroy_font(edessa_score);
    destroy_font(score_freeze);
    destroy_font(gameover);


}

///FUNÇÃO PRINCIPAL(EXECUTA TUDO,É A MÃE!)

int main()
{
    inicio();
    SAMPLE *musica_de_fundo = load_sample("sons/fundo.wav");
    SAMPLE *fundo_iniciante = load_sample("sons/fase_iniciante.wav");
    SAMPLE *fundo_facil = load_sample("sons/fase_facil.wav");
    SAMPLE *fundo_razoavel = load_sample("sons/fase_razoavel.wav");
    SAMPLE *fundo_dificil = load_sample("sons/fase_dificil.wav");

    play_sample(musica_de_fundo,255,128,1000,TRUE);

    while (!fechar_prog)
    {
        if (estado_de_tela == PREGAME)
        {
            stop_sample(fundo_iniciante);
            stop_sample(fundo_facil);
            stop_sample(fundo_razoavel);
            stop_sample(fundo_dificil);
            pregame();
        }
        else if (estado_de_tela == ESCOLHA_NIVEL)
        {
            stop_sample(fundo_iniciante);
            stop_sample(fundo_facil);
            stop_sample(fundo_razoavel);
            stop_sample(fundo_dificil);
            escolha_da_dificuldade();
        }
        else if (estado_de_tela == OLHE_OS_CREDITOS)
        {
            creditos();
        }
        else if (estado_de_tela == OLHE_AS_INSTRUCOES)
            instrus();
        else if (estado_de_tela == FASE_INICIANTE)
        {
            stop_sample(musica_de_fundo);
            stop_sample(fundo_iniciante);
            play_sample(fundo_iniciante,255,128,1000,TRUE);
            jogo_iniciante();
            play_sample(musica_de_fundo,255,128,1000,TRUE);
        }
        else if(estado_de_tela == FASE_FACIL)
        {
            stop_sample(musica_de_fundo);
            stop_sample(fundo_facil);
            play_sample(fundo_facil,255,128,1000,TRUE);
            jogo_facil();
            play_sample(musica_de_fundo,255,128,1000,TRUE);
        }
        else if(estado_de_tela == FASE_RAZOAVEL)
        {
            stop_sample(musica_de_fundo);
            stop_sample(fundo_razoavel);
            play_sample(fundo_razoavel,255,128,1000,TRUE);
            jogo_razoavel();
            play_sample(musica_de_fundo,255,128,1000,TRUE);
        }
        else if(estado_de_tela == FASE_DIFICIL)
        {
            stop_sample(musica_de_fundo);
            stop_sample(fundo_dificil);
            play_sample(fundo_dificil,255,128,1000,TRUE);
            jogo_dificil(fundo_dificil);
            play_sample(musica_de_fundo,255,128,1000,TRUE);
        }
    }


    ///FINALIZAÇÃO

    destroy_sample(musica_de_fundo);
    destroy_sample(fundo_iniciante);
    destroy_sample(fundo_facil);
    destroy_sample(fundo_razoavel);
    destroy_sample(fundo_dificil);
    return 0;
}
END_OF_MAIN();
