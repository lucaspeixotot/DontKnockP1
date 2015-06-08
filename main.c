#include <allegro.h>
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
void incremento_score()
{
    score++;
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
enum{PREGAME,ESCOLHA_NIVEL,CREDITOS,FASE_INICIANTE,FASE_FACIL,FASE_RAZOAVEL,FASE_DIFICIL};
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
    score=0;
    install_int_ex(incremento_score,SECS_TO_TIMER(1));
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

    BITMAP *telafake = create_bitmap(SCREEN_W, SCREEN_H);
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
    SAMPLE *click_selecionado = load_sample("click_selecionado.wav");
    SAMPLE *click_ativado = load_sample("escolhido.wav");
    SAMPLE *musica_de_fundo = load_sample("fundo.wav");

    ///BOTOES

    botao* jogar = create_button(iniciar,iniciar_s,click_selecionado,click_ativado,-1,21);
    botao* instrucoes = create_button(instr,instr_s,click_selecionado,click_ativado,200,21);
    botao* sair = create_button(s,s_s,click_selecionado,click_ativado,601,21);
    botao* creditos = create_button(credi,credi_s,click_selecionado,click_ativado,400,21);

    while (!fechar_prog && !fechar_tela)
    {
        button_input(jogar);
        button_input(sair);
        button_input(creditos);
        button_input(instrucoes);
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
            estado_de_tela = CREDITOS;
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

void escolha_da_dificuldade()
{
    int fechar_escolha = FALSE;
    set_close_button_callback(fechar_programa);

    /// BITMAP PARA TELA

    BITMAP *telafake = create_bitmap(SCREEN_W,SCREEN_H);
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
    SAMPLE *click_selecionado = load_sample("click_selecionado.wav");
    SAMPLE *click_ativado = load_sample("escolhido.wav");

    ///BOTÕES

    botao* but_iniciante = create_button(iniciante,iniciante_s,click_selecionado,click_ativado,-1,21);
    botao* but_facil = create_button(facil,facil_s,click_selecionado,click_ativado,200,21);
    botao* but_razoavel = create_button(razoavel,razoavel_s,click_selecionado,click_ativado,400,21);
    botao* but_dificil = create_button(dificil,dificil_s,click_selecionado,click_ativado,601,21);

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

    BITMAP *telafake = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *but = load_bitmap("imagens/bot_img.bmp",NULL);
    BITMAP *but_s= load_bitmap("imagens/bot_select.bmp",NULL);
    SAMPLE *click_selecionado = load_sample("click_selecionado.wav");
    SAMPLE *click_ativado = load_sample("escolhido.wav");
    BITMAP *cursor = load_bitmap("imagens/cursor.bmp",NULL);
    BITMAP *cursor_selecionado = load_bitmap ("imagens/cursor_selecionado.bmp",NULL);

    ///BOTOES

    botao* sair = create_button(but,but_s,click_selecionado,click_ativado,SCREEN_W/2,SCREEN_H/2);

    while (!fechar_prog &&!cred)
    {
        button_input(sair);
        button_draw(sair,telafake);
        if (sair->select)
            draw_sprite(telafake,cursor_selecionado,mouse_x-9,mouse_y);
        else
            draw_sprite(telafake,cursor,mouse_x-7,mouse_y);
        if (sair->ativado)
        {
            sair->ativado=FALSE;
            cred=TRUE;
            estado_de_tela = PREGAME;
        }
        if (key[KEY_ESC])
        {
            cred=TRUE;
            estado_de_tela = PREGAME;
        }
        textout_ex(telafake,font,"VOLTAR",(SCREEN_W/2)+30,(SCREEN_H/2)+25,makecol(255,255,255),-1);
        draw_sprite(screen,telafake,0,0);
        clear(telafake);
    }

    ///FINALIZAÇÃO

    destroy_bitmap(telafake);
    destroy_bitmap(but);
    destroy_bitmap(but_s);
    destroy_bitmap(cursor);
    destroy_bitmap(cursor_selecionado);
    destroy_sample(click_ativado);
    destroy_sample(click_selecionado);
}


/// FUNÇÃO PARA RODAR O JOGO ( FASE INICIANTE )

void jogo_iniciante()
{
    ///INICIALIZAÇÃO DAS VARIAVEIS

    int fechar_jogo=FALSE;
    int scoreatual;
    int freeze;
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
    score=0;
    tickrate=0;

    ///INICIO DO JOGO E BITMAPS

    BITMAP *telafake = create_bitmap(SCREEN_W,SCREEN_H);
    BITMAP *pista = load_bitmap("imagens/pista10.bmp",NULL);
    BITMAP *pneu = load_bitmap("imagens/pneu.bmp",NULL);
    BITMAP *carro = load_bitmap("imagens/carro_teste.bmp",NULL);
    BITMAP *banana = load_bitmap("imagens/casca_de_banana.bmp",NULL);

    while (!fechar_jogo && !fechar_prog)
    {
        while (tickrate > 0)
        {
            ///INPUT
            if (key[KEY_ESC])
            {
                fechar_jogo=TRUE;
                estado_de_tela = PREGAME;
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
            if (tempo - marcador_obstaculos1 >= 300 && pneu_y >= 600)
            {
                pneu_x = rand()%619;
                while (pneu_x < 125)
                    pneu_x = rand()%619;
                pneu_y = 0;
                marcador_obstaculos1 = tempo;
            }
            if (tempo - marcador_obstaculos2 >= 700 && banana_y >= 600)
            {
                banana_x = rand()%619;
                while (banana_x < 125)
                    banana_x = rand()%619;
                banana_y = 0;
                marcador_obstaculos2 = tempo;
            }
            draw_sprite(telafake,pneu,pneu_x,pneu_y);
            draw_sprite(telafake,banana,banana_x,banana_y);
            textprintf_centre_ex(telafake,font,20,50,makecol(255,255,255),-1,"%d",score);
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
                freeze=tempo;
                scoreatual=score;
                while (tempo - freeze <=3000)
                {
                    draw_sprite(telafake,pista,0,0);
                    draw_sprite(telafake,carro,car_x,car_y);
                    draw_sprite(telafake,pneu,pneu_x,pneu_y);
                    draw_sprite(telafake,banana,banana_x,banana_y);
                    textout_ex(telafake,font,"BATEU MININU",(SCREEN_W/2)+30,(SCREEN_H/2)+25,makecol(255,255,255),-1);
                    textprintf_centre_ex(telafake,font,20,50,makecol(255,255,255),-1,"%d",scoreatual);
                    draw_sprite(screen,telafake,0,0);
                    clear(telafake);
                }
                fechar_jogo = TRUE;
                estado_de_tela = PREGAME;
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
}
///FUNÇÃO PARA RODAR O JOGO ( FASE FACIL)

void jogo_facil()
{
    ///INICIALIZAÇÃO DAS VARIAVEIS

    int fechar_jogo=FALSE;
    int aux_aumentar_dificuldade=0;
    int freeze;
    int scoreatual;
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
    score=0;
    tickrate=0;

    ///INICIO DO JOGO E BITMAPS

    BITMAP *telafake = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *pista = load_bitmap("imagens/pista_deserto.bmp",NULL);
    BITMAP *pneu = load_bitmap("imagens/pneu.bmp",NULL);
    BITMAP *carro = load_bitmap("imagens/carro_teste.bmp",NULL);
    BITMAP *banana = load_bitmap("imagens/casca_de_banana.bmp",NULL);
    BITMAP *agua = load_bitmap("imagens/water.bmp",NULL);

    while (!fechar_prog && !fechar_jogo)
    {
        while(tickrate>0)
        {
            ///INPUT
            if (key[KEY_ESC])
            {
                fechar_jogo=TRUE;
                estado_de_tela = PREGAME;
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
            if (tempo - marcador_obstaculos >= 400 && pneu_y>=600)
            {
                pneu_x=rand()%619;
                while (pneu_x<125)
                    pneu_x=rand()%619;
                pneu_y=0;
                marcador_obstaculos=tempo;
            }
            if (tempo - marcador_obstaculos2 >=800 && agua_y>=600)
            {
                agua_x=rand()%619;
                while (agua_x<125)
                    agua_x=rand()%619;
                agua_y=0;
                marcador_obstaculos2=tempo;
            }
            if (tempo - marcador_obstaculos3 >=1200 && banana_y>=600)
            {
                banana_x=rand()%619;
                while (banana_x<125)
                    banana_x=rand()%619;
                banana_y=0;
                marcador_obstaculos3=tempo;
            }
            draw_sprite(telafake,pneu,pneu_x,pneu_y);
            draw_sprite(telafake,agua,agua_x,agua_y);
            draw_sprite(telafake,banana,banana_x,banana_y);
            textprintf_centre_ex(telafake,font,20,50,makecol(255,255,255),-1,"%d",score);
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
                freeze=tempo;
                scoreatual=score;
                while (tempo - freeze <= 3000)
                {
                    draw_sprite(telafake,pista,0,0);
                    draw_sprite(telafake,carro,car_x,car_y);
                    draw_sprite(telafake,pneu,pneu_x,pneu_y);
                    draw_sprite(telafake,agua,agua_x,agua_y);
                    draw_sprite(telafake,banana,banana_x,banana_y);
                    textout_ex(telafake,font,"BATEU MININU",(SCREEN_W/2)+30,(SCREEN_H/2)+25,makecol(255,255,255),-1);
                    textprintf_centre_ex(telafake,font,20,50,makecol(255,255,255),-1,"%d",scoreatual);
                    draw_sprite(screen,telafake,0,0);
                    clear(telafake);
                }
                fechar_jogo = TRUE;
                estado_de_tela = PREGAME;
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

}

///FUNÇÃO PARA RODAR O JOGO ( FASE RAZOAVEL )

void jogo_razoavel()
{
    ///INICIALIZAÇÃO DAS VARIAVEIS

    int fechar_jogo=FALSE;
    int aux_aumentar_dificuldade=0;
    int freeze;
    int scoreatual;
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
    score=0;
    tickrate=0;

    ///INICIO DO JOGO E BITMAPS

    BITMAP *telafake = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *pista = load_bitmap("imagens/pista_metal.bmp",NULL);
    BITMAP *pneu = load_bitmap("imagens/pneu.bmp",NULL);
    BITMAP *carro = load_bitmap("imagens/carro_teste.bmp",NULL);
    BITMAP *banana = load_bitmap("imagens/casca_de_banana.bmp",NULL);

    while (!fechar_prog && !fechar_jogo)
    {
        while(tickrate>0)
        {
            ///INPUT
            if (key[KEY_ESC])
            {
                fechar_jogo=TRUE;
                estado_de_tela = PREGAME;
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
            if (tempo - marcador_obstaculos >= 400 && pneu_y>=600)
            {
                pneu_x=rand()%619;
                while (pneu_x<125)
                    pneu_x=rand()%619;
                pneu_y=0;
                marcador_obstaculos=tempo;
            }
            if (tempo - marcador_obstaculos2 >=800 && pneu2_y>=600)
            {
                pneu2_x=rand()%619;
                while (pneu2_x<125)
                    pneu2_x=rand()%619;
                pneu2_y=0;
                marcador_obstaculos2=tempo;
            }
            if (tempo - marcador_obstaculos3 >=1200 && banana_y>=600)
            {
                banana_x=rand()%619;
                while (banana_x<125)
                    banana_x=rand()%619;
                banana_y=0;
                marcador_obstaculos3=tempo;
            }
            if (tempo - marcador_obstaculos4 >= 1500 && banana2_y >= 600)
            {
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
            textprintf_centre_ex(telafake,font,20,50,makecol(255,255,255),-1,"%d",score);
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
                freeze=tempo;
                scoreatual=score;
                while (tempo - freeze <= 3000)
                {
                    draw_sprite(telafake,pista,0,0);
                    draw_sprite(telafake,carro,car_x,car_y);
                    draw_sprite(telafake,pneu,pneu_x,pneu_y);
                    draw_sprite(telafake,pneu,pneu2_x,pneu2_y);
                    draw_sprite(telafake,banana,banana_x,banana_y);
                    draw_sprite(telafake,banana,banana2_x,banana2_y);
                    textout_ex(telafake,font,"BATEU MININU",(SCREEN_W/2)+30,(SCREEN_H/2)+25,makecol(255,255,255),-1);
                    textprintf_centre_ex(telafake,font,20,50,makecol(255,255,255),-1,"%d",scoreatual);
                    draw_sprite(screen,telafake,0,0);
                    clear(telafake);
                }
                fechar_jogo = TRUE;
                estado_de_tela = PREGAME;
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
}


///FUNÇÃO PARA RODAR O JOGO ( FASE DIFICIL(CAPIROTO) )

void jogo_dificil()
{
    ///INICIALIZAÇÃO DAS VARIAVEIS

    int fechar_jogo=FALSE;
    int aux_aumentar_dificuldade=0;
    int freeze;
    int scoreatual;
    float car_x=400,car_y=490;
    float velocidade=6.5;
    float velocidade_car=3.0;
    float pneu_x,pneu_y=600,pneu2_x,pneu2_y=600,banana_x,banana_y=600,banana2_x,banana2_y=600,banana3_x,banana3_y=600;
    int marcador_obstaculos,marcador_obstaculos2,marcador_obstaculos3,marcador_obstaculos4,marcador_obstaculos5;
    set_close_button_callback(fechar_programa);

    srand(time(NULL));
    marcador_obstaculos=tempo;
    marcador_obstaculos2=tempo;
    marcador_obstaculos3=tempo;
    marcador_obstaculos4=tempo;
    marcador_obstaculos5=tempo;
    score=0;
    tickrate=0;

    ///INICIO DO JOGO E BITMAPS

    BITMAP *telafake = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *pista = load_bitmap("imagens/pista10.bmp",NULL);
    BITMAP *pneu = load_bitmap("imagens/pneu.bmp",NULL);
    BITMAP *carro = load_bitmap("imagens/carro_teste.bmp",NULL);
    BITMAP *banana = load_bitmap("imagens/casca_de_banana.bmp",NULL);

    while (!fechar_prog && !fechar_jogo)
    {
        while(tickrate>0)
        {
            ///INPUT
            if (key[KEY_ESC])
            {
                fechar_jogo=TRUE;
                estado_de_tela = PREGAME;
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
            banana3_y=banana3_y+velocidade;
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
            if (tempo - marcador_obstaculos >= 400 && pneu_y>=600)
            {
                pneu_x=rand()%619;
                while (pneu_x<125)
                    pneu_x=rand()%619;
                pneu_y=0;
                marcador_obstaculos=tempo;
            }
            if (tempo - marcador_obstaculos2 >=800 && pneu2_y>=600)
            {
                pneu2_x=rand()%619;
                while (pneu2_x<125)
                    pneu2_x=rand()%619;
                pneu2_y=0;
                marcador_obstaculos2=tempo;
            }
            if (tempo - marcador_obstaculos3 >=1200 && banana_y>=600)
            {
                banana_x=rand()%619;
                while (banana_x<125)
                    banana_x=rand()%619;
                banana_y=0;
                marcador_obstaculos3=tempo;
            }
            if (tempo - marcador_obstaculos4 >= 1500 && banana2_y >= 600)
            {
                banana2_x=rand()%619;
                while (banana2_x<125)
                    banana2_x=rand()%619;
                banana2_y=0;
                marcador_obstaculos4=tempo;
            }
            if (tempo - marcador_obstaculos5 >=1900 && banana3_y >=600)
            {
                banana3_x = rand()%619;
                while (banana3_x < 125)
                    banana3_x = rand()%619;
                banana3_y=0;
                marcador_obstaculos5=tempo;
            }
            draw_sprite(telafake,pneu,pneu_x,pneu_y);
            draw_sprite(telafake,pneu,pneu2_x,pneu2_y);
            draw_sprite(telafake,banana,banana_x,banana_y);
            draw_sprite(telafake,banana,banana2_x,banana2_y);
            draw_sprite(telafake,banana,banana3_x,banana3_y);
            textprintf_centre_ex(telafake,font,20,50,makecol(255,255,255),-1,"%d",score);
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
                colisao_pixel_per_pixel(carro,car_x,car_y,banana,banana2_x,banana2_y) == TRUE ||
                colisao_pixel_per_pixel(carro,car_x,car_y,banana,banana3_x,banana3_y) == TRUE)
            {
                freeze=tempo;
                scoreatual=score;
                while (tempo - freeze <= 3000)
                {
                    draw_sprite(telafake,pista,0,0);
                    draw_sprite(telafake,carro,car_x,car_y);
                    draw_sprite(telafake,pneu,pneu_x,pneu_y);
                    draw_sprite(telafake,pneu,pneu2_x,pneu2_y);
                    draw_sprite(telafake,banana,banana_x,banana_y);
                    draw_sprite(telafake,banana,banana2_x,banana2_y);
                    draw_sprite(telafake,banana,banana3_x,banana3_y);
                    textout_ex(telafake,font,"BATEU MININU",(SCREEN_W/2)+30,(SCREEN_H/2)+25,makecol(255,255,255),-1);
                    textprintf_centre_ex(telafake,font,20,50,makecol(255,255,255),-1,"%d",scoreatual);
                    draw_sprite(screen,telafake,0,0);
                    clear(telafake);
                }
                fechar_jogo = TRUE;
                estado_de_tela = PREGAME;
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

}

///FUNÇÃO PRINCIPAL(EXECUTA TUDO,É A MÃE!)

int main()
{
    inicio();
    SAMPLE *musica_de_fundo = load_sample("fundo.wav");
    SAMPLE *fundo_iniciante = load_sample("fase_iniciante.wav");
    SAMPLE *fundo_facil = load_sample("fase_facil.wav");
    SAMPLE *fundo_razoavel = load_sample("fase_razoavel.wav");
    SAMPLE *fundo_dificil = load_sample("fase_dificil.wav");

    while (!fechar_prog)
    {
        if (estado_de_tela == PREGAME)
        {
            stop_sample(musica_de_fundo);
            stop_sample(fundo_iniciante);
            stop_sample(fundo_facil);
            stop_sample(fundo_razoavel);
            stop_sample(fundo_dificil);
            play_sample(musica_de_fundo,255,128,1000,TRUE);
            pregame();
        }
        else if (estado_de_tela == ESCOLHA_NIVEL)
            escolha_da_dificuldade();
        else if (estado_de_tela == CREDITOS)
            creditos();
        else if (estado_de_tela == FASE_INICIANTE)
        {
            stop_sample(musica_de_fundo);
            play_sample(fundo_iniciante,255,128,1000,TRUE);
            jogo_iniciante();
        }
        else if(estado_de_tela == FASE_FACIL)
        {
            stop_sample(musica_de_fundo);
            play_sample(fundo_facil,255,128,1000,TRUE);
            jogo_facil();
        }
        else if(estado_de_tela == FASE_RAZOAVEL)
        {
            stop_sample(musica_de_fundo);
            play_sample(fundo_razoavel,255,128,1000,TRUE);
            jogo_razoavel();
        }
        else if(estado_de_tela == FASE_DIFICIL)
        {
            stop_sample(musica_de_fundo);
            play_sample(fundo_dificil,255,128,1000,TRUE);
            jogo_dificil();
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
