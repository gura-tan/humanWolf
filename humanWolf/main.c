#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<conio.h>

enum
{
    PLAYER_JINRO  ,
    PLAYER_PROPHET,
    PLAYER_THIEF  ,
    PLAYER_VILLAGER,
    PLAYER_KNIGHT,
    PLAYER_MADMAN,
    PLAYER_ROLE_MAX
};
#define PLAYER_NONE 8//表示に限り+1する(1originの影響)
#define MAX_PLAYER 6

typedef struct playe
{
    char username[30];
    int position;
    int alive;
    int vote;
   
}player;

#if 0 /* 静的に確保する場合 */
player user[MAX_PLAYER];
#else /* 動的に確保する場合 */
player *user;
#endif
static void nightTurn(void);
static void dayTurn(void);

static void getPosition(void);
static void openPosition(void);
static void waitKey(char);
static char getKey(void);
static int getNum(void);
static void clearScreen(void);
static int getTarget(void);
static void judgeVictory(void);

char positionName[PLAYER_ROLE_MAX][50] =
{
    "人狼",
    "占い師",
    "怪盗",
    "村人",
    "騎士",
    "狂人"
};
int deathflag = 15;
int knightNum = 15;
char target[350] = "";
char buff[150] = "";
int f = 0;
int e = 0;
int targetPlayer = 0;
int maxPlayer = (int)MAX_PLAYER;
int Pnumber = NULL;


int
humanWolfMain(int argc, char* argv[])
{
    char buff[70];
    printf("プレイヤー人数を入力してください(最大8人)---");
    maxPlayer = getNum();

    user = malloc(sizeof(player)*maxPlayer);

    for (int e = 0; e < maxPlayer; e++ )
    {
        sprintf_s(buff, 70, "%dP", e + 1);
        strcpy_s(user[e].username, 20, buff );
        user[e].alive = 1;
        user[e].vote = 0;
    }
    clearScreen();
    

    getPosition();
/*
    for (int f = 0; f < maxPlayer; f++)//debug
    {
        printf("debug:%d,%d,%s\n", f, user[f].position,positionName[user[f].position]);
    }
   */
    openPosition();
    while (1)
    {
        nightTurn();

        clearScreen();
        printf("おはようございます！");

        dayTurn();

        clearScreen();
        printf("夜に入ります。おやすみなさい...\n");
    }
        
    return 0;
}

static void
getPosition(void)
{
    int jinroMax = (int)0;//確実に一人人狼が出るので追加の人狼人数を指定
    int knightMax = (int)1;
    int madmanMax = (int)1;

    srand(time(NULL) * 3);
    int getPositionFlag[PLAYER_ROLE_MAX];
    memset(getPositionFlag, 0, sizeof(getPositionFlag));
    
    Pnumber = rand() % maxPlayer + 1;
    user[Pnumber].position = PLAYER_JINRO;

    for (int i = 0; i < maxPlayer; i++)
    {
        int bRetry = 0;
        do
        {
            bRetry = 0;
            if (i == Pnumber)
            {
                break;
            }
            user[i].position = rand() % PLAYER_ROLE_MAX; /* まずはポジションを取得 */
            
            if( (user[i].position == PLAYER_JINRO) &&
                (jinroMax <= getPositionFlag[PLAYER_JINRO]) )
            { 
                bRetry = 1;
            }
            //else if (user[i].position == PLAYER_JINRO)
            //{
            //    bRetry = 1;
            //}
            else if ((user[i].position == PLAYER_KNIGHT) &&
                (knightMax <= getPositionFlag[PLAYER_KNIGHT]))
            { /* 騎士が定員オーバー */
                bRetry = 1;
            }
            else if ((user[i].position == PLAYER_MADMAN) &&
                (knightMax <= getPositionFlag[PLAYER_MADMAN]))
            { /* 騎士が定員オーバー */
                bRetry = 1;
            }
            else
            {
                if( (PLAYER_ROLE_MAX <= i) )
                {
                    /* 役職数より多くなったらフラグonにしなくていい */
                }
                else
                { /* 役職数より少ない場合は重複チェック  */
                    /* 同じやつがいたらフラグON */
                    if( getPositionFlag[user[i].position] && !(getPositionFlag[i] <= (int)(maxPlayer - 1) / (int)2))
                    {
                        bRetry = 1; /* 同じやつがいた*/
                    }
                    else
                    {
                        /* ok!!! */
                    }
                }

            }
        } while (bRetry); /* 条件に合致しない場合は繰り返す */

        getPositionFlag[user[i].position]++;
    }
}

static void openPosition(void)
{
    char collect;
    char sp = '　';


    for (int a = 0; a < maxPlayer; a++)
    {
        printf("%sさん、Spaceを押してください。\n", user[a].username);
        waitKey(' ');

        printf("あなたの役職は%sです。\n", positionName[user[a].position]);
        printf("Spaceを押してください。\n\n");
        waitKey(' ');
        clearScreen();
    }
}

static void nightTurn(void)
{

    for (int i = 0; i < maxPlayer; i++)
    {
        clearScreen();
        printf("%sさん、Spaceを押してください\n", user[i].username);
        waitKey(' ');
        printf("%sさんは%sです\n", user[i].username, positionName[user[i].position]);

        if (user[i].alive && deathflag != i)
        {
            target[0] = '\0';
            switch (user[i].position)
            {
            case PLAYER_JINRO://人狼の場合
                for (f = 0; f < maxPlayer; f++)
                {
                    if (user[f].alive && i != f && !user[f].position == PLAYER_JINRO)//味方と自分、死んでいる人は例外
                    {
                        sprintf_s(buff, 150, "%d:%s\n", f + 1, user[f].username);
                        strcat(target, buff);
                    }
                }
                sprintf(buff, "%d:襲撃しない\n", PLAYER_NONE + 1);
                strcat(target, buff);
                printf("襲撃する人を選んでください---\n%s", target);
                targetPlayer = getTarget() - 1;
                if (targetPlayer == PLAYER_NONE)
                {
                    printf("この夜をスキップします...Spaceを押し、朝までお待ちください。\n");
                    deathflag = -1;
                    waitKey(' ');
                    break;
                }

                printf("%sを攻撃します...Spaceを押し、朝までお待ちください。\n", user[targetPlayer].username);
                waitKey(' ');
                //user[targetPlayer].alive = 0;
                deathflag = targetPlayer;
                break;

            case PLAYER_PROPHET://占い師の場合
                /* 対象の人をリストアップ */
                for (f = 0; f < maxPlayer; f++)
                {
                    if (user[f].alive && i != f)
                    { /* 生きてて、自分以外 */
                        sprintf_s(buff, 150, "%d:%s\n", f + 1, user[f].username);
                        strcat(target, buff);
                    }
                }
                sprintf(buff, "%d:占わない\n", PLAYER_NONE + 1);
                strcat(target, buff);
                printf("占う人を選んで下さい---\n%s", target);
                targetPlayer = getTarget() - 1;
                if (user[targetPlayer].position != PLAYER_JINRO)
                {
                    sprintf(buff, "人狼ではありません");
                }
                else
                {
                    sprintf(buff, "人狼です");
                }
                if (targetPlayer == PLAYER_NONE)
                {
                    printf("この夜をスキップします...Spaceを押し、朝までお待ちください。\n");
                    deathflag = NULL;
                    waitKey(' ');
                    break;
                }
                printf("%sは%s!\n", user[targetPlayer].username, buff);
                printf("Spaceを押し、朝までお待ちください...\n");
                waitKey(' ');
                break;

            case PLAYER_THIEF://怪盗の場合
                for (f = 0; f < maxPlayer; f++)
                {
                    if (user[f].alive && i != f)
                    {
                        sprintf_s(buff, 150, "%d:%s\n", f + 1, user[f].username);
                        strcat(target, buff);
                    }
                }
                sprintf(buff, "%d:交換しない\n", PLAYER_NONE + 1);
                strcat(target, buff);
                printf("交換する人を選んで下さい---\n%s", target);
                targetPlayer = getTarget() - 1;
                if (targetPlayer == PLAYER_NONE)
                {
                    printf("この夜をスキップします...Spaceを押し、朝までお待ちください。\n");
                    waitKey(' ');
                    break;
                }
                e = user[i].position;
                user[i].position = user[targetPlayer].position;
                user[targetPlayer].position = e;
                printf("%sと役職を交換します...Spaceを押して朝までお待ちください。\n", user[targetPlayer].username);
                waitKey(' ');
                break;

            case PLAYER_VILLAGER:
                printf("Spaceを押して朝までお待ちください。\n");
                waitKey(' ');
                break;

            case PLAYER_MADMAN:
                printf("Spaceを押して朝までお待ちください。\n");
                waitKey(' ');
                break;

            case PLAYER_KNIGHT:
                for (f = 0; f < maxPlayer; f++)
                {
                    if (user[f].position != PLAYER_KNIGHT && user[f].alive)
                    {
                        sprintf_s(buff, 150, "%d:%s\n", f + 1, user[f].username);
                        strcat(target, buff);
                    }
                }
                sprintf(buff, "%d:守らない\n", PLAYER_NONE + 1);
                strcat(target, buff);
                printf("守る人を選んで下さい---\n%s", target);
                targetPlayer = getTarget() - 1;
                if (targetPlayer == PLAYER_NONE)
                {
                    printf("この夜をスキップします...Spaceを押し、朝までお待ちください。\n");
                    waitKey(' ');
                    break;
                }
                user[targetPlayer].alive = 1;
                knightNum = targetPlayer;
                printf("%sを守ります...Spaceを押して朝までお待ちください。\n", user[targetPlayer].username);
                waitKey(' ');
                break;


            }
        }
        else
        {

            printf("あなたは死にました。Spaceを押して朝までお待ちください。\n");
            waitKey(' ');

        }
    }
    int aliveCnt = 0;

    for (int i = 0; i < maxPlayer; i++)
    {
        if (user[i].alive && user[i].position != PLAYER_JINRO)
        {
            aliveCnt += 1;
        }
    }
    if (aliveCnt <= 1)
    {
        printf("\n人狼陣営の勝利です!!\nSpaceを押して終了します。\n");
        waitKey(' ');
        exit(0);
    }
    if (!user[Pnumber].alive)
    {
        printf("\n市民陣営の勝利です!!\nSpaceを押して終了します。\n");
        waitKey(' ');
        exit(0);
    }
}
    static void dayTurn(void)
    {
        buff[0] = NULL;
        target[0] = NULL;
        int maxVote = 0;
        int aliveCnt = 0;
        if(deathflag == -1)
        {
            printf("今日はだれも殺されませんでした。\n\n");
        }
        else if (deathflag != knightNum)
        {
            printf("夜に%sさんが襲撃されました...\n\n", user[deathflag].username);
            user[deathflag].alive = 0;
        }
        else if (deathflag == knightNum)
        {
            printf("%sさんが騎士に防衛されました!\n\n", user[knightNum].username);
            user[knightNum].alive = 1;//殺される前に騎士が動いた場合処理が必要
        }
        
        printf("それでは話し合いを始めます。投票を始めるには、Spaceを押して下さい。\n");
        waitKey(' ');

        printf("それでは投票に入ります。");

        for (int i = 0; i < maxPlayer; i++)
        {
            clearScreen();
            target[0] = NULL;
            printf("%sさん、Spaceを押してください\n", user[i].username);
            waitKey(' ');

            if (user[i].alive)
            {
                for (int f = 0; f < maxPlayer; f++)
                {
                    if (user[f].alive)
                    {
                        sprintf_s(buff, 150, "%d:%s\n", f + 1, user[f].username);
                        strcat(target, buff);
                    }
                }
                sprintf(buff, "%d:投票しない\n", PLAYER_NONE + 1);
                strcat(target, buff);
                printf("投票する人を選んでください---\n%s", target);
                targetPlayer = getTarget() - 1;
                if (targetPlayer == PLAYER_NONE)
                {
                    printf("この投票をスキップします...Spaceを押し、お待ちください。\n");
                    waitKey(' ');
                    continue;
                }

                printf("%sに投票します...Spaceを押し、お待ちください。\n", user[targetPlayer].username);
                user[targetPlayer].vote += 1;
                waitKey(' ');
                continue;
            }
            else
            {

                printf("あなたは死にました。Spaceを押してお待ちください。\n");
                waitKey(' ');

            }
        }
        for (int i = 0; i < maxPlayer; i++)
        {
            if (maxVote < user[i].vote && user[i].alive)
            {
                maxVote = i;

            }
            if (user[i].alive && user[i].position != PLAYER_JINRO)
            {
                aliveCnt += 1;
            }
        }
        clearScreen();
        printf("今日は%sさんが追放されます...(%d票)\nSpaceを押して下さい。\n", user[maxVote].username, user[maxVote].vote);
        user[maxVote].alive = 0;
        waitKey(' ');
        if (user[maxVote].position == PLAYER_JINRO)
        {
            printf("\n市民陣営の勝利です!!\nSpaceを押して終了します。\n");
            waitKey(' ');
            exit(0);
        }
        else if (aliveCnt <= 1)
        {
            printf("\n人狼陣営の勝利です!!Spaceを押して終了します。\n");
            waitKey(' ');
            exit(0);
        }
    }

static void waitKey(char key)
{
     while (1)
    {
        if (_kbhit())
        {
            if (_getch() == key)
            {
                break;
            }
        }
    }
}

static char getKey(void)
{
    char key;

    while (1)
    {
        if (_kbhit())
        {
            if ( (key = _getch()) )
            {
                break;
            }
        }
    }

    return key;
}

static void clearScreen(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

static int getTarget(void)
{
    int cNum,num;

    do
    {
        cNum = getKey();
        num = cNum - '0';
    } while ( (num != PLAYER_NONE + 1) && (num < 0 || maxPlayer + 1 < num) );//getKeyで入力できる文字の制限

    return num;
}

static void judgeVictory(void)
{}

static int getNum(void) 
{
    int cNum,num;

    do
    {
        cNum = getKey();
        num = cNum - '0';
    } while ( (num < 0 || 9 < num) );//getKeyで入力できる文字の制限

    return num;
}
