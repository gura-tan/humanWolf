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
    PLAYER_ROLE_MAX
};
#define PLAYER_NONE 8//�\���Ɍ���+1����(1origin�̉e��)
#define MAX_PLAYER 3

typedef struct playe
{
    char username[30];
    int position;
    int alive;
    int vote;
}player;

#if 0 /* �ÓI�Ɋm�ۂ���ꍇ */
player user[MAX_PLAYER];
#else /* ���I�Ɋm�ۂ���ꍇ */
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
    "�l�T",
    "�肢�t",
    "����",
    "���l"
};
int deathflag = NULL;
char target[350] = "";
char buff[150] = "";
int f = 0;
int e = 0;
int targetPlayer = 0;
int maxPlayer = (int)MAX_PLAYER;


int
humanWolfMain(int argc, char* argv[])
{
    char buff[70];
    printf("�v���C���[�l������͂��Ă�������(�ő�8�l)---");
    maxPlayer = getNum();

    user = malloc(sizeof(player)*maxPlayer);

    for (int e = 0; e < maxPlayer; e++ )
    {
        sprintf_s(buff, 70, "%dP", e + 1);
        strcpy_s(user[e].username, 20, buff );
        user[e].alive = 1;
        user[e].vote = 0;
    }

    

    getPosition();

    for (int f = 0; f < maxPlayer; f++)//debug
    {
        printf("debug:%d,%d,%s\n", f, user[f].position,positionName[user[f].position]);
    }

    openPosition();
    while (1)
    {
        nightTurn();

        clearScreen();
        printf("���͂悤�������܂��I");

        dayTurn();

        clearScreen();
        printf("��ɓ���܂��B���₷�݂Ȃ���...\n");
    }
        
    return 0;
}

static void
getPosition(void)
{
    int jinroMax = (int)1;

    srand(time(NULL) * 3);
    int getPositionFlag[PLAYER_ROLE_MAX];
    memset(getPositionFlag, 0, sizeof(getPositionFlag));

    for (int i = 0; i < maxPlayer; i++)
    {
        int bRetry = 0;

        do
        {
            bRetry = 0;
            user[i].position = rand() % PLAYER_ROLE_MAX; /* �܂��̓|�W�V�������擾 */

            if( (user[i].position == PLAYER_JINRO) &&
                (jinroMax <= getPositionFlag[PLAYER_JINRO]) )
            { /* �l�T������I�[�o�[ */
                bRetry = 1;
            }
            else
            {
                if( (PLAYER_ROLE_MAX <= i) )
                {
                    /* ��E����葽���Ȃ�����t���Oon�ɂ��Ȃ��Ă��� */
                }
                else
                { /* ��E����菭�Ȃ��ꍇ�͏d���`�F�b�N  */
                    /* �������������t���OON */
                    if( getPositionFlag[user[i].position] && !(getPositionFlag[i] <= (int)(maxPlayer - 1) / (int)2))
                    {
                        bRetry = 1; /* �����������*/
                    }
                    else
                    {
                        /* ok!!! */
                    }
                }

            }
        } while (bRetry); /* �����ɍ��v���Ȃ��ꍇ�͌J��Ԃ� */

        getPositionFlag[user[i].position]++;
    }
}

static void openPosition(void)
{
    char collect;
    char sp = '�@';


    for (int a = 0; a < maxPlayer; a++)
    {
        printf("%s����ASpace�������Ă��������B\n", user[a].username);
        waitKey(' ');

        printf("���Ȃ��̖�E��%s�ł��B\n", positionName[user[a].position]);
        printf("Space�������Ă��������B\n\n");
        waitKey(' ');
        clearScreen();
    }
}

static void nightTurn(void)
{
   
    for (int i = 0; i < maxPlayer;i++)
    {
        clearScreen();
        printf("%s����ASpace�������Ă�������\n", user[i].username);
        waitKey(' ');
        printf("%s�����%s�ł�\n", user[i].username, positionName[user[i].position]);

        if (user[i].alive)
        {
            target[0] = '\0';
            switch (user[i].position)
            {
                case PLAYER_JINRO://�l�T�̏ꍇ
                    for (f = 0; f < maxPlayer; f++)
                    {
                        if (user[f].alive && i != f && !user[f].position == PLAYER_JINRO)//�����Ǝ����A����ł���l�͗�O
                        {
                            sprintf_s(buff, 150, "%d:%s\n",f + 1,user[f].username);
                            strcat(target,buff);
                        }
                    }
                    sprintf(buff, "%d:�P�����Ȃ�\n", PLAYER_NONE + 1);
                    strcat(target, buff);
                    printf("�P������l��I��ł�������---\n%s",target);
                    targetPlayer = getTarget() - 1;
                    if (targetPlayer == PLAYER_NONE)
                    {
                        printf("���̖���X�L�b�v���܂�...Space�������A���܂ł��҂����������B\n");
                        deathflag = NULL;
                        waitKey(' ');
                        break;
                    }
                    
                    printf("%s���U�����܂�...Space�������A���܂ł��҂����������B\n",user[targetPlayer].username);
                    waitKey(' ');
                    if (targetPlayer != PLAYER_NONE)//�l�T���ݏ���
                    {
                        user[targetPlayer].alive = 0;
                        deathflag = targetPlayer;
                    }
                    break;

                case PLAYER_PROPHET://�肢�t�̏ꍇ
                    /* �Ώۂ̐l�����X�g�A�b�v */
                    for (f = 0; f < maxPlayer; f++)
                    {
                        if (user[f].alive && i != f)
                        { /* �����ĂāA�����ȊO */
                            sprintf_s(buff, 150, "%d:%s\n", f + 1, user[f].username);
                            strcat(target, buff);
                        }
                    }
                    sprintf(buff, "%d:���Ȃ�\n", PLAYER_NONE + 1);
                    strcat(target, buff);
                    printf("�肤�l��I��ŉ�����---\n%s",target);
                    targetPlayer = getTarget() - 1;
                    if (targetPlayer)
                    {
                        sprintf(buff,"�l�T�ł͂���܂���");
                    }
                    else
                    {
                        sprintf(buff, "�l�T�ł�");
                    }
                    if (targetPlayer == PLAYER_NONE)
                    {
                        printf("���̖���X�L�b�v���܂�...Space�������A���܂ł��҂����������B\n");
                        deathflag = NULL;
                        waitKey(' ');
                        break;
                    }
                    printf("%s��%s!\n",user[targetPlayer].username,buff);
                    printf("Space�������A���܂ł��҂���������...\n");
                    waitKey(' ');
                    break;

                case PLAYER_THIEF://�����̏ꍇ
                    for (f = 0; f < maxPlayer; f++)
                    {
                        if (user[f].alive && i != f)
                        {
                            sprintf_s(buff, 150, "%d:%s\n", f + 1, user[f].username);
                            strcat(target, buff);
                        }
                    }
                    sprintf(buff, "%d:�������Ȃ�\n", PLAYER_NONE + 1);
                    strcat(target, buff);
                    printf("��������l��I��ŉ�����---\n%s", target);
                    targetPlayer = getTarget() - 1;
                    if (targetPlayer == PLAYER_NONE)
                    {
                        printf("���̖���X�L�b�v���܂�...Space�������A���܂ł��҂����������B\n");
                        waitKey(' ');
                        break;
                    }
                    e = user[i].position;
                    user[i].position = user[targetPlayer].position;
                    user[targetPlayer].position = e;
                    printf("%s�Ɩ�E���������܂�...Space�������Ē��܂ł��҂����������B\n",user[targetPlayer].username);
                    waitKey(' ');
                    break;

                case PLAYER_VILLAGER:
                    printf("Space�������Ē��܂ł��҂����������B\n");
                    waitKey(' ');
                    break;
                    

            }
        }
        else
        {

            printf("���Ȃ��͎��ɂ܂����BSpace�������Ē��܂ł��҂����������B\n");
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
        printf("\n�l�T�w�c�̏����ł�!!\nSpace�������ďI�����܂��B\n");
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
    if (deathflag != NULL)
    {
        printf("���%s���񂪏P������܂���...\n\n",user[deathflag].username);
    }
    else
    {
        printf("�����͂�����E����܂���ł����B\n\n");
    }
    printf("����ł͘b���������n�߂܂��B���[���n�߂�ɂ́ASpace�������ĉ������B\n");
    waitKey(' ');

    printf("����ł͓��[�ɓ���܂��B");
    
    for (int i = 0;i < maxPlayer;i++)
    {
        clearScreen();
        target[0] = NULL;
        printf("%s����ASpace�������Ă�������\n", user[i].username);
        waitKey(' ');

        if (user[i].alive)
        {
            for (int f = 0; f < maxPlayer; f++)
            {
                if (user[f].alive && i != f)
                {
                    sprintf_s(buff, 150, "%d:%s\n", f + 1, user[f].username);
                    strcat(target, buff);
                }
            }
            sprintf(buff, "%d:���[���Ȃ�\n", PLAYER_NONE + 1);
            strcat(target, buff);
            printf("���[����l��I��ł�������---\n%s", target);
            targetPlayer = getTarget() - 1;
            if (targetPlayer == PLAYER_NONE)
            {
                printf("���̓��[���X�L�b�v���܂�...Space�������A���҂����������B\n");
                waitKey(' ');
                continue;
            }

            printf("%s�ɓ��[���܂�...Space�������A���҂����������B\n", user[targetPlayer].username);
            user[targetPlayer].vote += 1;
            waitKey(' ');
            continue;
        }else
        {

            printf("���Ȃ��͎��ɂ܂����BSpace�������Ă��҂����������B\n");
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
    printf("������%s���񂪒Ǖ�����܂�...(%d�[)\nSpace�������ĉ������B\n",user[maxVote].username,user[maxVote].vote);
    user[maxVote].alive = 0;
    waitKey(' ');
    if (user[maxVote].position == PLAYER_JINRO)
    {
        printf("\n�s���w�c�̏����ł�!!\nSpace�������ďI�����܂��B\n");
        waitKey(' ');
        exit(0);
    }
    else if (aliveCnt <= 1)
    {
        printf("\n�l�T�w�c�̏����ł�!!Space�������ďI�����܂��B\n");
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
    } while ( (num != PLAYER_NONE + 1) && (num < 0 || maxPlayer + 1 < num) );//getKey�œ��͂ł��镶���̐���

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
    } while ( (num < 0 || 9 < num) );//getKey�œ��͂ł��镶���̐���

    return num;
}
