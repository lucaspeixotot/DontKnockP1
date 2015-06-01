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
enum{PREGAME,JOGO,CREDITOS};
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

/// COLISOES

void colisoes(int freeze, float pneu_x, float pneu_y, float pneu2_x, float pneu2_y,
              float pneu3_x, float pneu3_y, float car_x, float car_y, int *end_game,int scoreatual)
{
    ///BITMAPS PARA A PARALIZAÇÃO

    BITMAP *telafake = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *pista = load_bitmap("imagens/pistafeita.bmp",NULL);
    BITMAP *obs = load_bitmap("imagens/pneu.bmp",NULL);
    BITMAP *car = load_bitmap("imagens/carro_teste.bmp",NULL);
    BITMAP *banana = load_bitmap("imagens/casca_de_banana.bmp",NULL);

    ///PARALIZAÇÃO


    while (tempo-freeze<=3000)
    {
        draw_sprite(telafake,pista,0,0);
        draw_sprite(telafake,car,car_x,car_y);
        draw_sprite(telafake,obs,pneu_x,pneu_y);
        draw_sprite(telafake,obs,pneu2_x,pneu2_y);
        draw_sprite(telafake,banana,pneu3_x,pneu3_y);
        textout_ex(telafake,font,"BATEU MININU",(SCREEN_W/2)+30,(SCREEN_H/2)+25,makecol(255,255,255),-1);
        textprintf_centre_ex(telafake,font,20,50,makecol(255,255,255),-1,"%d",scoreatual);
        draw_sprite(screen,telafake,0,0);
        clear(telafake);
    }
    *end_game=TRUE;
    estado_de_tela = PREGAME;


    /// FINALIZAÇÃO

    destroy_bitmap(telafake);
    destroy_bitmap(pista);
    destroy_bitmap(car);
    destroy_bitmap(obs);
}


///FUNÇÃO PARA TELA INICIAL


void pregame()
{
    int fechar_tela = FALSE;
    set_close_button_callback(fechar_programa);

    //BITMAP P/ A TELA E SONS

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
            estado_de_tela = JOGO;
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


///FUNÇÃO PARA RODAR O JOGO ( FASE 1 e FASE 2 )

void jogo()
{
    ///INICIALIZAÇÃO DAS VARIAVEIS

    int fechar_jogo=FALSE;
    int freeze;
    int scoreatual;
    float car_x=400,car_y=490;
    float velocidade=5.0;
    float velocidade_car=3.0;
    float pneu_x,pneu_y=600,pneu2_x,pneu2_y=600,pneu3_x,pneu3_y=600;
    int marcador_obstaculos,marcador_obstaculos2,marcador_obstaculos3;
    set_close_button_callback(fechar_programa);

    ///INICIO DO JOGO E BITMAPS

    BITMAP *telafake = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *pista = load_bitmap("imagens/pistafeita.bmp",NULL);
    BITMAP *pneu = load_bitmap("imagens/pneu.bmp",NULL);
    BITMAP *carro = load_bitmap("imagens/carro_teste.bmp",NULL);
    BITMAP *banana = load_bitmap("imagens/casca_de_banana.bmp",NULL);

    srand(time(NULL));
    marcador_obstaculos=tempo;
    marcador_obstaculos2=tempo;
    marcador_obstaculos3=tempo;
    score=0;
    tickrate=0;

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
            pneu3_y=pneu3_y+velocidade;
            if (car_x>=700)
            {
                    car_x=car_x-velocidade_car;
            }
            if (car_x<=50)
            {
                car_x=car_x+velocidade_car;
            }

            ///JOGO NA TELA

            draw_sprite(telafake,pista,0,0);
            draw_sprite(telafake,carro,car_x,car_y);
            if (tempo - marcador_obstaculos >= 400 && pneu_y>=600)
            {
                pneu_x=rand()%700;
                while (pneu_x<47)
                    pneu_x=rand()%700;
                pneu_y=0;
                marcador_obstaculos=tempo;
            }
            if (tempo - marcador_obstaculos2 >=800 && pneu2_y>=600)
            {
                pneu2_x=rand()%700;
                while (pneu2_x<47)
                    pneu2_x=rand()%700;
                pneu2_y=0;
                marcador_obstaculos2=tempo;
            }
            if (tempo - marcador_obstaculos3 >=1200 && pneu3_y>=600)
            {
                pneu3_x=rand()%700;
                while (pneu3_x<47)
                    pneu3_x=rand()%700;
                pneu3_y=0;
                marcador_obstaculos3=tempo;
            }
            draw_sprite(telafake,pneu,pneu_x,pneu_y);
            draw_sprite(telafake,pneu,pneu2_x,pneu2_y);
            draw_sprite(telafake,banana,pneu3_x,pneu3_y);
            textprintf_centre_ex(telafake,font,20,50,makecol(255,255,255),-1,"%d",score);
            if (score == 55)
            {
                velocidade=7.5;
                /// Aqui terá duas funções, uma para mudar para a tela mudança_de_fase, pra informar que a pessoa está indo para
                /// a fase 2. E o recomeço dessa mesma função, porém com um fundo diferente e com os objetos caindo mais rápido.
            }
            else if (colisao_pixel_per_pixel(carro,car_x,car_y,pneu,pneu_x,pneu_y) == TRUE ||
                colisao_pixel_per_pixel(carro,car_x,car_y,pneu,pneu2_x,pneu2_y)== TRUE ||
                colisao_pixel_per_pixel(carro,car_x,car_y,banana,pneu3_x,pneu3_y) == TRUE)
            {
                freeze=tempo;
                scoreatual=score;
                colisoes(freeze,pneu_x,pneu_y,pneu2_x,pneu2_y, pneu3_x, pneu3_y,car_x,car_y, &fechar_jogo, scoreatual);
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
    SAMPLE *musica_de_fundo = load_sample("fundo2.wav");
    SAMPLE *fundo_game = load_sample("fundo.wav");

    while (!fechar_prog)
    {
        if (estado_de_tela == PREGAME){
            stop_sample(fundo_game);
            play_sample(musica_de_fundo,255,128,1000,TRUE);
            pregame();
        }
        else if (estado_de_tela == CREDITOS)
            creditos();
        else if(estado_de_tela == JOGO)
        {
            stop_sample(musica_de_fundo);
            play_sample(fundo_game,255,128,1000,TRUE);
            jogo();
        }
    }
    destroy_sample(musica_de_fundo);
    destroy_sample(fundo_game);
    return 0;
}
END_OF_MAIN();
