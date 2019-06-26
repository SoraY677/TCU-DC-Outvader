#include "DxLib.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define OBJECT_SIZE 32
#define SHOT_SIZE 5

#define ENEMY_NUM_I 3
#define ENEMY_NUM_J 10
#define PLAYER_SHOT_NUM 20//プレイヤーの弾の数
#define ENEMY_SHOT_NUM 200//敵の弾の数

//プレイヤー情報
int player_x, player_y;
int player_img;
int player_shot_x[PLAYER_SHOT_NUM], player_shot_y[PLAYER_SHOT_NUM];
int player_shot_flag[PLAYER_SHOT_NUM] = { 0, };
int player_shot_interval = 0;
//エネミー情報
int enemy_x[ENEMY_NUM_I][ENEMY_NUM_J], enemy_y[ENEMY_NUM_I][ENEMY_NUM_J];
int enemy_flag[ENEMY_NUM_I][ENEMY_NUM_J];
int enemy_img[3];

//ゲームプレイ中の計測
int stage_time = 0;

//プレイヤーのキー操作+はみ出し防止
void Player_KeyOperation() {

    int movex = 0;
    
    switch (CheckHitKeyAll()) {
    case KEY_INPUT_RIGHT://右
        movex = 3;
        break;
    
    case KEY_INPUT_SPACE:
        for (int shotf_i = 0; shotf_i < PLAYER_SHOT_NUM; shotf_i++)
            if(player_shot_interval == 0)
                if(player_shot_flag[shotf_i] == 0){//空いている弾を探す
                    player_shot_flag[shotf_i] = 1;
                    player_shot_x[shotf_i] = player_x + OBJECT_SIZE / 2 - 10;
                    player_shot_y[shotf_i] = player_y - OBJECT_SIZE     + 20;
                    player_shot_interval++;
                }
        break;
    }

    player_x += movex;
}


//始める際の魔法の合言葉
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //DxLib関係の初期化処理一覧
    //(へぇ～ぐらいに考えておいてくれればOK)
    ChangeWindowMode(TRUE); // ウィンドウモードに設定
    SetWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);//画面サイズの設定
    DxLib_Init(); // DXライブラリ初期化処理
    SetDrawScreen(DX_SCREEN_BACK); //描画先を裏画面に設定

    //DXライブラリ初期化/失敗した(Init()==0)ときにエラーを吐いて終了させる
    if (DxLib_Init() == -1)return -1;			
    
    /*====初期化処理======*/
    player_x = WINDOW_WIDTH / 2;
    player_y = WINDOW_HEIGHT - OBJECT_SIZE - 3;//最後の-３は微調整用
    player_img = LoadGraph("img/player.png");

    for (int enemy_i = 0; enemy_i < ENEMY_NUM_I; enemy_i++) {
        for (int enemy_j = 0; enemy_j < ENEMY_NUM_J; enemy_j++) {
            //エネミーのx座標         = エネミーの実座標(+α)           + 微調整用
            enemy_x[enemy_i][enemy_j] = enemy_j * (OBJECT_SIZE + 20)    + 80 ;
            //エネミーのy座標 =         ウィンドウサイズ   - 敵全体の縦幅(+α)               - 微調整用    + 実際の座標値(+α)
            enemy_y[enemy_i][enemy_j] = WINDOW_HEIGHT      - ENEMY_NUM_J * (OBJECT_SIZE - 3) - 20          + (OBJECT_SIZE + 3) * enemy_i;
            enemy_flag[enemy_i][enemy_j] = 1;
        }
    }
    enemy_img[0] = LoadGraph("img/enemy1.png");
    enemy_img[1] = LoadGraph("img/enemy2.png");
    enemy_img[2] = LoadGraph("img/enemy3.png");

    /*======ゲームプレイ========*/
    //Escキーが押されていない間(==0のとき)&×ボタンが押されていなければ無限ループする。
    while (FALSE) {
        ProcessMessage();
        ClearDrawScreen(); //画面をクリア

        stage_time++;

        
        Player_KeyOperation();//プレイヤーのキー操作

        
        //エネミーの処理
        int colornum = 0;
        int enemy_imgbuf;//処理時のイメージバッファ用変数
        for (int enemy_i = 0; enemy_i < ENEMY_NUM_I; enemy_i++) {
            for (int enemy_j = 0; enemy_j < ENEMY_NUM_J; enemy_j++) {
                if (enemy_flag[enemy_i][enemy_j] == 1) {
                    //描画する敵の色分け
                    switch (colornum % 3) {
                    case 0:enemy_imgbuf = enemy_img[0]; break;
                    case 1:enemy_imgbuf = enemy_img[1]; break;
                    case 2:enemy_imgbuf = enemy_img[2]; break;
                    }
                    
                    DrawGraph(enemy_x[enemy_i][enemy_j], enemy_y[enemy_i][enemy_j], enemy_imgbuf, TRUE);

                    //ステージ時間が60ms経過するごとに移動
                    if (stage_time % 60 == 0) enemy_y[enemy_i][enemy_j] -= 20;

                    /******弾とのあたり判定*****/
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

        //プレイヤーの弾の処理
        for (int shotf_i = 0; shotf_i < PLAYER_SHOT_NUM; shotf_i++) {
            if (player_shot_flag[shotf_i] == 1) {
                player_shot_y[shotf_i] -= 2;
                DrawBox(player_shot_x[shotf_i], player_shot_y[shotf_i], player_shot_x[shotf_i]+SHOT_SIZE,player_shot_y[shotf_i]+SHOT_SIZE,GetColor(255,255,255),true);
            }
        }

        //弾のインターバル
        if (player_shot_interval != 0) {
            player_shot_interval++;
            if (player_shot_interval == 30)player_shot_interval = 0;
        }

        
        DrawGraph(player_x, player_y, player_img, true);
        
        ScreenFlip();//裏画面から表画面に反転
    }

    //DXライブラリの終了処理
    DxLib_End();				

    //プログラム終了
    return 0;				 
}