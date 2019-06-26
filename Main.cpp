#include "DxLib.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define OBJECT_SIZE 32
#define SHOT_SIZE 5

#define ENEMY_NUM_I 3
#define ENEMY_NUM_J 10
#define PLAYER_SHOT_NUM 20//�v���C���[�̒e�̐�
#define ENEMY_SHOT_NUM 200//�G�̒e�̐�

//�v���C���[���
int player_x, player_y;
int player_img;
int player_shot_x[PLAYER_SHOT_NUM], player_shot_y[PLAYER_SHOT_NUM];
int player_shot_flag[PLAYER_SHOT_NUM] = { 0, };
int player_shot_interval = 0;
//�G�l�~�[���
int enemy_x[ENEMY_NUM_I][ENEMY_NUM_J], enemy_y[ENEMY_NUM_I][ENEMY_NUM_J];
int enemy_flag[ENEMY_NUM_I][ENEMY_NUM_J];
int enemy_img[3];

//�Q�[���v���C���̌v��
int stage_time = 0;

//�v���C���[�̃L�[����+�͂ݏo���h�~
void Player_KeyOperation() {

    int movex = 0;
    
    switch (CheckHitKeyAll()) {
    case KEY_INPUT_RIGHT://�E
        movex = 3;
        break;
    
    case KEY_INPUT_SPACE:
        for (int shotf_i = 0; shotf_i < PLAYER_SHOT_NUM; shotf_i++)
            if(player_shot_interval == 0)
                if(player_shot_flag[shotf_i] == 0){//�󂢂Ă���e��T��
                    player_shot_flag[shotf_i] = 1;
                    player_shot_x[shotf_i] = player_x + OBJECT_SIZE / 2 - 10;
                    player_shot_y[shotf_i] = player_y - OBJECT_SIZE     + 20;
                    player_shot_interval++;
                }
        break;
    }

    player_x += movex;
}


//�n�߂�ۂ̖��@�̍����t
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //DxLib�֌W�̏����������ꗗ
    //(�ւ��`���炢�ɍl���Ă����Ă�����OK)
    ChangeWindowMode(TRUE); // �E�B���h�E���[�h�ɐݒ�
    SetWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);//��ʃT�C�Y�̐ݒ�
    DxLib_Init(); // DX���C�u��������������
    SetDrawScreen(DX_SCREEN_BACK); //�`���𗠉�ʂɐݒ�

    //DX���C�u����������/���s����(Init()==0)�Ƃ��ɃG���[��f���ďI��������
    if (DxLib_Init() == -1)return -1;			
    
    /*====����������======*/
    player_x = WINDOW_WIDTH / 2;
    player_y = WINDOW_HEIGHT - OBJECT_SIZE - 3;//�Ō��-�R�͔������p
    player_img = LoadGraph("img/player.png");

    for (int enemy_i = 0; enemy_i < ENEMY_NUM_I; enemy_i++) {
        for (int enemy_j = 0; enemy_j < ENEMY_NUM_J; enemy_j++) {
            //�G�l�~�[��x���W         = �G�l�~�[�̎����W(+��)           + �������p
            enemy_x[enemy_i][enemy_j] = enemy_j * (OBJECT_SIZE + 20)    + 80 ;
            //�G�l�~�[��y���W =         �E�B���h�E�T�C�Y   - �G�S�̂̏c��(+��)               - �������p    + ���ۂ̍��W�l(+��)
            enemy_y[enemy_i][enemy_j] = WINDOW_HEIGHT      - ENEMY_NUM_J * (OBJECT_SIZE - 3) - 20          + (OBJECT_SIZE + 3) * enemy_i;
            enemy_flag[enemy_i][enemy_j] = 1;
        }
    }
    enemy_img[0] = LoadGraph("img/enemy1.png");
    enemy_img[1] = LoadGraph("img/enemy2.png");
    enemy_img[2] = LoadGraph("img/enemy3.png");

    /*======�Q�[���v���C========*/
    //Esc�L�[��������Ă��Ȃ���(==0�̂Ƃ�)&�~�{�^����������Ă��Ȃ���Ζ������[�v����B
    while (FALSE) {
        ProcessMessage();
        ClearDrawScreen(); //��ʂ��N���A

        stage_time++;

        
        Player_KeyOperation();//�v���C���[�̃L�[����

        
        //�G�l�~�[�̏���
        int colornum = 0;
        int enemy_imgbuf;//�������̃C���[�W�o�b�t�@�p�ϐ�
        for (int enemy_i = 0; enemy_i < ENEMY_NUM_I; enemy_i++) {
            for (int enemy_j = 0; enemy_j < ENEMY_NUM_J; enemy_j++) {
                if (enemy_flag[enemy_i][enemy_j] == 1) {
                    //�`�悷��G�̐F����
                    switch (colornum % 3) {
                    case 0:enemy_imgbuf = enemy_img[0]; break;
                    case 1:enemy_imgbuf = enemy_img[1]; break;
                    case 2:enemy_imgbuf = enemy_img[2]; break;
                    }
                    
                    DrawGraph(enemy_x[enemy_i][enemy_j], enemy_y[enemy_i][enemy_j], enemy_imgbuf, TRUE);

                    //�X�e�[�W���Ԃ�60ms�o�߂��邲�ƂɈړ�
                    if (stage_time % 60 == 0) enemy_y[enemy_i][enemy_j] -= 20;

                    /******�e�Ƃ̂����蔻��*****/
                    for (int shotf_i = 0; shotf_i < PLAYER_SHOT_NUM; shotf_i++) {
                        if (player_shot_x[shotf_i] + SHOT_SIZE > enemy_x[enemy_i][enemy_j]  && player_shot_x[shotf_i] < enemy_x[enemy_i][enemy_j] +OBJECT_SIZE)
                            if (player_shot_y[shotf_i] < enemy_y[enemy_i][enemy_j] + OBJECT_SIZE){
                                enemy_flag[enemy_i][enemy_j] = 0;
                                player_shot_flag[shotf_i] = 0;
                            }
                    }
                }
                colornum++;
            }
        }

        //�v���C���[�̒e�̏���
        for (int shotf_i = 0; shotf_i < PLAYER_SHOT_NUM; shotf_i++) {
            if (player_shot_flag[shotf_i] == 1) {
                player_shot_y[shotf_i] -= 2;
                DrawBox(player_shot_x[shotf_i], player_shot_y[shotf_i], player_shot_x[shotf_i]+SHOT_SIZE,player_shot_y[shotf_i]+SHOT_SIZE,GetColor(255,255,255),true);
            }
        }

        //�e�̃C���^�[�o��
        if (player_shot_interval != 0) {
            player_shot_interval++;
            if (player_shot_interval == 30)player_shot_interval = 0;
        }

        
        DrawGraph(player_x, player_y, player_img, true);
        
        ScreenFlip();//����ʂ���\��ʂɔ��]
    }

    //DX���C�u�����̏I������
    DxLib_End();				

    //�v���O�����I��
    return 0;				 
}