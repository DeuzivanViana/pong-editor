#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>

extern "C"
{
	#include <raylib.h>
	#include <raymath.h>
}

constexpr std::uint16_t
	WINDOW_WIDTH {512},
	WINDOW_HEIGHT {620},
	BALL_DEFAULT_VELOCITY {4};

const char
	*WINDOW_CAPTION {"Pong Engine"};

struct Ball
{
	Vector2 velocity, position;
};

struct Block
{
	bool can_destroy, can_drop_buff;
	Vector2 position;
	Color color;
};

struct Buff
{
	int id;
	Vector2 position;
	Texture2D *texture;
};

bool isBlockHere(std::vector<Block> bs, Vector2 p) noexcept
{
	for(int i = 0; i < bs.size(); i++)
	{
		if(bs[i].position.x == p.x && bs[i].position.y == p.y)
			return true;
	}
}

int main(int, char *[])
{
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_CAPTION);
	SetTargetFPS(60);
	
	Texture2D 
		player_texture { LoadTexture("assets/player.png") },
		ball_texture { LoadTexture("assets/ball.png") },
		buff_double_texture { LoadTexture("assets/buff-double.png") },
		buff_speed_texture { LoadTexture("assets/buff-speed.png") },
		block_texture { LoadTexture("assets/block.png") };

	Vector2 player_position { WINDOW_WIDTH / 2 - 98 / 2, WINDOW_HEIGHT - 16 - 16 };

	std::vector<Ball> balls;
	std::vector<Block> blocks;
	std::vector<Buff> buff;

	balls.push_back({{0, BALL_DEFAULT_VELOCITY}, {WINDOW_WIDTH / 2 - 4, WINDOW_HEIGHT - 112}});

	bool can_start = false, lazy = false;
	int block_id = 0;

	while(!WindowShouldClose())
	{
		Vector2 mouse_position = GetMousePosition();

		if(can_start)
			player_position.x = Clamp(mouse_position.x - 98 / 2, 0, WINDOW_WIDTH - 98);

		if(IsKeyPressed(KEY_SPACE))
			can_start = !can_start;

		if(IsKeyPressed(KEY_Z))
			balls.push_back({{0, BALL_DEFAULT_VELOCITY}, {WINDOW_WIDTH / 2 - 4, WINDOW_HEIGHT - 112}});
		BeginDrawing();
		ClearBackground(GRAY);

		if(!can_start)
		{
			DrawText("To start, press SPACE!", 16, 16, 20, GREEN);

			if(IsKeyPressed(KEY_ONE))
				block_id = 0;
			if(IsKeyPressed(KEY_TWO))
				block_id = 1;

			if(IsKeyPressed(KEY_A))
				lazy = true;
			if(IsKeyPressed(KEY_S))
				lazy = false;
			
			if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
			{
				Vector2 block_position {
					static_cast<float>(std::ceil(static_cast<int>(mouse_position.x - 2) / 8 * 8)),
					static_cast<float>(std::ceil(static_cast<int>(mouse_position.y - 2) / 8 * 8))
				};

				for(int i = 0; i < blocks.size(); i++)
					if(block_position.x == blocks[i].position.x && block_position.y == blocks[i].position.y)
						blocks.erase(blocks.begin() + i);
			}
			if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
			{
				Vector2 block_position {
					static_cast<float>(std::ceil(static_cast<int>(mouse_position.x - 2) / 8 * 8)),
					static_cast<float>(std::ceil(static_cast<int>(mouse_position.y - 2) / 8 * 8))
				};
				
				Block block;
				if(block_id == 0)
					block = {
						false, false,
						block_position, WHITE
					};
				else if(block_id == 1)
					block = {
						true, true,
						block_position, GREEN
					};

				bool already_exists = false;

				if(!already_exists && block_id == 1 && lazy)
				{
					Block b = block;
					if(!isBlockHere(blocks, b.position))
						blocks.push_back(block);
					
					b.position.x -= 8;
					if(!isBlockHere(blocks, b.position))
						blocks.push_back(b);

					b = block;
					b.position.x += 8;
					if(!isBlockHere(blocks, b.position))
						blocks.push_back(b);
					
					b = block;
					b.position.y += 8;
					if(!isBlockHere(blocks, b.position))
						blocks.push_back(b);
					
					b = block;
					b.position.y -= 8;
					if(!isBlockHere(blocks, b.position))
						blocks.push_back(b);
					
					b = block;
					b.position.y -= 8;
					b.position.x -= 8;
					if(!isBlockHere(blocks, b.position))
						blocks.push_back(b);

					b = block;
					b.position.y += 8;
					b.position.x -= 8;
					if(!isBlockHere(blocks, b.position))
						blocks.push_back(b);

					b = block;
					b.position.y += 8;
					b.position.x += 8;
					if(!isBlockHere(blocks, b.position))
						blocks.push_back(b);

					b = block;
					b.position.y -= 8;
					b.position.x += 8;
					if(!isBlockHere(blocks, b.position))
						blocks.push_back(b);
				}
				else if(!isBlockHere(blocks, block.position))
					blocks.push_back(block);
			}
		}

		for(int i = 0; i < balls.size(); i++)
		{
			if(can_start)
			{
        balls[i].position.x += balls[i].velocity.x;

        if (balls[i].position.y <= 0) {
            balls[i].velocity.y = BALL_DEFAULT_VELOCITY;
        }
        if (balls[i].position.x <= 0 || balls[i].position.x >= WINDOW_WIDTH - 8) {
            balls[i].velocity.x *= -1;
        }

        if (CheckCollisionRecs({player_position.x, player_position.y, 98, 16}, 
					{balls[i].position.x, balls[i].position.y, 8, 8}))
				{
					float player_center_x = player_position.x + 98 / 2;
					float ball_center_x = balls[i].position.x + 4;

					float offset = (ball_center_x - player_center_x) / (98 / 2);

					balls[i].velocity.x += offset * BALL_DEFAULT_VELOCITY * 0.5f;

					if (std::abs(balls[i].velocity.x) < 0.2f) {
							balls[i].velocity.x = (balls[i].velocity.x > 0 ? 0.2f : -0.2f);
					}
					balls[i].velocity.y *= -1;
				}

        for (int b = 0; b < blocks.size(); b++)
				{
					if (CheckCollisionRecs({blocks[b].position.x, blocks[b].position.y, 8, 8},
																 {balls[i].position.x, balls[i].position.y, 8, 8}))
					{
						Ball &ball = balls[i];
						Block &blk = blocks[b];


						// Detectando o lado da colisão e invertendo a velocidade da bola
						float ballLeft = ball.position.x;
						float ballRight = ball.position.x + 16;
						float ballTop = ball.position.y;
						float ballBottom = ball.position.y + 16;

						float blockLeft = blk.position.x;
						float blockRight = blk.position.x + 16;
						float blockTop = blk.position.y;
						float blockBottom = blk.position.y + 16;

						// Verificando de qual lado ocorreu a colisão
						if (ballRight > blockLeft && ballLeft < blockRight &&
								ballBottom > blockTop && ballTop < blockBottom)
						{
								// Colisão com a esquerda ou direita
								if (ballRight > blockLeft && ballLeft < blockLeft) // Colisão pela esquerda
								{
										ball.velocity.x = -std::abs(ball.velocity.x); // Inverte a velocidade horizontal
								}
								else if (ballLeft < blockRight && ballRight > blockRight) // Colisão pela direita
								{
										ball.velocity.x = std::abs(ball.velocity.x); // Inverte a velocidade horizontal
								}

						}
							if((std::rand() % 20 == 1) && blocks[b].can_drop_buff)
						{
							buff.push_back({0, {blocks[b].position.x, blocks[b].position.y}, &buff_double_texture});
						}

						if(blocks[b].can_destroy)
							blocks.erase(blocks.begin() + b);
						break;	

					}
				}

        balls[i].position.y += balls[i].velocity.y;
			


        for (int b = 0; b < blocks.size(); b++)
				{
					if (CheckCollisionRecs({blocks[b].position.x, blocks[b].position.y, 8, 8},
																 {balls[i].position.x, balls[i].position.y, 8, 8}))
					{
						Ball &ball = balls[i];
						Block &blk = blocks[b];


						// Detectando o lado da colisão e invertendo a velocidade da bola
						float ballLeft = ball.position.x;
						float ballRight = ball.position.x + 16;
						float ballTop = ball.position.y;
						float ballBottom = ball.position.y + 16;

						float blockLeft = blk.position.x;
						float blockRight = blk.position.x + 16;
						float blockTop = blk.position.y;
						float blockBottom = blk.position.y + 16;

						// Verificando de qual lado ocorreu a colisão
						if (ballRight > blockLeft && ballLeft < blockRight &&
								ballBottom > blockTop && ballTop < blockBottom)
						{

								// Colisão com o topo ou fundo
								if (ballBottom > blockTop && ballTop < blockTop) // Colisão pelo topo
								{
										ball.velocity.y = -std::abs(ball.velocity.y); // Inverte a velocidade vertical
								}
								else if (ballTop < blockBottom && ballBottom > blockBottom) // Colisão pelo fundo
								{
										ball.velocity.y = std::abs(ball.velocity.y); // Inverte a velocidade vertical
								}
						}
							if((std::rand() % 20 == 1) && blocks[b].can_drop_buff)
						{
							buff.push_back({0, {blocks[b].position.x, blocks[b].position.y}, &buff_double_texture});
						}

						if(blocks[b].can_destroy)
							blocks.erase(blocks.begin() + b);
						break;	

					}
				}



				if(balls[i].position.y >= WINDOW_HEIGHT + 20)
					balls.erase(balls.begin() + i);
			}

			DrawTexture(ball_texture, balls[i].position.x, balls[i].position.y, WHITE);
		}

		for(int i = 0; i < blocks.size(); i++)
		{
			DrawTexture(block_texture, blocks[i].position.x, blocks[i].position.y, blocks[i].color);
		}
		for(int i = 0; i < buff.size(); i++)
		{
			buff[i].position.y += 2;

			DrawTexture(*buff[i].texture, buff[i].position.x, buff[i].position.y, WHITE);

			if (CheckCollisionRecs({player_position.x, player_position.y, 98, 16}, 
				{buff[i].position.x, buff[i].position.y, 16, 16}))
			{
				if(buff[i].id == 0)
				{
					int my_balls = balls.size();
					if(balls.size() < 1028)
						for(int b = 0; b < Clamp(my_balls, 0, 64); b++)
						{
							balls.push_back({
									{
										balls[b].velocity.x * (std::rand() % 2 == 1) ? -1 : 1,
										balls[b].velocity.y * (std::rand() % 2 == 1) ? -1 : 1,
									},
									balls[b].position
									});
						}
				}
				buff.erase(buff.begin() + i);
			}
		}

		DrawTexture(player_texture, player_position.x, player_position.y, RED);

		EndDrawing();
	}

	UnloadTexture(player_texture);
	UnloadTexture(ball_texture);
	UnloadTexture(block_texture);
	UnloadTexture(buff_double_texture);
	UnloadTexture(buff_speed_texture);

	CloseWindow();

	return EXIT_SUCCESS;
}
