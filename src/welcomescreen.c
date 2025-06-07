#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_video.h>
#include <stdio.h>
static ALLEGRO_DISPLAY *screen;
static char const *filename;
ALLEGRO_EVENT_QUEUE *queue;
ALLEGRO_EVENT event;
ALLEGRO_TIMER *timer;
ALLEGRO_VIDEO *video;

void video_display(ALLEGRO_VIDEO *video) 
{
    ALLEGRO_BITMAP *frame = al_get_video_frame(video);
    if ( !frame ) return;
    al_draw_scaled_bitmap(frame,
                          0, 0,
                          al_get_bitmap_width(frame),
                          al_get_bitmap_height(frame),
                          0, 0,
                          al_get_display_width(screen),
                          al_get_display_height(screen), 0);
    al_flip_display();
}

void init_video()
{
    al_init();
    al_init_video_addon();
    al_install_audio();
    timer = al_create_timer(1.0 / 60);
    screen = al_create_display(1024, 760);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    filename = "resources/sprites/wellcomescreenshot.ogv";
    video = al_open_video(filename);
    queue = al_create_event_queue();
    ALLEGRO_EVENT_SOURCE *temp = al_get_video_event_source(video);
    al_register_event_source(queue, temp);
    al_register_event_source(queue, al_get_display_event_source(screen));
    al_register_event_source(queue, al_get_timer_event_source(timer));
}

void video_begin()
{
    al_reserve_samples(1);
    al_start_video(video, al_get_default_mixer());
    al_start_timer(timer);
}

void destroy_video()
{
    al_destroy_display(screen);
    al_destroy_event_queue(queue);
}

int main() 
{
    init_video();
    video_begin();
    while( 1 )
    {
        al_wait_for_event(queue, &event);
        if( event.type == ALLEGRO_EVENT_TIMER ) 
        {
            video_display(video);
        } 
        else if( event.type == ALLEGRO_EVENT_DISPLAY_CLOSE ) 
        {
            al_close_video(video);
            break;
        } 
        else if( event.type == ALLEGRO_EVENT_VIDEO_FINISHED ) 
        {
            break;
        }
    }
    destroy_video();
    return 0;
}
