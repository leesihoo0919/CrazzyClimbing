//GameMode2.cpp
#include "Game.h"
#include "GameMode2.h"
#include <vector>
#include <cstdlib>
#include <chrono>

using namespace std;

Uint32 startTime = 0;
Uint32 gameoverTime = 0;

//인트로
SDL_Texture* g_texture_intro;
SDL_Rect g_source_rectangle_intro;
SDL_Rect g_destination_rectangle_intro;

//플레이버튼
SDL_Texture* g_texture_play;
SDL_Rect g_source_rectangle_play;
SDL_Rect g_destination_rectangle_play;

//뒤로가기버튼
SDL_Surface* introBack_surface;
SDL_Texture* introBack_texture;
SDL_Rect introBack_rect;
SDL_Rect introBack_dest_rect;

//레디
SDL_Surface* ready_surface;
SDL_Texture* ready_texture;
SDL_Rect ready_rect;
SDL_Rect ready_dest_rect;

//시작
SDL_Surface* start_surface;
SDL_Texture* start_texture;
SDL_Rect start_rect;
SDL_Rect start_dest_rect;

// 배경 이미지 크기
const int BACKGROUND_WIDTH = 800;
const int BACKGROUND_HEIGHT = 3000;

// bg
SDL_Surface* bg_surface;
SDL_Texture* bg_texture;
SDL_Rect bg_rect;
SDL_Rect bg_dest_rect;
int backgroundY; // 배경 이미지의 Y 좌표

// wall
SDL_Surface* wall_surface;
SDL_Texture* wall_texture;
SDL_Rect wall_rect;
SDL_Rect wall_dest_rect;

const int WALL_WIDTH = 400;
const int WALL_HEIGHT = 600;

// hold
vector<string> hold_paths = { "../src/redHold.png", "../src/greenHold.png", "../src/blueHold.png" };
vector<SDL_Surface*> hold_surfaces;
vector<SDL_Texture*> hold_textures;
vector<SDL_Rect> hold_rects;
vector<SDL_Rect> hold_dest_rects;
int leftHoldY = 475;
int rightHoldY = 375;

int hold[500]; // 1:red 2:green 3:blue
int f_state; // 1:red 2: green 3:blue
bool isLeftUser = true;
string nextHoldPath = "";
int prevHoldIndex = -1;

// leftUser
SDL_Surface* leftUser_surface;
SDL_Texture* leftUser_texture;
SDL_Rect leftUser_rect;
SDL_Rect leftUser_dest_rect;

// rightUser
SDL_Surface* rightUser_surface;
SDL_Texture* rightUser_texture;
SDL_Rect rightUser_rect;
SDL_Rect rightUser_dest_rect;

//게이지
int gauge = 150; // 초기 게이지 값
int gaugeDecreaseRate = 1; // 게이지 감소 속도
int gaugeDecreaseRate2 = 2; //50개 지나면 게이지 감소 늘리기

//엔딩사진
SDL_Texture* g_texture_ending;
SDL_Rect g_source_rectangle_ending;
SDL_Rect g_destination_rectangle_ending;

//홈버튼
SDL_Texture* g_texture_home;
SDL_Rect g_source_rectangle_home;
SDL_Rect g_destination_rectangle_home;

//다시시작버튼
SDL_Texture* g_texture_retry;
SDL_Rect g_source_rectangle_retry;
SDL_Rect g_destination_rectangle_retry;

Mode2::Mode2() {
	g_flag_running = true;
	cur_i = 1;
	prevHold = 0;

	//STATE
	tutorial = true;
	ready = false;
	start = false;
	game_start = false;
	game_over = false;
	game_ending = false;

	//인트로 튜토리얼
	SDL_Surface* intro_surface = IMG_Load("../src/intro.png");
	g_texture_intro = SDL_CreateTextureFromSurface(g_renderer, intro_surface);
	SDL_FreeSurface(intro_surface);
	SDL_QueryTexture(g_texture_intro, NULL, NULL, &g_source_rectangle_intro.w, &g_source_rectangle_intro.h);
	g_source_rectangle_intro = { 0, 0, 800, 600 };
	g_destination_rectangle_intro = { 95, 40, 620, 480 };

	//play버튼
	SDL_Surface* play_surface = IMG_Load("../src/play.png");
	g_texture_play = SDL_CreateTextureFromSurface(g_renderer, play_surface);
	SDL_FreeSurface(play_surface);
	SDL_QueryTexture(g_texture_play, NULL, NULL, &g_source_rectangle_play.w, &g_source_rectangle_play.h);
	g_source_rectangle_play = { 0, 0, 160, 80 };
	g_destination_rectangle_play = { 320, 510, 160, 80 };

	//뒤로가기
	introBack_surface = IMG_Load("../src/back.png");
	introBack_texture = SDL_CreateTextureFromSurface(g_renderer, introBack_surface);
	SDL_FreeSurface(introBack_surface);
	introBack_rect = { 0, 0, introBack_surface->w,introBack_surface->h };
	introBack_dest_rect = { 10, 10, introBack_surface->w, introBack_surface->h };

	//레디
	ready_surface = IMG_Load("../src/ready.png");
	ready_texture = SDL_CreateTextureFromSurface(g_renderer, ready_surface);
	SDL_FreeSurface(ready_surface);
	ready_rect = { 0, 0, ready_surface->w,ready_surface->h };
	ready_dest_rect = { 10, 10, ready_surface->w, ready_surface->h };

	//스타트
	start_surface = IMG_Load("../src/start.png");
	start_texture = SDL_CreateTextureFromSurface(g_renderer, start_surface);
	SDL_FreeSurface(start_surface);
	start_rect = { 0, 0, start_surface->w,start_surface->h };
	start_dest_rect = { 10, 10, start_surface->w, start_surface->h };

	// 배경 이미지 로드
	bg_surface = IMG_Load("../src/bg_mode2.png");
	bg_texture = SDL_CreateTextureFromSurface(g_renderer, bg_surface); // GPU로 옮기기 
	SDL_FreeSurface(bg_surface);
	backgroundY = -2400; 	// 배경 이미지 초기 위치 설정 (화면 위로)

	// wall 이미지 로드
	wall_surface = IMG_Load("../src/wall_mode2.png");
	wall_texture = SDL_CreateTextureFromSurface(g_renderer, wall_surface); // GPU로 옮기기 
	SDL_FreeSurface(wall_surface);
	wall_rect = { 0, 0, 400, 600 };
	wall_dest_rect = { 200, 0, 400, 600 };

	//홀드 이미지 로드
	srand((unsigned)time(NULL)); // srand는 한 번만 호출해야 합니다.
	for (int i = 0; i < 500; i++) {
		int random_hold_idx;
		do {
			random_hold_idx = rand() % 3; // 0~3 중 하나의 인덱스를 무작위로 선택
		} while (random_hold_idx == prevHoldIndex); // 현재 인덱스가 이전 인덱스와 같으면 새로운 인덱스를 계속 생성합니다
		
		prevHoldIndex = random_hold_idx;
		string hold_path = hold_paths[random_hold_idx];
		SDL_Surface* hold_surface = IMG_Load(hold_path.c_str());
		SDL_Texture* hold_texture = SDL_CreateTextureFromSurface(g_renderer, hold_surface);
		SDL_Rect hold_rect = { 0, 0, hold_surface->w, hold_surface->h };

		hold_surfaces.push_back(hold_surface);
		hold_textures.push_back(hold_texture);
		hold_rects.push_back(hold_rect);

		if (i % 2 == 0) {
			SDL_Rect hold_dest_rect = { 300, leftHoldY , hold_surface->w, hold_surface->h };
			hold_dest_rects.push_back(hold_dest_rect);
			leftHoldY -= 200;
		}
		else {
			SDL_Rect hold_dest_rect = { 430, rightHoldY , hold_surface->w, hold_surface->h };
			hold_dest_rects.push_back(hold_dest_rect);
			rightHoldY -= 200;
		}

		if (hold_path == "../src/redHold.png") {
			nextHoldPath = hold_path;
			hold[i] = 1;
		}
		else if (hold_path == "../src/greenHold.png") {
			nextHoldPath = hold_path;
			hold[i] = 2;
		}
		else if (hold_path == "../src/blueHold.png") {
			nextHoldPath = hold_path;
			hold[i] = 3;
		}
	}

	// leftUser
	leftUser_surface = IMG_Load("../src/leftUser.png");
	leftUser_texture = SDL_CreateTextureFromSurface(g_renderer, leftUser_surface);
	SDL_FreeSurface(leftUser_surface);
	leftUser_rect = { 0, 0, leftUser_surface->w, leftUser_surface->h };
	leftUser_dest_rect = { 280, 427, leftUser_surface->w, leftUser_surface->h };

	// rightUser
	rightUser_surface = IMG_Load("../src/rightUser.png");
	rightUser_texture = SDL_CreateTextureFromSurface(g_renderer, rightUser_surface);
	SDL_FreeSurface(rightUser_surface);
	rightUser_rect = { 0, 0, rightUser_surface->w, rightUser_surface->h };
	rightUser_dest_rect = { 410, 427, rightUser_surface->w, rightUser_surface->h };

	//ENDING
	//엔딩사진
	SDL_Surface* ending_surface = IMG_Load("../src/bg_mode2_ending.png");
	g_texture_ending = SDL_CreateTextureFromSurface(g_renderer, ending_surface);
	SDL_FreeSurface(ending_surface);
	SDL_QueryTexture(g_texture_ending, NULL, NULL, &g_source_rectangle_ending.w, &g_source_rectangle_ending.h);
	g_source_rectangle_ending = { 0, 0, 800, 600 };
	g_destination_rectangle_ending = { 0, 0, 800, 600 };

	//home 버튼
	SDL_Surface* home_surface = IMG_Load("../src/home_btn.png");
	g_texture_home = SDL_CreateTextureFromSurface(g_renderer, home_surface);
	SDL_FreeSurface(home_surface);
	SDL_QueryTexture(g_texture_home, NULL, NULL, &g_source_rectangle_home.w, &g_source_rectangle_home.h);
	g_source_rectangle_home = { 0, 0, 160, 80 };
	g_destination_rectangle_home = { 210, 435, 160, 80 };

	//retry
	SDL_Surface* retry_surface = IMG_Load("../src/retry.png");
	g_texture_retry = SDL_CreateTextureFromSurface(g_renderer, retry_surface);
	SDL_FreeSurface(retry_surface);
	SDL_QueryTexture(g_texture_retry, NULL, NULL, &g_source_rectangle_retry.w, &g_source_rectangle_retry.h);
	g_source_rectangle_retry = { 0, 0, 160, 80 };
	g_destination_rectangle_retry = { 440, 435, 160, 80 };
}

Mode2::~Mode2() {
	SDL_DestroyTexture(g_texture_intro);
	SDL_DestroyTexture(g_texture_play);

	SDL_DestroyTexture(bg_texture);
	SDL_DestroyTexture(wall_texture);

	for (SDL_Texture* texture : hold_textures) {
		SDL_DestroyTexture(texture);
	}
	SDL_DestroyTexture(leftUser_texture);
	SDL_DestroyTexture(rightUser_texture);
	SDL_DestroyTexture(introBack_texture);

	SDL_DestroyTexture(g_texture_ending);
	SDL_DestroyTexture(g_texture_home);
	SDL_DestroyTexture(g_texture_retry);
}

void Mode2::Update()
{
	Uint32 currentTime = SDL_GetTicks();

	//레디
	if (ready) {
		if (currentTime - startTime >= 2000) {
			startTime = currentTime;

			tutorial = false;
			ready = false;
			start = true;
			game_start = false;
			game_over = false;
			game_ending = false;
		}
	}
	//스타트
	else if (start) {
		if (currentTime - startTime >= 1000) {

			startTime = currentTime;

			tutorial = false;
			ready = false;
			start = false;
			game_start = true;
			game_over = false;
			game_ending = false;
		}
	}

	if (game_start && !game_over) {
		//맞는돌 눌렀을 때
		if (checkHold()) {
			userMove();
			holdMove();
			cur_i++;
			gauge += 5;
			backgroundY += 30;

			if (cur_i >= 50) { //홀드 50개 넘으면 스피드 업!
				gaugeDecreaseRate = gaugeDecreaseRate2;
			}
		}
		else {
			gauge -= gaugeDecreaseRate;
			if (gauge < 0) {
				gauge = 0;
			}
		}
		// 게이지가 0이면 게임 오버
		if (gauge == 0) {
			tutorial = false;
			ready = false;
			start = false;
			game_start = false;
			game_over = true;
			game_ending = false;
			ResetGame();
		}

		if (checkHold()) {
			//wall
			if (wall_dest_rect.y >= 600) {
				wall_dest_rect.y = 0;
			}
		}
	}

	if (game_over) {
		Uint32 elapsedTime = currentTime - gameoverTime;
		if (game_over && elapsedTime >= 2000) {
			tutorial = false;
			ready = false;
			start = false;
			game_start = false;
			game_over = false;
			game_ending = true;
		}
	}
	if (game_ending) {
		tutorial = false;
		ready = false;
		start = false;
		game_start = false;
		game_over = false;
	}
}

void Mode2::Render() {
	SDL_RenderClear(g_renderer);

	//배경이어지기
	SDL_Rect backgroundRect = { 0, backgroundY, BACKGROUND_WIDTH, BACKGROUND_HEIGHT };
	SDL_RenderCopy(g_renderer, bg_texture, NULL, &backgroundRect);
	
	//벽이어지기
	{
		int currentY = wall_dest_rect.y + wall_dest_rect.h;
		while (currentY > -wall_rect.h) {
			SDL_Rect tmp_r;
			tmp_r.x = wall_dest_rect.x;
			tmp_r.y = currentY - 600;
			tmp_r.w = wall_dest_rect.w;
			tmp_r.h = wall_dest_rect.h;
			SDL_RenderCopy(g_renderer, wall_texture, &wall_rect, &tmp_r);
			currentY -= wall_rect.h;
		}
	}
		// 게이지 그리기
		SDL_Rect gaugeRect = { 615, 20, gauge, 30 }; // 게이지 위치와 크기 설정
		SDL_SetRenderDrawColor(g_renderer, 255, 0, 0, 255); // 게이지 색상 (빨간색)
		SDL_RenderFillRect(g_renderer, &gaugeRect);

		// 게이지 테두리 그리기
		SDL_Rect gaugeBorderRect = { 615, 20, 150, 30 }; // 게이지 테두리 위치와 크기 설정
		SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255); // 게이지 테두리 색상 (흰색)
		int borderWidth = 4; // 테두리 두께 설정
		for (int i = 0; i < borderWidth; i++) {
			SDL_Rect borderRect = { gaugeBorderRect.x - i, gaugeBorderRect.y - i, gaugeBorderRect.w + 2 * i, gaugeBorderRect.h + 2 * i };
			SDL_RenderDrawRect(g_renderer, &borderRect);
		}

		if (tutorial) {
			//Tutorial
			SDL_RenderCopy(g_renderer, g_texture_intro, &g_source_rectangle_intro, &g_destination_rectangle_intro);
			// Play Button
			SDL_RenderCopy(g_renderer, g_texture_play, &g_source_rectangle_play, &g_destination_rectangle_play);
			// Back Button
			SDL_RenderCopy(g_renderer, introBack_texture, &introBack_rect, &introBack_dest_rect);	
		}

		else if (ready) {
			SDL_RenderCopy(g_renderer, ready_texture, &ready_rect, &ready_dest_rect);
		}

		else if (start) {
			SDL_RenderCopy(g_renderer, start_texture, &start_rect, &start_dest_rect);
		}

		else if (game_start) {
			SDL_Rect gaugeRect = { 615, 20, gauge, 30 }; // 게이지 위치와 크기 설정
			SDL_SetRenderDrawColor(g_renderer, 255, 0, 0, 255); // 게이지 색상 (빨간색)
			SDL_RenderFillRect(g_renderer, &gaugeRect);

			// 게이지 테두리 그리기
			SDL_Rect gaugeBorderRect = { 615, 20, 150, 30 }; // 게이지 테두리 위치와 크기 설정
			SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255); // 게이지 테두리 색상 (흰색)
			int borderWidth = 4; // 테두리 두께 설정
			for (int i = 0; i < borderWidth; i++) {
				SDL_Rect borderRect = { gaugeBorderRect.x - i, gaugeBorderRect.y - i, gaugeBorderRect.w + 2 * i, gaugeBorderRect.h + 2 * i };
				SDL_RenderDrawRect(g_renderer, &borderRect);
			}

			//홀드그리기
			for (size_t i = 0; i < hold_textures.size(); i++) {
				SDL_RenderCopy(g_renderer, hold_textures[i], &hold_rects[i], &hold_dest_rects[i]);
			}
			SDL_RenderCopy(g_renderer, leftUser_texture, &leftUser_rect, &leftUser_dest_rect);

		}
		else if (game_ending) {
			SDL_RenderCopy(g_renderer, g_texture_ending, &g_source_rectangle_ending, &g_destination_rectangle_ending);
			SDL_RenderCopy(g_renderer, g_texture_home, &g_source_rectangle_home, &g_destination_rectangle_home);
			SDL_RenderCopy(g_renderer, g_texture_retry, &g_source_rectangle_retry, &g_destination_rectangle_retry);

		}
		SDL_RenderPresent(g_renderer);
	}

void Mode2::userMove() {
	//배경 아래로 움직이기
	wall_dest_rect.y += 30;
	bg_dest_rect.y += 5;

	//왼쪽유저일때 오른쪽유저로 바꾸기
	if (isLeftUser) {
		leftUser_dest_rect.x += 150;
		rightUser_dest_rect.x -= 150;
	}
	//오른쪽유저일때 왼쪽유저로 바꾸기
	else {
		leftUser_dest_rect.x -= 150;
		rightUser_dest_rect.x += 150;
	}

	SDL_Surface* tempSurface = leftUser_surface;
	leftUser_surface = rightUser_surface;
	rightUser_surface = rightUser_surface;

	SDL_Texture* tempTexture = leftUser_texture;
	leftUser_texture = rightUser_texture;
	rightUser_texture = tempTexture;

	isLeftUser = !isLeftUser;
}

void Mode2::holdMove() {
	for (SDL_Rect& hold_dest_rect : hold_dest_rects) {
		hold_dest_rect.y += 100;
	}
}

bool Mode2::checkHold() {
	if (hold[cur_i] == f_state) {
		prevHold = f_state;
		return true;
	}
	else {
		prevHold = 0; // hold가 일치하지 않으면 prevHold 값을 초기화합니다.
	}
	return false;
}

void Mode2::HandleEvents() {
	SDL_Event event;

	if (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			g_flag_running = false;
			break;

		case SDL_MOUSEBUTTONDOWN:

			// If the mouse left button is pressed. 
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				int mouseX = event.button.x;
				int mouseY = event.button.y;
				//뒤로가기(home으로) 나중에 합치면!!!!!
				if (mouseX >= introBack_rect.x && mouseX < introBack_dest_rect.x + introBack_dest_rect.w
					&& mouseY >= introBack_dest_rect.y && mouseY < introBack_dest_rect.y + introBack_dest_rect.h
					&& g_current_game_phase == PHASE_MODE2) {
					// g_current_game_phase = PHASE_HOME;
					tutorial = true;
					ready = false;
					start = false;
					game_start = false;
					game_over = false;
					game_ending = false;
				}

				//game start
				if (mouseX >= start_dest_rect.x && mouseX < start_dest_rect.x + start_dest_rect.w
					&& mouseY >= start_dest_rect.y && mouseY < start_dest_rect.y + start_dest_rect.h
					&& g_current_game_phase == PHASE_MODE2 && tutorial == true) {
					tutorial = false;
					ready = true;
					start = false;
					game_start = false;
					game_over = false;
					game_ending = false;
					startTime = SDL_GetTicks();
				}
				// retry
				if (
					mouseX >= g_destination_rectangle_retry.x &&
					mouseX <= g_destination_rectangle_retry.x + g_destination_rectangle_retry.w &&
					mouseY >= g_destination_rectangle_retry.y &&
					mouseY <= g_destination_rectangle_retry.y + g_destination_rectangle_retry.h) {
					tutorial = true;
					ready = false;
					start = false;
					game_start = false;
					game_over = false;
					game_ending = false;
					// g_current_game_phase = PHASE_MODE2;
				}

				// home 버튼
				if (mouseX >= g_destination_rectangle_home.x &&
					mouseX <= g_destination_rectangle_home.x + g_destination_rectangle_home.w &&
					mouseY >= g_destination_rectangle_home.y &&
					mouseY <= g_destination_rectangle_home.y + g_destination_rectangle_home.h) {
					// g_current_game_phase = HOME;
					tutorial = true;
					ready = false;
					start = false;
					game_start = false;
					game_over = false;
					game_ending = false;
				}
			}
				break;

		case SDL_KEYDOWN:
			if (!ready && !start && event.key.keysym.sym)
			{
				if (event.key.keysym.sym == SDLK_r) {
					f_state = 1;
					gauge += 10;
					if (gauge > 100) {
						gauge = 100;
					}
				}
				else if (event.key.keysym.sym == SDLK_g) {
					f_state = 2;
					gauge += 10; // 버튼을 누를 때 게이지를 증가시킴
					if (gauge > 100) {
						gauge = 100;
					}
				}
				else if (event.key.keysym.sym == SDLK_b) {
					f_state = 3;
					gauge += 10; // 버튼을 누를 때 게이지를 증가시킴
					if (gauge > 100) {
						gauge = 100;
					}
				}
				//틀렸을때 게임오버
				if (f_state != hold[cur_i]) {
					SDL_Delay(2000); //맞는 효과음 넣으면 ㄱㅊ을듯!!
					tutorial = false;
					ready = false;
					start = false;
					game_start = false;
					game_over = true;
					game_ending = true;
					ResetGame();
				}
				break;

		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_DOWN) {
				// 아래 키를 떼면 f_state 값을 초기화합니다.
				f_state = 0;
			}
			}
			break;
		default:
			break;
			}
		}
	}

	void Mode2::ResetGame() {
		f_state = 0;
		cur_i = 1;
		prevHold = 0;
		gauge = 150;

		backgroundY = -2400;

		userMove();
		//isLeftUser = !isLeftUser;
		leftUser_dest_rect.x = 280;
		rightUser_dest_rect.x = 410;
		//leftUser_dest_rect = { 280, 427, leftUser_surface->w, leftUser_surface->h };
		//rightUser_dest_rect = { 410, 427, rightUser_surface->w, rightUser_surface->h };

		// Reset hold positions
		leftHoldY = 475;
		rightHoldY = 375;
		for (size_t i = 0; i < hold_dest_rects.size(); i++) {
			if (i % 2 == 0) {
				hold_dest_rects[i].y = leftHoldY;
				leftHoldY -= 200;
			}
			else {
				hold_dest_rects[i].y = rightHoldY;
				rightHoldY -= 200;
			}
		}

	}