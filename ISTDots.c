#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<math.h>
#include<string.h>
//for xor operand
#include<iso646.h>


#define MAX(a,b)    (((a)>(b))?(a):(b))
#define M_PI 3.14159265
#define STRING_SIZE 100       // max size for some strings
#define TABLE_SIZE 850        // main game space size
#define LEFT_BAR_SIZE 150     // left white bar size
#define WINDOW_POSX 200       // initial position of the window: x
#define WINDOW_POSY 200       // initial position of the window: y
#define SQUARE_SEPARATOR 8    // square separator in px
#define BOARD_SIZE_PER 0.7f   // board size in % wrt to table size
#define MAX_BOARD_POS 15      // maximum size of the board
#define MAX_COLORS 5
#define MARGIN 5
#define CROSSING_POINT 501    // adress of current_selected[][] in wich is saved point were the square is closed
#define BUFFER_SIZE 100  //size of buffer read from stdin


// declaration of the functions related to graphical issues
void InitEverything(int , int , TTF_Font **, TTF_Font **, SDL_Surface **, SDL_Window ** , SDL_Renderer ** );
void InitSDL();
void InitFont();
SDL_Window* CreateWindow(int , int );
SDL_Renderer* CreateRenderer(int , int , SDL_Window *);
int RenderText(int, int, const char *, TTF_Font *, SDL_Color *, SDL_Renderer *);
int RenderLogo(int, int, SDL_Surface *, SDL_Renderer *);
int RenderTable(int, int, int [], TTF_Font *, SDL_Surface **, SDL_Renderer *, char[]);
void ProcessMouseEvent(int , int , int [], int , int *, int *, int, int );
void RenderPoints(int [][MAX_BOARD_POS], int, int, int [], int, SDL_Renderer *);
void RenderStats( SDL_Renderer *, TTF_Font *, int [], int , int, char[], int, int [TABLE_SIZE][3], int, int);
void filledCircleRGBA(SDL_Renderer * , int , int , int , int , int , int );

//definition of the functions that give a purpose to the graphics
void ParamReading(int *, int *, char[] , int *, int[]);
void TestPrints(int, int , char[], int, int[]);
void InitialBoard(int[][MAX_BOARD_POS], int, int, int);
void CurrentMove(int, int,int[TABLE_SIZE][3],int[][MAX_BOARD_POS], int *, int *);
int  YNconnect(int*, int[TABLE_SIZE][3],int[][MAX_BOARD_POS], int, int, int*);
int DotToCoordinate(int,  int [], int, int);
void MovePoints(int [][MAX_BOARD_POS], int [TABLE_SIZE][3], int , int , int, int);
void FreshNewPoints(int [][MAX_BOARD_POS], int , int,int);
void CleanC_S(int[][3], int);
void SinalizePointsToBeDeleted(int [][MAX_BOARD_POS], int [TABLE_SIZE][3], int , int, int, int, int[6] );
void RadialDotCheck(int [][MAX_BOARD_POS], int, int, int [TABLE_SIZE][3], int[6]);
void SetGameStats(int[6], int[6]);
int RunOutOfPlays(int [][MAX_BOARD_POS], int, int);
void InfoDisplayer(int, SDL_Renderer *, TTF_Font *);
void VictoryOrDefeat(int[], int[], int *, int);

//advanced feature
void Undo(int [][MAX_BOARD_POS],int  [][MAX_BOARD_POS], int[6], int [6], int, int, int);
void CloneForUndo(int [][MAX_BOARD_POS],int  [][MAX_BOARD_POS], int[6], int [6], int, int, int);


void HidePointsToBeRemoved(int [], int, SDL_Renderer *,int [][MAX_BOARD_POS], int, int, int [TABLE_SIZE][3], int);
void RenderPath(SDL_Renderer *, int, int[TABLE_SIZE][3],int, int []);
void RemovePoints(int [], int, SDL_Renderer *, int, int[TABLE_SIZE][3]);
// definition of some strings: they cannot be changed when the program is executed !
const char myName[] = "Joao Figueiredo";
const char myNumber[] = "IST190108";
const int colors[3][MAX_COLORS] = {{140, 231, 140, 231, 153},{189, 90, 231, 222, 90},{254, 74, 148, 41, 180}};
/**
 * main function: entry point of the program
 * only to invoke other functions !
 */
int main( void ){
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *serif = NULL;
    TTF_Font *sans = NULL ;
    SDL_Surface *imgs[2];
    SDL_Event event;
    int delay = 30;
    int quit = 0;
    int width = (TABLE_SIZE + LEFT_BAR_SIZE);
    int height = TABLE_SIZE;
    int square_size_px = 0, board_size_px[2] = {0};
    int board_pos_x = 0, board_pos_y = 0;
    int board[MAX_BOARD_POS][MAX_BOARD_POS] = {{0}};
    int pt_x = 0, pt_y = 0;
    char player_name[BUFFER_SIZE];
    int ncolors=MAX_COLORS;
    int current_selected[TABLE_SIZE][3]= {{-1}}, pressed=0,num_selected=0; //(in order)coordinates and color of dots selected , state of the mouse button, number of dots selected
    int state=0; // state0 -reading or waiting, state1 - removing dots, state2 - adding dots
    int flag_square=0; //1 if a square is done
    int game_stats[6]= {0}, game_targets[6]={0}; //last cell - nº of plays; others - dots blown per color(color code mantained)
    char convToDisplay[3];
    int games_init_counter=0;
    int undo_board[MAX_BOARD_POS][MAX_BOARD_POS]={{0}}, undo_game_stats[6]={0};

    // initialize graphics
    ParamReading(&board_pos_x, &board_pos_y, player_name ,&ncolors, game_targets);
    InitialBoard(board, board_pos_x, board_pos_y, ncolors);


    InitEverything(width, height, &serif, &sans, imgs, &window, &renderer);

    TestPrints(board_pos_x,board_pos_y, player_name, ncolors, game_targets);

    state=-1;

    while( quit == 0 )
    {
        // while there's events to handle
        while( SDL_PollEvent( &event ) ){
            if( event.type == SDL_QUIT ){
                quit=1;
            }

            else if ( event.type == SDL_KEYDOWN ){
                switch ( event.key.keysym.sym ){
                    case SDLK_n:
                        InitialBoard(board, board_pos_x, board_pos_y, ncolors);
                        SetGameStats(game_stats, game_targets);
                        state=0;
                        games_init_counter++;
                        break;
                    case SDLK_q:
                        quit=1;
                    case SDLK_u:
                        if (state==0) Undo(board, undo_board, game_stats, undo_game_stats, board_pos_x, board_pos_y, ncolors);
                    default:
                        break;
                }
            }

            else if ( event.type == SDL_MOUSEBUTTONDOWN ){
                ProcessMouseEvent(event.button.x, event.button.y, board_size_px, square_size_px, &pt_x, &pt_y, board_pos_x, board_pos_y);
                //printf("Button down: %d %d\n", pt_x, pt_y);
                if (state==0){
                    pressed=1;
                    CloneForUndo(board, undo_board, game_stats, undo_game_stats, board_pos_x, board_pos_y, ncolors);
                    CurrentMove(pt_x, pt_y,current_selected,board,&num_selected, &flag_square);
                }
            }

            else if ( event.type == SDL_MOUSEBUTTONUP ){
                ProcessMouseEvent(event.button.x, event.button.y, board_size_px, square_size_px, &pt_x, &pt_y, board_pos_x, board_pos_y);
                //printf("Button up: %d %d\n", pt_x, pt_y);
                if (state==0) state=1;
                pressed=0;
            }

            else if ( event.type == SDL_MOUSEMOTION ){
            //    printf("NUMEROCORES[%d]\n",ncolors );
                ProcessMouseEvent(event.button.x, event.button.y, board_size_px, square_size_px, &pt_x, &pt_y,board_pos_x, board_pos_y);
                //printf("Moving Mouse: %d %d\n", pt_x, pt_y);
                if (pressed==1) CurrentMove(pt_x, pt_y,current_selected, board, &num_selected, &flag_square);

            }
        }

        // render game table
        square_size_px = RenderTable( board_pos_x, board_pos_y, board_size_px, serif, imgs, renderer, player_name);
        // render board
        RenderPoints(board, board_pos_x, board_pos_y, board_size_px, square_size_px, renderer);

        if (state==2){

            //as state==2 this function paint the spaces left blank after the descent of the points
            HidePointsToBeRemoved(board_size_px, square_size_px, renderer,board, board_pos_x, board_pos_y, current_selected, state);
            //fills with a color number the negative cells of the matrix
            FreshNewPoints(board, board_pos_x,board_pos_y, ncolors);
            SDL_Delay(delay);
            state=0;
            if (num_selected>1) game_stats[5]--;
            for (int o=0; o<6;o++){
                printf("game_stats __%d__[%d]\n",o, game_stats[o] );
            }
            num_selected=0;
        }

        if (state==1) {
            //Set points selected to -1. if a square is made sets the points of the square to -2, those inside to -3, and all others of the same color to -1
            SinalizePointsToBeDeleted(board, current_selected, num_selected, board_pos_x, board_pos_y,flag_square, game_stats);
            //paints the dots about to be removed with the background color
            HidePointsToBeRemoved(board_size_px, square_size_px, renderer,board, board_pos_x, board_pos_y, current_selected, state);
            //simulates gravity
            MovePoints(board, current_selected, num_selected, board_pos_x, board_pos_y,flag_square);
            state++;
            SDL_Delay(delay);
        }

        if (state<2){
            RenderPath(renderer, num_selected,current_selected,square_size_px, board_size_px);

        }

        if (state==0){
            if (RunOutOfPlays(board, board_pos_x, board_pos_y)) state=5;
            VictoryOrDefeat(game_stats, game_targets, &state, ncolors);
        }

        InfoDisplayer(state, renderer, sans);
        RenderStats(renderer, sans, game_stats, ncolors, num_selected, convToDisplay, state, current_selected, num_selected, flag_square);
        // render in the screen all changes above
        SDL_RenderPresent(renderer);
        //sets all the cells of current_selected not in use to -1
        CleanC_S(current_selected, num_selected);
        // add a delay
        SDL_Delay( delay);


    }

    // free memory allocated for images and textures and closes everything including fonts
    TTF_CloseFont(serif);
    TTF_CloseFont(sans);
    SDL_FreeSurface(imgs[0]);
    SDL_FreeSurface(imgs[1]);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}

/**
 * ProcessMouseEvent: gets the square pos based on the click positions !f
 * \param _mouse_pos_x position of the click on pixel coordinates
 * \param _mouse_pos_y position of the click on pixel coordinates
 * \param _board_size_px size of the board !
 * \param _square_size_px size of each square
 * \param _pt_x square nr
 * \param _pt_y square nr
 */

void ProcessMouseEvent(int _mouse_pos_x, int _mouse_pos_y, int _board_size_px[], int _square_size_px,
        int *_pt_x, int *_pt_y, int _board_pos_x, int _board_pos_y ){
    // corner of the board
    int x_corner = (TABLE_SIZE - _board_size_px[0]) >> 1;
    int y_corner = (TABLE_SIZE - _board_size_px[1] - 15);

    // verify if valid cordinates
    if (_mouse_pos_x < x_corner || _mouse_pos_y < y_corner || _mouse_pos_x > (x_corner + _board_size_px[0]-10)
        || _mouse_pos_y > (y_corner + _board_size_px[1]-10) )
    {
        *_pt_x = -1;
        *_pt_y = -1;
        return;
    }

    // computes the square where the mouse position is
    _mouse_pos_x = _mouse_pos_x - x_corner;
    _mouse_pos_y = _mouse_pos_y - y_corner;

    //verifica se o rato esta contido nalgum dos circulos (se for o caso identifica as suas coordenadas ja processadas)
    int CircRadius,centerCircX,centerCircY, distancetoPointCenter;

    for ( int i = 0; i < _board_pos_x; i++ )
    {
        for ( int j = 0; j < _board_pos_y; j++ )
        {
                centerCircX = (i+1)*SQUARE_SEPARATOR + i*(_square_size_px)+(_square_size_px>>1);
                centerCircY = (j+1)*SQUARE_SEPARATOR + j*(_square_size_px)+(_square_size_px>>1);
                CircRadius = (int)(_square_size_px*0.4f);
                distancetoPointCenter=sqrt(((centerCircX-_mouse_pos_x)*(centerCircX-_mouse_pos_x))+((centerCircY-_mouse_pos_y)*(centerCircY-_mouse_pos_y)));

                if (distancetoPointCenter<=CircRadius){
                    *_pt_x=i;
                    *_pt_y=j;
                    return;
                }
                else {
                    *_pt_x = -1;
                    *_pt_y = -1;
                }
        }
    }

}

/**
 * RenderPoints: renders the board
 * \param _board 2D array with integers representing board colors
 * \param _board_pos_x number of positions in the board (x axis)
 * \param _board_pos_y number of positions in the board (y axis)
 * \param _square_size_px size of each square
 * \param _board_size_px size of the board
 * \param _renderer renderer to handle all rendering in a window
 */
void RenderPoints(int _board[][MAX_BOARD_POS], int _board_pos_x, int _board_pos_y,
        int _board_size_px[], int _square_size_px, SDL_Renderer *_renderer ){
    int clr, x_corner, y_corner, circleX, circleY, circleR;

    // corner of the board
    x_corner = (TABLE_SIZE - _board_size_px[0]) >> 1;
    y_corner = (TABLE_SIZE - _board_size_px[1] - 15);

    // renders the squares where the dots will appear
    for ( int i = 0; i < _board_pos_x; i++ )
    {
        for ( int j = 0; j < _board_pos_y; j++ )
        {
                // define the size and copy the image to display
                circleX = x_corner + (i+1)*SQUARE_SEPARATOR + i*(_square_size_px)+(_square_size_px>>1);
                circleY = y_corner + (j+1)*SQUARE_SEPARATOR + j*(_square_size_px)+(_square_size_px>>1);
                circleR = (int)(_square_size_px*0.4f);
                // draw a circle
                clr = _board[i][j];
                filledCircleRGBA(_renderer, circleX, circleY, circleR, colors[0][clr], colors[1][clr], colors[2][clr]);
        }
    }
}

/**
 * filledCircleRGBA: renders a filled circle
 * \param _circleX x pos
 * \param _circleY y pos
 * \param _circleR radius
 * \param _r red
 * \param _g gree
 * \param _b blue
 */
void filledCircleRGBA(SDL_Renderer * _renderer, int _circleX, int _circleY, int _circleR, int _r, int _g, int _b){
    int off_x = 0;
    int off_y = 0;
    float degree = 0.0;
    float step = M_PI / (_circleR*8);

    SDL_SetRenderDrawColor(_renderer, _r, _g, _b, 255);

    while (_circleR > 0)
    {
        for (degree = 0.0; degree < M_PI/2; degree+=step)
        {
            off_x = (int)(_circleR * cos(degree));
            off_y = (int)(_circleR * sin(degree));
            SDL_RenderDrawPoint(_renderer, _circleX+off_x, _circleY+off_y);
            SDL_RenderDrawPoint(_renderer, _circleX-off_y, _circleY+off_x);
            SDL_RenderDrawPoint(_renderer, _circleX-off_x, _circleY-off_y);
            SDL_RenderDrawPoint(_renderer, _circleX+off_y, _circleY-off_x);
        }
        _circleR--;
    }
}

/*
 * Shows some information about the game:
 * - Goals of the game
 * - Number of moves
 * \param _renderer renderer to handle all rendering in a window
 * \param _font font to display the scores
 * \param _goals goals of the game
 * \param _ncolors number of colors
 * \param _moves number of moves remaining
 */


/**
 * RenderTable: Draws the table where the game will be played, namely:
 * -  some texture for the background
 * -  the right part with the IST logo and the student name and number
 * -  the grid for game board with squares and seperator lines
 * \param _board_pos_x number of positions in the board (x axis)
 * \param _board_pos_y number of positions in the board (y axis)
 * \param _board_size_px size of the board
 * \param _font font used to render the text
 * \param _img surfaces with the table background and IST logo (already loaded)
 * \param _renderer renderer to handle all rendering in a window
 */

int RenderTable( int _board_pos_x, int _board_pos_y, int _board_size_px[],
        TTF_Font *_font, SDL_Surface *_img[], SDL_Renderer* _renderer, char player_name[] ){

    SDL_Color black = { 0, 0, 0 }; // black
    SDL_Color light = { 240, 240, 240, 255 };
    SDL_Color dark = { 120, 110, 102 };
    SDL_Texture *table_texture;
    SDL_Rect tableSrc, tableDest, board, board_square;
    int height, board_size, square_size_px, max_pos;

    // set color of renderer to some color
    SDL_SetRenderDrawColor( _renderer, 255, 255, 255, 255 );

    // clear the window
    SDL_RenderClear( _renderer );

    tableDest.x = tableSrc.x = 0;
    tableDest.y = tableSrc.y = 0;
    tableSrc.w = _img[0]->w;
    tableSrc.h = _img[0]->h;
    tableDest.w = TABLE_SIZE;
    tableDest.h = TABLE_SIZE;

    // draws the table texture
    table_texture = SDL_CreateTextureFromSurface(_renderer, _img[0]);
    SDL_RenderCopy(_renderer, table_texture, &tableSrc, &tableDest);

    // render the IST Logo
    height = RenderLogo(TABLE_SIZE, 0, _img[1], _renderer);

    // render the student name
    height += RenderText(TABLE_SIZE+3*MARGIN, height, myName, _font, &black, _renderer);

    // this renders the student number
    height+=RenderText(TABLE_SIZE+3*MARGIN, height, myNumber, _font, &black, _renderer);

    //render the player_name
    height+=RenderText(TABLE_SIZE+3*MARGIN, height, "Jogador:" , _font, &black, _renderer);
    RenderText(TABLE_SIZE+5*MARGIN, height, player_name, _font, &black, _renderer);


    // compute and adjust the size of the table and squares
    max_pos = MAX(_board_pos_x, _board_pos_y);
    board_size = (int)(BOARD_SIZE_PER*TABLE_SIZE);
    square_size_px = (board_size - (max_pos+1)*SQUARE_SEPARATOR) / max_pos;
    _board_size_px[0] = _board_pos_x*(square_size_px+SQUARE_SEPARATOR)+SQUARE_SEPARATOR;
    _board_size_px[1] = _board_pos_y*(square_size_px+SQUARE_SEPARATOR)+SQUARE_SEPARATOR;

    // renders the entire board background
    SDL_SetRenderDrawColor(_renderer, light.r, light.g, light.b, light.a );
    board.x = (TABLE_SIZE - _board_size_px[0]) >> 1;
    board.y = (TABLE_SIZE - _board_size_px[1] - 15);
    board.w = _board_size_px[0];
    board.h = _board_size_px[1];
    SDL_RenderFillRect(_renderer, &board);

    // renders the squares where the numbers will appear


    // iterate over all squares
    for ( int i = 0; i < _board_pos_x; i++ )
    {
        for ( int j = 0; j < _board_pos_y; j++ )
        {
            board_square.x = board.x + (i+1)*SQUARE_SEPARATOR + i*square_size_px;
            board_square.y = board.y + (j+1)*SQUARE_SEPARATOR + j*square_size_px;
            board_square.w = square_size_px;
            board_square.h = square_size_px;
        //    SDL_RenderFillRect(_renderer, &board_square);
        }
    }

    // destroy everything
    SDL_DestroyTexture(table_texture);
    // return for later use
    return square_size_px;
}

/**
 * RenderLogo function: Renders the IST logo on the app window
 * \param x X coordinate of the Logo
 * \param y Y coordinate of the Logo
 * \param _logoIST surface with the IST logo image to render
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderLogo(int x, int y, SDL_Surface *_logoIST, SDL_Renderer* _renderer){
    SDL_Texture *text_IST;
    SDL_Rect boardPos;

    // space occupied by the logo
    boardPos.x = x;
    boardPos.y = y;
    boardPos.w = _logoIST->w;
    boardPos.h = _logoIST->h;

    // render it
    text_IST = SDL_CreateTextureFromSurface(_renderer, _logoIST);
    SDL_RenderCopy(_renderer, text_IST, NULL, &boardPos);

    // destroy associated texture !
    SDL_DestroyTexture(text_IST);
    return _logoIST->h;
}

/**
 * RenderText function: Renders some text on a position inside the app window
 * \param x X coordinate of the text
 * \param y Y coordinate of the text
 * \param text string with the text to be written
 * \param _font TTF font used to render the text
 * \param _color color of the text
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderText(int x, int y, const char *text, TTF_Font *_font, SDL_Color *_color, SDL_Renderer* _renderer){
    SDL_Surface *text_surface;
    SDL_Texture *text_texture;
    SDL_Rect solidRect;

    solidRect.x = x;
    solidRect.y = y;
    // create a surface from the string text with a predefined font
    text_surface = TTF_RenderText_Blended(_font,text,*_color);
    if(!text_surface)
    {
        printf("TTF_RenderText_Blended: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    // create texture
    text_texture = SDL_CreateTextureFromSurface(_renderer, text_surface);
    // obtain size
    SDL_QueryTexture( text_texture, NULL, NULL, &solidRect.w, &solidRect.h );
    // render it !
    SDL_RenderCopy(_renderer, text_texture, NULL, &solidRect);
    // clear memory
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
    return solidRect.h;
}

/**
 * InitEverything: Initializes the SDL2 library and all graphical components: font, window, renderer
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _font font that will be used to render the text
 * \param _img surface to be created with the table background and IST logo
 * \param _window represents the window of the application
 * \param _renderer renderer to handle all rendering in a window
 */
void InitEverything(int width, int height, TTF_Font **_font, TTF_Font **_font1, SDL_Surface *_img[], SDL_Window** _window, SDL_Renderer** _renderer){
    InitSDL();
    InitFont();
    *_window = CreateWindow(width, height);
    *_renderer = CreateRenderer(width, height, *_window);

    // load the table texture
    _img[0] = IMG_Load("table_texture.png");
    if (_img[0] == NULL)
    {
        printf("Unable to load image: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // load IST logo
    _img[1] = SDL_LoadBMP("ist_logo.bmp");
    if (_img[1] == NULL)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // this opens (loads) a font file and sets a size
    *_font = TTF_OpenFont("FreeSerif.ttf", 16);
    if(!*_font)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    // this opens (loads) a NEW font file and sets a size
    *_font1 = TTF_OpenFont("OpenSans.ttf", 30);
    if(!*_font1)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }



}

/**
 * InitSDL: Initializes the SDL2 graphic library
 */
void InitSDL(){
    // init SDL library
    if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        printf(" Failed to initialize SDL : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * InitFont: Initializes the SDL2_ttf font library
 */
void InitFont(){
    // Init font library
    if(TTF_Init()==-1)
    {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * CreateWindow: Creates a window for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \return pointer to the window created
 */
SDL_Window* CreateWindow(int width, int height){
    SDL_Window *window;
    // init window
    window = SDL_CreateWindow( "IST DOTS", WINDOW_POSX, WINDOW_POSY, width, height, 0 );
    // check for error !
    if ( window == NULL )
    {
        printf("Failed to create window : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    return window;
}

/**
 * CreateRenderer: Creates a renderer for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _window represents the window for which the renderer is associated
 * \return pointer to the renderer created
 */
SDL_Renderer* CreateRenderer(int width, int height, SDL_Window *_window){
    SDL_Renderer *renderer;
    // init renderer
    renderer = SDL_CreateRenderer( _window, -1, 0 );

    if ( renderer == NULL )
    {
        printf("Failed to create renderer : %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // set size of renderer to the same as window
    SDL_RenderSetLogicalSize( renderer, width, height );

    return renderer;
}

void ParamReading(int *_board_pos_x, int *_board_pos_y, char player_name[] ,int *_ncolors, int game_targets[]){
    char buffer[BUFFER_SIZE];

    while(1){
        printf("Introduz as dimensões do tabuleiro (5 a 15)(horizontal [espaço] vertical):  ");

        if(fgets(buffer, BUFFER_SIZE, stdin)==NULL) exit(-1);

        if((sscanf(buffer," %d %d", _board_pos_x, _board_pos_y)==2)&&(4<*_board_pos_x&&*_board_pos_x<16)&&(4<*_board_pos_y&&*_board_pos_y<16)) break;

        else (printf("Verifica os valores introduzidos\n"));
    }

    while(1){
        printf("Introduz o teu nome: ");

        if(fgets(buffer, BUFFER_SIZE, stdin)==NULL) exit(-1);

        if((sscanf(buffer," %s ", player_name)==1)){
            if(strlen(player_name)<9) break;
            else (printf("A dimensão máxima é 8 caracteres.\n"));
        }
        else (printf("Verifica os valores introduzidos\n"));
    }


    while(1){
        printf("Numero de cores(ate 5): ");
        if(fgets(buffer, BUFFER_SIZE, stdin)==NULL) exit(-1);
            if((sscanf(buffer," %d", _ncolors)==1)&&(0<*_ncolors&&*_ncolors<6)) break;
        else (printf("Nº de cores entre 1 e 5 (inclusive)\n"));
    }


    printf("OBJETIVOS:\n");
    for(int i=0; i<*_ncolors;i++){
        while(1){
            switch (i){
                case(0):
                    printf("Nº de azuis: " );
                    break;
                case(1):
                    printf("Nº de vermelhos: " );
                    break;
                case(2):
                    printf("Nº de verdes: " );
                    break;
                case(3):
                    printf("Nº de amarelos: " );
                    break;
                case(4):
                    printf("Nº de roxos: " );
                    break;
            }
            if(fgets(buffer, BUFFER_SIZE, stdin)==NULL) exit(-1);
            if((sscanf(buffer," %d", &game_targets[i])==1)&&(0<game_targets[i]&&game_targets[i]<100)) break;
            else (printf("Objetivos entre 1 e 99 (inclusive).\n"));
        }
    }

    while(1){
        printf("Numero de movimentos(ate 99): ");
        if(fgets(buffer, BUFFER_SIZE, stdin)==NULL) exit(-1);
        if((sscanf(buffer," %d", &game_targets[5])==1)&&(0<game_targets[5]&&game_targets[5]<100)) break;
        else (printf("Verifica os valores introduzidos\n"));
    }



}

//to be deleted
void TestPrints(int board_pos_x, int board_pos_y, char player_name[] ,int ncolors, int game_targets[]){

        printf("%d %d %s %d aqui %d\n", board_pos_x, board_pos_y, player_name, ncolors, game_targets[0]);
        for(int i=0;i<6;i++){
            printf("%d",game_targets[i]);
        }
}

void InitialBoard(int board[][MAX_BOARD_POS], int board_pos_x, int board_pos_y, int ncolors) {
    srand(time(NULL));
    for ( int i=0; i<board_pos_x;i++){
        for(int j=0; j<board_pos_y;j++){
                board[i][j]=(rand()%ncolors);
        }
    }
}

void CurrentMove(int _pt_x,int _pt_y,int current_selected[TABLE_SIZE][3],int _board[][MAX_BOARD_POS], int *ptrnum_selected, int *ptrflag_square){
    if ((*ptrnum_selected==0)&&(_pt_x!=-1&&_pt_y!=-1)){
        current_selected[0][0]=_pt_x;
        current_selected[0][1]=_pt_y;
        current_selected[0][2]=_board[_pt_x][_pt_y];
        *ptrnum_selected=1;
        *ptrflag_square=0;
    }
    else if(YNconnect(ptrnum_selected, current_selected,_board, _pt_x, _pt_y, ptrflag_square)==1){
        current_selected[*ptrnum_selected][0]=_pt_x;
        current_selected[*ptrnum_selected][1]=_pt_y;
        current_selected[*ptrnum_selected][2]=_board[_pt_x][_pt_y];
        *ptrnum_selected=*ptrnum_selected+1;
    }

    else        ;
}

//verifica se deve ligar os pontos
int YNconnect(int *ptrnum_selected, int current_selected[TABLE_SIZE][3], int board[][MAX_BOARD_POS], int _pt_x, int _pt_y, int *ptrflag_square){
    if ((_pt_x)==-1) return 0;

    if ((current_selected[*ptrnum_selected-2][0]==_pt_x)&&(current_selected[*ptrnum_selected-2][1]==_pt_y)){
        *ptrflag_square=0;

        *ptrnum_selected=*ptrnum_selected-2;

        if (*ptrnum_selected<1) *ptrnum_selected=0;

        return 1;
    }

    if ((*ptrflag_square)==1) return 0;

    if ((_pt_x!=-1 && _pt_y!=-1)&&(((abs(current_selected[*ptrnum_selected-1][0]-_pt_x)==1)&&(abs(current_selected[*ptrnum_selected-1][1]-_pt_y)==0)) xor ((abs(current_selected[*ptrnum_selected-1][1]-_pt_y)==1)&&(abs(current_selected[*ptrnum_selected-1][0]-_pt_x))==0))){
        //verifca se sao da mesma cor
        if ((current_selected[*ptrnum_selected-1][2]) == board[_pt_x][_pt_y]){
            //se o cursor recuar para os pontos sucessivamente anteriores esquece-os
            for (int i=0; i<*ptrnum_selected;i++){
                if (((current_selected[i][0]==_pt_x)&&(current_selected[i][1]==_pt_y))&&((*ptrnum_selected-1)!=i)){
                    current_selected[CROSSING_POINT][0]=i;
                    *ptrflag_square=1;
                    break;
                }
            }

            return 1;
        }
        else return 0;
    }
    else return 0;
}

void RenderPath(SDL_Renderer* _renderer, int _num_selected, int current_selected[TABLE_SIZE][3], int _square_size_px,int board_size_px[2]){

        int x1, x2, y1, y2,clr, thickness;
        //assures that the the thickness of the linker as a constant proportion to the dots size
        thickness=_square_size_px*0.1;
        for(int j=-thickness; j<thickness;j++){
            for (int i=0; i<_num_selected-1;i++){
                x1 = DotToCoordinate(0, board_size_px, _square_size_px,current_selected[i][0])+j;// * (_square_size_px + SQUARE_SEPARATOR)+200;
                y1 = DotToCoordinate(1, board_size_px, _square_size_px,current_selected[i][1])+j;// *(_square_size_px + SQUARE_SEPARATOR)+200;
                x2 = DotToCoordinate(0, board_size_px, _square_size_px,current_selected[i+1][0])+j;
                y2 = DotToCoordinate(1, board_size_px, _square_size_px,current_selected[i+1][1])+j;
                //    printf("%d %d %d %d pontos\n", x1,y1,x2,y2);
                clr = current_selected[0][2];
                SDL_SetRenderDrawColor(_renderer, colors[0][clr], colors[1][clr], colors[2][clr], 255);
                SDL_RenderDrawLine(_renderer, x1, y1, x2, y2);
            }
        }
}

void RemovePoints(int board_size_px[], int _square_size_px, SDL_Renderer *_renderer,
            int _num_selected, int current_selected[TABLE_SIZE][3]){

                int circleR,coord_x,coord_y;
                if (_num_selected==1) return;
                for ( int i = 0; i < _num_selected; i++ ){
                    coord_x = DotToCoordinate(0, board_size_px, _square_size_px,current_selected[i][0]);
                    coord_y = DotToCoordinate(1, board_size_px, _square_size_px,current_selected[i][1]);
                    circleR = (int)(_square_size_px*0.4f);
                    // 205, 193, 181 color of the square
                    filledCircleRGBA(_renderer, coord_x, coord_y, circleR, 205, 193, 181);
                }
}

void HidePointsToBeRemoved (int board_size_px[], int _square_size_px, SDL_Renderer *_renderer,int board[][MAX_BOARD_POS], int _board_pos_x, int _board_pos_y, int current_selected[TABLE_SIZE][3], int state){
    int circleR,coord_x,coord_y,clr;

    if (state==2){
            for(int j=0;j<_board_pos_x;j++){
                for(int k=_board_pos_y-1; k>=0;k--){
                    if(board[j][k]<0){
                        coord_x = DotToCoordinate(0, board_size_px, _square_size_px,j);
                        coord_y = DotToCoordinate(1, board_size_px, _square_size_px,k);
                        circleR = (int)(_square_size_px*0.4f);
                        // 205, 193, 181 color of the square
                        filledCircleRGBA(_renderer, coord_x, coord_y, circleR, 205, 193, 181);
                    }
                }
            }
            return;
    }

    for(int j=0;j<_board_pos_x;j++){
        for(int k=_board_pos_y-1; k>=0;k--){
            if(board[j][k]<0){
                coord_x = DotToCoordinate(0, board_size_px, _square_size_px,j);
                coord_y = DotToCoordinate(1, board_size_px, _square_size_px,k);
                circleR = (int)(_square_size_px*0.4f);

                clr=current_selected[0][2];
                // 205, 193, 181 color of the square

                if ((board[j][k])==-2) filledCircleRGBA(_renderer, coord_x, coord_y, circleR*1.5, colors[0][clr], colors[1][clr], colors[2][clr]);
                filledCircleRGBA(_renderer, coord_x, coord_y, circleR, 205, 193, 181);
                if ((board[j][k])==-3) filledCircleRGBA(_renderer, coord_x, coord_y, circleR, 249, 166, 2);
            }
        }
    }

}

int DotToCoordinate(int  X0Y1,  int board_size_px[], int _square_size_px, int INPUT){
    int x_corner, y_corner, coordinateX, coordinateY;
    if (X0Y1==0) {
        x_corner = (TABLE_SIZE - board_size_px[0]) >> 1;
        coordinateX = x_corner + (INPUT+1)*SQUARE_SEPARATOR + INPUT*(_square_size_px)+(_square_size_px>>1);
        return  coordinateX;
    }
    else{
        y_corner = (TABLE_SIZE - board_size_px[1] - 15);
        coordinateY = y_corner + (INPUT+1)*SQUARE_SEPARATOR + INPUT*(_square_size_px)+(_square_size_px>>1);
        return coordinateY;
    }
}

void SinalizePointsToBeDeleted(int board[][MAX_BOARD_POS], int current_selected[TABLE_SIZE][3], int _num_selected, int _board_pos_x, int _board_pos_y, int flag_square, int game_stats[6]){
    int marker=-1;

    if (_num_selected==1) return;

    game_stats[current_selected[0][2]]-=_num_selected;

    for(int i=0; i<_num_selected;i++){

        if(i>=current_selected[CROSSING_POINT][0]&&flag_square==1) marker=-2;

        board[current_selected[i][0]][current_selected[i][1]]=marker;
    }

    if(flag_square==1){
        //caso haja um quadrado o ponto de cruzamento esta selecionado duas vezes
        game_stats[current_selected[0][2]]--;

        for(int j=0;j<_board_pos_x;j++){
            for(int k=_board_pos_y; k>=0;k--){
                if(board[j][k]==current_selected[0][2]){

                    board[j][k]=-1;
                    game_stats[current_selected[0][2]]--;
                }
            }
        }
    if (_num_selected>4)    RadialDotCheck(board, _board_pos_x,_board_pos_y, current_selected, game_stats);
    }

}

void MovePoints(int board[][MAX_BOARD_POS], int current_selected[TABLE_SIZE][3], int _num_selected, int _board_pos_x, int _board_pos_y, int flag_square){
    int l=0;

    if (_num_selected==1) return; //se só houver um número

    for(int j=0;j<_board_pos_x;j++){
        for(int k=_board_pos_y-1; k>=0;k--){
            if (board[j][k]<0){
                l=k-1;
                while (l>=0){
                    if (board[j][l]>-1){
                        board[j][k]=board[j][l];
                        board[j][l]=-1;
                        break;
                    }
                    else l--;
                }
            }
            else {}
        }
    }

    //após sevir o seu propósito reinicia-se para um valor grande o suficiente para nao afetar futuras jogadas
    current_selected[CROSSING_POINT][0]=1000;

}

void FreshNewPoints(int board[][MAX_BOARD_POS], int _board_pos_x, int _board_pos_y, int ncolors){
    srand(time(NULL));
    for(int i=0; i<_board_pos_x;i++){
        for(int j=0; j<_board_pos_y;j++){
            if (board[i][j]<0) board[i][j]=(rand()%ncolors);
        }
    }
}

void CleanC_S(int current_selected[][3] , int num_selected){

    for(int i=CROSSING_POINT-1;i>num_selected;i--){
        for(int j=0; j<3; j++){
            current_selected[i][j]=-1;
        }
    }
}

void RadialDotCheck(int board[][MAX_BOARD_POS], int _board_pos_x, int _board_pos_y, int current_selected[TABLE_SIZE][3], int game_stats[6]){
    int x1, y1, x0, y0, direcao=0;

    for(int i=1; i<_board_pos_x-1;i++){
            for(int j=1; j<_board_pos_y-1;j++){


                    direcao=0;
                    x0=i;
                    y0=j;
                    x1=i;
                    y1=j;

                    while (direcao<9){
                        switch (direcao) {
                        case 0:
                            x1=x0+1;
                            break;
                        case 1:
                            x1=x0-1;
                            break;
                        case 2:
                            y1=y0+1;
                            break;
                        case 3:
                            y1=y0-1;
                            break;
                        case 4:
                            x1=x0+1;
                            y1=y0+1;
                            break;
                        case 5:
                            x1=x0+1;
                            y1=y0-1;
                            break;
                        case 6:
                            x1=x0-1;
                            y1=y0-1;
                            break;
                        case 7:
                            x1=x0-1;
                            y1=y0+1;
                            break;
                        case 8:
                            //inc. the counter of the dot color if it isn't already counted(the dots with the same color of the square)
                            if(board[i][j]!=current_selected[0][2]) game_stats[board[i][j]]++;
                            board[i][j]=-3;
                            break;
                        default:
                            break;
                        }

                        if (board[x1][y1]==-2){
                            x0=i;
                            y0=j;
                            direcao++;
                        }

                        else if ((x1==0)||(y1==0)||(x1==(_board_pos_x-1))||(y1==(_board_pos_y-1)))  direcao=9;

                        else{
                            x0=x1;
                            y0=y1;
                        }
                    }

            }
    }
}

void SetGameStats(int game_stats[6], int game_targets[6]){
    for(int i=0; i<6; i++){
        game_stats[i]=game_targets[i];
    }
}

void RenderStats( SDL_Renderer* _renderer, TTF_Font *_font1, int _game_stats[], int _ncolors, int _moves, char convToDisplay[], int state, int current_selected[TABLE_SIZE][3],
                    int num_selected, int flag_square){
    SDL_Color black={0,0,0};
    SDL_Rect stats_boxes;

    stats_boxes.x=(TABLE_SIZE/2)-((130*(_ncolors)+100)/2);
    stats_boxes.y=77;
    stats_boxes.w=100;
    stats_boxes.h=61;

    int dotsize;

    for(int i=0; i<=_ncolors;i++){



        SDL_SetRenderDrawColor( _renderer, 240, 240 , 240, 240 );
        SDL_RenderFillRect(_renderer, &stats_boxes);

        if (i==0){
            sprintf(convToDisplay, "%d", _game_stats[5]);
            RenderText(stats_boxes.x+25, stats_boxes.y+10 , convToDisplay, _font1, &black, _renderer);
        }

        if (i>0) {
            sprintf(convToDisplay, "%d", _game_stats[i-1]);
            if (_game_stats[i-1]<0) sprintf(convToDisplay, "%d", 0);
            RenderText(stats_boxes.x+65, stats_boxes.y+10 , convToDisplay, _font1, &black, _renderer);
            filledCircleRGBA(_renderer, stats_boxes.x+30, stats_boxes.y+30, 26, colors[0][i-1], colors[1][i-1], colors[2][i-1]);
            if(current_selected[1][2]==i-1){
                dotsize=26+num_selected*2;
                if (flag_square==1) filledCircleRGBA(_renderer, stats_boxes.x+30, stats_boxes.y+30, 100, colors[0][i-1], colors[1][i-1], colors[2][i-1]);
                filledCircleRGBA(_renderer, stats_boxes.x+30, stats_boxes.y+30, dotsize, colors[0][i-1], colors[1][i-1], colors[2][i-1]);
            }
        }


        stats_boxes.x+=130;
        
    }
}


int RunOutOfPlays(int board[][MAX_BOARD_POS], int board_pos_x, int board_pos_y){
    for(int i=0; i<board_pos_x-1; i++){
        for(int j=0; j<board_pos_y-1;j++){
            if ((board[i][j]==board[i+1][j])||(board[i][j]==board[i][j+1])){
                return 0;
            }
        }
    }

    return 1;
}

void InfoDisplayer(int state, SDL_Renderer* _renderer, TTF_Font *_font1){

    SDL_Rect translucid_background;
    SDL_Color black={0,0,0};

    translucid_background.x=180;
    translucid_background.y=360;
    translucid_background.w=500;
    translucid_background.h=307;

    if (state==-1){
        SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor( _renderer, 240, 240 , 240, 180 );
        SDL_RenderFillRect(_renderer, &translucid_background);
        RenderText(translucid_background.x+40, translucid_background.y+130 , "CLIQUE 'N' PARA COMEÇAR", _font1, &black, _renderer);

    }

    if(state==3){
        SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor( _renderer, 240, 240 , 240, 180 );
        SDL_RenderFillRect(_renderer, &translucid_background);
        RenderText(translucid_background.x+130, translucid_background.y+115 , "GANHASTE!", _font1, &black, _renderer);
        RenderText(translucid_background.x+25, translucid_background.y+150 , "CLIQUE 'N' PARA UM NOVO JOGO", _font1, &black, _renderer);
    }

    if(state==4){
        SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor( _renderer, 240, 240 , 240, 180 );
        SDL_RenderFillRect(_renderer, &translucid_background);
        RenderText(translucid_background.x+130, translucid_background.y+115 , "PERDESTE!", _font1, &black, _renderer);
        RenderText(translucid_background.x+25, translucid_background.y+150 , "CLIQUE 'N' PARA UM NOVO JOGO", _font1, &black, _renderer);
    }

    if (state==5){
        SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor( _renderer, 240, 240 , 240, 180 );
        SDL_RenderFillRect(_renderer, &translucid_background);
        RenderText(translucid_background.x+25, translucid_background.y+115 , "ESGOTOU AS JOGADAS POSSIVEIS", _font1, &black, _renderer);
        RenderText(translucid_background.x+25, translucid_background.y+150 , "CLIQUE 'N' PARA UM NOVO JOGO", _font1, &black, _renderer);

    }
}

void VictoryOrDefeat(int game_stats[6], int game_targets[6],int *state, int ncolors){
    int vict_flag=0, defeat_flag=0;

    for(int i=0; i<ncolors; i++){
        if (game_stats[i]>0) break;
        if (i==ncolors-1) vict_flag=1;
    }
    if (game_stats[5]<1){
        defeat_flag=1;
    }

    if (vict_flag==1){
        *state=3;
        //game_targets-game_stats
    }
    else if (defeat_flag==1){
        *state = 4;

    }
    else ;
}


void CloneForUndo(int board[][MAX_BOARD_POS],int  undo_board[][MAX_BOARD_POS], int game_stats[6], int undo_game_stats[6], int board_pos_x, int board_pos_y, int ncolors){

    for(int i=0; i<board_pos_x; i++){
        for(int j=0; j<board_pos_y;j++){
            undo_board[i][j]=board[i][j];
        }
    }

    for(int k=0; k<ncolors; k++){
        undo_game_stats[k]=game_stats[k];
    }

    undo_game_stats[5]=game_stats[5];

}


void Undo(int board[][MAX_BOARD_POS],int  undo_board[][MAX_BOARD_POS], int game_stats[6], int undo_game_stats[6], int board_pos_x, int board_pos_y, int ncolors){

    for(int i=0; i<board_pos_x; i++){
        for(int j=0; j<board_pos_y;j++){
            board[i][j]=undo_board[i][j];
        }
    }

    for(int k=0; k<ncolors; k++){
        game_stats[k]=undo_game_stats[k];
    }

    game_stats[5]=undo_game_stats[5];

}
