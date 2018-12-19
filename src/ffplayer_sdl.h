#ifndef FFPLAYER_SDL
#define FFPLAYER_SDL

#include "ffplayer_general.h"

int sdl2_showColor(const char *title,
                   int x, int y, int w, int h,
                   Uint8 red, Uint8 green, Uint8 blue, Uint8 a,
                   Uint32 flags)
{
    int quit = 1;
    SDL_Event event;
    SDL_Window *pWindow = NULL;
    SDL_Renderer *pRender = NULL;

    if(x<50 || y<50 || w<50 || h<50){
        SDL_Log("x/y/w/h < 50, invalide values.\n");
        return -1;
    }
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        SDL_Log("SDL_Init() error.\n");
        return -1;
    }
    pWindow = SDL_CreateWindow(title, x, y, w, h, flags);
    if(!pWindow){
        SDL_Log("SDL_CreateWindow() failed.\n");
        goto __EXIT;
    }

    pRender = SDL_CreateRenderer(pWindow, -1, 0);
    if(!pRender){
        SDL_Log("SDL_CreateRender() failed.\n");
        goto __WINDOWS;
    }

    SDL_SetRenderDrawColor(pRender, red, green, blue, a);
    SDL_RenderClear(pRender);
    SDL_RenderPresent(pRender);

    //SDL_Delay(3000);
    do{

        SDL_WaitEvent(&event);
        switch(event.type)
        {
        case SDL_QUIT:
            quit = 0;
            break;
        default:
            SDL_Log("event.type = %d\n", event.type);
            break;
        }
    } while(quit);

__WINDOWS:
    SDL_DestroyWindow(pWindow);
__EXIT:
    SDL_Quit();
    return 0;
}

int sdl2_showPic(char *filepath)
{
    using namespace std;
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    //The window we'll be rendering to
    SDL_Window* gWindow = NULL;

    //The surface contained by the window
    SDL_Surface* gScreenSurface = NULL;

    //The image we will load and show on the screen
    SDL_Surface* gHelloWorld = NULL;

    //首先初始化   初始化SD视频子系统
    if(SDL_Init(SDL_INIT_VIDEO)<0)
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }
    //创建窗口
    gWindow=SDL_CreateWindow("SHOW BMP",//窗口标题
                            SDL_WINDOWPOS_UNDEFINED,//窗口位置设置
                            SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_WIDTH,//窗口的宽度
                            SCREEN_HEIGHT,//窗口的高度
                            SDL_WINDOW_SHOWN//显示窗口
                            );
    if(gWindow==NULL)
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }
    //Use this function to get the SDL surface associated with the window.
    //获取窗口对应的surface
    gScreenSurface=SDL_GetWindowSurface(gWindow);

    //加载图片
    gHelloWorld = SDL_LoadBMP(filepath);//加载图片
    if( gHelloWorld == NULL )
    {
        printf( "Unable to load image %s! SDL Error: %s\n", "Hello_World.bmp", SDL_GetError() );
        return false;
    }
    //Use this function to perform a fast surface copy to a destination surface.
    //surface的快速复制
    //下面函数的参数分别为： SDL_Surface* src ,const SDL_Rect* srcrect , SDL_Surface* dst ,  SDL_Rect* dstrect
    SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
    SDL_UpdateWindowSurface(gWindow);//更新显示copy the window surface to the screen
    SDL_Delay(8000);//延时2000毫秒

    //释放内存
    SDL_FreeSurface( gHelloWorld );//释放空间
    gHelloWorld = NULL;

    SDL_DestroyWindow(gWindow);//销毁窗口
    gWindow = NULL ;

    SDL_Quit();//退出SDL
}

int sdl2_texture(const char *title,
                 int x, int y, int w, int h,
                 Uint8 red, Uint8 green, Uint8 blue, Uint8 a,
                 Uint32 flags)
{
    int quit = 1;
    SDL_Event event;
    SDL_Window *pWindow = NULL;
    SDL_Renderer *pRender = NULL;
    SDL_Texture *pTexture;

    if(x<480 || y<480 || w<480 || h<480){
        SDL_Log("x/y/w/h < 480, invalide values.\n");
        return -1;
    }
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        SDL_Log("SDL_Init() error.\n");
        return -1;
    }
    pWindow = SDL_CreateWindow(title, x, y, w, h, flags);
    if(!pWindow){
        SDL_Log("SDL_CreateWindow() failed.\n");
        goto __EXIT;
    }

    pRender = SDL_CreateRenderer(pWindow, -1, 0);
    if(!pRender){
        SDL_Log("SDL_CreateRender() failed.\n");
        goto __WINDOWS;
    }

    SDL_SetRenderDrawColor(pRender, red, green, blue, a);
    SDL_RenderClear(pRender);
    SDL_RenderPresent(pRender);

    pTexture = SDL_CreateTexture(pRender, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
                      w, h);
    if(!pTexture){
        SDL_Log("SDL_CtreateTexture() failed.\n");
        goto __RENDERER;
    }
    //SDL_Delay(3000);
    do{

        //SDL_WaitEvent(&event);
        SDL_PollEvent(&event);
        switch(event.type)
        {
        case SDL_QUIT:
            quit = 0;
            break;
        default:
            SDL_Log("event.type = %d\n", event.type);
            break;

        }

        SDL_Rect rect;
        rect.w = 50;
        rect.h = 50;
        rect.x = rand() % 480;
        rect.y = rand() % 480;
        // set Texture
        SDL_SetRenderTarget(pRender, pTexture);
        SDL_SetRenderDrawColor(pRender, 0, 0, 0, 0);
        SDL_RenderClear(pRender);
        // set rect
        SDL_RenderDrawRect(pRender, &rect);
        SDL_SetRenderDrawColor(pRender, 255, 0, 0, 0);
        SDL_RenderFillRect(pRender, &rect);

        SDL_SetRenderTarget(pRender, NULL);
        SDL_RenderCopy(pRender, pTexture, NULL, NULL);

        SDL_RenderPresent(pRender);
    } while(quit);

    SDL_DestroyTexture(pTexture);
__RENDERER:
    SDL_DestroyRenderer(pRender);
__WINDOWS:
    SDL_DestroyWindow(pWindow);
__EXIT:
    SDL_Quit();
    return 0;
}

#if 1 // YUVplayer with threads
#define BLOCK_SIZE 4096000
//event message
#define REFRESH_EVENT  (SDL_USEREVENT + 1)
#define QUIT_EVENT  (SDL_USEREVENT + 2)
int thread_exit = 0;

int refresh_video_timer(void *udata)
{
    thread_exit = 0;
    while(!thread_exit){
        SDL_Event event;
        event.type = REFRESH_EVENT;
        SDL_PushEvent(&event);
        SDL_Delay(40);
    }
    thread_exit = 0;

    //push quit event
    SDL_Event event;
    event.type = QUIT_EVENT;
    SDL_PushEvent(&event);

    return 0;
}
// sdl2_YUVplayer(640, 360, 608, 300);
int sdl2_YUVplayer(int w_width, int w_height,
                   int video_width, int video_height)
{

    FILE *video_fd = NULL;

    SDL_Event event;
    SDL_Rect rect;

    Uint32 pixformat = 0;

    SDL_Window *win = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;

    SDL_Thread *timer_thread = NULL;

    Uint8 *video_pos = NULL;
    Uint8 *video_end = NULL;

    unsigned int remain_len = 0;
    unsigned int video_buff_len = 0;
    unsigned int blank_space_len = 0;
    Uint8 video_buf[BLOCK_SIZE];

    const char *path = "../testData/test.yuv";

    const unsigned int yuv_frame_len = video_width * video_height * 12 / 8;

    //initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO)) {
        fprintf( stderr, "Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }

    //creat window from SDL
    win = SDL_CreateWindow("YUV Player",
                           SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED,
                           w_width, w_height,
                           SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if(!win) {
        fprintf(stderr, "Failed to create window, %s\n",SDL_GetError());
        goto __FAIL;
    }

    renderer = SDL_CreateRenderer(win, -1, 0);

    //IYUV: Y + U + V  (3 planes)
    //YV12: Y + V + U  (3 planes)
    pixformat= SDL_PIXELFORMAT_IYUV;

    //create texture for render
    texture = SDL_CreateTexture(renderer,
                                pixformat,
                                SDL_TEXTUREACCESS_STREAMING,
                                video_width,
                                video_height);

    //open yuv file
    video_fd = fopen(path, "r");
    if( !video_fd ){
        fprintf(stderr, "Failed to open yuv file\n");
        goto __FAIL;
    }

    //read block data
    if((video_buff_len = fread(video_buf, 1, BLOCK_SIZE, video_fd)) <= 0){
        fprintf(stderr, "Failed to read data from yuv file!\n");
        goto __FAIL;
    }

    //set video positon
    video_pos = video_buf;
    video_end = video_buf + video_buff_len;
    blank_space_len = BLOCK_SIZE - video_buff_len;

    timer_thread = SDL_CreateThread(refresh_video_timer,
                                    NULL,
                                    NULL);

    do {
        //Wait
        SDL_WaitEvent(&event);
        if(event.type==REFRESH_EVENT){
            //not enought data to render
            if((video_pos + yuv_frame_len) > video_end){

                //have remain data, but there isn't space
                remain_len = video_end - video_pos;
                if(remain_len && !blank_space_len) {
                    //copy data to header of buffer
                    memcpy(video_buf, video_pos, remain_len);

                    blank_space_len = BLOCK_SIZE - remain_len;
                    video_pos = video_buf;
                    video_end = video_buf + remain_len;
                }

                //at the end of buffer, so rotate to header of buffer
                if(video_end == (video_buf + BLOCK_SIZE)){
                    video_pos = video_buf;
                    video_end = video_buf;
                    blank_space_len = BLOCK_SIZE;
                }

                //read data from yuv file to buffer
                if((video_buff_len = fread(video_end, 1, blank_space_len, video_fd)) <= 0){
                    fprintf(stderr, "eof, exit thread!");
                    thread_exit = 1;
                    continue;// to wait event for exiting
                }

                //reset video_end
                video_end += video_buff_len;
                blank_space_len -= video_buff_len;
                printf("not enought data: pos:%p, video_end:%p, blank_space_len:%d\n", video_pos, video_end, blank_space_len);
            }

            SDL_UpdateTexture( texture, NULL, video_pos, video_width);

            //FIX: If window is resize
            rect.x = 0;
            rect.y = 0;
            rect.w = w_width;
            rect.h = w_height;

            SDL_RenderClear( renderer );
            SDL_RenderCopy( renderer, texture, NULL, &rect);
            SDL_RenderPresent( renderer );

            printf("not enought data: pos:%p, video_end:%p, blank_space_len:%d\n", video_pos, video_end, blank_space_len);
            video_pos += yuv_frame_len;

        }else if(event.type==SDL_WINDOWEVENT){
            //If Resize
            SDL_GetWindowSize(win, &w_width, &w_height);
        }else if(event.type==SDL_QUIT){
            thread_exit=1;
        }else if(event.type==QUIT_EVENT){
            break;
        }
    }while ( 1 );

__FAIL:

    //close file
    if(video_fd){
        fclose(video_fd);
    }

    SDL_Quit();

    return 0;
}
#endif












#endif // FFPLAYER_SDL

