#include "botoes.h"

botao* create_button(BITMAP *imagem, BITMAP *selecao, SAMPLE *s_efeito, SAMPLE *s_clique, int pos_x, int pos_y)
{
    botao* pic=(botao*) malloc(sizeof(botao));

    pic->img=imagem;
    pic->efeito=selecao;
    pic->som=s_efeito;
    pic->som_clique=s_clique;
    pic->x=pos_x;
    pic->y=pos_y;
    pic->select=FALSE;
    pic->ativado=FALSE;
    pic->bip=0;

    return pic;
}
void button_input(botao* pic)
{
    if (mouse_x > pic->x && mouse_x < pic->x + pic->img->w &&
        mouse_y > pic->y && mouse_y < pic->y + pic->img->h &&
        getpixel(pic->img,mouse_x - pic->x, mouse_y - pic->y) !=makecol(255,0,255))
    {
        pic->select=TRUE;
        pic->bip++;
        if (pic->bip==1)
            play_sample(pic->som, 255, 128, 1000, FALSE);
        if (mouse_b==1)
        {
            play_sample(pic->som_clique,255,128,2000,FALSE);
            mouse_b=0;
            if (pic->ativado==TRUE)
            {
                pic->ativado=FALSE;
            }
            else
            {
                pic->ativado=TRUE;
            }
        }
    }
    else
    {
        pic->bip=0;
        pic->select=FALSE;
    }
}
void button_draw(botao* pic, BITMAP* telafake)
{
    draw_sprite(telafake,pic->img,pic->x,pic->y);
    if (pic->select)
        draw_sprite(telafake,pic->efeito,pic->x,pic->y);
}
void destroy_button(botao* pic)
{
    free(pic);
}
