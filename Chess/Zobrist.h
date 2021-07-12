#pragma once
#include <stdint.h>
#include <random>

struct Zobrist
{
	uint64_t seed = 0x01AC4bA5D1B528E0;
	uint64_t pieces[2][6][64]; // colour, piece, sqr
	uint64_t turn; //black to go
	uint64_t castling[2][2]; // colour, queenside/kingside rights
	uint64_t enPassant[8]; // enPassant file
public: Zobrist() {
		std::random_device rd;
		std::default_random_engine generator(rd());
		generator.seed(seed);
		std::uniform_int_distribution<uint64_t> distribution(0, 0xFFFFFFFFFFFFFFFF);
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 6; j++) {
				for (int k = 0; k < 64; k++) {
					pieces[i][j][k] = distribution(generator);
				}
			}
		}
		turn = distribution(generator);
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				castling[i][j] = distribution(generator);
			}
		}
		for (int i = 0; i < 8; i++) {
			enPassant[i] = distribution(generator);
		}
	}
};

