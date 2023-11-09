#ifndef __CONFIG_H__
#define __CONFIG_H__

/*
 * Color pair numbers
 */
enum {
	WIN_BOX = 1,
	MSG_BOX = 2,
	ERR_BOX = 3,
	BAK_BOX = 4,
};

/*
 * Ascii key codes
 */
enum {
  ESC = 27,
  RET = 10,
  SPC = 32
};

/*
 * Holds dimensions for the screen & grid
 */
enum {
  WIN_Y = 24,
  WIN_X = 80,
  GRD_Y = 16,
  GRD_X = 20,
};

/*
 * Config options for the gameplay
 */
enum {
  BOMBS = 99,
  FLAGS = BOMBS
};

#endif /* __CONFIG_H__ */
