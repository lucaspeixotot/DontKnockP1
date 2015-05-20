#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

#include <allegro.h>
#include <stdlib.h>

typedef struct botao
{
    BITMAP *img;
    BITMAP *efeito;
    BITMAP *cursor;
    BITMAP *cursor_selecionado;
    SAMPLE *som;
    SAMPLE *som_clique;
    int x;
    int y;
    int select;
    int ativado;
    int bip;
}botao;

botao* create_button(BITMAP *imagem, BITMAP *selecao, SAMPLE *s_efeito, SAMPLE *s_clique, int pos_x, int pos_y);
void button_input(botao* pic);
void button_update(botao* pic);
void button_draw(botao* pic, BITMAP* telafake);
void destroy_button(botao* pic);

#endif
