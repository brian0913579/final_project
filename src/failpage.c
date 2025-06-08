#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_video.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h> 
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
static ALLEGRO_DISPLAY *screen;
static char const *filename;
ALLEGRO_EVENT_QUEUE *queue;
ALLEGRO_EVENT event;
ALLEGRO_TIMER *timer;
ALLEGRO_VIDEO *video;
ALLEGRO_BITMAP *img; 
ALLEGRO_BITMAP *img_s;
ALLEGRO_FONT *font; 

void video_display(ALLEGRO_VIDEO *video,int* idx_end) {
    ALLEGRO_BITMAP *frame = al_get_video_frame(video);
    // check if we get the frame successfully
    // Note the this code is necessary
    if ( !frame )
        return;
    al_draw_scaled_bitmap(frame,
                          // the rescale position of the frame
                          0, 0,
                          // the width and height you want to rescale
                          al_get_bitmap_width(frame),
                          al_get_bitmap_height(frame),
                          0, 0,
                          1280,720,
                           0);
    //星星累積數還要從其他地方引入
    int number_star=9;
    img = al_load_bitmap("star_0_0.png"); 
    for (int i=0;i<number_star;i++)
    {
        al_draw_scaled_bitmap(img,
                                0, 0,// source start (top-left of image)
                                al_get_bitmap_width(img),// original width
                                al_get_bitmap_height(img),// original height
                                1050-i*35, 250,// destination position on screen
                                35, 35,// scaled width and height
                                0// no special flags
                                );
    }
    int idx=*idx_end; 
    //初始設置為指向Menu (Menu=0, Exit=1) //方框左右移動當作selector
    img_s = al_load_bitmap("button_0.png"); // ANS: load image
    al_draw_scaled_bitmap(img_s,
                            0, 0,// source start (top-left of image)
                            al_get_bitmap_width(img_s),// original width
                            al_get_bitmap_height(img_s),// original height
                            657+idx*227, 506,// destination position on screen
                            180, 65,// scaled width and height
                            0// no special flags
                            );
    //輸出的時間還需要從其他地方引入
    font = al_load_ttf_font("13.ttf", 55, 0); // ANS: load font
    al_draw_text(font, al_map_rgb(0,0,0), 1008, 312, ALLEGRO_ALIGN_CENTRE,  "04:33.76"); // ANS: draw some text
    al_flip_display();
}

void init_video(){
    al_init();
    al_init_video_addon();
    al_install_audio();
    timer = al_create_timer(1.0 / 60);
    screen = al_create_display(1280, 720);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    filename = "fail.ogv";
    video = al_open_video(filename);
    queue = al_create_event_queue();
    ALLEGRO_EVENT_SOURCE *temp = al_get_video_event_source(video);
    al_register_event_source(queue, temp);
    al_register_event_source(queue, al_get_display_event_source(screen));
    al_register_event_source(queue, al_get_timer_event_source(timer));

}
void video_begin(){
    al_reserve_samples(1);
    al_start_video(video, al_get_default_mixer());
    al_start_timer(timer);
}
void destroy_video(){
    al_destroy_display(screen);
    al_destroy_event_queue(queue);
}


void game_init();
void game_destroy();

void game_init() {
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
}
void game_destroy() {
    al_destroy_bitmap(img); 
    al_destroy_bitmap(img_s);
    al_destroy_font(font); // destroy font
}



int main() {
    game_init();
    int idx_end=0;
    init_video();
    video_begin();
    while( 1 ){
        al_wait_for_event(queue, &event);
        if( event.type == ALLEGRO_EVENT_TIMER ) {
            video_display(video,&idx_end);
        } else if( event.type == ALLEGRO_EVENT_DISPLAY_CLOSE ) {
            al_close_video(video);
            break;
        } else if( event.type == ALLEGRO_EVENT_VIDEO_FINISHED ) {
            break;
        }
    }
    destroy_video();
    game_destroy();
    return 0;
}



//input.c
/*
void handle_menu_input(Game* game, ALLEGRO_EVENT* event, int * idx_end) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {        
        switch (game->state) {
            case VICTORY: 
                switch (event.keyboard.keycode) {
                    case ALLEGRO_KEY_RIGHT:
                        *idx_end = (*idx_end + 1) % 2;
                        break;
                    case ALLEGRO_KEY_LEFT:
                        *idx_end = (*idx_end + 1) % 2;
                        break;
                    case ALLEGRO_KEY_ENTER:
                        if (*idx_end==0) {
                            game->state = MAIN_MENU; 
                        } else if (*idx_end==1) {
                            game->running = false;
                        }
                        break;
                }
                return;
        }
    }
}
*/
