#include <SDL2/SDL.h>
#include <string>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
using namespace std;

class Judge
{
  public:
    Judge(const string &ip, const int &port);
    ~Judge();
    bool sendAns(int i);
    int recAns();
    bool wait_connect();
    
  private:
    int sock;
    int clnt_sock;
    bool connected;
    sockaddr_in serv_addr;
    char str[256];
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;
};

Judge::Judge(const string &ip, const int &port) : connected(false)
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));  
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());  
    serv_addr.sin_port = htons(port);  
    bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(sock, 20);
    clnt_addr_size = sizeof(clnt_addr);
}

bool Judge::wait_connect()
{
	cout << "waiting...\n";
    clnt_sock = accept(sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1) return false;
    connected = true;
    cout << "successful connect" << endl;
    return true;
}

int Judge::recAns()
{
    if (!connected)
    {
        if (wait_connect() == false) return -1;
    }
    int stat = recv(clnt_sock, str, sizeof(str) - 1, 0);
    if (stat == 0)
    {
        connected = false;
        cout << "gg\n";
        return -1;
    }
    if (stat == -1)
    {
        return -1;
    }
    int res = str[0] - '0';
    if (strlen(str) > 1 || res < 0 || res > 9) return -1;
    return res;
}

Judge::~Judge()
{
    close(clnt_sock);
    close(sock);
}

struct clientData
{
    int num;
    bool used;
    clientData()
    {
        num = -1;
        used = true;
    }
} data;

static void* Data_handler(void* d)
{
    clientData* data = (clientData*)d;
    Judge judge("0.0.0.0", 4331);
    int temp;
    while (1)
    {
        while (!data->used);
        while ((temp = judge.recAns()) == -1);
        data->num = temp;
        data->used = false;
    }
    return d;
}

void exit_handler(int g)
{
	cout << "press ESC to exit\n";
}

int main(int argc, char **argv)
{
  signal(SIGINT, exit_handler);
  pthread_t thread_id;
  if (pthread_create(&thread_id, NULL, (&Data_handler), (void *)&data) == -1)
  {
    cerr << "Creating thread error.\n";
    exit(1);
  }

  bool quit = false;
  SDL_Event event;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_DisplayMode DM;
  SDL_GetCurrentDisplayMode(0, &DM);

  SDL_Window *window = SDL_CreateWindow("My SDL Empty Window",
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768,
                                        SDL_WINDOW_FULLSCREEN_DESKTOP);

  SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);

  // initialize digits
  SDL_Surface * image[10];
  SDL_Texture * texture[10];
  int digit_width = DM.h / 130 * 86;
  SDL_Rect dstrect = { (DM.w - digit_width) / 2, 0, digit_width, DM.h };
  srand(time(NULL));
  for (int i = 0; i < 10; i++)
  {
    string t = "dig/";
    t += char(i + '0');
    t += ".bmp";
    image[i] = SDL_LoadBMP(t.c_str());
    texture[i] = SDL_CreateTextureFromSurface(renderer, image[i]);

  }

  // initialize indicator
  SDL_Surface * indicator = SDL_LoadBMP("dig/indicator.bmp");
  SDL_Texture * ind_texture = SDL_CreateTextureFromSurface(renderer, indicator);
  float block_dist = DM.h / 5.0;
  float ind_h = block_dist * 2.0 / 3.0;
  float ind_w = (DM.w - digit_width) / 3.0;
  float ind_x_offset = ((DM.w - digit_width) / 2.0 - ind_w) / 2.0;
  float ind_y_offset = block_dist / 6.0;
  SDL_Rect ind_location[10];
  for (int i = 0; i < 5; i++)
  {
    ind_location[2 * i] = {ind_x_offset, DM.h - ind_y_offset / 2.0 - ind_h - i * block_dist, ind_w, ind_h};
    ind_location[2 * i + 1] = {DM.w - ind_x_offset - ind_w, DM.h - ind_y_offset / 2.0 - ind_h - i * block_dist, ind_w, ind_h};
  }

  // initalize screen blocker
  SDL_Rect blocker = {0, 0, DM.w, DM.h};


  int curDigit, lastDigit = -1;
  curDigit = rand() % 10;
  SDL_RenderDrawRect(renderer, &blocker);
  SDL_RenderPresent(renderer);
  lastDigit = curDigit;
  int successCount = 0;
  bool gotAns = false;
  int tempAns;
  bool working = false;

  time_t start_time = time(NULL);
  while (!quit)
  {
    if (working)
    {
      if (!data.used)
      {
        tempAns = data.num;
        data.used = true;
        if (gotAns)
        {
          successCount = 0;
        }
        gotAns = true;
        cout << "get:" << tempAns << endl;
        if (tempAns == lastDigit)
        {
          successCount++;
        }
        else
        {
          successCount = 0;
        }
      }
      if (time(NULL) - start_time)
      {
        if (!gotAns)
        {
          successCount = 0;
        }
        start_time = time(NULL);
        curDigit = rand() % 10;
        SDL_RenderCopy(renderer, texture[curDigit], NULL, &dstrect);
        if (successCount > 0)
        {
          SDL_RenderCopy(renderer, ind_texture, NULL, &ind_location[(successCount - 1) * 2]);
          SDL_RenderCopy(renderer, ind_texture, NULL, &ind_location[(successCount - 1) * 2 + 1]);
        }
        SDL_RenderPresent(renderer);
        lastDigit = curDigit;
        gotAns = false;
        successCount += 1;
        if (successCount == 6)
        {
          SDL_RenderClear(renderer);
          working = false;
        }
      }
    }
    else
    {
      SDL_RenderDrawRect(renderer, &blocker);
      SDL_RenderPresent(renderer);
    }
    while (SDL_PollEvent(&event))
      switch (event.type)
      {
        case SDL_QUIT:
          quit = true;
          break;
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym)
          {
            case SDLK_SPACE:
              if (working)
              {
                SDL_RenderClear(renderer);
              }
              else
              {
                successCount = 0;
              }
              working = !working;
              break;
            case SDLK_ESCAPE:
              quit = true;
              break;
          }
          break;
      }
    }
  for (int i = 0; i < 10; i++)
  {
    SDL_DestroyTexture(texture[i]);
    SDL_FreeSurface(image[i]);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();

  return 0;
}
