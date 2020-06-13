#include <ncurses.h>
#include <unistd.h>
#include <vector>
#include <time.h>
#include <stdlib.h>

using namespace std;

#define tick 10000 // 틱레이트 마이크로초 단위 1초에 100틱

bool fail; // 게임 실패 여부
int stage;
char dir; // 방향 U D L R
int movetimer; // move 함수용 타이머 시간 저장
vector<int> snakex;
vector<int> snakey; // 뱀 x, y좌표
int g_itemx, g_itemy; // growth item x,y 좌표
int p_itemx, p_itemy; // poison item x,y 좌표
int gate1_x, gate1_y; // gate1의 x,y좌표
int gate2_x, gate2_y; // gate2의 x,y좌표

int map[4][21][21] = {
    {
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 1},// width: 10~12, heigth:10
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
    }
}; // [stage][행][열]) 일단은 스테이지 1만

void reset(); // 게임 시작시 함수 초기화
void input(); // 키 입력
void move(); // 입력 받은 키에 따라 이동
void show(); // 그래픽을 보여주는 함수
void growth_item(); // growth item 만드는 함수
void poison_item(); // poison item 만드는 함수
void gate(); //gate를 만드는 함수

int main()
{
    initscr(); // 터미널 초기화
    start_color(); // 컬러모드 사용
    nodelay(stdscr, true); // 키 입력 기다리지 않음
    noecho(); // 입력받은 키 출력하지 않음
    curs_set(0); // 커서 숨기기
    cbreak(); // 입력받은 키 바로 활용
    keypad(stdscr, TRUE); // 특수 키 사용 가능
    reset();
    while (!fail)
    {
        input();
        move();
        show();
        usleep(tick);
    }
    endwin(); // fail 시 종료
    return 0;
}

void reset()
{
    fail = false;
    stage = 0;
    movetimer = 0;

    snakex.push_back(9);
    snakey.push_back(9);
    snakex.push_back(10);
    snakey.push_back(9);
    snakex.push_back(11);
    snakey.push_back(9);
    dir = 'L';
    growth_item();
    poison_item();
    gate();
}

void input()
{
    int key = getch();

    switch (key)
    {
    case KEY_UP:
        if (dir == 'D')
            fail = true;
        else
            dir = 'U';
        break;
    case KEY_DOWN:
        if (dir == 'U')
            fail = true;
        else
            dir = 'D';
        break;
    case KEY_LEFT:
        if (dir == 'R')
            fail = true;
        else
            dir = 'L';
        break;
    case KEY_RIGHT:
        if (dir == 'L')
            fail = true;
        else
            dir = 'R';
        break;
    }
}

void move()
{
    if (map[stage][g_itemx][g_itemy] == 3 || map[stage][g_itemx][g_itemy] == 4) { // growth item과 충돌했을 경우
        growth_item(); //새로운 item 추가
    }
    if (map[stage][p_itemx][p_itemy] == 3 || map[stage][p_itemx][p_itemy] == 4) { // posion item과 충돌했을 경우
        poison_item(); //새로운 item 추가
    }
    movetimer += 1;
    if (movetimer > 50)
    {
        int last = snakex.size();

        map[stage][snakey[last - 1]][snakex[last - 1]] = 0; // 몸통 마지막 좌표 지우기
        for (int i = last - 1; i > 0; i--) // 몸통 앞으로 한 칸 씩 복제
        {
            snakex[i] = snakex[i - 1];
            snakey[i] = snakey[i - 1];
        }

        switch (dir) // 머리 좌표 이동
        {
        case 'U':
            snakey[0]--;
            break;
        case 'D':
            snakey[0]++;
            break;
        case 'L':
            snakex[0]--;
            break;
        case 'R':
            snakex[0]++;
            break;
        }

        if (map[stage][snakey[0]][snakex[0]] == 4 || map[stage][snakey[0]][snakex[0]] == 1) // Game Rule #1 실패 조건
        {
            fail = true;
        }
        else
        {
            map[stage][snakey[0]][snakex[0]] = 3; // map 에 수정된 snake의 좌표 전달
            for (int i = 1; i < last; i++)
            {
                map[stage][snakey[i]][snakex[i]] = 4;
            }
        }
        movetimer = 0;
    }
}

void show()
{
    clear();
    // stage1
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 21; j++) {
            printw("\n");
            for (int z = 0; z < 21; z++) {
                if (i == 0) {
                    if (map[i][j][z] == 2) {
                        printw("+"); //lmmune wall
                    }
                    else if (map[i][j][z] == 1) {
                        printw("-"); //wall
                    }
                    else if (map[i][j][z] == 0) {
                        printw(" ");
                    }
                    else if (map[i][j][z] == 3) {
                        printw("0"); //head
                    }
                    else if (map[i][j][z] == 4) {
                        printw("o"); //body
                    }
                    else if (map[i][j][z] == 5) {
                        printw("F"); //food
                    }
                    else if (map[i][j][z] == 6) {
                        printw("P"); //poison
                    }
                    else if (map[i][j][z] == 7) {
                        printw("G"); //gate
                    }
                }
            }
        }
    }
    refresh();
}
void growth_item() {
    int r = 0; //난수 생성 변수
    int food_crush_on = 0; //item이 뱀과 충돌했을 경우 1
    while (1) {
        food_crush_on = 0;
        srand((unsigned)time(NULL) + r); //난수표생성
        g_itemx = (rand() % 19) + 1; //난수를 좌표값에
        g_itemy = (rand() % 19) + 1;

        if (map[stage][g_itemx][g_itemy] != 0) { //item이 맵에서 0이 아닌 부분과 만나면
            food_crush_on = 1; //on
            r++;
        }
        if (food_crush_on == 1) continue; //부딪히면 while문 다시 시작

        map[stage][g_itemx][g_itemy] = 5; //안부딪히면 좌표에 item을 찍음
        break;

    }

}
void poison_item() {
    int r = 3;
    int food_crush_on = 0;
    while (1) {
        food_crush_on = 0;
        srand((unsigned)time(NULL) + r);
        p_itemx = (rand() % 19) + 1;
        p_itemy = (rand() % 19) + 1;

        if (map[stage][p_itemx][p_itemy] != 0) {
            food_crush_on = 1;
            r++;
        }
        if (food_crush_on == 1) continue;

        map[stage][p_itemx][p_itemy] = 6;
        break;

    }
}
void gate() {
    srand((unsigned)time(NULL));
    while (1) {
        gate1_x = (rand() % 22); //랜덤으로 gate1의 x,y좌표 설정
        gate1_y = (rand() % 22);
        if (map[stage][gate1_x][gate1_y] == 1) {  //난수가 wall이 될때까지 실행함
            map[stage][gate1_x][gate1_y] = 7;       // wall이면 gate로 변경
            break;
        }
    }
    srand((unsigned)time(NULL));
    while (1) {
        gate2_x = (rand() % 22); //랜덤으로 gate2의 x,y좌표 설정
        gate2_y = (rand() % 22);
        if (map[stage][gate2_x][gate2_y] == 1) {
            map[stage][gate2_x][gate2_y] = 7;
            break;
        }
    }
}